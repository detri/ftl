// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_MUTEX_HEADER
#define FTL_DETAIL_MUTEX_HEADER

#ifdef FTL_REPLACE_STL
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <detail/thread.hpp>
#include <detail/wait_notify.hpp>
#include <limits>
#define FTL_MUTEX_DETAIL_BEGIN_NAMESPACE namespace std::detail {
#define FTL_MUTEX_DETAIL_END_NAMESPACE }
#else
#include <ftl/atomic>
#include <ftl/chrono>
#include <ftl/cstddef>
#include <ftl/cstdint>
#include <ftl/detail/thread.hpp>
#include <ftl/detail/wait_notify.hpp>
#include <ftl/limits>
#define FTL_MUTEX_DETAIL_BEGIN_NAMESPACE namespace ftl::detail {
#define FTL_MUTEX_DETAIL_END_NAMESPACE }
#endif

FTL_MUTEX_DETAIL_BEGIN_NAMESPACE

template <class Rep, class Period>
uint64_t mutex_timeout_nanoseconds(
    const chrono::duration<Rep, Period> &duration) noexcept {
  using wide_nanoseconds = chrono::duration<long double, nano>;

  const long double value = wide_nanoseconds(duration).count();

  if (!(value > 0))
    return 0;

  constexpr uint64_t maximum = 0x7fffffffffffffffULL;

  constexpr long double maximum_as_float = 9223372036854775807.0L;

  if (value >= maximum_as_float)
    return maximum;

  uint64_t result = static_cast<uint64_t>(value);

  // Never round a positive timeout downward.
  if (static_cast<long double>(result) < value)
    ++result;

  return result;
}

template <class State, class Clock, class Duration>
bool mutex_try_lock_until(
    State &state, const chrono::time_point<Clock, Duration> &absolute_time) {
  for (;;) {
    const auto now = Clock::now();

    if (!(now < absolute_time))
      return state.try_lock();

    const uint64_t remaining = mutex_timeout_nanoseconds(absolute_time - now);

    if (state.try_lock_for(remaining))
      return true;

    if (!(Clock::now() < absolute_time))
      return false;
  }
}

class mutex_state {
public:
  constexpr mutex_state() noexcept = default;

  mutex_state(const mutex_state &) = delete;
  mutex_state &operator=(const mutex_state &) = delete;

  void lock() noexcept {
    atomic_ref<uint32_t> state{state_};

    uint32_t expected = unlocked;

    if (state.compare_exchange_strong(expected, locked, memory_order_acquire,
                                      memory_order_relaxed)) {
      return;
    }

    lock_slow(state);
  }

  bool try_lock() noexcept {
    atomic_ref<uint32_t> state{state_};

    uint32_t expected = unlocked;

    return state.compare_exchange_strong(expected, locked, memory_order_acquire,
                                         memory_order_relaxed);
  }

  bool try_lock_for(uint64_t timeout_nanoseconds) noexcept {
    return try_lock_until(wait_deadline_after(timeout_nanoseconds));
  }

  bool try_lock_until(uint64_t deadline) noexcept {
    atomic_ref<uint32_t> state{state_};

    uint32_t expected = unlocked;

    if (state.compare_exchange_strong(expected, locked, memory_order_acquire,
                                      memory_order_relaxed)) {
      return true;
    }

    for (;;) {
      const uint32_t previous = state.exchange(contended, memory_order_acquire);

      if (previous == unlocked)
        return true;

      if (!atomic_wait_until_changed_until<sizeof(uint32_t)>(
              &state_, contended, deadline,
              [&] { return state.load(memory_order_relaxed); })) {
        // One final acquisition attempt closes the
        // race between timeout and an unlock.
        expected = unlocked;

        return state.compare_exchange_strong(
            expected, contended, memory_order_acquire, memory_order_relaxed);
      }
    }
  }

  void unlock() noexcept {
    atomic_ref<uint32_t> state{state_};

    const uint32_t previous = state.exchange(unlocked, memory_order_release);

    if (previous == contended)
      state.notify_one();
  }

private:
  void lock_slow(atomic_ref<uint32_t> state) noexcept {
    for (;;) {
      const uint32_t previous = state.exchange(contended, memory_order_acquire);

      if (previous == unlocked)
        return;

      state.wait(contended, memory_order_relaxed);
    }
  }

  static constexpr uint32_t unlocked = 0;
  static constexpr uint32_t locked = 1;
  static constexpr uint32_t contended = 2;

  alignas(atomic_ref<uint32_t>::required_alignment) uint32_t state_ = unlocked;
};

class recursive_mutex_state {
public:
  constexpr recursive_mutex_state() noexcept = default;

  recursive_mutex_state(const recursive_mutex_state &) = delete;

  recursive_mutex_state &operator=(const recursive_mutex_state &) = delete;

  bool lock() noexcept {
    const native_thread_id self = native_this_thread_id();

    if (owner_.load(memory_order_relaxed) == self)
      return recurse();

    gate_.lock();

    owner_.store(self, memory_order_relaxed);

    recursion_ = 1;
    return true;
  }

  bool try_lock() noexcept {
    const native_thread_id self = native_this_thread_id();

    if (owner_.load(memory_order_relaxed) == self)
      return recurse();

    if (!gate_.try_lock())
      return false;

    owner_.store(self, memory_order_relaxed);

    recursion_ = 1;
    return true;
  }

  bool try_lock_for(uint64_t timeout_nanoseconds) noexcept {
    const native_thread_id self = native_this_thread_id();

    if (owner_.load(memory_order_relaxed) == self)
      return recurse();

    if (!gate_.try_lock_for(timeout_nanoseconds))
      return false;

    owner_.store(self, memory_order_relaxed);

    recursion_ = 1;
    return true;
  }

  void unlock() noexcept {
    --recursion_;

    if (recursion_ != 0)
      return;

    owner_.store(native_thread_id{0}, memory_order_relaxed);

    gate_.unlock();
  }

private:
  bool recurse() noexcept {
    if (recursion_ == numeric_limits<size_t>::max()) {
      return false;
    }

    ++recursion_;
    return true;
  }

  mutex_state gate_;

  atomic<native_thread_id> owner_{0};

  size_t recursion_ = 0;
};

class once_state {
public:
  constexpr once_state() noexcept = default;

  once_state(const once_state &) = delete;
  once_state &operator=(const once_state &) = delete;

  template <class Callable> void call(Callable &&callable) {
    atomic_ref<uint32_t> state{state_};

    for (;;) {
      uint32_t current = state.load(memory_order_acquire);

      if (current == complete)
        return;

      if (current == uninitialized) {
        uint32_t expected = uninitialized;

        if (state.compare_exchange_strong(
                expected, active, memory_order_acquire, memory_order_relaxed)) {
#if FTL_HAS_EXCEPTIONS
          try {
            static_cast<Callable &&>(callable)();
          } catch (...) {
            state.store(uninitialized, memory_order_release);

            state.notify_all();

            throw;
          }
#else
          static_cast<Callable &&>(callable)();
#endif

          state.store(complete, memory_order_release);

          state.notify_all();
          return;
        }

        current = expected;
      }

      if (current == active) {
        state.wait(active, memory_order_relaxed);
      }
    }
  }

private:
  static constexpr uint32_t uninitialized = 0;
  static constexpr uint32_t active = 1;
  static constexpr uint32_t complete = 2;

  alignas(atomic_ref<uint32_t>::required_alignment) uint32_t state_ =
      uninitialized;
};

FTL_MUTEX_DETAIL_END_NAMESPACE

#undef FTL_MUTEX_DETAIL_BEGIN_NAMESPACE
#undef FTL_MUTEX_DETAIL_END_NAMESPACE

#endif // FTL_DETAIL_MUTEX_HEADER
