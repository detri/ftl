#ifdef FTL_REPLACE_STL
#include <ranges>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/ranges>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct pointer_view : tested::ranges::view_base {
    int* first{};
    int* last{};
    constexpr pointer_view() = default;
    constexpr pointer_view(int* begin, int* end) : first(begin), last(end) {}
    constexpr int* begin() const { return first; }
    constexpr int* end() const { return last; }
    constexpr tested::size_t size() const { return static_cast<tested::size_t>(last - first); }
};

struct doubled {
    constexpr int operator()(int value) const { return value * 2; }
};

struct identity {
    constexpr int& operator()(int& value) const { return value; }
};

struct move_only {
    int offset{};
    constexpr explicit move_only(int value) : offset(value) {}
    move_only(const move_only&) = delete;
    constexpr move_only(move_only&&) = default;
    constexpr move_only& operator=(move_only&&) = default;
    constexpr int operator()(int value) { return value + offset; }
};

constexpr bool transform_works() {
    int values[] = {1, 2, 3};
    pointer_view base{values, values + 3};
    auto direct = tested::ranges::transform_view(base, doubled{});
    auto piped = base | tested::ranges::views::transform(doubled{});
    auto iterator = direct.begin();
    if (*iterator != 2 || iterator[2] != 6 || direct.size() != 3) return false;
    ++iterator;
    if (*iterator != 4 || direct.end() - iterator != 2) return false;
    auto reference = base | tested::ranges::views::transform(identity{});
    *reference.begin() = 9;
    auto moved = base | tested::ranges::views::transform(move_only{5});
    return values[0] == 9 && *piped.begin() == 18 && *moved.begin() == 14;
}

using transformed = tested::ranges::transform_view<pointer_view, doubled>;
static_assert(tested::ranges::view<transformed>);
static_assert(tested::ranges::random_access_range<transformed>);
static_assert(tested::ranges::sized_range<transformed>);
static_assert(tested::same_as<tested::ranges::range_value_t<transformed>, int>);
static_assert(transform_works());

bool ftl_test() { return transform_works(); }
