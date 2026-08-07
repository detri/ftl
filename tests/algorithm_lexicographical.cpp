#ifdef FTL_REPLACE_STL
#include <algorithm>
#include <compare>
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
#include <ftl/compare>
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

struct record {
  int key = 0;
  int sequence = 0;
};

struct key_projection {
  constexpr int operator()(const record &value) const noexcept {
    return value.key;
  }
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

struct weak_compare {
  constexpr tested::weak_ordering operator()(int left,
                                             int right) const noexcept {
    if (left < right) {
      return tested::weak_ordering::less;
    }

    if (left > right) {
      return tested::weak_ordering::greater;
    }

    return tested::weak_ordering::equivalent;
  }
};

template <class Comparator>
concept ranges_lexicographical_accepts =
    requires(int *first, int *last, Comparator comparator) {
      {
        tested::ranges::lexicographical_compare(first, last, first, last,
                                                comparator)
      } -> tested::same_as<bool>;
    };

static_assert(ranges_lexicographical_accepts<descending>);

static_assert(!ranges_lexicographical_accepts<void_comparator>);

template <class Policy>
concept policy_lexicographical_accepts =
    requires(Policy &&policy, int *first, int *last) {
      {
        tested::lexicographical_compare(tested::forward<Policy>(policy), first,
                                        last, first, last)
      } -> tested::same_as<bool>;
    };

static_assert(
    policy_lexicographical_accepts<tested::execution::sequenced_policy>);

static_assert(
    policy_lexicographical_accepts<tested::execution::parallel_policy>);

static_assert(policy_lexicographical_accepts<
              tested::execution::parallel_unsequenced_policy>);

static_assert(
    policy_lexicographical_accepts<tested::execution::unsequenced_policy>);

static_assert(!policy_lexicographical_accepts<int>);

constexpr bool classic_lexicographical_works() {
  int first[] = {1, 2, 3};
  int second[] = {1, 2, 4};
  int prefix[] = {1, 2};
  int equal[] = {1, 2, 3};

  if (!tested::lexicographical_compare(first, first + 3, second, second + 3)) {
    return false;
  }

  if (tested::lexicographical_compare(second, second + 3, first, first + 3)) {
    return false;
  }

  if (!tested::lexicographical_compare(prefix, prefix + 2, first, first + 3)) {
    return false;
  }

  if (tested::lexicographical_compare(first, first + 3, equal, equal + 3)) {
    return false;
  }

  int empty[1] = {};

  if (!tested::lexicographical_compare(empty, empty, first, first + 3)) {
    return false;
  }

  return !tested::lexicographical_compare(empty, empty, empty, empty);
}

static_assert(classic_lexicographical_works());

constexpr bool custom_comparator_works() {
  int first[] = {3, 2, 1};
  int second[] = {3, 1, 9};

  return tested::lexicographical_compare(first, first + 3, second, second + 3,
                                         descending{});
}

static_assert(custom_comparator_works());

constexpr bool ranges_lexicographical_works() {
  int first[] = {1, 2, 3};
  int second[] = {1, 2, 4};

  if (!tested::ranges::lexicographical_compare(
          first, pointer_sentinel<int>{first + 3}, second,
          pointer_sentinel<int>{second + 3})) {
    return false;
  }

  record left[] = {
      {1, 0},
      {2, 1},
      {3, 2},
  };

  record right[] = {
      {1, 3},
      {2, 4},
      {4, 5},
  };

  return tested::ranges::lexicographical_compare(
      left, right, tested::ranges::less{}, key_projection{}, key_projection{});
}

static_assert(ranges_lexicographical_works());

constexpr bool three_way_works() {
  int first[] = {1, 2, 3};
  int second[] = {1, 2, 4};
  int equal[] = {1, 2, 3};
  int prefix[] = {1, 2};

  static_assert(
      tested::is_same_v<decltype(tested::lexicographical_compare_three_way(
                            first, first + 3, second, second + 3)),
                        tested::strong_ordering>);

  if (tested::lexicographical_compare_three_way(first, first + 3, second,
                                                second + 3) !=
      tested::strong_ordering::less) {
    return false;
  }

  if (tested::lexicographical_compare_three_way(second, second + 3, first,
                                                first + 3) !=
      tested::strong_ordering::greater) {
    return false;
  }

  if (tested::lexicographical_compare_three_way(first, first + 3, equal,
                                                equal + 3) !=
      tested::strong_ordering::equal) {
    return false;
  }

  return tested::lexicographical_compare_three_way(prefix, prefix + 2, first,
                                                   first + 3) ==
         tested::strong_ordering::less;
}

static_assert(three_way_works());

constexpr bool three_way_custom_category_works() {
  int first[] = {1, 2, 3};
  int second[] = {1, 2, 4};

  static_assert(tested::is_same_v<
                decltype(tested::lexicographical_compare_three_way(
                    first, first + 3, second, second + 3, weak_compare{})),
                tested::weak_ordering>);

  return tested::lexicographical_compare_three_way(
             first, first + 3, second, second + 3, weak_compare{}) ==
         tested::weak_ordering::less;
}

static_assert(three_way_custom_category_works());

constexpr bool three_way_input_iterator_works() {
  int first[] = {1, 2, 3};
  int second[] = {1, 2, 3, 4};

  auto result = tested::lexicographical_compare_three_way(
      input_cursor{first}, input_cursor{first + 3}, input_cursor{second},
      input_cursor{second + 4});

  return result == tested::strong_ordering::less;
}

static_assert(three_way_input_iterator_works());

bool policy_works() {
  int first[] = {1, 2, 3};
  int second[] = {1, 2, 4};

  if (!tested::lexicographical_compare(tested::execution::seq, first, first + 3,
                                       second, second + 3)) {
    return false;
  }

  if (!tested::lexicographical_compare(tested::execution::par, first, first + 3,
                                       second, second + 3)) {
    return false;
  }

  return tested::lexicographical_compare(tested::execution::par_unseq, first,
                                         first + 3, second, second + 3);
}

bool ftl_test() {
  return classic_lexicographical_works() && custom_comparator_works() &&
         ranges_lexicographical_works() && three_way_works() &&
         three_way_custom_category_works() &&
         three_way_input_iterator_works() && policy_works();
}
