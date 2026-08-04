#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <iterator>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/tuple>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

/*
 * Generic adaptor probes.
 */

template<class Adaptor, class... Arguments>
concept can_invoke_adaptor =
    requires {
        tested::declval<Adaptor&>()(
            tested::declval<Arguments>()...
        );
    };

template<class Range, class Closure>
concept can_pipe =
    requires {
        tested::declval<Range>() |
        tested::declval<Closure>();
    };

/*
 * Test ranges.
 */

struct input_iterator {
    using value_type = int;
    using difference_type = tested::ptrdiff_t;
    using iterator_concept = tested::input_iterator_tag;

    int* current = nullptr;

    constexpr int& operator*() const {
        return *current;
    }

    constexpr input_iterator& operator++() {
        ++current;
        return *this;
    }

    constexpr void operator++(int) {
        ++current;
    }

    friend constexpr bool operator==(
        input_iterator,
        input_iterator
    ) = default;
};

struct input_sentinel {
    int* end = nullptr;

    friend constexpr bool operator==(
        input_iterator current,
        input_sentinel bound
    ) {
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

    constexpr input_iterator begin() {
        return {first};
    }

    constexpr input_sentinel end() {
        return {last};
    }
};

struct output_iterator {
    using difference_type = tested::ptrdiff_t;

    int* current = nullptr;

    constexpr int& operator*() const {
        return *current;
    }

    constexpr output_iterator& operator++() {
        ++current;
        return *this;
    }

    constexpr void operator++(int) {
        ++current;
    }
};

struct output_sentinel {
    int* end = nullptr;

    friend constexpr bool operator==(
        output_iterator current,
        output_sentinel bound
    ) {
        return current.current == bound.end;
    }
};

struct output_view
    : tested::ranges::view_base
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr output_view() = default;

    constexpr output_view(
        int* first,
        int* last
    )
        : first(first),
          last(last) {}

    constexpr output_iterator begin() {
        return {first};
    }

    constexpr output_sentinel end() {
        return {last};
    }
};

static_assert(
    tested::ranges::input_range<
        input_view
    >
);

static_assert(
    !tested::ranges::forward_range<
        input_view
    >
);

static_assert(
    tested::ranges::range<
        output_view
    >
);

static_assert(
    tested::ranges::view<
        output_view
    >
);

static_assert(
    !tested::ranges::input_range<
        output_view
    >
);

/*
 * Miscellaneous invalid argument types.
 */

struct non_range {
};

struct bad_count {
};

struct unrelated {
};

struct immobile {
    immobile() = default;
    immobile(const immobile&) = delete;
    immobile(immobile&&) = delete;
    immobile& operator=(const immobile&) = delete;
    immobile& operator=(immobile&&) = delete;
};

struct non_incrementable {
};

struct non_equality_bound {
};

/*
 * Predicates and transformations.
 */

struct unary_predicate {
    constexpr bool operator()(int) const {
        return true;
    }
};

struct void_unary_predicate {
    constexpr void operator()(int) const {
    }
};

struct wrong_unary_predicate {
    constexpr bool operator()(
        unrelated
    ) const {
        return true;
    }
};

struct binary_predicate {
    constexpr bool operator()(
        int,
        int
    ) const {
        return true;
    }
};

struct void_binary_predicate {
    constexpr void operator()(
        int,
        int
    ) const {
    }
};

struct unary_transform {
    constexpr int operator()(int value) const {
        return value;
    }
};

struct void_unary_transform {
    constexpr void operator()(int) const {
    }
};

struct wrong_unary_transform {
    constexpr int operator()(
        unrelated
    ) const {
        return 0;
    }
};

struct nullary_transform {
    constexpr int operator()() const {
        return 0;
    }
};

struct void_nullary_transform {
    constexpr void operator()() const {
    }
};

struct binary_transform {
    constexpr int operator()(
        int left,
        int right
    ) const {
        return left + right;
    }
};

struct void_binary_transform {
    constexpr void operator()(
        int,
        int
    ) const {
    }
};

/*
 * Common range types.
 */

using int_range =
    int (&)[4];

using pattern_range =
    int (&)[2];

using nested_int_range =
    int (&)[2][2];

using tuple_range =
    tested::tuple<int, int> (&)[2];

/*
 * Core view adaptors.
 */

using all_adaptor =
    decltype(
        tested::ranges::views::all
    );

static_assert(
    can_invoke_adaptor<
        all_adaptor,
        int_range
    >
);

static_assert(
    !can_invoke_adaptor<
        all_adaptor,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        all_adaptor
    >
);

static_assert(
    !can_invoke_adaptor<
        all_adaptor,
        int_range,
        int
    >
);

using single_adaptor =
    decltype(
        tested::ranges::views::single
    );

static_assert(
    can_invoke_adaptor<
        single_adaptor,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        single_adaptor,
        immobile
    >
);

static_assert(
    !can_invoke_adaptor<
        single_adaptor
    >
);

static_assert(
    !can_invoke_adaptor<
        single_adaptor,
        int,
        int
    >
);

/*
 * iota, repeat, and counted.
 */

using iota_adaptor =
    decltype(
        tested::ranges::views::iota
    );

static_assert(
    can_invoke_adaptor<
        iota_adaptor,
        int
    >
);

static_assert(
    can_invoke_adaptor<
        iota_adaptor,
        int,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        iota_adaptor,
        non_incrementable
    >
);

static_assert(
    !can_invoke_adaptor<
        iota_adaptor,
        int,
        non_equality_bound
    >
);

using repeat_adaptor =
    decltype(
        tested::ranges::views::repeat
    );

static_assert(
    can_invoke_adaptor<
        repeat_adaptor,
        int
    >
);

static_assert(
    can_invoke_adaptor<
        repeat_adaptor,
        int,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        repeat_adaptor,
        immobile
    >
);

static_assert(
    !can_invoke_adaptor<
        repeat_adaptor,
        int,
        double
    >
);

using counted_adaptor =
    decltype(
        tested::ranges::views::counted
    );

static_assert(
    can_invoke_adaptor<
        counted_adaptor,
        int*,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        counted_adaptor,
        int,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        counted_adaptor,
        int*,
        bad_count
    >
);

/*
 * reverse.
 */

using reverse_adaptor =
    decltype(
        tested::ranges::views::reverse
    );

static_assert(
    can_invoke_adaptor<
        reverse_adaptor,
        int_range
    >
);

static_assert(
    !can_invoke_adaptor<
        reverse_adaptor,
        input_view
    >
);

static_assert(
    !can_invoke_adaptor<
        reverse_adaptor,
        output_view
    >
);

static_assert(
    !can_invoke_adaptor<
        reverse_adaptor,
        non_range
    >
);

static_assert(
    can_pipe<
        int_range,
        reverse_adaptor
    >
);

static_assert(
    !can_pipe<
        input_view,
        reverse_adaptor
    >
);

/*
 * take and drop.
 */

using take_adaptor =
    decltype(
        tested::ranges::views::take
    );

using drop_adaptor =
    decltype(
        tested::ranges::views::drop
    );

static_assert(
    can_invoke_adaptor<
        take_adaptor,
        int_range,
        int
    >
);

static_assert(
    can_invoke_adaptor<
        take_adaptor,
        input_view,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        take_adaptor,
        int_range,
        bad_count
    >
);

static_assert(
    !can_invoke_adaptor<
        take_adaptor,
        non_range,
        int
    >
);

static_assert(
    can_invoke_adaptor<
        drop_adaptor,
        int_range,
        int
    >
);

static_assert(
    can_invoke_adaptor<
        drop_adaptor,
        input_view,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        drop_adaptor,
        int_range,
        bad_count
    >
);

static_assert(
    !can_invoke_adaptor<
        drop_adaptor,
        non_range,
        int
    >
);

using take_two =
    decltype(
        tested::ranges::views::take(2)
    );

using take_bad =
    decltype(
        tested::ranges::views::take(
            bad_count{}
        )
    );

using drop_two =
    decltype(
        tested::ranges::views::drop(2)
    );

using drop_bad =
    decltype(
        tested::ranges::views::drop(
            bad_count{}
        )
    );

static_assert(
    can_pipe<
        int_range,
        take_two
    >
);

static_assert(
    can_pipe<
        input_view,
        take_two
    >
);

static_assert(
    !can_pipe<
        int_range,
        take_bad
    >
);

static_assert(
    can_pipe<
        int_range,
        drop_two
    >
);

static_assert(
    can_pipe<
        input_view,
        drop_two
    >
);

static_assert(
    !can_pipe<
        int_range,
        drop_bad
    >
);

/*
 * Predicate adaptors.
 */

using take_while_adaptor =
    decltype(
        tested::ranges::views::take_while
    );

using drop_while_adaptor =
    decltype(
        tested::ranges::views::drop_while
    );

using filter_adaptor =
    decltype(
        tested::ranges::views::filter
    );

static_assert(
    can_invoke_adaptor<
        take_while_adaptor,
        int_range,
        unary_predicate
    >
);

static_assert(
    can_invoke_adaptor<
        take_while_adaptor,
        input_view,
        unary_predicate
    >
);

static_assert(
    !can_invoke_adaptor<
        take_while_adaptor,
        output_view,
        unary_predicate
    >
);

static_assert(
    !can_invoke_adaptor<
        take_while_adaptor,
        int_range,
        void_unary_predicate
    >
);

static_assert(
    !can_invoke_adaptor<
        take_while_adaptor,
        int_range,
        wrong_unary_predicate
    >
);

static_assert(
    can_invoke_adaptor<
        drop_while_adaptor,
        int_range,
        unary_predicate
    >
);

static_assert(
    can_invoke_adaptor<
        drop_while_adaptor,
        input_view,
        unary_predicate
    >
);

static_assert(
    !can_invoke_adaptor<
        drop_while_adaptor,
        output_view,
        unary_predicate
    >
);

static_assert(
    !can_invoke_adaptor<
        drop_while_adaptor,
        int_range,
        void_unary_predicate
    >
);

static_assert(
    !can_invoke_adaptor<
        drop_while_adaptor,
        int_range,
        wrong_unary_predicate
    >
);

static_assert(
    can_invoke_adaptor<
        filter_adaptor,
        int_range,
        unary_predicate
    >
);

static_assert(
    can_invoke_adaptor<
        filter_adaptor,
        input_view,
        unary_predicate
    >
);

static_assert(
    !can_invoke_adaptor<
        filter_adaptor,
        output_view,
        unary_predicate
    >
);

static_assert(
    !can_invoke_adaptor<
        filter_adaptor,
        int_range,
        void_unary_predicate
    >
);

static_assert(
    !can_invoke_adaptor<
        filter_adaptor,
        int_range,
        wrong_unary_predicate
    >
);

using take_while_good =
    decltype(
        tested::ranges::views::
            take_while(
                unary_predicate{}
            )
    );

using take_while_bad =
    decltype(
        tested::ranges::views::
            take_while(
                wrong_unary_predicate{}
            )
    );

using drop_while_good =
    decltype(
        tested::ranges::views::
            drop_while(
                unary_predicate{}
            )
    );

using drop_while_bad =
    decltype(
        tested::ranges::views::
            drop_while(
                wrong_unary_predicate{}
            )
    );

using filter_good =
    decltype(
        tested::ranges::views::
            filter(
                unary_predicate{}
            )
    );

using filter_bad =
    decltype(
        tested::ranges::views::
            filter(
                wrong_unary_predicate{}
            )
    );

static_assert(
    can_pipe<
        int_range,
        take_while_good
    >
);

static_assert(
    !can_pipe<
        int_range,
        take_while_bad
    >
);

static_assert(
    can_pipe<
        int_range,
        drop_while_good
    >
);

static_assert(
    !can_pipe<
        int_range,
        drop_while_bad
    >
);

static_assert(
    can_pipe<
        int_range,
        filter_good
    >
);

static_assert(
    !can_pipe<
        int_range,
        filter_bad
    >
);

/*
 * transform.
 */

using transform_adaptor =
    decltype(
        tested::ranges::views::transform
    );

static_assert(
    can_invoke_adaptor<
        transform_adaptor,
        int_range,
        unary_transform
    >
);

static_assert(
    can_invoke_adaptor<
        transform_adaptor,
        input_view,
        unary_transform
    >
);

static_assert(
    !can_invoke_adaptor<
        transform_adaptor,
        output_view,
        unary_transform
    >
);

static_assert(
    !can_invoke_adaptor<
        transform_adaptor,
        int_range,
        void_unary_transform
    >
);

static_assert(
    !can_invoke_adaptor<
        transform_adaptor,
        int_range,
        wrong_unary_transform
    >
);

using transform_good =
    decltype(
        tested::ranges::views::
            transform(
                unary_transform{}
            )
    );

using transform_void =
    decltype(
        tested::ranges::views::
            transform(
                void_unary_transform{}
            )
    );

using transform_wrong =
    decltype(
        tested::ranges::views::
            transform(
                wrong_unary_transform{}
            )
    );

static_assert(
    can_pipe<
        int_range,
        transform_good
    >
);

static_assert(
    !can_pipe<
        int_range,
        transform_void
    >
);

static_assert(
    !can_pipe<
        int_range,
        transform_wrong
    >
);

/*
 * common.
 */

using common_adaptor =
    decltype(
        tested::ranges::views::common
    );

static_assert(
    can_invoke_adaptor<
        common_adaptor,
        int_range
    >
);

static_assert(
    can_invoke_adaptor<
        common_adaptor,
        input_view
    >
);

static_assert(
    !can_invoke_adaptor<
        common_adaptor,
        non_range
    >
);

static_assert(
    can_pipe<
        input_view,
        common_adaptor
    >
);

/*
 * elements, keys, and values.
 */

using elements_zero_adaptor =
    decltype(
        tested::ranges::views::
            elements<0>
    );

using elements_one_adaptor =
    decltype(
        tested::ranges::views::
            elements<1>
    );

using elements_two_adaptor =
    decltype(
        tested::ranges::views::
            elements<2>
    );

using keys_adaptor =
    decltype(
        tested::ranges::views::keys
    );

using values_adaptor =
    decltype(
        tested::ranges::views::values
    );

static_assert(
    can_invoke_adaptor<
        elements_zero_adaptor,
        tuple_range
    >
);

static_assert(
    can_invoke_adaptor<
        elements_one_adaptor,
        tuple_range
    >
);

static_assert(
    !can_invoke_adaptor<
        elements_two_adaptor,
        tuple_range
    >
);

static_assert(
    !can_invoke_adaptor<
        elements_zero_adaptor,
        int_range
    >
);

static_assert(
    can_invoke_adaptor<
        keys_adaptor,
        tuple_range
    >
);

static_assert(
    can_invoke_adaptor<
        values_adaptor,
        tuple_range
    >
);

static_assert(
    can_pipe<
        tuple_range,
        elements_zero_adaptor
    >
);

/*
 * join.
 */

using join_adaptor =
    decltype(
        tested::ranges::views::join
    );

static_assert(
    can_invoke_adaptor<
        join_adaptor,
        nested_int_range
    >
);

static_assert(
    !can_invoke_adaptor<
        join_adaptor,
        int_range
    >
);

static_assert(
    !can_invoke_adaptor<
        join_adaptor,
        output_view
    >
);

static_assert(
    !can_invoke_adaptor<
        join_adaptor,
        non_range
    >
);

static_assert(
    can_pipe<
        nested_int_range,
        join_adaptor
    >
);

/*
 * split.
 */

using split_adaptor =
    decltype(
        tested::ranges::views::split
    );

static_assert(
    can_invoke_adaptor<
        split_adaptor,
        int_range,
        int
    >
);

static_assert(
    can_invoke_adaptor<
        split_adaptor,
        int_range,
        pattern_range
    >
);

static_assert(
    !can_invoke_adaptor<
        split_adaptor,
        input_view,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        split_adaptor,
        output_view,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        split_adaptor,
        int_range,
        input_view
    >
);

static_assert(
    !can_invoke_adaptor<
        split_adaptor,
        int_range,
        unrelated
    >
);

using split_zero =
    decltype(
        tested::ranges::views::split(0)
    );

using split_wrong =
    decltype(
        tested::ranges::views::split(
            unrelated{}
        )
    );

static_assert(
    can_pipe<
        int_range,
        split_zero
    >
);

static_assert(
    !can_pipe<
        input_view,
        split_zero
    >
);

static_assert(
    !can_pipe<
        int_range,
        split_wrong
    >
);

/*
 * lazy_split.
 */

using lazy_split_adaptor =
    decltype(
        tested::ranges::views::lazy_split
    );

static_assert(
    can_invoke_adaptor<
        lazy_split_adaptor,
        input_view,
        int
    >
);

static_assert(
    can_invoke_adaptor<
        lazy_split_adaptor,
        input_view,
        tested::ranges::empty_view<int>
    >
);

static_assert(
    can_invoke_adaptor<
        lazy_split_adaptor,
        int_range,
        pattern_range
    >
);

static_assert(
    !can_invoke_adaptor<
        lazy_split_adaptor,
        input_view,
        pattern_range
    >
);

static_assert(
    !can_invoke_adaptor<
        lazy_split_adaptor,
        output_view,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        lazy_split_adaptor,
        int_range,
        unrelated
    >
);

using lazy_split_zero =
    decltype(
        tested::ranges::views::
            lazy_split(0)
    );

using lazy_split_wrong =
    decltype(
        tested::ranges::views::
            lazy_split(
                unrelated{}
            )
    );

static_assert(
    can_pipe<
        int_range,
        lazy_split_zero
    >
);

static_assert(
    can_pipe<
        input_view,
        lazy_split_zero
    >
);

static_assert(
    !can_pipe<
        int_range,
        lazy_split_wrong
    >
);

/*
 * as_const.
 */

using as_const_adaptor =
    decltype(
        tested::ranges::views::as_const
    );

static_assert(
    can_invoke_adaptor<
        as_const_adaptor,
        int_range
    >
);

static_assert(
    !can_invoke_adaptor<
        as_const_adaptor,
        output_view
    >
);

static_assert(
    !can_invoke_adaptor<
        as_const_adaptor,
        non_range
    >
);

static_assert(
    can_pipe<
        int_range,
        as_const_adaptor
    >
);

/*
 * stride.
 */

using stride_adaptor =
    decltype(
        tested::ranges::views::stride
    );

static_assert(
    can_invoke_adaptor<
        stride_adaptor,
        int_range,
        int
    >
);

static_assert(
    can_invoke_adaptor<
        stride_adaptor,
        input_view,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        stride_adaptor,
        output_view,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        stride_adaptor,
        int_range,
        bad_count
    >
);

static_assert(
    !can_invoke_adaptor<
        stride_adaptor,
        non_range,
        int
    >
);

using stride_two =
    decltype(
        tested::ranges::views::stride(2)
    );

using stride_bad =
    decltype(
        tested::ranges::views::stride(
            bad_count{}
        )
    );

static_assert(
    can_pipe<
        int_range,
        stride_two
    >
);

static_assert(
    can_pipe<
        input_view,
        stride_two
    >
);

static_assert(
    !can_pipe<
        int_range,
        stride_bad
    >
);

/*
 * zip.
 */

using zip_adaptor =
    decltype(
        tested::ranges::views::zip
    );

static_assert(
    can_invoke_adaptor<
        zip_adaptor
    >
);

static_assert(
    can_invoke_adaptor<
        zip_adaptor,
        int_range
    >
);

static_assert(
    can_invoke_adaptor<
        zip_adaptor,
        int_range,
        int_range
    >
);

static_assert(
    can_invoke_adaptor<
        zip_adaptor,
        input_view,
        int_range
    >
);

static_assert(
    !can_invoke_adaptor<
        zip_adaptor,
        output_view
    >
);

static_assert(
    !can_invoke_adaptor<
        zip_adaptor,
        int_range,
        non_range
    >
);

/*
 * zip_transform.
 */

using zip_transform_adaptor =
    decltype(
        tested::ranges::views::
            zip_transform
    );

static_assert(
    can_invoke_adaptor<
        zip_transform_adaptor,
        nullary_transform
    >
);

static_assert(
    can_invoke_adaptor<
        zip_transform_adaptor,
        unary_transform,
        int_range
    >
);

static_assert(
    can_invoke_adaptor<
        zip_transform_adaptor,
        binary_transform,
        int_range,
        int_range
    >
);

static_assert(
    !can_invoke_adaptor<
        zip_transform_adaptor
    >
);

static_assert(
    !can_invoke_adaptor<
        zip_transform_adaptor,
        void_nullary_transform
    >
);

static_assert(
    !can_invoke_adaptor<
        zip_transform_adaptor,
        void_unary_transform,
        int_range
    >
);

static_assert(
    !can_invoke_adaptor<
        zip_transform_adaptor,
        unary_transform,
        int_range,
        int_range
    >
);

static_assert(
    !can_invoke_adaptor<
        zip_transform_adaptor,
        binary_transform,
        int_range
    >
);

static_assert(
    !can_invoke_adaptor<
        zip_transform_adaptor,
        unary_transform,
        output_view
    >
);

static_assert(
    !can_invoke_adaptor<
        zip_transform_adaptor,
        unary_transform,
        non_range
    >
);

/*
 * enumerate.
 */

using enumerate_adaptor =
    decltype(
        tested::ranges::views::enumerate
    );

static_assert(
    can_invoke_adaptor<
        enumerate_adaptor,
        int_range
    >
);

static_assert(
    can_invoke_adaptor<
        enumerate_adaptor,
        input_view
    >
);

static_assert(
    !can_invoke_adaptor<
        enumerate_adaptor,
        output_view
    >
);

static_assert(
    !can_invoke_adaptor<
        enumerate_adaptor,
        non_range
    >
);

static_assert(
    can_pipe<
        int_range,
        enumerate_adaptor
    >
);

/*
 * adjacent.
 */

using adjacent_zero_adaptor =
    decltype(
        tested::ranges::views::
            adjacent<0>
    );

using adjacent_two_adaptor =
    decltype(
        tested::ranges::views::
            adjacent<2>
    );

static_assert(
    can_invoke_adaptor<
        adjacent_two_adaptor,
        int_range
    >
);

static_assert(
    !can_invoke_adaptor<
        adjacent_zero_adaptor,
        int_range
    >
);

static_assert(
    !can_invoke_adaptor<
        adjacent_two_adaptor,
        input_view
    >
);

static_assert(
    !can_invoke_adaptor<
        adjacent_two_adaptor,
        output_view
    >
);

static_assert(
    !can_invoke_adaptor<
        adjacent_two_adaptor,
        non_range
    >
);

static_assert(
    can_pipe<
        int_range,
        adjacent_two_adaptor
    >
);

/*
 * adjacent_transform.
 */

using adjacent_transform_zero_adaptor =
    decltype(
        tested::ranges::views::
            adjacent_transform<0>
    );

using adjacent_transform_two_adaptor =
    decltype(
        tested::ranges::views::
            adjacent_transform<2>
    );

static_assert(
    can_invoke_adaptor<
        adjacent_transform_two_adaptor,
        int_range,
        binary_transform
    >
);

static_assert(
    !can_invoke_adaptor<
        adjacent_transform_zero_adaptor,
        int_range,
        nullary_transform
    >
);

static_assert(
    !can_invoke_adaptor<
        adjacent_transform_two_adaptor,
        int_range,
        unary_transform
    >
);

static_assert(
    !can_invoke_adaptor<
        adjacent_transform_two_adaptor,
        int_range,
        void_binary_transform
    >
);

static_assert(
    !can_invoke_adaptor<
        adjacent_transform_two_adaptor,
        input_view,
        binary_transform
    >
);

static_assert(
    !can_invoke_adaptor<
        adjacent_transform_two_adaptor,
        output_view,
        binary_transform
    >
);

using adjacent_transform_sum =
    decltype(
        tested::ranges::views::
            adjacent_transform<2>(
                binary_transform{}
            )
    );

using adjacent_transform_wrong =
    decltype(
        tested::ranges::views::
            adjacent_transform<2>(
                unary_transform{}
            )
    );

static_assert(
    can_pipe<
        int_range,
        adjacent_transform_sum
    >
);

static_assert(
    !can_pipe<
        int_range,
        adjacent_transform_wrong
    >
);

/*
 * slide.
 */

using slide_adaptor =
    decltype(
        tested::ranges::views::slide
    );

static_assert(
    can_invoke_adaptor<
        slide_adaptor,
        int_range,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        slide_adaptor,
        input_view,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        slide_adaptor,
        output_view,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        slide_adaptor,
        int_range,
        bad_count
    >
);

static_assert(
    !can_invoke_adaptor<
        slide_adaptor,
        non_range,
        int
    >
);

using slide_two =
    decltype(
        tested::ranges::views::slide(2)
    );

using slide_bad =
    decltype(
        tested::ranges::views::slide(
            bad_count{}
        )
    );

static_assert(
    can_pipe<
        int_range,
        slide_two
    >
);

static_assert(
    !can_pipe<
        input_view,
        slide_two
    >
);

static_assert(
    !can_pipe<
        int_range,
        slide_bad
    >
);

/*
 * chunk_by.
 */

using chunk_by_adaptor =
    decltype(
        tested::ranges::views::chunk_by
    );

static_assert(
    can_invoke_adaptor<
        chunk_by_adaptor,
        int_range,
        binary_predicate
    >
);

static_assert(
    !can_invoke_adaptor<
        chunk_by_adaptor,
        input_view,
        binary_predicate
    >
);

static_assert(
    !can_invoke_adaptor<
        chunk_by_adaptor,
        output_view,
        binary_predicate
    >
);

static_assert(
    !can_invoke_adaptor<
        chunk_by_adaptor,
        int_range,
        unary_predicate
    >
);

static_assert(
    !can_invoke_adaptor<
        chunk_by_adaptor,
        int_range,
        void_binary_predicate
    >
);

static_assert(
    !can_invoke_adaptor<
        chunk_by_adaptor,
        non_range,
        binary_predicate
    >
);

using chunk_by_good =
    decltype(
        tested::ranges::views::
            chunk_by(
                binary_predicate{}
            )
    );

using chunk_by_bad =
    decltype(
        tested::ranges::views::
            chunk_by(
                unary_predicate{}
            )
    );

static_assert(
    can_pipe<
        int_range,
        chunk_by_good
    >
);

static_assert(
    !can_pipe<
        int_range,
        chunk_by_bad
    >
);

/*
 * chunk.
 */

using chunk_adaptor =
    decltype(
        tested::ranges::views::chunk
    );

static_assert(
    can_invoke_adaptor<
        chunk_adaptor,
        int_range,
        int
    >
);

static_assert(
    can_invoke_adaptor<
        chunk_adaptor,
        input_view,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        chunk_adaptor,
        output_view,
        int
    >
);

static_assert(
    !can_invoke_adaptor<
        chunk_adaptor,
        int_range,
        bad_count
    >
);

static_assert(
    !can_invoke_adaptor<
        chunk_adaptor,
        non_range,
        int
    >
);

using chunk_two =
    decltype(
        tested::ranges::views::chunk(2)
    );

using chunk_bad =
    decltype(
        tested::ranges::views::chunk(
            bad_count{}
        )
    );

static_assert(
    can_pipe<
        int_range,
        chunk_two
    >
);

static_assert(
    can_pipe<
        input_view,
        chunk_two
    >
);

static_assert(
    !can_pipe<
        int_range,
        chunk_bad
    >
);

/*
 * cartesian_product.
 *
 * The first range need only be input; all subsequent ranges
 * must be forward ranges.
 */

using cartesian_product_adaptor =
    decltype(
        tested::ranges::views::
            cartesian_product
    );

static_assert(
    can_invoke_adaptor<
        cartesian_product_adaptor
    >
);

static_assert(
    can_invoke_adaptor<
        cartesian_product_adaptor,
        int_range
    >
);

static_assert(
    can_invoke_adaptor<
        cartesian_product_adaptor,
        int_range,
        int_range
    >
);

static_assert(
    can_invoke_adaptor<
        cartesian_product_adaptor,
        input_view,
        int_range
    >
);

static_assert(
    !can_invoke_adaptor<
        cartesian_product_adaptor,
        int_range,
        input_view
    >
);

static_assert(
    !can_invoke_adaptor<
        cartesian_product_adaptor,
        output_view
    >
);

static_assert(
    !can_invoke_adaptor<
        cartesian_product_adaptor,
        int_range,
        output_view
    >
);

static_assert(
    !can_invoke_adaptor<
        cartesian_product_adaptor,
        non_range
    >
);

bool ftl_test() {
    return true;
}
