#ifdef FTL_REPLACE_STL
#include <atomic>
#include <chrono>
#include <semaphore>
#include <thread>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/atomic>
#include <ftl/chrono>
#include <ftl/semaphore>
#include <ftl/thread>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

#if __cpp_lib_semaphore < 201907L
#error <semaphore> must advertise __cpp_lib_semaphore
#endif

static_assert(!tested::is_copy_constructible_v<tested::counting_semaphore<4>>);

static_assert(!tested::is_copy_assignable_v<tested::counting_semaphore<4>>);

static_assert(tested::counting_semaphore<7>::max() >= 7);

static_assert(tested::binary_semaphore::max() >= 1);

static_assert(!tested::is_move_constructible_v<tested::counting_semaphore<4>>);

static_assert(!tested::is_move_assignable_v<tested::counting_semaphore<4>>);

bool immediate_acquisition_works() {
  tested::counting_semaphore<4> semaphore(2);

  return semaphore.try_acquire() && semaphore.try_acquire() &&
         !semaphore.try_acquire();
}

bool release_update_works() {
  tested::counting_semaphore<4> semaphore(0);

  semaphore.release(3);

  return semaphore.try_acquire() && semaphore.try_acquire() &&
         semaphore.try_acquire() && !semaphore.try_acquire();
}

bool acquire_synchronizes_with_release() {
  tested::binary_semaphore semaphore(0);

  int value = 0;

  tested::thread producer([&] {
    value = 42;
    semaphore.release();
  });

  semaphore.acquire();

  const bool result = value == 42;

  producer.join();

  return result;
}

bool acquire_blocks_until_release() {
  tested::binary_semaphore semaphore(0);

  tested::atomic<bool> entered{false};
  tested::atomic<bool> passed{false};

  tested::thread waiter([&] {
    entered.store(true, tested::memory_order_release);

    semaphore.acquire();

    passed.store(true, tested::memory_order_release);
  });

  while (!entered.load(tested::memory_order_acquire))
    tested::this_thread::yield();

  tested::this_thread::sleep_for(tested::chrono::milliseconds{10});

  const bool blocked = !passed.load(tested::memory_order_acquire);

  semaphore.release();

  waiter.join();

  return blocked && passed.load(tested::memory_order_acquire);
}

bool timed_acquire_times_out() {
  tested::binary_semaphore semaphore(0);

  return !semaphore.try_acquire_for(tested::chrono::milliseconds{20});
}

bool timed_acquire_succeeds() {
  tested::binary_semaphore semaphore(0);

  tested::thread producer([&] {
    tested::this_thread::sleep_for(tested::chrono::milliseconds{10});

    semaphore.release();
  });

  const bool result = semaphore.try_acquire_for(tested::chrono::seconds{1});

  producer.join();

  return result;
}

bool acquire_until_succeeds() {
  tested::binary_semaphore semaphore(0);

  tested::thread producer([&] {
    tested::this_thread::sleep_for(tested::chrono::milliseconds{10});

    semaphore.release();
  });

  const bool result = semaphore.try_acquire_until(
      tested::chrono::steady_clock::now() + tested::chrono::seconds{1});

  producer.join();

  return result;
}

bool zero_timeout_still_attempts_acquire() {
  tested::binary_semaphore available(1);
  tested::binary_semaphore unavailable(0);

  return available.try_acquire_for(tested::chrono::milliseconds{0}) &&
         !unavailable.try_acquire_for(tested::chrono::milliseconds{0});
}

bool release_update_wakes_waiters() {
  tested::counting_semaphore<4> semaphore(0);

  tested::atomic<int> entered{0};
  tested::atomic<int> completed{0};

  auto waiter = [&] {
    entered.fetch_add(1, tested::memory_order_release);

    semaphore.acquire();

    completed.fetch_add(1, tested::memory_order_relaxed);
  };

  tested::thread first(waiter);
  tested::thread second(waiter);
  tested::thread third(waiter);

  while (entered.load(tested::memory_order_acquire) != 3)
    tested::this_thread::yield();

  semaphore.release(3);

  first.join();
  second.join();
  third.join();

  return completed.load(tested::memory_order_relaxed) == 3;
}

bool ftl_test() {
  return immediate_acquisition_works() && release_update_works() &&
         release_update_wakes_waiters() &&
         acquire_synchronizes_with_release() &&
         acquire_blocks_until_release() && timed_acquire_times_out() &&
         timed_acquire_succeeds() && acquire_until_succeeds() &&
         zero_timeout_still_attempts_acquire();
}
