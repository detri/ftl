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

struct same_parity {
  constexpr bool operator()(int left, int right) const noexcept {
    return left % 2 == right % 2;
  }
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

struct temporary_range {
  int values[4] = {1, 1, 2, 2};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 4; }
};

template <class Iterator>
concept ranges_remove_accepts = requires(Iterator first, Iterator last) {
  tested::ranges::remove(first, last, 2);

  tested::ranges::remove_if(first, last, even{});

  tested::ranges::unique(first, last);
};

static_assert(ranges_remove_accepts<int *>);
static_assert(!ranges_remove_accepts<const int *>);

template <class Predicate>
concept ranges_remove_copy_if_accepts =
    requires(int *first, int *last, int *output, Predicate predicate) {
      tested::ranges::remove_copy_if(first, last, output, predicate);
    };

static_assert(ranges_remove_copy_if_accepts<even>);

static_assert(!ranges_remove_copy_if_accepts<void_predicate>);

template <class Policy>
concept policy_remove_unique_accepts = requires(Policy &&policy, int *first,
                                                int *last, int *output) {
  {
    tested::remove(tested::forward<Policy>(policy), first, last, 2)
  } -> tested::same_as<int *>;

  {
    tested::remove_if(tested::forward<Policy>(policy), first, last, even{})
  } -> tested::same_as<int *>;

  {
    tested::remove_copy(tested::forward<Policy>(policy), first, last, output, 2)
  } -> tested::same_as<int *>;

  {
    tested::remove_copy_if(tested::forward<Policy>(policy), first, last, output,
                           even{})
  } -> tested::same_as<int *>;

  {
    tested::unique(tested::forward<Policy>(policy), first, last)
  } -> tested::same_as<int *>;

  {
    tested::unique_copy(tested::forward<Policy>(policy), first, last, output)
  } -> tested::same_as<int *>;
};

static_assert(
    policy_remove_unique_accepts<tested::execution::sequenced_policy>);

static_assert(policy_remove_unique_accepts<tested::execution::parallel_policy>);

static_assert(policy_remove_unique_accepts<
              tested::execution::parallel_unsequenced_policy>);

static_assert(
    policy_remove_unique_accepts<tested::execution::unsequenced_policy>);

static_assert(
    policy_remove_unique_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_remove_unique_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::remove)>);

static_assert(tested::is_object_v<decltype(tested::ranges::remove_if)>);

static_assert(tested::is_object_v<decltype(tested::ranges::remove_copy)>);

static_assert(tested::is_object_v<decltype(tested::ranges::remove_copy_if)>);

static_assert(tested::is_object_v<decltype(tested::ranges::unique)>);

static_assert(tested::is_object_v<decltype(tested::ranges::unique_copy)>);

static_assert(
    tested::is_same_v<tested::ranges::remove_copy_result<int *, int *>,
                      tested::ranges::in_out_result<int *, int *>>);

static_assert(
    tested::is_same_v<tested::ranges::remove_copy_if_result<int *, int *>,
                      tested::ranges::in_out_result<int *, int *>>);

static_assert(
    tested::is_same_v<tested::ranges::unique_copy_result<int *, int *>,
                      tested::ranges::in_out_result<int *, int *>>);

static_assert(tested::is_same_v<decltype(tested::ranges::remove(
                                    tested::declval<temporary_range>(), 2)),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::unique(
                                    tested::declval<temporary_range>())),
                                tested::ranges::dangling>);

static_assert(
    tested::is_same_v<
        decltype(tested::ranges::remove_copy(tested::declval<temporary_range>(),
                                             tested::declval<int *>(), 2)),
        tested::ranges::remove_copy_result<tested::ranges::dangling, int *>>);

static_assert(
    tested::is_same_v<
        decltype(tested::ranges::unique_copy(tested::declval<temporary_range>(),
                                             tested::declval<int *>())),
        tested::ranges::unique_copy_result<tested::ranges::dangling, int *>>);

constexpr bool classic_remove_works() {
  int values[] = {1, 2, 3, 2, 4};

  auto new_end = tested::remove(values, values + 5, 2);

  if (new_end != values + 3 || values[0] != 1 || values[1] != 3 ||
      values[2] != 4) {
    return false;
  }

  int conditional[] = {1, 2, 3, 4, 5};

  auto conditional_end =
      tested::remove_if(conditional, conditional + 5, even{});

  return conditional_end == conditional + 3 && conditional[0] == 1 &&
         conditional[1] == 3 && conditional[2] == 5;
}

static_assert(classic_remove_works());

constexpr bool classic_remove_copy_works() {
  int source[] = {1, 2, 3, 2, 4};
  int output[5] = {};

  auto output_end = tested::remove_copy(source, source + 5, output, 2);

  if (output_end != output + 3 || output[0] != 1 || output[1] != 3 ||
      output[2] != 4) {
    return false;
  }

  int conditional[5] = {};

  auto conditional_end =
      tested::remove_copy_if(source, source + 5, conditional, even{});

  return conditional_end == conditional + 2 && conditional[0] == 1 &&
         conditional[1] == 3 && source[0] == 1 && source[1] == 2 &&
         source[2] == 3 && source[3] == 2 && source[4] == 4;
}

static_assert(classic_remove_copy_works());

constexpr bool ranges_remove_works() {
  int values[] = {1, 2, 3, 2, 4};

  auto result =
      tested::ranges::remove(values, pointer_sentinel<int>{values + 5}, 2);

  if (result.begin() != values + 3 || result.end() != values + 5) {
    return false;
  }

  if (values[0] != 1 || values[1] != 3 || values[2] != 4) {
    return false;
  }

  int conditional[] = {1, 2, 3, 4, 5};

  auto conditional_result = tested::ranges::remove_if(conditional, even{});

  return conditional_result.begin() == conditional + 3 &&
         conditional_result.end() == conditional + 5 && conditional[0] == 1 &&
         conditional[1] == 3 && conditional[2] == 5;
}

static_assert(ranges_remove_works());

constexpr bool projected_remove_works() {
  record values[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {2, 40},
  };

  auto result = tested::ranges::remove(values, 2, key_projection{});

  if (result.begin() != values + 2 || result.end() != values + 4) {
    return false;
  }

  if (values[0].key != 1 || values[0].payload != 10 || values[1].key != 3 ||
      values[1].payload != 30) {
    return false;
  }

  record conditional[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {4, 40},
  };

  auto conditional_result =
      tested::ranges::remove_if(conditional, even{}, key_projection{});

  return conditional_result.begin() == conditional + 2 &&
         conditional_result.end() == conditional + 4 &&
         conditional[0].key == 1 && conditional[0].payload == 10 &&
         conditional[1].key == 3 && conditional[1].payload == 30;
}

static_assert(projected_remove_works());

constexpr bool ranges_remove_copy_works() {
  int source[] = {1, 2, 3, 2, 4};
  int output[5] = {};

  auto result = tested::ranges::remove_copy(
      source, pointer_sentinel<int>{source + 5}, output, 2);

  if (result.in != source + 5 || result.out != output + 3 || output[0] != 1 ||
      output[1] != 3 || output[2] != 4) {
    return false;
  }

  int conditional[5] = {};

  auto conditional_result =
      tested::ranges::remove_copy_if(source, conditional, even{});

  return conditional_result.in == source + 5 &&
         conditional_result.out == conditional + 2 && conditional[0] == 1 &&
         conditional[1] == 3;
}

static_assert(ranges_remove_copy_works());

constexpr bool projected_remove_copy_works() {
  record source[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {4, 40},
  };

  record output[4] = {};

  int projection_calls = 0;
  int predicate_calls = 0;

  auto result = tested::ranges::remove_copy_if(
      source, output, counting_even{&predicate_calls},
      counting_projection{&projection_calls});

  return result.in == source + 4 && result.out == output + 2 &&
         projection_calls == 4 && predicate_calls == 4 && output[0].key == 1 &&
         output[0].payload == 10 && output[1].key == 3 &&
         output[1].payload == 30;
}

static_assert(projected_remove_copy_works());

constexpr bool classic_unique_works() {
  int values[] = {
      1, 1, 2, 2, 2, 3, 1, 1,
  };

  auto new_end = tested::unique(values, values + 8);

  if (new_end != values + 4 || values[0] != 1 || values[1] != 2 ||
      values[2] != 3 || values[3] != 1) {
    return false;
  }

  int parity_values[] = {1, 3, 2, 4, 5};

  auto parity_end =
      tested::unique(parity_values, parity_values + 5, same_parity{});

  return parity_end == parity_values + 3 && parity_values[0] == 1 &&
         parity_values[1] == 2 && parity_values[2] == 5;
}

static_assert(classic_unique_works());

constexpr bool ranges_unique_works() {
  int values[] = {
      1, 1, 2, 2, 2, 3, 1, 1,
  };

  auto result =
      tested::ranges::unique(values, pointer_sentinel<int>{values + 8});

  if (result.begin() != values + 4 || result.end() != values + 8) {
    return false;
  }

  return values[0] == 1 && values[1] == 2 && values[2] == 3 && values[3] == 1;
}

static_assert(ranges_unique_works());

constexpr bool projected_unique_works() {
  record values[] = {
      {1, 10}, {1, 20}, {2, 30}, {2, 40}, {3, 50},
  };

  auto result = tested::ranges::unique(values, tested::ranges::equal_to{},
                                       key_projection{});

  return result.begin() == values + 3 && result.end() == values + 5 &&
         values[0].key == 1 && values[0].payload == 10 && values[1].key == 2 &&
         values[1].payload == 30 && values[2].key == 3 &&
         values[2].payload == 50;
}

static_assert(projected_unique_works());

constexpr bool classic_unique_copy_works() {
  int source[] = {
      1, 1, 2, 2, 2, 3, 1, 1,
  };

  int output[8] = {};

  auto output_end = tested::unique_copy(source, source + 8, output);

  if (output_end != output + 4 || output[0] != 1 || output[1] != 2 ||
      output[2] != 3 || output[3] != 1) {
    return false;
  }

  int parity_source[] = {1, 3, 2, 4, 5};
  int parity_output[5] = {};

  auto parity_end = tested::unique_copy(parity_source, parity_source + 5,
                                        parity_output, same_parity{});

  return parity_end == parity_output + 3 && parity_output[0] == 1 &&
         parity_output[1] == 2 && parity_output[2] == 5;
}

static_assert(classic_unique_copy_works());

constexpr bool ranges_unique_copy_works() {
  int source[] = {
      1, 1, 2, 2, 2, 3, 1, 1,
  };

  int output[8] = {};

  auto result = tested::ranges::unique_copy(
      source, pointer_sentinel<int>{source + 8}, output);

  return result.in == source + 8 && result.out == output + 4 &&
         output[0] == 1 && output[1] == 2 && output[2] == 3 && output[3] == 1;
}

static_assert(ranges_unique_copy_works());

constexpr bool single_pass_unique_copy_works() {
  int source[] = {
      1, 1, 2, 2, 3, 3, 1,
  };

  int output[7] = {};

  auto result = tested::ranges::unique_copy(input_cursor{source},
                                            input_cursor{source + 7}, output);

  return result.in.current == source + 7 && result.out == output + 4 &&
         output[0] == 1 && output[1] == 2 && output[2] == 3 && output[3] == 1;
}

static_assert(single_pass_unique_copy_works());

constexpr bool projected_unique_copy_works() {
  record source[] = {
      {1, 10}, {1, 20}, {2, 30}, {2, 40}, {3, 50},
  };

  record output[5] = {};

  auto result = tested::ranges::unique_copy(
      source, output, tested::ranges::equal_to{}, key_projection{});

  return result.in == source + 5 && result.out == output + 3 &&
         output[0].key == 1 && output[0].payload == 10 && output[1].key == 2 &&
         output[1].payload == 30 && output[2].key == 3 &&
         output[2].payload == 50;
}

static_assert(projected_unique_copy_works());

bool member_projection_works() {
  record values[] = {
      {1, 10},
      {2, 20},
      {2, 30},
      {3, 40},
  };

  auto removed = tested::ranges::remove(values, 2, &record::key);

  if (removed.begin() != values + 2 || removed.end() != values + 4 ||
      values[0].key != 1 || values[1].key != 3) {
    return false;
  }

  record source[] = {
      {1, 10},
      {1, 20},
      {2, 30},
      {2, 40},
  };

  record output[4] = {};

  auto copied = tested::ranges::unique_copy(
      source, output, tested::ranges::equal_to{}, &record::key);

  return copied.in == source + 4 && copied.out == output + 2 &&
         output[0].key == 1 && output[0].payload == 10 && output[1].key == 2 &&
         output[1].payload == 30;
}

bool policy_remove_unique_works() {
  int removed[] = {1, 2, 3, 2, 4};
  int removed_if[] = {1, 2, 3, 4, 5};
  int source[] = {1, 2, 3, 2, 4};
  int copied[5] = {};
  int copied_if[5] = {};

  int duplicates[] = {
      1, 1, 2, 2, 3, 3,
  };

  int unique_output[6] = {};

  auto removed_end =
      tested::remove(tested::execution::seq, removed, removed + 5, 2);

  auto removed_if_end = tested::remove_if(tested::execution::par, removed_if,
                                          removed_if + 5, even{});

  auto copied_end = tested::remove_copy(tested::execution::par_unseq, source,
                                        source + 5, copied, 2);

  auto copied_if_end = tested::remove_copy_if(tested::execution::unseq, source,
                                              source + 5, copied_if, even{});

  auto unique_end =
      tested::unique(tested::execution::seq, duplicates, duplicates + 6);

  auto unique_output_end = tested::unique_copy(tested::execution::par, source,
                                               source + 5, unique_output);

  return removed_end == removed + 3 && removed[0] == 1 && removed[1] == 3 &&
         removed[2] == 4 && removed_if_end == removed_if + 3 &&
         removed_if[0] == 1 && removed_if[1] == 3 && removed_if[2] == 5 &&
         copied_end == copied + 3 && copied[0] == 1 && copied[1] == 3 &&
         copied[2] == 4 && copied_if_end == copied_if + 2 &&
         copied_if[0] == 1 && copied_if[1] == 3 &&
         unique_end == duplicates + 3 && duplicates[0] == 1 &&
         duplicates[1] == 2 && duplicates[2] == 3 &&
         unique_output_end == unique_output + 5 && unique_output[0] == 1 &&
         unique_output[1] == 2 && unique_output[2] == 3 &&
         unique_output[3] == 2 && unique_output[4] == 4;
}

bool ftl_test() {
  return classic_remove_works() && classic_remove_copy_works() &&
         ranges_remove_works() && projected_remove_works() &&
         ranges_remove_copy_works() && projected_remove_copy_works() &&
         classic_unique_works() && ranges_unique_works() &&
         projected_unique_works() && classic_unique_copy_works() &&
         ranges_unique_copy_works() && single_pass_unique_copy_works() &&
         projected_unique_copy_works() && member_projection_works() &&
         policy_remove_unique_works();
}
