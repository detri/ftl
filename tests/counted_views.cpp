#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <iterator>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/span>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct random_access_test_iterator {
    using iterator_concept =
        tested::random_access_iterator_tag;

    using iterator_category =
        tested::random_access_iterator_tag;

    using value_type = int;
    using difference_type = tested::ptrdiff_t;
    using pointer = int*;
    using reference = int&;

    int* current = nullptr;

    constexpr int& operator*() const noexcept {
        return *current;
    }

    constexpr int* operator->() const noexcept {
        return current;
    }

    constexpr random_access_test_iterator&
    operator++() noexcept {
        ++current;
        return *this;
    }

    constexpr random_access_test_iterator
    operator++(int) noexcept {
        auto copy = *this;
        ++*this;
        return copy;
    }

    constexpr random_access_test_iterator&
    operator--() noexcept {
        --current;
        return *this;
    }

    constexpr random_access_test_iterator
    operator--(int) noexcept {
        auto copy = *this;
        --*this;
        return copy;
    }

    constexpr random_access_test_iterator&
    operator+=(difference_type offset) noexcept {
        current += offset;
        return *this;
    }

    constexpr random_access_test_iterator&
    operator-=(difference_type offset) noexcept {
        current -= offset;
        return *this;
    }

    constexpr int& operator[](
        difference_type offset
    ) const noexcept {
        return current[offset];
    }

    friend constexpr bool operator==(
        const random_access_test_iterator&,
        const random_access_test_iterator&
    ) = default;

    friend constexpr bool operator<(
        random_access_test_iterator left,
        random_access_test_iterator right
    ) noexcept {
        return left.current < right.current;
    }

    friend constexpr bool operator>(
        random_access_test_iterator left,
        random_access_test_iterator right
    ) noexcept {
        return right < left;
    }

    friend constexpr bool operator<=(
        random_access_test_iterator left,
        random_access_test_iterator right
    ) noexcept {
        return !(right < left);
    }

    friend constexpr bool operator>=(
        random_access_test_iterator left,
        random_access_test_iterator right
    ) noexcept {
        return !(left < right);
    }

    friend constexpr random_access_test_iterator
    operator+(
        random_access_test_iterator iterator,
        difference_type offset
    ) noexcept {
        iterator += offset;
        return iterator;
    }

    friend constexpr random_access_test_iterator
    operator+(
        difference_type offset,
        random_access_test_iterator iterator
    ) noexcept {
        iterator += offset;
        return iterator;
    }

    friend constexpr random_access_test_iterator
    operator-(
        random_access_test_iterator iterator,
        difference_type offset
    ) noexcept {
        iterator -= offset;
        return iterator;
    }

    friend constexpr difference_type operator-(
        random_access_test_iterator left,
        random_access_test_iterator right
    ) noexcept {
        return left.current - right.current;
    }
};

struct input_test_iterator {
    using iterator_concept =
        tested::input_iterator_tag;

    using iterator_category =
        tested::input_iterator_tag;

    using value_type = int;
    using difference_type = tested::ptrdiff_t;
    using pointer = int*;
    using reference = int&;

    int* current = nullptr;

    constexpr int& operator*() const noexcept {
        return *current;
    }

    constexpr input_test_iterator&
    operator++() noexcept {
        ++current;
        return *this;
    }

    constexpr void operator++(int) noexcept {
        ++current;
    }

    friend constexpr bool operator==(
        const input_test_iterator&,
        const input_test_iterator&
    ) = default;
};

static_assert(
    tested::random_access_iterator<
        random_access_test_iterator
    >
);

static_assert(
    !tested::contiguous_iterator<
        random_access_test_iterator
    >
);

static_assert(
    tested::input_iterator<
        input_test_iterator
    >
);

using contiguous_result = decltype(
    tested::ranges::views::counted(
        tested::declval<int*>(),
        tested::ptrdiff_t{2}
    )
);

using random_access_result = decltype(
    tested::ranges::views::counted(
        tested::declval<
            random_access_test_iterator
        >(),
        tested::ptrdiff_t{2}
    )
);

using input_result = decltype(
    tested::ranges::views::counted(
        tested::declval<
            input_test_iterator
        >(),
        tested::ptrdiff_t{2}
    )
);

static_assert(tested::same_as<
    contiguous_result,
    tested::span<int>
>);

static_assert(tested::same_as<
    random_access_result,
    tested::ranges::subrange<
        random_access_test_iterator,
        random_access_test_iterator
    >
>);

static_assert(tested::same_as<
    input_result,
    tested::ranges::subrange<
        tested::counted_iterator<
            input_test_iterator
        >,
        tested::default_sentinel_t
    >
>);

template<class Iterator, class Count>
concept can_count =
    requires(
        Iterator&& iterator,
        Count&& count
    ) {
        tested::ranges::views::counted(
            static_cast<Iterator&&>(
                iterator
            ),
            static_cast<Count&&>(
                count
            )
        );
    };

struct invalid_count {};

static_assert(
    can_count<int*, int>
);

static_assert(
    can_count<int*, short>
);

static_assert(
    !can_count<int*, invalid_count>
);

static_assert(
    !can_count<int, int>
);

constexpr bool counted_works() {
    int values[] = {1, 2, 3, 4};

    auto contiguous =
        tested::ranges::views::counted(
            values + 1,
            2
        );

    if (contiguous.data() != values + 1)
        return false;

    if (contiguous.size() != 2)
        return false;

    if (contiguous[0] != 2 ||
        contiguous[1] != 3)
        return false;

    auto random_access =
        tested::ranges::views::counted(
            random_access_test_iterator{
                values + 1
            },
            2
        );

    if (random_access.size() != 2)
        return false;

    if (*random_access.begin() != 2)
        return false;

    if (*(random_access.begin() + 1) != 3)
        return false;

    if (random_access.end() -
            random_access.begin() != 2)
        return false;

    auto input =
        tested::ranges::views::counted(
            input_test_iterator{
                values + 1
            },
            2
        );

    if (input.size() != 2)
        return false;

    auto iterator = input.begin();

    if (*iterator != 2)
        return false;

    ++iterator;

    if (*iterator != 3)
        return false;

    ++iterator;

    if (iterator != input.end())
        return false;

    auto empty_contiguous =
        tested::ranges::views::counted(
            values,
            0
        );

    auto empty_random_access =
        tested::ranges::views::counted(
            random_access_test_iterator{
                values
            },
            0
        );

    auto empty_input =
        tested::ranges::views::counted(
            input_test_iterator{
                values
            },
            0
            );

    return
        empty_contiguous.empty() &&
        empty_random_access.empty() &&
        empty_input.empty();
}

using input_result_iterator =
    tested::ranges::iterator_t<input_result>;

static_assert(tested::same_as<
    decltype(
        tested::declval<input_result_iterator&>()++
    ),
    void
>);

static_assert(counted_works());

bool ftl_test() {
    return counted_works();
}