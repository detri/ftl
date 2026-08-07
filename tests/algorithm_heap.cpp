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
  int values[5] = {1, 5, 4, 2, 3};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 5; }
};

template <class Comparator>
concept ranges_heap_accepts =
    requires(int *first, int *last, Comparator comparator) {
      {
        tested::ranges::push_heap(first, last, comparator)
      } -> tested::same_as<int *>;

      {
        tested::ranges::pop_heap(first, last, comparator)
      } -> tested::same_as<int *>;

      {
        tested::ranges::make_heap(first, last, comparator)
      } -> tested::same_as<int *>;

      {
        tested::ranges::sort_heap(first, last, comparator)
      } -> tested::same_as<int *>;

      {
        tested::ranges::is_heap(first, last, comparator)
      } -> tested::same_as<bool>;

      {
        tested::ranges::is_heap_until(first, last, comparator)
      } -> tested::same_as<int *>;
    };

static_assert(ranges_heap_accepts<descending>);

static_assert(!ranges_heap_accepts<void_comparator>);

template <class Policy>
concept policy_heap_inspection_accepts =
    requires(Policy &&policy, int *first, int *last) {
      {
        tested::is_heap(tested::forward<Policy>(policy), first, last)
      } -> tested::same_as<bool>;

      {
        tested::is_heap_until(tested::forward<Policy>(policy), first, last)
      } -> tested::same_as<int *>;
    };

static_assert(
    policy_heap_inspection_accepts<tested::execution::sequenced_policy>);

static_assert(
    policy_heap_inspection_accepts<tested::execution::parallel_policy>);

static_assert(policy_heap_inspection_accepts<
              tested::execution::parallel_unsequenced_policy>);

static_assert(
    policy_heap_inspection_accepts<tested::execution::unsequenced_policy>);

static_assert(policy_heap_inspection_accepts<
              const tested::execution::sequenced_policy &>);

static_assert(!policy_heap_inspection_accepts<int>);

template <class Policy>
concept policy_heap_mutation_accepts =
    requires(Policy &&policy, int *first, int *last) {
      tested::push_heap(tested::forward<Policy>(policy), first, last);

      tested::pop_heap(tested::forward<Policy>(policy), first, last);

      tested::make_heap(tested::forward<Policy>(policy), first, last);

      tested::sort_heap(tested::forward<Policy>(policy), first, last);
    };

static_assert(
    !policy_heap_mutation_accepts<tested::execution::sequenced_policy>);

static_assert(tested::is_object_v<decltype(tested::ranges::push_heap)>);

static_assert(tested::is_object_v<decltype(tested::ranges::pop_heap)>);

static_assert(tested::is_object_v<decltype(tested::ranges::make_heap)>);

static_assert(tested::is_object_v<decltype(tested::ranges::sort_heap)>);

static_assert(tested::is_object_v<decltype(tested::ranges::is_heap)>);

static_assert(tested::is_object_v<decltype(tested::ranges::is_heap_until)>);

static_assert(tested::is_same_v<decltype(tested::ranges::make_heap(
                                    tested::declval<temporary_range>())),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::push_heap(
                                    tested::declval<temporary_range>())),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::pop_heap(
                                    tested::declval<temporary_range>())),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::sort_heap(
                                    tested::declval<temporary_range>())),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::is_heap_until(
                                    tested::declval<temporary_range>())),
                                tested::ranges::dangling>);

constexpr bool classic_make_heap_works() {
  int values[] = {3, 1, 6, 5, 2, 4};

  tested::make_heap(values, values + 6);

  if (!tested::is_heap(values, values + 6)) {
    return false;
  }

  if (values[0] != 6) {
    return false;
  }

  int minimum_heap[] = {
      6, 4, 5, 1, 2, 3,
  };

  tested::make_heap(minimum_heap, minimum_heap + 6, descending{});

  return tested::is_heap(minimum_heap, minimum_heap + 6, descending{}) &&
         minimum_heap[0] == 1;
}

static_assert(classic_make_heap_works());

constexpr bool classic_push_heap_works() {
  int values[] = {
      9, 7, 8, 1, 4, 6, 10,
  };

  if (!tested::is_heap(values, values + 6)) {
    return false;
  }

  tested::push_heap(values, values + 7);

  return tested::is_heap(values, values + 7) && values[0] == 10;
}

static_assert(classic_push_heap_works());

constexpr bool classic_pop_heap_works() {
  int values[] = {
      10, 7, 9, 1, 4, 6,
  };

  tested::pop_heap(values, values + 6);

  return values[5] == 10 && tested::is_heap(values, values + 5);
}

static_assert(classic_pop_heap_works());

constexpr bool classic_sort_heap_works() {
  int values[] = {3, 1, 6, 5, 2, 4};

  tested::make_heap(values, values + 6);

  tested::sort_heap(values, values + 6);

  for (int index = 0; index < 6; ++index) {
    if (values[index] != index + 1) {
      return false;
    }
  }

  int descending_values[] = {
      3, 1, 6, 5, 2, 4,
  };

  tested::make_heap(descending_values, descending_values + 6, descending{});

  tested::sort_heap(descending_values, descending_values + 6, descending{});

  return descending_values[0] == 6 && descending_values[1] == 5 &&
         descending_values[2] == 4 && descending_values[3] == 3 &&
         descending_values[4] == 2 && descending_values[5] == 1;
}

static_assert(classic_sort_heap_works());

constexpr bool is_heap_until_works() {
  int valid[] = {
      9, 7, 8, 1, 4, 6,
  };

  if (tested::is_heap_until(valid, valid + 6) != valid + 6) {
    return false;
  }

  int invalid[] = {
      9, 7, 8, 10, 4, 6,
  };

  if (tested::is_heap_until(invalid, invalid + 6) != invalid + 3) {
    return false;
  }

  auto ranges_result = tested::ranges::is_heap_until(
      invalid, pointer_sentinel<int>{invalid + 6});

  return ranges_result == invalid + 3 && !tested::ranges::is_heap(invalid) &&
         tested::ranges::is_heap(valid);
}

static_assert(is_heap_until_works());

constexpr bool ranges_heap_works() {
  int values[] = {3, 1, 6, 5, 2, 4};

  auto make_end =
      tested::ranges::make_heap(values, pointer_sentinel<int>{values + 6});

  if (make_end != values + 6 || !tested::ranges::is_heap(values) ||
      values[0] != 6) {
    return false;
  }

  int pushed[] = {
      9, 7, 8, 1, 4, 6, 10,
  };

  auto push_end = tested::ranges::push_heap(pushed);

  if (push_end != pushed + 7 || pushed[0] != 10 ||
      !tested::ranges::is_heap(pushed)) {
    return false;
  }

  auto pop_end = tested::ranges::pop_heap(pushed);

  if (pop_end != pushed + 7 || pushed[6] != 10 ||
      !tested::ranges::is_heap(pushed, pushed + 6)) {
    return false;
  }

  auto sort_end = tested::ranges::sort_heap(pushed, pushed + 6);

  if (sort_end != pushed + 6) {
    return false;
  }

  for (int index = 1; index < 6; ++index) {
    if (pushed[index] < pushed[index - 1]) {
      return false;
    }
  }

  return true;
}

static_assert(ranges_heap_works());

constexpr bool projected_heap_works() {
  record values[] = {
      {3, 0}, {1, 1}, {6, 2}, {5, 3}, {2, 4}, {4, 5},
  };

  auto make_end = tested::ranges::make_heap(values, tested::ranges::less{},
                                            key_projection{});

  if (make_end != values + 6 || values[0].key != 6 ||
      !tested::ranges::is_heap(values, tested::ranges::less{},
                               key_projection{})) {
    return false;
  }

  auto pop_end = tested::ranges::pop_heap(values, tested::ranges::less{},
                                          key_projection{});

  if (pop_end != values + 6 || values[5].key != 6 ||
      !tested::ranges::is_heap(values, values + 5, tested::ranges::less{},
                               key_projection{})) {
    return false;
  }

  auto sort_end = tested::ranges::sort_heap(
      values, values + 5, tested::ranges::less{}, key_projection{});

  if (sort_end != values + 5) {
    return false;
  }

  for (int index = 1; index < 5; ++index) {
    if (values[index].key < values[index - 1].key) {
      return false;
    }
  }

  return true;
}

static_assert(projected_heap_works());

bool member_projection_works() {
  record values[] = {
      {3, 0}, {1, 1}, {6, 2}, {5, 3}, {2, 4}, {4, 5},
  };

  auto result =
      tested::ranges::make_heap(values, tested::ranges::less{}, &record::key);

  if (result != values + 6 || values[0].key != 6) {
    return false;
  }

  if (!tested::ranges::is_heap(values, tested::ranges::less{}, &record::key)) {
    return false;
  }

  tested::ranges::sort_heap(values, tested::ranges::less{}, &record::key);

  return values[0].key == 1 && values[1].key == 2 && values[2].key == 3 &&
         values[3].key == 4 && values[4].key == 5 && values[5].key == 6;
}

bool move_only_heap_works() {
  move_only_value values[] = {
      move_only_value{3}, move_only_value{1}, move_only_value{6},
      move_only_value{5}, move_only_value{2}, move_only_value{4},
  };

  auto make_end = tested::ranges::make_heap(values, move_only_less{});

  if (make_end != values + 6 || values[0].value != 6 ||
      !tested::ranges::is_heap(values, move_only_less{})) {
    return false;
  }

  auto pop_end = tested::ranges::pop_heap(values, move_only_less{});

  if (pop_end != values + 6 || values[5].value != 6 ||
      !tested::ranges::is_heap(values, values + 5, move_only_less{})) {
    return false;
  }

  tested::ranges::sort_heap(values, values + 5, move_only_less{});

  return values[0].value == 1 && values[1].value == 2 && values[2].value == 3 &&
         values[3].value == 4 && values[4].value == 5;
}

bool counted_calls_work() {
  record values[] = {
      {3, 0}, {1, 1}, {6, 2}, {5, 3}, {2, 4}, {4, 5},
  };

  int comparator_calls = 0;
  int projection_calls = 0;

  auto result =
      tested::ranges::make_heap(values, counting_less{&comparator_calls},
                                counting_projection{&projection_calls});

  return result == values + 6 && comparator_calls > 0 &&
         projection_calls == comparator_calls * 2;
}

bool policy_heap_inspection_works() {
  int valid[] = {
      9, 7, 8, 1, 4, 6,
  };

  int invalid[] = {
      9, 7, 8, 10, 4, 6,
  };

  if (!tested::is_heap(tested::execution::seq, valid, valid + 6)) {
    return false;
  }

  if (tested::is_heap(tested::execution::par, invalid, invalid + 6)) {
    return false;
  }

  if (tested::is_heap_until(tested::execution::par_unseq, invalid,
                            invalid + 6) != invalid + 3) {
    return false;
  }

  return tested::is_heap_until(tested::execution::unseq, valid, valid + 6) ==
         valid + 6;
}

bool large_heap_works() {
  int values[64];

  for (int index = 0; index < 64; ++index) {
    values[index] = (index * 37 + 11) % 64;
  }

  tested::ranges::make_heap(values);

  if (!tested::ranges::is_heap(values)) {
    return false;
  }

  tested::ranges::sort_heap(values);

  for (int index = 0; index < 64; ++index) {
    if (values[index] != index) {
      return false;
    }
  }

  return true;
}

bool ftl_test() {
  return classic_make_heap_works() && classic_push_heap_works() &&
         classic_pop_heap_works() && classic_sort_heap_works() &&
         is_heap_until_works() && ranges_heap_works() &&
         projected_heap_works() && member_projection_works() &&
         move_only_heap_works() && counted_calls_work() &&
         policy_heap_inspection_works() && large_heap_works();
}
