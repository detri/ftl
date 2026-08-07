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

#ifndef __cpp_lib_ranges_contains
#error "__cpp_lib_ranges_contains must be defined"
#endif

#ifndef __cpp_lib_ranges_find_last
#error "__cpp_lib_ranges_find_last must be defined"
#endif

static_assert(__cpp_lib_ranges_contains == 202207L);
static_assert(__cpp_lib_ranges_find_last == 202207L);

struct same_parity {
  constexpr bool operator()(int left, int right) const noexcept {
    return left % 2 == right % 2;
  }
};

struct equal_values {
  constexpr bool operator()(int left, int right) const noexcept {
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

struct counting_equal {
  int *calls;

  constexpr bool operator()(int left, int right) const noexcept {
    ++*calls;
    return left == right;
  }
};

struct temporary_range {
  int values[4] = {1, 2, 3, 4};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 4; }
};

struct simple_searcher {
  const int *pattern_first;
  const int *pattern_last;

  constexpr tested::pair<int *, int *> operator()(int *first, int *last) const {
    auto found = tested::search(first, last, pattern_first, pattern_last);

    if (found == last) {
      return {last, last};
    }

    return {
        found,
        found + (pattern_last - pattern_first),
    };
  }
};

template <class Predicate>
concept ranges_search_accepts = requires(int *first1, pointer_sentinel last1,
                                         int *first2, pointer_sentinel last2,
                                         Predicate predicate) {
  tested::ranges::find_end(first1, last1, first2, last2, predicate);

  {
    tested::ranges::find_first_of(first1, last1, first2, last2, predicate)
  } -> tested::same_as<int *>;

  {
    tested::ranges::adjacent_find(first1, last1, predicate)
  } -> tested::same_as<int *>;

  tested::ranges::search(first1, last1, first2, last2, predicate);

  tested::ranges::search_n(first1, last1, 2, 1, predicate);

  {
    tested::ranges::contains_subrange(first1, last1, first2, last2, predicate)
  } -> tested::same_as<bool>;
};

static_assert(ranges_search_accepts<equal_values>);
static_assert(!ranges_search_accepts<void_predicate>);

template <class Policy>
concept policy_search_accepts = requires(Policy &&policy, int *first1,
                                         int *last1, int *first2, int *last2) {
  {
    tested::find_end(tested::forward<Policy>(policy), first1, last1, first2,
                     last2)
  } -> tested::same_as<int *>;

  {
    tested::find_first_of(tested::forward<Policy>(policy), first1, last1,
                          first2, last2)
  } -> tested::same_as<int *>;

  {
    tested::adjacent_find(tested::forward<Policy>(policy), first1, last1)
  } -> tested::same_as<int *>;

  {
    tested::search(tested::forward<Policy>(policy), first1, last1, first2,
                   last2)
  } -> tested::same_as<int *>;

  {
    tested::search_n(tested::forward<Policy>(policy), first1, last1, 2, 1)
  } -> tested::same_as<int *>;
};

static_assert(policy_search_accepts<tested::execution::sequenced_policy>);

static_assert(policy_search_accepts<tested::execution::parallel_policy>);

static_assert(
    policy_search_accepts<tested::execution::parallel_unsequenced_policy>);

static_assert(policy_search_accepts<tested::execution::unsequenced_policy>);

static_assert(
    policy_search_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_search_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::find_end)>);

static_assert(tested::is_object_v<decltype(tested::ranges::find_first_of)>);

static_assert(tested::is_object_v<decltype(tested::ranges::adjacent_find)>);

static_assert(tested::is_object_v<decltype(tested::ranges::search)>);

static_assert(tested::is_object_v<decltype(tested::ranges::search_n)>);

static_assert(tested::is_object_v<decltype(tested::ranges::contains_subrange)>);

static_assert(tested::is_same_v<decltype(tested::ranges::search(
                                    tested::declval<temporary_range>(),
                                    tested::declval<temporary_range>())),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::find_end(
                                    tested::declval<temporary_range>(),
                                    tested::declval<temporary_range>())),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::search_n(
                                    tested::declval<temporary_range>(), 2, 1)),
                                tested::ranges::dangling>);

constexpr bool classic_find_end_works() {
  int values[] = {1, 2, 3, 1, 2, 3, 4};
  int pattern[] = {1, 2, 3};
  int missing[] = {3, 5};
  int empty[1] = {};

  if (tested::find_end(values, values + 7, pattern, pattern + 3) !=
      values + 3) {
    return false;
  }

  if (tested::find_end(values, values + 7, missing, missing + 2) !=
      values + 7) {
    return false;
  }

  if (tested::find_end(values, values + 7, empty, empty) != values + 7) {
    return false;
  }

  int parity_pattern[] = {3, 4, 5};

  return tested::find_end(values, values + 7, parity_pattern,
                          parity_pattern + 3, same_parity{}) == values + 3;
}

static_assert(classic_find_end_works());

constexpr bool classic_find_first_adjacent_work() {
  int values[] = {1, 3, 5, 8, 8, 10};
  int choices[] = {4, 8};

  if (tested::find_first_of(values, values + 6, choices, choices + 2) !=
      values + 3) {
    return false;
  }

  if (tested::adjacent_find(values, values + 6) != values + 3) {
    return false;
  }

  int parity_values[] = {1, 3, 4};

  return tested::adjacent_find(parity_values, parity_values + 3,
                               same_parity{}) == parity_values;
}

static_assert(classic_find_first_adjacent_work());

constexpr bool classic_search_works() {
  int values[] = {1, 2, 3, 1, 2, 4};
  int pattern[] = {1, 2};
  int missing[] = {2, 5};
  int empty[1] = {};

  if (tested::search(values, values + 6, pattern, pattern + 2) != values) {
    return false;
  }

  if (tested::search(values, values + 6, missing, missing + 2) != values + 6) {
    return false;
  }

  if (tested::search(values, values + 6, empty, empty) != values) {
    return false;
  }

  simple_searcher searcher{
      pattern,
      pattern + 2,
  };

  return tested::search(values, values + 6, searcher) == values;
}

static_assert(classic_search_works());

constexpr bool classic_search_n_works() {
  int values[] = {1, 2, 2, 2, 3, 2, 2};

  if (tested::search_n(values, values + 7, 3, 2) != values + 1) {
    return false;
  }

  if (tested::search_n(values, values + 7, 4, 2) != values + 7) {
    return false;
  }

  if (tested::search_n(values, values + 7, 0, 2) != values) {
    return false;
  }

  int parity_values[] = {1, 3, 5, 2};

  return tested::search_n(parity_values, parity_values + 4, 3, 7,
                          same_parity{}) == parity_values;
}

static_assert(classic_search_n_works());

constexpr bool ranges_search_works() {
  int values[] = {1, 2, 3, 1, 2, 3, 4};
  int pattern[] = {1, 2, 3};

  auto first_result =
      tested::ranges::search(values, pointer_sentinel{values + 7}, pattern,
                             pointer_sentinel{pattern + 3});

  if (first_result.begin() != values || first_result.end() != values + 3) {
    return false;
  }

  auto last_result =
      tested::ranges::find_end(values, pointer_sentinel{values + 7}, pattern,
                               pointer_sentinel{pattern + 3});

  if (last_result.begin() != values + 3 || last_result.end() != values + 6) {
    return false;
  }

  int choices[] = {8, 3};

  if (tested::ranges::find_first_of(values, choices) != values + 2) {
    return false;
  }

  int adjacent[] = {1, 2, 2, 3};

  if (tested::ranges::adjacent_find(adjacent) != adjacent + 1) {
    return false;
  }

  int repeated[] = {1, 3, 3, 4};

  auto run = tested::ranges::search_n(repeated, 2, 3);

  return run.begin() == repeated + 1 && run.end() == repeated + 3;

  return run.begin() == values + 2 && run.end() == values + 4;
}

static_assert(ranges_search_works());

constexpr bool ranges_projected_search_works() {
  record values[] = {
      {1, 10}, {2, 20}, {3, 30}, {1, 40}, {2, 50},
  };

  record pattern[] = {
      {1, 100},
      {2, 200},
  };

  int value_projection_calls = 0;
  int pattern_projection_calls = 0;
  int predicate_calls = 0;

  auto result =
      tested::ranges::search(values, pattern, counting_equal{&predicate_calls},
                             counting_projection{&value_projection_calls},
                             counting_projection{&pattern_projection_calls});

  if (result.begin() != values || result.end() != values + 2) {
    return false;
  }

  if (predicate_calls != 2 || value_projection_calls != 2 ||
      pattern_projection_calls != 2) {
    return false;
  }

  auto last_result = tested::ranges::find_end(
      values, pattern, tested::ranges::equal_to{}, &record::key, &record::key);

  if (last_result.begin() != values + 3 || last_result.end() != values + 5) {
    return false;
  }

  record choices[] = {
      {8, 0},
      {3, 0},
  };

  if (tested::ranges::find_first_of(values, choices, tested::ranges::equal_to{},
                                    &record::key, &record::key) != values + 2) {
    return false;
  }

  return tested::ranges::contains_subrange(
      values, pattern, tested::ranges::equal_to{}, &record::key, &record::key);
}

static_assert(ranges_projected_search_works());

constexpr bool contains_subrange_works() {
  int values[] = {1, 2, 3, 4, 5};
  int present[] = {2, 3, 4};
  int missing[] = {2, 4};
  int empty[1] = {};

  return tested::ranges::contains_subrange(values, present) &&
         !tested::ranges::contains_subrange(values, missing) &&
         tested::ranges::contains_subrange(values, values, empty, empty);
}

static_assert(contains_subrange_works());

bool policy_search_works() {
  int values[] = {1, 2, 3, 1, 2, 3, 4};
  int pattern[] = {1, 2, 3};
  int choices[] = {8, 3};
  int adjacent[] = {1, 2, 2, 3};
  int repeated[] = {1, 3, 3, 4};

  return tested::find_end(tested::execution::seq, values, values + 7, pattern,
                          pattern + 3) == values + 3 &&
         tested::find_first_of(tested::execution::par, values, values + 7,
                               choices, choices + 2) == values + 2 &&
         tested::adjacent_find(tested::execution::par_unseq, adjacent,
                               adjacent + 4) == adjacent + 1 &&
         tested::search(tested::execution::unseq, values, values + 7, pattern,
                        pattern + 3) == values &&
         tested::search_n(tested::execution::seq, repeated, repeated + 4, 2,
                          3) == repeated + 1;
}

bool ftl_test() {
  return classic_find_end_works() && classic_find_first_adjacent_work() &&
         classic_search_works() && classic_search_n_works() &&
         ranges_search_works() && ranges_projected_search_works() &&
         contains_subrange_works() && policy_search_works();
}