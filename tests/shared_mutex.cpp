#ifdef FTL_REPLACE_STL
#include <atomic>
#include <chrono>
#include <shared_mutex>
#include <thread>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/atomic>
#include <ftl/chrono>
#include <ftl/shared_mutex>
#include <ftl/thread>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

#ifndef __cpp_lib_shared_timed_mutex
#error __cpp_lib_shared_timed_mutex missing
#endif

#ifndef __cpp_lib_shared_mutex
#error __cpp_lib_shared_mutex missing
#endif

static_assert(__cpp_lib_shared_timed_mutex >= 201402L);

static_assert(__cpp_lib_shared_mutex >= 201505L);

static_assert(tested::is_standard_layout_v<tested::shared_mutex>);

static_assert(tested::is_standard_layout_v<tested::shared_timed_mutex>);

static_assert(!tested::is_copy_constructible_v<tested::shared_mutex>);

static_assert(!tested::is_copy_assignable_v<tested::shared_mutex>);

static_assert(!tested::is_move_constructible_v<tested::shared_mutex>);

static_assert(!tested::is_move_assignable_v<tested::shared_mutex>);

static_assert(!tested::is_copy_constructible_v<tested::shared_timed_mutex>);

static_assert(!tested::is_copy_assignable_v<tested::shared_timed_mutex>);

static_assert(tested::is_same_v<
              typename tested::shared_lock<tested::shared_mutex>::mutex_type,
              tested::shared_mutex>);

static_assert(!tested::is_copy_constructible_v<
              tested::shared_lock<tested::shared_mutex>>);

static_assert(
    tested::is_move_constructible_v<tested::shared_lock<tested::shared_mutex>>);

static_assert(
    tested::is_move_assignable_v<tested::shared_lock<tested::shared_mutex>>);

struct test_shared_mutex {
  void lock_shared() {
    ++lock_shared_calls;
    shared = true;
  }

  bool try_lock_shared() {
    ++try_lock_shared_calls;

    if (shared)
      return false;

    shared = true;
    return true;
  }

  void unlock_shared() {
    ++unlock_shared_calls;
    shared = false;
  }

  bool shared = false;

  int lock_shared_calls = 0;
  int try_lock_shared_calls = 0;
  int unlock_shared_calls = 0;
};

bool multiple_shared_owners_work() {
  tested::shared_mutex mutex;

  bool acquired = false;

  mutex.lock_shared();

  tested::thread second([&] {
    acquired = mutex.try_lock_shared();

    if (acquired)
      mutex.unlock_shared();
  });

  second.join();

  mutex.unlock_shared();

  return acquired;
}

bool shared_ownership_blocks_exclusive() {
  tested::shared_mutex mutex;

  bool acquired = true;

  mutex.lock_shared();

  tested::thread contender([&] {
    acquired = mutex.try_lock();

    if (acquired)
      mutex.unlock();
  });

  contender.join();

  mutex.unlock_shared();

  return !acquired;
}

bool exclusive_ownership_blocks_shared() {
  tested::shared_mutex mutex;

  bool acquired = true;

  mutex.lock();

  tested::thread contender([&] {
    acquired = mutex.try_lock_shared();

    if (acquired)
      mutex.unlock_shared();
  });

  contender.join();

  mutex.unlock();

  return !acquired;
}

bool writer_acquires_after_readers_leave() {
  tested::shared_mutex mutex;

  tested::atomic<bool> started{false};
  tested::atomic<bool> acquired{false};

  mutex.lock_shared();

  tested::thread writer([&] {
    started.store(true, tested::memory_order_release);

    mutex.lock();

    acquired.store(true, tested::memory_order_release);

    mutex.unlock();
  });

  while (!started.load(tested::memory_order_acquire)) {
    tested::this_thread::yield();
  }

  for (int index = 0; index < 64; ++index) {
    tested::this_thread::yield();
  }

  if (acquired.load(tested::memory_order_acquire)) {
    mutex.unlock_shared();
    writer.join();
    return false;
  }

  mutex.unlock_shared();
  writer.join();

  return acquired.load(tested::memory_order_acquire);
}

bool reader_acquires_after_writer_leaves() {
  tested::shared_mutex mutex;

  tested::atomic<bool> started{false};
  tested::atomic<bool> acquired{false};

  mutex.lock();

  tested::thread reader([&] {
    started.store(true, tested::memory_order_release);

    mutex.lock_shared();

    acquired.store(true, tested::memory_order_release);

    mutex.unlock_shared();
  });

  while (!started.load(tested::memory_order_acquire)) {
    tested::this_thread::yield();
  }

  for (int index = 0; index < 64; ++index) {
    tested::this_thread::yield();
  }

  if (acquired.load(tested::memory_order_acquire)) {
    mutex.unlock();
    reader.join();
    return false;
  }

  mutex.unlock();
  reader.join();

  return acquired.load(tested::memory_order_acquire);
}

bool shared_timed_exclusive_times_out() {
  tested::shared_timed_mutex mutex;

  mutex.lock_shared();

  bool acquired = true;

  tested::thread contender([&] {
    acquired = mutex.try_lock_for(tested::chrono::milliseconds{20});

    if (acquired)
      mutex.unlock();
  });

  contender.join();

  mutex.unlock_shared();

  return !acquired;
}

bool shared_timed_reader_times_out() {
  tested::shared_timed_mutex mutex;

  mutex.lock();

  bool acquired = true;

  tested::thread contender([&] {
    acquired = mutex.try_lock_shared_for(tested::chrono::milliseconds{20});

    if (acquired)
      mutex.unlock_shared();
  });

  contender.join();

  mutex.unlock();

  return !acquired;
}

bool shared_timed_writer_acquires() {
  tested::shared_timed_mutex mutex;

  tested::atomic<bool> started{false};

  bool acquired = false;

  mutex.lock_shared();

  tested::thread contender([&] {
    started.store(true, tested::memory_order_release);

    acquired = mutex.try_lock_for(tested::chrono::seconds{1});

    if (acquired)
      mutex.unlock();
  });

  while (!started.load(tested::memory_order_acquire)) {
    tested::this_thread::yield();
  }

  tested::this_thread::sleep_for(tested::chrono::milliseconds{10});

  mutex.unlock_shared();
  contender.join();

  return acquired;
}

bool shared_timed_reader_acquires() {
  tested::shared_timed_mutex mutex;

  tested::atomic<bool> started{false};

  bool acquired = false;

  mutex.lock();

  tested::thread contender([&] {
    started.store(true, tested::memory_order_release);

    acquired = mutex.try_lock_shared_for(tested::chrono::seconds{1});

    if (acquired)
      mutex.unlock_shared();
  });

  while (!started.load(tested::memory_order_acquire)) {
    tested::this_thread::yield();
  }

  tested::this_thread::sleep_for(tested::chrono::milliseconds{10});

  mutex.unlock();
  contender.join();

  return acquired;
}

bool shared_timed_until_works() {
  tested::shared_timed_mutex mutex;

  if (!mutex.try_lock_until(tested::chrono::steady_clock::now() +
                            tested::chrono::milliseconds{20})) {
    return false;
  }

  mutex.unlock();

  if (!mutex.try_lock_shared_until(tested::chrono::system_clock::now() +
                                   tested::chrono::milliseconds{20})) {
    return false;
  }

  mutex.unlock_shared();

  return true;
}

bool shared_lock_basic_operations_work() {
  test_shared_mutex mutex;

  {
    tested::shared_lock lock(mutex);

    if (!lock.owns_lock())
      return false;

    if (!mutex.shared)
      return false;

    if (lock.mutex() != &mutex)
      return false;
  }

  if (mutex.shared)
    return false;

  if (mutex.unlock_shared_calls != 1)
    return false;

  return true;
}

bool shared_lock_defer_try_adopt_work() {
  test_shared_mutex mutex;

  tested::shared_lock deferred(mutex, tested::defer_lock);

  if (deferred)
    return false;

  deferred.lock();

  if (!deferred)
    return false;

  deferred.unlock();

  {
    tested::shared_lock lock(mutex, tested::try_to_lock);

    if (!lock)
      return false;
  }

  mutex.lock_shared();

  {
    tested::shared_lock lock(mutex, tested::adopt_lock);

    if (!lock)
      return false;
  }

  return !mutex.shared;
}

bool shared_lock_move_release_swap_work() {
  test_shared_mutex first_mutex;
  test_shared_mutex second_mutex;

  tested::shared_lock first(first_mutex);

  tested::shared_lock second(tested::move(first));

  if (first.owns_lock())
    return false;

  if (first.mutex() != nullptr)
    return false;

  if (!second.owns_lock())
    return false;

  tested::shared_lock third(second_mutex, tested::defer_lock);

  second.swap(third);

  if (second.mutex() != &second_mutex) {
    return false;
  }

  if (second.owns_lock())
    return false;

  if (third.mutex() != &first_mutex) {
    return false;
  }

  if (!third.owns_lock())
    return false;

  test_shared_mutex *released = third.release();

  if (released != &first_mutex)
    return false;

  if (third.owns_lock())
    return false;

  if (third.mutex() != nullptr)
    return false;

  if (!first_mutex.shared)
    return false;

  released->unlock_shared();

  return !first_mutex.shared;
}

bool timed_shared_lock_works() {
  tested::shared_timed_mutex mutex;

  {
    tested::shared_lock lock(mutex, tested::chrono::milliseconds{20});

    if (!lock)
      return false;
  }

  {
    tested::shared_lock lock(mutex, tested::chrono::steady_clock::now() +
                                        tested::chrono::milliseconds{20});

    if (!lock)
      return false;
  }

  tested::shared_lock lock(mutex, tested::defer_lock);

  if (!lock.try_lock_for(tested::chrono::milliseconds{20})) {
    return false;
  }

  lock.unlock();

  return lock.try_lock_until(tested::chrono::steady_clock::now() +
                             tested::chrono::milliseconds{20});
}

bool ftl_test() {
  return multiple_shared_owners_work() && shared_ownership_blocks_exclusive() &&
         exclusive_ownership_blocks_shared() &&
         writer_acquires_after_readers_leave() &&
         reader_acquires_after_writer_leaves() &&
         shared_timed_exclusive_times_out() &&
         shared_timed_reader_times_out() && shared_timed_writer_acquires() &&
         shared_timed_reader_acquires() && shared_timed_until_works() &&
         shared_lock_basic_operations_work() &&
         shared_lock_defer_try_adopt_work() &&
         shared_lock_move_release_swap_work() && timed_shared_lock_works();
}
