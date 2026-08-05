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

struct sum_value {
    constexpr int operator()(
        int left,
        int right
    ) const {
        return left + right;
    }
};

struct first_reference {
    constexpr int& operator()(
        int& left,
        int&
    ) const {
        return left;
    }
};

struct mutable_sum {
    constexpr int operator()(
        int& left,
        int& right
    ) {
        return left + right;
    }
};

struct move_only_sum {
    constexpr move_only_sum() = default;

    move_only_sum(
        const move_only_sum&
    ) = delete;

    constexpr move_only_sum(
        move_only_sum&&
    ) = default;

    constexpr int operator()(
        int left,
        int right
    ) const {
        return left + right;
    }
};

struct move_only_zero {
    constexpr move_only_zero() = default;

    move_only_zero(
        const move_only_zero&
    ) = delete;

    constexpr move_only_zero(
        move_only_zero&&
    ) = default;

    constexpr int operator()() {
        return 42;
    }
};

struct counted_zero {
    int* calls = nullptr;

    constexpr int operator()() {
        ++*calls;
        return 42;
    }
};

struct void_zero {
    constexpr void operator()() const {
    }
};

struct void_binary {
    constexpr void operator()(
        int&,
        int&
    ) const {
    }
};

struct forward_iterator {
    using iterator_concept =
        tested::forward_iterator_tag;

    using iterator_category =
        tested::forward_iterator_tag;

    using value_type = int;

    using difference_type =
        tested::ptrdiff_t;

    int* current = nullptr;

    constexpr int& operator*() const {
        return *current;
    }

    constexpr forward_iterator&
    operator++() {
        ++current;
        return *this;
    }

    constexpr forward_iterator
    operator++(int) {
        auto previous = *this;
        ++*this;
        return previous;
    }

    friend constexpr bool operator==(
        const forward_iterator&,
        const forward_iterator&
    ) = default;
};

struct forward_sentinel {
    int* last = nullptr;

    friend constexpr bool operator==(
        forward_iterator iterator,
        forward_sentinel sentinel
    ) {
        return
            iterator.current ==
            sentinel.last;
    }

    friend constexpr tested::ptrdiff_t
    operator-(
        forward_sentinel sentinel,
        forward_iterator iterator
    ) {
        return
            sentinel.last -
            iterator.current;
    }

    friend constexpr tested::ptrdiff_t
    operator-(
        forward_iterator iterator,
        forward_sentinel sentinel
    ) {
        return
            iterator.current -
            sentinel.last;
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

    constexpr forward_iterator begin() {
        return forward_iterator{
            first
        };
    }

    constexpr forward_sentinel end() {
        return forward_sentinel{
            last
        };
    }

    constexpr tested::size_t size() const {
        return static_cast<
            tested::size_t
        >(last - first);
    }
};

template<class F>
concept can_zero_zip_transform =
    requires(F&& function) {
        tested::ranges::views::
            zip_transform(
                static_cast<F&&>(
                    function
                )
            );
    };

template<
    class F,
    class... Ranges
>
concept can_zip_transform =
    requires(
        F&& function,
        Ranges&&... ranges
    ) {
        tested::ranges::views::
            zip_transform(
                static_cast<F&&>(
                    function
                ),
                static_cast<Ranges&&>(
                    ranges
                )...
            );
    };

using integer_array = int[2];

using value_view =
    decltype(
        tested::ranges::views::
            zip_transform(
                sum_value{},
                tested::declval<
                    integer_array&
                >(),
                tested::declval<
                    integer_array&
                >()
            )
    );

using reference_view =
    decltype(
        tested::ranges::views::
            zip_transform(
                first_reference{},
                tested::declval<
                    integer_array&
                >(),
                tested::declval<
                    integer_array&
                >()
            )
    );

using mutable_only_view =
    decltype(
        tested::ranges::views::
            zip_transform(
                mutable_sum{},
                tested::declval<
                    integer_array&
                >(),
                tested::declval<
                    integer_array&
                >()
            )
    );

using zero_result =
    decltype(
        tested::ranges::views::
            zip_transform(
                tested::declval<
                    move_only_zero&
                >()
            )
    );

static_assert(
    tested::ranges::view<
        value_view
    >
);

static_assert(
    tested::ranges::random_access_range<
        value_view
    >
);

static_assert(
    tested::ranges::common_range<
        value_view
    >
);

static_assert(
    tested::ranges::sized_range<
        value_view
    >
);

static_assert(tested::same_as<
    tested::ranges::range_reference_t<
        value_view
    >,
    int
>);

static_assert(tested::same_as<
    tested::ranges::range_value_t<
        value_view
    >,
    int
>);

static_assert(tested::same_as<
    typename tested::iterator_traits<
        tested::ranges::iterator_t<
            value_view
        >
    >::iterator_category,
    tested::input_iterator_tag
>);

static_assert(tested::same_as<
    tested::ranges::range_reference_t<
        reference_view
    >,
    int&
>);

static_assert(tested::same_as<
    typename tested::iterator_traits<
        tested::ranges::iterator_t<
            reference_view
        >
    >::iterator_category,
    tested::random_access_iterator_tag
>);

static_assert(
    tested::ranges::range<
        mutable_only_view
    >
);

static_assert(
    !tested::ranges::range<
        const mutable_only_view
    >
);

static_assert(tested::same_as<
    zero_result,
    tested::ranges::empty_view<int>
>);

static_assert(
    can_zero_zip_transform<
        move_only_zero&
    >
);

static_assert(
    !can_zero_zip_transform<
        void_zero
    >
);

static_assert(
    can_zip_transform<
        move_only_sum,
        integer_array&,
        integer_array&
    >
);

static_assert(
    !can_zip_transform<
        move_only_sum&,
        integer_array&,
        integer_array&
    >
);

static_assert(
    !can_zip_transform<
        void_binary,
        integer_array&,
        integer_array&
    >
);

constexpr bool
zip_transform_works() {
    int left[] = {
        1,
        2,
        3
    };

    int right[] = {
        10,
        20
    };

    auto view =
        tested::ranges::views::
            zip_transform(
                sum_value{},
                left,
                right
            );

    if (view.size() != 2)
        return false;

    if (
        view.end() -
        view.begin() != 2
    )
        return false;

    if (
        view[0] != 11 ||
        view[1] != 22
    )
        return false;

    const auto& constant_view =
        view;

    if (constant_view[1] != 22)
        return false;

    auto references =
        tested::ranges::views::
            zip_transform(
                first_reference{},
                left,
                right
            );

    references[0] = 9;

    if (left[0] != 9)
        return false;

    auto moved_callable =
        tested::ranges::views::
            zip_transform(
                move_only_sum{},
                left,
                right
            );

    if (
        moved_callable[0] != 19 ||
        moved_callable[1] != 22
    )
        return false;

    int calls = 0;

    auto empty =
        tested::ranges::views::
            zip_transform(
                counted_zero{
                    &calls
                }
            );

    if (
        calls != 0 ||
        !empty.empty()
    )
        return false;

    move_only_zero zero_function;

    auto move_only_empty =
        tested::ranges::views::
            zip_transform(
                zero_function
            );

    if (!move_only_empty.empty())
        return false;

    auto direct =
        tested::ranges::
            zip_transform_view(
                sum_value{},
                left,
                right
            );

    if (
        direct.size() != 2 ||
        direct[0] != 19
    )
        return false;

    int short_values[] = {
        2,
        4
    };

    int long_values[] = {
        10,
        20,
        30
    };

    forward_view short_range{
        short_values,
        short_values + 2
    };

    forward_view long_range{
        long_values,
        long_values + 3
    };

    auto non_common =
        tested::ranges::views::
            zip_transform(
                sum_value{},
                short_range,
                long_range
            );

    static_assert(
        !tested::ranges::common_range<
            decltype(non_common)
        >
    );

    static_assert(
        tested::ranges::sized_range<
            decltype(non_common)
        >
    );

    if (
        non_common.end() -
        non_common.begin() != 2
    )
        return false;

    auto iterator =
        non_common.begin();

    if (*iterator != 12)
        return false;

    ++iterator;

    if (*iterator != 24)
        return false;

    ++iterator;

    if (iterator != non_common.end())
        return false;

    return true;
}

static_assert(
    zip_transform_works()
);

bool ftl_test() {
    return zip_transform_works();
}