// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_ASYNC_STATE_HEADER
#define FTL_DETAIL_ASYNC_STATE_HEADER

#ifdef FTL_REPLACE_STL
#include <atomic>
#include <cstdint>
#include <detail/future_state.hpp>
#include <detail/mutex.hpp>
#include <detail/wait_notify.hpp>
#include <exception>
#include <functional>
#include <memory>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#define FTL_ASYNC_STATE_BEGIN_NAMESPACE namespace std::detail {
#define FTL_ASYNC_STATE_END_NAMESPACE }
#else
#include <ftl/atomic>
#include <ftl/cstdint>
#include <ftl/detail/future_state.hpp>
#include <ftl/detail/mutex.hpp>
#include <ftl/detail/wait_notify.hpp>
#include <ftl/exception>
#include <ftl/functional>
#include <ftl/memory>
#include <ftl/thread>
#include <ftl/tuple>
#include <ftl/type_traits>
#include <ftl/utility>
#define FTL_ASYNC_STATE_BEGIN_NAMESPACE namespace ftl::detail {
#define FTL_ASYNC_STATE_END_NAMESPACE }
#endif

FTL_ASYNC_STATE_BEGIN_NAMESPACE

template <class R, class Function, class... StoredArgs>
class async_state final : public future_state<R> {
public:
  template <class F, class... Args>
  explicit async_state(F &&function, Args &&...arguments)
      : invocation_(forward<F>(function), forward<Args>(arguments)...) {}

  ~async_state() override {
    if (!deferred_)
      join_worker();
  }

  async_state(const async_state &) = delete;

  async_state &operator=(const async_state &) = delete;

  void select_deferred() noexcept { deferred_ = true; }

  void start_async(const shared_ptr<async_state> &self) {
    deferred_ = false;

    // The worker owns a physical reference to
    // its state so destroying the final future
    // from inside the worker cannot invalidate
    // the state while result publication is
    // still in progress.
    shared_ptr<async_state> keep_alive = self;

    worker_ = thread{
        [state = move(keep_alive)]() mutable noexcept { state->run_async(); }};
  }

protected:
  bool is_deferred() const noexcept override {
    return deferred_ && !deferred_started_.load(memory_order_acquire);
  }

  void execute_deferred() noexcept override {
    bool expected = false;

    if (!deferred_started_.compare_exchange_strong(
            expected, true, memory_order_acq_rel, memory_order_acquire)) {
      return;
    }

    invoke_stored();
  }

  void wait_completion() noexcept override {
    if (!deferred_)
      join_worker();
  }

  bool wait_completion_until(uint64_t deadline) noexcept override {
    if (deferred_)
      return true;

    if (!wait_execution_done_until(deadline)) {
      return false;
    }

    // execution_done_ is deliberately set at
    // the very end of our worker callable.
    // Joining here closes the final tiny gap
    // between that store and actual native
    // thread completion.
    join_worker();

    return true;
  }

  void on_last_return_reference() noexcept override {
    if (!deferred_)
      join_worker();
  }

private:
  void run_async() noexcept {
    invoke_stored();

    mark_execution_done();
  }

  void invoke_stored() noexcept {
    apply(
        [this](auto &&function, auto &&...arguments) {
          invoke_and_store(move(function), move(arguments)...);
        },
        move(invocation_));
  }

  template <class F, class... Args>
  void invoke_and_store(F &&function, Args &&...arguments) noexcept {
    bool stored = false;

#if FTL_HAS_EXCEPTIONS
    try {
#endif

      if constexpr (is_void_v<R>) {
        invoke(forward<F>(function), forward<Args>(arguments)...);

        stored = this->try_set_value();
      } else if constexpr (is_lvalue_reference_v<R>) {
        R result = invoke(forward<F>(function), forward<Args>(arguments)...);

        stored = this->try_set_value(result);
      } else {
        stored = this->try_set_value(
            invoke(forward<F>(function), forward<Args>(arguments)...));
      }

#if FTL_HAS_EXCEPTIONS
    } catch (...) {
      stored = this->try_set_exception(current_exception());
    }
#endif

    // There is exactly one provider for an
    // async state. Failure here means an
    // internal state-machine invariant was
    // violated.
    if (!stored)
      terminate();
  }

  void mark_execution_done() noexcept {
    atomic_ref<uint32_t> done{execution_done_};

    done.store(uint32_t{1}, memory_order_release);

    done.notify_all();
  }

  [[nodiscard]]
  bool wait_execution_done_until(uint64_t deadline) noexcept {
    atomic_ref<uint32_t> done{execution_done_};

    if (done.load(memory_order_acquire) != 0) {
      return true;
    }

    if (!atomic_wait_until_changed_until<sizeof(uint32_t)>(
            &execution_done_, uint32_t{0}, deadline,
            [&] { return done.load(memory_order_acquire); })) {
      return done.load(memory_order_acquire) != 0;
    }

    return done.load(memory_order_acquire) != 0;
  }

  void join_worker() noexcept {
    join_gate_.lock();

    if (!worker_.joinable()) {
      join_gate_.unlock();
      return;
    }

#if FTL_HAS_EXCEPTIONS
    try {
#endif

      // A worker can indirectly destroy its
      // own last future/shared_future. It
      // obviously cannot join itself; its
      // physical state reference keeps this
      // object alive until run_async returns.
      if (worker_.get_id() == this_thread::get_id()) {
        worker_.detach();
      } else {
        worker_.join();
      }

#if FTL_HAS_EXCEPTIONS
    } catch (...) {
      join_gate_.unlock();

      // A join/detach failure here represents
      // failure of an internal invariant, not
      // an error permitted by future::wait.
      terminate();
    }
#endif

    join_gate_.unlock();
  }

  tuple<Function, StoredArgs...> invocation_;

  bool deferred_ = false;

  atomic<bool> deferred_started_{false};

  detail::mutex_state join_gate_;

  thread worker_;

  alignas(atomic_ref<uint32_t>::required_alignment) uint32_t execution_done_ =
      0;
};

FTL_ASYNC_STATE_END_NAMESPACE

#undef FTL_ASYNC_STATE_BEGIN_NAMESPACE
#undef FTL_ASYNC_STATE_END_NAMESPACE

#endif // FTL_DETAIL_ASYNC_STATE_HEADER
