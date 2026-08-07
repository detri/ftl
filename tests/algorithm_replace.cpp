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

struct even {
  constexpr bool operator()(int value) const noexcept { return value % 2 == 0; }
};

struct greater_than_two {
  constexpr bool operator()(int value) const noexcept { return value > 2; }
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

struct temporary_range {
  int values[3] = {1, 2, 3};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 3; }
};

template <class Iterator>
concept ranges_replace_accepts = requires(Iterator first, Iterator last) {
  { tested::ranges::replace(first, last, 2, 9) } -> tested::same_as<Iterator>;

  {
    tested::ranges::replace_if(first, last, even{}, 9)
  } -> tested::same_as<Iterator>;
};

static_assert(ranges_replace_accepts<int *>);
static_assert(!ranges_replace_accepts<const int *>);

template <class Predicate>
concept ranges_replace_if_accepts =
    requires(int *first, int *last, Predicate predicate) {
      tested::ranges::replace_if(first, last, predicate, 9);

      tested::ranges::replace_copy_if(first, last, first, predicate, 9);
    };

static_assert(ranges_replace_if_accepts<even>);
static_assert(!ranges_replace_if_accepts<void_predicate>);

template <class Policy>
concept policy_replace_accepts = requires(Policy &&policy, int *first,
                                          int *last, int *output) {
  {
    tested::replace(tested::forward<Policy>(policy), first, last, 2, 9)
  } -> tested::same_as<void>;

  {
    tested::replace_if(tested::forward<Policy>(policy), first, last, even{}, 9)
  } -> tested::same_as<void>;

  {
    tested::replace_copy(tested::forward<Policy>(policy), first, last, output,
                         2, 9)
  } -> tested::same_as<int *>;

  {
    tested::replace_copy_if(tested::forward<Policy>(policy), first, last,
                            output, even{}, 9)
  } -> tested::same_as<int *>;
};

static_assert(policy_replace_accepts<tested::execution::sequenced_policy>);

static_assert(policy_replace_accepts<tested::execution::parallel_policy>);

static_assert(
    policy_replace_accepts<tested::execution::parallel_unsequenced_policy>);

static_assert(policy_replace_accepts<tested::execution::unsequenced_policy>);

static_assert(
    policy_replace_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_replace_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::replace)>);

static_assert(tested::is_object_v<decltype(tested::ranges::replace_if)>);

static_assert(tested::is_object_v<decltype(tested::ranges::replace_copy)>);

static_assert(tested::is_object_v<decltype(tested::ranges::replace_copy_if)>);

static_assert(
    tested::is_same_v<tested::ranges::replace_copy_result<int *, int *>,
                      tested::ranges::in_out_result<int *, int *>>);

static_assert(
    tested::is_same_v<tested::ranges::replace_copy_if_result<int *, int *>,
                      tested::ranges::in_out_result<int *, int *>>);

static_assert(tested::is_same_v<decltype(tested::ranges::replace(
                                    tested::declval<temporary_range>(), 2, 9)),
                                tested::ranges::dangling>);

static_assert(
    tested::is_same_v<decltype(tested::ranges::replace_if(
                          tested::declval<temporary_range>(), even{}, 9)),
                      tested::ranges::dangling>);

static_assert(
    tested::is_same_v<
        decltype(tested::ranges::replace_copy(
            tested::declval<temporary_range>(), tested::declval<int *>(), 2,
            9)),
        tested::ranges::replace_copy_result<tested::ranges::dangling, int *>>);

static_assert(tested::is_same_v<decltype(tested::ranges::replace_copy_if(
                                    tested::declval<temporary_range>(),
                                    tested::declval<int *>(), even{}, 9)),
                                tested::ranges::replace_copy_if_result<
                                    tested::ranges::dangling, int *>>);

constexpr bool classic_replace_works() {
  int values[] = {1, 2, 3, 2, 4};

  tested::replace(values, values + 5, 2, 9);

  if (values[0] != 1 || values[1] != 9 || values[2] != 3 || values[3] != 9 ||
      values[4] != 4) {
    return false;
  }

  tested::replace_if(values, values + 5, even{}, 0);

  return values[0] == 1 && values[1] == 9 && values[2] == 3 && values[3] == 9 &&
         values[4] == 0;
}

static_assert(classic_replace_works());

constexpr bool classic_replace_copy_works() {
  int source[] = {1, 2, 3, 2, 4};
  int replaced[5] = {};

  auto first_end = tested::replace_copy(source, source + 5, replaced, 2, 9);

  if (first_end != replaced + 5) {
    return false;
  }

  if (replaced[0] != 1 || replaced[1] != 9 || replaced[2] != 3 ||
      replaced[3] != 9 || replaced[4] != 4) {
    return false;
  }

  int conditional[5] = {};

  auto second_end =
      tested::replace_copy_if(source, source + 5, conditional, even{}, 0);

  if (second_end != conditional + 5) {
    return false;
  }

  return conditional[0] == 1 && conditional[1] == 0 && conditional[2] == 3 &&
         conditional[3] == 0 && conditional[4] == 0 && source[0] == 1 &&
         source[1] == 2 && source[2] == 3 && source[3] == 2 && source[4] == 4;
}

static_assert(classic_replace_copy_works());

constexpr bool ranges_replace_works() {
  int values[] = {1, 2, 3, 2, 4};

  auto first_end =
      tested::ranges::replace(values, pointer_sentinel<int>{values + 5}, 2, 9);

  if (first_end != values + 5) {
    return false;
  }

  if (values[0] != 1 || values[1] != 9 || values[2] != 3 || values[3] != 9 ||
      values[4] != 4) {
    return false;
  }

  int conditional[] = {1, 2, 3, 4, 5};

  auto second_end =
      tested::ranges::replace_if(conditional, greater_than_two{}, 0);

  return second_end == conditional + 5 && conditional[0] == 1 &&
         conditional[1] == 2 && conditional[2] == 0 && conditional[3] == 0 &&
         conditional[4] == 0;
}

static_assert(ranges_replace_works());

constexpr bool ranges_projected_replace_works() {
  record values[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {2, 40},
  };

  auto first_end =
      tested::ranges::replace(values, 2, record{9, 90}, key_projection{});

  if (first_end != values + 4) {
    return false;
  }

  if (values[0].key != 1 || values[0].payload != 10 || values[1].key != 9 ||
      values[1].payload != 90 || values[2].key != 3 ||
      values[2].payload != 30 || values[3].key != 9 ||
      values[3].payload != 90) {
    return false;
  }

  record conditional[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {4, 40},
  };

  auto second_end = tested::ranges::replace_if(
      conditional, even{}, record{0, 100}, key_projection{});

  return second_end == conditional + 4 && conditional[0].key == 1 &&
         conditional[0].payload == 10 && conditional[1].key == 0 &&
         conditional[1].payload == 100 && conditional[2].key == 3 &&
         conditional[2].payload == 30 && conditional[3].key == 0 &&
         conditional[3].payload == 100;
}

static_assert(ranges_projected_replace_works());

constexpr bool ranges_replace_copy_works() {
  int source[] = {1, 2, 3, 2, 4};
  int replaced[5] = {};

  auto first_result = tested::ranges::replace_copy(
      source, pointer_sentinel<int>{source + 5}, replaced, 2, 9);

  if (first_result.in != source + 5 || first_result.out != replaced + 5) {
    return false;
  }

  if (replaced[0] != 1 || replaced[1] != 9 || replaced[2] != 3 ||
      replaced[3] != 9 || replaced[4] != 4) {
    return false;
  }

  int conditional[5] = {};

  auto second_result =
      tested::ranges::replace_copy_if(source, conditional, even{}, 0);

  return second_result.in == source + 5 &&
         second_result.out == conditional + 5 && conditional[0] == 1 &&
         conditional[1] == 0 && conditional[2] == 3 && conditional[3] == 0 &&
         conditional[4] == 0;
}

static_assert(ranges_replace_copy_works());

constexpr bool projected_replace_copy_works() {
  record source[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {2, 40},
  };

  record replaced[4] = {};

  auto first_result = tested::ranges::replace_copy(
      source, replaced, 2, record{9, 90}, key_projection{});

  if (first_result.in != source + 4 || first_result.out != replaced + 4) {
    return false;
  }

  if (replaced[0].key != 1 || replaced[0].payload != 10 ||
      replaced[1].key != 9 || replaced[1].payload != 90 ||
      replaced[2].key != 3 || replaced[2].payload != 30 ||
      replaced[3].key != 9 || replaced[3].payload != 90) {
    return false;
  }

  record conditional[4] = {};

  auto second_result = tested::ranges::replace_copy_if(
      source, conditional, even{}, record{0, 100}, key_projection{});

  return second_result.in == source + 4 &&
         second_result.out == conditional + 4 && conditional[0].key == 1 &&
         conditional[0].payload == 10 && conditional[1].key == 0 &&
         conditional[1].payload == 100 && conditional[2].key == 3 &&
         conditional[2].payload == 30 && conditional[3].key == 0 &&
         conditional[3].payload == 100;
}

static_assert(projected_replace_copy_works());

bool member_projection_replace_works() {
  record values[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {2, 40},
  };

  auto first_end =
      tested::ranges::replace(values, 2, record{8, 80}, &record::key);

  if (first_end != values + 4 || values[1].key != 8 ||
      values[1].payload != 80 || values[3].key != 8 ||
      values[3].payload != 80) {
    return false;
  }

  record source[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {4, 40},
  };

  record output[4] = {};

  auto result = tested::ranges::replace_copy_if(source, output, even{},
                                                record{0, 50}, &record::key);

  return result.in == source + 4 && result.out == output + 4 &&
         output[0].key == 1 && output[0].payload == 10 && output[1].key == 0 &&
         output[1].payload == 50 && output[2].key == 3 &&
         output[2].payload == 30 && output[3].key == 0 &&
         output[3].payload == 50;
}

bool counted_projection_works() {
  record source[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {4, 40},
  };

  record output[4] = {};

  int predicate_calls = 0;
  int projection_calls = 0;

  auto result = tested::ranges::replace_copy_if(
      source, output, counting_even{&predicate_calls}, record{0, 100},
      counting_projection{&projection_calls});

  return result.in == source + 4 && result.out == output + 4 &&
         predicate_calls == 4 && projection_calls == 4 && output[0].key == 1 &&
         output[0].payload == 10 && output[1].key == 0 &&
         output[1].payload == 100 && output[2].key == 3 &&
         output[2].payload == 30 && output[3].key == 0 &&
         output[3].payload == 100;
}

bool policy_replace_works() {
  int replaced[] = {1, 2, 3, 2, 4};
  int conditional[] = {1, 2, 3, 4, 5};
  int copied[5] = {};
  int copied_if[5] = {};

  tested::replace(tested::execution::seq, replaced, replaced + 5, 2, 9);

  tested::replace_if(tested::execution::par, conditional, conditional + 5,
                     even{}, 0);

  auto copy_end = tested::replace_copy(tested::execution::par_unseq, replaced,
                                       replaced + 5, copied, 9, 7);

  auto copy_if_end =
      tested::replace_copy_if(tested::execution::unseq, conditional,
                              conditional + 5, copied_if, even{}, 8);

  return copy_end == copied + 5 && copy_if_end == copied_if + 5 &&
         replaced[0] == 1 && replaced[1] == 9 && replaced[2] == 3 &&
         replaced[3] == 9 && replaced[4] == 4 && conditional[0] == 1 &&
         conditional[1] == 0 && conditional[2] == 3 && conditional[3] == 0 &&
         conditional[4] == 5 && copied[0] == 1 && copied[1] == 7 &&
         copied[2] == 3 && copied[3] == 7 && copied[4] == 4 &&
         copied_if[0] == 1 && copied_if[1] == 8 && copied_if[2] == 3 &&
         copied_if[3] == 8 && copied_if[4] == 5;
}

bool ftl_test() {
  return classic_replace_works() && classic_replace_copy_works() &&
         ranges_replace_works() && ranges_projected_replace_works() &&
         ranges_replace_copy_works() && projected_replace_copy_works() &&
         member_projection_replace_works() && counted_projection_works() &&
         policy_replace_works();
}