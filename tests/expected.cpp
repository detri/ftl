#ifdef FTL_REPLACE_STL
#include <expected>
namespace tested = std;
#else
#include <ftl/expected>
namespace tested = ftl;
#endif

struct immovable {
    int number;
    constexpr explicit immovable(int input) : number(input) {}
    immovable(const immovable&) = delete;
    immovable(immovable&&) = delete;
};

struct throwing_move {
    throwing_move() = default;
    throwing_move(const throwing_move&) = default;
    throwing_move(throwing_move&&) noexcept(false) {}
    throwing_move& operator=(throwing_move&&) = default;
    friend void swap(throwing_move&, throwing_move&) noexcept {}
};

constexpr bool expected_works() {
    tested::expected<int, int> value = 3;
    auto doubled = value.transform([](int x) { return x * 2; });
    auto chained = value.and_then([](int x) { return tested::expected<int, int>(x + 1); });
    tested::expected<int, int> error = tested::unexpected(5);
    auto mapped = error.transform_error([](int x) { return x + 2; });
    tested::expected<void, int> nothing;
    auto void_mapped = nothing.transform([] { return 8; });
    auto void_chained = nothing.and_then([] {
        return tested::expected<int, int>(9);
    });
    return value && *value == 3 && doubled == 6 && chained == 4 &&
           !error && error.error() == 5 && mapped.error() == 7 && nothing &&
           void_mapped == 8 && void_chained == 9;
}

constexpr bool immovable_monadic_results_work() {
    tested::expected<int, int> value(4);
    auto mapped = value.transform([](int x) { return immovable{x}; });
    tested::expected<int, int> error(tested::unexpect, 5);
    auto mapped_error = error.transform_error(
        [](int x) { return immovable{x}; });
    return mapped->number == 4 && mapped_error.error().number == 5;
}

static_assert(expected_works());
static_assert(immovable_monadic_results_work());
static_assert(noexcept(tested::declval<tested::expected<int, int>&>().emplace(1)));
static_assert(!tested::is_swappable_v<
              tested::expected<throwing_move, throwing_move>>);
static_assert(tested::expected<int, int>{3} == 3);
static_assert(tested::expected<int, int>{tested::unexpected(2)} == tested::unexpected(2));
static_assert(tested::is_trivially_copyable_v<tested::expected<int, int>>);
static_assert(tested::is_trivially_copyable_v<tested::expected<void, int>>);
#if __cpp_lib_expected != 202211L
#error expected must advertise its C++23 corrected monadic surface
#endif

bool ftl_test() {
    if (!expected_works()) return false;
    try {
        (void)tested::expected<int, int>(tested::unexpected(9)).value();
    } catch (const tested::bad_expected_access<int>& error) {
        return error.error() == 9;
    }
    return false;
}
