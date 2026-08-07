#ifdef FTL_REPLACE_STL
#include <limits>
#include <numeric>
namespace tested = std;
#else
#include <ftl/limits>
#include <ftl/numeric>
namespace tested = ftl;
#endif

#ifndef __cpp_lib_constexpr_numeric
#error "__cpp_lib_constexpr_numeric must be defined by <numeric>"
#endif

#ifndef __cpp_lib_gcd_lcm
#error "__cpp_lib_gcd_lcm must be defined by <numeric>"
#endif

#ifndef __cpp_lib_interpolate
#error "__cpp_lib_interpolate must be defined by <numeric>"
#endif

#ifndef __cpp_lib_ranges_iota
#error "__cpp_lib_ranges_iota must be defined by <numeric>"
#endif

static_assert(__cpp_lib_constexpr_numeric >= 201911L);
static_assert(__cpp_lib_gcd_lcm >= 201606L);
static_assert(__cpp_lib_interpolate >= 201902L);
static_assert(__cpp_lib_ranges_iota >= 202202L);

struct multiply {
  constexpr int operator()(int left, int right) const { return left * right; }
};

struct add {
  constexpr int operator()(int left, int right) const { return left + right; }
};

struct subtract {
  constexpr int operator()(int left, int right) const { return left - right; }
};

struct square {
  constexpr int operator()(int value) const { return value * value; }
};

struct move_accumulator {
  int value = 0;

  constexpr move_accumulator() = default;
  constexpr explicit move_accumulator(int value) : value(value) {}

  constexpr move_accumulator(const move_accumulator &) = default;

  constexpr move_accumulator &operator=(const move_accumulator &) = default;

  friend constexpr move_accumulator operator+(move_accumulator &&left,
                                              int right) {

    return move_accumulator{left.value + right};
  }

  friend constexpr move_accumulator operator+(const move_accumulator &,
                                              int) = delete;
};

struct move_combine {
  constexpr move_accumulator operator()(move_accumulator &&left,
                                        int right) const {

    return move_accumulator{left.value + right};
  }

  constexpr move_accumulator operator()(const move_accumulator &,
                                        int) const = delete;
};

struct multiply_pair {
  constexpr int operator()(int left, int right) const { return left * right; }
};

struct not_execution_policy {};

template <class Policy>
concept has_policy_reduce = requires(Policy &&policy, int *first, int *last) {
  tested::reduce(static_cast<Policy &&>(policy), first, last);
};

static_assert(!has_policy_reduce<not_execution_policy>);

constexpr bool test_accumulate() {
  int values[]{1, 2, 3, 4};

  if (tested::accumulate(values, values + 4, 5) != 15)
    return false;

  if (tested::accumulate(values, values + 4, 1, multiply{}) != 24)
    return false;

  const auto moved =
      tested::accumulate(values, values + 4, move_accumulator{5});

  if (moved.value != 15)
    return false;

  const auto moved_op = tested::accumulate(values, values + 4,
                                           move_accumulator{5}, move_combine{});

  return moved_op.value == 15;
}

constexpr bool test_reduce() {
  int values[]{1, 2, 3, 4};

  if (tested::reduce(values, values + 4) != 10)
    return false;

  if (tested::reduce(values, values + 4, 10) != 20)
    return false;

  return tested::reduce(values, values + 4, 1, multiply{}) == 24;
}

constexpr bool test_inner_product() {
  int left[]{1, 2, 3};
  int right[]{4, 5, 6};

  if (tested::inner_product(left, left + 3, right, 0) != 32)
    return false;

  if (tested::inner_product(left, left + 3, right, 0, add{}, subtract{}) != -9)
    return false;

  const auto moved =
      tested::inner_product(left, left + 3, right, move_accumulator{},
                            move_combine{}, multiply_pair{});

  return moved.value == 32;
}

constexpr bool test_transform_reduce() {
  int left[]{1, 2, 3};
  int right[]{4, 5, 6};

  if (tested::transform_reduce(left, left + 3, right, 0) != 32)
    return false;

  if (tested::transform_reduce(left, left + 3, right, 0, add{}, subtract{}) !=
      -9)
    return false;

  return tested::transform_reduce(left, left + 3, 0, add{}, square{}) == 14;
}

constexpr bool test_partial_sum() {
  int input[]{1, 2, 3, 4};
  int output[4]{};

  if (tested::partial_sum(input, input + 4, output) != output + 4)
    return false;

  if (output[0] != 1 || output[1] != 3 || output[2] != 6 || output[3] != 10)
    return false;

  int products[]{1, 2, 3, 4};

  if (tested::partial_sum(products, products + 4, products, multiply{}) !=
      products + 4)
    return false;

  return products[0] == 1 && products[1] == 2 && products[2] == 6 &&
         products[3] == 24;
}

constexpr bool test_exclusive_scan() {
  int input[]{1, 2, 3, 4};
  int output[4]{};

  if (tested::exclusive_scan(input, input + 4, output, 0) != output + 4)
    return false;

  if (output[0] != 0 || output[1] != 1 || output[2] != 3 || output[3] != 6)
    return false;

  int inplace[]{1, 2, 3, 4};

  if (tested::exclusive_scan(inplace, inplace + 4, inplace, 1, multiply{}) !=
      inplace + 4)
    return false;

  return inplace[0] == 1 && inplace[1] == 1 && inplace[2] == 2 &&
         inplace[3] == 6;
}

constexpr bool test_inclusive_scan() {
  int input[]{1, 2, 3, 4};
  int output[4]{};

  if (tested::inclusive_scan(input, input + 4, output) != output + 4)
    return false;

  if (output[0] != 1 || output[1] != 3 || output[2] != 6 || output[3] != 10)
    return false;

  int product[4]{};

  if (tested::inclusive_scan(input, input + 4, product, multiply{}) !=
      product + 4)
    return false;

  if (product[0] != 1 || product[1] != 2 || product[2] != 6 || product[3] != 24)
    return false;

  int initialized[4]{};

  if (tested::inclusive_scan(input, input + 4, initialized, add{}, 10) !=
      initialized + 4)
    return false;

  return initialized[0] == 11 && initialized[1] == 13 && initialized[2] == 16 &&
         initialized[3] == 20;
}

constexpr bool test_transform_exclusive_scan() {
  int input[]{1, 2, 3, 4};
  int output[4]{};

  if (tested::transform_exclusive_scan(input, input + 4, output, 0, add{},
                                       square{}) != output + 4)
    return false;

  if (output[0] != 0 || output[1] != 1 || output[2] != 5 || output[3] != 14)
    return false;

  int inplace[]{1, 2, 3, 4};

  if (tested::transform_exclusive_scan(inplace, inplace + 4, inplace, 0, add{},
                                       square{}) != inplace + 4)
    return false;

  return inplace[0] == 0 && inplace[1] == 1 && inplace[2] == 5 &&
         inplace[3] == 14;
}

constexpr bool test_transform_inclusive_scan() {
  int input[]{1, 2, 3, 4};
  int output[4]{};

  if (tested::transform_inclusive_scan(input, input + 4, output, add{},
                                       square{}) != output + 4)
    return false;

  if (output[0] != 1 || output[1] != 5 || output[2] != 14 || output[3] != 30)
    return false;

  int initialized[4]{};

  if (tested::transform_inclusive_scan(input, input + 4, initialized, add{},
                                       square{}, 10) != initialized + 4)
    return false;

  return initialized[0] == 11 && initialized[1] == 15 && initialized[2] == 24 &&
         initialized[3] == 40;
}

constexpr bool test_adjacent_difference() {
  int input[]{1, 2, 3, 4};
  int output[4]{};

  if (tested::adjacent_difference(input, input + 4, output) != output + 4)
    return false;

  if (output[0] != 1 || output[1] != 1 || output[2] != 1 || output[3] != 1)
    return false;

  int inplace[]{1, 2, 3, 4};

  if (tested::adjacent_difference(inplace, inplace + 4, inplace, add{}) !=
      inplace + 4)
    return false;

  return inplace[0] == 1 && inplace[1] == 3 && inplace[2] == 5 &&
         inplace[3] == 7;
}

constexpr bool test_iota() {
  int values[4]{};

  tested::iota(values, values + 4, -2);

  if (values[0] != -2 || values[1] != -1 || values[2] != 0 || values[3] != 1)
    return false;

  int iterator_values[3]{};

  auto iterator_result =
      tested::ranges::iota(iterator_values, iterator_values + 3, 4);

  if (iterator_result.out != iterator_values + 3 ||
      iterator_result.value != 7 || iterator_values[0] != 4 ||
      iterator_values[1] != 5 || iterator_values[2] != 6)
    return false;

  int range_values[3]{};

  auto range_result = tested::ranges::iota(range_values, 7);

  return range_result.out == range_values + 3 && range_result.value == 10 &&
         range_values[0] == 7 && range_values[1] == 8 && range_values[2] == 9;
}

constexpr bool test_gcd_lcm() {
  if (tested::gcd(0, 0) != 0)
    return false;

  if (tested::gcd(48, 18) != 6)
    return false;

  if (tested::gcd(-48, 18) != 6)
    return false;

  if (tested::lcm(0, 18) != 0)
    return false;

  if (tested::lcm(12, 18) != 36)
    return false;

  return tested::lcm(-12, 18) == 36;
}

constexpr bool test_midpoint() {
  if (tested::midpoint(2, 5) != 3)
    return false;

  if (tested::midpoint(5, 2) != 4)
    return false;

  constexpr int lowest = tested::numeric_limits<int>::min();

  constexpr int highest = tested::numeric_limits<int>::max();

  if (tested::midpoint(lowest, highest) != -1)
    return false;

  if (tested::midpoint(highest, lowest) != 0)
    return false;

  constexpr double maximum = tested::numeric_limits<double>::max();

  if (tested::midpoint(maximum, maximum) != maximum)
    return false;

  int values[]{0, 1, 2, 3, 4, 5};

  if (tested::midpoint(values, values + 6) != values + 3)
    return false;

  return tested::midpoint(values + 5, values) == values + 3;
}

static_assert(test_accumulate());
static_assert(test_reduce());
static_assert(test_inner_product());
static_assert(test_transform_reduce());
static_assert(test_partial_sum());
static_assert(test_exclusive_scan());
static_assert(test_inclusive_scan());
static_assert(test_transform_exclusive_scan());
static_assert(test_transform_inclusive_scan());
static_assert(test_adjacent_difference());
static_assert(test_iota());
static_assert(test_gcd_lcm());
static_assert(test_midpoint());

bool test_policy_overloads() {
  int input[]{1, 2, 3, 4};
  int second[]{4, 5, 6, 7};
  int output[4]{};

  const auto policy = tested::execution::seq;

  if (tested::reduce(policy, input, input + 4, 0) != 10)
    return false;

  if (tested::transform_reduce(
          policy, input, input + 4, second, 0) != 60)
    return false;

  tested::exclusive_scan(
      policy, input, input + 4, output, 0);

  tested::inclusive_scan(
      policy, input, input + 4, output);

  tested::transform_exclusive_scan(
      policy, input, input + 4, output,
      0, add{}, square{});

  tested::transform_inclusive_scan(
      policy, input, input + 4, output,
      add{}, square{});

  tested::adjacent_difference(
      policy, input, input + 4, output);

  return true;
}

bool ftl_test() {
  return test_accumulate() && test_reduce() && test_inner_product() &&
         test_transform_reduce() && test_partial_sum() &&
         test_exclusive_scan() && test_inclusive_scan() &&
         test_transform_exclusive_scan() && test_transform_inclusive_scan() &&
         test_adjacent_difference() && test_iota() && test_gcd_lcm() &&
         test_midpoint() && test_policy_overloads();
}
