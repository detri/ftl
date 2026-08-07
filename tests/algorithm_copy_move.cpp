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

struct counting_even {
  int *calls;

  constexpr bool operator()(int value) const noexcept {
    ++*calls;
    return value % 2 == 0;
  }
};

struct counting_projection {
  int *calls;

  constexpr int operator()(const record &value) const noexcept {
    ++*calls;
    return value.key;
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

struct temporary_range {
  int values[3] = {1, 2, 3};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 3; }
};

template <class Policy>
concept policy_copy_move_accepts =
    requires(Policy &&policy, int *first, int *last, int *result) {
      {
        tested::copy(tested::forward<Policy>(policy), first, last, result)
      } -> tested::same_as<int *>;

      {
        tested::copy_n(tested::forward<Policy>(policy), first, 2, result)
      } -> tested::same_as<int *>;

      {
        tested::copy_if(tested::forward<Policy>(policy), first, last, result,
                        even{})
      } -> tested::same_as<int *>;

      {
        tested::move(tested::forward<Policy>(policy), first, last, result)
      } -> tested::same_as<int *>;
    };

static_assert(policy_copy_move_accepts<tested::execution::sequenced_policy>);

static_assert(policy_copy_move_accepts<tested::execution::parallel_policy>);

static_assert(
    policy_copy_move_accepts<tested::execution::parallel_unsequenced_policy>);

static_assert(policy_copy_move_accepts<tested::execution::unsequenced_policy>);

static_assert(
    policy_copy_move_accepts<const tested::execution::sequenced_policy &>);

static_assert(!policy_copy_move_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::copy)>);

static_assert(tested::is_object_v<decltype(tested::ranges::copy_n)>);

static_assert(tested::is_object_v<decltype(tested::ranges::copy_if)>);

static_assert(tested::is_object_v<decltype(tested::ranges::copy_backward)>);

static_assert(tested::is_object_v<decltype(tested::ranges::move)>);

static_assert(tested::is_object_v<decltype(tested::ranges::move_backward)>);

static_assert(tested::is_same_v<tested::ranges::copy_result<int *, int *>,
                                tested::ranges::in_out_result<int *, int *>>);

static_assert(tested::is_same_v<tested::ranges::copy_n_result<int *, int *>,
                                tested::ranges::in_out_result<int *, int *>>);

static_assert(tested::is_same_v<tested::ranges::copy_if_result<int *, int *>,
                                tested::ranges::in_out_result<int *, int *>>);

static_assert(
    tested::is_same_v<tested::ranges::copy_backward_result<int *, int *>,
                      tested::ranges::in_out_result<int *, int *>>);

static_assert(tested::is_same_v<tested::ranges::move_result<int *, int *>,
                                tested::ranges::in_out_result<int *, int *>>);

static_assert(
    tested::is_same_v<tested::ranges::move_backward_result<int *, int *>,
                      tested::ranges::in_out_result<int *, int *>>);

static_assert(tested::is_same_v<
              decltype(tested::ranges::copy(tested::declval<temporary_range>(),
                                            tested::declval<int *>())),
              tested::ranges::copy_result<tested::ranges::dangling, int *>>);

static_assert(
    tested::is_same_v<
        decltype(tested::ranges::copy_backward(
            tested::declval<temporary_range>(), tested::declval<int *>())),
        tested::ranges::copy_backward_result<tested::ranges::dangling, int *>>);

static_assert(tested::is_same_v<
              decltype(tested::ranges::move(tested::declval<temporary_range>(),
                                            tested::declval<int *>())),
              tested::ranges::move_result<tested::ranges::dangling, int *>>);

static_assert(
    tested::is_same_v<
        decltype(tested::ranges::move_backward(
            tested::declval<temporary_range>(), tested::declval<int *>())),
        tested::ranges::move_backward_result<tested::ranges::dangling, int *>>);

constexpr bool classic_copy_works() {
  int source[] = {1, 2, 3, 4};
  int output[] = {0, 0, 0, 0};

  if (tested::copy(source, source + 4, output) != output + 4) {
    return false;
  }

  if (output[0] != 1 || output[1] != 2 || output[2] != 3 || output[3] != 4) {
    return false;
  }

  int first_three[] = {0, 0, 0, 0};

  if (tested::copy_n(source, 3, first_three) != first_three + 3) {
    return false;
  }

  if (first_three[0] != 1 || first_three[1] != 2 || first_three[2] != 3 ||
      first_three[3] != 0) {
    return false;
  }

  if (tested::copy_n(source, 0, first_three) != first_three) {
    return false;
  }

  int evens[] = {0, 0, 0, 0};

  if (tested::copy_if(source, source + 4, evens, even{}) != evens + 2) {
    return false;
  }

  if (evens[0] != 2 || evens[1] != 4) {
    return false;
  }

  int overlap[] = {1, 2, 3, 4, 0};

  if (tested::copy_backward(overlap, overlap + 4, overlap + 5) != overlap + 1) {
    return false;
  }

  return overlap[0] == 1 && overlap[1] == 1 && overlap[2] == 2 &&
         overlap[3] == 3 && overlap[4] == 4;
}

static_assert(classic_copy_works());

constexpr bool ranges_copy_works() {
  int source[] = {1, 2, 3, 4};
  int output[] = {0, 0, 0, 0};

  auto copied =
      tested::ranges::copy(source, pointer_sentinel<int>{source + 4}, output);

  if (copied.in != source + 4 || copied.out != output + 4) {
    return false;
  }

  if (output[0] != 1 || output[1] != 2 || output[2] != 3 || output[3] != 4) {
    return false;
  }

  int first_three[] = {0, 0, 0, 0};

  auto copied_n = tested::ranges::copy_n(source, 3, first_three);

  if (copied_n.in != source + 3 || copied_n.out != first_three + 3) {
    return false;
  }

  int evens[] = {0, 0, 0, 0};

  auto filtered = tested::ranges::copy_if(source, evens, even{});

  if (filtered.in != source + 4 || filtered.out != evens + 2 || evens[0] != 2 ||
      evens[1] != 4) {
    return false;
  }

  record records[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {4, 40},
  };

  record selected[4] = {};

  int predicate_calls = 0;
  int projection_calls = 0;

  auto projected = tested::ranges::copy_if(
      records, selected, counting_even{&predicate_calls},
      counting_projection{&projection_calls});

  if (projected.in != records + 4 || projected.out != selected + 2) {
    return false;
  }

  if (predicate_calls != 4 || projection_calls != 4) {
    return false;
  }

  if (selected[0].key != 2 || selected[0].payload != 20 ||
      selected[1].key != 4 || selected[1].payload != 40) {
    return false;
  }

  int overlap[] = {1, 2, 3, 4, 0};

  auto backward = tested::ranges::copy_backward(
      overlap, pointer_sentinel<int>{overlap + 4}, overlap + 5);

  return backward.in == overlap + 4 && backward.out == overlap + 1 &&
         overlap[0] == 1 && overlap[1] == 1 && overlap[2] == 2 &&
         overlap[3] == 3 && overlap[4] == 4;
}

static_assert(ranges_copy_works());

bool pointer_member_projection_works() {
  record records[] = {
      {1, 10},
      {2, 20},
      {3, 30},
      {4, 40},
  };

  record selected[4] = {};

  auto result =
      tested::ranges::copy_if(records, selected, even{}, &record::key);

  return result.in == records + 4 && result.out == selected + 2 &&
         selected[0].key == 2 && selected[0].payload == 20 &&
         selected[1].key == 4 && selected[1].payload == 40;
}

bool classic_move_works() {
  move_only_value source[] = {
      move_only_value{1},
      move_only_value{2},
      move_only_value{3},
  };

  move_only_value output[3];

  auto final = tested::move(source, source + 3, output);

  if (final != output + 3) {
    return false;
  }

  if (source[0].value != -1 || source[1].value != -1 || source[2].value != -1) {
    return false;
  }

  if (output[0].value != 1 || output[1].value != 2 || output[2].value != 3) {
    return false;
  }

  move_only_value overlap[] = {
      move_only_value{1}, move_only_value{2}, move_only_value{3},
      move_only_value{4}, move_only_value{0},
  };

  auto backward = tested::move_backward(overlap, overlap + 4, overlap + 5);

  return backward == overlap + 1 && overlap[0].value == -1 &&
         overlap[1].value == 1 && overlap[2].value == 2 &&
         overlap[3].value == 3 && overlap[4].value == 4;
}

bool ranges_move_works() {
  move_only_value source[] = {
      move_only_value{1},
      move_only_value{2},
      move_only_value{3},
  };

  move_only_value output[3];

  auto result = tested::ranges::move(
      source, pointer_sentinel<move_only_value>{source + 3}, output);

  if (result.in != source + 3 || result.out != output + 3) {
    return false;
  }

  if (source[0].value != -1 || source[1].value != -1 || source[2].value != -1) {
    return false;
  }

  if (output[0].value != 1 || output[1].value != 2 || output[2].value != 3) {
    return false;
  }

  move_only_value overlap[] = {
      move_only_value{1}, move_only_value{2}, move_only_value{3},
      move_only_value{4}, move_only_value{0},
  };

  auto backward = tested::ranges::move_backward(
      overlap, pointer_sentinel<move_only_value>{overlap + 4}, overlap + 5);

  return backward.in == overlap + 4 && backward.out == overlap + 1 &&
         overlap[0].value == -1 && overlap[1].value == 1 &&
         overlap[2].value == 2 && overlap[3].value == 3 &&
         overlap[4].value == 4;
}

bool policy_copy_move_works() {
  int source[] = {1, 2, 3, 4};

  int copied[4] = {};
  int copied_n[4] = {};
  int filtered[4] = {};
  int moved[4] = {};

  auto copy_end =
      tested::copy(tested::execution::seq, source, source + 4, copied);

  auto copy_n_end = tested::copy_n(tested::execution::par, source, 3, copied_n);

  auto copy_if_end = tested::copy_if(tested::execution::par_unseq, source,
                                     source + 4, filtered, even{});

  auto move_end =
      tested::move(tested::execution::unseq, source, source + 4, moved);

  return copy_end == copied + 4 && copy_n_end == copied_n + 3 &&
         copy_if_end == filtered + 2 && move_end == moved + 4 &&
         copied[0] == 1 && copied[1] == 2 && copied[2] == 3 && copied[3] == 4 &&
         copied_n[0] == 1 && copied_n[1] == 2 && copied_n[2] == 3 &&
         filtered[0] == 2 && filtered[1] == 4 && moved[0] == 1 &&
         moved[1] == 2 && moved[2] == 3 && moved[3] == 4;
}

bool ftl_test() {
  return classic_copy_works() && ranges_copy_works() &&
         pointer_member_projection_works() && classic_move_works() &&
         ranges_move_works() && policy_copy_move_works();
}