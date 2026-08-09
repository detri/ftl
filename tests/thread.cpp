#ifdef FTL_REPLACE_STL
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/atomic>
#include <ftl/chrono>
#include <ftl/memory>
#include <ftl/thread>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(tested::is_default_constructible_v<tested::thread>);

static_assert(tested::is_move_constructible_v<tested::thread>);

static_assert(!tested::is_copy_constructible_v<tested::thread>);

static_assert(tested::is_trivially_copyable_v<tested::thread::id>);

static_assert(tested::is_default_constructible_v<tested::jthread>);

static_assert(tested::is_move_constructible_v<tested::jthread>);

static_assert(tested::is_move_assignable_v<tested::jthread>);

static_assert(!tested::is_copy_constructible_v<tested::jthread>);

static_assert(!tested::is_copy_assignable_v<tested::jthread>);

static_assert(tested::is_same_v<tested::jthread::id, tested::thread::id>);

static_assert(tested::is_same_v<tested::jthread::native_handle_type,
                                tested::thread::native_handle_type>);

bool basic_thread_works() {
  int value = 0;

  tested::thread worker{[&] { value = 42; }};

  if (!worker.joinable())
    return false;

  worker.join();

  return !worker.joinable() && value == 42;
}

bool arguments_work() {
  int result = 0;

  tested::thread worker{
      [](int *output, int left, int right) { *output = left + right; }, &result,
      20, 22};

  worker.join();

  return result == 42;
}

bool move_only_argument_works() {
  int result = 0;

  tested::thread worker{
      [&result](tested::unique_ptr<int> value) { result = *value; },
      tested::make_unique<int>(42)};

  worker.join();

  return result == 42;
}

bool thread_ids_work() {
  const tested::thread::id main_id = tested::this_thread::get_id();

  if (main_id == tested::thread::id{})
    return false;

  tested::thread::id observed{};

  tested::thread worker{[&] { observed = tested::this_thread::get_id(); }};

  const tested::thread::id created = worker.get_id();

  if (created == tested::thread::id{})
    return false;

  if (created == main_id)
    return false;

  worker.join();

  return observed == created && worker.get_id() == tested::thread::id{};
}

bool move_thread_works() {
  tested::atomic_bool ran{false};

  tested::thread first{[&] { ran.store(true, tested::memory_order_release); }};

  const auto id = first.get_id();

  tested::thread second{tested::move(first)};

  if (first.joinable())
    return false;

  if (!second.joinable())
    return false;

  if (second.get_id() != id)
    return false;

  second.join();

  return ran.load(tested::memory_order_acquire);
}

bool detach_works() {
  tested::atomic_bool done{false};

  tested::thread worker{[&] {
    done.store(true, tested::memory_order_release);

    done.notify_one();
  }};

  worker.detach();

  if (worker.joinable())
    return false;

  done.wait(false, tested::memory_order_acquire);

  return done.load(tested::memory_order_acquire);
}

template <class Atomic, class T>
bool atomic_wait_one_works(T initial, T changed) {
  Atomic value{initial};
  tested::atomic_bool completed{false};

  tested::thread waiter{[&] {
    value.wait(initial, tested::memory_order_acquire);

    completed.store(value.load(tested::memory_order_acquire) == changed,
                    tested::memory_order_release);
  }};

  value.store(changed, tested::memory_order_release);

  value.notify_one();

  waiter.join();

  return completed.load(tested::memory_order_acquire);
}

bool atomic_wait_widths_work() {
  return atomic_wait_one_works<tested::atomic_uint8_t>(tested::uint8_t{1},
                                                       tested::uint8_t{2}) &&
         atomic_wait_one_works<tested::atomic_uint16_t>(tested::uint16_t{1},
                                                        tested::uint16_t{2}) &&
         atomic_wait_one_works<tested::atomic_uint32_t>(tested::uint32_t{1},
                                                        tested::uint32_t{2}) &&
         atomic_wait_one_works<tested::atomic_uint64_t>(tested::uint64_t{1},
                                                        tested::uint64_t{2});
}

bool atomic_notify_all_works() {
  tested::atomic_int value{0};
  tested::atomic_int completed{0};

  tested::thread workers[4];

  for (int index = 0; index < 4; ++index) {
    workers[index] = tested::thread{[&] {
      value.wait(0, tested::memory_order_acquire);

      completed.fetch_add(1, tested::memory_order_release);
    }};
  }

  value.store(1, tested::memory_order_release);

  value.notify_all();

  for (auto &worker : workers)
    worker.join();

  return completed.load(tested::memory_order_acquire) == 4;
}

bool thread_sleep_works() {
  const auto start = tested::chrono::steady_clock::now();

  tested::this_thread::sleep_for(tested::chrono::milliseconds{2});

  const auto elapsed = tested::chrono::steady_clock::now() - start;

  return elapsed >= tested::chrono::milliseconds{2};
}

bool yield_works() {
  tested::this_thread::yield();
  return true;
}

bool hardware_concurrency_works() {
  const unsigned int count = tested::thread::hardware_concurrency();

  (void)count;
  return true;
}

bool invalid_join_throws() {
#if FTL_HAS_EXCEPTIONS
  tested::thread worker;

  try {
    worker.join();
  } catch (const tested::system_error &error) {
    return error.code() ==
           tested::make_error_code(tested::errc::invalid_argument);
  }

  return false;
#else
  return true;
#endif
}

bool invalid_detach_throws() {
#if FTL_HAS_EXCEPTIONS
  tested::thread worker;

  try {
    worker.detach();
  } catch (const tested::system_error &error) {
    return error.code() ==
           tested::make_error_code(tested::errc::invalid_argument);
  }

  return false;
#else
  return true;
#endif
}

bool default_jthread_works() {
  tested::jthread worker;

  return !worker.joinable() && worker.get_id() == tested::thread::id{} &&
         !worker.get_stop_source().stop_possible() &&
         !worker.get_stop_token().stop_possible() && !worker.request_stop();
}

bool jthread_without_token_works() {
  int value = 0;

  tested::jthread worker{[&value](int input) { value = input; }, 42};

  if (!worker.joinable())
    return false;

  if (!worker.get_stop_token().stop_possible()) {
    return false;
  }

  worker.join();

  return !worker.joinable() && value == 42;
}

bool jthread_token_injection_works() {
  bool received_token = false;

  tested::jthread worker{
      [&received_token](tested::stop_token token, int value) {
        received_token = token.stop_possible() && value == 42;
      },
      42};

  worker.join();

  return received_token;
}

struct dual_jthread_callable {
  int *selected;

  void operator()(int) const noexcept { *selected = 1; }

  void operator()(tested::stop_token token, int) const noexcept {
    *selected = token.stop_possible() ? 2 : 3;
  }
};

bool jthread_prefers_stop_token() {
  int selected = 0;

  tested::jthread worker{dual_jthread_callable{&selected}, 42};

  worker.join();

  return selected == 2;
}

bool jthread_request_stop_works() {
  bool observed = false;

  tested::jthread worker{[&observed](tested::stop_token token) {
    while (!token.stop_requested())
      tested::this_thread::yield();

    observed = true;
  }};

  if (!worker.get_stop_token().stop_possible()) {
    return false;
  }

  if (worker.get_stop_token().stop_requested()) {
    return false;
  }

  if (!worker.request_stop())
    return false;

  if (worker.request_stop())
    return false;

  worker.join();

  return observed;
}

bool jthread_external_source_works() {
  bool observed = false;

  tested::jthread worker{[&observed](tested::stop_token token) {
    while (!token.stop_requested())
      tested::this_thread::yield();

    observed = true;
  }};

  tested::stop_source source = worker.get_stop_source();

  tested::stop_token token = worker.get_stop_token();

  if (!source.stop_possible() || !token.stop_possible()) {
    return false;
  }

  if (!source.request_stop())
    return false;

  worker.join();

  return observed && token.stop_requested() && source.stop_requested();
}

bool jthread_destructor_stops_and_joins() {
  bool observed = false;

  {
    tested::jthread worker{[&observed](tested::stop_token token) {
      while (!token.stop_requested())
        tested::this_thread::yield();

      observed = true;
    }};
  }

  return observed;
}

bool jthread_move_constructor_works() {
  bool observed = false;

  tested::jthread first{[&observed](tested::stop_token token) {
    while (!token.stop_requested())
      tested::this_thread::yield();

    observed = true;
  }};

  const auto original_id = first.get_id();

  const auto original_token = first.get_stop_token();

  tested::jthread second{tested::move(first)};

  if (first.joinable())
    return false;

  if (first.get_stop_source().stop_possible()) {
    return false;
  }

  if (first.get_stop_token().stop_possible()) {
    return false;
  }

  if (!second.joinable())
    return false;

  if (second.get_id() != original_id)
    return false;

  if (!(second.get_stop_token() == original_token)) {
    return false;
  }

  if (!second.request_stop())
    return false;

  second.join();

  return observed;
}

bool jthread_move_assignment_stops_old_thread() {
  bool old_stopped = false;
  int replacement_value = 0;

  tested::jthread destination{[&old_stopped](tested::stop_token token) {
    while (!token.stop_requested())
      tested::this_thread::yield();

    old_stopped = true;
  }};

  tested::jthread source{[&replacement_value] { replacement_value = 42; }};

  const auto source_id = source.get_id();

  destination = tested::move(source);

  if (!old_stopped)
    return false;

  if (source.joinable())
    return false;

  if (source.get_stop_source().stop_possible()) {
    return false;
  }

  if (!destination.joinable())
    return false;

  if (destination.get_id() != source_id) {
    return false;
  }

  destination.join();

  return replacement_value == 42;
}

bool jthread_swap_works() {
  tested::jthread first{[] {}};

  tested::jthread second{[] {}};

  const auto first_id = first.get_id();

  const auto second_id = second.get_id();

  const auto first_token = first.get_stop_token();

  const auto second_token = second.get_stop_token();

  tested::swap(first, second);

  if (first.get_id() != second_id)
    return false;

  if (second.get_id() != first_id)
    return false;

  if (!(first.get_stop_token() == second_token)) {
    return false;
  }

  if (!(second.get_stop_token() == first_token)) {
    return false;
  }

  first.join();
  second.join();

  return true;
}

bool jthread_detach_works() {
  tested::atomic_bool done{false};

  tested::jthread worker{[&done] {
    done.store(true, tested::memory_order_release);

    done.notify_one();
  }};

  worker.detach();

  if (worker.joinable())
    return false;

  done.wait(false, tested::memory_order_acquire);

  return done.load(tested::memory_order_acquire);
}

bool jthread_hardware_concurrency_works() {
  const unsigned int count = tested::jthread::hardware_concurrency();

  (void)count;
  return true;
}

bool jthread_native_handle_works() {
  tested::jthread worker{[] {}};

  const auto handle = worker.native_handle();

  (void)handle;

  worker.join();
  return true;
}

bool ftl_test() {
  return basic_thread_works() && arguments_work() &&
         move_only_argument_works() && thread_ids_work() &&
         move_thread_works() && detach_works() && atomic_wait_widths_work() &&
         atomic_notify_all_works() && thread_sleep_works() && yield_works() &&
         hardware_concurrency_works() && invalid_join_throws() &&
         invalid_detach_throws() && default_jthread_works() &&
         jthread_without_token_works() && jthread_token_injection_works() &&
         jthread_prefers_stop_token() && jthread_request_stop_works() &&
         jthread_external_source_works() &&
         jthread_destructor_stops_and_joins() &&
         jthread_move_constructor_works() &&
         jthread_move_assignment_stops_old_thread() && jthread_swap_works() &&
         jthread_detach_works() && jthread_hardware_concurrency_works() &&
         jthread_native_handle_works();
}
