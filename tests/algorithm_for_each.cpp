#ifdef FTL_REPLACE_STL
#include <algorithm>
#include <concepts>
#include <execution>
#include <iterator>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/algorithm>
#include <ftl/concepts>
#include <ftl/execution>
#include <ftl/iterator>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct sum_values {
  int sum = 0;

  constexpr void operator()(int value) noexcept { sum += value; }
};

struct move_only_sum {
  int sum = 0;

  constexpr move_only_sum() noexcept = default;

  move_only_sum(const move_only_sum &) = delete;
  move_only_sum &operator=(const move_only_sum &) = delete;

  constexpr move_only_sum(move_only_sum &&) noexcept = default;
  constexpr move_only_sum &operator=(move_only_sum &&) noexcept = default;

  constexpr void operator()(int value) noexcept { sum += value; }
};

struct increment {
  constexpr void operator()(int &value) const noexcept { ++value; }
};

struct wrong_function {
  constexpr void operator()(const char *) const noexcept {}
};

struct size_like {
  int value;

  constexpr operator int() const noexcept { return value; }
};

struct pointer_sentinel {
  int *last = nullptr;

  friend constexpr bool operator==(int *iterator,
                                   pointer_sentinel sentinel) noexcept {
    return iterator == sentinel.last;
  }

  friend constexpr bool operator==(pointer_sentinel sentinel,
                                   int *iterator) noexcept {
    return iterator == sentinel.last;
  }

  friend constexpr bool operator!=(int *iterator,
                                   pointer_sentinel sentinel) noexcept {
    return !(iterator == sentinel);
  }

  friend constexpr bool operator!=(pointer_sentinel sentinel,
                                   int *iterator) noexcept {
    return !(sentinel == iterator);
  }
};

static_assert(tested::sentinel_for<pointer_sentinel, int *>);

struct record {
  int value;
};

struct count_calls {
  int calls = 0;

  constexpr void operator()(int) noexcept { ++calls; }
};

struct counting_projection {
  int *calls;

  constexpr int &operator()(record &value) const noexcept {
    ++*calls;
    return value.value;
  }
};

struct temporary_range {
  int values[3] = {1, 2, 3};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 3; }
};

template <class Function>
concept ranges_for_each_accepts =
    requires(int *first, pointer_sentinel last, Function function) {
      tested::ranges::for_each(first, last, function);
      tested::ranges::for_each_n(first, 2, function);
    };

static_assert(ranges_for_each_accepts<sum_values>);
static_assert(!ranges_for_each_accepts<wrong_function>);
static_assert(!ranges_for_each_accepts<move_only_sum>);

template <class Policy>
concept policy_for_each_accepts = requires(Policy &&policy, int *first,
                                           int *last) {
  tested::for_each(tested::forward<Policy>(policy), first, last, increment{});

  {
    tested::for_each_n(tested::forward<Policy>(policy), first, 2, increment{})
  } -> tested::same_as<int *>;
};

static_assert(policy_for_each_accepts<tested::execution::sequenced_policy>);

static_assert(policy_for_each_accepts<tested::execution::parallel_policy>);

static_assert(
    policy_for_each_accepts<tested::execution::parallel_unsequenced_policy>);

static_assert(policy_for_each_accepts<tested::execution::unsequenced_policy>);

static_assert(
    policy_for_each_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_for_each_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::for_each)>);

static_assert(tested::is_object_v<decltype(tested::ranges::for_each_n)>);

static_assert(
    tested::is_same_v<tested::ranges::for_each_result<int *, sum_values>,
                      tested::ranges::in_fun_result<int *, sum_values>>);

static_assert(
    tested::is_same_v<tested::ranges::for_each_n_result<int *, sum_values>,
                      tested::ranges::in_fun_result<int *, sum_values>>);

static_assert(
    tested::is_same_v<decltype(tested::for_each(tested::declval<int *>(),
                                                tested::declval<int *>(),
                                                tested::declval<sum_values>())),
                      sum_values>);

static_assert(
    tested::is_same_v<decltype(tested::for_each(
                          tested::execution::seq, tested::declval<int *>(),
                          tested::declval<int *>(), increment{})),
                      void>);

static_assert(tested::is_same_v<
              decltype(tested::for_each_n(tested::declval<int *>(), 2,
                                          tested::declval<sum_values>())),
              int *>);

static_assert(
    tested::is_same_v<
        decltype(tested::ranges::for_each(tested::declval<temporary_range>(),
                                          tested::declval<sum_values>())),
        tested::ranges::for_each_result<tested::ranges::dangling, sum_values>>);

constexpr bool classic_for_each_works() {
  int values[] = {1, 2, 3, 4};

  auto result = tested::for_each(values, values + 4, move_only_sum{});

  if (result.sum != 10) {
    return false;
  }

  tested::for_each(values, values + 4, increment{});

  if (values[0] != 2 || values[1] != 3 || values[2] != 4 || values[3] != 5) {
    return false;
  }

  auto final = tested::for_each_n(values, size_like{2}, increment{});

  return final == values + 2 && values[0] == 3 && values[1] == 4 &&
         values[2] == 4 && values[3] == 5;
}

static_assert(classic_for_each_works());

constexpr bool ranges_for_each_works() {
  int values[] = {1, 2, 3, 4};

  auto result = tested::ranges::for_each(values, pointer_sentinel{values + 4},
                                         sum_values{});

  if (result.in != values + 4 || result.fun.sum != 10) {
    return false;
  }

  auto zero_result = tested::ranges::for_each_n(values, 0, count_calls{});

  return zero_result.in == values && zero_result.fun.calls == 0;
}

static_assert(ranges_for_each_works());

bool policy_for_each_works() {
  int values[] = {1, 2, 3, 4};

  tested::for_each(tested::execution::seq, values, values + 4, increment{});

  if (values[0] != 2 || values[1] != 3 || values[2] != 4 || values[3] != 5) {
    return false;
  }

  auto final =
      tested::for_each_n(tested::execution::par, values, 2, increment{});

  if (final != values + 2) {
    return false;
  }

  if (values[0] != 3 || values[1] != 4 || values[2] != 4 || values[3] != 5) {
    return false;
  }

  tested::for_each(tested::execution::par_unseq, values, values, increment{});

  return tested::for_each_n(tested::execution::unseq, values, 0, increment{}) ==
         values;
}

bool ranges_for_each_returns_function_state() {
  int values[] = {1, 2, 3, 4};

  auto result = tested::ranges::for_each(values, pointer_sentinel{values + 4},
                                         sum_values{});

  return result.in == values + 4 && result.fun.sum == 10;
}

bool ranges_projected_for_each_works() {
  record records[] = {{1}, {2}, {3}, {4}};

  auto first_result =
      tested::ranges::for_each(records, increment{}, &record::value);

  if (first_result.in != records + 4 || records[0].value != 2 ||
      records[1].value != 3 || records[2].value != 4 || records[3].value != 5) {
    return false;
  }

  int projection_calls = 0;

  auto second_result = tested::ranges::for_each_n(
      records, 2, increment{}, counting_projection{&projection_calls});

  if (second_result.in != records + 2 || projection_calls != 2) {
    return false;
  }

  return records[0].value == 3 && records[1].value == 4 &&
         records[2].value == 4 && records[3].value == 5;
}

bool ftl_test() {
  return classic_for_each_works() && ranges_for_each_works() &&
         policy_for_each_works() && ranges_for_each_returns_function_state() &&
         ranges_projected_for_each_works();
}