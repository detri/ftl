#ifdef FTL_REPLACE_STL
#include <future>
#else
#include <ftl/future>
#endif

#ifdef FTL_REPLACE_STL
#include <atomic>
#include <thread>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/atomic>
#include <ftl/thread>
#include <ftl/type_traits>
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

bool ftl_test() {
  return error_vocabulary_works() && value_state_works() &&
         reference_state_works() && void_state_works() &&
         duplicate_satisfaction_is_rejected() &&
         future_retrieval_is_single_use() &&
         stored_value_is_not_ready_until_published() &&
         publication_synchronizes_waiters() &&
         multiple_waiters_are_released() &&
         failed_construction_does_not_satisfy_state() &&
         exception_state_works() && broken_promise_state_works();
}
