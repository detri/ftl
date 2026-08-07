#ifdef FTL_REPLACE_STL
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <optional>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/algorithm>
#include <ftl/concepts>
#include <ftl/cstddef>
#include <ftl/iterator>
#include <ftl/optional>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

static_assert(__cpp_lib_ranges_fold >= 202207L);

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

struct plus_fold {
  constexpr int operator()(int left, int right) const noexcept {
    return left + right;
  }
};

struct subtract_left {
  constexpr int operator()(int accumulator, int value) const noexcept {
    return accumulator - value;
  }
};

struct subtract_right {
  constexpr int operator()(int value, int accumulator) const noexcept {
    return value - accumulator;
  }
};

struct widen_left {
  constexpr long operator()(long accumulator, int value) const noexcept {
    return accumulator + value;
  }
};

struct widen_right {
  constexpr long operator()(int value, long accumulator) const noexcept {
    return value + accumulator;
  }
};

struct move_accumulator {
  int value = 0;

  constexpr move_accumulator() noexcept = default;

  constexpr explicit move_accumulator(int initial) noexcept : value(initial) {}

  move_accumulator(const move_accumulator &) = delete;

  move_accumulator &operator=(const move_accumulator &) = delete;

  constexpr move_accumulator(move_accumulator &&other) noexcept
      : value(other.value) {
    other.value = -1;
  }

  constexpr move_accumulator &operator=(move_accumulator &&other) noexcept {
    value = other.value;
    other.value = -1;
    return *this;
  }
};

struct move_fold {
  constexpr move_accumulator operator()(move_accumulator accumulator,
                                        int value) const noexcept {
    accumulator.value += value;
    return accumulator;
  }
};

struct temporary_range {
  int values[4] = {1, 2, 3, 4};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 4; }
};

static_assert(tested::is_object_v<decltype(tested::ranges::fold_left)>);

static_assert(tested::is_object_v<decltype(tested::ranges::fold_left_first)>);

static_assert(tested::is_object_v<decltype(tested::ranges::fold_right)>);

static_assert(tested::is_object_v<decltype(tested::ranges::fold_right_last)>);

static_assert(
    tested::is_object_v<decltype(tested::ranges::fold_left_with_iter)>);

static_assert(
    tested::is_object_v<decltype(tested::ranges::fold_left_first_with_iter)>);

static_assert(
    tested::is_same_v<tested::ranges::fold_left_with_iter_result<int *, int>,
                      tested::ranges::in_value_result<int *, int>>);

static_assert(tested::is_same_v<
              tested::ranges::fold_left_first_with_iter_result<
                  int *, tested::optional<int>>,
              tested::ranges::in_value_result<int *, tested::optional<int>>>);

static_assert(
    tested::is_same_v<decltype(tested::ranges::fold_left_with_iter(
                          tested::declval<temporary_range>(), 0, plus_fold{})),
                      tested::ranges::fold_left_with_iter_result<
                          tested::ranges::dangling, int>>);

static_assert(
    tested::is_same_v<decltype(tested::ranges::fold_left_first_with_iter(
                          tested::declval<temporary_range>(), plus_fold{})),
                      tested::ranges::fold_left_first_with_iter_result<
                          tested::ranges::dangling, tested::optional<int>>>);

constexpr bool fold_left_works() {
  int values[] = {1, 2, 3, 4};

  if (tested::ranges::fold_left(values, 0, plus_fold{}) != 10) {
    return false;
  }

  if (tested::ranges::fold_left(values, 0, subtract_left{}) != -10) {
    return false;
  }

  auto sentinel_result = tested::ranges::fold_left(
      values, pointer_sentinel<int>{values + 4}, 10, plus_fold{});

  if (sentinel_result != 20) {
    return false;
  }

  int *empty = values;

  return tested::ranges::fold_left(empty, empty, 42, plus_fold{}) == 42;
}

static_assert(fold_left_works());

constexpr bool fold_left_changes_type() {
  int values[] = {1, 2, 3};

  static_assert(
      tested::is_same_v<
          decltype(tested::ranges::fold_left(values, 0, widen_left{})), long>);

  return tested::ranges::fold_left(values, 0, widen_left{}) == 6L;
}

static_assert(fold_left_changes_type());

constexpr bool fold_left_first_works() {
  int values[] = {10, 2, 3};

  auto result = tested::ranges::fold_left_first(values, subtract_left{});

  if (!result || *result != 5) {
    return false;
  }

  int empty[1] = {};

  auto empty_result =
      tested::ranges::fold_left_first(empty, empty, plus_fold{});

  return !empty_result;
}

static_assert(fold_left_first_works());

constexpr bool fold_left_with_iter_works() {
  int values[] = {1, 2, 3, 4};

  auto result = tested::ranges::fold_left_with_iter(
      values, pointer_sentinel<int>{values + 4}, 10, plus_fold{});

  if (result.in != values + 4 || result.value != 20) {
    return false;
  }

  auto first_result = tested::ranges::fold_left_first_with_iter(
      values, pointer_sentinel<int>{values + 4}, plus_fold{});

  if (first_result.in != values + 4 || !first_result.value ||
      *first_result.value != 10) {
    return false;
  }

  auto empty_result =
      tested::ranges::fold_left_first_with_iter(values, values, plus_fold{});

  return empty_result.in == values && !empty_result.value;
}

static_assert(fold_left_with_iter_works());

constexpr bool fold_right_works() {
  int values[] = {1, 2, 3};

  /*
   * 1 - (2 - (3 - 0)) == 2
   */
  if (tested::ranges::fold_right(values, 0, subtract_right{}) != 2) {
    return false;
  }

  if (tested::ranges::fold_right(values, pointer_sentinel<int>{values + 3}, 0,
                                 plus_fold{}) != 6) {
    return false;
  }

  int *empty = values;

  return tested::ranges::fold_right(empty, empty, 42, plus_fold{}) == 42;
}

static_assert(fold_right_works());

constexpr bool fold_right_changes_type() {
  int values[] = {1, 2, 3};

  static_assert(tested::is_same_v<decltype(tested::ranges::fold_right(
                                      values, 0, widen_right{})),
                                  long>);

  return tested::ranges::fold_right(values, 0, widen_right{}) == 6L;
}

static_assert(fold_right_changes_type());

constexpr bool fold_right_last_works() {
  int values[] = {1, 2, 3};

  /*
   * 1 - (2 - 3) == 2
   */
  auto result = tested::ranges::fold_right_last(values, subtract_right{});

  if (!result || *result != 2) {
    return false;
  }

  int one[] = {7};

  auto one_result = tested::ranges::fold_right_last(one, plus_fold{});

  if (!one_result || *one_result != 7) {
    return false;
  }

  int empty[1] = {};

  auto empty_result =
      tested::ranges::fold_right_last(empty, empty, plus_fold{});

  return !empty_result;
}

static_assert(fold_right_last_works());

constexpr bool move_only_accumulator_works() {
  int values[] = {1, 2, 3, 4};

  auto result =
      tested::ranges::fold_left(values, move_accumulator{10}, move_fold{});

  return result.value == 20;
}

static_assert(move_only_accumulator_works());

constexpr bool range_overloads_work() {
  int values[] = {1, 2, 3, 4};

  if (tested::ranges::fold_left(values, 5, plus_fold{}) != 15) {
    return false;
  }

  auto left_first = tested::ranges::fold_left_first(values, plus_fold{});

  if (!left_first || *left_first != 10) {
    return false;
  }

  if (tested::ranges::fold_right(values, 5, plus_fold{}) != 15) {
    return false;
  }

  auto right_last = tested::ranges::fold_right_last(values, plus_fold{});

  return right_last && *right_last == 10;
}

static_assert(range_overloads_work());

bool ftl_test() {
  return fold_left_works() && fold_left_changes_type() &&
         fold_left_first_works() && fold_left_with_iter_works() &&
         fold_right_works() && fold_right_changes_type() &&
         fold_right_last_works() && move_only_accumulator_works() &&
         range_overloads_work();
}
