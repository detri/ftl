#ifdef FTL_REPLACE_STL
#include <ranges>
namespace tested = std;
#else
#include <ftl/ranges>
namespace tested = ftl;
#endif

struct pointer_sentinel {
    int* end{};
    friend constexpr bool operator==(int* current, pointer_sentinel bound) { return current == bound.end; }
};

struct non_common_view : tested::ranges::view_base {
    int* first{};
    int* last{};
    constexpr non_common_view() = default;
    constexpr non_common_view(int* begin, int* end) : first(begin), last(end) {}
    constexpr int* begin() const { return first; }
    constexpr pointer_sentinel end() const { return {last}; }
};

constexpr bool equal(auto range, const int* expected, tested::size_t size) {
    auto current = range.begin();
    for (tested::size_t index = 0; index < size; ++index, ++current)
        if (current == range.end() || *current != expected[index]) return false;
    return current == range.end();
}

constexpr int split_result() {
    int values[] = {1, 0, 2, 0, 0, 3, 0};
    auto split = values | tested::ranges::views::split(0);
    auto part = split.begin();
    const int one[] = {1};
    const int two[] = {2};
    const int* none = nullptr;
    const int three[] = {3};
    if (!equal(*part++, one, 1) || !equal(*part++, two, 1) ||
        !equal(*part++, none, 0) || !equal(*part++, three, 1) ||
        !equal(*part++, none, 0) || part != split.end()) return 1;

    int pattern[] = {0, 0};
    auto multi = tested::ranges::views::split(values, pattern);
    auto multi_part = multi.begin();
    const int prefix[] = {1, 0, 2};
    const int suffix[] = {3, 0};
    if (!equal(*multi_part++, prefix, 3) || !equal(*multi_part++, suffix, 2) || multi_part != multi.end()) return 2;

    int short_values[] = {4, 5};
    auto singles = tested::ranges::views::split(
        short_values, tested::ranges::empty_view<int>{});
    auto single = singles.begin();
    const int four[] = {4};
    const int five[] = {5};
    if (!equal(*single++, four, 1) || !equal(*single++, five, 1) || single != singles.end()) return 3;

    int non_common_values[] = {1, 0, 2};
    auto non_common = tested::ranges::views::split(
        non_common_view{non_common_values, non_common_values + 3}, 0);
    auto non_common_part = non_common.begin();
    return equal(*non_common_part++, one, 1) &&
        equal(*non_common_part++, two, 1) && non_common_part == non_common.end() ? 0 : 4;
}

static_assert(split_result() == 0);
bool ftl_test() { return split_result() == 0; }
