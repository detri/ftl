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

struct input_test_iterator {
    using iterator_concept =
        tested::input_iterator_tag;

    using iterator_category =
        tested::input_iterator_tag;

    using value_type = int;
    using difference_type =
        tested::ptrdiff_t;

    int* current = nullptr;

    constexpr int& operator*() const {
        return *current;
    }

    constexpr input_test_iterator&
    operator++() {
        ++current;
        return *this;
    }

    constexpr void operator++(int) {
        ++current;
    }

    friend constexpr bool operator==(
        const input_test_iterator&,
        const input_test_iterator&
    ) = default;
};

struct forward_test_iterator {
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

    constexpr forward_test_iterator&
    operator++() {
        ++current;
        return *this;
    }

    constexpr forward_test_iterator
    operator++(int) {
        auto previous = *this;
        ++*this;
        return previous;
    }

    friend constexpr bool operator==(
        const forward_test_iterator&,
        const forward_test_iterator&
    ) = default;
};

struct forward_sized_sentinel {
    int* last = nullptr;

    friend constexpr bool operator==(
        forward_test_iterator iterator,
        forward_sized_sentinel sentinel
    ) {
        return
            iterator.current ==
            sentinel.last;
    }

    friend constexpr tested::ptrdiff_t
    operator-(
        forward_sized_sentinel sentinel,
        forward_test_iterator iterator
    ) {
        return
            sentinel.last -
            iterator.current;
    }

    friend constexpr tested::ptrdiff_t
    operator-(
        forward_test_iterator iterator,
        forward_sized_sentinel sentinel
    ) {
        return
            iterator.current -
            sentinel.last;
    }
};

struct non_simple_view
    : tested::ranges::view_base
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr non_simple_view() = default;

    constexpr non_simple_view(
        int* first,
        int* last
    )
        : first(first),
          last(last) {}

    constexpr int* begin() {
        return first;
    }

    constexpr const int* begin() const {
        return first;
    }

    constexpr int* end() {
        return last;
    }

    constexpr const int* end() const {
        return last;
    }

    constexpr tested::size_t size() const {
        return static_cast<
            tested::size_t
        >(last - first);
    }
};

template<class T>
concept has_member_iterator_category =
    requires {
        typename T::iterator_category;
    };

using empty_zip_result =
    decltype(
        tested::ranges::views::zip()
    );

static_assert(tested::same_as<
    empty_zip_result,
    tested::ranges::empty_view<
        tested::tuple<>
    >
>);

using input_counted_range =
    decltype(
        tested::ranges::views::counted(
            tested::declval<
                input_test_iterator
            >(),
            tested::ptrdiff_t{2}
        )
    );

using input_zip_view =
    decltype(
        tested::ranges::views::zip(
            tested::declval<
                input_counted_range&
            >()
        )
    );

using input_zip_iterator =
    tested::ranges::iterator_t<
        input_zip_view
    >;

static_assert(
    tested::input_iterator<
        input_zip_iterator
    >
);

static_assert(
    !tested::forward_iterator<
        input_zip_iterator
    >
);

static_assert(
    !has_member_iterator_category<
        input_zip_iterator
    >
);

using mutable_zip_view =
    decltype(
        tested::ranges::views::zip(
            tested::declval<
                non_simple_view&
            >()
        )
    );

using mutable_zip_iterator =
    decltype(
        tested::declval<
            mutable_zip_view&
        >().begin()
    );

using const_zip_iterator =
    decltype(
        tested::declval<
            const mutable_zip_view&
        >().begin()
    );

static_assert(
    tested::convertible_to<
        mutable_zip_iterator,
        const_zip_iterator
    >
);

static_assert(
    !tested::convertible_to<
        const_zip_iterator,
        mutable_zip_iterator
    >
);

using non_common_range =
    tested::ranges::subrange<
        forward_test_iterator,
        forward_sized_sentinel
    >;

using non_common_zip =
    decltype(
        tested::ranges::views::zip(
            tested::declval<
                non_common_range&
            >(),
            tested::declval<
                non_common_range&
            >()
        )
    );

static_assert(
    !tested::ranges::common_range<
        non_common_zip
    >
);

static_assert(
    tested::ranges::sized_range<
        non_common_zip
    >
);

constexpr bool zip_works() {
    int left[] = {1, 2, 3};
    char right[] = {'a', 'b'};

    auto view =
        tested::ranges::views::zip(
            left,
            right
        );

    static_assert(
        tested::ranges::common_range<
            decltype(view)
        >
    );

    static_assert(
        tested::ranges::random_access_range<
            decltype(view)
        >
    );

    if (view.size() != 2)
        return false;

    if (
        view.end() -
        view.begin() != 2
    )
        return false;

    auto first = *view.begin();

    if (
        tested::get<0>(first) != 1 ||
        tested::get<1>(first) != 'a'
    )
        return false;

    tested::get<0>(first) = 7;

    if (left[0] != 7)
        return false;

    auto second = view.begin()[1];

    if (
        tested::get<0>(second) != 2 ||
        tested::get<1>(second) != 'b'
    )
        return false;

    tested::get<1>(second) = 'z';

    if (right[1] != 'z')
        return false;

    auto moved =
        tested::ranges::iter_move(
            view.begin()
        );

    static_assert(tested::same_as<
        decltype(moved),
        tested::tuple<
            int&&,
            char&&
        >
    >);

    if (
        tested::get<0>(moved) != 7 ||
        tested::get<1>(moved) != 'a'
    )
        return false;

    tested::ranges::iter_swap(
        view.begin(),
        view.begin() + 1
    );

    if (
        left[0] != 2 ||
        left[1] != 7 ||
        right[0] != 'z' ||
        right[1] != 'a'
    )
        return false;

    if (
        !(
            view.begin() <
            view.begin() + 1
        )
    )
        return false;

    int short_values[] = {10, 20};
    int long_values[] = {30, 40, 50};

    non_common_range short_range(
        forward_test_iterator{
            short_values
        },
        forward_sized_sentinel{
            short_values + 2
        }
    );

    non_common_range long_range(
        forward_test_iterator{
            long_values
        },
        forward_sized_sentinel{
            long_values + 3
        }
    );

    auto non_common =
        tested::ranges::views::zip(
            short_range,
            long_range
        );

    if (
        non_common.end() -
        non_common.begin() != 2
    )
        return false;

    auto iterator = non_common.begin();

    ++iterator;
    ++iterator;

    if (iterator != non_common.end())
        return false;

    non_simple_view source{
        left,
        left + 3
    };

    auto interoperable =
        tested::ranges::views::zip(
            source
        );

    auto mutable_iterator =
        interoperable.begin();

    const auto& const_interoperable =
        interoperable;

    auto converted =
        const_zip_iterator(
            mutable_iterator
        );

    if (
        tested::get<0>(*converted) !=
        tested::get<0>(
            *const_interoperable.begin()
        )
    )
        return false;

    return true;
}

static_assert(zip_works());

bool ftl_test() {
    return zip_works();
}