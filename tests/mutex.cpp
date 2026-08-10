#ifdef FTL_REPLACE_STL
#include <atomic>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/atomic>
#include <ftl/mutex>
#include <ftl/thread>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

template <class T>
concept has_mutex_type = requires { typename T::mutex_type; };

static_assert(tested::is_standard_layout_v<tested::mutex>);

static_assert(tested::is_standard_layout_v<tested::recursive_mutex>);

static_assert(tested::is_default_constructible_v<tested::mutex>);

static_assert(tested::is_default_constructible_v<tested::recursive_mutex>);

static_assert(!tested::is_copy_constructible_v<tested::mutex>);

static_assert(!tested::is_copy_assignable_v<tested::mutex>);

static_assert(!tested::is_move_constructible_v<tested::mutex>);

static_assert(!tested::is_move_assignable_v<tested::mutex>);

static_assert(!tested::is_copy_constructible_v<tested::recursive_mutex>);

static_assert(!tested::is_copy_assignable_v<tested::recursive_mutex>);

static_assert(!tested::is_move_constructible_v<tested::recursive_mutex>);

static_assert(!tested::is_move_assignable_v<tested::recursive_mutex>);

static_assert(tested::is_same_v<tested::lock_guard<tested::mutex>::mutex_type,
                                tested::mutex>);

static_assert(tested::is_same_v<tested::unique_lock<tested::mutex>::mutex_type,
                                tested::mutex>);

static_assert(has_mutex_type<tested::scoped_lock<tested::mutex>>);

static_assert(!has_mutex_type<tested::scoped_lock<>>);

static_assert(
    !has_mutex_type<tested::scoped_lock<tested::mutex, tested::mutex>>);

static_assert(
    !tested::is_copy_constructible_v<tested::lock_guard<tested::mutex>>);

static_assert(
    !tested::is_copy_constructible_v<tested::unique_lock<tested::mutex>>);

static_assert(
    tested::is_move_constructible_v<tested::unique_lock<tested::mutex>>);

static_assert(tested::is_move_assignable_v<tested::unique_lock<tested::mutex>>);

static_assert(tested::is_standard_layout_v<tested::timed_mutex>);

static_assert(tested::is_standard_layout_v<tested::recursive_timed_mutex>);

static_assert(!tested::is_copy_constructible_v<tested::timed_mutex>);

static_assert(!tested::is_copy_assignable_v<tested::timed_mutex>);

static_assert(!tested::is_copy_constructible_v<tested::recursive_timed_mutex>);

static_assert(!tested::is_copy_assignable_v<tested::recursive_timed_mutex>);

static_assert(tested::is_default_constructible_v<tested::once_flag>);

static_assert(!tested::is_copy_constructible_v<tested::once_flag>);

static_assert(!tested::is_copy_assignable_v<tested::once_flag>);

static_assert(noexcept(tested::once_flag{}));

static_assert(!noexcept(tested::declval<tested::mutex &>().try_lock()));

static_assert(
    noexcept(tested::declval<tested::recursive_mutex &>().try_lock()));

static_assert(!noexcept(tested::declval<tested::timed_mutex &>().try_lock()));

static_assert(
    noexcept(tested::declval<tested::recursive_timed_mutex &>().try_lock()));

struct test_mutex {
  void lock() {
    ++lock_calls;
    locked = true;
  }

  bool try_lock() {
    ++try_lock_calls;

    if (locked)
      return false;

    locked = true;
    return true;
  }

  void unlock() {
    ++unlock_calls;
    locked = false;
  }

  bool locked = false;

  int lock_calls = 0;
  int try_lock_calls = 0;
  int unlock_calls = 0;
};

bool basic_mutex_operations_work() {
  tested::mutex mutex;

  if (!mutex.try_lock())
    return false;

  mutex.unlock();

  mutex.lock();
  mutex.unlock();

  return true;
}

bool try_lock_reports_contention() {
  tested::mutex mutex;
  bool acquired = true;

  mutex.lock();

  tested::thread contender([&] {
    acquired = mutex.try_lock();

    if (acquired)
      mutex.unlock();
  });

  contender.join();
  mutex.unlock();

  return !acquired;
}

bool blocked_waiter_is_released() {
  tested::mutex mutex;

  tested::atomic<bool> started{false};

  int value = 0;

  mutex.lock();

  tested::thread waiter([&] {
    started.store(true, tested::memory_order_release);

    mutex.lock();

    value = 42;

    mutex.unlock();
  });

  while (!started.load(tested::memory_order_acquire)) {
    tested::this_thread::yield();
  }

  for (int index = 0; index < 64; ++index) {
    tested::this_thread::yield();
  }

  mutex.unlock();
  waiter.join();

  return value == 42;
}

bool mutex_provides_mutual_exclusion() {
  tested::mutex mutex;

  constexpr int iterations = 10000;

  int counter = 0;

  auto worker = [&] {
    for (int index = 0; index < iterations; ++index) {
      mutex.lock();
      ++counter;
      mutex.unlock();
    }
  };

  tested::thread first(worker);
  tested::thread second(worker);
  tested::thread third(worker);
  tested::thread fourth(worker);

  first.join();
  second.join();
  third.join();
  fourth.join();

  return counter == iterations * 4;
}

bool recursive_mutex_recurses() {
  tested::recursive_mutex mutex;

  mutex.lock();
  mutex.lock();

  mutex.unlock();
  mutex.unlock();

  if (!mutex.try_lock())
    return false;

  if (!mutex.try_lock())
    return false;

  mutex.unlock();
  mutex.unlock();

  return true;
}

bool recursive_mutex_blocks_other_threads_until_fully_released() {
  tested::recursive_mutex mutex;

  tested::atomic<int> stage{0};

  bool first_try_succeeded = false;

  mutex.lock();
  mutex.lock();

  tested::thread contender([&] {
    first_try_succeeded = mutex.try_lock();

    if (first_try_succeeded) {
      mutex.unlock();
      stage.store(3, tested::memory_order_release);
      return;
    }

    stage.store(1, tested::memory_order_release);

    mutex.lock();

    stage.store(2, tested::memory_order_release);

    mutex.unlock();
  });

  while (stage.load(tested::memory_order_acquire) == 0) {
    tested::this_thread::yield();
  }

  if (first_try_succeeded) {
    mutex.unlock();
    mutex.unlock();
    contender.join();
    return false;
  }

  // Still recursively owned once after this.
  mutex.unlock();

  for (int index = 0; index < 64; ++index) {
    tested::this_thread::yield();
  }

  if (stage.load(tested::memory_order_acquire) != 1) {
    mutex.unlock();
    contender.join();
    return false;
  }

  mutex.unlock();
  contender.join();

  return stage.load(tested::memory_order_acquire) == 2;
}

bool lock_guard_works() {
  test_mutex mutex;

  {
    tested::lock_guard guard(mutex);

    if (!mutex.locked)
      return false;

    if (mutex.lock_calls != 1)
      return false;
  }

  if (mutex.locked)
    return false;

  if (mutex.unlock_calls != 1)
    return false;

  mutex.lock();

  {
    tested::lock_guard guard(mutex, tested::adopt_lock);

    if (!mutex.locked)
      return false;
  }

  return !mutex.locked && mutex.unlock_calls == 2;
}

bool generic_try_lock_works() {
  test_mutex first;
  test_mutex second;
  test_mutex third;

  second.lock();

  const int result = tested::try_lock(first, second, third);

  if (result != 1)
    return false;

  if (first.locked)
    return false;

  if (!second.locked)
    return false;

  if (third.try_lock_calls != 0)
    return false;

  if (first.unlock_calls != 1)
    return false;

  second.unlock();

  return true;
}

bool generic_try_lock_success_works() {
  test_mutex first;
  test_mutex second;
  test_mutex third;

  if (tested::try_lock(first, second, third) != -1) {
    return false;
  }

  const bool result = first.locked && second.locked && third.locked;

  first.unlock();
  second.unlock();
  third.unlock();

  return result;
}

bool generic_lock_works() {
  test_mutex first;
  test_mutex second;
  test_mutex third;

  tested::lock(first, second, third);

  const bool result = first.locked && second.locked && third.locked;

  first.unlock();
  second.unlock();
  third.unlock();

  return result;
}

bool scoped_lock_works() {
  test_mutex first;
  test_mutex second;
  test_mutex third;

  {
    tested::scoped_lock lock(first, second, third);

    if (!first.locked || !second.locked || !third.locked) {
      return false;
    }
  }

  if (first.locked || second.locked || third.locked) {
    return false;
  }

  first.lock();
  second.lock();

  {
    tested::scoped_lock lock(tested::adopt_lock, first, second);

    if (!first.locked || !second.locked) {
      return false;
    }
  }

  if (first.locked || second.locked) {
    return false;
  }

  {
    tested::scoped_lock<> lock;
  }

  return true;
}

bool scoped_lock_avoids_opposite_order_deadlock() {
  tested::mutex first;
  tested::mutex second;

  constexpr int iterations = 2000;

  int counter = 0;

  tested::thread one([&] {
    for (int index = 0; index < iterations; ++index) {
      tested::scoped_lock lock(first, second);

      ++counter;
    }
  });

  tested::thread two([&] {
    for (int index = 0; index < iterations; ++index) {
      tested::scoped_lock lock(second, first);

      ++counter;
    }
  });

  one.join();
  two.join();

  return counter == iterations * 2;
}

bool unique_lock_default_and_deferred_work() {
  test_mutex mutex;

  tested::unique_lock<test_mutex> empty;

  if (empty.owns_lock())
    return false;

  if (empty)
    return false;

  if (empty.mutex() != nullptr)
    return false;

  tested::unique_lock deferred(mutex, tested::defer_lock);

  if (deferred.owns_lock())
    return false;

  if (deferred.mutex() != &mutex)
    return false;

  deferred.lock();

  if (!deferred.owns_lock())
    return false;

  if (!mutex.locked)
    return false;

  deferred.unlock();

  return !deferred.owns_lock() && !mutex.locked;
}

bool unique_lock_try_and_adopt_work() {
  test_mutex mutex;

  {
    tested::unique_lock lock(mutex, tested::try_to_lock);

    if (!lock)
      return false;

    if (!mutex.locked)
      return false;
  }

  if (mutex.locked)
    return false;

  mutex.lock();

  {
    tested::unique_lock lock(mutex, tested::adopt_lock);

    if (!lock)
      return false;
  }

  return !mutex.locked;
}

bool unique_lock_move_works() {
  test_mutex mutex;

  tested::unique_lock first(mutex);

  tested::unique_lock second(tested::move(first));

  if (first.owns_lock())
    return false;

  if (first.mutex() != nullptr)
    return false;

  if (!second.owns_lock())
    return false;

  if (second.mutex() != &mutex)
    return false;

  test_mutex other;

  tested::unique_lock third(other);

  third = tested::move(second);

  if (second.owns_lock())
    return false;

  if (second.mutex() != nullptr)
    return false;

  if (other.locked)
    return false;

  return third.owns_lock() && third.mutex() == &mutex && mutex.locked;
}

bool unique_lock_release_works() {
  test_mutex mutex;

  tested::unique_lock lock(mutex);

  test_mutex *released = lock.release();

  if (released != &mutex)
    return false;

  if (lock.owns_lock())
    return false;

  if (lock.mutex() != nullptr)
    return false;

  if (!mutex.locked)
    return false;

  released->unlock();

  return !mutex.locked;
}

bool unique_lock_swap_works() {
  test_mutex first_mutex;
  test_mutex second_mutex;

  tested::unique_lock first(first_mutex);

  tested::unique_lock second(second_mutex, tested::defer_lock);

  first.swap(second);

  if (first.mutex() != &second_mutex) {
    return false;
  }

  if (first.owns_lock())
    return false;

  if (second.mutex() != &first_mutex) {
    return false;
  }

  if (!second.owns_lock())
    return false;

  tested::swap(first, second);

  return first.mutex() == &first_mutex && first.owns_lock() &&
         second.mutex() == &second_mutex && !second.owns_lock();
}

bool timed_mutex_immediate_operations_work() {
  tested::timed_mutex mutex;

  if (!mutex.try_lock_for(tested::chrono::milliseconds{0})) {
    return false;
  }

  mutex.unlock();

  const auto deadline =
      tested::chrono::steady_clock::now() + tested::chrono::milliseconds{10};

  if (!mutex.try_lock_until(deadline))
    return false;

  mutex.unlock();

  return true;
}

bool timed_mutex_times_out() {
  tested::timed_mutex mutex;

  mutex.lock();

  bool acquired = true;

  tested::thread contender([&] {
    acquired = mutex.try_lock_for(tested::chrono::milliseconds{20});

    if (acquired)
      mutex.unlock();
  });

  contender.join();
  mutex.unlock();

  return !acquired;
}

bool timed_mutex_acquires_before_timeout() {
  tested::timed_mutex mutex;

  tested::atomic<bool> started{false};

  bool acquired = false;

  mutex.lock();

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

  mutex.unlock();
  contender.join();

  return acquired;
}

bool timed_mutex_until_times_out() {
  tested::timed_mutex mutex;

  mutex.lock();

  bool acquired = true;

  tested::thread contender([&] {
    acquired = mutex.try_lock_until(tested::chrono::system_clock::now() +
                                    tested::chrono::milliseconds{20});

    if (acquired)
      mutex.unlock();
  });

  contender.join();
  mutex.unlock();

  return !acquired;
}

bool recursive_timed_mutex_recurses() {
  tested::recursive_timed_mutex mutex;

  mutex.lock();

  if (!mutex.try_lock_for(tested::chrono::milliseconds{0})) {
    mutex.unlock();
    return false;
  }

  if (!mutex.try_lock_until(tested::chrono::steady_clock::now())) {
    mutex.unlock();
    mutex.unlock();
    return false;
  }

  mutex.unlock();
  mutex.unlock();
  mutex.unlock();

  return true;
}

bool recursive_timed_mutex_blocks_other_threads() {
  tested::recursive_timed_mutex mutex;

  mutex.lock();
  mutex.lock();

  bool acquired = true;

  tested::thread contender([&] {
    acquired = mutex.try_lock_for(tested::chrono::milliseconds{20});

    if (acquired)
      mutex.unlock();
  });

  contender.join();

  mutex.unlock();
  mutex.unlock();

  return !acquired;
}

bool recursive_timed_mutex_acquires_after_release() {
  tested::recursive_timed_mutex mutex;

  tested::atomic<bool> started{false};

  bool acquired = false;

  mutex.lock();
  mutex.lock();

  tested::thread contender([&] {
    started.store(true, tested::memory_order_release);

    acquired = mutex.try_lock_for(tested::chrono::seconds{1});

    if (acquired)
      mutex.unlock();
  });

  while (!started.load(tested::memory_order_acquire)) {
    tested::this_thread::yield();
  }

  mutex.unlock();

  tested::this_thread::sleep_for(tested::chrono::milliseconds{10});

  // One recursive acquisition still remains.
  if (acquired) {
    mutex.unlock();
    contender.join();
    return false;
  }

  mutex.unlock();
  contender.join();

  return acquired;
}

bool unique_lock_timed_operations_work() {
  tested::timed_mutex mutex;

  {
    tested::unique_lock lock(mutex, tested::chrono::milliseconds{10});

    if (!lock.owns_lock())
      return false;
  }

  {
    tested::unique_lock lock(mutex, tested::chrono::steady_clock::now() +
                                        tested::chrono::milliseconds{10});

    if (!lock)
      return false;
  }

  tested::unique_lock deferred(mutex, tested::defer_lock);

  if (!deferred.try_lock_for(tested::chrono::milliseconds{10})) {
    return false;
  }

  deferred.unlock();

  return deferred.try_lock_until(tested::chrono::steady_clock::now() +
                                 tested::chrono::milliseconds{10});
}

bool timed_wait_backend_links() {
  alignas(4) tested::uint32_t word = 1;

  tested::detail::platform_wait_for(&word, 1, sizeof(word), 1);

  return true;
}

bool call_once_runs_exactly_once() {
  tested::once_flag flag;

  tested::atomic<int> calls{0};

  int published = 0;

  auto worker = [&] {
    tested::call_once(flag, [&] {
      calls.fetch_add(1, tested::memory_order_relaxed);

      published = 42;
    });

    // Returning from a passive call_once must
    // observe the effects of the returning
    // active execution.
    return published == 42;
  };

  bool first_result = false;
  bool second_result = false;
  bool third_result = false;
  bool fourth_result = false;

  tested::thread first([&] { first_result = worker(); });

  tested::thread second([&] { second_result = worker(); });

  tested::thread third([&] { third_result = worker(); });

  tested::thread fourth([&] { fourth_result = worker(); });

  first.join();
  second.join();
  third.join();
  fourth.join();

  return calls.load(tested::memory_order_relaxed) == 1 && first_result &&
         second_result && third_result && fourth_result;
}

bool call_once_forwards_arguments() {
  struct target {
    void set(int first, int second) { value = first + second; }

    int value = 0;
  };

  tested::once_flag flag;
  target object;

  tested::call_once(flag, &target::set, &object, 20, 22);

  return object.value == 42;
}

bool call_once_does_not_require_copyable_callable() {
  struct callable {
    explicit callable(int *value) : value(value) {}

    callable(const callable &) = delete;
    callable &operator=(const callable &) = delete;

    callable(callable &&) = default;
    callable &operator=(callable &&) = default;

    void operator()() { *value = 42; }

    int *value;
  };

  tested::once_flag flag;

  int value = 0;

  tested::call_once(flag, callable{&value});

  return value == 42;
}

#if FTL_HAS_EXCEPTIONS

bool call_once_retries_after_exception() {
  tested::once_flag flag;

  int attempts = 0;
  int published = 0;

  bool exception_observed = false;

  try {
    tested::call_once(flag, [&] {
      ++attempts;
      throw 17;
    });
  } catch (int value) {
    exception_observed = value == 17;
  }

  if (!exception_observed)
    return false;

  if (attempts != 1)
    return false;

  tested::call_once(flag, [&] {
    ++attempts;
    published = 42;
  });

  tested::call_once(flag, [&] {
    ++attempts;
    published = 0;
  });

  return attempts == 2 && published == 42;
}

bool concurrent_call_once_retries_after_exception() {
  tested::once_flag flag;

  tested::atomic<int> attempts{0};
  tested::atomic<int> exceptions{0};

  int published = 0;

  auto worker = [&] {
    try {
      tested::call_once(flag, [&] {
        const int attempt = attempts.fetch_add(1, tested::memory_order_relaxed);

        if (attempt == 0)
          throw 123;

        published = 99;
      });
    } catch (int value) {
      if (value == 123) {
        exceptions.fetch_add(1, tested::memory_order_relaxed);
      }
    }
  };

  tested::thread first(worker);
  tested::thread second(worker);
  tested::thread third(worker);
  tested::thread fourth(worker);

  first.join();
  second.join();
  third.join();
  fourth.join();

  if (attempts.load(tested::memory_order_relaxed) != 2) {
    return false;
  }

  if (exceptions.load(tested::memory_order_relaxed) != 1) {
    return false;
  }

  if (published != 99)
    return false;

  tested::call_once(
      flag, [&] { attempts.fetch_add(100, tested::memory_order_relaxed); });

  return attempts.load(tested::memory_order_relaxed) == 2;
}

#endif

bool ftl_test() {
  return basic_mutex_operations_work() && try_lock_reports_contention() &&
         blocked_waiter_is_released() && mutex_provides_mutual_exclusion() &&
         recursive_mutex_recurses() &&
         recursive_mutex_blocks_other_threads_until_fully_released() &&
         lock_guard_works() && generic_try_lock_works() &&
         generic_try_lock_success_works() && generic_lock_works() &&
         scoped_lock_works() && scoped_lock_avoids_opposite_order_deadlock() &&
         unique_lock_default_and_deferred_work() &&
         unique_lock_try_and_adopt_work() && unique_lock_move_works() &&
         unique_lock_release_works() && unique_lock_swap_works() &&
         timed_mutex_immediate_operations_work() && timed_mutex_times_out() &&
         timed_mutex_acquires_before_timeout() &&
         timed_mutex_until_times_out() && recursive_timed_mutex_recurses() &&
         recursive_timed_mutex_blocks_other_threads() &&
         recursive_timed_mutex_acquires_after_release() &&
         unique_lock_timed_operations_work() && timed_wait_backend_links() &&
         call_once_runs_exactly_once() && call_once_forwards_arguments() &&
         call_once_does_not_require_copyable_callable() &&
#if FTL_HAS_EXCEPTIONS
         call_once_retries_after_exception() &&
         concurrent_call_once_retries_after_exception()
#endif
      ;
}
