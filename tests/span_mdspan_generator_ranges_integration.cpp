#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <generator>
#include <mdspan>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/generator>
#include <ftl/mdspan>
#include <ftl/ranges>
#include <ftl/span>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

using storage_span = tested::span<int, 6>;

using matrix_extents = tested::extents<tested::size_t, 2, 3>;

using matrix_view = tested::mdspan<int, matrix_extents>;

using reference_generator = tested::generator<int &>;

struct is_even {
  [[nodiscard]]
  constexpr bool operator()(int value) const noexcept {
    return value % 2 == 0;
  }
};

struct preserve_reference {
  [[nodiscard]]
  constexpr int &operator()(int &value) const noexcept {
    return value;
  }
};

using mutation_pipeline =
    decltype(tested::declval<reference_generator>() |
             tested::ranges::views::filter(is_even{}) |
             tested::ranges::views::transform(preserve_reference{}) |
             tested::ranges::views::take(2));

static_assert(tested::ranges::view<storage_span>);

static_assert(tested::ranges::borrowed_range<storage_span>);

static_assert(tested::ranges::contiguous_range<storage_span>);

static_assert(tested::ranges::sized_range<storage_span>);

static_assert(!tested::ranges::range<matrix_view>);

static_assert(tested::ranges::view<reference_generator>);

static_assert(tested::ranges::input_range<reference_generator>);

static_assert(!tested::ranges::forward_range<reference_generator>);

static_assert(tested::ranges::view<mutation_pipeline>);

static_assert(tested::ranges::input_range<mutation_pipeline>);

static_assert(!tested::ranges::forward_range<mutation_pipeline>);

static_assert(!tested::ranges::borrowed_range<mutation_pipeline>);

static_assert(
    tested::is_same_v<tested::ranges::range_value_t<mutation_pipeline>, int>);

static_assert(tested::is_same_v<
              tested::ranges::range_reference_t<mutation_pipeline>, int &>);

#ifdef FTL_REPLACE_STL

tested::generator<int &> yield_span(tested::span<int> values) {
  co_yield tested::ranges::elements_of(values);
}

#endif

bool ftl_test() {
#ifdef FTL_REPLACE_STL
  int values[] = {1, 2, 3, 4, 5, 6};

  storage_span storage{values};

  matrix_view matrix{storage.data()};

  if (matrix.extent(0) != 2 || matrix.extent(1) != 3 || matrix.size() != 6) {
    return false;
  }

  /*
   * Mutate the common storage through mdspan.
   */
  matrix[0, 1] = 20;
  matrix[1, 0] = 40;

  if (storage[1] != 20 || storage[3] != 40) {
    return false;
  }

  /*
   * Yield the middle four span elements by reference:
   *
   *     20, 3, 40, 5
   *
   * Filtering selects 20 and 40. Transform preserves
   * the references, allowing mutation of the original
   * span and mdspan storage.
   */
  auto pipeline = yield_span(storage.subspan(1, 4)) |
                  tested::ranges::views::filter(is_even{}) |
                  tested::ranges::views::transform(preserve_reference{}) |
                  tested::ranges::views::take(2);

  tested::size_t count = 0;

  for (int &value : pipeline) {
    value += 100;
    ++count;
  }

  if (count != 2) {
    return false;
  }

  const int expected[] = {1, 120, 3, 140, 5, 6};

  for (tested::size_t index = 0; index < storage.size(); ++index) {
    if (storage[index] != expected[index]) {
      return false;
    }
  }

  /*
   * Verify that the mutations performed through the
   * generator pipeline remain visible through mdspan.
   */
  if (matrix[0, 0] != 1 || matrix[0, 1] != 120 || matrix[0, 2] != 3 ||
      matrix[1, 0] != 140 || matrix[1, 1] != 5 || matrix[1, 2] != 6) {
    return false;
  }
#endif

  return true;
}