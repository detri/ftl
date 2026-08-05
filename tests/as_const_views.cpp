#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/ranges>
#include <ftl/span>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct test_range {
    int* first = nullptr;
    int* last = nullptr;

    constexpr int* begin() noexcept {
        return first;
    }

    constexpr const int* begin() const noexcept {
        return first;
    }

    constexpr int* end() noexcept {
        return last;
    }

    constexpr const int* end() const noexcept {
        return last;
    }

    constexpr tested::size_t size() const noexcept {
        return static_cast<tested::size_t>(
            last - first
        );
    }
};

static_assert(
    tested::ranges::range<test_range>
);

static_assert(
    tested::ranges::constant_range<
        const test_range
    >
);

static_assert(
    !tested::ranges::constant_range<
        test_range
    >
);

/*
 * Already-constant ranges pass through views::all.
 */
using already_const =
    decltype(
        tested::ranges::views::all(
            tested::declval<
                const int (&)[3]
            >()
        )
    );

static_assert(tested::same_as<
    decltype(
        tested::ranges::views::as_const(
            tested::declval<
                already_const
            >()
        )
    ),
    already_const
>);

/*
 * empty_view normalization.
 */
using mutable_empty =
    tested::ranges::empty_view<int>;

using const_empty_result =
    decltype(
        tested::ranges::views::as_const(
            tested::declval<
                mutable_empty
            >()
        )
    );

static_assert(tested::same_as<
    const_empty_result,
    tested::ranges::empty_view<
        const int
    >
>);

/*
 * Fixed-extent span normalization.
 */
using fixed_span =
    tested::span<int, 3>;

using fixed_span_result =
    decltype(
        tested::ranges::views::as_const(
            tested::declval<
                fixed_span
            >()
        )
    );

static_assert(tested::same_as<
    fixed_span_result,
    tested::span<const int, 3>
>);

/*
 * Dynamic-extent span normalization.
 */
using dynamic_span =
    tested::span<int>;

using dynamic_span_result =
    decltype(
        tested::ranges::views::as_const(
            tested::declval<
                dynamic_span
            >()
        )
    );

static_assert(tested::same_as<
    dynamic_span_result,
    tested::span<const int>
>);

/*
 * ref_view normalization.
 */
using mutable_ref_view =
    tested::ranges::ref_view<
        test_range
    >;

using const_ref_view_result =
    decltype(
        tested::ranges::views::as_const(
            tested::declval<
                mutable_ref_view
            >()
        )
    );

static_assert(tested::same_as<
    const_ref_view_result,
    tested::ranges::ref_view<
        const test_range
    >
>);

/*
 * A non-view lvalue whose const form is a constant range
 * also normalizes directly to ref_view<const T>.
 */
using lvalue_result =
    decltype(
        tested::ranges::views::as_const(
            tested::declval<
                test_range&
            >()
        )
    );

static_assert(tested::same_as<
    lvalue_result,
    tested::ranges::ref_view<
        const test_range
    >
>);

/*
 * Other mutable views use as_const_view.
 */
using mutable_single =
    tested::ranges::single_view<int>;

using single_result =
    decltype(
        tested::ranges::views::as_const(
            tested::declval<
                mutable_single
            >()
        )
    );

static_assert(tested::same_as<
    single_result,
    tested::ranges::as_const_view<
        mutable_single
    >
>);

static_assert(
    tested::ranges::constant_range<
        single_result
    >
);

template<class T>
concept can_as_const =
    requires(T&& value) {
        tested::ranges::views::as_const(
            static_cast<T&&>(value)
        );
    };

static_assert(
    can_as_const<test_range&>
);

static_assert(
    can_as_const<mutable_single>
);

static_assert(
    !can_as_const<int>
);

constexpr bool as_const_works() {
    int values[] = {1, 2, 3};

    test_range source{
        values,
        values + 3
    };

    auto lvalue =
        tested::ranges::views::as_const(
            source
        );

    if (lvalue.size() != 3)
        return false;

    if (*lvalue.begin() != 1)
        return false;

    static_assert(tested::same_as<
        tested::ranges::range_reference_t<
            decltype(lvalue)
        >,
        const int&
    >);

    fixed_span span_source(values);

    auto fixed =
        tested::ranges::views::as_const(
            span_source
        );

    if (fixed.data() != values)
        return false;

    if (fixed.size() != 3)
        return false;

    auto fallback =
        tested::ranges::views::as_const(
            tested::ranges::single_view<int>(
                42
            )
        );

    if (*fallback.begin() != 42)
        return false;

    return true;
}

static_assert(as_const_works());

bool ftl_test() {
    return as_const_works();
}