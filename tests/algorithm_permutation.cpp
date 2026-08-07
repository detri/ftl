#ifdef FTL_REPLACE_STL
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <execution>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/algorithm>
#include <ftl/concepts>
#include <ftl/cstddef>
#include <ftl/execution>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct absolute_equal {
  constexpr bool operator()(int left, int right) const noexcept {
    if (left < 0) {
      left = -left;
    }

    if (right < 0) {
      right = -right;
    }

    return left == right;
  }
};

struct descending {
  constexpr bool operator()(int left, int right) const noexcept {
    return left > right;
  }
};

struct void_predicate {
  constexpr void operator()(int, int) const noexcept {}
};

struct record {
  int key = 0;
  int sequence = 0;
};

struct key_projection {
  constexpr int operator()(const record &value) const noexcept {
    return value.key;
  }
};

template <class T> struct pointer_sentinel {
  T *last = nullptr;

  friend constexpr bool operator==(T *iterator,
                                   pointer_sentinel sentinel) noexcept {
    return iterator == sentinel.last;
  }

  friend constexpr bool operator==(pointer_sentinel sentinel,
                                   T *iterator) noexcept {
    return iterator == sentinel.last;
  }
};

static_assert(tested::sentinel_for<pointer_sentinel<int>, int *>);

struct temporary_range {
  int values[3] = {1, 2, 3};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 3; }
};

template <class Predicate>
concept ranges_is_permutation_accepts =
    requires(int *first, int *last, Predicate predicate) {
      {
        tested::ranges::is_permutation(first, last, first, last, predicate)
      } -> tested::same_as<bool>;
    };

static_assert(ranges_is_permutation_accepts<absolute_equal>);

static_assert(!ranges_is_permutation_accepts<void_predicate>);

template <class Policy>
concept policy_permutation_accepts = requires(Policy &&policy, int *first,
                                              int *last) {
  {
    tested::is_permutation(tested::forward<Policy>(policy), first, last, first)
  } -> tested::same_as<bool>;

  {
    tested::is_permutation(tested::forward<Policy>(policy), first, last, first,
                           last)
  } -> tested::same_as<bool>;

  {
    tested::next_permutation(tested::forward<Policy>(policy), first, last)
  } -> tested::same_as<bool>;

  {
    tested::prev_permutation(tested::forward<Policy>(policy), first, last)
  } -> tested::same_as<bool>;
};

static_assert(policy_permutation_accepts<tested::execution::sequenced_policy>);

static_assert(policy_permutation_accepts<tested::execution::parallel_policy>);

static_assert(
    policy_permutation_accepts<tested::execution::parallel_unsequenced_policy>);

static_assert(
    policy_permutation_accepts<tested::execution::unsequenced_policy>);

static_assert(!policy_permutation_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::is_permutation)>);

static_assert(tested::is_object_v<decltype(tested::ranges::next_permutation)>);

static_assert(tested::is_object_v<decltype(tested::ranges::prev_permutation)>);

static_assert(tested::is_same_v<tested::ranges::next_permutation_result<int *>,
                                tested::ranges::in_found_result<int *>>);

static_assert(tested::is_same_v<tested::ranges::prev_permutation_result<int *>,
                                tested::ranges::in_found_result<int *>>);

static_assert(
    tested::is_same_v<
        decltype(tested::ranges::next_permutation(
            tested::declval<temporary_range>())),
        tested::ranges::next_permutation_result<tested::ranges::dangling>>);

static_assert(
    tested::is_same_v<
        decltype(tested::ranges::prev_permutation(
            tested::declval<temporary_range>())),
        tested::ranges::prev_permutation_result<tested::ranges::dangling>>);

constexpr bool classic_is_permutation_works() {
  int first[] = {
      1, 2, 2, 3, 4,
  };

  int second[] = {
      2, 4, 2, 1, 3,
  };

  int wrong[] = {
      2, 4, 2, 1, 1,
  };

  if (!tested::is_permutation(first, first + 5, second)) {
    return false;
  }

  if (!tested::is_permutation(first, first + 5, second, second + 5)) {
    return false;
  }

  if (tested::is_permutation(first, first + 5, wrong, wrong + 5)) {
    return false;
  }

  if (tested::is_permutation(first, first + 5, second, second + 4)) {
    return false;
  }

  int signed_values[] = {
      -1,
      2,
      -3,
  };

  int positive_values[] = {
      3,
      1,
      2,
  };

  return tested::is_permutation(signed_values, signed_values + 3,
                                positive_values, absolute_equal{});
}

static_assert(classic_is_permutation_works());

constexpr bool ranges_is_permutation_works() {
  int first[] = {
      1, 2, 2, 3, 4,
  };

  int second[] = {
      2, 4, 2, 1, 3,
  };

  if (!tested::ranges::is_permutation(first, pointer_sentinel<int>{first + 5},
                                      second,
                                      pointer_sentinel<int>{second + 5})) {
    return false;
  }

  int shorter[] = {
      1,
      2,
      2,
      3,
  };

  if (tested::ranges::is_permutation(first, shorter)) {
    return false;
  }

  record left[] = {
      {1, 0},
      {2, 1},
      {2, 2},
      {3, 3},
  };

  record right[] = {
      {2, 4},
      {3, 5},
      {1, 6},
      {2, 7},
  };

  return tested::ranges::is_permutation(left, right, tested::ranges::equal_to{},
                                        key_projection{}, key_projection{});
}

static_assert(ranges_is_permutation_works());

constexpr bool next_permutation_works() {
  int values[] = {1, 2, 3};

  if (!tested::next_permutation(values, values + 3)) {
    return false;
  }

  if (values[0] != 1 || values[1] != 3 || values[2] != 2) {
    return false;
  }

  int final[] = {3, 2, 1};

  if (tested::next_permutation(final, final + 3)) {
    return false;
  }

  if (final[0] != 1 || final[1] != 2 || final[2] != 3) {
    return false;
  }

  int duplicates[] = {1, 1, 2};

  if (!tested::next_permutation(duplicates, duplicates + 3)) {
    return false;
  }

  return duplicates[0] == 1 && duplicates[1] == 2 && duplicates[2] == 1;
}

static_assert(next_permutation_works());

constexpr bool prev_permutation_works() {
  int values[] = {1, 3, 2};

  if (!tested::prev_permutation(values, values + 3)) {
    return false;
  }

  if (values[0] != 1 || values[1] != 2 || values[2] != 3) {
    return false;
  }

  int first[] = {1, 2, 3};

  if (tested::prev_permutation(first, first + 3)) {
    return false;
  }

  return first[0] == 3 && first[1] == 2 && first[2] == 1;
}

static_assert(prev_permutation_works());

constexpr bool ranges_permutations_work() {
  int next_values[] = {
      1,
      2,
      3,
  };

  auto next_result = tested::ranges::next_permutation(
      next_values, pointer_sentinel<int>{next_values + 3});

  if (next_result.in != next_values + 3 || !next_result.found ||
      next_values[0] != 1 || next_values[1] != 3 || next_values[2] != 2) {
    return false;
  }

  int previous_values[] = {
      1,
      3,
      2,
  };

  auto previous_result = tested::ranges::prev_permutation(previous_values);

  return previous_result.in == previous_values + 3 && previous_result.found &&
         previous_values[0] == 1 && previous_values[1] == 2 &&
         previous_values[2] == 3;
}

static_assert(ranges_permutations_work());

constexpr bool projection_works() {
  record values[] = {
      {1, 0},
      {2, 1},
      {3, 2},
  };

  auto result = tested::ranges::next_permutation(values, tested::ranges::less{},
                                                 key_projection{});

  if (!result.found || result.in != values + 3) {
    return false;
  }

  return values[0].key == 1 && values[1].key == 3 && values[2].key == 2;
}

static_assert(projection_works());

constexpr bool descending_works() {
  int values[] = {3, 2, 1};

  if (!tested::next_permutation(values, values + 3, descending{})) {
    return false;
  }

  return values[0] == 3 && values[1] == 1 && values[2] == 2;
}

static_assert(descending_works());

bool policy_works() {
  int first[] = {
      1,
      2,
      2,
      3,
  };

  int second[] = {
      2,
      3,
      1,
      2,
  };

  if (!tested::is_permutation(tested::execution::seq, first, first + 4,
                              second)) {
    return false;
  }

  if (!tested::is_permutation(tested::execution::par, first, first + 4, second,
                              second + 4)) {
    return false;
  }

  int next_values[] = {
      1,
      2,
      3,
  };

  if (!tested::next_permutation(tested::execution::par_unseq, next_values,
                                next_values + 3)) {
    return false;
  }

  int previous_values[] = {
      1,
      3,
      2,
  };

  return tested::prev_permutation(tested::execution::unseq, previous_values,
                                  previous_values + 3);
}

bool all_permutations_work() {
  int values[] = {1, 2, 3};

  int count = 1;

  while (tested::next_permutation(values, values + 3)) {
    ++count;
  }

  return count == 6 && values[0] == 1 && values[1] == 2 && values[2] == 3;
}

bool ftl_test() {
  return classic_is_permutation_works() && ranges_is_permutation_works() &&
         next_permutation_works() && prev_permutation_works() &&
         ranges_permutations_work() && projection_works() &&
         descending_works() && policy_works() && all_permutations_work();
}
