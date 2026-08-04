#ifdef FTL_REPLACE_STL
#include <concepts>
#include <cstddef>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/concepts>
#include <ftl/cstddef>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

/*
 * Invocation probes.
 */

template<class Closure, class Range>
concept can_call_closure =
    requires(
        Closure&& closure,
        Range&& range
    ) {
        static_cast<Closure&&>(
            closure
        )(
            static_cast<Range&&>(
                range
            )
        );
    };

template<class Range, class Closure>
concept can_pipe =
    requires(
        Range&& range,
        Closure&& closure
    ) {
        static_cast<Range&&>(
            range
        ) |
        static_cast<Closure&&>(
            closure
        );
    };

template<class Callable>
concept can_direct_transform =
    requires(
        int (&range)[4],
        Callable&& callable
    ) {
        tested::ranges::views::
            transform(
                range,
                static_cast<Callable&&>(
                    callable
                )
            );
    };

template<class Callable>
concept can_direct_zip_transform =
    requires(
        int (&left)[4],
        int (&right)[4],
        Callable&& callable
    ) {
        tested::ranges::views::
            zip_transform(
                static_cast<Callable&&>(
                    callable
                ),
                left,
                right
            );
    };

template<class Callable>
concept can_empty_zip_transform =
    requires(Callable&& callable) {
        tested::ranges::views::
            zip_transform(
                static_cast<Callable&&>(
                    callable
                )
            );
    };

template<class Callable>
concept can_direct_adjacent_transform =
    requires(
        int (&range)[4],
        Callable&& callable
    ) {
        tested::ranges::views::
            adjacent_transform<2>(
                range,
                static_cast<Callable&&>(
                    callable
                )
            );
    };

template<class Callable>
concept can_direct_take_while =
    requires(
        int (&range)[4],
        Callable&& callable
    ) {
        tested::ranges::views::
            take_while(
                range,
                static_cast<Callable&&>(
                    callable
                )
            );
    };

template<class Callable>
concept can_direct_drop_while =
    requires(
        int (&range)[4],
        Callable&& callable
    ) {
        tested::ranges::views::
            drop_while(
                range,
                static_cast<Callable&&>(
                    callable
                )
            );
    };

template<class Callable>
concept can_direct_filter =
    requires(
        int (&range)[4],
        Callable&& callable
    ) {
        tested::ranges::views::
            filter(
                range,
                static_cast<Callable&&>(
                    callable
                )
            );
    };

template<class Callable>
concept can_direct_chunk_by =
    requires(
        int (&range)[4],
        Callable&& callable
    ) {
        tested::ranges::views::
            chunk_by(
                range,
                static_cast<Callable&&>(
                    callable
                )
            );
    };

/*
 * Truly move-only transformation callables.
 */

struct move_only_unary_transform {
    int multiplier = 1;

    constexpr explicit
    move_only_unary_transform(
        int multiplier
    )
        : multiplier(multiplier) {}

    move_only_unary_transform(
        const move_only_unary_transform&
    ) = delete;

    constexpr move_only_unary_transform(
        move_only_unary_transform&&
    ) noexcept = default;

    move_only_unary_transform&
    operator=(
        const move_only_unary_transform&
    ) = delete;

    constexpr move_only_unary_transform&
    operator=(
        move_only_unary_transform&&
    ) noexcept = default;

    constexpr int operator()(
        int value
    ) const noexcept {
        return value * multiplier;
    }
};

struct move_only_binary_transform {
    int bias = 0;

    constexpr explicit
    move_only_binary_transform(
        int bias
    )
        : bias(bias) {}

    move_only_binary_transform(
        const move_only_binary_transform&
    ) = delete;

    constexpr move_only_binary_transform(
        move_only_binary_transform&&
    ) noexcept = default;

    move_only_binary_transform&
    operator=(
        const move_only_binary_transform&
    ) = delete;

    constexpr move_only_binary_transform&
    operator=(
        move_only_binary_transform&&
    ) noexcept = default;

    constexpr int operator()(
        int left,
        int right
    ) const noexcept {
        return left + right + bias;
    }
};

struct move_only_nullary_transform {
    int value = 0;

    constexpr explicit
    move_only_nullary_transform(
        int value
    )
        : value(value) {}

    move_only_nullary_transform(
        const move_only_nullary_transform&
    ) = delete;

    constexpr move_only_nullary_transform(
        move_only_nullary_transform&&
    ) noexcept = default;

    move_only_nullary_transform&
    operator=(
        const move_only_nullary_transform&
    ) = delete;

    constexpr move_only_nullary_transform&
    operator=(
        move_only_nullary_transform&&
    ) noexcept = default;

    constexpr int operator()() const noexcept {
        return value;
    }
};

/*
 * Predicate concepts still require copy construction, but
 * movable_box must support a predicate that is copy-constructible
 * without being assignable.
 */

struct reconstructible_unary_predicate {
    int bound = 0;

    constexpr explicit
    reconstructible_unary_predicate(
        int bound
    )
        : bound(bound) {}

    constexpr reconstructible_unary_predicate(
        const reconstructible_unary_predicate&
    ) noexcept = default;

    constexpr reconstructible_unary_predicate(
        reconstructible_unary_predicate&&
    ) noexcept = default;

    reconstructible_unary_predicate&
    operator=(
        const reconstructible_unary_predicate&
    ) = delete;

    reconstructible_unary_predicate&
    operator=(
        reconstructible_unary_predicate&&
    ) = delete;

    constexpr bool operator()(
        int value
    ) const noexcept {
        return value < bound;
    }
};

struct reconstructible_binary_predicate {
    int maximum_gap = 0;

    constexpr explicit
    reconstructible_binary_predicate(
        int maximum_gap
    )
        : maximum_gap(maximum_gap) {}

    constexpr reconstructible_binary_predicate(
        const reconstructible_binary_predicate&
    ) noexcept = default;

    constexpr reconstructible_binary_predicate(
        reconstructible_binary_predicate&&
    ) noexcept = default;

    reconstructible_binary_predicate&
    operator=(
        const reconstructible_binary_predicate&
    ) = delete;

    reconstructible_binary_predicate&
    operator=(
        reconstructible_binary_predicate&&
    ) = delete;

    constexpr bool operator()(
        int left,
        int right
    ) const noexcept {
        const int difference =
            right >= left
                ? right - left
                : left - right;

        return difference <= maximum_gap;
    }
};

/*
 * A move-only predicate must still be rejected because the
 * indirect predicate concepts require copy construction.
 */

struct move_only_unary_predicate {
    constexpr move_only_unary_predicate()
        = default;

    move_only_unary_predicate(
        const move_only_unary_predicate&
    ) = delete;

    constexpr move_only_unary_predicate(
        move_only_unary_predicate&&
    ) noexcept = default;

    move_only_unary_predicate&
    operator=(
        const move_only_unary_predicate&
    ) = delete;

    constexpr move_only_unary_predicate&
    operator=(
        move_only_unary_predicate&&
    ) noexcept = default;

    constexpr bool operator()(
        int value
    ) const noexcept {
        return value != 0;
    }
};

struct move_only_binary_predicate {
    constexpr move_only_binary_predicate()
        = default;

    move_only_binary_predicate(
        const move_only_binary_predicate&
    ) = delete;

    constexpr move_only_binary_predicate(
        move_only_binary_predicate&&
    ) noexcept = default;

    move_only_binary_predicate&
    operator=(
        const move_only_binary_predicate&
    ) = delete;

    constexpr move_only_binary_predicate&
    operator=(
        move_only_binary_predicate&&
    ) noexcept = default;

    constexpr bool operator()(
        int left,
        int right
    ) const noexcept {
        return left == right;
    }
};

using int_range =
    int (&)[4];

/*
 * Callable type properties.
 */

static_assert(
    tested::move_constructible<
        move_only_unary_transform
    >
);

static_assert(
    !tested::copy_constructible<
        move_only_unary_transform
    >
);

static_assert(
    tested::move_constructible<
        move_only_binary_transform
    >
);

static_assert(
    !tested::copy_constructible<
        move_only_binary_transform
    >
);

static_assert(
    tested::move_constructible<
        move_only_nullary_transform
    >
);

static_assert(
    !tested::copy_constructible<
        move_only_nullary_transform
    >
);

static_assert(
    tested::copy_constructible<
        reconstructible_unary_predicate
    >
);

static_assert(
    !tested::copyable<
        reconstructible_unary_predicate
    >
);

static_assert(
    tested::copy_constructible<
        reconstructible_binary_predicate
    >
);

static_assert(
    !tested::copyable<
        reconstructible_binary_predicate
    >
);

/*
 * Direct transform invocation must consume move-only callables.
 */

static_assert(
    can_direct_transform<
        move_only_unary_transform
    >
);

static_assert(
    !can_direct_transform<
        move_only_unary_transform&
    >
);

static_assert(
    !can_direct_transform<
        const move_only_unary_transform&
    >
);

/*
 * zip_transform supports both nullary and ranged move-only
 * callables.
 */

static_assert(
    can_direct_zip_transform<
        move_only_binary_transform
    >
);

static_assert(
    !can_direct_zip_transform<
        move_only_binary_transform&
    >
);

static_assert(
    !can_direct_zip_transform<
        const move_only_binary_transform&
    >
);

static_assert(
    can_empty_zip_transform<
        move_only_nullary_transform
    >
);

static_assert(
    can_empty_zip_transform<
        move_only_nullary_transform&
    >
);

static_assert(
    can_empty_zip_transform<
        const move_only_nullary_transform&
    >
);

/*
 * adjacent_transform must also move the callable into the
 * resulting transform view.
 */

static_assert(
    can_direct_adjacent_transform<
        move_only_binary_transform
    >
);

static_assert(
    !can_direct_adjacent_transform<
        move_only_binary_transform&
    >
);

static_assert(
    !can_direct_adjacent_transform<
        const move_only_binary_transform&
    >
);

/*
 * Predicate views accept copy-constructible, nonassignable
 * predicates.
 */

static_assert(
    can_direct_take_while<
        reconstructible_unary_predicate
    >
);

static_assert(
    can_direct_drop_while<
        reconstructible_unary_predicate
    >
);

static_assert(
    can_direct_filter<
        reconstructible_unary_predicate
    >
);

static_assert(
    can_direct_chunk_by<
        reconstructible_binary_predicate
    >
);

/*
 * Predicate views must reject truly move-only predicates
 * cleanly rather than producing a hard error.
 */

static_assert(
    !can_direct_take_while<
        move_only_unary_predicate
    >
);

static_assert(
    !can_direct_drop_while<
        move_only_unary_predicate
    >
);

static_assert(
    !can_direct_filter<
        move_only_unary_predicate
    >
);

static_assert(
    !can_direct_chunk_by<
        move_only_binary_predicate
    >
);

/*
 * Partial transform closure value-category behavior.
 */

using transform_closure =
    decltype(
        tested::ranges::views::
            transform(
                move_only_unary_transform{
                    2
                }
            )
    );

static_assert(
    tested::move_constructible<
        transform_closure
    >
);

static_assert(
    !tested::copy_constructible<
        transform_closure
    >
);

static_assert(
    !can_call_closure<
        transform_closure&,
        int_range
    >
);

static_assert(
    !can_call_closure<
        const transform_closure&,
        int_range
    >
);

static_assert(
    can_call_closure<
        transform_closure,
        int_range
    >
);

static_assert(
    !can_call_closure<
        const transform_closure&&,
        int_range
    >
);

static_assert(
    !can_pipe<
        int_range,
        transform_closure&
    >
);

static_assert(
    !can_pipe<
        int_range,
        const transform_closure&
    >
);

static_assert(
    can_pipe<
        int_range,
        transform_closure
    >
);

static_assert(
    !can_pipe<
        int_range,
        const transform_closure&&
    >
);

/*
 * Partial adjacent_transform closure value-category behavior.
 */

using adjacent_transform_closure =
    decltype(
        tested::ranges::views::
            adjacent_transform<2>(
                move_only_binary_transform{
                    0
                }
            )
    );

static_assert(
    tested::move_constructible<
        adjacent_transform_closure
    >
);

static_assert(
    !tested::copy_constructible<
        adjacent_transform_closure
    >
);

static_assert(
    !can_call_closure<
        adjacent_transform_closure&,
        int_range
    >
);

static_assert(
    !can_call_closure<
        const adjacent_transform_closure&,
        int_range
    >
);

static_assert(
    can_call_closure<
        adjacent_transform_closure,
        int_range
    >
);

static_assert(
    !can_call_closure<
        const adjacent_transform_closure&&,
        int_range
    >
);

static_assert(
    !can_pipe<
        int_range,
        adjacent_transform_closure&
    >
);

static_assert(
    !can_pipe<
        int_range,
        const adjacent_transform_closure&
    >
);

static_assert(
    can_pipe<
        int_range,
        adjacent_transform_closure
    >
);

/*
 * Copy-constructible predicates allow every useful closure
 * value category because they can be copied from lvalue
 * closures and moved from rvalue closures.
 */

using filter_closure =
    decltype(
        tested::ranges::views::
            filter(
                reconstructible_unary_predicate{
                    3
                }
            )
    );

static_assert(
    can_call_closure<
        filter_closure&,
        int_range
    >
);

static_assert(
    can_call_closure<
        const filter_closure&,
        int_range
    >
);

static_assert(
    can_call_closure<
        filter_closure,
        int_range
    >
);

static_assert(
    can_pipe<
        int_range,
        filter_closure&
    >
);

static_assert(
    can_pipe<
        int_range,
        const filter_closure&
    >
);

static_assert(
    can_pipe<
        int_range,
        filter_closure
    >
);

/*
 * Resulting views preserve the callable's ownership model.
 */

using move_only_transform_view =
    decltype(
        tested::ranges::views::
            transform(
                tested::declval<
                    int_range
                >(),
                move_only_unary_transform{
                    2
                }
            )
    );

using move_only_zip_transform_view =
    decltype(
        tested::ranges::views::
            zip_transform(
                move_only_binary_transform{
                    0
                },
                tested::declval<
                    int_range
                >(),
                tested::declval<
                    int_range
                >()
            )
    );

using move_only_adjacent_transform_view =
    decltype(
        tested::ranges::views::
            adjacent_transform<2>(
                tested::declval<
                    int_range
                >(),
                move_only_binary_transform{
                    0
                }
            )
    );

using reconstructible_filter_view =
    decltype(
        tested::ranges::views::
            filter(
                tested::declval<
                    int_range
                >(),
                reconstructible_unary_predicate{
                    3
                }
            )
    );

static_assert(
    tested::ranges::view<
        move_only_transform_view
    >
);

static_assert(
    tested::move_constructible<
        move_only_transform_view
    >
);

static_assert(
    !tested::copy_constructible<
        move_only_transform_view
    >
);

static_assert(
    tested::ranges::view<
        move_only_zip_transform_view
    >
);

static_assert(
    tested::move_constructible<
        move_only_zip_transform_view
    >
);

static_assert(
    !tested::copy_constructible<
        move_only_zip_transform_view
    >
);

static_assert(
    tested::ranges::view<
        move_only_adjacent_transform_view
    >
);

static_assert(
    tested::move_constructible<
        move_only_adjacent_transform_view
    >
);

static_assert(
    !tested::copy_constructible<
        move_only_adjacent_transform_view
    >
);

/*
 * movable_box supplies assignment by reconstruction when the
 * predicate itself is copy-constructible but not assignable.
 */

static_assert(
    tested::copyable<
        reconstructible_filter_view
    >
);

/*
 * Runtime/constexpr behavior.
 */

constexpr bool equal_values(
    auto&& range,
    const int* expected,
    tested::size_t count
) {
    auto current =
        tested::ranges::begin(range);

    const auto bound =
        tested::ranges::end(range);

    for (
        tested::size_t index = 0;
        index < count;
        ++index,
        ++current
    ) {
        if (
            current == bound ||
            *current != expected[index]
        ) {
            return false;
        }
    }

    return current == bound;
}

constexpr bool
direct_move_only_transform_works() {
    int values[] = {
        1,
        2,
        3,
        4
    };

    auto view =
        tested::ranges::views::
            transform(
                values,
                move_only_unary_transform{
                    3
                }
            );

    const int expected[] = {
        3,
        6,
        9,
        12
    };

    return equal_values(
        view,
        expected,
        4
    );
}

constexpr bool
rvalue_transform_closure_works() {
    int values[] = {
        1,
        2,
        3,
        4
    };

    auto closure =
        tested::ranges::views::
            transform(
                move_only_unary_transform{
                    4
                }
            );

    auto view =
        values |
        tested::move(closure);

    const int expected[] = {
        4,
        8,
        12,
        16
    };

    return equal_values(
        view,
        expected,
        4
    );
}

constexpr bool
move_only_zip_transform_works() {
    int left[] = {
        1,
        2,
        3,
        4
    };

    int right[] = {
        10,
        20,
        30,
        40
    };

    auto view =
        tested::ranges::views::
            zip_transform(
                move_only_binary_transform{
                    5
                },
                left,
                right
            );

    const int expected[] = {
        16,
        27,
        38,
        49
    };

    return equal_values(
        view,
        expected,
        4
    );
}

constexpr bool
move_only_nullary_zip_transform_works() {
    auto view =
        tested::ranges::views::
            zip_transform(
                move_only_nullary_transform{
                    42
                }
            );

    static_assert(
        tested::same_as<
            tested::ranges::range_value_t<
                decltype(view)
            >,
            int
        >
    );

    return
        tested::ranges::begin(view) ==
        tested::ranges::end(view);
}

constexpr bool
direct_move_only_adjacent_transform_works() {
    int values[] = {
        1,
        2,
        3,
        4
    };

    auto view =
        tested::ranges::views::
            adjacent_transform<2>(
                values,
                move_only_binary_transform{
                    10
                }
            );

    const int expected[] = {
        13,
        15,
        17
    };

    return equal_values(
        view,
        expected,
        3
    );
}

constexpr bool
rvalue_adjacent_transform_closure_works() {
    int values[] = {
        1,
        2,
        3,
        4
    };

    auto closure =
        tested::ranges::views::
            adjacent_transform<2>(
                move_only_binary_transform{
                    1
                }
            );

    auto view =
        values |
        tested::move(closure);

    const int expected[] = {
        4,
        6,
        8
    };

    return equal_values(
        view,
        expected,
        3
    );
}

constexpr bool
predicate_closure_categories_work() {
    int values[] = {
        1,
        2,
        3,
        4
    };

    auto closure =
        tested::ranges::views::
            filter(
                reconstructible_unary_predicate{
                    4
                }
            );

    auto lvalue_view =
        closure(values);

    const int expected[] = {
        1,
        2,
        3
    };

    if (
        !equal_values(
            lvalue_view,
            expected,
            3
        )
    ) {
        return false;
    }

    const auto constant_closure =
        tested::ranges::views::
            filter(
                reconstructible_unary_predicate{
                    3
                }
            );

    auto constant_view =
        constant_closure(values);

    const int constant_expected[] = {
        1,
        2
    };

    return equal_values(
        constant_view,
        constant_expected,
        2
    );
}

constexpr bool
predicate_assignment_reconstructs() {
    int values[] = {
        1,
        2,
        3,
        4
    };

    auto source =
        tested::ranges::views::
            filter(
                values,
                reconstructible_unary_predicate{
                    4
                }
            );

    auto destination =
        tested::ranges::views::
            filter(
                values,
                reconstructible_unary_predicate{
                    2
                }
            );

    destination = source;

    const int expected[] = {
        1,
        2,
        3
    };

    return equal_values(
        destination,
        expected,
        3
    );
}

constexpr bool
take_and_drop_predicates_work() {
    int values[] = {
        1,
        2,
        3,
        4,
        5
    };

    auto taken =
        tested::ranges::views::
            take_while(
                values,
                reconstructible_unary_predicate{
                    4
                }
            );

    const int taken_expected[] = {
        1,
        2,
        3
    };

    if (
        !equal_values(
            taken,
            taken_expected,
            3
        )
    ) {
        return false;
    }

    auto dropped =
        tested::ranges::views::
            drop_while(
                values,
                reconstructible_unary_predicate{
                    4
                }
            );

    const int dropped_expected[] = {
        4,
        5
    };

    return equal_values(
        dropped,
        dropped_expected,
        2
    );
}

constexpr bool
chunk_by_reconstructible_predicate_works() {
    int values[] = {
        1,
        2,
        4,
        5,
        9
    };

    auto view =
        tested::ranges::views::
            chunk_by(
                values,
                reconstructible_binary_predicate{
                    1
                }
            );

    const int expected[] = {
        1,
        2,
        4,
        5,
        9
    };

    const tested::size_t lengths[] = {
        2,
        2,
        1
    };

    auto outer =
        tested::ranges::begin(view);

    const auto bound =
        tested::ranges::end(view);

    tested::size_t offset = 0;

    for (
        tested::size_t index = 0;
        index < 3;
        ++index
    ) {
        if (outer == bound)
            return false;

        if (
            !equal_values(
                *outer,
                expected + offset,
                lengths[index]
            )
        ) {
            return false;
        }

        offset += lengths[index];
        ++outer;
    }

    return outer == bound;
}

constexpr bool
range_adaptor_callables_work() {
    return
        direct_move_only_transform_works() &&
        rvalue_transform_closure_works() &&
        move_only_zip_transform_works() &&
        move_only_nullary_zip_transform_works() &&
        direct_move_only_adjacent_transform_works() &&
        rvalue_adjacent_transform_closure_works() &&
        predicate_closure_categories_work() &&
        predicate_assignment_reconstructs() &&
        take_and_drop_predicates_work() &&
        chunk_by_reconstructible_predicate_works();
}

#ifdef FTL_REPLACE_STL
static_assert(
    range_adaptor_callables_work()
);
#endif

bool ftl_test() {
    return range_adaptor_callables_work();
}
