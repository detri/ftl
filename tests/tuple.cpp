#ifdef FTL_REPLACE_STL
#include <array>
#include <ranges>
#include <tuple>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/ranges>
#include <ftl/tuple>
namespace tested = ftl;
#endif

struct no_throw_call {
  constexpr int operator()(int value) const noexcept { return value; }
};

using one_int = tested::tuple<int>;
using pointer_subrange = tested::ranges::subrange<int *, int *>;

using pointer_tuple = tested::tuple<int *, int *>;

template <class T> using lvalue_qualifier = T &;

static_assert(tested::tuple_size_v<pointer_subrange> == 2);

static_assert(tested::tuple_size_v<const pointer_subrange> == 2);

static_assert(
    tested::is_same_v<tested::tuple_element_t<0, pointer_subrange>, int *>);

static_assert(
    tested::is_same_v<tested::tuple_element_t<1, pointer_subrange>, int *>);

/*
 * const subrange still returns its iterator and sentinel
 * by value. These must be int*, not int* const.
 */
static_assert(tested::is_same_v<
              tested::tuple_element_t<0, const pointer_subrange>, int *>);

static_assert(tested::is_same_v<
              tested::tuple_element_t<1, const pointer_subrange>, int *>);

static_assert(tested::is_constructible_v<pointer_tuple, pointer_subrange &>);

static_assert(
    tested::is_constructible_v<tested::pair<int *, int *>, pointer_subrange &>);

static_assert(
    tested::is_same_v<tested::common_type_t<pointer_tuple, pointer_subrange>,
                      pointer_tuple>);

static_assert(tested::is_same_v<typename tested::basic_common_reference<
                                    pointer_tuple, pointer_subrange,
                                    lvalue_qualifier, lvalue_qualifier>::type,
                                tested::tuple<int *&, int *&>>);
static_assert(tested::is_constructible_v<tested::tuple<long>, one_int &>);
static_assert(tested::is_constructible_v<tested::tuple<long>, const one_int &>);
static_assert(tested::is_constructible_v<tested::tuple<long>, one_int &&>);
static_assert(
    tested::is_constructible_v<tested::tuple<long>, const one_int &&>);
static_assert(
    tested::is_same_v<decltype(tested::tuple(tested::pair<int, long>{})),
                      tested::tuple<int, long>>);
static_assert(tested::is_same_v<tested::common_type_t<tested::tuple<int, short>,
                                                      tested::array<long, 2>>,
                                tested::tuple<long, long>>);
static_assert(noexcept(tested::apply(tested::declval<no_throw_call>(),
                                     tested::declval<tested::tuple<int> &>())));

constexpr bool subrange_tuple_interop_works() {
  int values[] = {1, 2, 3, 4};

  pointer_subrange subrange{values, values + 4};

  /*
   * Both public spellings must work.
   */
  if (tested::ranges::get<0>(subrange) != values ||
      tested::ranges::get<1>(subrange) != values + 4) {
    return false;
  }

  if (tested::get<0>(subrange) != values ||
      tested::get<1>(subrange) != values + 4) {
    return false;
  }

  /*
   * Structured bindings use tuple_size,
   * tuple_element, and ADL get.
   */
#ifdef FTL_REPLACE_STL

  auto [first, last] = subrange;

  if (first != values || last != values + 4) {
    return false;
  }

  const pointer_subrange constant_subrange{values, values + 4};

  auto [constant_first, constant_last] = constant_subrange;

  if (constant_first != values || constant_last != values + 4) {
    return false;
  }

#endif

  /*
   * Existing C++23 tuple-like constructors.
   */
  pointer_tuple tuple_value{subrange};

  tested::pair<int *, int *> pair_value{subrange};

  if (tested::get<0>(tuple_value) != values ||
      tested::get<1>(tuple_value) != values + 4 || pair_value.first != values ||
      pair_value.second != values + 4) {
    return false;
  }

  /*
   * Existing tuple-like comparison.
   */
  if (!(tuple_value == subrange)) {
    return false;
  }

  /*
   * apply must find ranges::get through ADL.
   */
  const auto distance = tested::apply(
      [](int *begin, int *end) constexpr { return end - begin; }, subrange);

  if (distance != 4) {
    return false;
  }

  /*
   * tuple_cat uses tuple_size, tuple_element,
   * and get for every tuple-like input.
   */
  auto joined = tested::tuple_cat(subrange, tested::tuple<int>{9});

  static_assert(
      tested::is_same_v<decltype(joined), tested::tuple<int *, int *, int>>);

  if (tested::get<0>(joined) != values ||
      tested::get<1>(joined) != values + 4 || tested::get<2>(joined) != 9) {
    return false;
  }

  return true;
}

static_assert(subrange_tuple_interop_works());

constexpr bool tuple_works() {
  tested::tuple<int, long> value{1, 2};
  tested::get<0>(value) = 3;
  auto made = tested::make_tuple(4, 5L);
  int a = 0, b = 0;
  tested::tie(a, b) = made;
  auto joined = tested::tuple_cat(value, made);
  tested::array<int, 2> source{7, 8};
  tested::tuple<long, long> from_array(source);
  return tested::tuple_size_v<decltype(value)> == 2 &&
         tested::get<int>(value) == 3 && a == 4 && b == 5 &&
         tested::get<2>(joined) == 4 && tested::get<1>(from_array) == 8 &&
         value < tested::tuple<int, long>{4, 0} &&
         tested::apply([](int x, long y) { return x + y; }, value) == 5;
}

static_assert(tuple_works());
bool ftl_test() { return tuple_works() && subrange_tuple_interop_works(); }