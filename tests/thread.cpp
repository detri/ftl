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

struct wide_atomic_value {
  tested::uint32_t first;
  tested::uint32_t second;
  tested::uint32_t third;
};

class thread_output_buffer : public tested::streambuf {
public:
  tested::string text;
protected:
  int_type overflow(int_type value) override {
    if (traits_type::eq_int_type(value, traits_type::eof()))
      return traits_type::not_eof(value);
    text.push_back(traits_type::to_char_type(value));
    return value;
  }
};

static_assert(
    sizeof(wide_atomic_value) == 12);

static_assert(
    !tested::atomic<
        wide_atomic_value>::
        is_always_lock_free);

static_assert(
    !tested::atomic_ref<
        wide_atomic_value>::
        is_always_lock_free);

template <class T> void wait_until_equal(tested::atomic<T> &value, T expected) {
  T current = value.load(tested::memory_order_acquire);

  while (current != expected) {
    value.wait(current, tested::memory_order_acquire);

    current = value.load(tested::memory_order_acquire);
  }
}

void wait_until_true(tested::atomic_bool &value) {
  while (!value.load(tested::memory_order_acquire)) {
    value.wait(false, tested::memory_order_acquire);
  }
}

struct blocking_stop_callback {
  tested::atomic_bool *entered;
  tested::atomic_bool *release;

  void operator()() noexcept {
    entered->store(true, tested::memory_order_release);
    entered->notify_all();

    while (!release->load(tested::memory_order_acquire)) {
      release->wait(false, tested::memory_order_acquire);
    }
  }
};

struct counting_stop_callback {
  tested::atomic_int *count;

  void operator()() noexcept {
    count->fetch_add(1, tested::memory_order_relaxed);
  }
};

struct self_destroy_callback;
using self_destroy_registration = tested::stop_callback<self_destroy_callback>;

struct self_destroy_callback {
  void **registration;
  tested::atomic_bool *invoked;

  void operator()() noexcept;
};

void destroy_self_registration(void *value) noexcept {
  delete static_cast<self_destroy_registration *>(value);
}

void self_destroy_callback::operator()() noexcept {
  invoked->store(true, tested::memory_order_release);

  void *current = *registration;
  *registration = nullptr;

  destroy_self_registration(current);

  // Do not touch any members after this point.
}

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

bool atomic_notify_without_change_does_not_return() {
  tested::atomic_int value{0};
  tested::atomic_bool ready{false};
  tested::atomic_bool completed{false};

  tested::thread waiter{[&] {
    ready.store(true, tested::memory_order_release);
    ready.notify_all();

    value.wait(0, tested::memory_order_acquire);

    completed.store(true, tested::memory_order_release);
    completed.notify_all();
  }};

  wait_until_true(ready);

  // Give the waiter a chance to actually enter
  // the platform blocking path.
  tested::this_thread::sleep_for(tested::chrono::milliseconds{2});

  value.notify_all();

  tested::this_thread::sleep_for(tested::chrono::milliseconds{2});

  if (completed.load(tested::memory_order_acquire)) {
    value.store(1, tested::memory_order_release);
    value.notify_all();
    waiter.join();
    return false;
  }

  value.store(1, tested::memory_order_release);
  value.notify_all();

  waiter.join();

  return completed.load(tested::memory_order_acquire);
}

template <class Atomic, class T>
bool blocking_wait_width_works(T initial, T changed) {
  Atomic value{initial};

  tested::atomic_bool ready{false};
  tested::atomic_bool completed{false};

  tested::thread waiter{[&] {
    ready.store(true, tested::memory_order_release);
    ready.notify_all();

    value.wait(initial, tested::memory_order_acquire);

    completed.store(value.load(tested::memory_order_acquire) == changed,
                    tested::memory_order_release);
  }};

  wait_until_true(ready);

  tested::this_thread::sleep_for(tested::chrono::milliseconds{2});

  if (completed.load(tested::memory_order_acquire)) {
    waiter.join();
    return false;
  }

  value.store(changed, tested::memory_order_release);

  value.notify_one();

  waiter.join();

  return completed.load(tested::memory_order_acquire);
}

bool atomic_blocking_widths_work() {
  return blocking_wait_width_works<tested::atomic_uint8_t>(
             tested::uint8_t{0}, tested::uint8_t{1}) &&

         blocking_wait_width_works<tested::atomic_uint16_t>(
             tested::uint16_t{0}, tested::uint16_t{1}) &&

         blocking_wait_width_works<tested::atomic_uint32_t>(
             tested::uint32_t{0}, tested::uint32_t{1}) &&

         blocking_wait_width_works<tested::atomic_uint64_t>(
             tested::uint64_t{0}, tested::uint64_t{1});
}

bool atomic_notify_all_blocked_waiters_work() {
  constexpr int worker_count = 8;

  tested::atomic_int value{0};
  tested::atomic_int ready{0};
  tested::atomic_int completed{0};

  tested::thread workers[worker_count];

  for (int index = 0; index < worker_count; ++index) {
    workers[index] = tested::thread{[&] {
      const int count = ready.fetch_add(1, tested::memory_order_release) + 1;

      ready.notify_all();

      value.wait(0, tested::memory_order_acquire);

      completed.fetch_add(1, tested::memory_order_release);

      completed.notify_all();
    }};
  }

  wait_until_equal(ready, worker_count);

  tested::this_thread::sleep_for(tested::chrono::milliseconds{2});

  if (completed.load(tested::memory_order_acquire) != 0) {
    value.store(1);
    value.notify_all();

    for (auto &worker : workers)
      worker.join();

    return false;
  }

  value.store(1, tested::memory_order_release);

  value.notify_all();

  for (auto &worker : workers)
    worker.join();

  return completed.load(tested::memory_order_acquire) == worker_count;
}

bool atomic_ref_wait_notify_works() {
  alignas(tested::atomic_ref<int>::required_alignment) int value = 0;

  tested::atomic_ref<int> waiter_ref{value};

  tested::atomic_ref<int> notifier_ref{value};

  tested::atomic_bool ready{false};
  tested::atomic_bool completed{false};

  tested::thread waiter{[&] {
    ready.store(true, tested::memory_order_release);
    ready.notify_all();

    waiter_ref.wait(0, tested::memory_order_acquire);

    completed.store(waiter_ref.load(tested::memory_order_acquire) == 42,
                    tested::memory_order_release);
  }};

  wait_until_true(ready);

  tested::this_thread::sleep_for(tested::chrono::milliseconds{2});

  notifier_ref.store(42, tested::memory_order_release);

  notifier_ref.notify_one();

  waiter.join();

  return completed.load(tested::memory_order_acquire);
}

bool atomic_flag_blocking_wait_works() {
  tested::atomic_flag flag{};
  tested::atomic_bool ready{false};
  tested::atomic_bool completed{false};

  tested::thread waiter{[&] {
    ready.store(true, tested::memory_order_release);
    ready.notify_all();

    flag.wait(false, tested::memory_order_acquire);

    completed.store(flag.test(tested::memory_order_acquire),
                    tested::memory_order_release);
  }};

  wait_until_true(ready);

  tested::this_thread::sleep_for(tested::chrono::milliseconds{2});

  flag.test_and_set(tested::memory_order_release);

  flag.notify_one();

  waiter.join();

  return completed.load(tested::memory_order_acquire);
}

bool atomic_proxy_collision_notify_one_works() {
#if defined(_WIN32)
  return true;
#else
  alignas(16) tested::uint8_t storage[16]{};

  if (tested::detail::wait_bucket_for(&storage[0]) !=
      tested::detail::wait_bucket_for(&storage[1])) {
    return false;
  }

  tested::atomic_ref<tested::uint8_t> first{storage[0]};

  tested::atomic_ref<tested::uint8_t> second{storage[1]};

  tested::atomic_int ready{0};

  tested::atomic_bool first_completed{false};

  tested::atomic_bool second_completed{false};

  tested::thread first_waiter{[&] {
    ready.fetch_add(1, tested::memory_order_release);
    ready.notify_all();

    first.wait(tested::uint8_t{0}, tested::memory_order_acquire);

    first_completed.store(true, tested::memory_order_release);
  }};

  tested::thread second_waiter{[&] {
    ready.fetch_add(1, tested::memory_order_release);
    ready.notify_all();

    second.wait(tested::uint8_t{0}, tested::memory_order_acquire);

    second_completed.store(true, tested::memory_order_release);
  }};

  wait_until_equal(ready, 2);

  tested::this_thread::sleep_for(tested::chrono::milliseconds{2});

  first.store(tested::uint8_t{1}, tested::memory_order_release);

  first.notify_one();

  first_waiter.join();

  if (!first_completed.load(tested::memory_order_acquire)) {
    second.store(1);
    second.notify_all();
    second_waiter.join();
    return false;
  }

  // second shares the proxy bucket and may have
  // been physically woken, but wait() must have
  // rechecked its own object and gone back to sleep.
  tested::this_thread::sleep_for(tested::chrono::milliseconds{2});

  if (second_completed.load(tested::memory_order_acquire)) {
    second_waiter.join();
    return false;
  }

  second.store(tested::uint8_t{1}, tested::memory_order_release);

  second.notify_one();

  second_waiter.join();

  return second_completed.load(tested::memory_order_acquire);
#endif
}

bool stop_callback_destructor_waits_for_execution() {
  tested::stop_source source;

  tested::stop_token token = source.get_token();

  tested::atomic_bool entered{false};
  tested::atomic_bool release{false};

  tested::atomic_bool destructor_returned{false};

  using registration = tested::stop_callback<blocking_stop_callback>;

  auto *callback =
      new registration{token, blocking_stop_callback{&entered, &release}};

  tested::thread requester{[&] { source.request_stop(); }};

  wait_until_true(entered);

  tested::thread destroyer{[&] {
    delete callback;

    destructor_returned.store(true, tested::memory_order_release);

    destructor_returned.notify_all();
  }};

  // The callback is still executing, therefore
  // its destructor must not have returned.
  tested::this_thread::sleep_for(tested::chrono::milliseconds{2});

  const bool returned_early =
      destructor_returned.load(tested::memory_order_acquire);

  release.store(true, tested::memory_order_release);

  release.notify_all();

  requester.join();
  destroyer.join();

  return !returned_early &&
         destructor_returned.load(tested::memory_order_acquire);
}

bool stop_callback_destructor_ignores_unrelated_execution() {
  tested::stop_source source;

  tested::stop_token token = source.get_token();

  tested::atomic_bool entered{false};
  tested::atomic_bool release{false};

  tested::atomic_int target_count{0};

  using target_registration = tested::stop_callback<counting_stop_callback>;

  auto *target =
      new target_registration{token, counting_stop_callback{&target_count}};

  // Registered second => list head => executes first.
  tested::stop_callback blocker{token,
                                blocking_stop_callback{&entered, &release}};

  tested::thread requester{[&] { source.request_stop(); }};

  wait_until_true(entered);

  // request_stop() is currently blocked inside
  // blocker. Destroying target must merely unlink
  // target; it must not wait for blocker.
  delete target;

  if (target_count.load(tested::memory_order_relaxed) != 0) {
    release.store(true);
    release.notify_all();
    requester.join();
    return false;
  }

  release.store(true, tested::memory_order_release);
  release.notify_all();

  requester.join();

  return target_count.load(tested::memory_order_relaxed) == 0;
}

bool stop_callback_can_destroy_itself() {
  tested::stop_source source;

  tested::stop_token token = source.get_token();

  tested::atomic_bool invoked{false};

  void *registration = nullptr;

  registration = new self_destroy_registration{
      token, self_destroy_callback{&registration, &invoked}};

  if (!source.request_stop())
    return false;

  return registration == nullptr && invoked.load(tested::memory_order_acquire);
}

bool stop_callback_registration_request_race() {
  constexpr int iterations = 256;

  for (int iteration = 0; iteration < iterations; ++iteration) {
    tested::stop_source source;

    tested::stop_token token = source.get_token();

    tested::atomic_bool go{false};
    tested::atomic_bool constructed{false};
    tested::atomic_bool release{false};

    tested::atomic_int calls{0};

    tested::thread registrar{[&] {
      go.wait(false, tested::memory_order_acquire);

      tested::stop_callback callback{
          token,
          [&] noexcept { calls.fetch_add(1, tested::memory_order_relaxed); }};

      constructed.store(true, tested::memory_order_release);

      constructed.notify_all();

      release.wait(false, tested::memory_order_acquire);
    }};

    go.store(true, tested::memory_order_release);
    go.notify_one();

    source.request_stop();

    wait_until_true(constructed);

    release.store(true, tested::memory_order_release);
    release.notify_one();

    registrar.join();

    if (calls.load(tested::memory_order_relaxed) != 1) {
      return false;
    }
  }

  return true;
}

bool atomic_shared_ptr_wait_notify_works() {
  auto initial = tested::make_shared<int>(1);

  auto replacement = tested::make_shared<int>(2);

  tested::atomic<tested::shared_ptr<int>> value{initial};

  tested::atomic_bool ready{false};
  tested::atomic_bool completed{false};

  tested::thread waiter{[&] {
    ready.store(true, tested::memory_order_release);

    ready.notify_all();

    value.wait(initial, tested::memory_order_acquire);

    auto observed = value.load(tested::memory_order_acquire);

    completed.store(observed.get() == replacement.get(),
                    tested::memory_order_release);
  }};

  wait_until_true(ready);

  tested::this_thread::sleep_for(tested::chrono::milliseconds{2});

  if (completed.load(tested::memory_order_acquire)) {
    waiter.join();
    return false;
  }

  value.store(replacement, tested::memory_order_release);

  value.notify_one();

  waiter.join();

  return completed.load(tested::memory_order_acquire);
}

bool non_lock_free_atomic_wait_works() {
  const wide_atomic_value initial{
      1, 2, 3};

  const wide_atomic_value changed{
      4, 5, 6};

  tested::atomic<
      wide_atomic_value>
      value{initial};

  tested::atomic_bool ready{
      false};

  tested::atomic_bool completed{
      false};

  tested::thread waiter{[&] {
    ready.store(
        true,
        tested::memory_order_release);

    ready.notify_all();

    value.wait(
        initial,
        tested::memory_order_acquire);

    const auto observed =
        value.load(
            tested::memory_order_acquire);

    completed.store(
        observed.first == 4 &&
            observed.second == 5 &&
            observed.third == 6,
        tested::memory_order_release);
  }};

  wait_until_true(ready);

  tested::this_thread::sleep_for(
      tested::chrono::milliseconds{
          2});

  if (completed.load(
          tested::memory_order_acquire)) {
    waiter.join();
    return false;
  }

  value.store(
      changed,
      tested::memory_order_release);

  value.notify_one();

  waiter.join();

  return completed.load(
      tested::memory_order_acquire);
}

bool non_lock_free_atomic_ref_wait_works() {
  const wide_atomic_value initial{
      1, 2, 3};

  const wide_atomic_value changed{
      4, 5, 6};

  alignas(
      tested::atomic_ref<
          wide_atomic_value
      >::required_alignment)
  wide_atomic_value storage =
      initial;

  tested::atomic_ref<
      wide_atomic_value>
      waiter_ref{storage};

  tested::atomic_ref<
      wide_atomic_value>
      writer_ref{storage};

  tested::atomic_bool ready{
      false};

  tested::atomic_bool completed{
      false};

  tested::thread waiter{[&] {
    ready.store(
        true,
        tested::memory_order_release);

    ready.notify_all();

    waiter_ref.wait(
        initial,
        tested::memory_order_acquire);

    const auto observed =
        waiter_ref.load(
            tested::memory_order_acquire);

    completed.store(
        observed.first == 4 &&
            observed.second == 5 &&
            observed.third == 6,
        tested::memory_order_release);
  }};

  wait_until_true(ready);

  tested::this_thread::sleep_for(
      tested::chrono::milliseconds{
          2});

  if (completed.load(
          tested::memory_order_acquire)) {
    waiter.join();
    return false;
  }

  writer_ref.store(
      changed,
      tested::memory_order_release);

  // Important: notify through a DIFFERENT
  // atomic_ref object referencing the same storage.
  writer_ref.notify_one();

  waiter.join();

  return completed.load(
      tested::memory_order_acquire);
}

bool thread_id_stream_and_format_work() {
  const tested::thread::id id = tested::this_thread::get_id();
  thread_output_buffer buffer;
  tested::ostream stream(&buffer);
  stream << id;
  const tested::string formatted = tested::format("{}", id);
  return !buffer.text.empty() && buffer.text == formatted &&
         tested::format("{:>20}", id).size() >= 20;
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
         jthread_native_handle_works() &&
         atomic_notify_without_change_does_not_return() &&
         atomic_blocking_widths_work() &&
         atomic_notify_all_blocked_waiters_work() &&
         atomic_ref_wait_notify_works() && atomic_flag_blocking_wait_works() &&
         atomic_proxy_collision_notify_one_works() &&
         stop_callback_destructor_waits_for_execution() &&
         stop_callback_destructor_ignores_unrelated_execution() &&
         stop_callback_can_destroy_itself() &&
         stop_callback_registration_request_race() &&
         atomic_shared_ptr_wait_notify_works() &&
         non_lock_free_atomic_wait_works() && non_lock_free_atomic_ref_wait_works() &&
         thread_id_stream_and_format_work();
}
