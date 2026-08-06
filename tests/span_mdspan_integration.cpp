#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <mdspan>
#include <ranges>
#include <span>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/mdspan>
#include <ftl/ranges>
#include <ftl/span>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

using fixed_span = tested::span<int, 6>;

using dynamic_span = tested::span<int>;

using fixed_mdspan = tested::mdspan<int, tested::extents<tested::size_t, 2, 3>>;

using dynamic_mdspan = tested::mdspan<int, tested::dextents<tested::size_t, 2>>;

static_assert(tested::ranges::view<fixed_span>);

static_assert(tested::ranges::view<dynamic_span>);

static_assert(tested::ranges::borrowed_range<fixed_span>);

static_assert(tested::ranges::borrowed_range<dynamic_span>);

static_assert(tested::ranges::contiguous_range<fixed_span>);

static_assert(tested::ranges::sized_range<fixed_span>);

static_assert(tested::ranges::common_range<fixed_span>);

static_assert(tested::ranges::random_access_range<fixed_span>);

static_assert(!tested::ranges::range<fixed_mdspan>);

static_assert(!tested::ranges::range<dynamic_mdspan>);

constexpr bool span_mdspan_integration_works() {
  int values[] = {1, 2, 3, 4, 5, 6};

  tested::span storage{values};

  static_assert(tested::is_same_v<decltype(storage), tested::span<int, 6>>);

  if (tested::ranges::data(storage) != values) {
    return false;
  }

  if (tested::ranges::size(storage) != 6) {
    return false;
  }

  if (tested::ranges::begin(storage) != values) {
    return false;
  }

  if (tested::ranges::end(storage) != values + 6) {
    return false;
  }

  fixed_mdspan fixed_grid{storage.data()};

  if (fixed_grid.extent(0) != 2 || fixed_grid.extent(1) != 3) {
    return false;
  }

  if (fixed_grid[0, 0] != 1 || fixed_grid[1, 2] != 6) {
    return false;
  }

  fixed_grid[1, 1] = 50;

  if (storage[4] != 50) {
    return false;
  }

  dynamic_mdspan dynamic_grid{storage.data(), 2, 3};

  if (dynamic_grid.extent(0) != 2 || dynamic_grid.extent(1) != 3) {
    return false;
  }

  if (dynamic_grid[1, 1] != 50) {
    return false;
  }

  auto second_row = storage.subspan<3, 3>();

  static_assert(tested::is_same_v<decltype(second_row), tested::span<int, 3>>);

  tested::mdspan row{second_row.data(), second_row.size()};

  if (row.extent(0) != 3 || row[0] != 4 || row[1] != 50 || row[2] != 6) {
    return false;
  }

  return true;
}

static_assert(span_mdspan_integration_works());

bool ftl_test() { return span_mdspan_integration_works(); }