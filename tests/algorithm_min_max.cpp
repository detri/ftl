#ifdef FTL_REPLACE_STL
#include <algorithm>
#include <concepts>
#include <execution>
#include <initializer_list>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/algorithm>
#include <ftl/concepts>
#include <ftl/execution>
#include <ftl/initializer_list>
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

struct record {
  int key = 0;
  int sequence = 0;
};

struct key_projection {
  constexpr int operator()(const record &value) const noexcept {
    return value.key;
  }
};

struct temporary_range {
  int values[5] = {3, 1, 5, 1, 5};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 5; }
};

template <class Comparator>
concept ranges_min_max_accepts =
    requires(int *first, int *last, Comparator comparator) {
      {
        tested::ranges::min(*first, *last, comparator)
      } -> tested::same_as<const int &>;

      {
        tested::ranges::max(*first, *last, comparator)
      } -> tested::same_as<const int &>;

      tested::ranges::minmax(*first, *last, comparator);

      {
        tested::ranges::min_element(first, last, comparator)
      } -> tested::same_as<int *>;

      {
        tested::ranges::max_element(first, last, comparator)
      } -> tested::same_as<int *>;

      tested::ranges::minmax_element(first, last, comparator);

      {
        tested::ranges::clamp(*first, *first, *last, comparator)
      } -> tested::same_as<const int &>;
    };

static_assert(ranges_min_max_accepts<descending>);

static_assert(!ranges_min_max_accepts<void_comparator>);

template <class Policy>
concept policy_element_accepts =
    requires(Policy &&policy, int *first, int *last) {
      {
        tested::min_element(tested::forward<Policy>(policy), first, last)
      } -> tested::same_as<int *>;

      {
        tested::max_element(tested::forward<Policy>(policy), first, last)
      } -> tested::same_as<int *>;

      tested::minmax_element(tested::forward<Policy>(policy), first, last);
    };

static_assert(policy_element_accepts<tested::execution::sequenced_policy>);

static_assert(policy_element_accepts<tested::execution::parallel_policy>);

static_assert(
    policy_element_accepts<tested::execution::parallel_unsequenced_policy>);

static_assert(policy_element_accepts<tested::execution::unsequenced_policy>);

static_assert(!policy_element_accepts<int>);

template <class Policy>
concept policy_value_accepts = requires(Policy &&policy, int a, int b) {
  tested::min(tested::forward<Policy>(policy), a, b);

  tested::max(tested::forward<Policy>(policy), a, b);

  tested::minmax(tested::forward<Policy>(policy), a, b);

  tested::clamp(tested::forward<Policy>(policy), a, a, b);
};

static_assert(!policy_value_accepts<tested::execution::sequenced_policy>);

static_assert(tested::is_object_v<decltype(tested::ranges::min)>);

static_assert(tested::is_object_v<decltype(tested::ranges::max)>);

static_assert(tested::is_object_v<decltype(tested::ranges::minmax)>);

static_assert(tested::is_object_v<decltype(tested::ranges::min_element)>);

static_assert(tested::is_object_v<decltype(tested::ranges::max_element)>);

static_assert(tested::is_object_v<decltype(tested::ranges::minmax_element)>);

static_assert(tested::is_object_v<decltype(tested::ranges::clamp)>);

static_assert(tested::is_same_v<tested::ranges::minmax_element_result<int *>,
                                tested::ranges::min_max_result<int *>>);

static_assert(tested::is_same_v<decltype(tested::ranges::min_element(
                                    tested::declval<temporary_range>())),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::max_element(
                                    tested::declval<temporary_range>())),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<
              decltype(tested::ranges::minmax_element(
                  tested::declval<temporary_range>())),
              tested::ranges::minmax_element_result<tested::ranges::dangling>>);

constexpr bool classic_values_work() {
  int one = 4;
  int two = 4;

  if (&tested::min(one, two) != &one) {
    return false;
  }

  if (&tested::max(one, two) != &one) {
    return false;
  }

  auto equivalent = tested::minmax(one, two);

  if (&equivalent.first != &one || &equivalent.second != &two) {
    return false;
  }

  int seven = 7;
  int three = 3;

  if (tested::min(seven, three) != 3 || tested::max(seven, three) != 7) {
    return false;
  }

  auto values = tested::minmax(seven, three);

  if (values.first != 3 || values.second != 7) {
    return false;
  }

  if (tested::min({4, 2, 7, 1, 5}) != 1 || tested::max({4, 2, 7, 1, 5}) != 7) {
    return false;
  }

  auto list_result = tested::minmax({4, 2, 7, 1, 7});

  return list_result.first == 1 && list_result.second == 7;
}

static_assert(classic_values_work());

constexpr bool comparator_values_work() {
  int seven = 7;
  int three = 3;

  if (tested::min(seven, three, descending{}) != 7) {
    return false;
  }

  if (tested::max(seven, three, descending{}) != 3) {
    return false;
  }

  auto result = tested::minmax(seven, three, descending{});

  if (result.first != 7 || result.second != 3) {
    return false;
  }

  return tested::min({1, 9, 3, 7}, descending{}) == 9 &&
         tested::max({1, 9, 3, 7}, descending{}) == 1;
}

static_assert(comparator_values_work());

constexpr bool classic_elements_work() {
  int values[] = {
      5, 1, 3, 5, 1, 5,
  };

  if (tested::min_element(values, values + 6) != values + 1) {
    return false;
  }

  if (tested::max_element(values, values + 6) != values) {
    return false;
  }

  auto result = tested::minmax_element(values, values + 6);

  if (result.first != values + 1 || result.second != values + 5) {
    return false;
  }

  return tested::min_element(values, values) == values &&
         tested::max_element(values, values) == values;
}

static_assert(classic_elements_work());

constexpr bool ranges_values_work() {
  int left = 4;
  int right = 4;

  if (&tested::ranges::min(left, right) != &left) {
    return false;
  }

  if (&tested::ranges::max(left, right) != &left) {
    return false;
  }

  auto references = tested::ranges::minmax(left, right);

  if (&references.min != &left || &references.max != &right) {
    return false;
  }

  int values[] = {
      5, 1, 3, 5, 1, 5,
  };

  if (tested::ranges::min(values) != 1 || tested::ranges::max(values) != 5) {
    return false;
  }

  auto range_result = tested::ranges::minmax(values);

  if (range_result.min != 1 || range_result.max != 5) {
    return false;
  }

  if (tested::ranges::min({4, 2, 7, 1, 5}) != 1 ||
      tested::ranges::max({4, 2, 7, 1, 5}) != 7) {
    return false;
  }

  auto list_result = tested::ranges::minmax({4, 2, 7, 1, 7});

  return list_result.min == 1 && list_result.max == 7;
}

static_assert(ranges_values_work());

constexpr bool ranges_elements_work() {
  int values[] = {
      5, 1, 3, 5, 1, 5,
  };

  if (tested::ranges::min_element(values) != values + 1) {
    return false;
  }

  if (tested::ranges::max_element(values) != values) {
    return false;
  }

  auto result = tested::ranges::minmax_element(values);

  return result.min == values + 1 && result.max == values + 5;
}

static_assert(ranges_elements_work());

constexpr bool projection_works() {
  record values[] = {
      {5, 0}, {1, 1}, {3, 2}, {5, 3}, {1, 4}, {5, 5},
  };

  if (tested::ranges::min(values, tested::ranges::less{}, key_projection{})
          .key != 1) {
    return false;
  }

  if (tested::ranges::max(values, tested::ranges::less{}, key_projection{})
          .key != 5) {
    return false;
  }

  auto value_result =
      tested::ranges::minmax(values, tested::ranges::less{}, key_projection{});

  if (value_result.min.key != 1 || value_result.max.key != 5) {
    return false;
  }

  auto element_result = tested::ranges::minmax_element(
      values, tested::ranges::less{}, key_projection{});

  return element_result.min == values + 1 && element_result.max == values + 5;
}

static_assert(projection_works());

bool member_projection_works() {
  record values[] = {
      {5, 0}, {1, 1}, {3, 2}, {5, 3}, {1, 4}, {5, 5},
  };

  auto minimum =
      tested::ranges::min_element(values, tested::ranges::less{}, &record::key);

  auto maximum =
      tested::ranges::max_element(values, tested::ranges::less{}, &record::key);

  auto both = tested::ranges::minmax_element(values, tested::ranges::less{},
                                             &record::key);

  return minimum == values + 1 && maximum == values && both.min == values + 1 &&
         both.max == values + 5;
}

constexpr bool clamp_works() {
  if (tested::clamp(0, 1, 5) != 1 || tested::clamp(3, 1, 5) != 3 ||
      tested::clamp(8, 1, 5) != 5) {
    return false;
  }

  if (tested::ranges::clamp(0, 1, 5) != 1 ||
      tested::ranges::clamp(3, 1, 5) != 3 ||
      tested::ranges::clamp(8, 1, 5) != 5) {
    return false;
  }

  record low{1, 0};
  record middle{3, 1};
  record high{5, 2};
  record below{0, 3};
  record above{8, 4};

  if (&tested::ranges::clamp(below, low, high, tested::ranges::less{},
                             key_projection{}) != &low) {
    return false;
  }

  if (&tested::ranges::clamp(middle, low, high, tested::ranges::less{},
                             key_projection{}) != &middle) {
    return false;
  }

  return &tested::ranges::clamp(above, low, high, tested::ranges::less{},
                                key_projection{}) == &high;
}

static_assert(clamp_works());

bool policy_elements_work() {
  int values[] = {
      5, 1, 3, 5, 1, 5,
  };

  auto minimum =
      tested::min_element(tested::execution::seq, values, values + 6);

  auto maximum =
      tested::max_element(tested::execution::par, values, values + 6);

  auto both =
      tested::minmax_element(tested::execution::par_unseq, values, values + 6);

  return minimum == values + 1 && maximum == values &&
         both.first == values + 1 && both.second == values + 5;
}

bool ftl_test() {
  return classic_values_work() && comparator_values_work() &&
         classic_elements_work() && ranges_values_work() &&
         ranges_elements_work() && projection_works() &&
         member_projection_works() && clamp_works() && policy_elements_work();
}
