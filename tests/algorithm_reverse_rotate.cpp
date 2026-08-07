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
  int values[4] = {1, 2, 3, 4};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 4; }
};

template <class Policy>
concept policy_reverse_rotate_accepts = requires(
    Policy &&policy, int *first, int *middle, int *last, int *output) {
  {
    tested::reverse(tested::forward<Policy>(policy), first, last)
  } -> tested::same_as<void>;

  {
    tested::reverse_copy(tested::forward<Policy>(policy), first, last, output)
  } -> tested::same_as<int *>;

  {
    tested::rotate(tested::forward<Policy>(policy), first, middle, last)
  } -> tested::same_as<int *>;

  {
    tested::rotate_copy(tested::forward<Policy>(policy), first, middle, last,
                        output)
  } -> tested::same_as<int *>;
};

static_assert(
    policy_reverse_rotate_accepts<tested::execution::sequenced_policy>);

static_assert(
    policy_reverse_rotate_accepts<tested::execution::parallel_policy>);

static_assert(policy_reverse_rotate_accepts<
              tested::execution::parallel_unsequenced_policy>);

static_assert(
    policy_reverse_rotate_accepts<tested::execution::unsequenced_policy>);

static_assert(
    policy_reverse_rotate_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_reverse_rotate_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::reverse)>);

static_assert(tested::is_object_v<decltype(tested::ranges::reverse_copy)>);

static_assert(tested::is_object_v<decltype(tested::ranges::rotate)>);

static_assert(tested::is_object_v<decltype(tested::ranges::rotate_copy)>);

static_assert(
    tested::is_same_v<tested::ranges::reverse_copy_result<int *, int *>,
                      tested::ranges::in_out_result<int *, int *>>);

static_assert(
    tested::is_same_v<tested::ranges::rotate_copy_result<int *, int *>,
                      tested::ranges::in_out_result<int *, int *>>);

static_assert(tested::is_same_v<decltype(tested::ranges::reverse(
                                    tested::declval<temporary_range>())),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::rotate(
                                    tested::declval<temporary_range>(),
                                    tested::declval<int *>())),
                                tested::ranges::dangling>);

static_assert(
    tested::is_same_v<
        decltype(tested::ranges::reverse_copy(
            tested::declval<temporary_range>(), tested::declval<int *>())),
        tested::ranges::reverse_copy_result<tested::ranges::dangling, int *>>);

static_assert(
    tested::is_same_v<
        decltype(tested::ranges::rotate_copy(tested::declval<temporary_range>(),
                                             tested::declval<int *>(),
                                             tested::declval<int *>())),
        tested::ranges::rotate_copy_result<tested::ranges::dangling, int *>>);

constexpr bool classic_reverse_works() {
  int even_values[] = {1, 2, 3, 4};

  tested::reverse(even_values, even_values + 4);

  if (even_values[0] != 4 || even_values[1] != 3 || even_values[2] != 2 ||
      even_values[3] != 1) {
    return false;
  }

  int odd_values[] = {1, 2, 3, 4, 5};

  tested::reverse(odd_values, odd_values + 5);

  if (odd_values[0] != 5 || odd_values[1] != 4 || odd_values[2] != 3 ||
      odd_values[3] != 2 || odd_values[4] != 1) {
    return false;
  }

  int empty[1] = {};

  tested::reverse(empty, empty);

  return true;
}

static_assert(classic_reverse_works());

constexpr bool ranges_reverse_works() {
  int values[] = {1, 2, 3, 4, 5};

  auto result =
      tested::ranges::reverse(values, pointer_sentinel<int>{values + 5});

  return result == values + 5 && values[0] == 5 && values[1] == 4 &&
         values[2] == 3 && values[3] == 2 && values[4] == 1;
}

static_assert(ranges_reverse_works());

constexpr bool reverse_copy_works() {
  int source[] = {1, 2, 3, 4};
  int classic_output[4] = {};

  auto classic_end = tested::reverse_copy(source, source + 4, classic_output);

  if (classic_end != classic_output + 4 || classic_output[0] != 4 ||
      classic_output[1] != 3 || classic_output[2] != 2 ||
      classic_output[3] != 1) {
    return false;
  }

  int ranges_output[4] = {};

  auto ranges_result = tested::ranges::reverse_copy(
      source, pointer_sentinel<int>{source + 4}, ranges_output);

  return ranges_result.in == source + 4 &&
         ranges_result.out == ranges_output + 4 && ranges_output[0] == 4 &&
         ranges_output[1] == 3 && ranges_output[2] == 2 &&
         ranges_output[3] == 1 && source[0] == 1 && source[1] == 2 &&
         source[2] == 3 && source[3] == 4;
}

static_assert(reverse_copy_works());

constexpr bool classic_rotate_works() {
  int values[] = {1, 2, 3, 4, 5};

  auto result = tested::rotate(values, values + 2, values + 5);

  if (result != values + 3 || values[0] != 3 || values[1] != 4 ||
      values[2] != 5 || values[3] != 1 || values[4] != 2) {
    return false;
  }

  int no_left[] = {1, 2, 3};

  auto no_left_result = tested::rotate(no_left, no_left, no_left + 3);

  if (no_left_result != no_left + 3 || no_left[0] != 1 || no_left[1] != 2 ||
      no_left[2] != 3) {
    return false;
  }

  int no_right[] = {1, 2, 3};

  auto no_right_result = tested::rotate(no_right, no_right + 3, no_right + 3);

  return no_right_result == no_right && no_right[0] == 1 && no_right[1] == 2 &&
         no_right[2] == 3;
}

static_assert(classic_rotate_works());

constexpr bool ranges_rotate_works() {
  int values[] = {1, 2, 3, 4, 5};

  auto result = tested::ranges::rotate(values, values + 2,
                                       pointer_sentinel<int>{values + 5});

  if (result.begin() != values + 3 || result.end() != values + 5) {
    return false;
  }

  if (values[0] != 3 || values[1] != 4 || values[2] != 5 || values[3] != 1 ||
      values[4] != 2) {
    return false;
  }

  int forward_values[] = {1, 2, 3, 4, 5};

  auto forward_result = tested::ranges::rotate(
      forward_cursor{forward_values}, forward_cursor{forward_values + 2},
      forward_cursor{forward_values + 5});

  return forward_result.begin().current == forward_values + 3 &&
         forward_result.end().current == forward_values + 5 &&
         forward_values[0] == 3 && forward_values[1] == 4 &&
         forward_values[2] == 5 && forward_values[3] == 1 &&
         forward_values[4] == 2;
}

static_assert(ranges_rotate_works());

constexpr bool rotate_copy_works() {
  int source[] = {1, 2, 3, 4, 5};
  int classic_output[5] = {};

  auto classic_end =
      tested::rotate_copy(source, source + 2, source + 5, classic_output);

  if (classic_end != classic_output + 5 || classic_output[0] != 3 ||
      classic_output[1] != 4 || classic_output[2] != 5 ||
      classic_output[3] != 1 || classic_output[4] != 2) {
    return false;
  }

  int ranges_output[5] = {};

  auto ranges_result = tested::ranges::rotate_copy(
      source, source + 2, pointer_sentinel<int>{source + 5}, ranges_output);

  return ranges_result.in == source + 5 &&
         ranges_result.out == ranges_output + 5 && ranges_output[0] == 3 &&
         ranges_output[1] == 4 && ranges_output[2] == 5 &&
         ranges_output[3] == 1 && ranges_output[4] == 2 && source[0] == 1 &&
         source[1] == 2 && source[2] == 3 && source[3] == 4 && source[4] == 5;
}

static_assert(rotate_copy_works());

bool move_only_rearrangement_works() {
  move_only_value reversed[] = {
      move_only_value{1},
      move_only_value{2},
      move_only_value{3},
      move_only_value{4},
  };

  auto reverse_end = tested::ranges::reverse(reversed);

  if (reverse_end != reversed + 4 || reversed[0].value != 4 ||
      reversed[1].value != 3 || reversed[2].value != 2 ||
      reversed[3].value != 1) {
    return false;
  }

  move_only_value rotated[] = {
      move_only_value{1}, move_only_value{2}, move_only_value{3},
      move_only_value{4}, move_only_value{5},
  };

  auto rotate_result = tested::ranges::rotate(rotated, rotated + 2);

  return rotate_result.begin() == rotated + 3 &&
         rotate_result.end() == rotated + 5 && rotated[0].value == 3 &&
         rotated[1].value == 4 && rotated[2].value == 5 &&
         rotated[3].value == 1 && rotated[4].value == 2;
}

bool policy_reverse_rotate_works() {
  int reversed[] = {1, 2, 3, 4};
  int reverse_output[4] = {};

  int rotated[] = {1, 2, 3, 4, 5};
  int rotate_output[5] = {};

  tested::reverse(tested::execution::seq, reversed, reversed + 4);

  auto reverse_copy_end = tested::reverse_copy(tested::execution::par, reversed,
                                               reversed + 4, reverse_output);

  auto rotate_result = tested::rotate(tested::execution::par_unseq, rotated,
                                      rotated + 2, rotated + 5);

  auto rotate_copy_end =
      tested::rotate_copy(tested::execution::unseq, reversed, reversed + 1,
                          reversed + 4, rotate_output);

  return reversed[0] == 4 && reversed[1] == 3 && reversed[2] == 2 &&
         reversed[3] == 1 && reverse_copy_end == reverse_output + 4 &&
         reverse_output[0] == 1 && reverse_output[1] == 2 &&
         reverse_output[2] == 3 && reverse_output[3] == 4 &&
         rotate_result == rotated + 3 && rotated[0] == 3 && rotated[1] == 4 &&
         rotated[2] == 5 && rotated[3] == 1 && rotated[4] == 2 &&
         rotate_copy_end == rotate_output + 4 && rotate_output[0] == 3 &&
         rotate_output[1] == 2 && rotate_output[2] == 1 &&
         rotate_output[3] == 4;
}

bool ftl_test() {
  return classic_reverse_works() && ranges_reverse_works() &&
         reverse_copy_works() && classic_rotate_works() &&
         ranges_rotate_works() && rotate_copy_works() &&
         move_only_rearrangement_works() && policy_reverse_rotate_works();
}