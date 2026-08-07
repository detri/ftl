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

struct even {
  constexpr bool operator()(int value) const noexcept { return value % 2 == 0; }
};

struct positive {
  constexpr bool operator()(int value) const noexcept { return value > 0; }
};

struct void_predicate {
  constexpr void operator()(int) const noexcept {}
};

struct move_only_even {
  constexpr move_only_even() noexcept = default;

  move_only_even(const move_only_even &) = delete;
  move_only_even &operator=(const move_only_even &) = delete;

  constexpr move_only_even(move_only_even &&) noexcept = default;
  constexpr move_only_even &operator=(move_only_even &&) noexcept = default;

  constexpr bool operator()(int value) const noexcept { return value % 2 == 0; }
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

struct counting_predicate {
  int *calls;

  constexpr bool operator()(int value) const noexcept {
    ++*calls;
    return value > 1;
  }
};

struct counting_projection {
  int *calls;

  constexpr int operator()(const record &value) const noexcept {
    ++*calls;
    return value.value;
  }
};

struct tracked_value {
  int value;
  int *comparisons;

  friend constexpr bool operator==(const tracked_value &left,
                                   const tracked_value &right) noexcept {
    ++*left.comparisons;
    return left.value == right.value;
  }
};

struct not_comparable {};

template <class Predicate>
concept ranges_count_if_accepts =
    requires(int *first, pointer_sentinel last, Predicate predicate) {
      {
        tested::ranges::count_if(first, last, predicate)
      } -> tested::same_as<tested::iter_difference_t<int *>>;
    };

static_assert(ranges_count_if_accepts<positive>);
static_assert(!ranges_count_if_accepts<void_predicate>);

template <class T>
concept ranges_count_accepts =
    requires(int *first, pointer_sentinel last, const T &value) {
      {
        tested::ranges::count(first, last, value)
      } -> tested::same_as<tested::iter_difference_t<int *>>;
    };

static_assert(ranges_count_accepts<int>);
static_assert(!ranges_count_accepts<not_comparable>);

template <class Policy>
concept policy_count_accepts = requires(Policy &&policy, int *first,
                                        int *last) {
  {
    tested::count(tested::forward<Policy>(policy), first, last, 2)
  }
  -> tested::same_as<typename tested::iterator_traits<int *>::difference_type>;

  {
    tested::count_if(tested::forward<Policy>(policy), first, last, even{})
  }
  -> tested::same_as<typename tested::iterator_traits<int *>::difference_type>;
};

static_assert(policy_count_accepts<tested::execution::sequenced_policy>);

static_assert(policy_count_accepts<tested::execution::parallel_policy>);

static_assert(
    policy_count_accepts<tested::execution::parallel_unsequenced_policy>);

static_assert(policy_count_accepts<tested::execution::unsequenced_policy>);

static_assert(
    policy_count_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_count_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::count)>);
static_assert(tested::is_object_v<decltype(tested::ranges::count_if)>);

static_assert(tested::is_same_v<
              decltype(tested::count(tested::declval<int *>(),
                                     tested::declval<int *>(), 0)),
              typename tested::iterator_traits<int *>::difference_type>);

static_assert(tested::is_same_v<decltype(tested::ranges::count(
                                    tested::declval<int *>(),
                                    tested::declval<pointer_sentinel>(), 0)),
                                tested::iter_difference_t<int *>>);

constexpr bool classic_count_works() {
  int values[] = {1, 2, 2, 3, 2, 4};

  if (tested::count(values, values + 6, 2) != 3) {
    return false;
  }

  if (tested::count(values, values + 6, 9) != 0) {
    return false;
  }

  if (tested::count(values, values, 1) != 0) {
    return false;
  }

  if (tested::count_if(values, values + 6, even{}) != 4) {
    return false;
  }

  if (tested::count_if(values, values, move_only_even{}) != 0) {
    return false;
  }

  int predicate_calls = 0;

  if (tested::count_if(values, values + 6,
                       counting_predicate{&predicate_calls}) != 5) {
    return false;
  }

  if (predicate_calls != 6) {
    return false;
  }

  int comparisons = 0;

  tracked_value tracked[] = {
      {1, &comparisons},
      {2, &comparisons},
      {2, &comparisons},
      {3, &comparisons},
  };

  if (tested::count(tracked, tracked + 4, tracked_value{2, &comparisons}) !=
      2) {
    return false;
  }

  return comparisons == 4;
}

static_assert(classic_count_works());

constexpr bool ranges_count_works() {
  int values[] = {1, 2, 2, 3, 2, 4};
  pointer_sentinel last{values + 6};

  if (tested::ranges::count(values, last, 2) != 3) {
    return false;
  }

  if (tested::ranges::count_if(values, last, even{}) != 4) {
    return false;
  }

  if (tested::ranges::count(values, 2) != 3) {
    return false;
  }

  if (tested::ranges::count_if(values, positive{}) != 6) {
    return false;
  }

  record records[] = {{1}, {2}, {2}, {3}, {2}, {4}};

  if (tested::ranges::count(records, 2, &record::value) != 3) {
    return false;
  }

  if (tested::ranges::count_if(records, even{}, &record::value) != 4) {
    return false;
  }

  int predicate_calls = 0;
  int projection_calls = 0;

  if (tested::ranges::count_if(records, counting_predicate{&predicate_calls},
                               counting_projection{&projection_calls}) != 5) {
    return false;
  }

  return predicate_calls == 6 && projection_calls == 6;
}

static_assert(ranges_count_works());

bool policy_count_works() {
  int values[] = {1, 2, 2, 3, 2, 4};

  return tested::count(tested::execution::seq, values, values + 6, 2) == 3 &&
         tested::count_if(tested::execution::par, values, values + 6, even{}) ==
             4 &&
         tested::count(tested::execution::par_unseq, values, values + 6, 9) ==
             0 &&
         tested::count_if(tested::execution::unseq, values, values + 6,
                          positive{}) == 6;
}

bool ftl_test() {
  return classic_count_works() && ranges_count_works() && policy_count_works();
}