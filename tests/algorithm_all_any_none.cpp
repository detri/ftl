#ifdef FTL_REPLACE_STL
#include <algorithm>
#include <concepts>
#include <execution>
#include <iterator>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/algorithm>
#include <ftl/concepts>
#include <ftl/execution>
#include <ftl/iterator>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct positive {
  constexpr bool operator()(int value) const noexcept { return value > 0; }
};

struct negative {
  constexpr bool operator()(int value) const noexcept { return value < 0; }
};

struct equals_value {
  int expected;

  constexpr bool operator()(int value) const noexcept {
    return value == expected;
  }
};

struct counting_less_than {
  int limit;
  int *calls;

  constexpr bool operator()(int value) const noexcept {
    ++*calls;
    return value < limit;
  }
};

struct counting_equals {
  int expected;
  int *calls;

  constexpr bool operator()(int value) const noexcept {
    ++*calls;
    return value == expected;
  }
};

struct move_only_positive {
  constexpr move_only_positive() noexcept = default;

  move_only_positive(const move_only_positive &) = delete;
  move_only_positive &operator=(const move_only_positive &) = delete;

  constexpr move_only_positive(move_only_positive &&) noexcept = default;
  constexpr move_only_positive &
  operator=(move_only_positive &&) noexcept = default;

  constexpr bool operator()(int value) const noexcept { return value > 0; }
};

struct void_predicate {
  constexpr void operator()(int) const noexcept {}
};

struct pointer_sentinel {
  int *last = nullptr;

  friend constexpr bool operator==(int *iterator,
                                   pointer_sentinel sentinel) noexcept {
    return iterator == sentinel.last;
  }

  friend constexpr bool operator==(pointer_sentinel sentinel,
                                   int *iterator) noexcept {
    return iterator == sentinel;
  }

  friend constexpr bool operator!=(int *iterator,
                                   pointer_sentinel sentinel) noexcept {
    return !(iterator == sentinel);
  }

  friend constexpr bool operator!=(pointer_sentinel sentinel,
                                   int *iterator) noexcept {
    return !(sentinel == iterator);
  }
};

static_assert(tested::sentinel_for<pointer_sentinel, int *>);

struct record {
  int value;
};

struct counting_projection {
  int *calls;

  constexpr int operator()(const record &value) const noexcept {
    ++*calls;
    return value.value;
  }
};

struct counting_positive {
  int *calls;

  constexpr bool operator()(int value) const noexcept {
    ++*calls;
    return value > 0;
  }
};

template <class Predicate>
concept ranges_quantifiers_accept =
    requires(int *first, pointer_sentinel last, Predicate predicate) {
      {
        tested::ranges::all_of(first, last, predicate)
      } -> tested::same_as<bool>;

      {
        tested::ranges::any_of(first, last, predicate)
      } -> tested::same_as<bool>;

      {
        tested::ranges::none_of(first, last, predicate)
      } -> tested::same_as<bool>;
    };

static_assert(ranges_quantifiers_accept<positive>);
static_assert(!ranges_quantifiers_accept<void_predicate>);

template <class Policy>
concept policy_quantifiers_accept = requires(Policy &&policy, int *first,
                                             int *last) {
  {
    tested::all_of(tested::forward<Policy>(policy), first, last, positive{})
  } -> tested::same_as<bool>;

  {
    tested::any_of(tested::forward<Policy>(policy), first, last, positive{})
  } -> tested::same_as<bool>;

  {
    tested::none_of(tested::forward<Policy>(policy), first, last, positive{})
  } -> tested::same_as<bool>;
};

static_assert(policy_quantifiers_accept<tested::execution::sequenced_policy>);

static_assert(policy_quantifiers_accept<tested::execution::parallel_policy>);

static_assert(
    policy_quantifiers_accept<tested::execution::parallel_unsequenced_policy>);

static_assert(policy_quantifiers_accept<tested::execution::unsequenced_policy>);

static_assert(
    policy_quantifiers_accept<const tested::execution::sequenced_policy &>);

static_assert(!policy_quantifiers_accept<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::all_of)>);
static_assert(tested::is_object_v<decltype(tested::ranges::any_of)>);
static_assert(tested::is_object_v<decltype(tested::ranges::none_of)>);

constexpr bool classic_quantifiers_work() {
  int positive_values[] = {1, 2, 3, 4};
  int mixed_values[] = {1, 2, -3, 4};

  if (!tested::all_of(positive_values, positive_values + 4, positive{})) {
    return false;
  }

  if (tested::all_of(mixed_values, mixed_values + 4, positive{})) {
    return false;
  }

  if (!tested::any_of(mixed_values, mixed_values + 4, negative{})) {
    return false;
  }

  if (tested::any_of(positive_values, positive_values + 4, negative{})) {
    return false;
  }

  if (!tested::none_of(positive_values, positive_values + 4, negative{})) {
    return false;
  }

  if (tested::none_of(mixed_values, mixed_values + 4, negative{})) {
    return false;
  }

  if (!tested::all_of(positive_values, positive_values, move_only_positive{})) {
    return false;
  }

  if (tested::any_of(positive_values, positive_values, move_only_positive{})) {
    return false;
  }

  if (!tested::none_of(positive_values, positive_values,
                       move_only_positive{})) {
    return false;
  }

  int calls = 0;

  if (tested::all_of(positive_values, positive_values + 4,
                     counting_less_than{3, &calls})) {
    return false;
  }

  if (calls != 3) {
    return false;
  }

  calls = 0;

  if (!tested::any_of(positive_values, positive_values + 4,
                      counting_equals{3, &calls})) {
    return false;
  }

  if (calls != 3) {
    return false;
  }

  calls = 0;

  if (tested::none_of(positive_values, positive_values + 4,
                      counting_equals{3, &calls})) {
    return false;
  }

  return calls == 3;
}

static_assert(classic_quantifiers_work());

constexpr bool ranges_quantifiers_work() {
  int values[] = {1, 2, 3, 4};
  pointer_sentinel last{values + 4};

  if (!tested::ranges::all_of(values, last, positive{})) {
    return false;
  }

  if (!tested::ranges::any_of(values, last, equals_value{3})) {
    return false;
  }

  if (!tested::ranges::none_of(values, last, negative{})) {
    return false;
  }

  record records[] = {{1}, {2}, {-3}, {4}};

  if (tested::ranges::all_of(records, positive{}, &record::value)) {
    return false;
  }

  if (!tested::ranges::any_of(records, negative{}, &record::value)) {
    return false;
  }

  if (tested::ranges::none_of(records, negative{}, &record::value)) {
    return false;
  }

  int predicate_calls = 0;
  int projection_calls = 0;

  if (tested::ranges::all_of(records, counting_positive{&predicate_calls},
                             counting_projection{&projection_calls})) {
    return false;
  }

  return predicate_calls == 3 && projection_calls == 3;
}

static_assert(ranges_quantifiers_work());

bool policy_quantifiers_work() {
  int positive_values[] = {1, 2, 3, 4};
  int mixed_values[] = {1, -2, 3, 4};

  return tested::all_of(tested::execution::seq, positive_values,
                        positive_values + 4, positive{}) &&
         tested::any_of(tested::execution::par, mixed_values, mixed_values + 4,
                        negative{}) &&
         tested::none_of(tested::execution::par_unseq, positive_values,
                         positive_values + 4, negative{}) &&
         tested::all_of(tested::execution::unseq, positive_values,
                        positive_values + 4, positive{});
}

bool ftl_test() {
  return classic_quantifiers_work() && ranges_quantifiers_work() &&
         policy_quantifiers_work();
}