#ifdef FTL_REPLACE_STL
#include <ranges>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/ranges>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct sentinel {
    int* end{};
    friend constexpr bool operator==(int* current, sentinel bound) { return current == bound.end; }
    friend constexpr tested::ptrdiff_t operator-(sentinel bound, int* current) { return bound.end - current; }
    friend constexpr tested::ptrdiff_t operator-(int* current, sentinel bound) { return current - bound.end; }
};

struct non_common_view : tested::ranges::view_base {
    int* first{};
    int* last{};
    constexpr non_common_view() = default;
    constexpr non_common_view(int* begin, int* end) : first(begin), last(end) {}
    constexpr int* begin() const { return first; }
    constexpr sentinel end() const { return {last}; }
    constexpr tested::size_t size() const { return static_cast<tested::size_t>(last - first); }
};

struct forward_iterator {
    using value_type = int;
    using difference_type = tested::ptrdiff_t;
    using iterator_concept = tested::forward_iterator_tag;
    using iterator_category = tested::forward_iterator_tag;
    int* current{};
    constexpr int& operator*() const { return *current; }
    constexpr forward_iterator& operator++() { ++current; return *this; }
    constexpr forward_iterator operator++(int) { auto copy = *this; ++*this; return copy; }
    friend constexpr bool operator==(forward_iterator, forward_iterator) = default;
};

struct forward_sentinel {
    int* end{};
    friend constexpr bool operator==(forward_iterator current, forward_sentinel bound)
    { return current.current == bound.end; }
};

struct forward_view : tested::ranges::view_base {
    int* first{};
    int* last{};
    constexpr forward_view() = default;
    constexpr forward_view(int* begin, int* end) : first(begin), last(end) {}
    constexpr forward_iterator begin() const { return {first}; }
    constexpr forward_sentinel end() const { return {last}; }
};

constexpr bool common_works() {
    int values[] = {1, 2, 3};
    non_common_view base(values, values + 3);
    auto direct = tested::ranges::common_view(base);
    auto piped = base | tested::ranges::views::common;
    auto iterator = direct.begin();
    if (*iterator != 1 || direct.end() - iterator != 3 || direct.size() != 3) return false;
    ++iterator;
    if (*iterator != 2) return false;
    auto already_common = values | tested::ranges::views::common;
    auto general = forward_view{values, values + 3} | tested::ranges::views::common;
    auto current = general.begin();
    ++current;
    return *piped.begin() == 1 && *current == 2 && current != general.end() &&
        tested::same_as<decltype(already_common), tested::ranges::ref_view<int[3]>>;
}

using common_type = tested::ranges::common_view<non_common_view>;
static_assert(tested::ranges::common_range<common_type>);
static_assert(tested::ranges::sized_range<common_type>);
static_assert(common_works());

bool ftl_test() { return common_works(); }
