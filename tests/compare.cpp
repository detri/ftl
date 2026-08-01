#ifdef FTL_REPLACE_STL
#include <compare>
namespace tested = std;
#else
#include <ftl/compare>
namespace tested = ftl;
#endif

#if __cpp_lib_three_way_comparison != 201907L
#error invalid <compare> feature-test macro
#endif

static_assert(tested::compare_three_way{}(2, 1) > 0);
static_assert(tested::strong_ordering::equal == 0);
static_assert(tested::is_eq(tested::strong_ordering::equal));
static_assert(tested::is_neq(tested::partial_ordering::unordered));
static_assert(tested::is_lt(tested::weak_ordering::less));
static_assert(tested::is_same_v<
              tested::common_comparison_category_t<
                  tested::strong_ordering, tested::weak_ordering>,
              tested::weak_ordering>);

struct comparable {
    int value;
    friend constexpr tested::strong_ordering
    operator<=>(comparable left, comparable right) noexcept {
        if (left.value < right.value) return tested::strong_ordering::less;
        if (left.value > right.value) return tested::strong_ordering::greater;
        return tested::strong_ordering::equal;
    }
    friend constexpr bool operator==(comparable, comparable) = default;
};

static_assert(tested::three_way_comparable<comparable,
                                           tested::strong_ordering>);
static_assert(tested::is_same_v<tested::compare_three_way_result_t<comparable>,
                                tested::strong_ordering>);
static_assert(tested::strong_order(comparable{1}, comparable{2}) < 0);
static_assert(tested::strong_order(1, 2) < 0);
static_assert(tested::strong_order(-0.0, 0.0) < 0);
static_assert(tested::strong_order(-0.0L, 0.0L) < 0);
constexpr float quiet_nan_one = __builtin_bit_cast(float, 0x7fc00001u);
constexpr float quiet_nan_two = __builtin_bit_cast(float, 0x7fc00002u);
static_assert(tested::strong_order(quiet_nan_one, quiet_nan_two) < 0);
static_assert(noexcept(tested::strong_order(1, 2)));
static_assert(noexcept(tested::weak_order(1, 2)));
static_assert(noexcept(tested::partial_order(1, 2)));

struct legacy_ordered {
    int value;
    friend constexpr bool operator==(legacy_ordered, legacy_ordered) = default;
    friend constexpr bool operator<(legacy_ordered left,
                                    legacy_ordered right) {
        return left.value < right.value;
    }
    friend constexpr bool operator>(legacy_ordered left,
                                    legacy_ordered right) { return right < left; }
    friend constexpr bool operator<=(legacy_ordered left,
                                     legacy_ordered right) { return !(right < left); }
    friend constexpr bool operator>=(legacy_ordered left,
                                     legacy_ordered right) { return !(left < right); }
};
static_assert(tested::compare_strong_order_fallback(
                  legacy_ordered{1}, legacy_ordered{2}) < 0);
static_assert(noexcept(tested::compare_strong_order_fallback(1, 2)));

struct partial_custom {
    int value;
    friend constexpr tested::partial_ordering
    partial_order(partial_custom left, partial_custom right) noexcept {
        if (left.value < right.value) return tested::partial_ordering::less;
        if (left.value > right.value) return tested::partial_ordering::greater;
        return tested::partial_ordering::equivalent;
    }
};
static_assert(tested::partial_order(partial_custom{2}, partial_custom{1}) > 0);

bool ftl_test() { return true; }
