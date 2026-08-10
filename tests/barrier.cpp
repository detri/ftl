#ifdef FTL_REPLACE_STL
#include <barrier>
#else
#include <ftl/barrier>
#endif

#if __cpp_lib_barrier < 202302L
#error <barrier> must advertise the N4950 barrier facility
#endif

#ifdef FTL_REPLACE_STL
#include <atomic>
#include <thread>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/atomic>
#include <ftl/thread>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

using default_barrier = tested::barrier<>;
using arrival_token = typename default_barrier::arrival_token;

static_assert(!tested::is_copy_constructible_v<default_barrier>);

static_assert(!tested::is_copy_assignable_v<default_barrier>);

static_assert(tested::is_move_constructible_v<arrival_token>);

static_assert(tested::is_move_assignable_v<arrival_token>);

static_assert(tested::is_destructible_v<arrival_token>);

static_assert(default_barrier::max() > 0);

static_assert(!tested::is_move_constructible_v<default_barrier>);
static_assert(!tested::is_move_assignable_v<default_barrier>);

struct counting_completion {
  tested::atomic<int> *calls;

  void operator()() noexcept {
    calls->fetch_add(1, tested::memory_order_relaxed);
  }
};

struct publishing_completion {
  int *value;

  void operator()() noexcept { *value = 42; }
};

bool barrier_synchronizes_participants() {
  tested::barrier barrier(3);

  int values[3]{0, 0, 0};

  tested::atomic<int> correct{0};

  tested::thread first([&] {
    values[0] = 1;

    barrier.arrive_and_wait();

    if (values[0] + values[1] + values[2] == 6)
      correct.fetch_add(1, tested::memory_order_relaxed);
  });

  tested::thread second([&] {
    values[1] = 2;

    barrier.arrive_and_wait();

    if (values[0] + values[1] + values[2] == 6)
      correct.fetch_add(1, tested::memory_order_relaxed);
  });

  values[2] = 3;

  barrier.arrive_and_wait();

  if (values[0] + values[1] + values[2] == 6)
    correct.fetch_add(1, tested::memory_order_relaxed);

  first.join();
  second.join();

  return correct.load(tested::memory_order_relaxed) == 3;
}

bool barrier_is_reusable() {
  tested::atomic<int> completions{0};

  tested::barrier<counting_completion> barrier(
      2, counting_completion{&completions});

  tested::thread worker([&] {
    for (int phase = 0; phase < 4; ++phase)
      barrier.arrive_and_wait();
  });

  for (int phase = 0; phase < 4; ++phase)
    barrier.arrive_and_wait();

  worker.join();

  return completions.load(tested::memory_order_relaxed) == 4;
}

bool explicit_arrival_token_works() {
  tested::barrier barrier(2);

  tested::atomic<bool> arrived{false};
  tested::atomic<bool> returned{false};

  tested::thread worker([&] {
    auto arrival = barrier.arrive();

    arrived.store(true, tested::memory_order_release);

    barrier.wait(tested::move(arrival));

    returned.store(true, tested::memory_order_release);
  });

  while (!arrived.load(tested::memory_order_acquire))
    tested::this_thread::yield();

  tested::this_thread::yield();

  const bool blocked = !returned.load(tested::memory_order_acquire);

  barrier.arrive_and_wait();

  worker.join();

  return blocked && returned.load(tested::memory_order_acquire);
}

bool arrive_update_works() {
  tested::atomic<int> completions{0};

  tested::barrier<counting_completion> barrier(
      3, counting_completion{&completions});

  auto arrival = barrier.arrive(2);

  barrier.arrive_and_wait();

  // The token now refers to the immediately
  // preceding completed phase and must return
  // immediately.
  barrier.wait(tested::move(arrival));

  return completions.load(tested::memory_order_relaxed) == 1;
}

bool completion_happens_before_waiters_return() {
  int published = 0;

  tested::barrier<publishing_completion> barrier(
      2, publishing_completion{&published});

  tested::atomic<bool> worker_observed{false};

  tested::thread worker([&] {
    barrier.arrive_and_wait();

    worker_observed.store(published == 42, tested::memory_order_release);
  });

  barrier.arrive_and_wait();

  const bool main_observed = published == 42;

  worker.join();

  return main_observed && worker_observed.load(tested::memory_order_acquire);
}

bool arrive_and_drop_changes_future_phases() {
  tested::atomic<int> completions{0};

  tested::barrier<counting_completion> barrier(
      3, counting_completion{&completions});

  tested::thread dropper([&] { barrier.arrive_and_drop(); });

  tested::thread worker([&] {
    for (int phase = 0; phase < 3; ++phase)
      barrier.arrive_and_wait();
  });

  for (int phase = 0; phase < 3; ++phase)
    barrier.arrive_and_wait();

  dropper.join();
  worker.join();

  return completions.load(tested::memory_order_relaxed) == 3;
}

bool ftl_test() {
  return barrier_synchronizes_participants() && barrier_is_reusable() &&
         explicit_arrival_token_works() && arrive_update_works() &&
         completion_happens_before_waiters_return() &&
         arrive_and_drop_changes_future_phases();
}
