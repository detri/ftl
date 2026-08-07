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

struct even {
  constexpr bool operator()(int value) const noexcept { return value % 2 == 0; }
};

struct positive {
  constexpr bool operator()(int value) const noexcept { return value > 0; }
};

struct void_predicate {
  constexpr void operator()(int) const noexcept {}
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

static_assert(!tested::bidirectional_iterator<forward_cursor>);

static_assert(tested::permutable<forward_cursor>);

struct record {
  int key = 0;
  int payload = 0;
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

struct counting_even {
  int *calls;

  constexpr bool operator()(int value) const noexcept {
    ++*calls;
    return value % 2 == 0;
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

struct move_only_even {
  constexpr bool operator()(const move_only_value &value) const noexcept {
    return value.value % 2 == 0;
  }
};

struct temporary_range {
  int values[6] = {1, 2, 3, 4, 5, 6};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 6; }
};

template <class Predicate>
concept ranges_partition_accepts = requires(int *first, int *last, int *output1,
                                            int *output2, Predicate predicate) {
  {
    tested::ranges::is_partitioned(first, last, predicate)
  } -> tested::same_as<bool>;

  tested::ranges::partition(first, last, predicate);

  tested::ranges::stable_partition(first, last, predicate);

  tested::ranges::partition_copy(first, last, output1, output2, predicate);

  {
    tested::ranges::partition_point(first, last, predicate)
  } -> tested::same_as<int *>;
};

static_assert(ranges_partition_accepts<even>);
static_assert(!ranges_partition_accepts<void_predicate>);

template <class Policy>
concept policy_partition_accepts = requires(
    Policy &&policy, int *first, int *last, int *output1, int *output2) {
  {
    tested::is_partitioned(tested::forward<Policy>(policy), first, last, even{})
  } -> tested::same_as<bool>;

  {
    tested::partition(tested::forward<Policy>(policy), first, last, even{})
  } -> tested::same_as<int *>;

  {
    tested::stable_partition(tested::forward<Policy>(policy), first, last,
                             even{})
  } -> tested::same_as<int *>;

  {
    tested::partition_copy(tested::forward<Policy>(policy), first, last,
                           output1, output2, even{})
  } -> tested::same_as<tested::pair<int *, int *>>;
};

static_assert(policy_partition_accepts<tested::execution::sequenced_policy>);

static_assert(policy_partition_accepts<tested::execution::parallel_policy>);

static_assert(
    policy_partition_accepts<tested::execution::parallel_unsequenced_policy>);

static_assert(policy_partition_accepts<tested::execution::unsequenced_policy>);

static_assert(
    policy_partition_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_partition_accepts<int>);

template <class Policy>
concept policy_partition_point_accepts = requires(Policy &&policy, int *first,
                                                  int *last) {
  tested::partition_point(tested::forward<Policy>(policy), first, last, even{});
};

static_assert(
    !policy_partition_point_accepts<tested::execution::sequenced_policy>);

static_assert(tested::is_object_v<decltype(tested::ranges::is_partitioned)>);

static_assert(tested::is_object_v<decltype(tested::ranges::partition)>);

static_assert(tested::is_object_v<decltype(tested::ranges::stable_partition)>);

static_assert(tested::is_object_v<decltype(tested::ranges::partition_copy)>);

static_assert(tested::is_object_v<decltype(tested::ranges::partition_point)>);

static_assert(tested::is_same_v<
              tested::ranges::partition_copy_result<int *, int *, int *>,
              tested::ranges::in_out_out_result<int *, int *, int *>>);

static_assert(
    tested::is_same_v<decltype(tested::ranges::partition(
                          tested::declval<temporary_range>(), even{})),
                      tested::ranges::dangling>);

static_assert(
    tested::is_same_v<decltype(tested::ranges::stable_partition(
                          tested::declval<temporary_range>(), even{})),
                      tested::ranges::dangling>);

static_assert(
    tested::is_same_v<decltype(tested::ranges::partition_point(
                          tested::declval<temporary_range>(), even{})),
                      tested::ranges::dangling>);

static_assert(tested::is_same_v<
              decltype(tested::ranges::partition_copy(
                  tested::declval<temporary_range>(), tested::declval<int *>(),
                  tested::declval<int *>(), even{})),
              tested::ranges::partition_copy_result<tested::ranges::dangling,
                                                    int *, int *>>);

constexpr bool classic_is_partitioned_works() {
  int partitioned[] = {2, 4, 6, 1, 3, 5};
  int not_partitioned[] = {2, 1, 4, 3};

  return tested::is_partitioned(partitioned, partitioned + 6, even{}) &&
         !tested::is_partitioned(not_partitioned, not_partitioned + 4, even{});
}

static_assert(classic_is_partitioned_works());

constexpr bool classic_partition_works() {
  int values[] = {1, 2, 3, 4, 5, 6};

  auto point = tested::partition(values, values + 6, even{});

  return point == values + 3 && values[0] == 2 && values[1] == 4 &&
         values[2] == 6 && values[3] == 1 && values[4] == 5 && values[5] == 3 &&
         tested::is_partitioned(values, values + 6, even{});
}

static_assert(classic_partition_works());

constexpr bool ranges_partition_works() {
  int values[] = {1, 2, 3, 4, 5, 6};

  auto result = tested::ranges::partition(
      values, pointer_sentinel<int>{values + 6}, even{});

  if (result.begin() != values + 3 || result.end() != values + 6) {
    return false;
  }

  if (!tested::ranges::is_partitioned(values, even{})) {
    return false;
  }

  int forward_values[] = {1, 2, 3, 4, 5, 6};

  auto forward_result =
      tested::ranges::partition(forward_cursor{forward_values},
                                forward_cursor{forward_values + 6}, even{});

  return forward_result.begin().current == forward_values + 3 &&
         forward_result.end().current == forward_values + 6 &&
         tested::ranges::is_partitioned(forward_cursor{forward_values},
                                        forward_cursor{forward_values + 6},
                                        even{});
}

static_assert(ranges_partition_works());

constexpr bool projected_partition_works() {
  record values[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {4, 40},
  };

  int predicate_calls = 0;
  int projection_calls = 0;

  auto result =
      tested::ranges::partition(values, counting_even{&predicate_calls},
                                counting_projection{&projection_calls});

  return result.begin() == values + 2 && result.end() == values + 4 &&
         predicate_calls == 4 && projection_calls == 4 && values[0].key == 2 &&
         values[1].key == 4 &&
         tested::ranges::is_partitioned(values, even{}, key_projection{});
}

static_assert(projected_partition_works());

bool stable_partition_works() {
  int classic_values[] = {
      1, 2, 3, 4, 5, 6,
  };

  auto classic_point =
      tested::stable_partition(classic_values, classic_values + 6, even{});

  if (classic_point != classic_values + 3 || classic_values[0] != 2 ||
      classic_values[1] != 4 || classic_values[2] != 6 ||
      classic_values[3] != 1 || classic_values[4] != 3 ||
      classic_values[5] != 5) {
    return false;
  }

  record records[] = {
      {1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60},
  };

  auto ranges_result =
      tested::ranges::stable_partition(records, even{}, &record::key);

  return ranges_result.begin() == records + 3 &&
         ranges_result.end() == records + 6 && records[0].key == 2 &&
         records[0].payload == 20 && records[1].key == 4 &&
         records[1].payload == 40 && records[2].key == 6 &&
         records[2].payload == 60 && records[3].key == 1 &&
         records[3].payload == 10 && records[4].key == 3 &&
         records[4].payload == 30 && records[5].key == 5 &&
         records[5].payload == 50;
}

constexpr bool partition_copy_works() {
  int source[] = {1, 2, 3, 4, 5, 6};
  int true_output[6] = {};
  int false_output[6] = {};

  auto classic_result = tested::partition_copy(source, source + 6, true_output,
                                               false_output, even{});

  if (classic_result.first != true_output + 3 ||
      classic_result.second != false_output + 3 || true_output[0] != 2 ||
      true_output[1] != 4 || true_output[2] != 6 || false_output[0] != 1 ||
      false_output[1] != 3 || false_output[2] != 5) {
    return false;
  }

  record records[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {4, 40},
  };

  record true_records[4] = {};
  record false_records[4] = {};

  auto ranges_result = tested::ranges::partition_copy(
      records, true_records, false_records, even{}, key_projection{});

  return ranges_result.in == records + 4 &&
         ranges_result.out1 == true_records + 2 &&
         ranges_result.out2 == false_records + 2 && true_records[0].key == 2 &&
         true_records[0].payload == 20 && true_records[1].key == 4 &&
         true_records[1].payload == 40 && false_records[0].key == 1 &&
         false_records[0].payload == 10 && false_records[1].key == 3 &&
         false_records[1].payload == 30;
}

static_assert(partition_copy_works());

constexpr bool partition_point_works() {
  int values[] = {2, 4, 6, 1, 3, 5};

  if (tested::partition_point(values, values + 6, even{}) != values + 3) {
    return false;
  }

  if (tested::ranges::partition_point(values, pointer_sentinel<int>{values + 6},
                                      even{}) != values + 3) {
    return false;
  }

  auto forward_result = tested::ranges::partition_point(
      forward_cursor{values}, forward_cursor{values + 6}, even{});

  if (forward_result.current != values + 3) {
    return false;
  }

  int all_true[] = {2, 4, 6};

  if (tested::ranges::partition_point(all_true, even{}) != all_true + 3) {
    return false;
  }

  int all_false[] = {1, 3, 5};

  return tested::ranges::partition_point(all_false, even{}) == all_false;
}

static_assert(partition_point_works());

bool move_only_partition_works() {
  move_only_value values[] = {
      move_only_value{1},
      move_only_value{2},
      move_only_value{3},
      move_only_value{4},
  };

  auto result = tested::ranges::partition(values, move_only_even{});

  if (result.begin() != values + 2 || result.end() != values + 4 ||
      !tested::ranges::is_partitioned(values, move_only_even{})) {
    return false;
  }

  move_only_value stable_values[] = {
      move_only_value{1},
      move_only_value{2},
      move_only_value{3},
      move_only_value{4},
  };

  auto stable_result =
      tested::ranges::stable_partition(stable_values, move_only_even{});

  return stable_result.begin() == stable_values + 2 &&
         stable_result.end() == stable_values + 4 &&
         stable_values[0].value == 2 && stable_values[1].value == 4 &&
         stable_values[2].value == 1 && stable_values[3].value == 3;
}

bool policy_partition_works() {
  int checked[] = {2, 4, 6, 1, 3, 5};

  if (!tested::is_partitioned(tested::execution::seq, checked, checked + 6,
                              even{})) {
    return false;
  }

  int partitioned[] = {1, 2, 3, 4, 5, 6};

  auto partition_point = tested::partition(tested::execution::par, partitioned,
                                           partitioned + 6, even{});

  if (partition_point != partitioned + 3 ||
      !tested::is_partitioned(partitioned, partitioned + 6, even{})) {
    return false;
  }

  int stable[] = {1, 2, 3, 4, 5, 6};

  auto stable_point = tested::stable_partition(tested::execution::par_unseq,
                                               stable, stable + 6, even{});

  if (stable_point != stable + 3 || stable[0] != 2 || stable[1] != 4 ||
      stable[2] != 6 || stable[3] != 1 || stable[4] != 3 || stable[5] != 5) {
    return false;
  }

  int source[] = {1, 2, 3, 4, 5, 6};
  int true_output[6] = {};
  int false_output[6] = {};

  auto copy_result =
      tested::partition_copy(tested::execution::unseq, source, source + 6,
                             true_output, false_output, even{});

  return copy_result.first == true_output + 3 &&
         copy_result.second == false_output + 3 && true_output[0] == 2 &&
         true_output[1] == 4 && true_output[2] == 6 && false_output[0] == 1 &&
         false_output[1] == 3 && false_output[2] == 5;
}

bool ftl_test() {
  return classic_is_partitioned_works() && classic_partition_works() &&
         ranges_partition_works() && projected_partition_works() &&
         stable_partition_works() && partition_copy_works() &&
         partition_point_works() && move_only_partition_works() &&
         policy_partition_works();
}
