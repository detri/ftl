#ifdef FTL_REPLACE_STL
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <stop_token>
#include <thread>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/atomic>
#include <ftl/chrono>
#include <ftl/condition_variable>
#include <ftl/mutex>
#include <ftl/stop_token>
#include <ftl/thread>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

static_assert(tested::is_standard_layout_v<tested::condition_variable>);

static_assert(!tested::is_copy_constructible_v<tested::condition_variable>);

static_assert(!tested::is_copy_assignable_v<tested::condition_variable>);

static_assert(!tested::is_copy_constructible_v<tested::condition_variable_any>);

static_assert(!tested::is_copy_assignable_v<tested::condition_variable_any>);

static_assert(
    noexcept(tested::declval<tested::condition_variable &>().notify_one()));

static_assert(
    noexcept(tested::declval<tested::condition_variable &>().notify_all()));

struct tracking_lock {
  void lock() {
    ++lock_calls;
    locked = true;
  }

  void unlock() {
    ++unlock_calls;
    locked = false;
  }

  bool locked = true;
  int lock_calls = 0;
  int unlock_calls = 0;
};

struct thread_exit_probe {
  tested::atomic<bool> *destroyed = nullptr;

  ~thread_exit_probe() {
    if (destroyed != nullptr) {
      destroyed->store(true, tested::memory_order_release);
    }
  }
};

thread_local thread_exit_probe early_exit_probe;

thread_local thread_exit_probe late_exit_probe;

bool condition_variable_predicate_wait_works() {
  tested::mutex mutex;
  tested::condition_variable condition;

  bool ready = false;
  int value = 0;

  tested::unique_lock lock(mutex);

  tested::thread producer([&] {
    tested::unique_lock producer_lock(mutex);

    value = 42;
    ready = true;

    producer_lock.unlock();

    condition.notify_one();
  });

  condition.wait(lock, [&] { return ready; });

  const bool result = value == 42 && lock.owns_lock();

  lock.unlock();
  producer.join();

  return result;
}

bool notify_all_wakes_waiters() {
  tested::mutex mutex;
  tested::condition_variable condition;

  tested::atomic<int> waiting{0};

  bool released = false;

  tested::atomic<int> completed{0};

  auto waiter = [&] {
    tested::unique_lock lock(mutex);

    waiting.fetch_add(1, tested::memory_order_release);

    condition.wait(lock, [&] { return released; });

    completed.fetch_add(1, tested::memory_order_relaxed);
  };

  tested::thread first(waiter);
  tested::thread second(waiter);
  tested::thread third(waiter);

  while (waiting.load(tested::memory_order_acquire) != 3) {
    tested::this_thread::yield();
  }

  {
    tested::lock_guard lock(mutex);
    released = true;
  }

  condition.notify_all();

  first.join();
  second.join();
  third.join();

  return completed.load(tested::memory_order_relaxed) == 3;
}

bool condition_variable_wait_for_times_out() {
  tested::mutex mutex;
  tested::condition_variable condition;

  tested::unique_lock lock(mutex);

  const auto result =
      condition.wait_for(lock, tested::chrono::milliseconds{20});

  return result == tested::cv_status::timeout && lock.owns_lock();
}

bool condition_variable_wait_for_notified() {
  tested::mutex mutex;
  tested::condition_variable condition;

  tested::atomic<bool> waiting{false};

  bool result = false;

  tested::thread waiter([&] {
    tested::unique_lock lock(mutex);

    waiting.store(true, tested::memory_order_release);

    result = condition.wait_for(lock, tested::chrono::seconds{1}) ==
             tested::cv_status::no_timeout;
  });

  while (!waiting.load(tested::memory_order_acquire)) {
    tested::this_thread::yield();
  }

  tested::this_thread::sleep_for(tested::chrono::milliseconds{10});

  condition.notify_one();

  waiter.join();

  return result;
}

bool condition_variable_until_predicate_works() {
  tested::mutex mutex;
  tested::condition_variable condition;

  bool ready = false;

  tested::unique_lock lock(mutex);

  tested::thread producer([&] {
    tested::this_thread::sleep_for(tested::chrono::milliseconds{10});

    tested::lock_guard guard(mutex);

    ready = true;

    condition.notify_one();
  });

  const bool result = condition.wait_until(
      lock, tested::chrono::steady_clock::now() + tested::chrono::seconds{1},
      [&] { return ready; });

  lock.unlock();
  producer.join();

  return result;
}

bool condition_variable_any_works_with_plain_mutex() {
  tested::mutex mutex;
  tested::condition_variable_any condition;

  bool ready = false;
  int value = 0;

  mutex.lock();

  tested::thread producer([&] {
    mutex.lock();

    value = 123;
    ready = true;

    mutex.unlock();

    condition.notify_one();
  });

  condition.wait(mutex, [&] { return ready; });

  const bool result = value == 123;

  mutex.unlock();
  producer.join();

  return result;
}

bool condition_variable_any_timed_wait_works() {
  tested::recursive_mutex mutex;

  tested::condition_variable_any condition;

  mutex.lock();

  const auto result =
      condition.wait_for(mutex, tested::chrono::milliseconds{20});

  mutex.unlock();

  return result == tested::cv_status::timeout;
}

bool condition_variable_any_stop_request_works() {
  tested::mutex mutex;
  tested::condition_variable_any condition;

  tested::stop_source source;

  tested::atomic<bool> started{false};

  bool result = true;

  tested::thread waiter([&] {
    tested::unique_lock lock(mutex);

    started.store(true, tested::memory_order_release);

    result = condition.wait(lock, source.get_token(), [] { return false; });
  });

  while (!started.load(tested::memory_order_acquire)) {
    tested::this_thread::yield();
  }

  source.request_stop();

  waiter.join();

  return !result;
}

bool condition_variable_any_stopped_predicate_wins() {
  tested::mutex mutex;
  tested::condition_variable_any condition;

  tested::stop_source source;

  source.request_stop();

  tested::unique_lock lock(mutex);

  return condition.wait(lock, source.get_token(), [] { return true; });
}

bool condition_variable_any_interruptible_timeout_works() {
  tested::mutex mutex;
  tested::condition_variable_any condition;

  tested::stop_source source;

  tested::unique_lock lock(mutex);

  const bool result = condition.wait_for(lock, source.get_token(),
                                         tested::chrono::milliseconds{20},
                                         [] { return false; });

  return !result && lock.owns_lock();
}

bool notify_all_at_thread_exit_works() {
  tested::mutex mutex;
  tested::condition_variable condition;

  bool finished = false;

  tested::atomic<bool> early_tls_destroyed{false};

  tested::atomic<bool> late_tls_destroyed{false};

  tested::unique_lock lock(mutex);

  tested::thread worker([&] {
    // Construct one TLS object before registration.
    early_exit_probe.destroyed = &early_tls_destroyed;

    tested::unique_lock worker_lock(mutex);

    finished = true;

    tested::notify_all_at_thread_exit(condition, tested::move(worker_lock));

    // This is the important half of the test:
    // construct TLS *after* the notification has
    // already been registered.
    late_exit_probe.destroyed = &late_tls_destroyed;
  });

  condition.wait(lock, [&] { return finished; });

  // Acquiring the mutex after the deferred
  // notification must occur after destruction of
  // every thread-storage-duration object, including
  // one initialized after registration.
  const bool result = early_tls_destroyed.load(tested::memory_order_acquire) &&
                      late_tls_destroyed.load(tested::memory_order_acquire);

  lock.unlock();
  worker.join();

  return result;
}

bool repeated_notifications_do_not_break_waiting() {
  tested::mutex mutex;
  tested::condition_variable condition;

  bool ready = false;

  tested::atomic<bool> waiting{false};

  bool observed = false;

  tested::thread waiter([&] {
    tested::unique_lock lock(mutex);

    waiting.store(true, tested::memory_order_release);

    condition.wait(lock, [&] { return ready; });

    observed = true;
  });

  while (!waiting.load(tested::memory_order_acquire)) {
    tested::this_thread::yield();
  }

  for (int index = 0; index < 32; ++index) {
    condition.notify_one();
    tested::this_thread::yield();
  }

  {
    tested::lock_guard lock(mutex);
    ready = true;
  }

  condition.notify_one();

  waiter.join();

  return observed;
}

bool expired_wait_releases_and_reacquires() {
  tested::condition_variable_any condition;
  tracking_lock lock;

  const auto result = condition.wait_for(lock, tested::chrono::milliseconds{0});

  return result == tested::cv_status::timeout && lock.locked &&
         lock.unlock_calls == 1 && lock.lock_calls == 1;
}

bool ftl_test() {
  return condition_variable_predicate_wait_works() &&
         notify_all_wakes_waiters() &&
         condition_variable_wait_for_times_out() &&
         condition_variable_wait_for_notified() &&
         condition_variable_until_predicate_works() &&
         condition_variable_any_works_with_plain_mutex() &&
         condition_variable_any_timed_wait_works() &&
         condition_variable_any_stop_request_works() &&
         condition_variable_any_stopped_predicate_wins() &&
         condition_variable_any_interruptible_timeout_works() &&
         notify_all_at_thread_exit_works() &&
         repeated_notifications_do_not_break_waiting() &&
         expired_wait_releases_and_reacquires();
}
