#ifdef FTL_REPLACE_STL
#include <algorithm>
#include <concepts>
#include <execution>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/algorithm>
#include <ftl/concepts>
#include <ftl/execution>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct even {
  constexpr bool operator()(int value) const noexcept { return value % 2 == 0; }
};

struct positive {
  constexpr bool operator()(int value) const noexcept { return value > 0; }
};

struct less_than_three {
  constexpr bool operator()(int value) const noexcept { return value < 3; }
};

struct void_predicate {
  constexpr void operator()(int) const noexcept {}
};

struct move_only_even {
  constexpr move_only_even() noexcept = default;

  move_only_even(const move_only_even &) = delete;
  move_only_even &operator=(const move_only_even &) = delete;

  constexpr move_only_even(move_only_even &&) noexcept = default;
  constexpr move_only_even &operator=(move_only_even &&) noexcept = default;

  constexpr bool operator()(int value) const noexcept { return value % 2 == 0; }
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
  int key;
  int payload;
};

struct counting_equals {
  int expected;
  int *calls;

  constexpr bool operator()(int value) const noexcept {
    ++*calls;
    return value == expected;
  }
};

struct counting_projection {
  int *calls;

  constexpr int operator()(const record &value) const noexcept {
    ++*calls;
    return value.key;
  }
};

struct not_comparable {};

struct temporary_range {
  int values[3] = {1, 2, 3};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 3; }
};

template <class Predicate>
concept ranges_find_predicate_accepts =
    requires(int *first, pointer_sentinel last, Predicate predicate) {
      {
        tested::ranges::find_if(first, last, predicate)
      } -> tested::same_as<int *>;

      {
        tested::ranges::find_if_not(first, last, predicate)
      } -> tested::same_as<int *>;

      tested::ranges::find_last_if(first, last, predicate);
      tested::ranges::find_last_if_not(first, last, predicate);
    };

static_assert(ranges_find_predicate_accepts<even>);
static_assert(!ranges_find_predicate_accepts<void_predicate>);

template <class T>
concept ranges_find_value_accepts =
    requires(int *first, pointer_sentinel last, const T &value) {
      { tested::ranges::find(first, last, value) } -> tested::same_as<int *>;

      { tested::ranges::contains(first, last, value) } -> tested::same_as<bool>;

      tested::ranges::find_last(first, last, value);
    };

static_assert(ranges_find_value_accepts<int>);
static_assert(!ranges_find_value_accepts<not_comparable>);

template <class Policy>
concept policy_find_accepts = requires(Policy &&policy, int *first, int *last) {
  {
    tested::find(tested::forward<Policy>(policy), first, last, 2)
  } -> tested::same_as<int *>;

  {
    tested::find_if(tested::forward<Policy>(policy), first, last, even{})
  } -> tested::same_as<int *>;

  {
    tested::find_if_not(tested::forward<Policy>(policy), first, last, even{})
  } -> tested::same_as<int *>;
};

static_assert(policy_find_accepts<tested::execution::sequenced_policy>);

static_assert(policy_find_accepts<tested::execution::parallel_policy>);

static_assert(
    policy_find_accepts<tested::execution::parallel_unsequenced_policy>);

static_assert(policy_find_accepts<tested::execution::unsequenced_policy>);

static_assert(policy_find_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_find_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::find)>);
static_assert(tested::is_object_v<decltype(tested::ranges::find_if)>);
static_assert(tested::is_object_v<decltype(tested::ranges::find_if_not)>);
static_assert(tested::is_object_v<decltype(tested::ranges::contains)>);
static_assert(tested::is_object_v<decltype(tested::ranges::find_last)>);
static_assert(tested::is_object_v<decltype(tested::ranges::find_last_if)>);
static_assert(tested::is_object_v<decltype(tested::ranges::find_last_if_not)>);

static_assert(tested::is_same_v<decltype(tested::ranges::find(
                                    tested::declval<temporary_range>(), 2)),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::find_last(
                                    tested::declval<temporary_range>(), 2)),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::find_last(
                                    tested::declval<int (&)[3]>(), 2)),
                                tested::ranges::subrange<int *>>);

constexpr bool classic_find_works() {
  int values[] = {1, 2, 3, 2, 4};

  if (tested::find(values, values + 5, 2) != values + 1) {
    return false;
  }

  if (tested::find(values, values + 5, 9) != values + 5) {
    return false;
  }

  if (tested::find_if(values, values + 5, even{}) != values + 1) {
    return false;
  }

  if (tested::find_if_not(values, values + 5, positive{}) != values + 5) {
    return false;
  }

  if (tested::find_if(values, values + 5, move_only_even{}) != values + 1) {
    return false;
  }

  int calls = 0;

  if (tested::find_if(values, values + 5, counting_equals{3, &calls}) !=
      values + 2) {
    return false;
  }

  return calls == 3;
}

static_assert(classic_find_works());

constexpr bool ranges_find_works() {
  int values[] = {1, 2, 3, 2, 4};
  pointer_sentinel last{values + 5};

  if (tested::ranges::find(values, last, 2) != values + 1) {
    return false;
  }

  if (tested::ranges::find_if(values, last, even{}) != values + 1) {
    return false;
  }

  if (tested::ranges::find_if_not(values, last, less_than_three{}) !=
      values + 2) {
    return false;
  }

  if (!tested::ranges::contains(values, last, 3)) {
    return false;
  }

  if (tested::ranges::contains(values, last, 9)) {
    return false;
  }

  record records[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {2, 40},
  };

  if (tested::ranges::find(records, 2, &record::key) != records + 1) {
    return false;
  }

  if (tested::ranges::find_if(records, even{}, &record::key) != records + 1) {
    return false;
  }

  if (!tested::ranges::contains(records, 3, &record::key)) {
    return false;
  }

  int projection_calls = 0;

  if (tested::ranges::find(
          records, 3, counting_projection{&projection_calls}) != records + 2) {
    return false;
  }

  return projection_calls == 3;
}

static_assert(ranges_find_works());

constexpr bool ranges_find_last_works() {
  int values[] = {1, 2, 3, 2, 4};
  pointer_sentinel last{values + 5};

  auto value_result = tested::ranges::find_last(values, last, 2);

  if (value_result.begin() != values + 3 || value_result.end() != values + 5) {
    return false;
  }

  auto predicate_result = tested::ranges::find_last_if(values, last, even{});

  if (predicate_result.begin() != values + 4 ||
      predicate_result.end() != values + 5) {
    return false;
  }

  auto negative_result =
      tested::ranges::find_last_if_not(values, last, positive{});

  if (negative_result.begin() != values + 5 ||
      negative_result.end() != values + 5) {
    return false;
  }

  auto missing_result = tested::ranges::find_last(values, last, 9);

  if (missing_result.begin() != values + 5 ||
      missing_result.end() != values + 5) {
    return false;
  }

  record records[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {2, 40},
  };

  auto projected_result = tested::ranges::find_last(records, 2, &record::key);

  if (projected_result.begin() != records + 3 ||
      projected_result.end() != records + 4) {
    return false;
  }

  auto projected_predicate_result =
      tested::ranges::find_last_if(records, even{}, &record::key);

  return projected_predicate_result.begin() == records + 3 &&
         projected_predicate_result.end() == records + 4;
}

static_assert(ranges_find_last_works());

bool policy_find_works() {
  int values[] = {1, 2, 3, 2, 4};

  return tested::find(tested::execution::seq, values, values + 5, 2) ==
             values + 1 &&
         tested::find_if(tested::execution::par, values, values + 5, even{}) ==
             values + 1 &&
         tested::find_if_not(tested::execution::par_unseq, values, values + 5,
                             less_than_three{}) == values + 2 &&
         tested::find(tested::execution::unseq, values, values + 5, 9) ==
             values + 5;
}

bool ftl_test() {
  return classic_find_works() && ranges_find_works() &&
         ranges_find_last_works() && policy_find_works();
}