#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <iterator>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/span>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

using row_span = tested::span<int>;

using outer_span = tested::span<row_span>;

using pattern_span = tested::span<int>;

using joined_view = tested::ranges::join_with_view<outer_span, pattern_span>;

using joined_iterator = tested::ranges::iterator_t<joined_view>;

static_assert(tested::ranges::view<joined_view>);

static_assert(tested::ranges::input_range<joined_view>);

static_assert(tested::ranges::forward_range<joined_view>);

static_assert(tested::ranges::bidirectional_range<joined_view>);

static_assert(!tested::ranges::random_access_range<joined_view>);

static_assert(tested::ranges::common_range<joined_view>);

static_assert(!tested::ranges::sized_range<joined_view>);

static_assert(!tested::ranges::borrowed_range<joined_view>);

static_assert(tested::ranges::range<const joined_view>);

static_assert(
    tested::is_same_v<tested::ranges::range_value_t<joined_view>, int>);

static_assert(
    tested::is_same_v<tested::ranges::range_reference_t<joined_view>, int &>);

static_assert(tested::is_same_v<
              tested::ranges::range_rvalue_reference_t<joined_view>, int &&>);

static_assert(tested::is_same_v<typename joined_iterator::iterator_concept,
                                tested::bidirectional_iterator_tag>);

static_assert(tested::is_same_v<typename joined_iterator::iterator_category,
                                tested::bidirectional_iterator_tag>);

static_assert(tested::default_initializable<joined_view>);

static_assert(
    tested::is_constructible_v<joined_view, outer_span, pattern_span>);

static_assert(
    tested::is_same_v<decltype(tested::declval<const joined_view &>().base()),
                      outer_span>);

template <class V, class P>
concept can_form_join_with_view =
    requires { typename tested::ranges::join_with_view<V, P>; };

struct unrelated {};

using unrelated_pattern = tested::span<unrelated>;

static_assert(can_form_join_with_view<outer_span, pattern_span>);

static_assert(!can_form_join_with_view<outer_span, unrelated_pattern>);

constexpr bool equal_values(auto &&range, const int *expected,
                            tested::size_t expected_size) {
  tested::size_t index = 0;

  for (int value : range) {
    if (index >= expected_size || value != expected[index]) {
      return false;
    }

    ++index;
  }

  return index == expected_size;
}

constexpr bool join_with_view_works() {
  int first_values[] = {1, 2};

  int third_values[] = {3, 4, 5};

  row_span first{first_values};

  row_span empty{};

  row_span third{third_values};

  row_span rows[] = {first, empty, third};

  int pattern_values[] = {-1, -2};

  outer_span outer{rows};

  pattern_span pattern{pattern_values};

  joined_view view{outer, pattern};

  /*
   * The pattern occurs between outer elements.
   *
   * The empty middle inner range does not remove either
   * delimiter occurrence:
   *
   * [1, 2], [], [3, 4, 5]
   *       [-1, -2]
   *
   * becomes:
   *
   * 1, 2, -1, -2, -1, -2, 3, 4, 5
   */
  const int expected[] = {1, 2, -1, -2, -1, -2, 3, 4, 5};

  if (!equal_values(view, expected, 9)) {
    return false;
  }

  /*
   * Verify const traversal.
   */
  const joined_view &const_view = view;

  if (!equal_values(const_view, expected, 9)) {
    return false;
  }

  /*
   * base() returns the underlying view by value.
   */
  auto copied_base = view.base();

  if (copied_base.data() != rows || copied_base.size() != 3) {
    return false;
  }

  /*
   * This configuration must produce a common,
   * bidirectional range.
   */
  auto iterator = view.end();

  for (tested::size_t index = 9; index > 0; --index) {
    --iterator;

    if (*iterator != expected[index - 1]) {
      return false;
    }
  }

  if (iterator != view.begin()) {
    return false;
  }

  /*
   * Dereferencing delimiter positions must preserve
   * the pattern's reference.
   */
  auto delimiter = view.begin();

  ++delimiter;
  ++delimiter;

  *delimiter = 10;

  if (pattern_values[0] != 10) {
    return false;
  }

  /*
   * Restore it for later checks.
   */
  pattern_values[0] = -1;

  /*
   * An empty pattern is equivalent to ordinary joining.
   */
  pattern_span empty_pattern{};

  joined_view without_pattern{outer, empty_pattern};

  const int flat_expected[] = {1, 2, 3, 4, 5};

  if (!equal_values(without_pattern, flat_expected, 5)) {
    return false;
  }

  /*
   * A single outer element has no inserted pattern.
   */
  row_span one_row[] = {first};

  joined_view single_outer{outer_span{one_row}, pattern};

  const int single_expected[] = {1, 2};

  if (!equal_values(single_outer, single_expected, 2)) {
    return false;
  }

  /*
   * An empty outer range is empty regardless of the
   * pattern.
   */
  joined_view empty_outer{outer_span{}, pattern};

  if (empty_outer.begin() != empty_outer.end()) {
    return false;
  }

  /*
   * All-empty inner ranges still produce separators
   * between the outer elements.
   */
  row_span empty_rows[] = {row_span{}, row_span{}, row_span{}};

  joined_view all_empty{outer_span{empty_rows}, pattern};

  const int empty_expected[] = {-1, -2, -1, -2};

  if (!equal_values(all_empty, empty_expected, 4)) {
    return false;
  }

  return true;
}

/*
 * C++23 constant evaluation gives std::construct_at special treatment.
 * An equivalent ftl::construct_at cannot portably start object lifetime
 * during constant evaluation across the supported compilers.
 *
 * Replacement mode provides the required std::construct_at spelling.
 * Normal namespaced mode retains full runtime coverage.
 */
#ifdef FTL_REPLACE_STL
static_assert(join_with_view_works());
#endif

bool ftl_test() {
  return join_with_view_works();
}
