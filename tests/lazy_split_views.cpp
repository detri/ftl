#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct input_iterator
{
    using value_type = int;

    using difference_type =
    tested::ptrdiff_t;

    using iterator_concept =
    tested::input_iterator_tag;

    int* current = nullptr;

    constexpr int& operator*() const
    {
        return *current;
    }

    constexpr input_iterator&
    operator++()
    {
        ++current;
        return *this;
    }

    constexpr void operator++(int)
    {
        ++current;
    }

    friend constexpr bool operator==(
        input_iterator,
        input_iterator) = default;
};

struct input_sentinel
{
    int* end = nullptr;

    friend constexpr bool operator==(
        input_iterator current,
        input_sentinel bound
    )
    {
        return current.current == bound.end;
    }
};

struct input_view
        : tested::ranges::view_base
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr input_view() = default;

    constexpr input_view(
        int* first,
        int* last
    )
        : first(first),
          last(last) {}

    constexpr input_iterator begin()
    {
        return input_iterator{
            first
        };
    }

    constexpr input_sentinel end()
    {
        return input_sentinel{
            last
        };
    }
};

struct forward_iterator
{
    using value_type = int;

    using difference_type =
    tested::ptrdiff_t;

    using iterator_concept =
    tested::forward_iterator_tag;

    using iterator_category =
    tested::forward_iterator_tag;

    int* current = nullptr;

    constexpr int& operator*() const
    {
        return *current;
    }

    constexpr forward_iterator&
    operator++()
    {
        ++current;
        return *this;
    }

    constexpr forward_iterator
    operator++(int)
    {
        auto previous = *this;
        ++*this;
        return previous;
    }

    friend constexpr bool operator==(
        const forward_iterator&,
        const forward_iterator&
    ) = default;
};

struct forward_sentinel
{
    int* end = nullptr;

    friend constexpr bool operator==(
        forward_iterator current,
        forward_sentinel bound
    )
    {
        return current.current == bound.end;
    }
};

struct forward_view
        : tested::ranges::view_base
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr forward_view() = default;

    constexpr forward_view(
        int* first,
        int* last
    )
        : first(first),
          last(last) {}

    constexpr forward_iterator begin() const
    {
        return forward_iterator{
            first
        };
    }

    constexpr forward_sentinel end() const
    {
        return forward_sentinel{
            last
        };
    }
};

template<class Iterator>
concept has_iterator_category =
        requires
        {
            typename Iterator::iterator_category;
        };

template<
    class Range,
    class Pattern
>
concept can_lazy_split =
        requires(
    Range&& range,
    Pattern&& pattern
)
        {
            tested::ranges::views::
            lazy_split(
                static_cast<Range&&>(
                    range
                ),
                static_cast<Pattern&&>(
                    pattern
                )
            );
        };

constexpr bool equal_sequence(
    auto&& range,
    const int* expected,
    tested::size_t size
)
{
    auto current =
            tested::ranges::begin(range);

    const auto bound =
            tested::ranges::end(range);

    for (
        tested::size_t index = 0;
        index < size;
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

constexpr bool equal_parts(
    auto&& range,
    const int* expected,
    const tested::size_t* lengths,
    tested::size_t count
)
{
    auto outer =
            tested::ranges::begin(range);

    const auto bound =
            tested::ranges::end(range);

    tested::size_t offset = 0;

    for (
        tested::size_t index = 0;
        index < count;
        ++index
    )
    {
        if (outer == bound)
            return false;

        if (
            !equal_sequence(
                *outer,
                expected + offset,
                lengths[index]
            )
        )
        {
            return false;
        }

        offset += lengths[index];
        ++outer;
    }

    return outer == bound;
}

using array_type = int[4];

using forward_lazy_view =
decltype(
    tested::ranges::views::
    lazy_split(
        tested::declval<
            array_type&>(),
        0
    )
);

using forward_outer_iterator =
tested::ranges::iterator_t<
    forward_lazy_view
>;

using forward_inner_view =
tested::ranges::range_value_t<
    forward_lazy_view
>;

using forward_inner_iterator =
tested::ranges::iterator_t<
    forward_inner_view
>;

using input_lazy_view =
decltype(
    tested::ranges::views::
    lazy_split(
        input_view{},
        0
    )
);

using input_outer_iterator =
tested::ranges::iterator_t<
    input_lazy_view
>;

using input_inner_view =
tested::ranges::range_value_t<
    input_lazy_view
>;

using input_inner_iterator =
tested::ranges::iterator_t<
    input_inner_view
>;

using non_common_lazy_view =
decltype(
    tested::ranges::views::
    lazy_split(
        forward_view{},
        0
    )
);

/*
 * Forward-base outer range behavior.
 */

static_assert(
    tested::ranges::view<
        forward_lazy_view
    >
);

static_assert(
    tested::ranges::forward_range<
        forward_lazy_view
    >
);

static_assert(
    tested::ranges::common_range<
        forward_lazy_view
    >
);

static_assert(
    tested::ranges::range<
        const forward_lazy_view>
);

static_assert(
    tested::ranges::common_range<
        const forward_lazy_view>
);

/*
 * The yielded inner view is itself forward when the
 * underlying range is forward.
 */

static_assert(
    tested::ranges::view<
        forward_inner_view
    >
);

static_assert(
    tested::ranges::forward_range<
        forward_inner_view
    >
);

static_assert(tested::same_as<
    typename forward_outer_iterator::
    iterator_concept,
    tested::forward_iterator_tag
>);

static_assert(tested::same_as<
    typename tested::iterator_traits<
        forward_outer_iterator
    >::iterator_category,
    tested::input_iterator_tag
>);

static_assert(tested::same_as<
    typename forward_inner_iterator::
    iterator_concept,
    tested::forward_iterator_tag
>);

static_assert(tested::same_as<
    typename tested::iterator_traits<
        forward_inner_iterator
    >::iterator_category,
    tested::forward_iterator_tag
>);

static_assert(tested::same_as<
    decltype(
        tested::declval<
            forward_outer_iterator&>()++
    ),
    forward_outer_iterator
>);

static_assert(tested::same_as<
    decltype(
        tested::declval<
            forward_inner_iterator&>()++
    ),
    forward_inner_iterator
>);

/*
 * Single-pass base behavior.
 */

static_assert(
    tested::ranges::input_range<
        input_lazy_view
    >
);

static_assert(
    !tested::ranges::forward_range<
        input_lazy_view
    >
);

static_assert(
    !tested::ranges::range<
        const input_lazy_view>
);

static_assert(
    tested::ranges::view<
        input_inner_view
    >
);

static_assert(
    tested::ranges::input_range<
        input_inner_view
    >
);

static_assert(
    !tested::ranges::forward_range<
        input_inner_view
    >
);

static_assert(tested::same_as<
    typename input_outer_iterator::
    iterator_concept,
    tested::input_iterator_tag
>);

static_assert(tested::same_as<
    typename input_inner_iterator::
    iterator_concept,
    tested::input_iterator_tag
>);

static_assert(
    !has_iterator_category<
        input_outer_iterator
    >
);

static_assert(
    !has_iterator_category<
        input_inner_iterator
    >
);

static_assert(tested::same_as<
    decltype(
        tested::declval<
            input_outer_iterator&>()++
    ),
    void>);

static_assert(tested::same_as<
    decltype(
        tested::declval<
            input_inner_iterator&>()++
    ),
    void>);

/*
 * A non-common forward base remains forward but produces
 * default_sentinel for the outer range.
 */

static_assert(
    tested::ranges::forward_range<
        non_common_lazy_view
    >
);

static_assert(
    !tested::ranges::common_range<
        non_common_lazy_view
    >
);

static_assert(
    tested::ranges::range<
        const non_common_lazy_view>
);

static_assert(
    !tested::ranges::common_range<
        const non_common_lazy_view>
);

/*
 * Constraint coverage:
 *
 * - input bases accept a one-element delimiter;
 * - input bases reject multi-element patterns;
 * - forward bases accept multi-element patterns;
 * - Pattern itself must be forward.
 */

static_assert(
    can_lazy_split<
        input_view,
        int>
);

static_assert(
    !can_lazy_split<
        input_view,
        int(&)[2]
    >
);

static_assert(
    can_lazy_split<
        int(&)[4],
        int(&)[2]
    >
);

static_assert(
    !can_lazy_split<
        int(&)[4],
        input_view
    >
);

constexpr bool
input_element_delimiter_works()
{
    int values[] = {
        0,
        1,
        0,
        0,
        2,
        0
    };

    auto view =
            input_view(
                values,
                values + 6
            ) |
            tested::ranges::views::
            lazy_split(0);

    const int expected[] = {
        1,
        2
    };

    const tested::size_t lengths[] = {
        0,
        1,
        0,
        1,
        0
    };

    return equal_parts(
        view,
        expected,
        lengths,
        5
    );
}

constexpr bool
input_empty_base_works()
{
    int value = 0;

    auto view =
            tested::ranges::views::
            lazy_split(
                input_view(
                    &value,
                    &value
                ),
                0
            );

    return
            tested::ranges::begin(view) ==
            tested::ranges::end(view);
}

constexpr bool
input_empty_pattern_works()
{
    int values[] = {
        4,
        5,
        6
    };

    auto view =
            tested::ranges::views::
            lazy_split(
                input_view(
                    values,
                    values + 3
                ),
                tested::ranges::
                empty_view<int>{}
            );

    const int expected[] = {
        4,
        5,
        6
    };

    const tested::size_t lengths[] = {
        1,
        1,
        1
    };

    return equal_parts(
        view,
        expected,
        lengths,
        3
    );
}

constexpr bool
input_skipped_and_partial_parts_work()
{
    int values[] = {
        1,
        2,
        0,
        3,
        4,
        0,
        5
    };

    auto view =
            tested::ranges::views::
            lazy_split(
                input_view(
                    values,
                    values + 7
                ),
                0
            );

    auto outer =
            view.begin();

    /*
     * Skip the first inner range without calling begin().
     */
    ++outer;

    if (outer == view.end())
        return false;

    /*
     * Partially consume {3, 4}, leaving the shared input
     * cursor positioned at 4.
     */
    auto inner =
            (*outer).begin();

    if (
        inner == (*outer).end() ||
        *inner != 3
    )
    {
        return false;
    }

    ++inner;

    if (
        inner == (*outer).end() ||
        *inner != 4
    )
    {
        return false;
    }

    /*
     * Advancing the outer iterator must finish scanning the
     * partially consumed segment and skip its delimiter.
     */
    ++outer;

    if (outer == view.end())
        return false;

    const int expected[] = {
        5
    };

    if (
        !equal_sequence(
            *outer,
            expected,
            1
        )
    )
    {
        return false;
    }

    ++outer;

    return outer == view.end();
}

constexpr bool
forward_element_delimiter_works()
{
    int values[] = {
        0,
        1,
        0,
        0,
        2,
        0
    };

    auto view =
            tested::ranges::views::
            lazy_split(
                values,
                0
            );

    const int expected[] = {
        1,
        2
    };

    const tested::size_t lengths[] = {
        0,
        1,
        0,
        1,
        0
    };

    if (
        !equal_parts(
            view,
            expected,
            lengths,
            5
        )
    )
    {
        return false;
    }

    const auto& constant_view =
            view;

    return equal_parts(
        constant_view,
        expected,
        lengths,
        5
    );
}

constexpr bool
forward_multi_element_pattern_works()
{
    int values[] = {
        1,
        9,
        9,
        2,
        9,
        9,
        9,
        3
    };

    int pattern[] = {
        9,
        9
    };

    auto direct =
            tested::ranges::views::
            lazy_split(
                values,
                pattern
            );

    const int expected[] = {
        1,
        2,
        9,
        3
    };

    const tested::size_t lengths[] = {
        1,
        1,
        2
    };

    if (
        !equal_parts(
            direct,
            expected,
            lengths,
            3
        )
    )
    {
        return false;
    }

    /*
     * Exercise the closure form using a view as the stored
     * range pattern.
     */
    auto pattern_view =
            tested::ranges::views::all(
                pattern
            );

    auto piped =
            values |
            tested::ranges::views::
            lazy_split(pattern_view);

    return equal_parts(
        piped,
        expected,
        lengths,
        3
    );
}

constexpr bool
forward_consecutive_patterns_work()
{
    int values[] = {
        1,
        0,
        0,
        0,
        0,
        2
    };

    int pattern[] = {
        0,
        0
    };

    auto view =
            tested::ranges::views::
            lazy_split(
                values,
                pattern
            );

    const int expected[] = {
        1,
        2
    };

    const tested::size_t lengths[] = {
        1,
        0,
        1
    };

    return equal_parts(
        view,
        expected,
        lengths,
        3
    );
}

constexpr bool
forward_empty_pattern_works()
{
    int values[] = {
        7,
        8,
        9
    };

    auto view =
            tested::ranges::views::
            lazy_split(
                values,
                tested::ranges::
                empty_view<int>{}
            );

    const int expected[] = {
        7,
        8,
        9
    };

    const tested::size_t lengths[] = {
        1,
        1,
        1
    };

    return equal_parts(
        view,
        expected,
        lengths,
        3
    );
}

constexpr bool
forward_inner_iteration_is_independent()
{
    int values[] = {
        1,
        2,
        0,
        3
    };

    auto view =
            tested::ranges::views::
            lazy_split(
                values,
                0
            );

    auto outer =
            view.begin();

    auto inner =
            (*outer).begin();

    if (*inner != 1)
        return false;

    ++inner;

    if (
        inner == (*outer).end() ||
        *inner != 2
    )
    {
        return false;
    }

    /*
     * Forward inner iteration owns a copied outer position.
     * Consuming it must not alter the outer iterator.
     */
    ++outer;

    if (outer == view.end())
        return false;

    const int expected[] = {
        3
    };

    return equal_sequence(
        *outer,
        expected,
        1
    );
}

constexpr bool
non_common_forward_base_works()
{
    int values[] = {
        1,
        0,
        0,
        2,
        0
    };

    forward_view source(
        values,
        values + 5
    );

    auto view =
            tested::ranges::views::
            lazy_split(
                source,
                0
            );

    const int expected[] = {
        1,
        2
    };

    const tested::size_t lengths[] = {
        1,
        0,
        1,
        0
    };

    if (
        !equal_parts(
            view,
            expected,
            lengths,
            4
        )
    )
    {
        return false;
    }

    const auto& constant_view =
            view;

    return equal_parts(
        constant_view,
        expected,
        lengths,
        4
    );
}

constexpr bool
inner_customizations_work()
{
    int values[] = {
        1,
        0,
        2
    };

    auto view =
            tested::ranges::views::
            lazy_split(
                values,
                0
            );

    auto first_outer =
            view.begin();

    auto second_outer =
            first_outer;

    ++second_outer;

    auto first_inner =
            (*first_outer).begin();

    auto second_inner =
            (*second_outer).begin();

    static_assert(tested::same_as<
        decltype(
            tested::ranges::iter_move(
                first_inner
            )
        ),
        int&&>);

    auto&& moved =
            tested::ranges::iter_move(
                first_inner
            );

    if (moved != 1)
        return false;

    moved = 4;

    tested::ranges::iter_swap(
        first_inner,
        second_inner
    );

    return
            values[0] == 2 &&
            values[2] == 4;
}

constexpr bool
direct_construction_works()
{
    int values[] = {
        1,
        0,
        2
    };

    tested::ranges::lazy_split_view
            element_view(
                values,
                0
            );

    const int expected[] = {
        1,
        2
    };

    const tested::size_t lengths[] = {
        1,
        1
    };

    if (
        !equal_parts(
            element_view,
            expected,
            lengths,
            2
        )
    )
    {
        return false;
    }

    int pattern[] = {
        0
    };

    tested::ranges::lazy_split_view
            pattern_view(
                values,
                pattern
            );

    if (
        !equal_parts(
            pattern_view,
            expected,
            lengths,
            2
        )
    )
    {
        return false;
    }

    auto copied_base =
            element_view.base();

    return
            tested::ranges::begin(
                copied_base
            ) == values;
}

constexpr bool lazy_split_works()
{
    return
            input_element_delimiter_works() &&
            input_empty_base_works() &&
            input_empty_pattern_works() &&
            input_skipped_and_partial_parts_work() &&
            forward_element_delimiter_works() &&
            forward_multi_element_pattern_works() &&
            forward_consecutive_patterns_work() &&
            forward_empty_pattern_works() &&
            forward_inner_iteration_is_independent() &&
            non_common_forward_base_works() &&
            inner_customizations_work() &&
            direct_construction_works();
}

static_assert(
    lazy_split_works()
);

bool ftl_test()
{
    return lazy_split_works();
}
