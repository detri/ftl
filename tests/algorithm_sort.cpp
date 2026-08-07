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

static_assert(!tested::random_access_iterator<forward_cursor>);

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
  int values[5] = {5, 4, 3, 2, 1};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 5; }
};

template <class Comparator>
concept ranges_sort_accepts =
    requires(int *first, int *last, Comparator comparator) {
      {
        tested::ranges::is_sorted(first, last, comparator)
      } -> tested::same_as<bool>;

      {
        tested::ranges::is_sorted_until(first, last, comparator)
      } -> tested::same_as<int *>;

      {
        tested::ranges::sort(first, last, comparator)
      } -> tested::same_as<int *>;

      {
        tested::ranges::stable_sort(first, last, comparator)
      } -> tested::same_as<int *>;
    };

static_assert(ranges_sort_accepts<descending>);
static_assert(!ranges_sort_accepts<void_comparator>);

template <class Policy>
concept policy_sort_accepts = requires(Policy &&policy, int *first, int *last) {
  {
    tested::is_sorted(tested::forward<Policy>(policy), first, last)
  } -> tested::same_as<bool>;

  {
    tested::is_sorted_until(tested::forward<Policy>(policy), first, last)
  } -> tested::same_as<int *>;

  {
    tested::sort(tested::forward<Policy>(policy), first, last)
  } -> tested::same_as<void>;

  {
    tested::stable_sort(tested::forward<Policy>(policy), first, last)
  } -> tested::same_as<void>;
};

static_assert(policy_sort_accepts<tested::execution::sequenced_policy>);

static_assert(policy_sort_accepts<tested::execution::parallel_policy>);

static_assert(
    policy_sort_accepts<tested::execution::parallel_unsequenced_policy>);

static_assert(policy_sort_accepts<tested::execution::unsequenced_policy>);

static_assert(policy_sort_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_sort_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::is_sorted)>);

static_assert(tested::is_object_v<decltype(tested::ranges::is_sorted_until)>);

static_assert(tested::is_object_v<decltype(tested::ranges::sort)>);

static_assert(tested::is_object_v<decltype(tested::ranges::stable_sort)>);

static_assert(tested::is_same_v<decltype(tested::ranges::sort(
                                    tested::declval<temporary_range>())),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::stable_sort(
                                    tested::declval<temporary_range>())),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::is_sorted_until(
                                    tested::declval<temporary_range>())),
                                tested::ranges::dangling>);

constexpr bool inspection_works() {
  int sorted[] = {1, 2, 2, 3, 4};
  int unsorted[] = {1, 2, 4, 3, 5};

  if (!tested::is_sorted(sorted, sorted + 5)) {
    return false;
  }

  if (tested::is_sorted(unsorted, unsorted + 5)) {
    return false;
  }

  if (tested::is_sorted_until(unsorted, unsorted + 5) != unsorted + 3) {
    return false;
  }

  int descending_values[] = {5, 4, 4, 2, 1};

  if (!tested::is_sorted(descending_values, descending_values + 5,
                         descending{})) {
    return false;
  }

  auto ranges_result = tested::ranges::is_sorted_until(
      unsorted, pointer_sentinel<int>{unsorted + 5});

  if (ranges_result != unsorted + 3) {
    return false;
  }

  auto forward_result = tested::ranges::is_sorted_until(
      forward_cursor{unsorted}, forward_cursor{unsorted + 5});

  return forward_result.current == unsorted + 3;
}

static_assert(inspection_works());

constexpr bool classic_sort_works() {
  int values[] = {
      9, 1, 4, 7, 3, 8, 2, 6, 5, 0,
  };

  tested::sort(values, values + 10);

  for (int index = 0; index < 10; ++index) {
    if (values[index] != index) {
      return false;
    }
  }

  int duplicates[] = {
      4, 1, 3, 1, 2, 4, 2, 3,
  };

  tested::sort(duplicates, duplicates + 8);

  if (!tested::is_sorted(duplicates, duplicates + 8)) {
    return false;
  }

  int descending_values[] = {
      1, 4, 2, 5, 3,
  };

  tested::sort(descending_values, descending_values + 5, descending{});

  return descending_values[0] == 5 && descending_values[1] == 4 &&
         descending_values[2] == 3 && descending_values[3] == 2 &&
         descending_values[4] == 1;
}

static_assert(classic_sort_works());

constexpr bool ranges_sort_works() {
  int values[] = {
      8, 3, 7, 1, 6, 2, 5, 4,
  };

  auto result = tested::ranges::sort(values, pointer_sentinel<int>{values + 8});

  if (result != values + 8 || !tested::ranges::is_sorted(values)) {
    return false;
  }

  record records[] = {
      {4, 0},
      {1, 1},
      {3, 2},
      {2, 3},
  };

  auto projected_result =
      tested::ranges::sort(records, tested::ranges::less{}, key_projection{});

  return projected_result == records + 4 && records[0].key == 1 &&
         records[1].key == 2 && records[2].key == 3 && records[3].key == 4 &&
         tested::ranges::is_sorted(records, tested::ranges::less{},
                                   key_projection{});
}

static_assert(ranges_sort_works());

bool member_projection_sort_works() {
  record records[] = {
      {4, 0},
      {1, 1},
      {3, 2},
      {2, 3},
  };

  auto result =
      tested::ranges::sort(records, tested::ranges::less{}, &record::key);

  return result == records + 4 && records[0].key == 1 && records[1].key == 2 &&
         records[2].key == 3 && records[3].key == 4;
}

bool stable_sort_works() {
  record records[] = {
      {2, 0}, {1, 1}, {2, 2}, {1, 3}, {3, 4}, {2, 5}, {1, 6},
  };

  tested::stable_sort(records, records + 7,
                      [](const record &left, const record &right) {
                        return left.key < right.key;
                      });

  if (records[0].key != 1 || records[0].sequence != 1 || records[1].key != 1 ||
      records[1].sequence != 3 || records[2].key != 1 ||
      records[2].sequence != 6 || records[3].key != 2 ||
      records[3].sequence != 0 || records[4].key != 2 ||
      records[4].sequence != 2 || records[5].key != 2 ||
      records[5].sequence != 5 || records[6].key != 3 ||
      records[6].sequence != 4) {
    return false;
  }

  record ranges_records[] = {
      {3, 0}, {2, 1}, {3, 2}, {1, 3}, {2, 4}, {1, 5},
  };

  auto result = tested::ranges::stable_sort(
      ranges_records, tested::ranges::less{}, &record::key);

  return result == ranges_records + 6 && ranges_records[0].key == 1 &&
         ranges_records[0].sequence == 3 && ranges_records[1].key == 1 &&
         ranges_records[1].sequence == 5 && ranges_records[2].key == 2 &&
         ranges_records[2].sequence == 1 && ranges_records[3].key == 2 &&
         ranges_records[3].sequence == 4 && ranges_records[4].key == 3 &&
         ranges_records[4].sequence == 0 && ranges_records[5].key == 3 &&
         ranges_records[5].sequence == 2;
}

bool move_only_sort_works() {
  move_only_value values[] = {
      move_only_value{5}, move_only_value{1}, move_only_value{4},
      move_only_value{2}, move_only_value{3},
  };

  tested::sort(values, values + 5, move_only_less{});

  if (values[0].value != 1 || values[1].value != 2 || values[2].value != 3 ||
      values[3].value != 4 || values[4].value != 5) {
    return false;
  }

  move_only_value stable_values[] = {
      move_only_value{4},
      move_only_value{1},
      move_only_value{3},
      move_only_value{2},
  };

  tested::ranges::stable_sort(stable_values, move_only_less{});

  return stable_values[0].value == 1 && stable_values[1].value == 2 &&
         stable_values[2].value == 3 && stable_values[3].value == 4;
}

bool large_sort_works() {
  int values[64];

  for (int index = 0; index < 64; ++index) {
    values[index] = 63 - index;
  }

  tested::ranges::sort(values);

  if (!tested::ranges::is_sorted(values)) {
    return false;
  }

  int equal_values[64];

  for (int &value : equal_values) {
    value = 7;
  }

  tested::ranges::sort(equal_values);

  return tested::ranges::is_sorted(equal_values);
}

bool policy_sort_works() {
  int inspected[] = {1, 2, 4, 3, 5};

  if (tested::is_sorted(tested::execution::seq, inspected, inspected + 5)) {
    return false;
  }

  if (tested::is_sorted_until(tested::execution::par, inspected,
                              inspected + 5) != inspected + 3) {
    return false;
  }

  int sorted[] = {5, 1, 4, 2, 3};

  tested::sort(tested::execution::par_unseq, sorted, sorted + 5);

  if (!tested::is_sorted(sorted, sorted + 5)) {
    return false;
  }

  record stable[] = {
      {2, 0},
      {1, 1},
      {2, 2},
      {1, 3},
  };

  tested::stable_sort(tested::execution::unseq, stable, stable + 4,
                      [](const record &left, const record &right) {
                        return left.key < right.key;
                      });

  return stable[0].key == 1 && stable[0].sequence == 1 && stable[1].key == 1 &&
         stable[1].sequence == 3 && stable[2].key == 2 &&
         stable[2].sequence == 0 && stable[3].key == 2 &&
         stable[3].sequence == 2;
}

bool ftl_test() {
  return inspection_works() && classic_sort_works() && ranges_sort_works() &&
         member_projection_sort_works() && stable_sort_works() &&
         move_only_sort_works() && large_sort_works() && policy_sort_works();
}
