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

struct bidirectional_cursor {
  using value_type = int;
  using difference_type = tested::ptrdiff_t;
  using pointer = int *;
  using reference = int &;
  using iterator_concept = tested::bidirectional_iterator_tag;
  using iterator_category = tested::bidirectional_iterator_tag;

  int *current = nullptr;

  constexpr int &operator*() const noexcept { return *current; }

  constexpr bidirectional_cursor &operator++() noexcept {
    ++current;
    return *this;
  }

  constexpr bidirectional_cursor operator++(int) noexcept {
    auto copy = *this;
    ++*this;
    return copy;
  }

  constexpr bidirectional_cursor &operator--() noexcept {
    --current;
    return *this;
  }

  constexpr bidirectional_cursor operator--(int) noexcept {
    auto copy = *this;
    --*this;
    return copy;
  }

  friend constexpr bool operator==(bidirectional_cursor left,
                                   bidirectional_cursor right) noexcept {
    return left.current == right.current;
  }
};

static_assert(tested::bidirectional_iterator<bidirectional_cursor>);

static_assert(!tested::random_access_iterator<bidirectional_cursor>);

static_assert(tested::permutable<bidirectional_cursor>);

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

struct record_less {
  constexpr bool operator()(const record &left,
                            const record &right) const noexcept {
    return left.key < right.key;
  }
};

struct move_only_value {
  int key = 0;
  int sequence = 0;

  constexpr move_only_value() noexcept = default;

  constexpr move_only_value(int initial_key, int initial_sequence) noexcept
      : key(initial_key), sequence(initial_sequence) {}

  move_only_value(const move_only_value &) = delete;

  move_only_value &operator=(const move_only_value &) = delete;

  constexpr move_only_value(move_only_value &&other) noexcept
      : key(other.key), sequence(other.sequence) {
    other.key = -1;
    other.sequence = -1;
  }

  constexpr move_only_value &operator=(move_only_value &&other) noexcept {
    key = other.key;
    sequence = other.sequence;

    other.key = -1;
    other.sequence = -1;

    return *this;
  }
};

struct move_only_less {
  constexpr bool operator()(const move_only_value &left,
                            const move_only_value &right) const noexcept {
    return left.key < right.key;
  }
};

struct temporary_range {
  int values[3] = {1, 3, 5};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 3; }
};

template <class Comparator>
concept ranges_merge_accepts =
    requires(int *first1, int *last1, int *first2, int *last2, int *output,
             Comparator comparator) {
      tested::ranges::merge(first1, last1, first2, last2, output, comparator);

      {
        tested::ranges::inplace_merge(first1, first1 + 2, last1, comparator)
      } -> tested::same_as<int *>;
    };

static_assert(ranges_merge_accepts<descending>);

static_assert(!ranges_merge_accepts<void_comparator>);

template <class Policy>
concept policy_merge_accepts =
    requires(Policy &&policy, int *first1, int *last1, int *first2, int *last2,
             int *output) {
      {
        tested::merge(tested::forward<Policy>(policy), first1, last1, first2,
                      last2, output)
      } -> tested::same_as<int *>;

      {
        tested::inplace_merge(tested::forward<Policy>(policy), first1,
                              first1 + 2, last1)
      } -> tested::same_as<void>;
    };

static_assert(policy_merge_accepts<tested::execution::sequenced_policy>);

static_assert(policy_merge_accepts<tested::execution::parallel_policy>);

static_assert(
    policy_merge_accepts<tested::execution::parallel_unsequenced_policy>);

static_assert(policy_merge_accepts<tested::execution::unsequenced_policy>);

static_assert(
    policy_merge_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_merge_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::merge)>);

static_assert(tested::is_object_v<decltype(tested::ranges::inplace_merge)>);

static_assert(tested::is_same_v<
              tested::ranges::merge_result<int *, const int *, int *>,
              tested::ranges::in_in_out_result<int *, const int *, int *>>);

static_assert(tested::is_same_v<
              decltype(tested::ranges::merge(tested::declval<temporary_range>(),
                                             tested::declval<temporary_range>(),
                                             tested::declval<int *>())),
              tested::ranges::merge_result<tested::ranges::dangling,
                                           tested::ranges::dangling, int *>>);

static_assert(tested::is_same_v<decltype(tested::ranges::inplace_merge(
                                    tested::declval<temporary_range>(),
                                    tested::declval<int *>())),
                                tested::ranges::dangling>);

constexpr bool classic_merge_works() {
  int left[] = {1, 3, 5};
  int right[] = {2, 4, 6};
  int output[6] = {};

  auto result = tested::merge(left, left + 3, right, right + 3, output);

  if (result != output + 6) {
    return false;
  }

  for (int index = 0; index < 6; ++index) {
    if (output[index] != index + 1) {
      return false;
    }
  }

  int short_left[] = {1};
  int long_right[] = {2, 3, 4};
  int uneven_output[4] = {};

  auto uneven_result = tested::merge(short_left, short_left + 1, long_right,
                                     long_right + 3, uneven_output);

  return uneven_result == uneven_output + 4 && uneven_output[0] == 1 &&
         uneven_output[1] == 2 && uneven_output[2] == 3 &&
         uneven_output[3] == 4;
}

static_assert(classic_merge_works());

constexpr bool comparator_merge_works() {
  int left[] = {9, 7, 5};
  int right[] = {8, 6, 4};
  int output[6] = {};

  auto result =
      tested::merge(left, left + 3, right, right + 3, output, descending{});

  return result == output + 6 && output[0] == 9 && output[1] == 8 &&
         output[2] == 7 && output[3] == 6 && output[4] == 5 && output[5] == 4;
}

static_assert(comparator_merge_works());

constexpr bool ranges_merge_works() {
  int left[] = {1, 3, 5};
  int right[] = {2, 4};
  int output[5] = {};

  auto result =
      tested::ranges::merge(left, pointer_sentinel<int>{left + 3}, right,
                            pointer_sentinel<int>{right + 2}, output);

  return result.in1 == left + 3 && result.in2 == right + 2 &&
         result.out == output + 5 && output[0] == 1 && output[1] == 2 &&
         output[2] == 3 && output[3] == 4 && output[4] == 5;
}

static_assert(ranges_merge_works());

constexpr bool projected_merge_works() {
  record left[] = {
      {1, 0},
      {2, 1},
      {2, 2},
      {4, 3},
  };

  record right[] = {
      {2, 4},
      {2, 5},
      {3, 6},
  };

  record output[7] = {};

  auto result =
      tested::ranges::merge(left, right, output, tested::ranges::less{},
                            key_projection{}, key_projection{});

  return result.in1 == left + 4 && result.in2 == right + 3 &&
         result.out == output + 7 && output[0].key == 1 &&
         output[0].sequence == 0 && output[1].key == 2 &&
         output[1].sequence == 1 && output[2].key == 2 &&
         output[2].sequence == 2 && output[3].key == 2 &&
         output[3].sequence == 4 && output[4].key == 2 &&
         output[4].sequence == 5 && output[5].key == 3 &&
         output[5].sequence == 6 && output[6].key == 4 &&
         output[6].sequence == 3;
}

static_assert(projected_merge_works());

bool member_projection_merge_works() {
  record left[] = {
      {1, 0},
      {3, 1},
  };

  record right[] = {
      {2, 2},
      {4, 3},
  };

  record output[4] = {};

  auto result = tested::ranges::merge(
      left, right, output, tested::ranges::less{}, &record::key, &record::key);

  return result.in1 == left + 2 && result.in2 == right + 2 &&
         result.out == output + 4 && output[0].key == 1 && output[1].key == 2 &&
         output[2].key == 3 && output[3].key == 4;
}

bool counted_merge_works() {
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

  auto result = tested::ranges::merge(
      left, right, output, counting_less{&comparator_calls},
      counting_projection{&left_projection_calls},
      counting_projection{&right_projection_calls});

  return result.out == output + 6 && comparator_calls == 5 &&
         left_projection_calls == comparator_calls &&
         right_projection_calls == comparator_calls;
}

bool classic_inplace_merge_works() {
  int values[] = {
      1, 3, 5, 2, 4, 6,
  };

  tested::inplace_merge(values, values + 3, values + 6);

  for (int index = 0; index < 6; ++index) {
    if (values[index] != index + 1) {
      return false;
    }
  }

  int descending_values[] = {
      9, 7, 5, 8, 6, 4,
  };

  tested::inplace_merge(descending_values, descending_values + 3,
                        descending_values + 6, descending{});

  return descending_values[0] == 9 && descending_values[1] == 8 &&
         descending_values[2] == 7 && descending_values[3] == 6 &&
         descending_values[4] == 5 && descending_values[5] == 4;
}

bool ranges_inplace_merge_works() {
  int values[] = {
      1, 3, 5, 2, 4, 6,
  };

  auto result = tested::ranges::inplace_merge(values, values + 3);

  if (result != values + 6) {
    return false;
  }

  for (int index = 0; index < 6; ++index) {
    if (values[index] != index + 1) {
      return false;
    }
  }

  int sentinel_values[] = {
      1, 4, 7, 2, 3, 8,
  };

  auto sentinel_result =
      tested::ranges::inplace_merge(sentinel_values, sentinel_values + 3,
                                    pointer_sentinel<int>{sentinel_values + 6});

  return sentinel_result == sentinel_values + 6 && sentinel_values[0] == 1 &&
         sentinel_values[1] == 2 && sentinel_values[2] == 3 &&
         sentinel_values[3] == 4 && sentinel_values[4] == 7 &&
         sentinel_values[5] == 8;
}

bool bidirectional_inplace_merge_works() {
  int values[] = {
      1, 3, 5, 2, 4, 6,
  };

  auto result = tested::ranges::inplace_merge(bidirectional_cursor{values},
                                              bidirectional_cursor{values + 3},
                                              bidirectional_cursor{values + 6});

  return result.current == values + 6 && values[0] == 1 && values[1] == 2 &&
         values[2] == 3 && values[3] == 4 && values[4] == 5 && values[5] == 6;
}

bool stable_inplace_merge_works() {
  record values[] = {
      {1, 0}, {2, 1}, {2, 2}, {4, 3},

      {2, 4}, {2, 5}, {3, 6}, {4, 7},
  };

  auto result = tested::ranges::inplace_merge(
      values, values + 4, tested::ranges::less{}, &record::key);

  return result == values + 8 && values[0].key == 1 &&
         values[0].sequence == 0 && values[1].key == 2 &&
         values[1].sequence == 1 && values[2].key == 2 &&
         values[2].sequence == 2 && values[3].key == 2 &&
         values[3].sequence == 4 && values[4].key == 2 &&
         values[4].sequence == 5 && values[5].key == 3 &&
         values[5].sequence == 6 && values[6].key == 4 &&
         values[6].sequence == 3 && values[7].key == 4 &&
         values[7].sequence == 7;
}

bool move_only_inplace_merge_works() {
  move_only_value values[] = {
      move_only_value{1, 0}, move_only_value{3, 1}, move_only_value{5, 2},

      move_only_value{2, 3}, move_only_value{4, 4}, move_only_value{6, 5},
  };

  auto result =
      tested::ranges::inplace_merge(values, values + 3, move_only_less{});

  return result == values + 6 && values[0].key == 1 && values[1].key == 2 &&
         values[2].key == 3 && values[3].key == 4 && values[4].key == 5 &&
         values[5].key == 6;
}

bool policy_merge_works() {
  int left[] = {1, 3, 5};
  int right[] = {2, 4, 6};
  int output[6] = {};

  auto output_end = tested::merge(tested::execution::seq, left, left + 3, right,
                                  right + 3, output);

  if (output_end != output + 6) {
    return false;
  }

  for (int index = 0; index < 6; ++index) {
    if (output[index] != index + 1) {
      return false;
    }
  }

  int values[] = {
      1, 3, 5, 2, 4, 6,
  };

  tested::inplace_merge(tested::execution::par, values, values + 3, values + 6);

  for (int index = 0; index < 6; ++index) {
    if (values[index] != index + 1) {
      return false;
    }
  }

  int descending_left[] = {9, 7, 5};
  int descending_right[] = {8, 6, 4};
  int descending_output[6] = {};

  auto descending_end = tested::merge(
      tested::execution::par_unseq, descending_left, descending_left + 3,
      descending_right, descending_right + 3, descending_output, descending{});

  return descending_end == descending_output + 6 && descending_output[0] == 9 &&
         descending_output[1] == 8 && descending_output[2] == 7 &&
         descending_output[3] == 6 && descending_output[4] == 5 &&
         descending_output[5] == 4;
}

bool ftl_test() {
  return classic_merge_works() && comparator_merge_works() &&
         ranges_merge_works() && projected_merge_works() &&
         member_projection_merge_works() && counted_merge_works() &&
         classic_inplace_merge_works() && ranges_inplace_merge_works() &&
         bidirectional_inplace_merge_works() && stable_inplace_merge_works() &&
         move_only_inplace_merge_works() && policy_merge_works();
}
