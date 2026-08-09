// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_CONDITION_VARIABLE_HEADER
#define FTL_DETAIL_CONDITION_VARIABLE_HEADER

#ifdef FTL_REPLACE_STL
#include <atomic>
#include <cstdint>
#include <detail/mutex.hpp>
#include <detail/wait_notify.hpp>
#include <exception>
#define FTL_CONDITION_VARIABLE_DETAIL_BEGIN_NAMESPACE namespace std::detail {
#define FTL_CONDITION_VARIABLE_DETAIL_END_NAMESPACE }
#else
#include <ftl/atomic>
#include <ftl/cstdint>
#include <ftl/detail/mutex.hpp>
#include <ftl/detail/wait_notify.hpp>
#include <ftl/exception>
#define FTL_CONDITION_VARIABLE_DETAIL_BEGIN_NAMESPACE namespace ftl::detail {
#define FTL_CONDITION_VARIABLE_DETAIL_END_NAMESPACE }
#endif

FTL_CONDITION_VARIABLE_DETAIL_BEGIN_NAMESPACE

class condition_variable_state {
public:
  constexpr condition_variable_state() noexcept = default;

  condition_variable_state(const condition_variable_state &) = delete;

  condition_variable_state &
  operator=(const condition_variable_state &) = delete;

  uint32_t prepare_wait() noexcept {
    atomic_ref<uint32_t> epoch{epoch_};

    return epoch.load(memory_order_relaxed);
  }

  void wait(uint32_t observed) noexcept {
    atomic_ref<uint32_t> epoch{epoch_};

    epoch.wait(observed, memory_order_relaxed);
  }

  bool wait_until(uint32_t observed, uint64_t deadline) noexcept {
    return atomic_wait_until_changed_until<sizeof(uint32_t)>(
        &epoch_, observed, deadline, [&] {
          atomic_ref<uint32_t> epoch{epoch_};

          return epoch.load(memory_order_relaxed);
        });
  }

  void notify_one() noexcept {
    atomic_ref<uint32_t> epoch{epoch_};

    (void)epoch.fetch_add(uint32_t{1}, memory_order_release);

    epoch.notify_one();
  }

  void notify_all() noexcept {
    atomic_ref<uint32_t> epoch{epoch_};

    (void)epoch.fetch_add(uint32_t{1}, memory_order_release);

    epoch.notify_all();
  }

private:
  alignas(atomic_ref<uint32_t>::required_alignment) uint32_t epoch_ = 0;
};

template <class Lock> void condition_relock(Lock &lock) noexcept {
#if FTL_HAS_EXCEPTIONS
  try {
    lock.lock();
  } catch (...) {
    terminate();
  }
#else
  lock.lock();
#endif
}

template <class Lock>
void condition_wait(condition_variable_state &state, Lock &lock) noexcept {
  const uint32_t observed = state.prepare_wait();

  lock.unlock();

  state.wait(observed);

  condition_relock(lock);
}

template <class Lock, class Clock, class Duration>
bool condition_wait_until(
    condition_variable_state &state, Lock &lock,
    const chrono::time_point<Clock, Duration> &absolute_time) {
  const auto now = Clock::now();

  if (!(now < absolute_time))
    return true;

  const uint64_t timeout = mutex_timeout_nanoseconds(absolute_time - now);

  const uint64_t deadline = wait_deadline_after(timeout);

  const uint32_t observed = state.prepare_wait();

  lock.unlock();

  (void)state.wait_until(observed, deadline);

  condition_relock(lock);

  return !(Clock::now() < absolute_time);
}

template <class Lock, class StopToken>
void condition_wait_interruptible(condition_variable_state &state, Lock &lock,
                                  const StopToken &token) noexcept {
  const uint32_t observed = state.prepare_wait();

  // This check occurs after sampling the
  // generation. If stop is requested after
  // this point, its callback changes the
  // generation and cannot be lost in the
  // unlock-to-sleep window.
  if (token.stop_requested())
    return;

  lock.unlock();

  state.wait(observed);

  condition_relock(lock);
}

template <class Lock, class StopToken, class Clock, class Duration>
bool condition_wait_until_interruptible(
    condition_variable_state &state, Lock &lock, const StopToken &token,
    const chrono::time_point<Clock, Duration> &absolute_time) {
  const auto now = Clock::now();

  if (!(now < absolute_time))
    return true;

  const uint64_t timeout = mutex_timeout_nanoseconds(absolute_time - now);

  const uint64_t deadline = wait_deadline_after(timeout);

  const uint32_t observed = state.prepare_wait();

  if (token.stop_requested())
    return false;

  lock.unlock();

  (void)state.wait_until(observed, deadline);

  condition_relock(lock);

  return !(Clock::now() < absolute_time);
}

struct condition_stop_notifier {
  condition_variable_state *state;

  void operator()() const noexcept { state->notify_all(); }
};

FTL_CONDITION_VARIABLE_DETAIL_END_NAMESPACE

#undef FTL_CONDITION_VARIABLE_DETAIL_BEGIN_NAMESPACE
#undef FTL_CONDITION_VARIABLE_DETAIL_END_NAMESPACE

#endif // FTL_DETAIL_CONDITION_VARIABLE_HEADER
