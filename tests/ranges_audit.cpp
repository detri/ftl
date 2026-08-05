#ifdef FTL_REPLACE_STL
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

using base_view = tested::ranges::ref_view<int[4]>;
using stride_type = decltype(tested::ranges::views::stride(tested::declval<base_view>(), 2));
using zip_type = decltype(tested::ranges::views::zip(tested::declval<base_view>(), tested::declval<base_view>()));
using enumerate_type = decltype(tested::ranges::views::enumerate(tested::declval<base_view>()));
using adjacent_type = decltype(tested::ranges::views::adjacent<2>(tested::declval<base_view>()));
using slide_type = decltype(tested::ranges::views::slide(tested::declval<base_view>(), 2));
using chunk_type = decltype(tested::ranges::views::chunk(tested::declval<base_view>(), 2));
using cartesian_type = decltype(tested::ranges::views::cartesian_product(tested::declval<base_view>(), tested::declval<base_view>()));

static_assert(tested::ranges::random_access_range<stride_type> && tested::ranges::sized_range<stride_type>);
static_assert(tested::ranges::random_access_range<zip_type> && tested::ranges::sized_range<zip_type>);
static_assert(tested::ranges::random_access_range<enumerate_type> && tested::ranges::sized_range<enumerate_type>);
static_assert(tested::ranges::random_access_range<adjacent_type> && tested::ranges::sized_range<adjacent_type>);
static_assert(tested::ranges::random_access_range<slide_type> && tested::ranges::sized_range<slide_type>);
static_assert(tested::ranges::random_access_range<chunk_type> && tested::ranges::sized_range<chunk_type>);
static_assert(tested::ranges::random_access_range<cartesian_type> && tested::ranges::sized_range<cartesian_type>);

static_assert(tested::ranges::range<const stride_type>);
static_assert(tested::ranges::range<const zip_type>);
static_assert(tested::ranges::range<const enumerate_type>);
static_assert(tested::ranges::range<const adjacent_type>);
static_assert(tested::ranges::range<const slide_type>);
static_assert(tested::ranges::range<const chunk_type>);
static_assert(tested::ranges::random_access_range<const cartesian_type>);
static_assert(tested::convertible_to<
    tested::ranges::iterator_t<cartesian_type>,
    tested::ranges::iterator_t<const cartesian_type>>);

static_assert(tested::ranges::borrowed_range<stride_type>);
static_assert(tested::ranges::borrowed_range<zip_type>);
static_assert(tested::ranges::borrowed_range<enumerate_type>);
static_assert(tested::ranges::borrowed_range<adjacent_type>);
static_assert(tested::ranges::borrowed_range<slide_type>);
static_assert(tested::ranges::borrowed_range<chunk_type>);

bool ftl_test() { return true; }
