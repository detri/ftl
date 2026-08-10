#ifdef FTL_REPLACE_STL
#include <future>
#else
#include <ftl/future>
#endif

#ifdef FTL_REPLACE_STL
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/atomic>
#include <ftl/chrono>
#include <ftl/memory>
#include <ftl/thread>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

static_assert(static_cast<int>(tested::future_errc::broken_promise) != 0);

static_assert(static_cast<int>(tested::future_errc::future_already_retrieved) !=
              0);

static_assert(
    static_cast<int>(tested::future_errc::promise_already_satisfied) != 0);

static_assert(static_cast<int>(tested::future_errc::no_state) != 0);

static_assert(static_cast<int>(tested::future_errc::broken_promise) !=
              static_cast<int>(tested::future_errc::future_already_retrieved));

static_assert(tested::is_error_code_enum_v<tested::future_errc>);

static_assert(tested::is_base_of_v<tested::logic_error, tested::future_error>);

static_assert(((tested::launch::async | tested::launch::deferred) &
               tested::launch::async) == tested::launch::async);

static_assert(((tested::launch::async | tested::launch::deferred) &
               tested::launch::deferred) == tested::launch::deferred);

static_assert(tested::is_default_constructible_v<tested::future<int>>);

static_assert(tested::is_move_constructible_v<tested::future<int>>);

static_assert(tested::is_move_assignable_v<tested::future<int>>);

static_assert(!tested::is_copy_constructible_v<tested::future<int>>);

static_assert(!tested::is_copy_assignable_v<tested::future<int>>);

static_assert(tested::is_copy_constructible_v<tested::shared_future<int>>);

static_assert(tested::is_copy_assignable_v<tested::shared_future<int>>);

static_assert(tested::is_move_constructible_v<tested::shared_future<int>>);

static_assert(tested::is_move_assignable_v<tested::shared_future<int>>);

static_assert(tested::is_same_v<
              decltype(tested::declval<tested::future<int> &>().get()), int>);

static_assert(
    tested::is_same_v<
        decltype(tested::declval<tested::future<int &> &>().get()), int &>);

static_assert(
    tested::is_same_v<
        decltype(tested::declval<tested::shared_future<int> const &>().get()),
        const int &>);

static_assert(
    tested::is_same_v<
        decltype(tested::declval<tested::shared_future<int &> const &>().get()),
        int &>);

static_assert(noexcept(tested::declval<tested::future<int> &>().share()));

bool strings_equal(const char *left, const char *right) {
  while (*left != '\0' && *right != '\0') {
    if (*left != *right)
      return false;

    ++left;
    ++right;
  }

  return *left == *right;
}

bool error_vocabulary_works() {
  const tested::error_code broken =
      tested::make_error_code(tested::future_errc::broken_promise);

  if (broken.value() != static_cast<int>(tested::future_errc::broken_promise)) {
    return false;
  }

  if (&broken.category() != &tested::future_category()) {
    return false;
  }

  if (!strings_equal(broken.category().name(), "future")) {
    return false;
  }

  tested::future_error error{tested::future_errc::broken_promise};

  if (error.code() != broken)
    return false;

  const auto message = error.code().message();

  return strings_equal(error.what(), message.c_str());
}

bool value_state_works() {
  tested::detail::future_state<int> state;

  if (state.is_ready())
    return false;

  if (!state.try_set_value(42))
    return false;

  if (!state.is_ready())
    return false;

  state.wait();

  return state.value() == 42;
}

bool reference_state_works() {
  int value = 17;

  tested::detail::future_state<int &> state;

  if (!state.try_set_value(value))
    return false;

  state.wait();

  return &state.value() == &value;
}

bool void_state_works() {
  tested::detail::future_state<void> state;

  if (!state.try_set_value())
    return false;

  state.wait();

  return state.is_ready();
}

bool duplicate_satisfaction_is_rejected() {
  tested::detail::future_state<int> state;

  if (!state.try_set_value(1))
    return false;

  return !state.try_set_value(2);
}

bool future_retrieval_is_single_use() {
  tested::detail::future_state<int> state;

  if (!state.try_mark_future_retrieved())
    return false;

  return !state.try_mark_future_retrieved();
}

bool stored_value_is_not_ready_until_published() {
  tested::detail::future_state<int> state;

  if (!state.try_store_value(73))
    return false;

  if (state.is_ready())
    return false;

  if (state.wait_for(0) != tested::detail::future_wait_result::timeout) {
    return false;
  }

  state.make_ready();

  if (state.wait_for(0) != tested::detail::future_wait_result::ready) {
    return false;
  }

  return state.value() == 73;
}

bool publication_synchronizes_waiters() {
  tested::detail::future_state<int> state;

  int side_effect = 0;

  tested::thread producer([&] {
    side_effect = 91;

    (void)state.try_set_value(42);
  });

  state.wait();

  const bool result = state.value() == 42 && side_effect == 91;

  producer.join();

  return result;
}

bool multiple_waiters_are_released() {
  tested::detail::future_state<int> state;

  tested::atomic<int> waiting{0};
  tested::atomic<int> observed{0};

  tested::thread first([&] {
    waiting.fetch_add(1, tested::memory_order_release);

    state.wait();

    if (state.value() == 99) {
      observed.fetch_add(1, tested::memory_order_relaxed);
    }
  });

  tested::thread second([&] {
    waiting.fetch_add(1, tested::memory_order_release);

    state.wait();

    if (state.value() == 99) {
      observed.fetch_add(1, tested::memory_order_relaxed);
    }
  });

  while (waiting.load(tested::memory_order_acquire) != 2) {
    tested::this_thread::yield();
  }

  if (!state.try_set_value(99)) {
    first.join();
    second.join();
    return false;
  }

  first.join();
  second.join();

  return observed.load(tested::memory_order_relaxed) == 2;
}

struct construction_failure {
  explicit construction_failure(int input) : value(input) {
    if (input < 0)
      throw input;
  }

  int value;
};

bool failed_construction_does_not_satisfy_state() {
  tested::detail::future_state<construction_failure> state;

  bool threw = false;

  try {
    (void)state.try_emplace_value(-1);
  } catch (int value) {
    threw = value == -1;
  }

  if (!threw)
    return false;

  if (state.is_satisfied())
    return false;

  if (!state.try_emplace_value(123))
    return false;

  state.wait();

  return state.value().value == 123;
}

struct stored_exception {};

bool exception_state_works() {
  tested::detail::future_state<int> state;

  if (!state.try_set_exception(
          tested::make_exception_ptr(stored_exception{}))) {
    return false;
  }

  try {
    state.rethrow_if_exception();
  } catch (const stored_exception &) {
    return true;
  } catch (...) {
    return false;
  }

  return false;
}

bool broken_promise_state_works() {
  tested::detail::future_state<void> state;

  state.abandon_with_exception(tested::make_exception_ptr(
      tested::future_error{tested::future_errc::broken_promise}));

  try {
    state.rethrow_if_exception();
  } catch (const tested::future_error &error) {
    return error.code() ==
           tested::make_error_code(tested::future_errc::broken_promise);
  } catch (...) {
    return false;
  }

  return false;
}

bool future_move_and_validity_work() {
  auto state = tested::make_shared<tested::detail::future_state<int>>();

  auto first = tested::detail::future_access::make_future<int>(state);

  if (!first.valid())
    return false;

  tested::future<int> second{tested::move(first)};

  if (first.valid() || !second.valid())
    return false;

  tested::future<int> third;

  third = tested::move(second);

  return !second.valid() && third.valid();
}

bool future_get_consumes_value() {
  auto state = tested::make_shared<tested::detail::future_state<int>>();

  if (!state->try_set_value(42))
    return false;

  auto value = tested::detail::future_access::make_future<int>(state);

  if (value.get() != 42)
    return false;

  return !value.valid();
}

bool future_get_rethrows_and_invalidates() {
  auto state = tested::make_shared<tested::detail::future_state<int>>();

  if (!state->try_set_exception(
          tested::make_exception_ptr(stored_exception{}))) {
    return false;
  }

  auto value = tested::detail::future_access::make_future<int>(state);

  try {
    (void)value.get();
  } catch (const stored_exception &) {
    return !value.valid();
  } catch (...) {
    return false;
  }

  return false;
}

bool future_reference_get_preserves_identity() {
  int target = 81;

  auto state = tested::make_shared<tested::detail::future_state<int &>>();

  if (!state->try_set_value(target))
    return false;

  auto value = tested::detail::future_access::make_future<int &>(state);

  int &result = value.get();

  return &result == &target && !value.valid();
}

bool future_void_get_works() {
  auto state = tested::make_shared<tested::detail::future_state<void>>();

  if (!state->try_set_value())
    return false;

  auto value = tested::detail::future_access::make_future<void>(state);

  value.get();

  return !value.valid();
}

bool future_wait_surface_works() {
  auto state = tested::make_shared<tested::detail::future_state<int>>();

  auto value = tested::detail::future_access::make_future<int>(state);

  tested::thread producer([state] { (void)state->try_set_value(101); });

  value.wait();

  const bool still_valid = value.valid();

  const bool correct = value.get() == 101;

  producer.join();

  return still_valid && correct && !value.valid();
}

bool future_timed_waits_work() {
  auto state = tested::make_shared<tested::detail::future_state<int>>();

  auto value = tested::detail::future_access::make_future<int>(state);

  if (value.wait_for(tested::chrono::milliseconds{1}) !=
      tested::future_status::timeout) {
    return false;
  }

  if (value.wait_until(tested::chrono::steady_clock::now()) !=
      tested::future_status::timeout) {
    return false;
  }

  if (!state->try_set_value(33))
    return false;

  if (value.wait_for(tested::chrono::hours{1}) !=
      tested::future_status::ready) {
    return false;
  }

  return value.wait_until(tested::chrono::steady_clock::now()) ==
         tested::future_status::ready;
}

bool invalid_future_reports_no_state() {
  tested::future<int> value;

  try {
    value.wait();
  } catch (const tested::future_error &error) {
    if (error.code() !=
        tested::make_error_code(tested::future_errc::no_state)) {
      return false;
    }
  } catch (...) {
    return false;
  }

  tested::shared_future<int> shared;

  try {
    (void)shared.get();
  } catch (const tested::future_error &error) {
    return error.code() ==
           tested::make_error_code(tested::future_errc::no_state);
  } catch (...) {
    return false;
  }

  return false;
}

bool share_transfers_state() {
  auto state = tested::make_shared<tested::detail::future_state<int>>();

  if (!state->try_set_value(67))
    return false;

  auto value = tested::detail::future_access::make_future<int>(state);

  tested::shared_future<int> shared = value.share();

  return !value.valid() && shared.valid() && shared.get() == 67;
}

bool shared_future_copy_and_repeated_get_work() {
  auto state = tested::make_shared<tested::detail::future_state<int>>();

  if (!state->try_set_value(123))
    return false;

  auto value = tested::detail::future_access::make_future<int>(state);

  tested::shared_future<int> first = value.share();

  tested::shared_future<int> second = first;

  const int &a = first.get();
  const int &b = second.get();
  const int &c = first.get();

  return first.valid() && second.valid() && &a == &b && &a == &c && a == 123;
}

bool shared_future_reference_and_void_work() {
  int target = 45;

  auto reference_state =
      tested::make_shared<tested::detail::future_state<int &>>();

  if (!reference_state->try_set_value(target))
    return false;

  auto reference_future =
      tested::detail::future_access::make_future<int &>(reference_state);

  auto reference_shared = reference_future.share();

  if (&reference_shared.get() != &target)
    return false;

  if (&reference_shared.get() != &target)
    return false;

  auto void_state = tested::make_shared<tested::detail::future_state<void>>();

  if (!void_state->try_set_value())
    return false;

  auto void_future =
      tested::detail::future_access::make_future<void>(void_state);

  auto void_shared = void_future.share();

  void_shared.get();
  void_shared.get();

  return void_shared.valid();
}

bool shared_future_rethrows_repeatedly() {
  auto state = tested::make_shared<tested::detail::future_state<int>>();

  if (!state->try_set_exception(
          tested::make_exception_ptr(stored_exception{}))) {
    return false;
  }

  auto value = tested::detail::future_access::make_future<int>(state);

  auto shared = value.share();

  int catches = 0;

  for (int attempt = 0; attempt < 2; ++attempt) {
    try {
      (void)shared.get();
    } catch (const stored_exception &) {
      ++catches;
    } catch (...) {
      return false;
    }
  }

  return catches == 2 && shared.valid();
}

bool shared_future_waiters_share_one_state() {
  auto state = tested::make_shared<tested::detail::future_state<int>>();

  auto value = tested::detail::future_access::make_future<int>(state);

  auto first = value.share();
  auto second = first;

  tested::atomic<int> waiting{0};
  tested::atomic<int> observed{0};

  tested::thread first_waiter([first, &waiting, &observed] {
    waiting.fetch_add(1, tested::memory_order_release);

    if (first.get() == 777) {
      observed.fetch_add(1, tested::memory_order_relaxed);
    }
  });

  tested::thread second_waiter([second, &waiting, &observed] {
    waiting.fetch_add(1, tested::memory_order_release);

    if (second.get() == 777) {
      observed.fetch_add(1, tested::memory_order_relaxed);
    }
  });

  while (waiting.load(tested::memory_order_acquire) != 2) {
    tested::this_thread::yield();
  }

  if (!state->try_set_value(777)) {
    first_waiter.join();
    second_waiter.join();
    return false;
  }

  first_waiter.join();
  second_waiter.join();

  return observed.load(tested::memory_order_relaxed) == 2;
}

struct move_only_future_value {
  explicit move_only_future_value(int input) : value(input) {}

  move_only_future_value(const move_only_future_value &) = delete;

  move_only_future_value &operator=(const move_only_future_value &) = delete;

  move_only_future_value(move_only_future_value &&) noexcept = default;

  move_only_future_value &
  operator=(move_only_future_value &&) noexcept = default;

  int value;
};

bool future_moves_stored_value() {
  auto state = tested::make_shared<
      tested::detail::future_state<move_only_future_value>>();

  if (!state->try_emplace_value(909))
    return false;

  auto value =
      tested::detail::future_access::make_future<move_only_future_value>(state);

  move_only_future_value result = value.get();

  return result.value == 909 && !value.valid();
}

struct throwing_future_move {
  explicit throwing_future_move(int input) : value(input) {}

  throwing_future_move(const throwing_future_move &) = delete;

  throwing_future_move(throwing_future_move &&other) : value(other.value) {
    throw 812;
  }

  int value;
};

bool throwing_get_move_still_invalidates_future() {
  auto state =
      tested::make_shared<tested::detail::future_state<throwing_future_move>>();

  if (!state->try_emplace_value(11))
    return false;

  auto value =
      tested::detail::future_access::make_future<throwing_future_move>(state);

  try {
    (void)value.get();
  } catch (int error) {
    return error == 812 && !value.valid();
  } catch (...) {
    return false;
  }

  return false;
}

class deferred_test_state final : public tested::detail::future_state<int> {
public:
  [[nodiscard]]
  int execution_count() const noexcept {
    return executions_.load(tested::memory_order_acquire);
  }

protected:
  bool is_deferred() const noexcept override {
    return !started_.load(tested::memory_order_acquire);
  }

  void execute_deferred() noexcept override {
    bool expected = false;

    if (!started_.compare_exchange_strong(expected, true,
                                          tested::memory_order_acq_rel,
                                          tested::memory_order_acquire)) {
      return;
    }

    executions_.fetch_add(1, tested::memory_order_relaxed);

    (void)try_set_value(314);
  }

private:
  tested::atomic<bool> started_{false};
  tested::atomic<int> executions_{0};
};

bool timed_waits_preserve_deferred_status() {
  auto deferred = tested::make_shared<deferred_test_state>();

  tested::shared_ptr<tested::detail::future_state<int>> state = deferred;

  auto value =
      tested::detail::future_access::make_future<int>(tested::move(state));

  if (value.wait_for(tested::chrono::hours{1}) !=
      tested::future_status::deferred) {
    return false;
  }

  if (value.wait_until(tested::chrono::steady_clock::now() +
                       tested::chrono::hours{1}) !=
      tested::future_status::deferred) {
    return false;
  }

  if (deferred->execution_count() != 0)
    return false;

  value.wait();

  if (deferred->execution_count() != 1)
    return false;

  if (value.wait_for(tested::chrono::milliseconds{0}) !=
      tested::future_status::ready) {
    return false;
  }

  return value.get() == 314;
}

bool ftl_test() {
  return error_vocabulary_works() && value_state_works() &&
         reference_state_works() && void_state_works() &&
         duplicate_satisfaction_is_rejected() &&
         future_retrieval_is_single_use() &&
         stored_value_is_not_ready_until_published() &&
         publication_synchronizes_waiters() &&
         multiple_waiters_are_released() &&
         failed_construction_does_not_satisfy_state() &&
         exception_state_works() && broken_promise_state_works() &&
         future_move_and_validity_work() && future_get_consumes_value() &&
         future_get_rethrows_and_invalidates() &&
         future_reference_get_preserves_identity() && future_void_get_works() &&
         future_wait_surface_works() && future_timed_waits_work() &&
         invalid_future_reports_no_state() && share_transfers_state() &&
         shared_future_copy_and_repeated_get_work() &&
         shared_future_reference_and_void_work() &&
         shared_future_rethrows_repeatedly() &&
         shared_future_waiters_share_one_state() &&
         future_moves_stored_value() &&
         throwing_get_move_still_invalidates_future() &&
         timed_waits_preserve_deferred_status();
}
