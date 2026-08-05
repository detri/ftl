#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/ranges>
#include <ftl/tuple>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

/*
 * Expression probes.
 */

template<class Left, class Right>
concept can_compose =
    requires {
        tested::declval<Left>() |
        tested::declval<Right>();
    };

template<class Closure, class Range>
concept can_call_closure =
    requires {
        tested::declval<Closure>()(
            tested::declval<Range>()
        );
    };

template<class Range, class Closure>
concept can_pipe =
    requires {
        tested::declval<Range>() |
        tested::declval<Closure>();
    };

template<class Closure, class Range>
consteval bool
closure_call_is_noexcept()
{
    if constexpr (
        can_call_closure<
            Closure,
            Range
        >
    )
    {
        return noexcept(
            tested::declval<Closure>()(
                tested::declval<Range>()
            )
        );
    }
    else
    {
        return false;
    }
}

using int_range =
    int (&)[8];

/*
 * Basic custom closure objects.
 */

struct identity_closure
    : tested::ranges::range_adaptor_closure<
          identity_closure
      >
{
    template<tested::ranges::range R>
    constexpr R&& operator()(
        R&& range
    ) const noexcept
    {
        return static_cast<R&&>(range);
    }
};

struct throwing_identity_closure
    : tested::ranges::range_adaptor_closure<
          throwing_identity_closure
      >
{
    template<tested::ranges::range R>
    constexpr R&& operator()(
        R&& range
    ) const noexcept(false)
    {
        return static_cast<R&&>(range);
    }
};

struct plain_callable
{
    template<tested::ranges::range R>
    constexpr R&& operator()(
        R&& range
    ) const noexcept
    {
        return static_cast<R&&>(range);
    }
};

/*
 * Detect which cvref category a composed closure forwards
 * to each stored state entity.
 */

struct cvref_probe_closure
    : tested::ranges::range_adaptor_closure<
          cvref_probe_closure
      >
{
    int* lvalue_calls = nullptr;
    int* const_lvalue_calls = nullptr;
    int* rvalue_calls = nullptr;
    int* const_rvalue_calls = nullptr;

    constexpr cvref_probe_closure(
        int& lvalue,
        int& const_lvalue,
        int& rvalue,
        int& const_rvalue
    ) noexcept
        : lvalue_calls(
              tested::addressof(lvalue)
          ),
          const_lvalue_calls(
              tested::addressof(
                  const_lvalue
              )
          ),
          rvalue_calls(
              tested::addressof(rvalue)
          ),
          const_rvalue_calls(
              tested::addressof(
                  const_rvalue
              )
          )
    {
    }

    template<tested::ranges::range R>
    constexpr R&& operator()(
        R&& range
    ) &
    {
        ++*lvalue_calls;
        return static_cast<R&&>(range);
    }

    template<tested::ranges::range R>
    constexpr R&& operator()(
        R&& range
    ) const&
    {
        ++*const_lvalue_calls;
        return static_cast<R&&>(range);
    }

    template<tested::ranges::range R>
    constexpr R&& operator()(
        R&& range
    ) &&
    {
        ++*rvalue_calls;
        return static_cast<R&&>(range);
    }

    template<tested::ranges::range R>
    constexpr R&& operator()(
        R&& range
    ) const&&
    {
        ++*const_rvalue_calls;
        return static_cast<R&&>(range);
    }
};

/*
 * A closure callable only when its stored state is forwarded
 * as const rvalue.
 */

struct const_rvalue_only_closure
    : tested::ranges::range_adaptor_closure<
          const_rvalue_only_closure
      >
{
    template<tested::ranges::range R>
    constexpr R&& operator()(
        R&& range
    ) const&& noexcept
    {
        return static_cast<R&&>(range);
    }
};

/*
 * Invocation-order recording.
 */

struct recording_closure
    : tested::ranges::range_adaptor_closure<
          recording_closure
      >
{
    int* log = nullptr;
    int digit = 0;

    constexpr recording_closure(
        int& value,
        int digit
    ) noexcept
        : log(
              tested::addressof(value)
          ),
          digit(digit)
    {
    }

    template<tested::ranges::range R>
    constexpr R&& operator()(
        R&& range
    ) const noexcept
    {
        *log =
            *log * 10 +
            digit;

        return static_cast<R&&>(range);
    }
};

/*
 * Move-only closure state.
 */

struct move_only_identity_closure
    : tested::ranges::range_adaptor_closure<
          move_only_identity_closure
      >
{
    int token = 0;

    constexpr explicit
    move_only_identity_closure(
        int token = 0
    ) noexcept
        : token(token)
    {
    }

    move_only_identity_closure(
        const move_only_identity_closure&
    ) = delete;

    move_only_identity_closure& operator=(
        const move_only_identity_closure&
    ) = delete;

    constexpr move_only_identity_closure(
        move_only_identity_closure&&
    ) noexcept = default;

    constexpr move_only_identity_closure&
    operator=(
        move_only_identity_closure&&
    ) noexcept = default;

    template<tested::ranges::range R>
    constexpr R&& operator()(
        R&& range
    ) && noexcept
    {
        return static_cast<R&&>(range);
    }
};

struct immobile_closure
    : tested::ranges::range_adaptor_closure<
          immobile_closure
      >
{
    immobile_closure() = default;

    immobile_closure(
        const immobile_closure&
    ) = delete;

    immobile_closure(
        immobile_closure&&
    ) = delete;

    immobile_closure& operator=(
        const immobile_closure&
    ) = delete;

    immobile_closure& operator=(
        immobile_closure&&
    ) = delete;

    template<tested::ranges::range R>
    constexpr R&& operator()(
        R&& range
    ) const noexcept
    {
        return static_cast<R&&>(range);
    }
};

/*
 * Range-value-category forwarding.
 */

struct movable_pointer_view
    : tested::ranges::view_base
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr movable_pointer_view() =
        default;

    constexpr movable_pointer_view(
        int* first,
        int* last
    ) noexcept
        : first(first),
          last(last)
    {
    }

    movable_pointer_view(
        const movable_pointer_view&
    ) = delete;

    movable_pointer_view& operator=(
        const movable_pointer_view&
    ) = delete;

    constexpr movable_pointer_view(
        movable_pointer_view&&
    ) noexcept = default;

    constexpr movable_pointer_view&
    operator=(
        movable_pointer_view&&
    ) noexcept = default;

    constexpr int* begin() noexcept
    {
        return first;
    }

    constexpr int* end() noexcept
    {
        return last;
    }
};

struct lvalue_range_only_closure
    : tested::ranges::range_adaptor_closure<
          lvalue_range_only_closure
      >
{
    template<class R>
        requires (
            tested::ranges::range<R> &&
            tested::is_lvalue_reference_v<
                R&&
            >
        )
    constexpr R&& operator()(
        R&& range
    ) const noexcept
    {
        return static_cast<R&&>(range);
    }
};

struct rvalue_range_only_closure
    : tested::ranges::range_adaptor_closure<
          rvalue_range_only_closure
      >
{
    template<class R>
        requires (
            tested::ranges::range<R> &&
            !tested::is_lvalue_reference_v<
                R&&
            >
        )
    constexpr tested::remove_cvref_t<R>
    operator()(
        R&& range
    ) const
    {
        return tested::remove_cvref_t<R>(
            static_cast<R&&>(range)
        );
    }
};

/*
 * Standard-adaptor callables.
 */

struct is_even
{
    constexpr bool operator()(
        int value
    ) const noexcept
    {
        return value % 2 == 0;
    }
};

struct less_than
{
    int limit = 0;

    constexpr bool operator()(
        int value
    ) const noexcept
    {
        return value < limit;
    }
};

struct times_ten
{
    constexpr int operator()(
        int value
    ) const noexcept
    {
        return value * 10;
    }
};

struct move_only_multiplier
{
    int multiplier = 1;

    constexpr explicit
    move_only_multiplier(
        int multiplier
    ) noexcept
        : multiplier(multiplier)
    {
    }

    move_only_multiplier(
        const move_only_multiplier&
    ) = delete;

    move_only_multiplier& operator=(
        const move_only_multiplier&
    ) = delete;

    constexpr move_only_multiplier(
        move_only_multiplier&&
    ) noexcept = default;

    constexpr move_only_multiplier&
    operator=(
        move_only_multiplier&&
    ) noexcept = default;

    constexpr int operator()(
        int value
    ) const noexcept
    {
        return value * multiplier;
    }
};

struct tuple_sum
{
    template<class Tuple>
    constexpr int operator()(
        Tuple&& value
    ) const
    {
        return
            tested::get<0>(
                static_cast<Tuple&&>(
                    value
                )
            ) +
            tested::get<1>(
                static_cast<Tuple&&>(
                    value
                )
            );
    }
};

struct range_sum
{
    template<tested::ranges::range R>
    constexpr int operator()(
        R&& range
    ) const
    {
        int result = 0;

        auto current =
            tested::ranges::begin(
                range
            );

        const auto bound =
            tested::ranges::end(
                range
            );

        while (current != bound)
        {
            result += *current;
            ++current;
        }

        return result;
    }
};

/*
 * Sequence comparison.
 */

template<class Range>
constexpr bool equal_range(
    Range&& range,
    const int* expected,
    tested::size_t count
)
{
    auto current =
        tested::ranges::begin(range);

    const auto bound =
        tested::ranges::end(range);

    for (
        tested::size_t index = 0;
        index < count;
        ++index,
        ++current
    )
    {
        if (
            current == bound ||
            *current != expected[index]
        )
        {
            return false;
        }
    }

    return current == bound;
}

/*
 * Basic closure recognition and expression equivalence.
 */

static_assert(
    can_compose<
        identity_closure,
        identity_closure
    >
);

static_assert(
    !can_compose<
        plain_callable,
        identity_closure
    >
);

static_assert(
    !can_compose<
        identity_closure,
        plain_callable
    >
);

using direct_identity_result =
    decltype(
        tested::declval<
            identity_closure&
        >()(
            tested::declval<
                int_range
            >()
        )
    );

using piped_identity_result =
    decltype(
        tested::declval<
            int_range
        >() |
        tested::declval<
            identity_closure&
        >()
    );

static_assert(tested::same_as<
    direct_identity_result,
    piped_identity_result
>);

/*
 * Composition decays both stored closure types.
 */

using identity_pipeline_from_lvalues =
    decltype(
        tested::declval<
            identity_closure&
        >() |
        tested::declval<
            recording_closure&
        >()
    );

using identity_pipeline_from_const_lvalues =
    decltype(
        tested::declval<
            const identity_closure&
        >() |
        tested::declval<
            const recording_closure&
        >()
    );

using identity_pipeline_from_rvalues =
    decltype(
        tested::declval<
            identity_closure&&
        >() |
        tested::declval<
            recording_closure&&
        >()
    );

static_assert(tested::same_as<
    identity_pipeline_from_lvalues,
    identity_pipeline_from_const_lvalues
>);

static_assert(tested::same_as<
    identity_pipeline_from_lvalues,
    identity_pipeline_from_rvalues
>);

/*
 * Composition itself depends only on storing C and D.
 *
 * reverse | join is storable even though it cannot be called
 * with a flat integer range.
 */

using reverse_join_pipeline =
    decltype(
        tested::ranges::views::reverse |
        tested::ranges::views::join
    );

static_assert(
    can_compose<
        decltype(
            tested::ranges::views::
                reverse
        ),
        decltype(
            tested::ranges::views::
                join
        )
    >
);

static_assert(
    !can_call_closure<
        reverse_join_pipeline&,
        int_range
    >
);

/*
 * Immobile state cannot be captured by composition.
 */

static_assert(
    !can_compose<
        immobile_closure,
        identity_closure
    >
);

static_assert(
    !can_compose<
        identity_closure,
        immobile_closure
    >
);

/*
 * Perfect-forwarding call-wrapper exception specification.
 */

using nothrow_pipeline =
    decltype(
        identity_closure{} |
        identity_closure{}
    );

using throwing_pipeline =
    decltype(
        identity_closure{} |
        throwing_identity_closure{}
    );

static_assert(
    closure_call_is_noexcept<
        nothrow_pipeline&,
        int_range
    >()
);

static_assert(
    !closure_call_is_noexcept<
        throwing_pipeline&,
        int_range
    >()
);

/*
 * const-rvalue state forwarding.
 */

using const_rvalue_pipeline =
    decltype(
        const_rvalue_only_closure{} |
        const_rvalue_only_closure{}
    );

static_assert(
    !can_call_closure<
        const_rvalue_pipeline&,
        int_range
    >
);

static_assert(
    !can_call_closure<
        const const_rvalue_pipeline&,
        int_range
    >
);

static_assert(
    can_call_closure<
        const_rvalue_pipeline&&,
        int_range
    >
);

static_assert(
    can_call_closure<
        const const_rvalue_pipeline&&,
        int_range
    >
);

/*
 * Move-only closure composition.
 */

using take_two_closure =
    decltype(
        tested::ranges::views::take(2)
    );

static_assert(
    !can_compose<
        move_only_identity_closure&,
        take_two_closure
    >
);

static_assert(
    can_compose<
        move_only_identity_closure,
        take_two_closure
    >
);

using move_only_identity_pipeline =
    decltype(
        move_only_identity_closure{} |
        tested::ranges::views::take(2)
    );

static_assert(
    tested::move_constructible<
        move_only_identity_pipeline
    >
);

static_assert(
    !tested::copy_constructible<
        move_only_identity_pipeline
    >
);

static_assert(
    !can_call_closure<
        move_only_identity_pipeline&,
        int_range
    >
);

static_assert(
    can_call_closure<
        move_only_identity_pipeline&&,
        int_range
    >
);

static_assert(
    !can_pipe<
        int_range,
        move_only_identity_pipeline&
    >
);

static_assert(
    can_pipe<
        int_range,
        move_only_identity_pipeline&&
    >
);

/*
 * A partial adaptor containing a move-only bound callable
 * remains movable through composition.
 */

using move_transform_closure =
    decltype(
        tested::ranges::views::transform(
            move_only_multiplier{
                3
            }
        )
    );

using move_transform_pipeline =
    decltype(
        tested::ranges::views::transform(
            move_only_multiplier{
                3
            }
        ) |
        tested::ranges::views::take(2)
    );

static_assert(
    tested::move_constructible<
        move_transform_pipeline
    >
);

static_assert(
    !tested::copy_constructible<
        move_transform_pipeline
    >
);

static_assert(
    !can_compose<
        move_transform_closure&,
        take_two_closure
    >
);

static_assert(
    can_compose<
        move_transform_closure&&,
        take_two_closure
    >
);

static_assert(
    !can_call_closure<
        move_transform_pipeline&,
        int_range
    >
);

static_assert(
    can_call_closure<
        move_transform_pipeline&&,
        int_range
    >
);

/*
 * Range argument value-category forwarding.
 */

using lvalue_range_pipeline =
    decltype(
        lvalue_range_only_closure{} |
        identity_closure{}
    );

using rvalue_range_pipeline =
    decltype(
        rvalue_range_only_closure{} |
        identity_closure{}
    );

static_assert(
    can_call_closure<
        lvalue_range_pipeline&,
        movable_pointer_view&
    >
);

static_assert(
    !can_call_closure<
        lvalue_range_pipeline&,
        movable_pointer_view&&
    >
);

static_assert(
    !can_call_closure<
        rvalue_range_pipeline&,
        movable_pointer_view&
    >
);

static_assert(
    can_call_closure<
        rvalue_range_pipeline&,
        movable_pointer_view&&
    >
);

/*
 * Verify the stored closures receive the composed object's
 * cvref category.
 */

constexpr bool cvref_forwarding_works()
{
    int left_lvalue = 0;
    int left_const_lvalue = 0;
    int left_rvalue = 0;
    int left_const_rvalue = 0;

    int right_lvalue = 0;
    int right_const_lvalue = 0;
    int right_rvalue = 0;
    int right_const_rvalue = 0;

    int values[] = {
        1,
        2,
        3
    };

    auto composed =
        cvref_probe_closure{
            left_lvalue,
            left_const_lvalue,
            left_rvalue,
            left_const_rvalue
        } |
        cvref_probe_closure{
            right_lvalue,
            right_const_lvalue,
            right_rvalue,
            right_const_rvalue
        };

    composed(values);

    if (
        left_lvalue != 1 ||
        right_lvalue != 1
    )
    {
        return false;
    }

    const auto& constant =
        composed;

    constant(values);

    if (
        left_const_lvalue != 1 ||
        right_const_lvalue != 1
    )
    {
        return false;
    }

    tested::move(composed)(
        values
    );

    if (
        left_rvalue != 1 ||
        right_rvalue != 1
    )
    {
        return false;
    }

    return
        left_const_rvalue == 0 &&
        right_const_rvalue == 0;
}

/*
 * Call pattern and associativity.
 */

constexpr bool invocation_order_works()
{
    int values[] = {
        1,
        2,
        3
    };

    int log = 0;

    auto left_associated =
        recording_closure{
            log,
            1
        } |
        recording_closure{
            log,
            2
        } |
        recording_closure{
            log,
            3
        };

    left_associated(values);

    if (log != 123)
    {
        return false;
    }

    log = 0;

    auto right_associated =
        recording_closure{
            log,
            1
        } |
        (
            recording_closure{
                log,
                2
            } |
            recording_closure{
                log,
                3
            }
        );

    static_cast<void>(values | right_associated);

    return log == 123;
}

/*
 * Bound arguments are decay-copied into partial closures.
 */

constexpr bool bound_argument_decay_works()
{
    int values[] = {
        1,
        2,
        3,
        4,
        5
    };

    int count = 2;

    auto take_count =
        tested::ranges::views::take(
            count
        );

    count = 4;

    auto taken =
        values |
        take_count;

    if (
        taken.size() != 2 ||
        taken[0] != 1 ||
        taken[1] != 2
    )
    {
        return false;
    }

    less_than predicate{
        4
    };

    auto take_while =
        tested::ranges::views::
            take_while(
                predicate
            );

    predicate.limit = 2;

    const int expected[] = {
        1,
        2,
        3
    };

    auto result =
        values |
        take_while;

    if (
        !equal_range(
            result,
            expected,
            3
        )
    )
    {
        return false;
    }

    auto composed =
        take_count |
        tested::ranges::views::
            transform(
                times_ten{}
            );

    const int transformed_expected[] = {
        10,
        20
    };

    return equal_range(
        values | composed,
        transformed_expected,
        2
    );
}

/*
 * Representative standard-adaptor compositions.
 */

constexpr bool standard_composition_works()
{
    int values[] = {
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7
    };

    auto closure =
        tested::ranges::views::drop(1) |
        tested::ranges::views::take(5) |
        tested::ranges::views::filter(
            is_even{}
        ) |
        tested::ranges::views::transform(
            times_ten{}
        ) |
        tested::ranges::views::reverse;

    const int expected[] = {
        40,
        20
    };

    auto directly_called =
        closure(values);

    if (
        !equal_range(
            directly_called,
            expected,
            2
        )
    )
    {
        return false;
    }

    auto piped =
        values |
        closure;

    if (
        !equal_range(
            piped,
            expected,
            2
        )
    )
    {
        return false;
    }

    const auto constant_closure =
        tested::ranges::views::drop(1) |
        tested::ranges::views::take(5) |
        tested::ranges::views::filter(
            is_even{}
        ) |
        tested::ranges::views::transform(
            times_ten{}
        ) |
        tested::ranges::views::reverse;

    auto const_result =
        constant_closure(values);

    if (
        !equal_range(
            const_result,
            expected,
            2
        )
    )
    {
        return false;
    }

    auto movable_closure =
        tested::ranges::views::drop(1) |
        tested::ranges::views::take(5) |
        tested::ranges::views::filter(
            is_even{}
        ) |
        tested::ranges::views::transform(
            times_ten{}
        ) |
        tested::ranges::views::reverse;

    auto moved_result =
        values |
        tested::move(
            movable_closure
        );

    if (
        !equal_range(
            moved_result,
            expected,
            2
        )
    )
    {
        return false;
    }

    auto left_associated =
        (
            tested::ranges::views::drop(1) |
            tested::ranges::views::take(4)
        ) |
        tested::ranges::views::transform(
            times_ten{}
        );

    auto right_associated =
        tested::ranges::views::drop(1) |
        (
            tested::ranges::views::take(4) |
            tested::ranges::views::transform(
                times_ten{}
            )
        );

    const int associated_expected[] = {
        10,
        20,
        30,
        40
    };

    if (
        !equal_range(
            values |
                left_associated,
            associated_expected,
            4
        )
    )
    {
        return false;
    }

    if (
        !equal_range(
            values |
                right_associated,
            associated_expected,
            4
        )
    )
    {
        return false;
    }

    auto stride_reverse =
        tested::ranges::views::stride(2) |
        tested::ranges::views::reverse;

    const int stride_expected[] = {
        6,
        4,
        2,
        0
    };

    if (
        !equal_range(
            values |
                stride_reverse,
            stride_expected,
            4
        )
    )
    {
        return false;
    }

    return true;
}

/*
 * Compositions whose intermediate values are themselves views
 * of tuples or subranges.
 */

constexpr bool structural_composition_works()
{
    int values[] = {
        1,
        2,
        3,
        4,
        5,
        6,
        7
    };

    auto adjacent_sums =
        tested::ranges::views::
            adjacent<2> |
        tested::ranges::views::
            transform(
                tuple_sum{}
            );

    const int adjacent_expected[] = {
        3,
        5,
        7,
        9,
        11,
        13
    };

    if (
        !equal_range(
            values |
                adjacent_sums,
            adjacent_expected,
            6
        )
    )
    {
        return false;
    }

    auto chunk_sums =
        tested::ranges::views::chunk(3) |
        tested::ranges::views::transform(
            range_sum{}
        );

    const int chunk_expected[] = {
        6,
        15,
        7
    };

    if (
        !equal_range(
            values |
                chunk_sums,
            chunk_expected,
            3
        )
    )
    {
        return false;
    }

    int split_values[] = {
        1,
        0,
        2,
        0,
        3
    };

    auto split_join =
        tested::ranges::views::split(0) |
        tested::ranges::views::join;

    const int joined_expected[] = {
        1,
        2,
        3
    };

    if (
        !equal_range(
            split_values |
                split_join,
            joined_expected,
            3
        )
    )
    {
        return false;
    }

    return true;
}

/*
 * Execute a composition containing a move-only bound callable.
 */

constexpr bool move_only_composition_works()
{
    int values[] = {
        1,
        2,
        3,
        4
    };

    auto closure =
        tested::ranges::views::transform(
            move_only_multiplier{
                3
            }
        ) |
        tested::ranges::views::take(2);

    auto result =
        values |
        tested::move(closure);

    const int expected[] = {
        3,
        6
    };

    return equal_range(
        result,
        expected,
        2
    );
}

static_assert(
    cvref_forwarding_works()
);

static_assert(
    invocation_order_works()
);

static_assert(
    bound_argument_decay_works()
);

static_assert(
    standard_composition_works()
);

static_assert(
    structural_composition_works()
);

static_assert(
    move_only_composition_works()
);

bool ftl_test()
{
    return
        cvref_forwarding_works() &&
        invocation_order_works() &&
        bound_argument_decay_works() &&
        standard_composition_works() &&
        structural_composition_works() &&
        move_only_composition_works();
}