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
          last(end_value)
    {
    }

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
          comparisons(comparison_counter)
    {
    }

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
          comparisons(comparison_counter)
    {
    }

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
        int*
    >
);

static_assert(
    !tested::sized_sentinel_for<
        counting_sentinel,
        int*
    >
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
            const reverse_pointer_view&
        >().base()
    ),
    pointer_view
>);

static_assert(tested::is_same_v<
    decltype(
        tested::declval<
            reverse_pointer_view&&
        >().base()
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

static_assert(reverse_non_common_range_works());


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

static_assert(reverse_non_common_cache_works());

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
        : value(pointer)
    {
    }

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

bool ftl_test()
{
    return
        reverse_view_direct_works() &&
        reverse_view_empty_works() &&
        reverse_adaptor_call_works() &&
        reverse_adaptor_pipeline_works() &&
        reverse_array_pipeline_works() &&
        reverse_const_base_works() &&
        reverse_non_common_range_works() &&
        reverse_non_common_cache_works() &&
        reverse_view_normalization_works() &&
        sized_reverse_subrange_normalization_works() &&
        unsized_reverse_subrange_normalization_works();
}