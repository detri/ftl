#ifdef FTL_REPLACE_STL
#include <limits>
#include <type_traits>
#include <valarray>
namespace tested = std;
#else
#include <ftl/limits>
#include <ftl/type_traits>
#include <ftl/valarray>
namespace tested = ftl;
#endif

static_assert(tested::is_same_v<tested::valarray<int>::value_type, int>);
static_assert(
    tested::is_same_v<
        decltype(tested::begin(tested::declval<tested::valarray<int> &>())),
        int *>);
static_assert(
    tested::is_same_v<decltype(tested::begin(
                          tested::declval<const tested::valarray<int> &>())),
                      const int *>);

using int_array = tested::valarray<int>;
using bool_array = tested::valarray<bool>;
using size_array = tested::valarray<tested::size_t>;
static_assert(
    tested::is_same_v<decltype(tested::declval<int_array &>()
                                   [tested::declval<tested::slice>()]),
                      tested::slice_array<int>>);
static_assert(
    tested::is_same_v<decltype(tested::declval<const int_array &>()
                                   [tested::declval<tested::slice>()]),
                      int_array>);
static_assert(
    tested::is_same_v<decltype(tested::declval<int_array &>()
                                   [tested::declval<const tested::gslice &>()]),
                      tested::gslice_array<int>>);
static_assert(
    tested::is_same_v<decltype(tested::declval<const int_array &>()
                                   [tested::declval<const tested::gslice &>()]),
                      int_array>);
static_assert(
    tested::is_same_v<decltype(tested::declval<int_array &>()
                                   [tested::declval<const bool_array &>()]),
                      tested::mask_array<int>>);
static_assert(
    tested::is_same_v<decltype(tested::declval<const int_array &>()
                                   [tested::declval<const bool_array &>()]),
                      int_array>);
static_assert(
    tested::is_same_v<decltype(tested::declval<int_array &>()
                                   [tested::declval<const size_array &>()]),
                      tested::indirect_array<int>>);
static_assert(
    tested::is_same_v<decltype(tested::declval<const int_array &>()
                                   [tested::declval<const size_array &>()]),
                      int_array>);

template <class Proxy>
concept complete_proxy_surface =
    requires(const Proxy proxy, const Proxy other, const int_array values) {
      typename Proxy::value_type;
      { proxy = 1 } -> tested::same_as<void>;
      { proxy = values } -> tested::same_as<void>;
      { proxy = other } -> tested::same_as<const Proxy &>;
      { proxy *= values } -> tested::same_as<void>;
      { proxy /= values } -> tested::same_as<void>;
      { proxy %= values } -> tested::same_as<void>;
      { proxy += values } -> tested::same_as<void>;
      { proxy -= values } -> tested::same_as<void>;
      { proxy ^= values } -> tested::same_as<void>;
      { proxy &= values } -> tested::same_as<void>;
      { proxy |= values } -> tested::same_as<void>;
      { proxy <<= values } -> tested::same_as<void>;
      { proxy >>= values } -> tested::same_as<void>;
    };
static_assert(complete_proxy_surface<tested::slice_array<int>>);
static_assert(complete_proxy_surface<tested::gslice_array<int>>);
static_assert(complete_proxy_surface<tested::mask_array<int>>);
static_assert(complete_proxy_surface<tested::indirect_array<int>>);

template <class T>
concept complete_valarray_surface =
    requires(tested::valarray<T> value, const tested::valarray<T> constant,
             const T scalar, T unary(T), T unary_reference(const T &)) {
      typename tested::valarray<T>::value_type;
      tested::valarray<T>{};
      tested::valarray<T>{tested::size_t{3}};
      tested::valarray<T>{scalar, tested::size_t{3}};
      tested::valarray<T>{&scalar, tested::size_t{1}};
      { value = scalar } -> tested::same_as<tested::valarray<T> &>;
      { value.size() } -> tested::same_as<tested::size_t>;
      { constant.sum() } -> tested::same_as<T>;
      { constant.min() } -> tested::same_as<T>;
      { constant.max() } -> tested::same_as<T>;
      { constant.shift(1) } -> tested::same_as<tested::valarray<T>>;
      { constant.cshift(1) } -> tested::same_as<tested::valarray<T>>;
      { constant.apply(unary) } -> tested::same_as<tested::valarray<T>>;
      {
        constant.apply(unary_reference)
      } -> tested::same_as<tested::valarray<T>>;
      { value.resize(3, scalar) } -> tested::same_as<void>;
    };
static_assert(complete_valarray_surface<int>);

static_assert(requires(const int_array a, const int_array b, int scalar) {
  { a * b } -> tested::same_as<int_array>;
  { a / scalar } -> tested::same_as<int_array>;
  { scalar % a } -> tested::same_as<int_array>;
  { a + b } -> tested::same_as<int_array>;
  { a - scalar } -> tested::same_as<int_array>;
  { scalar ^ a } -> tested::same_as<int_array>;
  { a && b } -> tested::same_as<bool_array>;
  { a == scalar } -> tested::same_as<bool_array>;
  { scalar < a } -> tested::same_as<bool_array>;
});

static_assert(requires(const tested::valarray<double> a, double scalar) {
  { tested::abs(a) } -> tested::same_as<tested::valarray<double>>;
  { tested::acos(a) } -> tested::same_as<tested::valarray<double>>;
  { tested::atan2(a, scalar) } -> tested::same_as<tested::valarray<double>>;
  { tested::pow(scalar, a) } -> tested::same_as<tested::valarray<double>>;
  { tested::tanh(a) } -> tested::same_as<tested::valarray<double>>;
});

struct no_default {
  int value;

  no_default() = delete;
  explicit no_default(int input) : value(input) {}
  no_default(const no_default &) = default;
  no_default &operator=(const no_default &) = default;
};

struct throwing_copy {
  static inline int alive = 0;
  static inline int copies_before_throw = -1;

  int value;

  throwing_copy() = delete;
  explicit throwing_copy(int input) : value(input) { ++alive; }

  throwing_copy(const throwing_copy &other) : value(other.value) {
    if (copies_before_throw == 0)
      throw 1;
    if (copies_before_throw > 0)
      --copies_before_throw;
    ++alive;
  }

  throwing_copy &operator=(const throwing_copy &other) {
    value = other.value;
    return *this;
  }

  ~throwing_copy() { --alive; }
};

int square(int value) { return value * value; }

bool non_default_value_construction_works() {
  no_default seed{42};
  tested::valarray<no_default> values(seed, 3);
  if (values.size() != 3 || values[0].value != 42 || values[2].value != 42)
    return false;

  values.resize(5, seed);
  return values.size() == 5 && values[0].value == 42 && values[4].value == 42;
}

bool throwing_construction_rolls_back() {
  throwing_copy::alive = 0;
  throwing_copy::copies_before_throw = -1;

  {
    throwing_copy seed{7};
    if (throwing_copy::alive != 1)
      return false;

    throwing_copy::copies_before_throw = 1;

    try {
      tested::valarray<throwing_copy> values(seed, 3);
      (void)values;
      return false;
    } catch (...) {
    }

    if (throwing_copy::alive != 1)
      return false;
  }

  return throwing_copy::alive == 0;
}

bool empty_iterators_work() {
  tested::valarray<int> values;
  return tested::begin(values) == tested::end(values);
}

bool extreme_shift_works() {
  tested::valarray<int> values{1, 2, 3, 4, 5};
  const int minimum = tested::numeric_limits<int>::min();

  const auto shifted = values.shift(minimum);
  for (tested::size_t i = 0; i < shifted.size(); ++i) {
    if (shifted[i] != 0)
      return false;
  }

  const auto rotated = values.cshift(minimum);
  const auto reduced = values.cshift(minimum % static_cast<int>(values.size()));
  if (rotated.size() != reduced.size())
    return false;

  for (tested::size_t i = 0; i < rotated.size(); ++i) {
    if (rotated[i] != reduced[i])
      return false;
  }

  return true;
}

bool owning_surface_works() {
  tested::valarray<int> values{1, 2, 3, 4};
  if (values.size() != 4 || values.sum() != 10 || values.min() != 1 ||
      values.max() != 4)
    return false;
  const auto arithmetic = (values + 2) * values;
  if (arithmetic[0] != 3 || arithmetic[3] != 24)
    return false;
  const auto compared = values >= 3;
  if (compared[0] || compared[1] || !compared[2] || !compared[3])
    return false;
  const auto shifted = values.shift(1);
  const auto rotated = values.cshift(-1);
  if (shifted[0] != 2 || shifted[3] != 0 || rotated[0] != 4 || rotated[1] != 1)
    return false;
  const auto applied = values.apply(square);
  if (applied[0] != 1 || applied[3] != 16)
    return false;
  values.resize(3, 7);
  return values.size() == 3 && values.sum() == 21;
}

bool math_surface_works() {
  tested::valarray<double> values{1.0, 4.0, 9.0};
  const auto roots = tested::sqrt(values);
  const auto powers = tested::pow(roots, 2.0);
  return roots[0] == 1.0 && roots[1] == 2.0 && roots[2] == 3.0 &&
         powers[0] == 1.0 && powers[1] == 4.0 && powers[2] == 9.0;
}

bool selector_surface_works() {
  tested::valarray<int> values{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  values[tested::slice(1, 3, 2)] = tested::valarray<int>{10, 20, 30};
  if (values[1] != 10 || values[3] != 20 || values[5] != 30)
    return false;

  tested::valarray<bool> mask{true,  false, true,  false, true,
                              false, false, false, false, false};
  values[mask] = 7;
  if (values[0] != 7 || values[2] != 7 || values[4] != 7)
    return false;

  tested::valarray<tested::size_t> indirect{9, 7, 8};
  values[indirect] = tested::valarray<int>{90, 70, 80};
  if (values[7] != 70 || values[8] != 80 || values[9] != 90)
    return false;

  tested::valarray<tested::size_t> lengths{2, 2};
  tested::valarray<tested::size_t> strides{4, 1};
  tested::gslice selection(0, lengths, strides);
  const tested::valarray<int> selected =
      static_cast<const tested::valarray<int> &>(values)[selection];
  return selected.size() == 4 && selected[0] == 7 && selected[1] == 10 &&
         selected[2] == 7 && selected[3] == 30;
}

bool ftl_test() {
  return non_default_value_construction_works() &&
         throwing_construction_rolls_back() && empty_iterators_work() &&
         extreme_shift_works() && owning_surface_works() &&
         math_surface_works() && selector_surface_works();
}
