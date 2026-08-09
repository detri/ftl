// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_SHARED_MUTEX_HEADER
#define FTL_DETAIL_SHARED_MUTEX_HEADER

#ifdef FTL_REPLACE_STL
#include <atomic>
#include <cstdint>
#include <detail/mutex.hpp>
#include <detail/wait_notify.hpp>
#include <limits>
#define FTL_SHARED_MUTEX_DETAIL_BEGIN_NAMESPACE namespace std::detail {
#define FTL_SHARED_MUTEX_DETAIL_END_NAMESPACE }
#else
#include <ftl/atomic>
#include <ftl/cstdint>
#include <ftl/detail/mutex.hpp>
#include <ftl/detail/wait_notify.hpp>
#include <ftl/limits>
#define FTL_SHARED_MUTEX_DETAIL_BEGIN_NAMESPACE namespace ftl::detail {
#define FTL_SHARED_MUTEX_DETAIL_END_NAMESPACE }
#endif

FTL_SHARED_MUTEX_DETAIL_BEGIN_NAMESPACE

class shared_mutex_state {
public:
  constexpr shared_mutex_state() noexcept = default;

  shared_mutex_state(const shared_mutex_state &) = delete;
  shared_mutex_state &operator=(const shared_mutex_state &) = delete;

  void lock() noexcept {
    gate_.lock();

    ++waiting_writers_;

    for (;;) {
      if (!writer_ && readers_ == 0) {
        --waiting_writers_;
        writer_ = true;

        gate_.unlock();
        return;
      }

      const uint32_t observed = epoch_load();

      gate_.unlock();

      epoch_wait(observed);

      gate_.lock();
    }
  }

  bool try_lock() noexcept {
    if (!gate_.try_lock())
      return false;

    const bool acquired = !writer_ && readers_ == 0;

    if (acquired)
      writer_ = true;

    gate_.unlock();

    return acquired;
  }

  bool try_lock_for(uint64_t timeout_nanoseconds) noexcept {
    if (timeout_nanoseconds == 0)
      return try_lock();

    return try_lock_until(wait_deadline_after(timeout_nanoseconds));
  }

  bool try_lock_until(uint64_t deadline) noexcept {
    if (wait_remaining(deadline) == 0)
      return try_lock();

    if (!gate_.try_lock_until(deadline))
      return false;

    if (!writer_ && readers_ == 0) {
      writer_ = true;

      gate_.unlock();
      return true;
    }

    ++waiting_writers_;

    for (;;) {
      const uint32_t observed = epoch_load();

      gate_.unlock();

      if (!epoch_wait_until(observed, deadline)) {
        abandon_writer_wait();
        return false;
      }

      if (!gate_.try_lock_until(deadline)) {
        abandon_writer_wait();
        return false;
      }

      if (!writer_ && readers_ == 0) {
        --waiting_writers_;
        writer_ = true;

        gate_.unlock();
        return true;
      }
    }
  }

  void unlock() noexcept {
    gate_.lock();

    writer_ = false;

    epoch_bump();

    gate_.unlock();

    epoch_notify_all();
  }

  void lock_shared() noexcept {
    gate_.lock();

    for (;;) {
      if (can_lock_shared()) {
        ++readers_;

        gate_.unlock();
        return;
      }

      const uint32_t observed = epoch_load();

      gate_.unlock();

      epoch_wait(observed);

      gate_.lock();
    }
  }

  bool try_lock_shared() noexcept {
    if (!gate_.try_lock())
      return false;

    const bool acquired = can_lock_shared();

    if (acquired)
      ++readers_;

    gate_.unlock();

    return acquired;
  }

  bool try_lock_shared_for(uint64_t timeout_nanoseconds) noexcept {
    if (timeout_nanoseconds == 0)
      return try_lock_shared();

    return try_lock_shared_until(wait_deadline_after(timeout_nanoseconds));
  }

  bool try_lock_shared_until(uint64_t deadline) noexcept {
    if (wait_remaining(deadline) == 0)
      return try_lock_shared();

    if (!gate_.try_lock_until(deadline))
      return false;

    for (;;) {
      if (can_lock_shared()) {
        ++readers_;

        gate_.unlock();
        return true;
      }

      const uint32_t observed = epoch_load();

      gate_.unlock();

      if (!epoch_wait_until(observed, deadline)) {
        return false;
      }

      if (!gate_.try_lock_until(deadline)) {
        return false;
      }
    }
  }

  void unlock_shared() noexcept {
    gate_.lock();

    const bool capacity_was_full = readers_ == max_readers;

    --readers_;

    const bool wake =
        readers_ == 0 || (capacity_was_full && waiting_writers_ == 0);

    if (wake)
      epoch_bump();

    gate_.unlock();

    if (wake)
      epoch_notify_all();
  }

private:
  bool can_lock_shared() const noexcept {
    return !writer_ && waiting_writers_ == 0 && readers_ < max_readers;
  }

  uint32_t epoch_load() noexcept {
    atomic_ref<uint32_t> epoch{epoch_};

    return epoch.load(memory_order_relaxed);
  }

  void epoch_bump() noexcept {
    atomic_ref<uint32_t> epoch{epoch_};

    (void)epoch.fetch_add(uint32_t{1}, memory_order_release);
  }

  void epoch_wait(uint32_t observed) noexcept {
    atomic_ref<uint32_t> epoch{epoch_};

    epoch.wait(observed, memory_order_relaxed);
  }

  bool epoch_wait_until(uint32_t observed, uint64_t deadline) noexcept {
    return atomic_wait_until_changed_until<sizeof(uint32_t)>(
        &epoch_, observed, deadline, [&] { return epoch_load(); });
  }

  void epoch_notify_all() noexcept {
    atomic_ref<uint32_t> epoch{epoch_};

    epoch.notify_all();
  }

  void abandon_writer_wait() noexcept {
    gate_.lock();

    --waiting_writers_;

    // If this was the last queued writer,
    // readers that were held back solely by
    // writer preference can proceed again.
    const bool wake_readers = waiting_writers_ == 0 && !writer_;

    if (wake_readers)
      epoch_bump();

    gate_.unlock();

    if (wake_readers)
      epoch_notify_all();
  }

  static constexpr uint32_t max_readers = numeric_limits<uint32_t>::max();

  mutex_state gate_;

  alignas(atomic_ref<uint32_t>::required_alignment) uint32_t epoch_ = 0;

  uint32_t readers_ = 0;
  uint32_t waiting_writers_ = 0;
  bool writer_ = false;
};

FTL_SHARED_MUTEX_DETAIL_END_NAMESPACE

#undef FTL_SHARED_MUTEX_DETAIL_BEGIN_NAMESPACE
#undef FTL_SHARED_MUTEX_DETAIL_END_NAMESPACE

#endif // FTL_DETAIL_SHARED_MUTEX_HEADER
