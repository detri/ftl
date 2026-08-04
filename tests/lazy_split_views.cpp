#ifdef FTL_REPLACE_STL
#include <ranges>
namespace tested = std;
#else
#include <ftl/ranges>
namespace tested = ftl;
#endif

struct input_iterator {
    using value_type = int;
    using difference_type = tested::ptrdiff_t;
    using iterator_concept = tested::input_iterator_tag;
    int* current{};
    constexpr int& operator*() const { return *current; }
    constexpr input_iterator& operator++() { ++current; return *this; }
    constexpr void operator++(int) { ++current; }
    friend constexpr bool operator==(input_iterator, input_iterator) = default;
};

struct input_sentinel {
    int* end{};
    friend constexpr bool operator==(input_iterator current, input_sentinel bound)
    { return current.current == bound.end; }
};

struct input_view : tested::ranges::view_base {
    int* first{};
    int* last{};
    constexpr input_view() = default;
    constexpr input_view(int* begin, int* end) : first(begin), last(end) {}
    constexpr input_iterator begin() { return {first}; }
    constexpr input_sentinel end() { return {last}; }
};

constexpr bool equal(auto range, const int* expected, tested::size_t size) {
    auto current = range.begin();
    for (tested::size_t index = 0; index < size; ++index, ++current)
        if (current == range.end() || *current != expected[index]) return false;
    return current == range.end();
}

constexpr bool lazy_split_works() {
    int values[] = {1, 0, 2, 0};
    auto input = input_view(values, values + 4) | tested::ranges::views::lazy_split(0);
    auto part = input.begin();
    const int one[] = {1};
    const int two[] = {2};
    const int* none = nullptr;
    if (!equal(*part, one, 1)) return false;
    ++part;
    if (!equal(*part, two, 1)) return false;
    ++part;
    if (!equal(*part, none, 0)) return false;
    ++part;
    if (part != input.end()) return false;

    int forward_values[] = {1, 0, 0, 2};
    int pattern[] = {0, 0};
    auto forward = tested::ranges::views::lazy_split(forward_values, pattern);
    auto forward_part = forward.begin();
    if (!equal(*forward_part++, one, 1) || !equal(*forward_part++, two, 1) ||
        forward_part != forward.end()) return false;

    int empty_values[] = {4, 5};
    auto empty = tested::ranges::views::lazy_split(
        input_view(empty_values, empty_values + 2), tested::ranges::empty_view<int>{});
    auto empty_part = empty.begin();
    const int four[] = {4};
    const int five[] = {5};
    return equal(*empty_part, four, 1) && (++empty_part, equal(*empty_part, five, 1)) &&
        (++empty_part, empty_part == empty.end());
}

static_assert(lazy_split_works());
bool ftl_test() { return lazy_split_works(); }
