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

struct counting_less {
  int *calls;

  constexpr bool operator()(int left, int right) const noexcept {
    ++*calls;
    return left < right;
  }
};

struct temporary_range {
  int values[6] = {1, 2, 2, 2, 3, 4};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 6; }
};

template <class Comparator>
concept ranges_binary_search_accepts =
    requires(int *first, int *last, Comparator comparator) {
      {
        tested::ranges::lower_bound(first, last, 2, comparator)
      } -> tested::same_as<int *>;

      {
        tested::ranges::upper_bound(first, last, 2, comparator)
      } -> tested::same_as<int *>;

      tested::ranges::equal_range(first, last, 2, comparator);

      {
        tested::ranges::binary_search(first, last, 2, comparator)
      } -> tested::same_as<bool>;
    };

static_assert(ranges_binary_search_accepts<descending>);

static_assert(!ranges_binary_search_accepts<void_comparator>);

template <class Policy>
concept policy_binary_search_accepts =
    requires(Policy &&policy, int *first, int *last) {
      tested::lower_bound(tested::forward<Policy>(policy), first, last, 2);

      tested::upper_bound(tested::forward<Policy>(policy), first, last, 2);

      tested::equal_range(tested::forward<Policy>(policy), first, last, 2);

      tested::binary_search(tested::forward<Policy>(policy), first, last, 2);
    };

static_assert(
    !policy_binary_search_accepts<tested::execution::sequenced_policy>);

static_assert(
    !policy_binary_search_accepts<tested::execution::parallel_policy>);

static_assert(tested::is_object_v<decltype(tested::ranges::lower_bound)>);

static_assert(tested::is_object_v<decltype(tested::ranges::upper_bound)>);

static_assert(tested::is_object_v<decltype(tested::ranges::equal_range)>);

static_assert(tested::is_object_v<decltype(tested::ranges::binary_search)>);

static_assert(tested::is_same_v<decltype(tested::ranges::lower_bound(
                                    tested::declval<temporary_range>(), 2)),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::upper_bound(
                                    tested::declval<temporary_range>(), 2)),
                                tested::ranges::dangling>);

static_assert(tested::is_same_v<decltype(tested::ranges::equal_range(
                                    tested::declval<temporary_range>(), 2)),
                                tested::ranges::dangling>);

constexpr bool classic_binary_search_works() {
  int values[] = {1, 2, 2, 2, 3, 4};

  if (tested::lower_bound(values, values + 6, 2) != values + 1) {
    return false;
  }

  if (tested::upper_bound(values, values + 6, 2) != values + 4) {
    return false;
  }

  auto range = tested::equal_range(values, values + 6, 2);

  if (range.first != values + 1 || range.second != values + 4) {
    return false;
  }

  if (!tested::binary_search(values, values + 6, 3)) {
    return false;
  }

  if (tested::binary_search(values, values + 6, 5)) {
    return false;
  }

  if (tested::lower_bound(values, values + 6, 0) != values) {
    return false;
  }

  return tested::upper_bound(values, values + 6, 5) == values + 6;
}

static_assert(classic_binary_search_works());

constexpr bool classic_comparator_works() {
  int values[] = {9, 7, 7, 7, 5, 3};

  if (tested::lower_bound(values, values + 6, 7, descending{}) != values + 1) {
    return false;
  }

  if (tested::upper_bound(values, values + 6, 7, descending{}) != values + 4) {
    return false;
  }

  auto range = tested::equal_range(values, values + 6, 7, descending{});

  return range.first == values + 1 && range.second == values + 4 &&
         tested::binary_search(values, values + 6, 5, descending{}) &&
         !tested::binary_search(values, values + 6, 6, descending{});
}

static_assert(classic_comparator_works());

constexpr bool ranges_binary_search_works() {
  int values[] = {1, 2, 2, 2, 3, 4};

  auto lower =
      tested::ranges::lower_bound(values, pointer_sentinel<int>{values + 6}, 2);

  if (lower != values + 1) {
    return false;
  }

  auto upper =
      tested::ranges::upper_bound(values, pointer_sentinel<int>{values + 6}, 2);

  if (upper != values + 4) {
    return false;
  }

  auto range =
      tested::ranges::equal_range(values, pointer_sentinel<int>{values + 6}, 2);

  if (range.begin() != values + 1 || range.end() != values + 4) {
    return false;
  }

  return tested::ranges::binary_search(values, 3) &&
         !tested::ranges::binary_search(values, 5);
}

static_assert(ranges_binary_search_works());

constexpr bool forward_binary_search_works() {
  int values[] = {1, 2, 2, 2, 3, 4};

  auto lower = tested::ranges::lower_bound(forward_cursor{values},
                                           forward_cursor{values + 6}, 2);

  if (lower.current != values + 1) {
    return false;
  }

  auto upper = tested::ranges::upper_bound(forward_cursor{values},
                                           forward_cursor{values + 6}, 2);

  if (upper.current != values + 4) {
    return false;
  }

  auto range = tested::ranges::equal_range(forward_cursor{values},
                                           forward_cursor{values + 6}, 2);

  return range.begin().current == values + 1 &&
         range.end().current == values + 4 &&
         tested::ranges::binary_search(forward_cursor{values},
                                       forward_cursor{values + 6}, 3) &&
         !tested::ranges::binary_search(forward_cursor{values},
                                        forward_cursor{values + 6}, 5);
}

static_assert(forward_binary_search_works());

constexpr bool projected_binary_search_works() {
  record values[] = {
      {1, 10}, {2, 20}, {2, 30}, {2, 40}, {3, 50}, {4, 60},
  };

  auto lower = tested::ranges::lower_bound(values, 2, tested::ranges::less{},
                                           key_projection{});

  if (lower != values + 1) {
    return false;
  }

  auto upper = tested::ranges::upper_bound(values, 2, tested::ranges::less{},
                                           key_projection{});

  if (upper != values + 4) {
    return false;
  }

  auto range = tested::ranges::equal_range(values, 2, tested::ranges::less{},
                                           key_projection{});

  return range.begin() == values + 1 && range.end() == values + 4 &&
         tested::ranges::binary_search(values, 3, tested::ranges::less{},
                                       key_projection{}) &&
         !tested::ranges::binary_search(values, 5, tested::ranges::less{},
                                        key_projection{});
}

static_assert(projected_binary_search_works());

bool member_projection_works() {
  record values[] = {
      {1, 10}, {2, 20}, {2, 30}, {2, 40}, {3, 50}, {4, 60},
  };

  auto lower = tested::ranges::lower_bound(values, 2, tested::ranges::less{},
                                           &record::key);

  auto upper = tested::ranges::upper_bound(values, 2, tested::ranges::less{},
                                           &record::key);

  auto range = tested::ranges::equal_range(values, 2, tested::ranges::less{},
                                           &record::key);

  return lower == values + 1 && upper == values + 4 &&
         range.begin() == values + 1 && range.end() == values + 4 &&
         tested::ranges::binary_search(values, 3, tested::ranges::less{},
                                       &record::key);
}

bool counted_calls_work() {
  record values[] = {
      {1, 10}, {2, 20}, {2, 30}, {2, 40}, {3, 50}, {4, 60}, {5, 70}, {6, 80},
  };

  int comparator_calls = 0;
  int projection_calls = 0;

  auto result =
      tested::ranges::lower_bound(values, 4, counting_less{&comparator_calls},
                                  counting_projection{&projection_calls});

  return result == values + 5 && comparator_calls > 0 &&
         comparator_calls <= 4 && projection_calls == comparator_calls;
}

bool ftl_test() {
  return classic_binary_search_works() && classic_comparator_works() &&
         ranges_binary_search_works() && forward_binary_search_works() &&
         projected_binary_search_works() && member_projection_works() &&
         counted_calls_work();
}
