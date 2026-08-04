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

template<class T>
struct pointer_view : tested::ranges::view_base {
    T* first{};
    T* last{};
    constexpr pointer_view() = default;
    constexpr pointer_view(T* begin, T* end) : first(begin), last(end) {}
    constexpr T* begin() const { return first; }
    constexpr T* end() const { return last; }
    constexpr tested::size_t size() const { return static_cast<tested::size_t>(last - first); }
};

constexpr bool elements_works() {
    tested::pair<int, long> pairs[] = {{1, 10}, {2, 20}};
    pointer_view base(pairs, pairs + 2);
    auto first = base | tested::ranges::views::elements<0>;
    auto second = base | tested::ranges::views::values;
    *first.begin() = 7;
    second.begin()[1] = 30;
    return pairs[0].first == 7 && pairs[1].second == 30 &&
        first.size() == 2 && *tested::ranges::views::keys(base).begin() == 7;
}

using elements_type = tested::ranges::elements_view<
    pointer_view<tested::pair<int, long>>, 0>;
static_assert(tested::ranges::random_access_range<elements_type>);
static_assert(tested::ranges::sized_range<elements_type>);
static_assert(tested::same_as<tested::ranges::range_value_t<elements_type>, int>);
static_assert(elements_works());

bool ftl_test() { return elements_works(); }
