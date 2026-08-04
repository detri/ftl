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
 * A random-access iterator whose mutable form converts to its
 * const form, but never in the opposite direction.
 */

template<bool Const>
class interop_iterator {
private:
    using pointer_type =
        tested::conditional_t<
            Const,
            const int*,
            int*
        >;

    pointer_type current_ = nullptr;

public:
    using iterator_concept =
        tested::random_access_iterator_tag;

    using iterator_category =
        tested::random_access_iterator_tag;

    using value_type = int;

    using difference_type =
        tested::ptrdiff_t;

    using reference =
        tested::conditional_t<
            Const,
            const int&,
            int&
        >;

    using pointer = pointer_type;

    constexpr interop_iterator() = default;

    constexpr explicit interop_iterator(
        pointer_type current
    )
        : current_(current) {}

    interop_iterator(
        const interop_iterator&
    ) = default;

    interop_iterator(
        interop_iterator&&
    ) = default;

    constexpr interop_iterator& operator=(
        const interop_iterator&
    ) = default;

    constexpr interop_iterator& operator=(
        interop_iterator&&
    ) = default;

    template<bool OtherConst>
        requires (
            Const &&
            !OtherConst
        )
    constexpr interop_iterator(
        const interop_iterator<
            OtherConst
        >& other
    )
        : current_(other.base()) {}

    [[nodiscard]]
    constexpr pointer_type
    base() const noexcept {
        return current_;
    }

    [[nodiscard]]
    constexpr reference
    operator*() const noexcept {
        return *current_;
    }

    [[nodiscard]]
    constexpr pointer
    operator->() const noexcept {
        return current_;
    }

    constexpr interop_iterator&
    operator++() noexcept {
        ++current_;
        return *this;
    }

    constexpr interop_iterator
    operator++(int) noexcept {
        auto previous = *this;
        ++*this;
        return previous;
    }

    constexpr interop_iterator&
    operator--() noexcept {
        --current_;
        return *this;
    }

    constexpr interop_iterator
    operator--(int) noexcept {
        auto previous = *this;
        --*this;
        return previous;
    }

    constexpr interop_iterator&
    operator+=(
        difference_type offset
    ) noexcept {
        current_ += offset;
        return *this;
    }

    constexpr interop_iterator&
    operator-=(
        difference_type offset
    ) noexcept {
        current_ -= offset;
        return *this;
    }

    [[nodiscard]]
    constexpr reference operator[](
        difference_type offset
    ) const noexcept {
        return current_[offset];
    }
};

template<bool Const>
[[nodiscard]]
constexpr interop_iterator<Const>
operator+(
    interop_iterator<Const> iterator,
    typename interop_iterator<
        Const
    >::difference_type offset
) noexcept {
    iterator += offset;
    return iterator;
}

template<bool Const>
[[nodiscard]]
constexpr interop_iterator<Const>
operator+(
    typename interop_iterator<
        Const
    >::difference_type offset,
    interop_iterator<Const> iterator
) noexcept {
    iterator += offset;
    return iterator;
}

template<bool Const>
[[nodiscard]]
constexpr interop_iterator<Const>
operator-(
    interop_iterator<Const> iterator,
    typename interop_iterator<
        Const
    >::difference_type offset
) noexcept {
    iterator -= offset;
    return iterator;
}

template<bool LeftConst, bool RightConst>
[[nodiscard]]
constexpr tested::ptrdiff_t operator-(
    const interop_iterator<
        LeftConst
    >& left,
    const interop_iterator<
        RightConst
    >& right
) noexcept {
    return left.base() - right.base();
}

template<bool LeftConst, bool RightConst>
[[nodiscard]]
constexpr bool operator==(
    const interop_iterator<
        LeftConst
    >& left,
    const interop_iterator<
        RightConst
    >& right
) noexcept {
    return left.base() == right.base();
}

template<bool LeftConst, bool RightConst>
[[nodiscard]]
constexpr bool operator<(
    const interop_iterator<
        LeftConst
    >& left,
    const interop_iterator<
        RightConst
    >& right
) noexcept {
    return left.base() < right.base();
}

template<bool LeftConst, bool RightConst>
[[nodiscard]]
constexpr bool operator>(
    const interop_iterator<
        LeftConst
    >& left,
    const interop_iterator<
        RightConst
    >& right
) noexcept {
    return right < left;
}

template<bool LeftConst, bool RightConst>
[[nodiscard]]
constexpr bool operator<=(
    const interop_iterator<
        LeftConst
    >& left,
    const interop_iterator<
        RightConst
    >& right
) noexcept {
    return !(right < left);
}

template<bool LeftConst, bool RightConst>
[[nodiscard]]
constexpr bool operator>=(
    const interop_iterator<
        LeftConst
    >& left,
    const interop_iterator<
        RightConst
    >& right
) noexcept {
    return !(left < right);
}

/*
 * A separately typed sentinel with the same one-way
 * mutable-to-const conversion.
 */

template<bool Const>
class interop_sentinel {
private:
    using pointer_type =
        tested::conditional_t<
            Const,
            const int*,
            int*
        >;

    pointer_type end_ = nullptr;

public:
    constexpr interop_sentinel() = default;

    constexpr explicit interop_sentinel(
        pointer_type end
    )
        : end_(end) {}

    interop_sentinel(
        const interop_sentinel&
    ) = default;

    interop_sentinel(
        interop_sentinel&&
    ) = default;

    constexpr interop_sentinel& operator=(
        const interop_sentinel&
    ) = default;

    constexpr interop_sentinel& operator=(
        interop_sentinel&&
    ) = default;

    template<bool OtherConst>
        requires (
            Const &&
            !OtherConst
        )
    constexpr interop_sentinel(
        const interop_sentinel<
            OtherConst
        >& other
    )
        : end_(other.base()) {}

    [[nodiscard]]
    constexpr pointer_type
    base() const noexcept {
        return end_;
    }
};

template<bool IteratorConst, bool SentinelConst>
[[nodiscard]]
constexpr bool operator==(
    const interop_iterator<
        IteratorConst
    >& iterator,
    const interop_sentinel<
        SentinelConst
    >& sentinel
) noexcept {
    return
        iterator.base() ==
        sentinel.base();
}

template<bool IteratorConst, bool SentinelConst>
[[nodiscard]]
constexpr bool operator==(
    const interop_sentinel<
        SentinelConst
    >& sentinel,
    const interop_iterator<
        IteratorConst
    >& iterator
) noexcept {
    return iterator == sentinel;
}

template<bool IteratorConst, bool SentinelConst>
[[nodiscard]]
constexpr tested::ptrdiff_t operator-(
    const interop_sentinel<
        SentinelConst
    >& sentinel,
    const interop_iterator<
        IteratorConst
    >& iterator
) noexcept {
    return
        sentinel.base() -
        iterator.base();
}

template<bool IteratorConst, bool SentinelConst>
[[nodiscard]]
constexpr tested::ptrdiff_t operator-(
    const interop_iterator<
        IteratorConst
    >& iterator,
    const interop_sentinel<
        SentinelConst
    >& sentinel
) noexcept {
    return
        iterator.base() -
        sentinel.base();
}

/*
 * Common and non-common base views.
 */

struct interop_common_view
    : tested::ranges::view_base
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr interop_common_view()
        = default;

    constexpr interop_common_view(
        int* first,
        int* last
    )
        : first(first),
          last(last) {}

    [[nodiscard]]
    constexpr interop_iterator<false>
    begin() noexcept {
        return interop_iterator<false>(
            first
        );
    }

    [[nodiscard]]
    constexpr interop_iterator<false>
    end() noexcept {
        return interop_iterator<false>(
            last
        );
    }

    [[nodiscard]]
    constexpr interop_iterator<true>
    begin() const noexcept {
        return interop_iterator<true>(
            first
        );
    }

    [[nodiscard]]
    constexpr interop_iterator<true>
    end() const noexcept {
        return interop_iterator<true>(
            last
        );
    }

    [[nodiscard]]
    constexpr tested::size_t
    size() const noexcept {
        return static_cast<
            tested::size_t
        >(last - first);
    }
};

struct interop_sentinel_view
    : tested::ranges::view_base
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr interop_sentinel_view()
        = default;

    constexpr interop_sentinel_view(
        int* first,
        int* last
    )
        : first(first),
          last(last) {}

    [[nodiscard]]
    constexpr interop_iterator<false>
    begin() noexcept {
        return interop_iterator<false>(
            first
        );
    }

    [[nodiscard]]
    constexpr interop_sentinel<false>
    end() noexcept {
        return interop_sentinel<false>(
            last
        );
    }

    [[nodiscard]]
    constexpr interop_iterator<true>
    begin() const noexcept {
        return interop_iterator<true>(
            first
        );
    }

    [[nodiscard]]
    constexpr interop_sentinel<true>
    end() const noexcept {
        return interop_sentinel<true>(
            last
        );
    }

    [[nodiscard]]
    constexpr tested::size_t
    size() const noexcept {
        return static_cast<
            tested::size_t
        >(last - first);
    }
};

static_assert(
    tested::ranges::random_access_range<
        interop_common_view
    >
);

static_assert(
    tested::ranges::random_access_range<
        const interop_common_view
    >
);

static_assert(
    tested::ranges::common_range<
        interop_common_view
    >
);

static_assert(
    tested::ranges::common_range<
        const interop_common_view
    >
);

static_assert(
    tested::ranges::random_access_range<
        interop_sentinel_view
    >
);

static_assert(
    tested::ranges::random_access_range<
        const interop_sentinel_view
    >
);

static_assert(
    !tested::ranges::common_range<
        interop_sentinel_view
    >
);

static_assert(
    !tested::ranges::common_range<
        const interop_sentinel_view
    >
);

static_assert(
    tested::ranges::sized_range<
        interop_sentinel_view
    >
);

/*
 * Callables used by the adaptor types below.
 */

struct identity_transform {
    template<class T>
    constexpr T&& operator()(
        T&& value
    ) const noexcept {
        return static_cast<T&&>(
            value
        );
    }
};

struct binary_transform {
    constexpr int operator()(
        int left,
        int right
    ) const noexcept {
        return left + right;
    }
};

struct unary_predicate {
    constexpr bool operator()(
        int value
    ) const noexcept {
        return value < 100;
    }
};

struct binary_predicate {
    constexpr bool operator()(
        int left,
        int right
    ) const noexcept {
        return left == right;
    }
};

/*
 * Generic const-interop requirements.
 */

template<class View>
concept const_iterator_interoperable =
    tested::ranges::range<View> &&
    tested::ranges::range<const View> &&
    tested::copyable<
        tested::ranges::iterator_t<
            View
        >
    > &&
    tested::copyable<
        tested::ranges::iterator_t<
            const View
        >
    > &&
    tested::convertible_to<
        tested::ranges::iterator_t<
            View
        >,
        tested::ranges::iterator_t<
            const View
        >
    > &&
    (
        !tested::convertible_to<
            tested::ranges::iterator_t<
                const View
            >,
            tested::ranges::iterator_t<
                View
            >
        >
    ) &&
    requires(
        tested::ranges::iterator_t<
            View
        > mutable_iterator,
        tested::ranges::iterator_t<
            const View
        > const_iterator
    ) {
        {
            mutable_iterator ==
            const_iterator
        } -> tested::convertible_to<bool>;

        {
            const_iterator ==
            mutable_iterator
        } -> tested::convertible_to<bool>;

        {
            mutable_iterator !=
            const_iterator
        } -> tested::convertible_to<bool>;

        {
            const_iterator !=
            mutable_iterator
        } -> tested::convertible_to<bool>;
    };

template<class View>
concept const_sentinel_interoperable =
    tested::ranges::range<View> &&
    tested::ranges::range<const View> &&
    tested::convertible_to<
        tested::ranges::sentinel_t<
            View
        >,
        tested::ranges::sentinel_t<
            const View
        >
    > &&
    tested::sentinel_for<
        tested::ranges::sentinel_t<
            const View
        >,
        tested::ranges::iterator_t<
            View
        >
    >;

template<class View>
concept mixed_sized_sentinel_interoperable =
    !tested::sized_sentinel_for<
        tested::ranges::sentinel_t<
            const View
        >,
        tested::ranges::iterator_t<
            const View
        >
    > ||
    tested::sized_sentinel_for<
        tested::ranges::sentinel_t<
            const View
        >,
        tested::ranges::iterator_t<
            View
        >
    >;

template<class View>
concept mixed_random_access_operations =
    !(
        tested::ranges::random_access_range<
            View
        > &&
        tested::ranges::random_access_range<
            const View
        >
    ) ||
    requires(
        tested::ranges::iterator_t<
            View
        > mutable_iterator,
        tested::ranges::iterator_t<
            const View
        > const_iterator
    ) {
        const_iterator -
        mutable_iterator;

        mutable_iterator -
        const_iterator;

        {
            mutable_iterator <
            const_iterator
        } -> tested::convertible_to<bool>;

        {
            const_iterator <
            mutable_iterator
        } -> tested::convertible_to<bool>;
    };

/*
 * View types under audit.
 */

using take_interop_view =
    decltype(
        tested::ranges::views::take(
            tested::declval<
                interop_sentinel_view
            >(),
            3
        )
    );

using drop_interop_view =
    decltype(
        tested::ranges::views::drop(
            tested::declval<
                interop_sentinel_view
            >(),
            1
        )
    );

using take_while_interop_view =
    decltype(
        tested::ranges::views::
            take_while(
                tested::declval<
                    interop_sentinel_view
                >(),
                unary_predicate{}
            )
    );

using reverse_interop_view =
    decltype(
        tested::ranges::views::reverse(
            tested::declval<
                interop_common_view
            >()
        )
    );

using transform_interop_view =
    decltype(
        tested::ranges::views::transform(
            tested::declval<
                interop_sentinel_view
            >(),
            identity_transform{}
        )
    );

using common_interop_view =
    decltype(
        tested::ranges::views::common(
            tested::declval<
                interop_sentinel_view
            >()
        )
    );

using stride_interop_view =
    decltype(
        tested::ranges::views::stride(
            tested::declval<
                interop_sentinel_view
            >(),
            2
        )
    );

using zip_interop_view =
    decltype(
        tested::ranges::views::zip(
            tested::declval<
                interop_common_view
            >(),
            tested::declval<
                interop_sentinel_view
            >()
        )
    );

using zip_transform_interop_view =
    decltype(
        tested::ranges::views::
            zip_transform(
                binary_transform{},
                tested::declval<
                    interop_common_view
                >(),
                tested::declval<
                    interop_sentinel_view
                >()
            )
    );

using enumerate_interop_view =
    decltype(
        tested::ranges::views::enumerate(
            tested::declval<
                interop_sentinel_view
            >()
        )
    );

using adjacent_interop_view =
    decltype(
        tested::ranges::views::adjacent<2>(
            tested::declval<
                interop_sentinel_view
            >()
        )
    );

using adjacent_transform_interop_view =
    decltype(
        tested::ranges::views::
            adjacent_transform<2>(
                tested::declval<
                    interop_sentinel_view
                >(),
                binary_transform{}
            )
    );

using slide_interop_view =
    decltype(
        tested::ranges::views::slide(
            tested::declval<
                interop_sentinel_view
            >(),
            2
        )
    );

using chunk_interop_view =
    decltype(
        tested::ranges::views::chunk(
            tested::declval<
                interop_sentinel_view
            >(),
            2
        )
    );

using cartesian_product_interop_view =
    decltype(
        tested::ranges::views::
            cartesian_product(
                tested::declval<
                    interop_sentinel_view
                >(),
                tested::declval<
                    interop_common_view
                >()
            )
    );

using nested_array =
    int[2][3];

using join_interop_view =
    decltype(
        tested::ranges::views::join(
            tested::declval<
                nested_array&
            >()
        )
    );

using lazy_split_interop_view =
    decltype(
        tested::ranges::views::
            lazy_split(
                tested::declval<
                    interop_common_view
                >(),
                3
            )
    );

/*
 * Every applicable mutable iterator must promote to the
 * const iterator without losing ordinary copy/move support.
 */

static_assert(
    const_iterator_interoperable<
        take_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        drop_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        take_while_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        reverse_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        transform_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        common_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        stride_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        zip_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        zip_transform_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        enumerate_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        adjacent_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        adjacent_transform_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        slide_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        chunk_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        cartesian_product_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        join_interop_view
    >
);

static_assert(
    const_iterator_interoperable<
        lazy_split_interop_view
    >
);

/*
 * A const sentinel must accept both the const iterator and a
 * mutable iterator promoted to the const iterator domain.
 */

static_assert(
    const_sentinel_interoperable<
        take_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        drop_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        take_while_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        reverse_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        transform_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        common_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        stride_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        zip_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        zip_transform_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        enumerate_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        adjacent_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        adjacent_transform_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        slide_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        chunk_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        cartesian_product_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        join_interop_view
    >
);

static_assert(
    const_sentinel_interoperable<
        lazy_split_interop_view
    >
);

/*
 * Sized const sentinels must preserve subtraction when paired
 * with mutable iterators.
 */

static_assert(
    mixed_sized_sentinel_interoperable<
        take_interop_view
    >
);

static_assert(
    mixed_sized_sentinel_interoperable<
        drop_interop_view
    >
);

static_assert(
    mixed_sized_sentinel_interoperable<
        transform_interop_view
    >
);

static_assert(
    mixed_sized_sentinel_interoperable<
        stride_interop_view
    >
);

static_assert(
    mixed_sized_sentinel_interoperable<
        zip_interop_view
    >
);

static_assert(
    mixed_sized_sentinel_interoperable<
        zip_transform_interop_view
    >
);

static_assert(
    mixed_sized_sentinel_interoperable<
        enumerate_interop_view
    >
);

static_assert(
    mixed_sized_sentinel_interoperable<
        adjacent_interop_view
    >
);

static_assert(
    mixed_sized_sentinel_interoperable<
        adjacent_transform_interop_view
    >
);

static_assert(
    mixed_sized_sentinel_interoperable<
        slide_interop_view
    >
);

static_assert(
    mixed_sized_sentinel_interoperable<
        chunk_interop_view
    >
);

static_assert(
    mixed_sized_sentinel_interoperable<
        cartesian_product_interop_view
    >
);

/*
 * Random-access wrappers must support mixed-const ordering and
 * subtraction.
 */

static_assert(
    mixed_random_access_operations<
        take_interop_view
    >
);

static_assert(
    mixed_random_access_operations<
        drop_interop_view
    >
);

static_assert(
    mixed_random_access_operations<
        reverse_interop_view
    >
);

static_assert(
    mixed_random_access_operations<
        transform_interop_view
    >
);

static_assert(
    mixed_random_access_operations<
        common_interop_view
    >
);

static_assert(
    mixed_random_access_operations<
        stride_interop_view
    >
);

static_assert(
    mixed_random_access_operations<
        zip_interop_view
    >
);

static_assert(
    mixed_random_access_operations<
        zip_transform_interop_view
    >
);

static_assert(
    mixed_random_access_operations<
        enumerate_interop_view
    >
);

static_assert(
    mixed_random_access_operations<
        adjacent_interop_view
    >
);

static_assert(
    mixed_random_access_operations<
        adjacent_transform_interop_view
    >
);

static_assert(
    mixed_random_access_operations<
        slide_interop_view
    >
);

static_assert(
    mixed_random_access_operations<
        chunk_interop_view
    >
);

static_assert(
    mixed_random_access_operations<
        cartesian_product_interop_view
    >
);

/*
 * These views intentionally do not currently expose const
 * iteration. The absence must be represented cleanly by the
 * range concepts.
 */

using filter_nonconst_view =
    decltype(
        tested::ranges::views::filter(
            tested::declval<
                interop_common_view
            >(),
            unary_predicate{}
        )
    );

using drop_while_nonconst_view =
    decltype(
        tested::ranges::views::
            drop_while(
                tested::declval<
                    interop_common_view
                >(),
                unary_predicate{}
            )
    );

using chunk_by_nonconst_view =
    decltype(
        tested::ranges::views::chunk_by(
            tested::declval<
                interop_common_view
            >(),
            binary_predicate{}
        )
    );

static_assert(
    tested::ranges::range<
        filter_nonconst_view
    >
);

static_assert(
    !tested::ranges::range<
        const filter_nonconst_view
    >
);

static_assert(
    tested::ranges::range<
        drop_while_nonconst_view
    >
);

static_assert(
    !tested::ranges::range<
        const drop_while_nonconst_view
    >
);

static_assert(
    tested::ranges::range<
        chunk_by_nonconst_view
    >
);

static_assert(
    !tested::ranges::range<
        const chunk_by_nonconst_view
    >
);

/*
 * Runtime instantiation verifies that the converting
 * constructors and mixed sentinel operators are usable from
 * real begin()/end() expressions, not merely nameable.
 */

template<class View>
bool promoted_begin_matches(
    View& view
) {
    using const_iterator =
        tested::ranges::iterator_t<
            const View
        >;

    auto mutable_begin =
        tested::ranges::begin(view);

    const_iterator promoted =
        mutable_begin;

    const View& constant_view =
        view;

    const auto constant_begin =
        tested::ranges::begin(
            constant_view
        );

    const auto constant_end =
        tested::ranges::end(
            constant_view
        );

    return
        promoted ==
            constant_begin &&
        !(
            mutable_begin ==
            constant_end
        );
}

bool ftl_test() {
    int values[] = {
        1,
        2,
        3,
        4,
        5,
        6
    };

    interop_common_view common_base(
        values,
        values + 6
    );

    interop_sentinel_view sentinel_base(
        values,
        values + 6
    );

    auto taken =
        tested::ranges::views::take(
            sentinel_base,
            3
        );

    auto dropped =
        tested::ranges::views::drop(
            sentinel_base,
            1
        );

    auto taken_while =
        tested::ranges::views::
            take_while(
                sentinel_base,
                unary_predicate{}
            );

    auto reversed =
        tested::ranges::views::reverse(
            common_base
        );

    auto transformed =
        tested::ranges::views::transform(
            sentinel_base,
            identity_transform{}
        );

    auto common =
        tested::ranges::views::common(
            sentinel_base
        );

    auto strided =
        tested::ranges::views::stride(
            sentinel_base,
            2
        );

    auto zipped =
        tested::ranges::views::zip(
            common_base,
            sentinel_base
        );

    auto zip_transformed =
        tested::ranges::views::
            zip_transform(
                binary_transform{},
                common_base,
                sentinel_base
            );

    auto enumerated =
        tested::ranges::views::enumerate(
            sentinel_base
        );

    auto adjacent =
        tested::ranges::views::adjacent<2>(
            sentinel_base
        );

    auto adjacent_transformed =
        tested::ranges::views::
            adjacent_transform<2>(
                sentinel_base,
                binary_transform{}
            );

    auto slid =
        tested::ranges::views::slide(
            sentinel_base,
            2
        );

    auto chunked =
        tested::ranges::views::chunk(
            sentinel_base,
            2
        );

    auto product =
        tested::ranges::views::
            cartesian_product(
                sentinel_base,
                common_base
            );

    int nested[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };

    auto joined =
        tested::ranges::views::join(
            nested
        );

    auto lazily_split =
        tested::ranges::views::
            lazy_split(
                common_base,
                3
            );

    return
        promoted_begin_matches(taken) &&
        promoted_begin_matches(dropped) &&
        promoted_begin_matches(
            taken_while
        ) &&
        promoted_begin_matches(reversed) &&
        promoted_begin_matches(
            transformed
        ) &&
        promoted_begin_matches(common) &&
        promoted_begin_matches(strided) &&
        promoted_begin_matches(zipped) &&
        promoted_begin_matches(
            zip_transformed
        ) &&
        promoted_begin_matches(
            enumerated
        ) &&
        promoted_begin_matches(adjacent) &&
        promoted_begin_matches(
            adjacent_transformed
        ) &&
        promoted_begin_matches(slid) &&
        promoted_begin_matches(chunked) &&
        promoted_begin_matches(product) &&
        promoted_begin_matches(joined) &&
        promoted_begin_matches(
            lazily_split
        );
}