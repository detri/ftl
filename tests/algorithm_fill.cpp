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

struct temporary_range {
  int values[3] = {};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 3; }
};

template <class Output>
concept ranges_fill_accepts =
    requires(Output first, Output last, const int &value) {
      { tested::ranges::fill(first, last, value) } -> tested::same_as<Output>;

      { tested::ranges::fill_n(first, 2, value) } -> tested::same_as<Output>;
    };

static_assert(ranges_fill_accepts<int *>);
static_assert(!ranges_fill_accepts<const int *>);

template <class Policy>
concept policy_fill_accepts = requires(Policy &&policy, int *first, int *last) {
  tested::fill(tested::forward<Policy>(policy), first, last, 7);

  {
    tested::fill_n(tested::forward<Policy>(policy), first, 2, 7)
  } -> tested::same_as<int *>;
};

static_assert(policy_fill_accepts<tested::execution::sequenced_policy>);

static_assert(policy_fill_accepts<tested::execution::parallel_policy>);

static_assert(
    policy_fill_accepts<tested::execution::parallel_unsequenced_policy>);

static_assert(policy_fill_accepts<tested::execution::unsequenced_policy>);

static_assert(policy_fill_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_fill_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::fill)>);

static_assert(tested::is_object_v<decltype(tested::ranges::fill_n)>);

static_assert(tested::is_same_v<
              decltype(tested::fill_n(tested::declval<int *>(), 3, 7)), int *>);

static_assert(tested::is_same_v<decltype(tested::ranges::fill(
                                    tested::declval<temporary_range>(), 7)),
                                tested::ranges::dangling>);

constexpr bool classic_fill_works() {
  int values[] = {1, 2, 3, 4};

  tested::fill(values, values + 4, 7);

  if (values[0] != 7 || values[1] != 7 || values[2] != 7 || values[3] != 7) {
    return false;
  }

  int counted[] = {1, 2, 3, 4};

  auto final = tested::fill_n(counted, 3, 9);

  if (final != counted + 3) {
    return false;
  }

  if (counted[0] != 9 || counted[1] != 9 || counted[2] != 9 ||
      counted[3] != 4) {
    return false;
  }

  auto zero_final = tested::fill_n(counted, 0, 20);

  return zero_final == counted && counted[0] == 9 && counted[1] == 9 &&
         counted[2] == 9 && counted[3] == 4;
}

static_assert(classic_fill_works());

constexpr bool ranges_fill_works() {
  int values[] = {1, 2, 3, 4};

  auto final = tested::ranges::fill(values, pointer_sentinel{values + 4}, 5);

  if (final != values + 4) {
    return false;
  }

  if (values[0] != 5 || values[1] != 5 || values[2] != 5 || values[3] != 5) {
    return false;
  }

  int range_values[] = {1, 2, 3, 4};

  auto range_final = tested::ranges::fill(range_values, 8);

  if (range_final != range_values + 4) {
    return false;
  }

  if (range_values[0] != 8 || range_values[1] != 8 || range_values[2] != 8 ||
      range_values[3] != 8) {
    return false;
  }

  int counted[] = {1, 2, 3, 4};

  auto counted_final = tested::ranges::fill_n(counted, 3, 6);

  if (counted_final != counted + 3) {
    return false;
  }

  if (counted[0] != 6 || counted[1] != 6 || counted[2] != 6 ||
      counted[3] != 4) {
    return false;
  }

  auto negative_final = tested::ranges::fill_n(counted, -2, 50);

  return negative_final == counted && counted[0] == 6 && counted[1] == 6 &&
         counted[2] == 6 && counted[3] == 4;
}

static_assert(ranges_fill_works());

bool policy_fill_works() {
  int first[4] = {};
  int second[4] = {};
  int third[4] = {};
  int fourth[4] = {};

  tested::fill(tested::execution::seq, first, first + 4, 1);

  tested::fill(tested::execution::par, second, second + 4, 2);

  auto third_end = tested::fill_n(tested::execution::par_unseq, third, 3, 3);

  auto fourth_end = tested::fill_n(tested::execution::unseq, fourth, 4, 4);

  return third_end == third + 3 && fourth_end == fourth + 4 && first[0] == 1 &&
         first[1] == 1 && first[2] == 1 && first[3] == 1 && second[0] == 2 &&
         second[1] == 2 && second[2] == 2 && second[3] == 2 && third[0] == 3 &&
         third[1] == 3 && third[2] == 3 && third[3] == 0 && fourth[0] == 4 &&
         fourth[1] == 4 && fourth[2] == 4 && fourth[3] == 4;
}

bool ftl_test() {
  return classic_fill_works() && ranges_fill_works() && policy_fill_works();
}