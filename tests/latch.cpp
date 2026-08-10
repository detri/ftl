#ifdef FTL_REPLACE_STL
#include <atomic>
#include <latch>
#include <thread>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/atomic>
#include <ftl/latch>
#include <ftl/thread>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

#if __cpp_lib_latch < 201907L
#error <latch> must advertise __cpp_lib_latch
#endif

static_assert(!tested::is_copy_constructible_v<tested::latch>);

static_assert(!tested::is_copy_assignable_v<tested::latch>);

static_assert(tested::latch::max() > 0);

static_assert(!tested::is_move_constructible_v<tested::latch>);
static_assert(!tested::is_move_assignable_v<tested::latch>);

bool zero_latch_is_ready() {
  tested::latch latch(0);

  latch.wait();

  return latch.try_wait();
}

bool count_down_works() {
  tested::latch latch(2);

  if (latch.try_wait())
    return false;

  latch.count_down();

  if (latch.try_wait())
    return false;

  latch.count_down();

  return latch.try_wait();
}

bool count_down_update_works() {
  tested::latch latch(4);

  latch.count_down(3);

  if (latch.try_wait())
    return false;

  latch.count_down();

  return latch.try_wait();
}

bool wait_blocks_until_zero() {
  tested::latch latch(1);

  tested::atomic<bool> waiting{false};
  tested::atomic<bool> returned{false};

  tested::thread waiter([&] {
    waiting.store(true, tested::memory_order_release);

    latch.wait();

    returned.store(true, tested::memory_order_release);
  });

  while (!waiting.load(tested::memory_order_acquire))
    tested::this_thread::yield();

  tested::this_thread::yield();

  const bool blocked = !returned.load(tested::memory_order_acquire);

  latch.count_down();

  waiter.join();

  return blocked && returned.load(tested::memory_order_acquire);
}

bool wait_observes_arriving_threads() {
  tested::latch latch(2);

  int first = 0;
  int second = 0;

  tested::thread one([&] {
    first = 17;
    latch.count_down();
  });

  tested::thread two([&] {
    second = 25;
    latch.count_down();
  });

  latch.wait();

  const bool result = first == 17 && second == 25;

  one.join();
  two.join();

  return result;
}

bool arrive_and_wait_works() {
  tested::latch latch(2);

  tested::atomic<bool> started{false};
  tested::atomic<bool> returned{false};

  tested::thread worker([&] {
    started.store(true, tested::memory_order_release);

    latch.arrive_and_wait();

    returned.store(true, tested::memory_order_release);
  });

  while (!started.load(tested::memory_order_acquire))
    tested::this_thread::yield();

  latch.arrive_and_wait();

  worker.join();

  return returned.load(tested::memory_order_acquire);
}

bool ftl_test() {
  return zero_latch_is_ready() && count_down_works() &&
         count_down_update_works() && wait_blocks_until_zero() &&
         wait_observes_arriving_threads() && arrive_and_wait_works();
}
