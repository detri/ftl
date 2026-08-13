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

struct legacy_ordered {
    int number;
    friend constexpr bool operator==(legacy_ordered, legacy_ordered) = default;
    friend constexpr bool operator!=(legacy_ordered a, legacy_ordered b) {
        return a.number != b.number;
    }
    friend constexpr bool operator<(legacy_ordered a, legacy_ordered b) {
        return a.number < b.number;
    }
    friend constexpr bool operator>(legacy_ordered a, legacy_ordered b) {
        return a.number > b.number;
    }
    friend constexpr bool operator<=(legacy_ordered a, legacy_ordered b) {
        return a.number <= b.number;
    }
    friend constexpr bool operator>=(legacy_ordered a, legacy_ordered b) {
        return a.number >= b.number;
    }
};

struct immovable {
    int number;
    constexpr explicit immovable(int input) : number(input) {}
    immovable(const immovable&) = delete;
    immovable(immovable&&) = delete;
};

struct not_callable {};

template<class O>
concept has_const_or_else = requires(const O& value) {
    value.or_else(not_callable{});
};

template<class O>
concept has_rvalue_or_else = requires(O value) {
    tested::move(value).or_else(not_callable{});
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

constexpr bool legacy_comparisons_work() {
    tested::optional<legacy_ordered> one{legacy_ordered{1}};
    tested::optional<legacy_ordered> two{legacy_ordered{2}};
    legacy_ordered value_two{2};
    return one != two && one < two && two > one && one <= two && two >= one &&
           one != value_two && value_two != one && one < value_two &&
           value_two > one && one <= value_two && value_two >= one;
}

constexpr bool immovable_transform_works() {
    tested::optional<int> source{7};
    auto result = source.transform([](int input) { return immovable{input}; });
    return result->number == 7;
}

static_assert(reference_optional_works());
static_assert(monadic_optional_works());
static_assert(legacy_comparisons_work());
static_assert(immovable_transform_works());
static_assert(!has_const_or_else<tested::optional<int>>);
static_assert(!has_rvalue_or_else<tested::optional<int>>);
static_assert(tested::is_trivially_copyable_v<tested::optional<int&>>);
static_assert(tested::is_trivially_copyable_v<tested::optional<int>>);
static_assert(tested::optional<int>{1} < tested::optional<int>{2});
static_assert(tested::is_assignable_v<tested::optional<long>&,
                                     const tested::optional<int>&>);
static_assert(tested::is_constructible_v<tested::optional<bool>,
                                        const tested::optional<int>&>);
#if __cpp_lib_optional != 202110L
#error optional must advertise its C++23 monadic surface
#endif

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
