#ifdef FTL_REPLACE_STL
#include <ranges>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/ranges>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct inner_view : tested::ranges::view_base {
    int* first{};
    int* last{};
    constexpr inner_view() = default;
    constexpr inner_view(int* begin, int* end) : first(begin), last(end) {}
    constexpr int* begin() const { return first; }
    constexpr int* end() const { return last; }
};

struct outer_view : tested::ranges::view_base {
    inner_view* first{};
    inner_view* last{};
    constexpr outer_view() = default;
    constexpr outer_view(inner_view* begin, inner_view* end) : first(begin), last(end) {}
    constexpr inner_view* begin() const { return first; }
    constexpr inner_view* end() const { return last; }
};

struct copy_inner {
    constexpr inner_view operator()(const inner_view& value) const { return value; }
};

constexpr bool join_works() {
    int a[] = {1, 2};
    int b[] = {3};
    inner_view inners[] = {{}, {a, a + 2}, {b, b + 1}, {}};
    outer_view outer(inners, inners + 4);
    auto joined = outer | tested::ranges::views::join;
    const auto& const_joined = joined;
    auto const_current = const_joined.begin();
    decltype(const_current) converted = joined.begin();
    if (*const_current != 1 || *converted != 1) return false;
    auto current = joined.begin();
    if (*current != 1) return false;
    ++current;
    if (*current != 2) return false;
    ++current;
    if (*current != 3) return false;
    ++current;
    if (current != joined.end()) return false;
    --current;
    if (*current != 3) return false;
    auto prvalue = outer | tested::ranges::views::transform(copy_inner{}) |
        tested::ranges::views::join;
    auto input = prvalue.begin();
    if (*input != 1) return false;
    ++input;
    ++input;
    return *input == 3;
}

using join_type = tested::ranges::join_view<outer_view>;
static_assert(tested::ranges::bidirectional_range<join_type>);
static_assert(tested::ranges::common_range<join_type>);
using prvalue_join = decltype(
    tested::declval<outer_view&>() |
    tested::ranges::views::transform(copy_inner{}) |
    tested::ranges::views::join);
static_assert(tested::ranges::input_range<prvalue_join>);
static_assert(!tested::ranges::forward_range<prvalue_join>);
static_assert(join_works());

bool ftl_test() { return join_works(); }
