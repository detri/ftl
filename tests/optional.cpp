#ifdef FTL_REPLACE_STL
#include <optional>
namespace tested = std;
#else
#include <ftl/optional>
namespace tested = ftl;
#endif

struct value {
    int number;
    constexpr explicit value(int input) : number{input} {}
    friend constexpr bool operator==(const value&, const value&) = default;
};

constexpr bool value_optional_works() {
    tested::optional<value> item;
    if (item || item.value_or(value{3}).number != 3)
        return false;
    item.emplace(7);
    tested::optional<value> copy = item;
    copy = value{9};
    item.swap(copy);
    return item->number == 9 && copy->number == 7;
}

constexpr bool reference_optional_works() {
    int first = 1;
    int second = 2;
    tested::optional<int&> reference = first;
    reference = second;
    *reference = 3;
    reference.reset();
    return first == 1 && second == 3 && !reference;
}

constexpr bool monadic_optional_works() {
    tested::optional<int> value{3};
    auto doubled = value.transform([](int input) { return input * 2; });
    auto chained = value.and_then([](int input) {
        return tested::optional<int>{input + 1};
    });
    auto fallback = tested::optional<int>{}.or_else([] {
        return tested::optional<int>{5};
    });
    return doubled == 6 && chained == 4 && fallback == 5;
}

static_assert(reference_optional_works());
static_assert(monadic_optional_works());
static_assert(tested::is_trivially_copyable_v<tested::optional<int&>>);
static_assert(tested::is_trivially_copyable_v<tested::optional<int>>);
static_assert(tested::optional<int>{1} < tested::optional<int>{2});

bool ftl_test() {
    if (!value_optional_works())
        return false;
    if (tested::hash<tested::optional<int>>{}(tested::optional<int>{4}) !=
        tested::hash<int>{}(4))
        return false;
    try {
        tested::optional<int>{}.value();
    } catch (const tested::bad_optional_access&) {
        return true;
    }
    return false;
}
