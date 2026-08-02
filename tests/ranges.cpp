#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <iterator>
#include <ranges>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct member_range {
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

    constexpr bool empty() const noexcept {
        return first == last;
    }

    constexpr int* data() noexcept {
        return first;
    }

    constexpr const int* data() const noexcept {
        return first;
    }
};

struct subtraction_range {
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
};

struct iterator_empty_range {
    int* first = nullptr;
    int* last = nullptr;

    constexpr int* begin() noexcept {
        return first;
    }

    constexpr int* end() noexcept {
        return last;
    }
};

struct borrowed_member_range {
    int* first = nullptr;
    int* last = nullptr;

    constexpr int* begin() noexcept {
        return first;
    }

    constexpr int* end() noexcept {
        return last;
    }
};

struct explicitly_unsized_range {
    int* first = nullptr;
    int* last = nullptr;

    constexpr tested::size_t size() const noexcept {
        return 999;
    }

    constexpr int* begin() noexcept {
        return first;
    }

    constexpr int* end() noexcept {
        return last;
    }
};

namespace adl_test {

struct range {
    int* first = nullptr;
    int* last = nullptr;
};

constexpr int* begin(range& value) noexcept {
    return value.first;
}

constexpr const int* begin(
    const range& value
) noexcept {
    return value.first;
}

constexpr int* end(range& value) noexcept {
    return value.last;
}

constexpr const int* end(
    const range& value
) noexcept {
    return value.last;
}

constexpr tested::size_t size(
    const range& value
) noexcept {
    return static_cast<tested::size_t>(
        value.last - value.first
    );
}

} // namespace adl_test

namespace reverse_adl_test {

struct range {
    int* first = nullptr;
    int* last = nullptr;
};

constexpr auto rbegin(range& value) noexcept {
    return tested::reverse_iterator<int*>(
        value.last
    );
}

constexpr auto rend(range& value) noexcept {
    return tested::reverse_iterator<int*>(
        value.first
    );
}

} // namespace reverse_adl_test

#ifdef FTL_REPLACE_STL

namespace std::ranges {

template<>
inline constexpr bool enable_borrowed_range<
    ::borrowed_member_range
> = true;

template<>
inline constexpr bool disable_sized_range<
    ::explicitly_unsized_range
> = true;

} // namespace std::ranges

#else

namespace ftl::ranges {

template<>
inline constexpr bool enable_borrowed_range<
    ::borrowed_member_range
> = true;

template<>
inline constexpr bool disable_sized_range<
    ::explicitly_unsized_range
> = true;

} // namespace ftl::ranges

#endif

template<class T>
concept can_begin_rvalue =
    requires {
        tested::ranges::begin(T{});
    };

template<class T>
concept can_end_rvalue =
    requires {
        tested::ranges::end(T{});
    };

template<class T>
concept can_data_rvalue =
    requires {
        tested::ranges::data(T{});
    };

template<class T>
concept can_size =
    requires(T& value) {
        tested::ranges::size(value);
    };

static_assert(
    tested::ranges::range<member_range>
);

static_assert(
    tested::ranges::range<adl_test::range>
);

static_assert(
    tested::ranges::input_range<member_range>
);

static_assert(
    tested::ranges::forward_range<member_range>
);

static_assert(
    tested::ranges::bidirectional_range<member_range>
);

static_assert(
    tested::ranges::random_access_range<member_range>
);

static_assert(
    tested::ranges::common_range<member_range>
);

static_assert(
    tested::ranges::sized_range<member_range>
);

static_assert(
    tested::ranges::sized_range<subtraction_range>
);

static_assert(
    tested::ranges::borrowed_range<member_range&>
);

static_assert(
    !tested::ranges::borrowed_range<member_range>
);

static_assert(
    tested::ranges::borrowed_range<
        borrowed_member_range
    >
);

static_assert(
    !can_begin_rvalue<member_range>
);

static_assert(
    !can_end_rvalue<member_range>
);

static_assert(
    !can_data_rvalue<member_range>
);

static_assert(
    can_begin_rvalue<borrowed_member_range>
);

static_assert(
    can_end_rvalue<borrowed_member_range>
);

static_assert(
    can_data_rvalue<borrowed_member_range>
);

static_assert(
    tested::is_same_v<
        tested::ranges::iterator_t<member_range>,
        int*
    >
);

static_assert(
    tested::is_same_v<
        tested::ranges::sentinel_t<member_range>,
        int*
    >
);

static_assert(
    tested::is_same_v<
        tested::ranges::range_value_t<member_range>,
        int
    >
);

static_assert(
    tested::is_same_v<
        tested::ranges::range_reference_t<member_range>,
        int&
    >
);

static_assert(
    tested::is_same_v<
        tested::ranges::range_rvalue_reference_t<
            member_range
        >,
        int&&
    >
);

static_assert(
    tested::is_same_v<
        tested::ranges::range_difference_t<
            member_range
        >,
        tested::ptrdiff_t
    >
);

static_assert(
    tested::is_same_v<
        tested::ranges::borrowed_iterator_t<
            member_range
        >,
        tested::ranges::dangling
    >
);

static_assert(
    tested::is_same_v<
        tested::ranges::borrowed_iterator_t<
            member_range&
        >,
        int*
    >
);

static_assert(
    tested::is_same_v<
        tested::ranges::borrowed_iterator_t<
            borrowed_member_range
        >,
        int*
    >
);

static_assert(
    tested::is_default_constructible_v<
        tested::ranges::dangling
    >
);

static_assert(
    tested::is_constructible_v<
        tested::ranges::dangling,
        int*,
        int*
    >
);

static_assert(
    tested::ranges::sized_range<
        explicitly_unsized_range
    >
);

constexpr bool disabled_member_size_works() {
    int values[] = {1, 2, 3, 4};

    explicitly_unsized_range value{
        values,
        values + 4
    };

    return tested::ranges::size(value) == 4;
}

static_assert(disabled_member_size_works());

constexpr bool ranges_constexpr_works() {
    int values[] = {1, 2, 3, 4};

    if (tested::ranges::begin(values) != values)
        return false;

    if (tested::ranges::end(values) != values + 4)
        return false;

    if (tested::ranges::size(values) != 4)
        return false;

    if (tested::ranges::ssize(values) != 4)
        return false;

    if (tested::ranges::empty(values))
        return false;

    if (tested::ranges::data(values) != values)
        return false;

    if (tested::ranges::cdata(values) != values)
        return false;

    member_range member{
        values,
        values + 4
    };

    if (tested::ranges::begin(member) != values)
        return false;

    if (tested::ranges::end(member) != values + 4)
        return false;

    if (tested::ranges::size(member) != 4)
        return false;

    if (tested::ranges::ssize(member) != 4)
        return false;

    if (tested::ranges::empty(member))
        return false;

    if (tested::ranges::data(member) != values)
        return false;

    if (tested::ranges::cdata(member) != values)
        return false;

    const member_range constant_member{
        values,
        values + 4
    };

    if (
        tested::ranges::begin(constant_member) !=
        values
    ) {
        return false;
    }

    if (
        tested::ranges::end(constant_member) !=
        values + 4
    ) {
        return false;
    }

    if (
        tested::ranges::cbegin(member) !=
        values
    ) {
        return false;
    }

    if (
        tested::ranges::cend(member) !=
        values + 4
    ) {
        return false;
    }

    subtraction_range subtraction{
        values,
        values + 4
    };

    if (
        tested::ranges::size(subtraction) != 4
    ) {
        return false;
    }

    if (
        tested::ranges::data(subtraction) !=
        values
    ) {
        return false;
    }

    iterator_empty_range empty_range{
        values,
        values
    };

    if (!tested::ranges::empty(empty_range))
        return false;

    iterator_empty_range nonempty_range{
        values,
        values + 4
    };

    if (tested::ranges::empty(nonempty_range))
        return false;

    adl_test::range adl{
        values,
        values + 4
    };

    if (tested::ranges::begin(adl) != values)
        return false;

    if (tested::ranges::end(adl) != values + 4)
        return false;

    if (tested::ranges::size(adl) != 4)
        return false;

    if (*tested::ranges::rbegin(values) != 4)
        return false;

    if (
        *(tested::ranges::rend(values) - 1) !=
        1
    ) {
        return false;
    }

    if (
        *tested::ranges::crbegin(values) !=
        4
    ) {
        return false;
    }

    if (
        *(tested::ranges::crend(values) - 1) !=
        1
    ) {
        return false;
    }

    return true;
}

static_assert(ranges_constexpr_works());

bool ftl_test() {
    int values[] = {1, 2, 3, 4};

    borrowed_member_range borrowed{
        values,
        values + 4
    };

    if (
        tested::ranges::begin(
            static_cast<
                borrowed_member_range&&
            >(borrowed)
        ) != values
    ) {
        return false;
    }

    if (
        tested::ranges::end(
            static_cast<
                borrowed_member_range&&
            >(borrowed)
        ) != values + 4
    ) {
        return false;
    }

    reverse_adl_test::range reversed{
        values,
        values + 4
    };

    if (
        *tested::ranges::rbegin(reversed) != 4
    ) {
        return false;
    }

    if (
        *(tested::ranges::rend(reversed) - 1) !=
        1
    ) {
        return false;
    }

    tested::ranges::dangling ignored{
        values,
        values + 4
    };

    (void) ignored;

    return ranges_constexpr_works();
}
