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

#ifndef __cpp_lib_ranges_starts_ends_with
#error "__cpp_lib_ranges_starts_ends_with must be defined"
#endif

static_assert(__cpp_lib_ranges_starts_ends_with == 202106L);

struct same_parity {
  constexpr bool operator()(int left, int right) const noexcept {
    return left % 2 == right % 2;
  }
};

struct counting_equal {
  int *calls;

  constexpr bool operator()(int left, int right) const noexcept {
    ++*calls;
    return left == right;
  }
};

struct void_predicate {
  constexpr void operator()(int, int) const noexcept {}
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
static_assert(!tested::sized_sentinel_for<pointer_sentinel, int *>);

struct record {
  int key;
  int payload;
};

struct counting_projection {
  int *calls;

  constexpr int operator()(const record &value) const noexcept {
    ++*calls;
    return value.key;
  }
};

struct temporary_range {
  int values[3] = {1, 2, 3};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 3; }
};

template <class Predicate>
concept ranges_compare_accepts =
    requires(int *first1, pointer_sentinel last1, int *first2,
             pointer_sentinel last2, Predicate predicate) {
      tested::ranges::mismatch(first1, last1, first2, last2, predicate);

      {
        tested::ranges::equal(first1, last1, first2, last2, predicate)
      } -> tested::same_as<bool>;

      {
        tested::ranges::starts_with(first1, last1, first2, last2, predicate)
      } -> tested::same_as<bool>;

      {
        tested::ranges::ends_with(first1, last1, first2, last2, predicate)
      } -> tested::same_as<bool>;
    };

static_assert(ranges_compare_accepts<same_parity>);
static_assert(!ranges_compare_accepts<void_predicate>);

template <class Policy>
concept policy_compare_accepts = requires(Policy &&policy, int *first1,
                                          int *last1, int *first2, int *last2) {
  {
    tested::mismatch(tested::forward<Policy>(policy), first1, last1, first2)
  } -> tested::same_as<tested::pair<int *, int *>>;

  {
    tested::mismatch(tested::forward<Policy>(policy), first1, last1, first2,
                     last2)
  } -> tested::same_as<tested::pair<int *, int *>>;

  {
    tested::equal(tested::forward<Policy>(policy), first1, last1, first2)
  } -> tested::same_as<bool>;

  {
    tested::equal(tested::forward<Policy>(policy), first1, last1, first2, last2)
  } -> tested::same_as<bool>;
};

static_assert(policy_compare_accepts<tested::execution::sequenced_policy>);

static_assert(policy_compare_accepts<tested::execution::parallel_policy>);

static_assert(
    policy_compare_accepts<tested::execution::parallel_unsequenced_policy>);

static_assert(policy_compare_accepts<tested::execution::unsequenced_policy>);

static_assert(
    policy_compare_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_compare_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::mismatch)>);

static_assert(tested::is_object_v<decltype(tested::ranges::equal)>);

static_assert(tested::is_object_v<decltype(tested::ranges::starts_with)>);

static_assert(tested::is_object_v<decltype(tested::ranges::ends_with)>);

static_assert(
    tested::is_same_v<tested::ranges::mismatch_result<int *, const int *>,
                      tested::ranges::in_in_result<int *, const int *>>);

static_assert(
    tested::is_same_v<
        decltype(tested::ranges::mismatch(tested::declval<temporary_range>(),
                                          tested::declval<temporary_range>())),
        tested::ranges::mismatch_result<tested::ranges::dangling,
                                        tested::ranges::dangling>>);

constexpr bool classic_mismatch_works() {
  int left[] = {1, 2, 3, 4};
  int right[] = {1, 2, 9, 4};
  int short_right[] = {1, 2};

  auto result = tested::mismatch(left, left + 4, right);

  if (result.first != left + 2 || result.second != right + 2) {
    return false;
  }

  auto predicate_result =
      tested::mismatch(left, left + 4, right, same_parity{});

  if (predicate_result.first != left + 4 ||
      predicate_result.second != right + 4) {
    return false;
  }

  auto bounded_result =
      tested::mismatch(left, left + 4, short_right, short_right + 2);

  return bounded_result.first == left + 2 &&
         bounded_result.second == short_right + 2;
}

static_assert(classic_mismatch_works());

constexpr bool classic_equal_works() {
  int values[] = {1, 2, 3, 4};
  int same[] = {1, 2, 3, 4};
  int parity_equivalent[] = {3, 4, 5, 6};
  int shorter[] = {1, 2};

  if (!tested::equal(values, values + 4, same)) {
    return false;
  }

  if (!tested::equal(values, values + 4, same, same + 4)) {
    return false;
  }

  if (tested::equal(values, values + 4, shorter, shorter + 2)) {
    return false;
  }

  if (!tested::equal(values, values + 4, parity_equivalent,
                     parity_equivalent + 4, same_parity{})) {
    return false;
  }

  int calls = 0;

  if (tested::equal(values, values + 4, shorter, shorter + 2,
                    counting_equal{&calls})) {
    return false;
  }

  return calls == 0;
}

static_assert(classic_equal_works());

constexpr bool ranges_mismatch_works() {
  int left[] = {1, 2, 3, 4};
  int right[] = {1, 2, 9, 4};

  auto result = tested::ranges::mismatch(left, pointer_sentinel{left + 4},
                                         right, pointer_sentinel{right + 4});

  if (result.in1 != left + 2 || result.in2 != right + 2) {
    return false;
  }

  record records1[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {4, 40},
  };

  record records2[] = {
      {1, 100},
      {2, 200},
      {9, 300},
      {4, 400},
  };

  int projection1_calls = 0;
  int projection2_calls = 0;

  auto projected_result =
      tested::ranges::mismatch(records1, records2, tested::ranges::equal_to{},
                               counting_projection{&projection1_calls},
                               counting_projection{&projection2_calls});

  return projected_result.in1 == records1 + 2 &&
         projected_result.in2 == records2 + 2 && projection1_calls == 3 &&
         projection2_calls == 3;
}

static_assert(ranges_mismatch_works());

constexpr bool ranges_equal_works() {
  int values[] = {1, 2, 3, 4};
  int same[] = {1, 2, 3, 4};
  int different[] = {1, 2, 9, 4};
  int shorter[] = {1, 2};

  if (!tested::ranges::equal(values, same)) {
    return false;
  }

  if (tested::ranges::equal(values, different)) {
    return false;
  }

  if (tested::ranges::equal(values, shorter)) {
    return false;
  }

  int calls = 0;

  if (tested::ranges::equal(values, values + 4, shorter, shorter + 2,
                            counting_equal{&calls})) {
    return false;
  }

  return calls == 0;
}

static_assert(ranges_equal_works());

constexpr bool starts_ends_with_work() {
  int values[] = {1, 2, 3, 4, 5};
  int prefix[] = {1, 2, 3};
  int wrong_prefix[] = {1, 2, 4};
  int suffix[] = {4, 5};
  int wrong_suffix[] = {3, 5};
  int longer[] = {1, 2, 3, 4, 5, 6};
  int empty[1] = {};

  if (!tested::ranges::starts_with(values, prefix)) {
    return false;
  }

  if (tested::ranges::starts_with(values, wrong_prefix)) {
    return false;
  }

  if (tested::ranges::starts_with(values, longer)) {
    return false;
  }

  if (!tested::ranges::starts_with(values, values, empty, empty)) {
    return false;
  }

  if (!tested::ranges::ends_with(values, suffix)) {
    return false;
  }

  if (tested::ranges::ends_with(values, wrong_suffix)) {
    return false;
  }

  if (tested::ranges::ends_with(values, longer)) {
    return false;
  }

  if (!tested::ranges::ends_with(values, values, empty, empty)) {
    return false;
  }

  if (!tested::ranges::ends_with(values, pointer_sentinel{values + 5}, suffix,
                                 pointer_sentinel{suffix + 2})) {
    return false;
  }

  record records[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {4, 40},
  };

  record record_prefix[] = {
      {1, 100},
      {2, 200},
  };

  record record_suffix[] = {
      {3, 300},
      {4, 400},
  };

  return tested::ranges::starts_with(records, record_prefix,
                                     tested::ranges::equal_to{}, &record::key,
                                     &record::key) &&
         tested::ranges::ends_with(records, record_suffix,
                                   tested::ranges::equal_to{}, &record::key,
                                   &record::key);
}

static_assert(starts_ends_with_work());

bool policy_compare_works() {
  int left[] = {1, 2, 3, 4};
  int right[] = {1, 2, 9, 4};
  int same[] = {1, 2, 3, 4};

  auto first_result =
      tested::mismatch(tested::execution::seq, left, left + 4, right);

  if (first_result.first != left + 2 || first_result.second != right + 2) {
    return false;
  }

  auto second_result = tested::mismatch(tested::execution::par, left, left + 4,
                                        right, right + 4);

  if (second_result.first != left + 2 || second_result.second != right + 2) {
    return false;
  }

  return tested::equal(tested::execution::par_unseq, left, left + 4, same) &&
         !tested::equal(tested::execution::unseq, left, left + 4, right,
                        right + 4);
}

bool ftl_test() {
  return classic_mismatch_works() && classic_equal_works() &&
         ranges_mismatch_works() && ranges_equal_works() &&
         starts_ends_with_work() && policy_compare_works();
}