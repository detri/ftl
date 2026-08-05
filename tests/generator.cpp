#ifdef FTL_REPLACE_STL
#include <generator>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/generator>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

using value_generator =
    tested::generator<int>;

using reference_generator =
    tested::generator<int&>;

using const_reference_generator =
    tested::generator<const int&>;

using explicit_value_generator =
    tested::generator<int, int>;

static_assert(tested::is_same_v<
    typename value_generator::yielded,
    int&&
>);

static_assert(tested::is_same_v<
    typename reference_generator::yielded,
    int&
>);

static_assert(tested::is_same_v<
    typename const_reference_generator::yielded,
    const int&
>);

static_assert(tested::is_same_v<
    typename tested::generator<int&&>::yielded,
    int&&
>);

static_assert(tested::is_same_v<
    typename explicit_value_generator::yielded,
    const int&
>);

static_assert(
    !tested::is_default_constructible_v<
        value_generator
    >
);

static_assert(
    !tested::is_copy_constructible_v<
        value_generator
    >
);

static_assert(
    !tested::is_copy_assignable_v<
        value_generator
    >
);

static_assert(
    tested::is_nothrow_move_constructible_v<
        value_generator
    >
);

static_assert(
    tested::is_nothrow_move_assignable_v<
        value_generator
    >
);

static_assert(
    tested::ranges::view<
        value_generator
    >
);

static_assert(
    tested::ranges::input_range<
        value_generator
    >
);

static_assert(
    !tested::ranges::forward_range<
        value_generator
    >
);

static_assert(
    !tested::ranges::borrowed_range<
        value_generator
    >
);

static_assert(tested::is_same_v<
    tested::ranges::range_value_t<
        value_generator
    >,
    int
>);

static_assert(tested::is_same_v<
    tested::ranges::range_reference_t<
        value_generator
    >,
    int&&
>);

static_assert(tested::is_same_v<
    tested::ranges::range_reference_t<
        reference_generator
    >,
    int&
>);

using value_iterator =
    tested::ranges::iterator_t<
        value_generator
    >;

static_assert(
    tested::input_iterator<
        value_iterator
    >
);

static_assert(
    tested::sentinel_for<
        tested::default_sentinel_t,
        value_iterator
    >
);

static_assert(
    !tested::is_copy_constructible_v<
        value_iterator
    >
);

static_assert(
    tested::is_nothrow_move_constructible_v<
        value_iterator
    >
);

static_assert(tested::is_same_v<
    decltype(
        tested::declval<value_iterator&>()++
    ),
    void
>);

#ifdef FTL_REPLACE_STL

tested::generator<int> values() {
    /*
     * These are lvalues, so this tests the materializing
     * yield_value overload.
     */
    for (int value = 1; value <= 3; ++value) {
        co_yield value;
    }
}

tested::generator<int> direct_values() {
    /*
     * These test direct rvalue yielding.
     */
    co_yield 4;
    co_yield 5;
}

tested::generator<int> empty_values() {
    co_return;
}

tested::generator<int&> references(
    int& first,
    int& second
) {
    co_yield first;
    co_yield second;
}

tested::generator<const int&> const_references(
    const int& first,
    const int& second
) {
    co_yield first;
    co_yield second;
}

#if FTL_HAS_EXCEPTIONS

struct generator_test_exception {};

tested::generator<int> throwing_values() {
    co_yield 9;
    throw generator_test_exception{};
}

#endif

#endif

bool ftl_test() {
#ifdef FTL_REPLACE_STL
    {
        int expected = 1;

        for (int value : values()) {
            if (value != expected) {
                return false;
            }

            ++expected;
        }

        if (expected != 4) {
            return false;
        }
    }

    {
        auto sequence = empty_values();
        auto iterator = sequence.begin();

        if (iterator != sequence.end()) {
            return false;
        }
    }

    {
        int first = 10;
        int second = 20;

        for (int& value : references(first, second)) {
            value += 5;
        }

        if (first != 15 || second != 25) {
            return false;
        }
    }

    {
        const int first = 7;
        const int second = 11;
        int total = 0;

        for (
            const int& value :
            const_references(first, second)
        ) {
            total += value;
        }

        if (total != 18) {
            return false;
        }
    }

    {
        auto source = direct_values();
        auto iterator = source.begin();

        if (*iterator != 4) {
            return false;
        }

        /*
         * Existing iterators remain attached to the coroutine
         * after generator ownership moves.
         */
        auto destination =
            tested::move(source);

        ++iterator;

        if (
            iterator == destination.end() ||
            *iterator != 5
        ) {
            return false;
        }

        ++iterator;

        if (iterator != destination.end()) {
            return false;
        }
    }

#if FTL_HAS_EXCEPTIONS
    {
        bool caught = false;

        try {
            auto sequence = throwing_values();
            auto iterator = sequence.begin();

            if (*iterator != 9) {
                return false;
            }

            ++iterator;
        } catch (const generator_test_exception&) {
            caught = true;
        }

        if (!caught) {
            return false;
        }
    }
#endif
#endif

    return true;
}
