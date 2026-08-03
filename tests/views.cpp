#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <iterator>
#include <memory>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/iterator>
#include <ftl/memory>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif


// A simple common, sized, random-access view.

struct pointer_view
        : tested::ranges::view_base
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr pointer_view() noexcept = default;

    constexpr pointer_view(
        int* begin_value,
        int* end_value
    ) noexcept
        : first(begin_value),
          last(end_value) {}

    constexpr int* begin() noexcept
    {
        return first;
    }

    constexpr const int* begin() const noexcept
    {
        return first;
    }

    constexpr int* end() noexcept
    {
        return last;
    }

    constexpr const int* end() const noexcept
    {
        return last;
    }

    constexpr tested::size_t size() const noexcept
    {
        return static_cast<tested::size_t>(
            last - first
        );
    }
};


// pointer_view's iterators remain valid independently of the view object.

#ifdef FTL_REPLACE_STL

namespace std::ranges
{
    template<>
    inline constexpr bool
    enable_borrowed_range<::pointer_view> = true;
} // namespace std::ranges

#else

namespace ftl::ranges
{
    template<>
    inline constexpr bool
    enable_borrowed_range<::pointer_view> = true;
} // namespace ftl::ranges

#endif


// A sentinel used to test reverse_view's non-common cached begin path.
//
// comparisons points to a local counter so the test can verify that the
// second reverse_view::begin() call reuses the cached ending iterator.

struct counting_sentinel
{
    int* value = nullptr;
    int* comparisons = nullptr;

    constexpr counting_sentinel() noexcept = default;

    constexpr counting_sentinel(
        int* pointer,
        int* comparison_counter
    ) noexcept
        : value(pointer),
          comparisons(comparison_counter) {}

    friend constexpr bool operator==(
        int* iterator,
        counting_sentinel sentinel
    ) noexcept
    {
        if (sentinel.comparisons != nullptr)
            ++*sentinel.comparisons;

        return iterator == sentinel.value;
    }

    friend constexpr bool operator==(
        counting_sentinel sentinel,
        int* iterator
    ) noexcept
    {
        if (sentinel.comparisons != nullptr)
            ++*sentinel.comparisons;

        return sentinel.value == iterator;
    }

    friend constexpr bool operator!=(
        int* iterator,
        counting_sentinel sentinel
    ) noexcept
    {
        return !(iterator == sentinel);
    }

    friend constexpr bool operator!=(
        counting_sentinel sentinel,
        int* iterator
    ) noexcept
    {
        return !(sentinel == iterator);
    }
};


// A bidirectional, non-common view.
//
// begin() returns int*, while end() returns counting_sentinel.

struct non_common_view
        : tested::ranges::view_base
{
    int* first = nullptr;
    int* last = nullptr;
    int* comparisons = nullptr;

    constexpr non_common_view() noexcept = default;

    constexpr non_common_view(
        int* begin_value,
        int* end_value,
        int* comparison_counter
    ) noexcept
        : first(begin_value),
          last(end_value),
          comparisons(comparison_counter) {}

    constexpr int* begin() noexcept
    {
        return first;
    }

    constexpr counting_sentinel end() noexcept
    {
        return counting_sentinel{
            last,
            comparisons
        };
    }
};


using reverse_pointer_view =
tested::ranges::reverse_view<pointer_view>;

using reverse_non_common_view =
tested::ranges::reverse_view<non_common_view>;


static_assert(
    tested::ranges::view<pointer_view>
);

static_assert(
    tested::ranges::borrowed_range<pointer_view>
);

static_assert(
    tested::ranges::common_range<pointer_view>
);

static_assert(
    tested::ranges::sized_range<pointer_view>
);

static_assert(
    tested::ranges::random_access_range<pointer_view>
);


static_assert(
    tested::sentinel_for<
        counting_sentinel,
        int*>
);

static_assert(
    !tested::sized_sentinel_for<
        counting_sentinel,
        int*>
);

static_assert(
    tested::ranges::view<non_common_view>
);

static_assert(
    tested::ranges::bidirectional_range<
        non_common_view
    >
);

static_assert(
    !tested::ranges::common_range<
        non_common_view
    >
);


static_assert(
    tested::ranges::view<
        reverse_pointer_view
    >
);

static_assert(
    tested::ranges::bidirectional_range<
        reverse_pointer_view
    >
);

static_assert(
    tested::ranges::random_access_range<
        reverse_pointer_view
    >
);

static_assert(
    tested::ranges::common_range<
        reverse_pointer_view
    >
);

static_assert(
    tested::ranges::sized_range<
        reverse_pointer_view
    >
);

// reverse_iterator is random-access, but not contiguous.

static_assert(
    !tested::ranges::contiguous_range<
        reverse_pointer_view
    >
);

// reverse_view propagates enable_borrowed_range from its base view.

static_assert(
    tested::ranges::borrowed_range<
        reverse_pointer_view
    >
);


static_assert(
    tested::ranges::view<
        reverse_non_common_view
    >
);

static_assert(
    tested::ranges::bidirectional_range<
        reverse_non_common_view
    >
);

// reverse_view itself has matching reverse_iterator begin/end types,
// even when its base is non-common.

static_assert(
    tested::ranges::common_range<
        reverse_non_common_view
    >
);


static_assert(tested::is_same_v<
    decltype(
        tested::declval<
            const reverse_pointer_view&>().base()
    ),
    pointer_view
>);

static_assert(tested::is_same_v<
    decltype(
        tested::declval<
            reverse_pointer_view&&>().base()
    ),
    pointer_view
>);


template<class T>
concept has_const_begin =
        requires(const T& value)
        {
            value.begin();
        };

static_assert(
    has_const_begin<reverse_pointer_view>
);

// The non-common base does not provide a const range, so its reverse_view
// does not provide the constrained const begin() overload.

static_assert(
    !has_const_begin<reverse_non_common_view>
);


constexpr bool reverse_view_direct_works()
{
    int values[] = {1, 2, 3, 4};

    pointer_view base{
        values,
        values + 4
    };

    tested::ranges::reverse_view reversed(base);

    static_assert(tested::is_same_v<
        decltype(reversed),
        reverse_pointer_view
    >);

    if (reversed.empty())
        return false;

    if (reversed.size() != 4)
        return false;

    if (reversed.front() != 4)
        return false;

    if (reversed.back() != 1)
        return false;

    if (reversed[0] != 4)
        return false;

    if (reversed[1] != 3)
        return false;

    if (reversed[2] != 2)
        return false;

    if (reversed[3] != 1)
        return false;

    auto iterator = reversed.begin();

    if (iterator.base() != values + 4)
        return false;

    if (*iterator != 4)
        return false;

    ++iterator;

    if (*iterator != 3)
        return false;

    ++iterator;

    if (*iterator != 2)
        return false;

    ++iterator;

    if (*iterator != 1)
        return false;

    ++iterator;

    if (iterator != reversed.end())
        return false;

    if (reversed.end().base() != values)
        return false;

    auto copied_base =
            static_cast<const decltype(reversed)&>(
                reversed
            ).base();

    if (copied_base.begin() != values)
        return false;

    if (copied_base.end() != values + 4)
        return false;

    const auto& constant = reversed;

    if (constant.empty())
        return false;

    if (constant.size() != 4)
        return false;

    if (constant.front() != 4)
        return false;

    if (constant.back() != 1)
        return false;

    if (constant[2] != 2)
        return false;

    if (constant.begin().base() != values + 4)
        return false;

    if (constant.end().base() != values)
        return false;

    auto moved_base =
            static_cast<decltype(reversed)&&>(
                reversed
            ).base();

    if (moved_base.begin() != values)
        return false;

    if (moved_base.end() != values + 4)
        return false;

    return true;
}

static_assert(reverse_view_direct_works());


constexpr bool reverse_view_empty_works()
{
    int values[] = {1};

    pointer_view base{
        values,
        values
    };

    tested::ranges::reverse_view reversed(base);

    if (!reversed.empty())
        return false;

    if (reversed.size() != 0)
        return false;

    if (reversed.begin() != reversed.end())
        return false;

    if (reversed.begin().base() != values)
        return false;

    if (reversed.end().base() != values)
        return false;

    return true;
}

static_assert(reverse_view_empty_works());


constexpr bool reverse_adaptor_call_works()
{
    int values[] = {1, 2, 3, 4};

    pointer_view base{
        values,
        values + 4
    };

    auto reversed =
            tested::ranges::views::reverse(base);

    static_assert(tested::is_same_v<
        decltype(reversed),
        reverse_pointer_view
    >);

    if (reversed.front() != 4)
        return false;

    if (reversed.back() != 1)
        return false;

    if (reversed.size() != 4)
        return false;

    return true;
}

static_assert(reverse_adaptor_call_works());


constexpr bool reverse_adaptor_pipeline_works()
{
    int values[] = {1, 2, 3, 4};

    pointer_view base{
        values,
        values + 4
    };

    auto reversed =
            base |
            tested::ranges::views::reverse;

    static_assert(tested::is_same_v<
        decltype(reversed),
        reverse_pointer_view
    >);

    auto iterator = reversed.begin();

    if (*iterator++ != 4)
        return false;

    if (*iterator++ != 3)
        return false;

    if (*iterator++ != 2)
        return false;

    if (*iterator++ != 1)
        return false;

    if (iterator != reversed.end())
        return false;

    return true;
}

static_assert(reverse_adaptor_pipeline_works());


constexpr bool reverse_array_pipeline_works()
{
    int values[] = {1, 2, 3, 4};

    auto reversed =
            values |
            tested::ranges::views::reverse;

    if (reversed.size() != 4)
        return false;

    if (reversed.front() != 4)
        return false;

    if (reversed.back() != 1)
        return false;

    if (reversed[0] != 4)
        return false;

    if (reversed[3] != 1)
        return false;

    return true;
}

static_assert(reverse_array_pipeline_works());


constexpr bool reverse_const_base_works()
{
    int values[] = {1, 2, 3, 4};

    const pointer_view base{
        values,
        values + 4
    };

    auto reversed =
            tested::ranges::views::reverse(base);

    if (reversed.front() != 4)
        return false;

    if (reversed.back() != 1)
        return false;

    if (reversed.size() != 4)
        return false;

    return true;
}

static_assert(reverse_const_base_works());


constexpr bool reverse_non_common_range_works()
{
    int values[] = {1, 2, 3, 4};
    int comparisons = 0;

    non_common_view base{
        values,
        values + 4,
        &comparisons
    };

    auto reversed =
            tested::ranges::views::reverse(base);

    static_assert(tested::is_same_v<
        decltype(reversed),
        reverse_non_common_view
    >);

    auto iterator = reversed.begin();

    if (*iterator != 4)
        return false;

    ++iterator;

    if (*iterator != 3)
        return false;

    ++iterator;

    if (*iterator != 2)
        return false;

    ++iterator;

    if (*iterator != 1)
        return false;

    ++iterator;

    if (iterator != reversed.end())
        return false;

    return true;
}

constexpr bool reverse_non_common_cache_works()
{
    int values[] = {1, 2, 3, 4};
    int comparisons = 0;

    non_common_view base{
        values,
        values + 4,
        &comparisons
    };

    auto reversed =
            tested::ranges::views::reverse(base);

    auto first_begin = reversed.begin();

    if (*first_begin != 4)
        return false;

    const int comparisons_after_first_begin =
            comparisons;

    if (comparisons_after_first_begin == 0)
        return false;

    auto second_begin = reversed.begin();

    if (*second_begin != 4)
        return false;

    if (second_begin != first_begin)
        return false;

    // The second begin() should use reverse_view's non-propagating cache
    // instead of walking from the base begin iterator to its sentinel again.

    if (comparisons != comparisons_after_first_begin)
        return false;

    return true;
}

#if defined(FTL_REPLACE_STL) || defined(_MSC_VER)

static_assert(reverse_non_common_range_works());
static_assert(reverse_non_common_cache_works());

#endif

struct bidirectional_pointer_iterator
{
    using value_type = int;
    using difference_type = tested::ptrdiff_t;
    using iterator_concept =
    tested::bidirectional_iterator_tag;
    using iterator_category =
    tested::bidirectional_iterator_tag;

    int* value = nullptr;

    constexpr bidirectional_pointer_iterator()
        noexcept = default;

    constexpr explicit bidirectional_pointer_iterator(
        int* pointer
    ) noexcept
        : value(pointer) {}

    [[nodiscard]]
    constexpr int& operator*() const noexcept
    {
        return *value;
    }

    constexpr bidirectional_pointer_iterator&
    operator++() noexcept
    {
        ++value;
        return *this;
    }

    constexpr bidirectional_pointer_iterator
    operator++(int) noexcept
    {
        auto previous = *this;
        ++*this;
        return previous;
    }

    constexpr bidirectional_pointer_iterator&
    operator--() noexcept
    {
        --value;
        return *this;
    }

    constexpr bidirectional_pointer_iterator
    operator--(int) noexcept
    {
        auto previous = *this;
        --*this;
        return previous;
    }

    friend constexpr bool operator==(
        bidirectional_pointer_iterator left,
        bidirectional_pointer_iterator right
    ) noexcept
    {
        return left.value == right.value;
    }

    friend constexpr bool operator!=(
        bidirectional_pointer_iterator left,
        bidirectional_pointer_iterator right
    ) noexcept
    {
        return !(left == right);
    }
};

static_assert(
    tested::bidirectional_iterator<
        bidirectional_pointer_iterator
    >
);

static_assert(
    !tested::random_access_iterator<
        bidirectional_pointer_iterator
    >
);

static_assert(
    !tested::sized_sentinel_for<
        bidirectional_pointer_iterator,
        bidirectional_pointer_iterator
    >
);

constexpr bool reverse_view_normalization_works()
{
    int values[] = {1, 2, 3, 4};

    pointer_view base{
        values,
        values + 4
    };

    auto reversed =
            tested::ranges::views::reverse(base);

    auto restored_from_lvalue =
            tested::ranges::views::reverse(reversed);

    static_assert(tested::is_same_v<
        decltype(restored_from_lvalue),
        pointer_view
    >);

    if (restored_from_lvalue.begin() != values)
        return false;

    if (restored_from_lvalue.end() != values + 4)
        return false;

    auto restored_from_pipeline =
            base |
            tested::ranges::views::reverse |
            tested::ranges::views::reverse;

    static_assert(tested::is_same_v<
        decltype(restored_from_pipeline),
        pointer_view
    >);

    if (restored_from_pipeline.begin() != values)
        return false;

    if (restored_from_pipeline.end() != values + 4)
        return false;

    if (*restored_from_pipeline.begin() != 1)
        return false;

    return true;
}

static_assert(reverse_view_normalization_works());

constexpr bool sized_reverse_subrange_normalization_works()
{
    int values[] = {1, 2, 3, 4};

    using reverse_iterator_type =
            tested::reverse_iterator<int*>;

    using reversed_subrange_type =
            tested::ranges::subrange<
                reverse_iterator_type,
                reverse_iterator_type,
                tested::ranges::subrange_kind::sized
            >;

    reversed_subrange_type reversed{
        reverse_iterator_type{values + 4},
        reverse_iterator_type{values},
        4
    };

    auto restored =
            tested::ranges::views::reverse(reversed);

    using expected_type =
            tested::ranges::subrange<
                int*,
                int*,
                tested::ranges::subrange_kind::sized
            >;

    static_assert(tested::is_same_v<
        decltype(restored),
        expected_type
    >);

    if (restored.begin() != values)
        return false;

    if (restored.end() != values + 4)
        return false;

    if (restored.size() != 4)
        return false;

    if (restored.front() != 1)
        return false;

    if (restored.back() != 4)
        return false;

    return true;
}

static_assert(
    sized_reverse_subrange_normalization_works()
);

constexpr bool unsized_reverse_subrange_normalization_works()
{
    int values[] = {1, 2, 3, 4};

    using iterator_type =
            bidirectional_pointer_iterator;

    using reverse_iterator_type =
            tested::reverse_iterator<iterator_type>;

    using reversed_subrange_type =
            tested::ranges::subrange<
                reverse_iterator_type,
                reverse_iterator_type,
                tested::ranges::subrange_kind::unsized
            >;

    reversed_subrange_type reversed{
        reverse_iterator_type{
            iterator_type{values + 4}
        },
        reverse_iterator_type{
            iterator_type{values}
        }
    };

    auto restored =
            tested::ranges::views::reverse(reversed);

    using expected_type =
            tested::ranges::subrange<
                iterator_type,
                iterator_type,
                tested::ranges::subrange_kind::unsized
            >;

    static_assert(tested::is_same_v<
        decltype(restored),
        expected_type
    >);

    static_assert(
        !tested::ranges::sized_range<
            decltype(restored)
        >
    );

    if (restored.begin().value != values)
        return false;

    if (restored.end().value != values + 4)
        return false;

    auto iterator = restored.begin();

    if (*iterator++ != 1)
        return false;

    if (*iterator++ != 2)
        return false;

    if (*iterator++ != 3)
        return false;

    if (*iterator++ != 4)
        return false;

    if (iterator != restored.end())
        return false;

    return true;
}

static_assert(
    unsized_reverse_subrange_normalization_works()
);

// iota_view

struct forward_counter
{
    using difference_type = tested::ptrdiff_t;

    int value = 0;

    constexpr forward_counter() noexcept = default;

    constexpr explicit forward_counter(int initial) noexcept
        : value(initial) {}

    constexpr forward_counter& operator++() noexcept
    {
        ++value;
        return *this;
    }

    constexpr forward_counter operator++(int) noexcept
    {
        auto previous = *this;
        ++*this;
        return previous;
    }

    friend constexpr bool operator==(
        forward_counter left,
        forward_counter right
    ) noexcept
    {
        return left.value == right.value;
    }

    friend constexpr bool operator!=(
        forward_counter left,
        forward_counter right
    ) noexcept
    {
        return !(left == right);
    }
};


struct bidirectional_counter
{
    using difference_type = tested::ptrdiff_t;

    int value = 0;

    constexpr bidirectional_counter() noexcept = default;

    constexpr explicit bidirectional_counter(
        int initial
    ) noexcept
        : value(initial) {}

    constexpr bidirectional_counter&
    operator++() noexcept
    {
        ++value;
        return *this;
    }

    constexpr bidirectional_counter
    operator++(int) noexcept
    {
        auto previous = *this;
        ++*this;
        return previous;
    }

    constexpr bidirectional_counter&
    operator--() noexcept
    {
        --value;
        return *this;
    }

    constexpr bidirectional_counter
    operator--(int) noexcept
    {
        auto previous = *this;
        --*this;
        return previous;
    }

    friend constexpr bool operator==(
        bidirectional_counter left,
        bidirectional_counter right
    ) noexcept
    {
        return left.value == right.value;
    }

    friend constexpr bool operator!=(
        bidirectional_counter left,
        bidirectional_counter right
    ) noexcept
    {
        return !(left == right);
    }
};


struct integer_bound
{
    int value = 0;

    constexpr integer_bound() noexcept = default;

    constexpr explicit integer_bound(int bound) noexcept
        : value(bound) {}

    friend constexpr bool operator==(
        int current,
        integer_bound bound
    ) noexcept
    {
        return current == bound.value;
    }

    friend constexpr bool operator==(
        integer_bound bound,
        int current
    ) noexcept
    {
        return bound.value == current;
    }

    friend constexpr bool operator!=(
        int current,
        integer_bound bound
    ) noexcept
    {
        return !(current == bound);
    }

    friend constexpr bool operator!=(
        integer_bound bound,
        int current
    ) noexcept
    {
        return !(bound == current);
    }

    friend constexpr tested::iter_difference_t<int>
    operator-(
        integer_bound bound,
        int current
    ) noexcept
    {
        return static_cast<
            tested::iter_difference_t<int>>(bound.value - current);
    }

    friend constexpr tested::iter_difference_t<int>
    operator-(
        int current,
        integer_bound bound
    ) noexcept
    {
        return static_cast<
            tested::iter_difference_t<int>>(current - bound.value);
    }
};


using bounded_iota =
tested::ranges::iota_view<int, int>;

using unbounded_iota =
tested::ranges::iota_view<int>;

using different_bound_iota =
tested::ranges::iota_view<
    int,
    integer_bound
>;

using forward_iota =
tested::ranges::iota_view<
    forward_counter,
    forward_counter
>;

using bidirectional_iota =
tested::ranges::iota_view<
    bidirectional_counter,
    bidirectional_counter
>;


static_assert(
    tested::ranges::view<bounded_iota>
);

static_assert(
    tested::ranges::borrowed_range<bounded_iota>
);

static_assert(
    tested::ranges::common_range<bounded_iota>
);

static_assert(
    tested::ranges::sized_range<bounded_iota>
);

static_assert(
    tested::ranges::random_access_range<bounded_iota>
);

static_assert(
    !tested::ranges::contiguous_range<bounded_iota>
);

static_assert(tested::is_same_v<
    tested::ranges::range_value_t<bounded_iota>,
    int>);

static_assert(tested::is_same_v<
    tested::ranges::range_reference_t<bounded_iota>,
    int>);

static_assert(
    tested::signed_integral<
        tested::ranges::range_difference_t<
            bounded_iota
        >
    >
);


static_assert(
    tested::ranges::view<unbounded_iota>
);

static_assert(
    tested::ranges::borrowed_range<unbounded_iota>
);

static_assert(
    tested::ranges::random_access_range<unbounded_iota>
);

static_assert(
    !tested::ranges::common_range<unbounded_iota>
);

static_assert(
    !tested::ranges::sized_range<unbounded_iota>
);

static_assert(tested::is_same_v<
    tested::ranges::sentinel_t<unbounded_iota>,
    tested::unreachable_sentinel_t
>);


static_assert(
    tested::ranges::view<different_bound_iota>
);

static_assert(
    tested::ranges::random_access_range<
        different_bound_iota
    >
);

static_assert(
    !tested::ranges::common_range<
        different_bound_iota
    >
);

static_assert(
    !tested::ranges::sized_range<
        different_bound_iota
    >
);


static_assert(
    tested::ranges::forward_range<forward_iota>
);

static_assert(
    !tested::ranges::bidirectional_range<
        forward_iota
    >
);

static_assert(
    !tested::ranges::sized_range<forward_iota>
);


static_assert(
    tested::ranges::bidirectional_range<
        bidirectional_iota
    >
);

static_assert(
    !tested::ranges::random_access_range<
        bidirectional_iota
    >
);

static_assert(
    !tested::ranges::sized_range<
        bidirectional_iota
    >
);


template<class T>
concept can_make_single_iota =
        requires(T&& value)
        {
            tested::ranges::views::iota(
                static_cast<T&&>(value)
            );
        };

template<class T, class U>
concept can_make_double_iota =
        requires(T&& value, U&& bound)
        {
            tested::ranges::views::iota(
                static_cast<T&&>(value),
                static_cast<U&&>(bound)
            );
        };

static_assert(
    can_make_single_iota<int>
);

static_assert(
    can_make_double_iota<int, int>
);

// The deduction guide rejects mixed signedness for integer-like types.

static_assert(
    !can_make_double_iota<int, unsigned int>
);

// LWG 4096: views::iota(existing_iota_view) is not another iota factory.

static_assert(
    !can_make_single_iota<bounded_iota>
);


constexpr bool bounded_iota_works()
{
    auto view =
            tested::ranges::views::iota(-2, 3);

    static_assert(tested::is_same_v<
        decltype(view),
        bounded_iota
    >);

    if (view.empty())
        return false;

    if (view.size() != 5)
        return false;

    if (view.front() != -2)
        return false;

    if (view.back() != 2)
        return false;

    if (view[0] != -2)
        return false;

    if (view[2] != 0)
        return false;

    if (view[4] != 2)
        return false;

    auto iterator = view.begin();

    if (*iterator != -2)
        return false;

    auto previous = iterator++;

    if (*previous != -2)
        return false;

    if (*iterator != -1)
        return false;

    ++iterator;

    if (*iterator != 0)
        return false;

    --iterator;

    if (*iterator != -1)
        return false;

    iterator += 3;

    if (*iterator != 2)
        return false;

    iterator -= 2;

    if (*iterator != 0)
        return false;

    if (iterator[2] != 2)
        return false;

    auto advanced = iterator + 2;

    if (*advanced != 2)
        return false;

    auto commuted = 2 + iterator;

    if (*commuted != 2)
        return false;

    auto retreated = advanced - 3;

    if (*retreated != -1)
        return false;

    if (advanced - iterator != 2)
        return false;

    if (iterator - advanced != -2)
        return false;

    if (!(iterator < advanced))
        return false;

    if (!(advanced > iterator))
        return false;

    if (!(iterator <= advanced))
        return false;

    if (!(advanced >= iterator))
        return false;

    if (view.end() - view.begin() != 5)
        return false;

    return true;
}

static_assert(bounded_iota_works());


constexpr bool empty_iota_works()
{
    auto view =
            tested::ranges::views::iota(4, 4);

    if (!view.empty())
        return false;

    if (view.size() != 0)
        return false;

    if (view.begin() != view.end())
        return false;

    return true;
}

static_assert(empty_iota_works());


constexpr bool unbounded_iota_works()
{
    auto view =
            tested::ranges::views::iota(7);

    static_assert(tested::is_same_v<
        decltype(view),
        unbounded_iota
    >);

    if (view.empty())
        return false;

    auto iterator = view.begin();

    if (*iterator++ != 7)
        return false;

    if (*iterator++ != 8)
        return false;

    if (*iterator++ != 9)
        return false;

    iterator += 10;

    if (*iterator != 20)
        return false;

    if (iterator == view.end())
        return false;

    return true;
}

static_assert(unbounded_iota_works());


constexpr bool unsigned_iota_arithmetic_works()
{
    auto view =
            tested::ranges::views::iota(
                2u,
                8u
            );

    auto iterator = view.begin();

    iterator += 4;

    if (*iterator != 6u)
        return false;

    iterator += -2;

    if (*iterator != 4u)
        return false;

    iterator -= -3;

    if (*iterator != 7u)
        return false;

    iterator -= 5;

    if (*iterator != 2u)
        return false;

    auto last = view.end();

    if (last - iterator != 6)
        return false;

    if (iterator - last != -6)
        return false;

    return true;
}

static_assert(unsigned_iota_arithmetic_works());


constexpr bool different_bound_iota_works()
{
    auto view =
            tested::ranges::views::iota(
                3,
                integer_bound{7}
            );

    static_assert(tested::is_same_v<
        decltype(view),
        different_bound_iota
    >);

    if (view.empty())
        return false;

    auto iterator = view.begin();

    if (*iterator++ != 3)
        return false;

    if (*iterator++ != 4)
        return false;

    if (*iterator++ != 5)
        return false;

    if (*iterator++ != 6)
        return false;

    if (iterator != view.end())
        return false;

    return true;
}

static_assert(different_bound_iota_works());


constexpr bool forward_iota_works()
{
    auto view =
            tested::ranges::views::iota(
                forward_counter{1},
                forward_counter{4}
            );

    auto iterator = view.begin();

    if ((*iterator++).value != 1)
        return false;

    if ((*iterator++).value != 2)
        return false;

    if ((*iterator++).value != 3)
        return false;

    if (iterator != view.end())
        return false;

    return true;
}

static_assert(forward_iota_works());


constexpr bool bidirectional_iota_works()
{
    auto view =
            tested::ranges::views::iota(
                bidirectional_counter{1},
                bidirectional_counter{4}
            );

    auto iterator = view.end();

    --iterator;

    if ((*iterator).value != 3)
        return false;

    --iterator;

    if ((*iterator).value != 2)
        return false;

    auto previous = iterator--;

    if ((*previous).value != 2)
        return false;

    if ((*iterator).value != 1)
        return false;

    if (iterator != view.begin())
        return false;

    return true;
}

static_assert(bidirectional_iota_works());


constexpr bool iota_iterator_constructor_works()
{
    using view_type =
            tested::ranges::iota_view<int, int>;

    view_type original(2, 6);

    view_type reconstructed(
        original.begin(),
        original.end()
    );

    if (reconstructed.size() != 4)
        return false;

    if (reconstructed.front() != 2)
        return false;

    if (reconstructed.back() != 5)
        return false;

    return true;
}

static_assert(iota_iterator_constructor_works());


constexpr bool iota_reverse_integration_works()
{
    auto reversed =
            tested::ranges::views::iota(1, 5) |
            tested::ranges::views::reverse;

    if (reversed.size() != 4)
        return false;

    if (reversed[0] != 4)
        return false;

    if (reversed[1] != 3)
        return false;

    if (reversed[2] != 2)
        return false;

    if (reversed[3] != 1)
        return false;

    return true;
}

static_assert(iota_reverse_integration_works());

using mixed_integral_iota =
tested::ranges::iota_view<int, long>;

static_assert(
    tested::ranges::view<
        mixed_integral_iota
    >
);

static_assert(
    !tested::ranges::common_range<
        mixed_integral_iota
    >
);

static_assert(
    tested::ranges::sized_range<
        mixed_integral_iota
    >
);

static_assert(
    tested::ranges::random_access_range<
        mixed_integral_iota
    >
);

constexpr bool mixed_integral_iota_works()
{
    auto view =
            tested::ranges::views::iota(
                3,
                7L
            );

    static_assert(tested::is_same_v<
        decltype(view),
        mixed_integral_iota
    >);

    if (view.size() != 4)
        return false;

    auto iterator = view.begin();

    if (*iterator++ != 3)
        return false;

    if (*iterator++ != 4)
        return false;

    if (*iterator++ != 5)
        return false;

    if (*iterator++ != 6)
        return false;

    if (iterator != view.end())
        return false;

    return true;
}

static_assert(mixed_integral_iota_works());

// take_view test support

struct forward_sized_view
        : tested::ranges::view_base
{
    bidirectional_pointer_iterator first{};
    bidirectional_pointer_iterator last{};

    constexpr forward_sized_view() noexcept = default;

    constexpr forward_sized_view(
        int* begin_value,
        int* end_value
    ) noexcept
        : first(begin_value),
          last(end_value) {}

    constexpr bidirectional_pointer_iterator
    begin() const noexcept
    {
        return first;
    }

    constexpr bidirectional_pointer_iterator
    end() const noexcept
    {
        return last;
    }

    constexpr tested::size_t
    size() const noexcept
    {
        return static_cast<tested::size_t>(
            last.value - first.value
        );
    }
};


struct sized_pointer_sentinel
{
    int* value = nullptr;

    constexpr sized_pointer_sentinel()
        noexcept = default;

    constexpr explicit sized_pointer_sentinel(
        int* pointer
    ) noexcept
        : value(pointer) {}

    friend constexpr bool operator==(
        int* iterator,
        sized_pointer_sentinel sentinel
    ) noexcept
    {
        return iterator == sentinel.value;
    }

    friend constexpr bool operator==(
        sized_pointer_sentinel sentinel,
        int* iterator
    ) noexcept
    {
        return sentinel.value == iterator;
    }

    friend constexpr bool operator!=(
        int* iterator,
        sized_pointer_sentinel sentinel
    ) noexcept
    {
        return !(iterator == sentinel);
    }

    friend constexpr bool operator!=(
        sized_pointer_sentinel sentinel,
        int* iterator
    ) noexcept
    {
        return !(sentinel == iterator);
    }

    friend constexpr tested::ptrdiff_t
    operator-(
        sized_pointer_sentinel sentinel,
        int* iterator
    ) noexcept
    {
        return sentinel.value - iterator;
    }

    friend constexpr tested::ptrdiff_t
    operator-(
        int* iterator,
        sized_pointer_sentinel sentinel
    ) noexcept
    {
        return iterator - sentinel.value;
    }
};


struct explicitly_unsized_sentinel_view
        : tested::ranges::view_base
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr explicitly_unsized_sentinel_view()
        noexcept = default;

    constexpr explicitly_unsized_sentinel_view(
        int* begin_value,
        int* end_value
    ) noexcept
        : first(begin_value),
          last(end_value) {}

    constexpr int* begin() const noexcept
    {
        return first;
    }

    constexpr sized_pointer_sentinel
    end() const noexcept
    {
        return sized_pointer_sentinel{last};
    }
};


struct simple_pointer_view
        : tested::ranges::view_base
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr simple_pointer_view() noexcept = default;

    constexpr simple_pointer_view(
        int* begin_value,
        int* end_value
    ) noexcept
        : first(begin_value),
          last(end_value) {}

    constexpr int* begin() const noexcept
    {
        return first;
    }

    constexpr int* end() const noexcept
    {
        return last;
    }

    constexpr tested::size_t
    size() const noexcept
    {
        return static_cast<tested::size_t>(
            last - first
        );
    }
};

#ifdef FTL_REPLACE_STL

namespace std::ranges
{
    template<>
    inline constexpr bool
    disable_sized_range<
        ::explicitly_unsized_sentinel_view
    > = true;
} // namespace std::ranges

#else

namespace ftl::ranges
{
    template<>
    inline constexpr bool
    disable_sized_range<
        ::explicitly_unsized_sentinel_view
    > = true;
} // namespace ftl::ranges

#endif

using take_pointer_view =
tested::ranges::take_view<pointer_view>;

using take_forward_sized_view =
tested::ranges::take_view<
    forward_sized_view
>;

using take_non_common_view =
tested::ranges::take_view<
    non_common_view
>;

using take_explicitly_unsized_view =
tested::ranges::take_view<
    explicitly_unsized_sentinel_view
>;

using take_simple_pointer_view =
tested::ranges::take_view<
    simple_pointer_view
>;


static_assert(
    tested::ranges::view<
        take_pointer_view
    >
);

static_assert(
    tested::ranges::random_access_range<
        take_pointer_view
    >
);

static_assert(
    tested::ranges::common_range<
        take_pointer_view
    >
);

static_assert(
    tested::ranges::sized_range<
        take_pointer_view
    >
);

static_assert(
    tested::ranges::borrowed_range<
        take_pointer_view
    >
);

static_assert(
    tested::ranges::contiguous_range<
        take_pointer_view
    >
);


static_assert(
    tested::ranges::bidirectional_range<
        take_forward_sized_view
    >
);

static_assert(
    !tested::ranges::random_access_range<
        take_forward_sized_view
    >
);

static_assert(
    tested::ranges::sized_range<
        take_forward_sized_view
    >
);

static_assert(
    !tested::ranges::common_range<
        take_forward_sized_view
    >
);


static_assert(
    tested::ranges::random_access_range<
        take_non_common_view
    >
);

static_assert(
    !tested::ranges::sized_range<
        take_non_common_view
    >
);

static_assert(
    !tested::ranges::common_range<
        take_non_common_view
    >
);


static_assert(
    tested::ranges::sized_range<
        explicitly_unsized_sentinel_view
    >
);

static_assert(
    tested::sized_sentinel_for<
        sized_pointer_sentinel,
        int*>
);

static_assert(
    tested::ranges::random_access_range<
        take_explicitly_unsized_view
    >
);

static_assert(
    tested::ranges::sized_range<
        take_explicitly_unsized_view
    >
);

static_assert(
    tested::ranges::common_range<
        take_explicitly_unsized_view
    >
);


static_assert(
    tested::ranges::view<
        take_simple_pointer_view
    >
);

static_assert(
    tested::ranges::common_range<
        take_simple_pointer_view
    >
);

static_assert(tested::is_same_v<
    tested::ranges::iterator_t<
        take_simple_pointer_view
    >,
    tested::ranges::iterator_t<
        const take_simple_pointer_view>
>);


static_assert(tested::is_same_v<
    decltype(
        tested::declval<
            const take_pointer_view&>().base()
    ),
    pointer_view
>);

static_assert(tested::is_same_v<
    decltype(
        tested::declval<
            take_pointer_view&&>().base()
    ),
    pointer_view
>);

constexpr bool take_view_direct_works()
{
    int values[] = {1, 2, 3, 4, 5};

    pointer_view base{
        values,
        values + 5
    };

    tested::ranges::take_view taken(
        base,
        tested::ptrdiff_t{3}
    );

    static_assert(tested::is_same_v<
        decltype(taken),
        take_pointer_view
    >);

    if (taken.empty())
        return false;

    if (taken.size() != 3)
        return false;

    if (taken.begin() != values)
        return false;

    if (taken.end() != values + 3)
        return false;

    if (taken.front() != 1)
        return false;

    if (taken.back() != 3)
        return false;

    if (taken[0] != 1)
        return false;

    if (taken[1] != 2)
        return false;

    if (taken[2] != 3)
        return false;

    auto copied_base =
            static_cast<const decltype(taken)&>(
                taken
            ).base();

    if (copied_base.begin() != values)
        return false;

    if (copied_base.end() != values + 5)
        return false;

    auto moved_base =
            static_cast<decltype(taken)&&>(
                taken
            ).base();

    if (moved_base.begin() != values)
        return false;

    if (moved_base.end() != values + 5)
        return false;

    return true;
}

static_assert(take_view_direct_works());


constexpr bool take_view_clamps_to_base_size()
{
    int values[] = {1, 2, 3};

    pointer_view base{
        values,
        values + 3
    };

    tested::ranges::take_view taken(
        base,
        tested::ptrdiff_t{100}
    );

    if (taken.size() != 3)
        return false;

    if (taken.begin() != values)
        return false;

    if (taken.end() != values + 3)
        return false;

    if (taken.back() != 3)
        return false;

    return true;
}

static_assert(take_view_clamps_to_base_size());


constexpr bool take_view_zero_works()
{
    int values[] = {1, 2, 3};

    pointer_view base{
        values,
        values + 3
    };

    tested::ranges::take_view taken(
        base,
        tested::ptrdiff_t{0}
    );

    if (!taken.empty())
        return false;

    if (taken.size() != 0)
        return false;

    if (taken.begin() != taken.end())
        return false;

    if (taken.begin() != values)
        return false;

    return true;
}

static_assert(take_view_zero_works());


constexpr bool take_view_const_works()
{
    int values[] = {1, 2, 3, 4};

    const tested::ranges::take_view taken(
        pointer_view{
            values,
            values + 4
        },
        tested::ptrdiff_t{2}
    );

    if (taken.size() != 2)
        return false;

    if (taken.begin() != values)
        return false;

    if (taken.end() != values + 2)
        return false;

    if (taken.front() != 1)
        return false;

    if (taken.back() != 2)
        return false;

    return true;
}

static_assert(take_view_const_works());


constexpr bool take_adaptor_direct_works()
{
    int values[] = {1, 2, 3, 4};

    pointer_view base{
        values,
        values + 4
    };

    auto taken =
            tested::ranges::views::take(
                base,
                2
            );

    static_assert(tested::is_same_v<
        decltype(taken),
        take_pointer_view
    >);

    return
            taken.size() == 2 &&
            taken.front() == 1 &&
            taken.back() == 2;
}

static_assert(take_adaptor_direct_works());


constexpr bool take_adaptor_pipeline_works()
{
    int values[] = {1, 2, 3, 4};

    pointer_view base{
        values,
        values + 4
    };

    auto taken =
            base |
            tested::ranges::views::take(3);

    static_assert(tested::is_same_v<
        decltype(taken),
        take_pointer_view
    >);

    if (taken.size() != 3)
        return false;

    if (taken[0] != 1)
        return false;

    if (taken[1] != 2)
        return false;

    if (taken[2] != 3)
        return false;

    return true;
}

static_assert(take_adaptor_pipeline_works());


constexpr bool take_array_pipeline_works()
{
    int values[] = {1, 2, 3, 4};

    auto taken =
            values |
            tested::ranges::views::take(2);

    if (taken.size() != 2)
        return false;

    if (taken.front() != 1)
        return false;

    if (taken.back() != 2)
        return false;

    return true;
}

static_assert(take_array_pipeline_works());

constexpr bool take_sized_non_random_access_works()
{
    int values[] = {1, 2, 3, 4};

    forward_sized_view base{
        values,
        values + 4
    };

    tested::ranges::take_view taken(
        base,
        tested::ptrdiff_t{3}
    );

    static_assert(tested::is_same_v<
        tested::ranges::iterator_t<
            decltype(taken)
        >,
        tested::counted_iterator<
            bidirectional_pointer_iterator
        >
    >);

    static_assert(tested::is_same_v<
        tested::ranges::sentinel_t<
            decltype(taken)
        >,
        tested::default_sentinel_t
    >);

    if (taken.size() != 3)
        return false;

    auto iterator = taken.begin();

    if (*iterator++ != 1)
        return false;

    if (*iterator++ != 2)
        return false;

    if (*iterator++ != 3)
        return false;

    if (iterator != taken.end())
        return false;

    return true;
}

static_assert(
    take_sized_non_random_access_works()
);


constexpr bool take_sized_non_random_clamps()
{
    int values[] = {1, 2, 3};

    forward_sized_view base{
        values,
        values + 3
    };

    tested::ranges::take_view taken(
        base,
        tested::ptrdiff_t{10}
    );

    if (taken.size() != 3)
        return false;

    auto iterator = taken.begin();

    if (*iterator++ != 1)
        return false;

    if (*iterator++ != 2)
        return false;

    if (*iterator++ != 3)
        return false;

    return iterator == taken.end();
}

static_assert(
    take_sized_non_random_clamps()
);

constexpr bool take_unsized_non_common_stops_at_count()
{
    int values[] = {1, 2, 3, 4, 5};
    int comparisons = 0;

    non_common_view base{
        values,
        values + 5,
        &comparisons
    };

    tested::ranges::take_view taken(
        base,
        tested::ptrdiff_t{3}
    );

    auto iterator = taken.begin();

    if (*iterator++ != 1)
        return false;

    if (*iterator++ != 2)
        return false;

    if (*iterator++ != 3)
        return false;

    if (iterator != taken.end())
        return false;

    return true;
}

static_assert(
    take_unsized_non_common_stops_at_count()
);


constexpr bool take_unsized_non_common_stops_at_base_end()
{
    int values[] = {1, 2, 3};
    int comparisons = 0;

    non_common_view base{
        values,
        values + 3,
        &comparisons
    };

    tested::ranges::take_view taken(
        base,
        tested::ptrdiff_t{10}
    );

    auto iterator = taken.begin();

    if (*iterator++ != 1)
        return false;

    if (*iterator++ != 2)
        return false;

    if (*iterator++ != 3)
        return false;

    if (iterator != taken.end())
        return false;

    if (iterator.count() != 7)
        return false;

    return true;
}

static_assert(
    take_unsized_non_common_stops_at_base_end()
);

constexpr bool take_sized_subtraction_path_works()
{
    int values[] = {1, 2, 3};

    explicitly_unsized_sentinel_view base{
        values,
        values + 3
    };

    tested::ranges::take_view taken(
        base,
        tested::ptrdiff_t{10}
    );

    static_assert(
        tested::ranges::sized_range<
            explicitly_unsized_sentinel_view
        >
    );

    static_assert(tested::is_same_v<
        tested::ranges::iterator_t<
            decltype(taken)
        >,
        int*>);

    static_assert(tested::is_same_v<
        tested::ranges::sentinel_t<
            decltype(taken)
        >,
        int*>);

    if (taken.size() != 3)
        return false;

    if (taken.begin() != values)
        return false;

    if (taken.end() != values + 3)
        return false;

    if (taken[0] != 1)
        return false;

    if (taken[1] != 2)
        return false;

    if (taken[2] != 3)
        return false;

    return true;
}

static_assert(take_sized_subtraction_path_works());

struct input_pointer_iterator
{
    using value_type = int;
    using difference_type = tested::ptrdiff_t;
    using iterator_concept =
    tested::input_iterator_tag;
    using iterator_category =
    tested::input_iterator_tag;

    int* value = nullptr;

    constexpr input_pointer_iterator() noexcept = default;

    constexpr explicit input_pointer_iterator(
        int* pointer
    ) noexcept
        : value(pointer) {}

    [[nodiscard]]
    constexpr int& operator*() const noexcept
    {
        return *value;
    }

    constexpr input_pointer_iterator&
    operator++() noexcept
    {
        ++value;
        return *this;
    }

    constexpr void operator++(int) noexcept
    {
        ++value;
    }
};


struct sized_input_sentinel
{
    int* value = nullptr;

    constexpr sized_input_sentinel() noexcept = default;

    constexpr explicit sized_input_sentinel(
        int* pointer
    ) noexcept
        : value(pointer) {}

    friend constexpr bool operator==(
        input_pointer_iterator iterator,
        sized_input_sentinel sentinel
    ) noexcept
    {
        return iterator.value == sentinel.value;
    }

    friend constexpr bool operator==(
        sized_input_sentinel sentinel,
        input_pointer_iterator iterator
    ) noexcept
    {
        return sentinel.value == iterator.value;
    }

    friend constexpr bool operator!=(
        input_pointer_iterator iterator,
        sized_input_sentinel sentinel
    ) noexcept
    {
        return !(iterator == sentinel);
    }

    friend constexpr bool operator!=(
        sized_input_sentinel sentinel,
        input_pointer_iterator iterator
    ) noexcept
    {
        return !(sentinel == iterator);
    }

    friend constexpr tested::ptrdiff_t operator-(
        sized_input_sentinel sentinel,
        input_pointer_iterator iterator
    ) noexcept
    {
        return sentinel.value - iterator.value;
    }

    friend constexpr tested::ptrdiff_t operator-(
        input_pointer_iterator iterator,
        sized_input_sentinel sentinel
    ) noexcept
    {
        return iterator.value - sentinel.value;
    }
};


struct input_sized_sentinel_view
        : tested::ranges::view_base
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr input_sized_sentinel_view() noexcept = default;

    constexpr input_sized_sentinel_view(
        int* begin_value,
        int* end_value
    ) noexcept
        : first(begin_value),
          last(end_value) {}

    [[nodiscard]]
    constexpr input_pointer_iterator
    begin() noexcept
    {
        return input_pointer_iterator{first};
    }

    [[nodiscard]]
    constexpr sized_input_sentinel
    end() noexcept
    {
        return sized_input_sentinel{last};
    }
};


static_assert(
    tested::input_iterator<
        input_pointer_iterator
    >
);

static_assert(
    !tested::forward_iterator<
        input_pointer_iterator
    >
);

static_assert(
    tested::sentinel_for<
        sized_input_sentinel,
        input_pointer_iterator
    >
);

static_assert(
    tested::sized_sentinel_for<
        sized_input_sentinel,
        input_pointer_iterator
    >
);

static_assert(
    tested::ranges::input_range<
        input_sized_sentinel_view
    >
);

// The iterator is not forward, so ranges::size cannot use the
// sized-sentinel subtraction fallback.
static_assert(
    !tested::ranges::sized_range<
        input_sized_sentinel_view
    >
);


using take_input_sized_sentinel_view =
tested::ranges::take_view<
    input_sized_sentinel_view
>;

static_assert(
    tested::ranges::input_range<
        take_input_sized_sentinel_view
    >
);

static_assert(
    !tested::ranges::forward_range<
        take_input_sized_sentinel_view
    >
);

static_assert(
    !tested::ranges::sized_range<
        take_input_sized_sentinel_view
    >
);

static_assert(
    !tested::ranges::common_range<
        take_input_sized_sentinel_view
    >
);

static_assert(tested::is_same_v<
    tested::detail::iter_concept_t<
        input_pointer_iterator
    >,
    tested::input_iterator_tag
>);

static_assert(tested::is_same_v<
    typename tested::counted_iterator<
        input_pointer_iterator
    >::iterator_concept,
    tested::input_iterator_tag
>);

using counted_input_iterator =
tested::counted_iterator<
    input_pointer_iterator
>;

static_assert(
    tested::weakly_incrementable<
        counted_input_iterator
    >
);

static_assert(
    tested::input_or_output_iterator<
        counted_input_iterator
    >
);

static_assert(
    tested::indirectly_readable<
        counted_input_iterator
    >
);

static_assert(
    tested::input_iterator<
        tested::counted_iterator<
            input_pointer_iterator
        >
    >
);

static_assert(
    tested::ranges::input_range<
        take_input_sized_sentinel_view
    >
);

static_assert(
    tested::input_iterator<
        tested::counted_iterator<input_pointer_iterator>
    >
);

using counted_input_iterator =
tested::counted_iterator<
    input_pointer_iterator
>;

static_assert(tested::is_same_v<
    tested::detail::iter_concept_t<
        counted_input_iterator
    >,
    tested::input_iterator_tag
>);

using counted_input_iterator =
tested::counted_iterator<
    input_pointer_iterator
>;

static_assert(
    tested::detail::uses_generated_iterator_traits<
        counted_input_iterator
    >
);

static_assert(tested::is_same_v<
    tested::ranges::iterator_t<
        take_input_sized_sentinel_view
    >,
    tested::counted_iterator<
        input_pointer_iterator
    >
>);

static_assert(tested::is_same_v<
    tested::ranges::sentinel_t<
        take_input_sized_sentinel_view
    >,
    tested::default_sentinel_t
>);

constexpr bool take_sized_sentinel_path_works()
{
    int values[] = {1, 2, 3};

    input_sized_sentinel_view base{
        values,
        values + 3
    };

    tested::ranges::take_view taken(
        base,
        tested::ptrdiff_t{10}
    );

    auto iterator = taken.begin();

    // take_view clamps the requested ten elements to the
    // three elements reported by end(base) - begin(base).
    if (iterator.count() != 3)
        return false;

    if (*iterator != 1)
        return false;

    ++iterator;

    if (*iterator != 2)
        return false;

    ++iterator;

    if (*iterator != 3)
        return false;

    ++iterator;

    if (iterator != taken.end())
        return false;

    if (iterator.count() != 0)
        return false;

    return true;
}

static_assert(
    take_sized_sentinel_path_works()
);

constexpr bool take_simple_view_works()
{
    int values[] = {1, 2, 3, 4};

    tested::ranges::take_view taken(
        simple_pointer_view{
            values,
            values + 4
        },
        tested::ptrdiff_t{2}
    );

    const auto& constant = taken;

    if (taken.begin() != values)
        return false;

    if (taken.end() != values + 2)
        return false;

    if (constant.begin() != values)
        return false;

    if (constant.end() != values + 2)
        return false;

    return true;
}

static_assert(take_simple_view_works());

constexpr bool take_empty_view_normalization_works()
{
    constexpr auto empty =
            tested::ranges::views::empty<int>;

    auto taken =
            tested::ranges::views::take(
                empty,
                20
            );

    static_assert(tested::is_same_v<
        decltype(taken),
        tested::ranges::empty_view<int>
    >);

    return
            taken.empty() &&
            taken.size() == 0;
}

static_assert(
    take_empty_view_normalization_works()
);


constexpr bool take_subrange_normalization_works()
{
    int values[] = {1, 2, 3, 4};

    tested::ranges::subrange original(
        values,
        values + 4
    );

    auto taken =
            tested::ranges::views::take(
                original,
                2
            );

    using expected_type =
            tested::ranges::subrange<int*>;

    static_assert(tested::is_same_v<
        decltype(taken),
        expected_type
    >);

    if (taken.begin() != values)
        return false;

    if (taken.end() != values + 2)
        return false;

    if (taken.size() != 2)
        return false;

    return true;
}

static_assert(
    take_subrange_normalization_works()
);


constexpr bool take_subrange_clamping_works()
{
    int values[] = {1, 2, 3};

    tested::ranges::subrange original(
        values,
        values + 3
    );

    auto taken =
            tested::ranges::views::take(
                original,
                20
            );

    return
            taken.begin() == values &&
            taken.end() == values + 3 &&
            taken.size() == 3;
}

static_assert(
    take_subrange_clamping_works()
);


constexpr bool take_iota_normalization_works()
{
    auto original =
            tested::ranges::views::iota(
                10,
                20
            );

    auto taken =
            tested::ranges::views::take(
                original,
                4
            );

    using expected_type =
            tested::ranges::iota_view<int, int>;

    static_assert(tested::is_same_v<
        decltype(taken),
        expected_type
    >);

    if (taken.size() != 4)
        return false;

    if (taken.front() != 10)
        return false;

    if (taken.back() != 13)
        return false;

    return true;
}

static_assert(
    take_iota_normalization_works()
);


constexpr bool take_iota_clamping_works()
{
    auto original =
            tested::ranges::views::iota(
                10,
                13
            );

    auto taken =
            original |
            tested::ranges::views::take(100);

    return
            taken.size() == 3 &&
            taken.front() == 10 &&
            taken.back() == 12;
}

static_assert(
    take_iota_clamping_works()
);

constexpr bool take_reverse_integration_works()
{
    int values[] = {1, 2, 3, 4, 5};

    auto result =
            values |
            tested::ranges::views::take(3) |
            tested::ranges::views::reverse;

    if (result.size() != 3)
        return false;

    if (result[0] != 3)
        return false;

    if (result[1] != 2)
        return false;

    if (result[2] != 1)
        return false;

    return true;
}

static_assert(
    take_reverse_integration_works()
);


constexpr bool iota_take_reverse_integration_works()
{
    auto result =
            tested::ranges::views::iota(1, 10) |
            tested::ranges::views::take(4) |
            tested::ranges::views::reverse;

    if (result.size() != 4)
        return false;

    if (result[0] != 4)
        return false;

    if (result[1] != 3)
        return false;

    if (result[2] != 2)
        return false;

    if (result[3] != 1)
        return false;

    return true;
}

static_assert(
    iota_take_reverse_integration_works()
);

bool ftl_test()
{
    return reverse_view_direct_works() &&
           reverse_view_empty_works() &&
           reverse_adaptor_call_works() &&
           reverse_adaptor_pipeline_works() &&
           reverse_array_pipeline_works() &&
           reverse_const_base_works() &&
           reverse_non_common_range_works() &&
           reverse_non_common_cache_works() &&
           reverse_view_normalization_works() &&
           sized_reverse_subrange_normalization_works() &&
           unsized_reverse_subrange_normalization_works() &&
           bounded_iota_works() &&
           empty_iota_works() &&
           unbounded_iota_works() &&
           unsigned_iota_arithmetic_works() &&
           different_bound_iota_works() &&
           mixed_integral_iota_works() &&
           forward_iota_works() &&
           bidirectional_iota_works() &&
           iota_iterator_constructor_works() &&
           iota_reverse_integration_works() &&
           take_view_direct_works() &&
           take_view_clamps_to_base_size() &&
           take_view_zero_works() &&
           take_view_const_works() &&
           take_adaptor_direct_works() &&
           take_adaptor_pipeline_works() &&
           take_array_pipeline_works() &&
           take_sized_non_random_access_works() &&
           take_sized_non_random_clamps() &&
           take_unsized_non_common_stops_at_count() &&
           take_unsized_non_common_stops_at_base_end() &&
           take_sized_subtraction_path_works() &&
           take_simple_view_works() &&
           take_empty_view_normalization_works() &&
           take_subrange_normalization_works() &&
           take_subrange_clamping_works() &&
           take_iota_normalization_works() &&
           take_iota_clamping_works() &&
           take_reverse_integration_works() &&
           iota_take_reverse_integration_works();
}
