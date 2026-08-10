// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_FUTURE_STATE_HEADER
#define FTL_DETAIL_FUTURE_STATE_HEADER

#ifdef FTL_REPLACE_STL
#include <atomic>
#include <cstdint>
#include <detail/mutex.hpp>
#include <detail/wait_notify.hpp>
#include <exception>
#include <memory>
#include <new>
#include <utility>
#define FTL_FUTURE_STATE_NAMESPACE std
#define FTL_FUTURE_STATE_BEGIN_NAMESPACE namespace std::detail {
#define FTL_FUTURE_STATE_END_NAMESPACE }
#else
#include <ftl/atomic>
#include <ftl/cstdint>
#include <ftl/detail/mutex.hpp>
#include <ftl/detail/wait_notify.hpp>
#include <ftl/exception>
#include <ftl/memory>
#include <ftl/new>
#include <ftl/utility>
#define FTL_FUTURE_STATE_NAMESPACE ftl
#define FTL_FUTURE_STATE_BEGIN_NAMESPACE namespace ftl::detail {
#define FTL_FUTURE_STATE_END_NAMESPACE }
#endif

FTL_FUTURE_STATE_BEGIN_NAMESPACE

enum class future_wait_result {
  ready,
  timeout,
  deferred,
};

enum class future_state_status : uint32_t {
  empty,
  stored_value,
  stored_exception,
  ready_value,
  ready_exception,
};

class future_state_base {
public:
  future_state_base() = default;

  future_state_base(const future_state_base &) = delete;

  future_state_base &operator=(const future_state_base &) = delete;

  virtual ~future_state_base() = default;

  [[nodiscard]]
  bool is_ready() const noexcept {
    return is_ready_status(load_status(memory_order_acquire));
  }

  [[nodiscard]]
  bool is_satisfied() const noexcept {
    return load_status(memory_order_acquire) != future_state_status::empty;
  }

  void add_return_reference() noexcept {
    return_references_.fetch_add(1, memory_order_relaxed);
  }

  void release_return_reference() noexcept {
    if (return_references_.fetch_sub(1, memory_order_acq_rel) == 1) {
      on_last_return_reference();
    }
  }

  [[nodiscard]]
  bool try_mark_future_retrieved() noexcept {
    return !future_retrieved_.exchange(true, memory_order_acq_rel);
  }

  void wait() {
    if (is_deferred())
      execute_deferred();

    wait_ready();

    // async-created states override this so
    // successful non-timed waits do not return
    // before their associated thread completes.
    wait_completion();
  }

  [[nodiscard]]
  future_wait_result wait_for(uint64_t timeout_nanoseconds) {
    if (is_deferred())
      return future_wait_result::deferred;

    const uint64_t deadline = wait_deadline_after(timeout_nanoseconds);

    if (!is_ready()) {
      if (!wait_ready_until(deadline))
        return future_wait_result::timeout;
    }

    if (!wait_completion_until(deadline))
      return future_wait_result::timeout;

    return future_wait_result::ready;
  }

  void make_ready() noexcept {
    bool notify = false;

    gate_.lock();

    atomic_ref<uint32_t> status{status_};

    const future_state_status current =
        decode_status(status.load(memory_order_relaxed));

    future_state_status replacement = current;

    if (current == future_state_status::stored_value) {
      replacement = future_state_status::ready_value;
    } else if (current == future_state_status::stored_exception) {
      replacement = future_state_status::ready_exception;
    }

    if (replacement != current) {
      status.store(encode_status(replacement), memory_order_release);

      notify = true;
    }

    gate_.unlock();

    if (notify)
      status.notify_all();
  }

  [[nodiscard]]
  bool try_set_exception(exception_ptr exception) {
    return try_store_exception(move(exception), true);
  }

  [[nodiscard]]
  bool try_store_exception(exception_ptr exception) {
    return try_store_exception(move(exception), false);
  }

  void abandon_with_exception(exception_ptr exception) noexcept {
    bool notify = false;

    gate_.lock();

    atomic_ref<uint32_t> status{status_};

    const future_state_status current =
        decode_status(status.load(memory_order_relaxed));

    if (current == future_state_status::empty) {
      exception_ = move(exception);

      status.store(encode_status(future_state_status::ready_exception),
                   memory_order_release);

      notify = true;
    }

    gate_.unlock();

    if (notify)
      status.notify_all();
  }

  void rethrow_if_exception() {
    wait();

    if (load_status(memory_order_acquire) !=
        future_state_status::ready_exception) {
      return;
    }

#if FTL_HAS_EXCEPTIONS
    FTL_FUTURE_STATE_NAMESPACE::rethrow_exception(exception_);
#else
    terminate();
#endif
  }

protected:
  template <class Writer>
  [[nodiscard]]
  bool try_store_result(future_state_status stored_status,
                        future_state_status ready_status,
                        bool ready_immediately, Writer &&writer) {
    gate_.lock();

    atomic_ref<uint32_t> status{status_};

    if (decode_status(status.load(memory_order_relaxed)) !=
        future_state_status::empty) {
      gate_.unlock();
      return false;
    }

#if FTL_HAS_EXCEPTIONS
    try {
#endif

      forward<Writer>(writer)();

#if FTL_HAS_EXCEPTIONS
    } catch (...) {
      gate_.unlock();
      throw;
    }
#endif

    status.store(
        encode_status(ready_immediately ? ready_status : stored_status),
        memory_order_release);

    gate_.unlock();

    if (ready_immediately)
      status.notify_all();

    return true;
  }

  [[nodiscard]]
  future_state_status status() const noexcept {
    return load_status(memory_order_acquire);
  }

  [[nodiscard]]
  static bool contains_value(future_state_status status) noexcept {
    return status == future_state_status::stored_value ||
           status == future_state_status::ready_value;
  }

  virtual bool is_deferred() const noexcept { return false; }

  virtual void execute_deferred() noexcept {}

  // Async states use these hooks to couple
  // readiness to actual associated-thread
  // completion.
  virtual void wait_completion() noexcept {}

  [[nodiscard]]
  virtual bool wait_completion_until(uint64_t) noexcept {
    return true;
  }

  // Physical shared_ptr ownership includes
  // implementation references such as the
  // running async worker. This hook instead
  // observes the last future/shared_future
  // reference disappearing.
  virtual void on_last_return_reference() noexcept {}

private:
  [[nodiscard]]
  bool try_store_exception(exception_ptr exception, bool ready_immediately) {
    return try_store_result(future_state_status::stored_exception,
                            future_state_status::ready_exception,
                            ready_immediately,
                            [&] { exception_ = move(exception); });
  }

  void wait_ready() const noexcept {
    atomic_ref<uint32_t> status{status_};

    for (;;) {
      const uint32_t current = status.load(memory_order_acquire);

      if (is_ready_status(decode_status(current))) {
        return;
      }

      status.wait(current, memory_order_acquire);
    }
  }

  [[nodiscard]]
  bool wait_ready_until(uint64_t deadline) const noexcept {
    atomic_ref<uint32_t> status{status_};

    for (;;) {
      const uint32_t current = status.load(memory_order_acquire);

      if (is_ready_status(decode_status(current))) {
        return true;
      }

      if (!atomic_wait_until_changed_until<sizeof(uint32_t)>(
              &status_, current, deadline,
              [&] { return status.load(memory_order_acquire); })) {
        return is_ready_status(
            decode_status(status.load(memory_order_acquire)));
      }
    }
  }

  [[nodiscard]]
  future_state_status load_status(memory_order order) const noexcept {
    atomic_ref<uint32_t> status{status_};

    return decode_status(status.load(order));
  }

  [[nodiscard]]
  static constexpr uint32_t encode_status(future_state_status status) noexcept {
    return static_cast<uint32_t>(status);
  }

  [[nodiscard]]
  static constexpr future_state_status decode_status(uint32_t status) noexcept {
    return static_cast<future_state_status>(status);
  }

  [[nodiscard]]
  static constexpr bool is_ready_status(future_state_status status) noexcept {
    return status == future_state_status::ready_value ||
           status == future_state_status::ready_exception;
  }

  mutable detail::mutex_state gate_;

  alignas(atomic_ref<uint32_t>::required_alignment) mutable uint32_t status_ =
      encode_status(future_state_status::empty);

  atomic<bool> future_retrieved_{false};

  atomic<unsigned int> return_references_{0};

  exception_ptr exception_{};
};

template <class T> class future_state : public future_state_base {
public:
  future_state() = default;

  ~future_state() override {
    if (contains_value(status()))
      destroy_at(value_pointer());
  }

  template <class... Args>
  [[nodiscard]]
  bool try_emplace_value(Args &&...args) {
    return try_emplace(true, forward<Args>(args)...);
  }

  template <class... Args>
  [[nodiscard]]
  bool try_emplace_stored_value(Args &&...args) {
    return try_emplace(false, forward<Args>(args)...);
  }

  template <class U>
  [[nodiscard]]
  bool try_set_value(U &&value) {
    return try_emplace_value(forward<U>(value));
  }

  template <class U>
  [[nodiscard]]
  bool try_store_value(U &&value) {
    return try_emplace_stored_value(forward<U>(value));
  }

  [[nodiscard]]
  T &value() noexcept {
    return *value_pointer();
  }

  [[nodiscard]]
  const T &value() const noexcept {
    return *value_pointer();
  }

private:
  template <class... Args>
  [[nodiscard]]
  bool try_emplace(bool ready_immediately, Args &&...args) {
    return try_store_result(
        future_state_status::stored_value, future_state_status::ready_value,
        ready_immediately,
        [&] { construct_at(value_pointer(), forward<Args>(args)...); });
  }

  [[nodiscard]]
  T *value_pointer() noexcept {
    return launder(reinterpret_cast<T *>(storage_));
  }

  [[nodiscard]]
  const T *value_pointer() const noexcept {
    return launder(reinterpret_cast<const T *>(storage_));
  }

  alignas(T) unsigned char storage_[sizeof(T)];
};

template <class T> class future_state<T &> : public future_state_base {
public:
  future_state() = default;

  [[nodiscard]]
  bool try_set_value(T &value) noexcept {
    return try_store(value, true);
  }

  [[nodiscard]]
  bool try_store_value(T &value) noexcept {
    return try_store(value, false);
  }

  [[nodiscard]]
  T &value() const noexcept {
    return *value_;
  }

private:
  [[nodiscard]]
  bool try_store(T &value, bool ready_immediately) noexcept {
    return try_store_result(future_state_status::stored_value,
                            future_state_status::ready_value, ready_immediately,
                            [&] { value_ = addressof(value); });
  }

  T *value_ = nullptr;
};

template <> class future_state<void> : public future_state_base {
public:
  future_state() = default;

  [[nodiscard]]
  bool try_set_value() noexcept {
    return try_store(true);
  }

  [[nodiscard]]
  bool try_store_value() noexcept {
    return try_store(false);
  }

private:
  [[nodiscard]]
  bool try_store(bool ready_immediately) noexcept {
    return try_store_result(future_state_status::stored_value,
                            future_state_status::ready_value, ready_immediately,
                            [] {});
  }
};

FTL_FUTURE_STATE_END_NAMESPACE

#undef FTL_FUTURE_STATE_BEGIN_NAMESPACE
#undef FTL_FUTURE_STATE_END_NAMESPACE
#undef FTL_FUTURE_STATE_NAMESPACE

#endif // FTL_DETAIL_FUTURE_STATE_HEADER
