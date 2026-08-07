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

#ifndef __cpp_lib_shift
#error "__cpp_lib_shift must be defined"
#endif

static_assert(__cpp_lib_shift >= 202202L);

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

struct forward_cursor {
  using value_type = int;
  using difference_type = tested::ptrdiff_t;
  using pointer = int *;
  using reference = int &;
  using iterator_concept = tested::forward_iterator_tag;
  using iterator_category = tested::forward_iterator_tag;

  int *current = nullptr;

  constexpr int &operator*() const noexcept { return *current; }

  constexpr forward_cursor &operator++() noexcept {
    ++current;
    return *this;
  }

  constexpr forward_cursor operator++(int) noexcept {
    auto copy = *this;
    ++*this;
    return copy;
  }

  friend constexpr bool operator==(forward_cursor left,
                                   forward_cursor right) noexcept {
    return left.current == right.current;
  }
};

static_assert(tested::forward_iterator<forward_cursor>);

static_assert(!tested::bidirectional_iterator<forward_cursor>);

static_assert(tested::permutable<forward_cursor>);

struct move_only_value {
  int value = 0;

  constexpr move_only_value() noexcept = default;

  constexpr explicit move_only_value(int initial) noexcept : value(initial) {}

  move_only_value(const move_only_value &) = delete;

  move_only_value &operator=(const move_only_value &) = delete;

  constexpr move_only_value(move_only_value &&other) noexcept
      : value(other.value) {
    other.value = -1;
  }

  constexpr move_only_value &operator=(move_only_value &&other) noexcept {
    value = other.value;
    other.value = -1;
    return *this;
  }
};

struct temporary_range {
  int values[5] = {1, 2, 3, 4, 5};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 5; }
};

template <class Policy>
concept policy_swap_shift_accepts = requires(Policy &&policy, int *first,
                                             int *middle, int *last) {
  {
    tested::swap_ranges(tested::forward<Policy>(policy), first, middle, last)
  } -> tested::same_as<int *>;

  {
    tested::shift_left(tested::forward<Policy>(policy), first, last, 2)
  } -> tested::same_as<int *>;

  {
    tested::shift_right(tested::forward<Policy>(policy), first, last, 2)
  } -> tested::same_as<int *>;
};

static_assert(policy_swap_shift_accepts<tested::execution::sequenced_policy>);

static_assert(policy_swap_shift_accepts<tested::execution::parallel_policy>);

static_assert(
    policy_swap_shift_accepts<tested::execution::parallel_unsequenced_policy>);

static_assert(policy_swap_shift_accepts<tested::execution::unsequenced_policy>);

static_assert(
    policy_swap_shift_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_swap_shift_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::swap_ranges)>);

static_assert(tested::is_object_v<decltype(tested::ranges::shift_left)>);

static_assert(tested::is_object_v<decltype(tested::ranges::shift_right)>);

static_assert(
    tested::is_same_v<tested::ranges::swap_ranges_result<int *, int *>,
                      tested::ranges::in_in_result<int *, int *>>);

static_assert(
    tested::is_same_v<decltype(tested::ranges::swap_ranges(
                          tested::declval<temporary_range>(),
                          tested::declval<temporary_range>())),
                      tested::ranges::swap_ranges_result<
                          tested::ranges::dangling, tested::ranges::dangling>>);

static_assert(tested::is_same_v<decltype(tested::ranges::shift_left(
                                    tested::declval<temporary_range>(), 2)),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::shift_right(
                                    tested::declval<temporary_range>(), 2)),
                                tested::ranges::dangling>);

constexpr bool classic_swap_ranges_works() {
  int left[] = {1, 2, 3};
  int right[] = {4, 5, 6, 7};

  auto result = tested::swap_ranges(left, left + 3, right);

  return result == right + 3 && left[0] == 4 && left[1] == 5 && left[2] == 6 &&
         right[0] == 1 && right[1] == 2 && right[2] == 3 && right[3] == 7;
}

static_assert(classic_swap_ranges_works());

constexpr bool ranges_swap_ranges_works() {
  int left[] = {1, 2, 3, 4};
  int right[] = {5, 6};

  auto result =
      tested::ranges::swap_ranges(left, pointer_sentinel<int>{left + 4}, right,
                                  pointer_sentinel<int>{right + 2});

  return result.in1 == left + 2 && result.in2 == right + 2 && left[0] == 5 &&
         left[1] == 6 && left[2] == 3 && left[3] == 4 && right[0] == 1 &&
         right[1] == 2;
}

static_assert(ranges_swap_ranges_works());

constexpr bool classic_shift_left_works() {
  int values[] = {1, 2, 3, 4, 5};

  auto result = tested::shift_left(values, values + 5, 2);

  if (result != values + 3 || values[0] != 3 || values[1] != 4 ||
      values[2] != 5) {
    return false;
  }

  int unchanged[] = {1, 2, 3};

  auto unchanged_result = tested::shift_left(unchanged, unchanged + 3, 0);

  if (unchanged_result != unchanged + 3 || unchanged[0] != 1 ||
      unchanged[1] != 2 || unchanged[2] != 3) {
    return false;
  }

  int exhausted[] = {1, 2, 3};

  return tested::shift_left(exhausted, exhausted + 3, 3) == exhausted;
}

static_assert(classic_shift_left_works());

constexpr bool classic_shift_right_works() {
  int values[] = {1, 2, 3, 4, 5};

  auto result = tested::shift_right(values, values + 5, 2);

  if (result != values + 2 || values[2] != 1 || values[3] != 2 ||
      values[4] != 3) {
    return false;
  }

  int unchanged[] = {1, 2, 3};

  auto unchanged_result = tested::shift_right(unchanged, unchanged + 3, 0);

  if (unchanged_result != unchanged || unchanged[0] != 1 || unchanged[1] != 2 ||
      unchanged[2] != 3) {
    return false;
  }

  int exhausted[] = {1, 2, 3};

  return tested::shift_right(exhausted, exhausted + 3, 3) == exhausted + 3;
}

static_assert(classic_shift_right_works());

constexpr bool ranges_shift_left_works() {
  int values[] = {1, 2, 3, 4, 5};

  auto result =
      tested::ranges::shift_left(values, pointer_sentinel<int>{values + 5}, 2);

  if (result.begin() != values || result.end() != values + 3 ||
      values[0] != 3 || values[1] != 4 || values[2] != 5) {
    return false;
  }

  int exhausted[] = {1, 2, 3};

  auto exhausted_result = tested::ranges::shift_left(exhausted, 5);

  return exhausted_result.begin() == exhausted &&
         exhausted_result.end() == exhausted;
}

static_assert(ranges_shift_left_works());

constexpr bool ranges_shift_right_works() {
  int values[] = {1, 2, 3, 4, 5};

  auto result =
      tested::ranges::shift_right(values, pointer_sentinel<int>{values + 5}, 2);

  if (result.begin() != values + 2 || result.end() != values + 5 ||
      values[2] != 1 || values[3] != 2 || values[4] != 3) {
    return false;
  }

  int exhausted[] = {1, 2, 3};

  auto exhausted_result = tested::ranges::shift_right(exhausted, 5);

  return exhausted_result.begin() == exhausted + 3 &&
         exhausted_result.end() == exhausted + 3;
}

static_assert(ranges_shift_right_works());

constexpr bool forward_shift_right_works() {
  int values[] = {1, 2, 3, 4, 5};

  auto result = tested::ranges::shift_right(forward_cursor{values},
                                            forward_cursor{values + 5}, 2);

  return result.begin().current == values + 2 &&
         result.end().current == values + 5 && values[2] == 1 &&
         values[3] == 2 && values[4] == 3;
}

static_assert(forward_shift_right_works());

bool move_only_shift_works() {
  move_only_value left[] = {
      move_only_value{1}, move_only_value{2}, move_only_value{3},
      move_only_value{4}, move_only_value{5},
  };

  auto left_result = tested::ranges::shift_left(left, 2);

  if (left_result.begin() != left || left_result.end() != left + 3 ||
      left[0].value != 3 || left[1].value != 4 || left[2].value != 5) {
    return false;
  }

  move_only_value right[] = {
      move_only_value{1}, move_only_value{2}, move_only_value{3},
      move_only_value{4}, move_only_value{5},
  };

  auto right_result = tested::ranges::shift_right(right, 2);

  return right_result.begin() == right + 2 && right_result.end() == right + 5 &&
         right[2].value == 1 && right[3].value == 2 && right[4].value == 3;
}

bool policy_swap_shift_works() {
  int left[] = {1, 2, 3};
  int right[] = {4, 5, 6};

  auto swapped_end =
      tested::swap_ranges(tested::execution::seq, left, left + 3, right);

  if (swapped_end != right + 3 || left[0] != 4 || left[1] != 5 ||
      left[2] != 6 || right[0] != 1 || right[1] != 2 || right[2] != 3) {
    return false;
  }

  int shifted_left[] = {1, 2, 3, 4, 5};

  auto left_end = tested::shift_left(tested::execution::par, shifted_left,
                                     shifted_left + 5, 2);

  if (left_end != shifted_left + 3 || shifted_left[0] != 3 ||
      shifted_left[1] != 4 || shifted_left[2] != 5) {
    return false;
  }

  int shifted_right[] = {1, 2, 3, 4, 5};

  auto right_begin = tested::shift_right(tested::execution::par_unseq,
                                         shifted_right, shifted_right + 5, 2);

  return right_begin == shifted_right + 2 && shifted_right[2] == 1 &&
         shifted_right[3] == 2 && shifted_right[4] == 3;
}

bool ftl_test() {
  return classic_swap_ranges_works() && ranges_swap_ranges_works() &&
         classic_shift_left_works() && classic_shift_right_works() &&
         ranges_shift_left_works() && ranges_shift_right_works() &&
         forward_shift_right_works() && move_only_shift_works() &&
         policy_swap_shift_works();
}
