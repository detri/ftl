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

struct double_value {
  constexpr int operator()(int value) const noexcept { return value * 2; }
};

struct add_values {
  constexpr int operator()(int left, int right) const noexcept {
    return left + right;
  }
};

struct void_unary {
  constexpr void operator()(int) const noexcept {}
};

struct void_binary {
  constexpr void operator()(int, int) const noexcept {}
};

struct pointer_sentinel {
  int *last = nullptr;

  friend constexpr bool operator==(int *iterator,
                                   pointer_sentinel sentinel) noexcept {
    return iterator == sentinel.last;
  }

  friend constexpr bool operator==(pointer_sentinel sentinel,
                                   int *iterator) noexcept {
    return iterator == sentinel.last;
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
  int value = 0;
};

struct value_projection {
  constexpr int operator()(const record &value) const noexcept {
    return value.value;
  }
};

struct counting_projection {
  int *calls;

  constexpr int operator()(const record &value) const noexcept {
    ++*calls;
    return value.value;
  }
};

struct counting_unary {
  int *calls;

  constexpr int operator()(int value) const noexcept {
    ++*calls;
    return value * 3;
  }
};

struct sequence_generator {
  int next = 0;

  constexpr int operator()() noexcept { return next++; }
};

struct constant_generator {
  int value = 0;

  constexpr int operator()() const noexcept { return value; }
};

struct void_generator {
  constexpr void operator()() const noexcept {}
};

struct temporary_range {
  int values[3] = {1, 2, 3};

  constexpr int *begin() noexcept { return values; }

  constexpr int *end() noexcept { return values + 3; }
};

template <class Function>
concept unary_transform_accepts = requires(int *first, pointer_sentinel last,
                                           int *output, Function function) {
  tested::ranges::transform(first, last, output, function);
};

static_assert(unary_transform_accepts<double_value>);
static_assert(!unary_transform_accepts<void_unary>);

template <class Function>
concept binary_transform_accepts =
    requires(int *first1, pointer_sentinel last1, int *first2,
             pointer_sentinel last2, int *output, Function function) {
      tested::ranges::transform(first1, last1, first2, last2, output, function);
    };

static_assert(binary_transform_accepts<add_values>);
static_assert(!binary_transform_accepts<void_binary>);

template <class Function>
concept generate_accepts =
    requires(int *first, pointer_sentinel last, Function function) {
      {
        tested::ranges::generate(first, last, function)
      } -> tested::same_as<int *>;

      {
        tested::ranges::generate_n(first, 2, function)
      } -> tested::same_as<int *>;
    };

static_assert(generate_accepts<constant_generator>);
static_assert(!generate_accepts<void_generator>);

template <class Policy>
concept policy_transform_generate_accepts = requires(
    Policy &&policy, int *first1, int *last1, int *first2, int *output) {
  {
    tested::transform(tested::forward<Policy>(policy), first1, last1, output,
                      double_value{})
  } -> tested::same_as<int *>;

  {
    tested::transform(tested::forward<Policy>(policy), first1, last1, first2,
                      output, add_values{})
  } -> tested::same_as<int *>;

  tested::generate(tested::forward<Policy>(policy), output, output + 3,
                   constant_generator{7});

  {
    tested::generate_n(tested::forward<Policy>(policy), output, 3,
                       constant_generator{7})
  } -> tested::same_as<int *>;
};

static_assert(
    policy_transform_generate_accepts<tested::execution::sequenced_policy>);

static_assert(
    policy_transform_generate_accepts<tested::execution::parallel_policy>);

static_assert(policy_transform_generate_accepts<
              tested::execution::parallel_unsequenced_policy>);

static_assert(
    policy_transform_generate_accepts<tested::execution::unsequenced_policy>);

static_assert(policy_transform_generate_accepts<
              const tested::execution::sequenced_policy &>);

static_assert(!policy_transform_generate_accepts<int>);

static_assert(tested::is_object_v<decltype(tested::ranges::transform)>);

static_assert(tested::is_object_v<decltype(tested::ranges::generate)>);

static_assert(tested::is_object_v<decltype(tested::ranges::generate_n)>);

static_assert(
    tested::is_same_v<tested::ranges::unary_transform_result<int *, int *>,
                      tested::ranges::in_out_result<int *, int *>>);

static_assert(
    tested::is_same_v<
        tested::ranges::binary_transform_result<int *, const int *, int *>,
        tested::ranges::in_in_out_result<int *, const int *, int *>>);

static_assert(tested::is_same_v<decltype(tested::ranges::transform(
                                    tested::declval<temporary_range>(),
                                    tested::declval<int *>(), double_value{})),
                                tested::ranges::unary_transform_result<
                                    tested::ranges::dangling, int *>>);

static_assert(tested::is_same_v<
              decltype(tested::ranges::transform(
                  tested::declval<temporary_range>(),
                  tested::declval<temporary_range>(), tested::declval<int *>(),
                  add_values{})),
              tested::ranges::binary_transform_result<
                  tested::ranges::dangling, tested::ranges::dangling, int *>>);

static_assert(tested::is_same_v<decltype(tested::ranges::generate(
                                    tested::declval<temporary_range>(),
                                    constant_generator{1})),
                                tested::ranges::dangling>);

constexpr bool classic_transform_works() {
  int input[] = {1, 2, 3, 4};
  int unary_output[4] = {};

  if (tested::transform(input, input + 4, unary_output, double_value{}) !=
      unary_output + 4) {
    return false;
  }

  if (unary_output[0] != 2 || unary_output[1] != 4 || unary_output[2] != 6 ||
      unary_output[3] != 8) {
    return false;
  }

  int right[] = {10, 20, 30, 40};
  int binary_output[4] = {};

  if (tested::transform(input, input + 4, right, binary_output, add_values{}) !=
      binary_output + 4) {
    return false;
  }

  return binary_output[0] == 11 && binary_output[1] == 22 &&
         binary_output[2] == 33 && binary_output[3] == 44;
}

static_assert(classic_transform_works());

constexpr bool ranges_transform_works() {
  int input[] = {1, 2, 3, 4};
  int unary_output[4] = {};

  auto unary_result = tested::ranges::transform(
      input, pointer_sentinel{input + 4}, unary_output, double_value{});

  if (unary_result.in != input + 4 || unary_result.out != unary_output + 4) {
    return false;
  }

  if (unary_output[0] != 2 || unary_output[1] != 4 || unary_output[2] != 6 ||
      unary_output[3] != 8) {
    return false;
  }

  int right[] = {10, 20};
  int binary_output[4] = {};

  auto binary_result = tested::ranges::transform(
      input, pointer_sentinel{input + 4}, right, pointer_sentinel{right + 2},
      binary_output, add_values{});

  return binary_result.in1 == input + 2 && binary_result.in2 == right + 2 &&
         binary_result.out == binary_output + 2 && binary_output[0] == 11 &&
         binary_output[1] == 22 && binary_output[2] == 0 &&
         binary_output[3] == 0;
}

static_assert(ranges_transform_works());

constexpr bool projected_transform_works() {
  record input[] = {{1}, {2}, {3}};
  int output[3] = {};

  int projection_calls = 0;
  int operation_calls = 0;

  auto result =
      tested::ranges::transform(input, output, counting_unary{&operation_calls},
                                counting_projection{&projection_calls});

  return result.in == input + 3 && result.out == output + 3 &&
         projection_calls == 3 && operation_calls == 3 && output[0] == 3 &&
         output[1] == 6 && output[2] == 9;
}

static_assert(projected_transform_works());

bool member_projection_transform_works() {
  record left[] = {{1}, {2}, {3}};
  record right[] = {{10}, {20}, {30}};
  int output[3] = {};

  auto result = tested::ranges::transform(left, right, output, add_values{},
                                          &record::value, &record::value);

  return result.in1 == left + 3 && result.in2 == right + 3 &&
         result.out == output + 3 && output[0] == 11 && output[1] == 22 &&
         output[2] == 33;
}

constexpr bool classic_generate_works() {
  int generated[4] = {};

  tested::generate(generated, generated + 4, sequence_generator{3});

  if (generated[0] != 3 || generated[1] != 4 || generated[2] != 5 ||
      generated[3] != 6) {
    return false;
  }

  int generated_n[4] = {};

  if (tested::generate_n(generated_n, 3, sequence_generator{7}) !=
      generated_n + 3) {
    return false;
  }

  if (generated_n[0] != 7 || generated_n[1] != 8 || generated_n[2] != 9 ||
      generated_n[3] != 0) {
    return false;
  }

  return tested::generate_n(generated_n, 0, sequence_generator{50}) ==
         generated_n;
}

static_assert(classic_generate_works());

constexpr bool ranges_generate_works() {
  int generated[4] = {};

  auto final = tested::ranges::generate(generated, sequence_generator{10});

  if (final != generated + 4) {
    return false;
  }

  if (generated[0] != 10 || generated[1] != 11 || generated[2] != 12 ||
      generated[3] != 13) {
    return false;
  }

  int generated_n[4] = {};

  auto counted_final =
      tested::ranges::generate_n(generated_n, 3, sequence_generator{20});

  if (counted_final != generated_n + 3) {
    return false;
  }

  if (generated_n[0] != 20 || generated_n[1] != 21 || generated_n[2] != 22 ||
      generated_n[3] != 0) {
    return false;
  }

  return tested::ranges::generate_n(generated_n, -2, sequence_generator{90}) ==
         generated_n;
}

static_assert(ranges_generate_works());

bool policy_transform_generate_works() {
  int input[] = {1, 2, 3};
  int right[] = {10, 20, 30};
  int unary_output[3] = {};
  int binary_output[3] = {};
  int generated[3] = {};
  int generated_n[3] = {};

  auto unary_end = tested::transform(tested::execution::seq, input, input + 3,
                                     unary_output, double_value{});

  auto binary_end = tested::transform(tested::execution::par, input, input + 3,
                                      right, binary_output, add_values{});

  tested::generate(tested::execution::par_unseq, generated, generated + 3,
                   constant_generator{8});

  auto generated_n_end = tested::generate_n(
      tested::execution::unseq, generated_n, 3, constant_generator{9});

  return unary_end == unary_output + 3 && binary_end == binary_output + 3 &&
         generated_n_end == generated_n + 3 && unary_output[0] == 2 &&
         unary_output[1] == 4 && unary_output[2] == 6 &&
         binary_output[0] == 11 && binary_output[1] == 22 &&
         binary_output[2] == 33 && generated[0] == 8 && generated[1] == 8 &&
         generated[2] == 8 && generated_n[0] == 9 && generated_n[1] == 9 &&
         generated_n[2] == 9;
}

bool ftl_test() {
  return classic_transform_works() && ranges_transform_works() &&
         projected_transform_works() && member_projection_transform_works() &&
         classic_generate_works() && ranges_generate_works() &&
         policy_transform_generate_works();
}