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

struct input_cursor {
  using value_type = int;
  using difference_type = tested::ptrdiff_t;
  using iterator_concept = tested::input_iterator_tag;
  using iterator_category = tested::input_iterator_tag;

  int *current = nullptr;

  constexpr int &operator*() const noexcept { return *current; }

  constexpr input_cursor &operator++() noexcept {
    ++current;
    return *this;
  }

  constexpr void operator++(int) noexcept { ++current; }

  friend constexpr bool operator==(input_cursor left,
                                   input_cursor right) noexcept {
    return left.current == right.current;
  }
};

static_assert(tested::input_iterator<input_cursor>);
static_assert(!tested::forward_iterator<input_cursor>);

struct record {
  int key = 0;
  int sequence = 0;
};

struct key_projection {
  constexpr int operator()(const record &value) const noexcept {
    return value.key;
  }
};

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

struct move_only_less {
  constexpr bool operator()(const move_only_value &left,
                            const move_only_value &right) const noexcept {
    return left.value < right.value;
  }
};

struct temporary_range {
  int values[6] = {6, 5, 4, 3, 2, 1};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 6; }
};

template <class Comparator>
concept ranges_partial_sort_accepts = requires(
    int *first, int *middle, int *last, int *output, Comparator comparator) {
  {
    tested::ranges::partial_sort(first, middle, last, comparator)
  } -> tested::same_as<int *>;

  tested::ranges::partial_sort_copy(first, last, output, output + 3,
                                    comparator);

  {
    tested::ranges::nth_element(first, middle, last, comparator)
  } -> tested::same_as<int *>;
};

static_assert(ranges_partial_sort_accepts<descending>);

static_assert(!ranges_partial_sort_accepts<void_comparator>);

template <class Policy>
concept policy_partial_sort_accepts = requires(
    Policy &&policy, int *first, int *middle, int *last, int *output) {
  {
    tested::partial_sort(tested::forward<Policy>(policy), first, middle, last)
  } -> tested::same_as<void>;

  {
    tested::partial_sort_copy(tested::forward<Policy>(policy), first, last,
                              output, output + 3)
  } -> tested::same_as<int *>;

  {
    tested::nth_element(tested::forward<Policy>(policy), first, middle, last)
  } -> tested::same_as<void>;
};

static_assert(policy_partial_sort_accepts<tested::execution::sequenced_policy>);

static_assert(policy_partial_sort_accepts<tested::execution::parallel_policy>);

static_assert(policy_partial_sort_accepts<
              tested::execution::parallel_unsequenced_policy>);

static_assert(
    policy_partial_sort_accepts<tested::execution::unsequenced_policy>);

static_assert(
    policy_partial_sort_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_partial_sort_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::partial_sort)>);

static_assert(tested::is_object_v<decltype(tested::ranges::partial_sort_copy)>);

static_assert(tested::is_object_v<decltype(tested::ranges::nth_element)>);

static_assert(
    tested::is_same_v<tested::ranges::partial_sort_copy_result<int *, int *>,
                      tested::ranges::in_out_result<int *, int *>>);

static_assert(tested::is_same_v<decltype(tested::ranges::partial_sort(
                                    tested::declval<temporary_range>(),
                                    tested::declval<int *>())),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::nth_element(
                                    tested::declval<temporary_range>(),
                                    tested::declval<int *>())),
                                tested::ranges::dangling>);

static_assert(
    tested::is_same_v<decltype(tested::ranges::partial_sort_copy(
                          tested::declval<temporary_range>(),
                          tested::declval<temporary_range>())),
                      tested::ranges::partial_sort_copy_result<
                          tested::ranges::dangling, tested::ranges::dangling>>);

constexpr bool first_four_are_smallest(const int *values, int length) {
  if (values[0] != 0 || values[1] != 1 || values[2] != 2 || values[3] != 3) {
    return false;
  }

  for (int index = 4; index < length; ++index) {
    if (values[index] < values[3]) {
      return false;
    }
  }

  return true;
}

constexpr bool classic_partial_sort_works() {
  int values[] = {
      9, 1, 7, 3, 8, 2, 6, 4, 5, 0,
  };

  tested::partial_sort(values, values + 4, values + 10);

  if (!first_four_are_smallest(values, 10)) {
    return false;
  }

  int descending_values[] = {
      1, 9, 3, 7, 5, 8, 2, 6, 4,
  };

  tested::partial_sort(descending_values, descending_values + 3,
                       descending_values + 9, descending{});

  return descending_values[0] == 9 && descending_values[1] == 8 &&
         descending_values[2] == 7;
}

static_assert(classic_partial_sort_works());

constexpr bool ranges_partial_sort_works() {
  int values[] = {
      9, 1, 7, 3, 8, 2, 6, 4, 5, 0,
  };

  auto result = tested::ranges::partial_sort(
      values, values + 4, pointer_sentinel<int>{values + 10});

  if (result != values + 10 || !first_four_are_smallest(values, 10)) {
    return false;
  }

  record records[] = {
      {6, 0}, {2, 1}, {5, 2}, {1, 3}, {4, 4}, {3, 5},
  };

  auto projected_result = tested::ranges::partial_sort(
      records, records + 3, tested::ranges::less{}, key_projection{});

  return projected_result == records + 6 && records[0].key == 1 &&
         records[1].key == 2 && records[2].key == 3;
}

static_assert(ranges_partial_sort_works());

constexpr bool classic_partial_sort_copy_works() {
  int source[] = {
      9, 1, 7, 3, 8, 2, 6, 4, 5, 0,
  };

  int output[4] = {};

  auto result =
      tested::partial_sort_copy(source, source + 10, output, output + 4);

  if (result != output + 4 || output[0] != 0 || output[1] != 1 ||
      output[2] != 2 || output[3] != 3) {
    return false;
  }

  int short_source[] = {4, 1, 3};
  int large_output[6] = {};

  auto short_result = tested::partial_sort_copy(short_source, short_source + 3,
                                                large_output, large_output + 6);

  return short_result == large_output + 3 && large_output[0] == 1 &&
         large_output[1] == 3 && large_output[2] == 4;
}

static_assert(classic_partial_sort_copy_works());

constexpr bool ranges_partial_sort_copy_works() {
  int source[] = {
      9, 1, 7, 3, 8, 2, 6, 4, 5, 0,
  };

  int output[4] = {};

  auto result = tested::ranges::partial_sort_copy(
      input_cursor{source}, input_cursor{source + 10}, output,
      pointer_sentinel<int>{output + 4});

  if (result.in.current != source + 10 || result.out != output + 4 ||
      output[0] != 0 || output[1] != 1 || output[2] != 2 || output[3] != 3) {
    return false;
  }

  int empty_output[1] = {};

  auto empty_result = tested::ranges::partial_sort_copy(
      input_cursor{source}, input_cursor{source + 10}, empty_output,
      empty_output);

  return empty_result.in.current == source + 10 &&
         empty_result.out == empty_output;
}

static_assert(ranges_partial_sort_copy_works());

constexpr bool projected_partial_sort_copy_works() {
  record source[] = {
      {6, 0}, {2, 1}, {5, 2}, {1, 3}, {4, 4}, {3, 5},
  };

  record output[3] = {};

  auto result =
      tested::ranges::partial_sort_copy(source, output, tested::ranges::less{},
                                        key_projection{}, key_projection{});

  return result.in == source + 6 && result.out == output + 3 &&
         output[0].key == 1 && output[1].key == 2 && output[2].key == 3;
}

static_assert(projected_partial_sort_copy_works());

constexpr bool nth_partitioned(const int *values, int length, int nth) {
  for (int index = 0; index < nth; ++index) {
    if (values[nth] < values[index]) {
      return false;
    }
  }

  for (int index = nth + 1; index < length; ++index) {
    if (values[index] < values[nth]) {
      return false;
    }
  }

  return true;
}

constexpr bool classic_nth_element_works() {
  int values[] = {
      9, 1, 7, 3, 8, 2, 6, 4, 5, 0,
  };

  tested::nth_element(values, values + 5, values + 10);

  if (values[5] != 5 || !nth_partitioned(values, 10, 5)) {
    return false;
  }

  int duplicates[] = {
      2, 1, 2, 3, 2, 0, 2, 4, 2,
  };

  tested::nth_element(duplicates, duplicates + 4, duplicates + 9);

  return duplicates[4] == 2 && nth_partitioned(duplicates, 9, 4);
}

static_assert(classic_nth_element_works());

constexpr bool ranges_nth_element_works() {
  int values[] = {
      9, 1, 7, 3, 8, 2, 6, 4, 5, 0,
  };

  auto result = tested::ranges::nth_element(values, values + 5,
                                            pointer_sentinel<int>{values + 10});

  if (result != values + 10 || values[5] != 5 ||
      !nth_partitioned(values, 10, 5)) {
    return false;
  }

  record records[] = {
      {6, 0}, {2, 1}, {5, 2}, {1, 3}, {4, 4}, {3, 5},
  };

  auto projected_result = tested::ranges::nth_element(
      records, records + 3, tested::ranges::less{}, key_projection{});

  return projected_result == records + 6 && records[3].key == 4;
}

static_assert(ranges_nth_element_works());

bool member_projection_works() {
  record values[] = {
      {6, 0}, {2, 1}, {5, 2}, {1, 3}, {4, 4}, {3, 5},
  };

  auto partial_end = tested::ranges::partial_sort(
      values, values + 3, tested::ranges::less{}, &record::key);

  if (partial_end != values + 6 || values[0].key != 1 || values[1].key != 2 ||
      values[2].key != 3) {
    return false;
  }

  record nth_values[] = {
      {6, 0}, {2, 1}, {5, 2}, {1, 3}, {4, 4}, {3, 5},
  };

  auto nth_end = tested::ranges::nth_element(
      nth_values, nth_values + 3, tested::ranges::less{}, &record::key);

  return nth_end == nth_values + 6 && nth_values[3].key == 4;
}

bool move_only_algorithms_work() {
  move_only_value partial_values[] = {
      move_only_value{6}, move_only_value{2}, move_only_value{5},
      move_only_value{1}, move_only_value{4}, move_only_value{3},
  };

  tested::ranges::partial_sort(partial_values, partial_values + 3,
                               move_only_less{});

  if (partial_values[0].value != 1 || partial_values[1].value != 2 ||
      partial_values[2].value != 3) {
    return false;
  }

  move_only_value nth_values[] = {
      move_only_value{6}, move_only_value{2}, move_only_value{5},
      move_only_value{1}, move_only_value{4}, move_only_value{3},
  };

  tested::ranges::nth_element(nth_values, nth_values + 3, move_only_less{});

  return nth_values[3].value == 4;
}

bool policy_algorithms_work() {
  int partial_values[] = {
      9, 1, 7, 3, 8, 2, 6, 4, 5, 0,
  };

  tested::partial_sort(tested::execution::seq, partial_values,
                       partial_values + 4, partial_values + 10);

  if (!first_four_are_smallest(partial_values, 10)) {
    return false;
  }

  int source[] = {
      9, 1, 7, 3, 8, 2, 6, 4, 5, 0,
  };

  int output[4] = {};

  auto output_end = tested::partial_sort_copy(tested::execution::par, source,
                                              source + 10, output, output + 4);

  if (output_end != output + 4 || output[0] != 0 || output[1] != 1 ||
      output[2] != 2 || output[3] != 3) {
    return false;
  }

  int nth_values[] = {
      9, 1, 7, 3, 8, 2, 6, 4, 5, 0,
  };

  tested::nth_element(tested::execution::par_unseq, nth_values, nth_values + 5,
                      nth_values + 10);

  return nth_values[5] == 5 && nth_partitioned(nth_values, 10, 5);
}

bool all_equal_nth_works() {
  int values[64];

  for (int &value : values) {
    value = 7;
  }

  tested::ranges::nth_element(values, values + 32);

  return values[32] == 7 && nth_partitioned(values, 64, 32);
}

bool ftl_test() {
  return classic_partial_sort_works() && ranges_partial_sort_works() &&
         classic_partial_sort_copy_works() &&
         ranges_partial_sort_copy_works() &&
         projected_partial_sort_copy_works() && classic_nth_element_works() &&
         ranges_nth_element_works() && member_projection_works() &&
         move_only_algorithms_work() && policy_algorithms_work() &&
         all_equal_nth_works();
}
