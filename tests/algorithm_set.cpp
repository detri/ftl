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

struct descending {
  constexpr bool operator()(int left, int right) const noexcept {
    return left > right;
  }
};

struct void_comparator {
  constexpr void operator()(int, int) const noexcept {}
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

  friend constexpr bool operator!=(T *iterator,
                                   pointer_sentinel sentinel) noexcept {
    return !(iterator == sentinel);
  }

  friend constexpr bool operator!=(pointer_sentinel sentinel,
                                   T *iterator) noexcept {
    return !(sentinel == iterator);
  }
};

static_assert(tested::sentinel_for<pointer_sentinel<int>, int *>);

struct record {
  int key = 0;
  int sequence = 0;
};

struct key_projection {
  constexpr int operator()(const record &value) const noexcept {
    return value.key;
  }
};

struct counting_projection {
  int *calls;

  constexpr int operator()(const record &value) const noexcept {
    ++*calls;
    return value.key;
  }
};

struct counting_less {
  int *calls;

  constexpr bool operator()(int left, int right) const noexcept {
    ++*calls;
    return left < right;
  }
};

struct temporary_range {
  int values[3] = {1, 2, 3};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 3; }
};

template <class Comparator>
concept ranges_set_operations_accepts = requires(
    int *first1, int *last1, int *first2, int *last2, int *output,
    Comparator comparator) {
  {
    tested::ranges::includes(first1, last1, first2, last2, comparator)
  } -> tested::same_as<bool>;

  tested::ranges::set_union(first1, last1, first2, last2, output, comparator);

  tested::ranges::set_intersection(first1, last1, first2, last2, output,
                                   comparator);

  tested::ranges::set_difference(first1, last1, first2, last2, output,
                                 comparator);

  tested::ranges::set_symmetric_difference(first1, last1, first2, last2, output,
                                           comparator);
};

static_assert(ranges_set_operations_accepts<descending>);

static_assert(!ranges_set_operations_accepts<void_comparator>);

template <class Policy>
concept policy_set_operations_accepts =
    requires(Policy &&policy, int *first1, int *last1, int *first2, int *last2,
             int *output) {
      {
        tested::includes(tested::forward<Policy>(policy), first1, last1, first2,
                         last2)
      } -> tested::same_as<bool>;

      {
        tested::set_union(tested::forward<Policy>(policy), first1, last1,
                          first2, last2, output)
      } -> tested::same_as<int *>;

      {
        tested::set_intersection(tested::forward<Policy>(policy), first1, last1,
                                 first2, last2, output)
      } -> tested::same_as<int *>;

      {
        tested::set_difference(tested::forward<Policy>(policy), first1, last1,
                               first2, last2, output)
      } -> tested::same_as<int *>;

      {
        tested::set_symmetric_difference(tested::forward<Policy>(policy),
                                         first1, last1, first2, last2, output)
      } -> tested::same_as<int *>;
    };

static_assert(
    policy_set_operations_accepts<tested::execution::sequenced_policy>);

static_assert(
    policy_set_operations_accepts<tested::execution::parallel_policy>);

static_assert(policy_set_operations_accepts<
              tested::execution::parallel_unsequenced_policy>);

static_assert(
    policy_set_operations_accepts<tested::execution::unsequenced_policy>);

static_assert(
    policy_set_operations_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_set_operations_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::includes)>);

static_assert(tested::is_object_v<decltype(tested::ranges::set_union)>);

static_assert(tested::is_object_v<decltype(tested::ranges::set_intersection)>);

static_assert(tested::is_object_v<decltype(tested::ranges::set_difference)>);

static_assert(
    tested::is_object_v<decltype(tested::ranges::set_symmetric_difference)>);

static_assert(tested::is_same_v<
              tested::ranges::set_union_result<int *, const int *, int *>,
              tested::ranges::in_in_out_result<int *, const int *, int *>>);

static_assert(
    tested::is_same_v<
        tested::ranges::set_intersection_result<int *, const int *, int *>,
        tested::ranges::in_in_out_result<int *, const int *, int *>>);

static_assert(
    tested::is_same_v<tested::ranges::set_difference_result<int *, int *>,
                      tested::ranges::in_out_result<int *, int *>>);

static_assert(tested::is_same_v<
              tested::ranges::set_symmetric_difference_result<
                  int *, const int *, int *>,
              tested::ranges::in_in_out_result<int *, const int *, int *>>);

static_assert(
    tested::is_same_v<
        decltype(tested::ranges::set_union(tested::declval<temporary_range>(),
                                           tested::declval<temporary_range>(),
                                           tested::declval<int *>())),
        tested::ranges::set_union_result<tested::ranges::dangling,
                                         tested::ranges::dangling, int *>>);

static_assert(
    tested::is_same_v<
        decltype(tested::ranges::set_intersection(
            tested::declval<temporary_range>(),
            tested::declval<temporary_range>(), tested::declval<int *>())),
        tested::ranges::set_intersection_result<
            tested::ranges::dangling, tested::ranges::dangling, int *>>);

static_assert(tested::is_same_v<decltype(tested::ranges::set_difference(
                                    tested::declval<temporary_range>(),
                                    tested::declval<temporary_range>(),
                                    tested::declval<int *>())),
                                tested::ranges::set_difference_result<
                                    tested::ranges::dangling, int *>>);

static_assert(
    tested::is_same_v<
        decltype(tested::ranges::set_symmetric_difference(
            tested::declval<temporary_range>(),
            tested::declval<temporary_range>(), tested::declval<int *>())),
        tested::ranges::set_symmetric_difference_result<
            tested::ranges::dangling, tested::ranges::dangling, int *>>);

constexpr bool includes_works() {
  int superset[] = {
      1, 1, 2, 2, 2, 3, 4,
  };

  int subset[] = {
      1,
      2,
      2,
      4,
  };

  int too_many[] = {
      1,
      1,
      1,
  };

  if (!tested::includes(superset, superset + 7, subset, subset + 4)) {
    return false;
  }

  if (tested::includes(superset, superset + 7, too_many, too_many + 3)) {
    return false;
  }

  if (!tested::ranges::includes(superset, pointer_sentinel<int>{superset + 7},
                                subset, pointer_sentinel<int>{subset + 4})) {
    return false;
  }

  int descending_superset[] = {
      5, 4, 3, 2, 2, 1,
  };

  int descending_subset[] = {
      4,
      2,
      1,
  };

  return tested::includes(descending_superset, descending_superset + 6,
                          descending_subset, descending_subset + 3,
                          descending{}) &&
         tested::ranges::includes(descending_superset, descending_subset,
                                  descending{});
}

static_assert(includes_works());

constexpr bool classic_set_operations_work() {
  int left[] = {
      1, 1, 2, 2, 2, 4,
  };

  int right[] = {
      1, 2, 2, 3, 4, 4,
  };

  int union_output[12] = {};

  auto union_end =
      tested::set_union(left, left + 6, right, right + 6, union_output);

  if (union_end != union_output + 8 || union_output[0] != 1 ||
      union_output[1] != 1 || union_output[2] != 2 || union_output[3] != 2 ||
      union_output[4] != 2 || union_output[5] != 3 || union_output[6] != 4 ||
      union_output[7] != 4) {
    return false;
  }

  int intersection_output[12] = {};

  auto intersection_end = tested::set_intersection(
      left, left + 6, right, right + 6, intersection_output);

  if (intersection_end != intersection_output + 4 ||
      intersection_output[0] != 1 || intersection_output[1] != 2 ||
      intersection_output[2] != 2 || intersection_output[3] != 4) {
    return false;
  }

  int difference_output[12] = {};

  auto difference_end = tested::set_difference(left, left + 6, right, right + 6,
                                               difference_output);

  if (difference_end != difference_output + 2 || difference_output[0] != 1 ||
      difference_output[1] != 2) {
    return false;
  }

  int symmetric_output[12] = {};

  auto symmetric_end = tested::set_symmetric_difference(
      left, left + 6, right, right + 6, symmetric_output);

  return symmetric_end == symmetric_output + 4 && symmetric_output[0] == 1 &&
         symmetric_output[1] == 2 && symmetric_output[2] == 3 &&
         symmetric_output[3] == 4;
}

static_assert(classic_set_operations_work());

constexpr bool ranges_set_operations_work() {
  int left[] = {
      1, 1, 2, 2, 2, 4,
  };

  int right[] = {
      1, 2, 2, 3, 4, 4,
  };

  int union_output[12] = {};

  auto union_result =
      tested::ranges::set_union(left, pointer_sentinel<int>{left + 6}, right,
                                pointer_sentinel<int>{right + 6}, union_output);

  if (union_result.in1 != left + 6 || union_result.in2 != right + 6 ||
      union_result.out != union_output + 8) {
    return false;
  }

  int intersection_output[12] = {};

  auto intersection_result =
      tested::ranges::set_intersection(left, right, intersection_output);

  if (intersection_result.in1 != left + 6 ||
      intersection_result.in2 != right + 6 ||
      intersection_result.out != intersection_output + 4) {
    return false;
  }

  int difference_output[12] = {};

  auto difference_result =
      tested::ranges::set_difference(left, right, difference_output);

  if (difference_result.in != left + 6 ||
      difference_result.out != difference_output + 2) {
    return false;
  }

  int symmetric_output[12] = {};

  auto symmetric_result =
      tested::ranges::set_symmetric_difference(left, right, symmetric_output);

  return symmetric_result.in1 == left + 6 &&
         symmetric_result.in2 == right + 6 &&
         symmetric_result.out == symmetric_output + 4 && union_output[0] == 1 &&
         union_output[1] == 1 && union_output[2] == 2 && union_output[3] == 2 &&
         union_output[4] == 2 && union_output[5] == 3 && union_output[6] == 4 &&
         union_output[7] == 4 && intersection_output[0] == 1 &&
         intersection_output[1] == 2 && intersection_output[2] == 2 &&
         intersection_output[3] == 4 && difference_output[0] == 1 &&
         difference_output[1] == 2 && symmetric_output[0] == 1 &&
         symmetric_output[1] == 2 && symmetric_output[2] == 3 &&
         symmetric_output[3] == 4;
}

static_assert(ranges_set_operations_work());

constexpr bool projected_stability_works() {
  record left[] = {
      {1, 0},
      {2, 1},
      {2, 2},
      {4, 3},
  };

  record right[] = {
      {2, 4}, {2, 5}, {2, 6}, {3, 7}, {4, 8},
  };

  record union_output[9] = {};

  auto union_result = tested::ranges::set_union(
      left, right, union_output, tested::ranges::less{}, key_projection{},
      key_projection{});

  if (union_result.out != union_output + 6 || union_output[0].key != 1 ||
      union_output[0].sequence != 0 || union_output[1].key != 2 ||
      union_output[1].sequence != 1 || union_output[2].key != 2 ||
      union_output[2].sequence != 2 || union_output[3].key != 2 ||
      union_output[3].sequence != 6 || union_output[4].key != 3 ||
      union_output[4].sequence != 7 || union_output[5].key != 4 ||
      union_output[5].sequence != 3) {
    return false;
  }

  record intersection_output[9] = {};

  auto intersection_result = tested::ranges::set_intersection(
      left, right, intersection_output, tested::ranges::less{},
      key_projection{}, key_projection{});

  if (intersection_result.out != intersection_output + 3 ||
      intersection_output[0].key != 2 || intersection_output[0].sequence != 1 ||
      intersection_output[1].key != 2 || intersection_output[1].sequence != 2 ||
      intersection_output[2].key != 4 || intersection_output[2].sequence != 3) {
    return false;
  }

  record symmetric_output[9] = {};

  auto symmetric_result = tested::ranges::set_symmetric_difference(
      left, right, symmetric_output, tested::ranges::less{}, key_projection{},
      key_projection{});

  return symmetric_result.out == symmetric_output + 3 &&
         symmetric_output[0].key == 1 && symmetric_output[0].sequence == 0 &&
         symmetric_output[1].key == 2 && symmetric_output[1].sequence == 6 &&
         symmetric_output[2].key == 3 && symmetric_output[2].sequence == 7;
}

static_assert(projected_stability_works());

constexpr bool projected_difference_uses_tail() {
  record left[] = {
      {2, 0},
      {2, 1},
      {2, 2},
      {4, 3},
  };

  record right[] = {
      {2, 4},
      {2, 5},
  };

  record output[4] = {};

  auto result = tested::ranges::set_difference(
      left, right, output, tested::ranges::less{}, key_projection{},
      key_projection{});

  return result.in == left + 4 && result.out == output + 2 &&
         output[0].key == 2 && output[0].sequence == 2 && output[1].key == 4 &&
         output[1].sequence == 3;
}

static_assert(projected_difference_uses_tail());

bool member_projection_works() {
  record superset[] = {
      {1, 0},
      {2, 1},
      {2, 2},
      {3, 3},
  };

  record subset[] = {
      {2, 4},
      {3, 5},
  };

  if (!tested::ranges::includes(superset, subset, tested::ranges::less{},
                                &record::key, &record::key)) {
    return false;
  }

  record output[6] = {};

  auto result = tested::ranges::set_union(superset, subset, output,
                                          tested::ranges::less{}, &record::key,
                                          &record::key);

  return result.out == output + 4 && output[0].key == 1 && output[1].key == 2 &&
         output[2].key == 2 && output[3].key == 3;
}

bool counted_calls_work() {
  record left[] = {
      {1, 0},
      {3, 1},
      {5, 2},
  };

  record right[] = {
      {2, 3},
      {4, 4},
      {6, 5},
  };

  record output[6] = {};

  int comparator_calls = 0;
  int left_projection_calls = 0;
  int right_projection_calls = 0;

  auto result = tested::ranges::set_union(
      left, right, output, counting_less{&comparator_calls},
      counting_projection{&left_projection_calls},
      counting_projection{&right_projection_calls});

  return result.out == output + 6 && comparator_calls > 0 &&
         left_projection_calls == comparator_calls &&
         right_projection_calls == comparator_calls;
}

bool policy_set_operations_work() {
  int superset[] = {
      1, 1, 2, 2, 3, 4,
  };

  int subset[] = {
      1,
      2,
      4,
  };

  if (!tested::includes(tested::execution::seq, superset, superset + 6, subset,
                        subset + 3)) {
    return false;
  }

  int left[] = {
      1, 1, 2, 2, 2, 4,
  };

  int right[] = {
      1, 2, 2, 3, 4, 4,
  };

  int union_output[12] = {};
  int intersection_output[12] = {};
  int difference_output[12] = {};
  int symmetric_output[12] = {};

  auto union_end = tested::set_union(tested::execution::par, left, left + 6,
                                     right, right + 6, union_output);

  auto intersection_end =
      tested::set_intersection(tested::execution::par_unseq, left, left + 6,
                               right, right + 6, intersection_output);

  auto difference_end =
      tested::set_difference(tested::execution::unseq, left, left + 6, right,
                             right + 6, difference_output);

  auto symmetric_end =
      tested::set_symmetric_difference(tested::execution::seq, left, left + 6,
                                       right, right + 6, symmetric_output);

  return union_end == union_output + 8 &&
         intersection_end == intersection_output + 4 &&
         difference_end == difference_output + 2 &&
         symmetric_end == symmetric_output + 4 && union_output[0] == 1 &&
         union_output[1] == 1 && union_output[2] == 2 && union_output[3] == 2 &&
         union_output[4] == 2 && union_output[5] == 3 && union_output[6] == 4 &&
         union_output[7] == 4 && intersection_output[0] == 1 &&
         intersection_output[1] == 2 && intersection_output[2] == 2 &&
         intersection_output[3] == 4 && difference_output[0] == 1 &&
         difference_output[1] == 2 && symmetric_output[0] == 1 &&
         symmetric_output[1] == 2 && symmetric_output[2] == 3 &&
         symmetric_output[3] == 4;
}

bool ftl_test() {
  return includes_works() && classic_set_operations_work() &&
         ranges_set_operations_work() && projected_stability_works() &&
         projected_difference_uses_tail() && member_projection_works() &&
         counted_calls_work() && policy_set_operations_work();
}
