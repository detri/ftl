#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/initializer_list>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct member_range
{
    int* first = nullptr;
    int* last = nullptr;

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
        return static_cast<tested::size_t>(last - first);
    }

    constexpr bool empty() const noexcept
    {
        return first == last;
    }

    constexpr int* data() noexcept
    {
        return first;
    }

    constexpr const int* data() const noexcept
    {
        return first;
    }
};

struct subtraction_range
{
    int* first = nullptr;
    int* last = nullptr;

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
};

struct iterator_empty_range
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr int* begin() noexcept
    {
        return first;
    }

    constexpr int* end() noexcept
    {
        return last;
    }
};

struct borrowed_member_range
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr int* begin() noexcept
    {
        return first;
    }

    constexpr int* end() noexcept
    {
        return last;
    }
};

struct explicitly_unsized_range
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr tested::size_t size() const noexcept
    {
        return 999;
    }

    constexpr int* begin() noexcept
    {
        return first;
    }

    constexpr int* end() noexcept
    {
        return last;
    }
};

struct move_only_range
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr move_only_range() = default;

    constexpr move_only_range(int* begin_value, int* end_value) noexcept
        : first(begin_value), last(end_value) {}

    move_only_range(const move_only_range&) = delete;

    move_only_range& operator=(const move_only_range&) = delete;

    constexpr move_only_range(move_only_range&&) noexcept = default;

    constexpr move_only_range& operator=(move_only_range&&) noexcept = default;

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
        return static_cast<tested::size_t>(last - first);
    }

    constexpr int* data() noexcept
    {
        return first;
    }

    constexpr const int* data() const noexcept
    {
        return first;
    }
};

struct interface_view
        : tested::ranges::view_interface<interface_view>
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr interface_view() = default;

    constexpr interface_view(int* begin_value, int* end_value) noexcept
        : first(begin_value), last(end_value) {}

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
};

struct base_view : tested::ranges::view_base
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr int* begin() noexcept
    {
        return first;
    }

    constexpr int* end() noexcept
    {
        return last;
    }
};

struct opted_view
{
    int* first = nullptr;
    int* last = nullptr;

    constexpr int* begin() noexcept
    {
        return first;
    }

    constexpr int* end() noexcept
    {
        return last;
    }
};

namespace adl_test
{
    struct range
    {
        int* first = nullptr;
        int* last = nullptr;
    };

    constexpr int* begin(range& value) noexcept
    {
        return value.first;
    }

    constexpr const int* begin(const range& value) noexcept
    {
        return value.first;
    }

    constexpr int* end(range& value) noexcept
    {
        return value.last;
    }

    constexpr const int* end(const range& value) noexcept
    {
        return value.last;
    }

    constexpr tested::size_t size(const range& value) noexcept
    {
        return static_cast<tested::size_t>(value.last - value.first);
    }
} // namespace adl_test

namespace reverse_adl_test
{
    struct range
    {
        int* first = nullptr;
        int* last = nullptr;
    };

    constexpr auto rbegin(range& value) noexcept
    {
        return tested::reverse_iterator<int*>(value.last);
    }

    constexpr auto rend(range& value) noexcept
    {
        return tested::reverse_iterator<int*>(value.first);
    }
} // namespace reverse_adl_test

#ifdef FTL_REPLACE_STL

namespace std::ranges
{
    template<>
    inline constexpr bool
    enable_borrowed_range<::borrowed_member_range> = true;

    template<>
    inline constexpr bool
    disable_sized_range<::explicitly_unsized_range> = true;

    template<>
    inline constexpr bool enable_view<::opted_view> = true;
} // namespace std::ranges

#else

namespace ftl::ranges
{
    template<>
    inline constexpr bool
    enable_borrowed_range<::borrowed_member_range> = true;

    template<>
    inline constexpr bool
    disable_sized_range<::explicitly_unsized_range> = true;

    template<>
    inline constexpr bool enable_view<::opted_view> = true;
} // namespace ftl::ranges

#endif

template<class T>
concept can_begin_rvalue =
        requires
        {
            tested::ranges::begin(T{});
        };

template<class T>
concept can_end_rvalue =
        requires
        {
            tested::ranges::end(T{});
        };

template<class T>
concept can_data_rvalue =
        requires
        {
            tested::ranges::data(T{});
        };

template<class T>
concept can_ref_view =
        requires(T&& value)
        {
            tested::ranges::ref_view{
                static_cast<T&&>(value)
            };
        };

static_assert(tested::ranges::range<member_range>);
static_assert(tested::ranges::input_range<member_range>);
static_assert(tested::ranges::forward_range<member_range>);
static_assert(tested::ranges::bidirectional_range<member_range>);
static_assert(tested::ranges::random_access_range<member_range>);
static_assert(tested::ranges::contiguous_range<member_range>);
static_assert(tested::ranges::common_range<member_range>);
static_assert(tested::ranges::sized_range<member_range>);
static_assert(tested::ranges::sized_range<subtraction_range>);
static_assert(tested::ranges::output_range<member_range, int>);

static_assert(tested::ranges::borrowed_range<member_range&>);
static_assert(!tested::ranges::borrowed_range<member_range>);
static_assert(
    tested::ranges::borrowed_range<borrowed_member_range>
);

static_assert(!can_begin_rvalue<member_range>);
static_assert(!can_end_rvalue<member_range>);
static_assert(!can_data_rvalue<member_range>);

static_assert(can_begin_rvalue<borrowed_member_range>);
static_assert(can_end_rvalue<borrowed_member_range>);
static_assert(can_data_rvalue<borrowed_member_range>);

static_assert(tested::is_same_v<
    tested::ranges::iterator_t<member_range>,
    int*>);

static_assert(tested::is_same_v<
    tested::ranges::sentinel_t<member_range>,
    int*>);

static_assert(tested::is_same_v<
    tested::ranges::const_iterator_t<member_range>,
    const int*>);

static_assert(tested::is_same_v<
    tested::ranges::const_sentinel_t<member_range>,
    const int*>);

static_assert(tested::is_same_v<
    tested::ranges::range_value_t<member_range>,
    int>);

static_assert(tested::is_same_v<
    tested::ranges::range_reference_t<member_range>,
    int&>);

static_assert(tested::is_same_v<
    tested::ranges::range_const_reference_t<member_range>,
    const int&>);

static_assert(tested::detail::has_common_ref<const int&, int&>);
static_assert(tested::detail::has_common_ref<const int&&, int&>);

static_assert(tested::is_same_v<
    tested::ranges::range_rvalue_reference_t<member_range>,
    int&&>);

static_assert(tested::is_same_v<
    tested::ranges::range_difference_t<member_range>,
    tested::ptrdiff_t
>);

static_assert(tested::is_same_v<
    tested::ranges::borrowed_iterator_t<member_range>,
    tested::ranges::dangling
>);

static_assert(tested::is_same_v<
    tested::ranges::borrowed_iterator_t<member_range&>,
    int*>);

static_assert(tested::is_default_constructible_v<
    tested::ranges::dangling
>);

static_assert(tested::is_constructible_v<
    tested::ranges::dangling,
    int*,
    int*>);

static_assert(tested::ranges::view<interface_view>);
static_assert(tested::ranges::view<base_view>);
static_assert(tested::ranges::view<opted_view>);
static_assert(!tested::ranges::view<member_range>);

static_assert(tested::ranges::viewable_range<member_range&>);
static_assert(tested::ranges::viewable_range<move_only_range>);
static_assert(!tested::ranges::viewable_range<
    tested::initializer_list<int>
>);

static_assert(can_ref_view<member_range&>);
static_assert(!can_ref_view<member_range>);

static_assert(tested::ranges::view<
    tested::ranges::ref_view<member_range>
>);

static_assert(tested::ranges::borrowed_range<
    tested::ranges::ref_view<member_range>
>);

static_assert(tested::ranges::view<
    tested::ranges::owning_view<move_only_range>
>);

static_assert(!tested::is_copy_constructible_v<
    tested::ranges::owning_view<move_only_range>
>);

static_assert(tested::is_move_constructible_v<
    tested::ranges::owning_view<move_only_range>
>);

static_assert(tested::ranges::view<
    tested::ranges::empty_view<int>
>);

static_assert(tested::ranges::borrowed_range<
    tested::ranges::empty_view<int>
>);

static_assert(tested::ranges::view<
    tested::ranges::single_view<int>
>);

static_assert(tested::is_same_v<
    tested::ranges::views::all_t<member_range&>,
    tested::ranges::ref_view<member_range>
>);

static_assert(tested::is_same_v<
    tested::ranges::views::all_t<move_only_range>,
    tested::ranges::owning_view<move_only_range>
>);

static_assert(tested::is_same_v<
    tested::ranges::views::all_t<interface_view>,
    interface_view
>);

static_assert(tested::contiguous_iterator<
    tested::ranges::iterator_t<interface_view>
>);

static_assert(tested::is_same_v<
    decltype(tested::ranges::data(
        tested::declval<interface_view&>()
    )),
    int*>);

static_assert(tested::is_same_v<
    decltype(tested::ranges::data(
        tested::declval<const interface_view&>()
    )),
    const int*>);

static_assert(tested::is_same_v<
    tested::add_pointer_t<
        tested::ranges::range_reference_t<interface_view>
    >,
    int*>);

static_assert(tested::ranges::contiguous_range<interface_view>);
static_assert(tested::ranges::contiguous_range<const interface_view>);

constexpr bool range_iterator_operations_work()
{
    int values[] = {1, 2, 3, 4};
    int* iterator = values;

    tested::ranges::advance(iterator, 2);
    if (iterator != values + 2)
        return false;

    tested::ranges::advance(iterator, -1);
    if (iterator != values + 1)
        return false;

    tested::ranges::advance(iterator, values + 4);
    if (iterator != values + 4)
        return false;

    iterator = values;

    auto remainder = tested::ranges::advance(
        iterator,
        tested::ptrdiff_t{8},
        values + 4
    );

    if (iterator != values + 4 || remainder != 4)
        return false;

    if (tested::ranges::distance(values, values + 4) != 4)
        return false;

    member_range range{values, values + 4};

    if (tested::ranges::distance(range) != 4)
        return false;

    if (tested::ranges::next(values) != values + 1)
        return false;

    if (tested::ranges::next(values, 3) != values + 3)
        return false;

    if (tested::ranges::next(values, values + 4) != values + 4)
        return false;

    if (tested::ranges::prev(values + 4) != values + 3)
        return false;

    if (tested::ranges::prev(values + 4, 2) != values + 2)
        return false;

    return true;
}

static_assert(range_iterator_operations_work());

constexpr bool ranges_constexpr_works()
{
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

    member_range member{values, values + 4};

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

    if (tested::ranges::cbegin(member) != values)
        return false;

    if (tested::ranges::cend(member) != values + 4)
        return false;

    const member_range constant_member{values, values + 4};

    if (tested::ranges::begin(constant_member) != values)
        return false;

    if (tested::ranges::end(constant_member) != values + 4)
        return false;

    subtraction_range subtraction{values, values + 4};

    if (tested::ranges::size(subtraction) != 4)
        return false;

    if (tested::ranges::data(subtraction) != values)
        return false;

    explicitly_unsized_range disabled{values, values + 4};

    // disable_sized_range suppresses the member size(), but pointer subtraction
    // still makes this a sized range.
    if (tested::ranges::size(disabled) != 4)
        return false;

    iterator_empty_range empty_range{values, values};

    if (!tested::ranges::empty(empty_range))
        return false;

    iterator_empty_range nonempty_range{values, values + 4};

    if (tested::ranges::empty(nonempty_range))
        return false;

    adl_test::range adl{values, values + 4};

    if (tested::ranges::begin(adl) != values)
        return false;

    if (tested::ranges::end(adl) != values + 4)
        return false;

    if (tested::ranges::size(adl) != 4)
        return false;

    if (*tested::ranges::rbegin(values) != 4)
        return false;

    if (*(tested::ranges::rend(values) - 1) != 1)
        return false;

    if (*tested::ranges::crbegin(values) != 4)
        return false;

    if (*(tested::ranges::crend(values) - 1) != 1)
        return false;

    tested::ranges::ref_view reference{member};

    if (&reference.base() != &member)
        return false;

    if (reference.begin() != values)
        return false;

    if (reference.end() != values + 4)
        return false;

    if (reference.size() != 4)
        return false;

    if (reference.data() != values)
        return false;

    auto all_reference = tested::ranges::views::all(member);

    static_assert(tested::is_same_v<
        decltype(all_reference),
        tested::ranges::ref_view<member_range>
    >);

    if (all_reference.begin() != values)
        return false;

    if (all_reference.end() != values + 4)
        return false;

    interface_view existing{values, values + 4};

    auto all_existing = tested::ranges::views::all(
        static_cast<interface_view&&>(existing)
    );

    static_assert(tested::is_same_v<
        decltype(all_existing),
        interface_view
    >);

    if (all_existing.front() != 1)
        return false;

    if (all_existing.back() != 4)
        return false;

    if (all_existing[2] != 3)
        return false;

    if (all_existing.size() != 4)
        return false;

    if (all_existing.data() != values)
        return false;

    if (!static_cast<bool>(all_existing))
        return false;

    constexpr auto empty = tested::ranges::views::empty<int>;

    if (!empty.empty())
        return false;

    if (empty.size() != 0)
        return false;

    if (empty.begin() != nullptr)
        return false;

    if (empty.end() != nullptr)
        return false;

    if (empty.data() != nullptr)
        return false;

    auto single = tested::ranges::views::single(42);

    if (single.empty())
        return false;

    if (single.size() != 1)
        return false;

    if (*single.begin() != 42)
        return false;

    if (single.front() != 42)
        return false;

    if (single.back() != 42)
        return false;

    if (single[0] != 42)
        return false;

    tested::ranges::subrange sub(values, values + 4);

    if (sub.begin() != values)
        return false;

    if (sub.end() != values + 4)
        return false;

    if (sub.size() != 4)
        return false;

    auto next = sub.next(2);

    if (next.begin() != values + 2)
        return false;

    auto prev = next.prev(1);

    if (prev.begin() != values + 1)
        return false;

    return true;
}

static_assert(ranges_constexpr_works());

static_assert(tested::ranges::view<
    tested::ranges::subrange<int*>
>);

static_assert(tested::ranges::borrowed_range<
    tested::ranges::subrange<int*>
>);

static_assert(tested::is_same_v<
    decltype(tested::ranges::get<0>(
        tested::declval<const tested::ranges::subrange<int*>&>()
    )),
    int*>);

static_assert(tested::is_same_v<
    decltype(tested::ranges::get<1>(
        tested::declval<const tested::ranges::subrange<int*>&>()
    )),
    int*>);

struct identity_adaptor
        : tested::ranges::range_adaptor_closure<identity_adaptor>
{
    template<tested::ranges::range R>
    constexpr R&& operator()(R&& range) const
    {
        return static_cast<R&&>(range);
    }
};

static_assert(
    tested::ranges::detail::range_adaptor_closure_object<
        identity_adaptor
    >
);

constexpr bool pipeline_works()
{
    int values[] = {1, 2, 3, 4};

    identity_adaptor identity{};

    auto result =
            values | identity | identity;

    return result == values;
}

static_assert(pipeline_works());

struct move_only
{
    int value;

    constexpr move_only(int v)
        : value(v) {}

    move_only(const move_only&) = delete;

    move_only& operator=(const move_only&) = delete;

    constexpr move_only(move_only&&) = default;

    constexpr move_only& operator=(move_only&&) = default;
};

static_assert(
    tested::movable<move_only>
);

constexpr bool movable_box_works()
{
    tested::ranges::single_view<move_only> view(
        move_only{42}
    );

    return view.front().value == 42;
}

static_assert(movable_box_works());

static_assert(
    tested::ranges::borrowed_range<
        tested::ranges::owning_view<
            borrowed_member_range
        >
    >
);

constexpr bool view_interface_const_operations_work()
{
    int values[] = {1, 2, 3, 4};

    const interface_view view{values, values + 4};

    if (view.empty())
        return false;

    if (view.front() != 1)
        return false;

    if (view.back() != 4)
        return false;

    if (view[2] != 3)
        return false;

    if (view.size() != 4)
        return false;

    if (view.data() != values)
        return false;

    if (!static_cast<bool>(view))
        return false;

    return true;
}

static_assert(view_interface_const_operations_work());


// subrange: unsized sentinel storage

struct unsized_sentinel
{
    int* value = nullptr;

    constexpr unsized_sentinel() noexcept = default;

    constexpr explicit unsized_sentinel(int* pointer) noexcept
        : value(pointer) {}

    friend constexpr bool operator==(
        unsized_sentinel left,
        unsized_sentinel right
    ) noexcept
    {
        return left.value == right.value;
    }

    friend constexpr bool operator!=(
        unsized_sentinel left,
        unsized_sentinel right
    ) noexcept
    {
        return !(left == right);
    }

    friend constexpr bool operator==(
        int* iterator,
        unsized_sentinel sentinel
    ) noexcept
    {
        return iterator == sentinel.value;
    }

    friend constexpr bool operator==(
        unsized_sentinel sentinel,
        int* iterator
    ) noexcept
    {
        return sentinel.value == iterator;
    }

    friend constexpr bool operator!=(
        int* iterator,
        unsized_sentinel sentinel
    ) noexcept
    {
        return !(iterator == sentinel);
    }

    friend constexpr bool operator!=(
        unsized_sentinel sentinel,
        int* iterator
    ) noexcept
    {
        return !(sentinel == iterator);
    }
};

static_assert(
    tested::sentinel_for<
        unsized_sentinel,
        int*
    >
);

constexpr bool unsized_subrange_works()
{
    int values[] = {1, 2, 3, 4};

    tested::ranges::subrange<
        int*,
        unsized_sentinel,
        tested::ranges::subrange_kind::unsized
    > range(values, unsized_sentinel{values + 4});

    if (range.begin() != values)
        return false;

    if (range.end().value != values + 4)
        return false;

    if (range.empty())
        return false;

    return true;
}

static_assert(unsized_subrange_works());


// subrange: explicit size storage

constexpr bool sized_subrange_storage_works()
{
    int values[] = {1, 2, 3, 4};

    tested::ranges::subrange<
        int*,
        unsized_sentinel,
        tested::ranges::subrange_kind::sized
    > range(
        values,
        unsized_sentinel{values + 4},
        4
    );

    if (range.size() != 4)
        return false;

    range.advance(2);

    if (range.begin() != values + 2)
        return false;

    if (range.size() != 2)
        return false;

    return true;
}

static_assert(sized_subrange_storage_works());


// subrange tuple conversion

constexpr bool subrange_tuple_conversion_works()
{
    int values[] = {1, 2, 3};

    tested::ranges::subrange range(values, values + 3);

    auto pair =
        static_cast<
            tested::pair<int*, int*>
        >(range);

    return pair.first == values &&
           pair.second == values + 3;
}

static_assert(subrange_tuple_conversion_works());


// subrange structured binding support

static_assert(
    requires(const tested::ranges::subrange<int*>& range)
    {
        tested::ranges::get<0>(range);
        tested::ranges::get<1>(range);
    }
);

constexpr bool get_works()
{
    int values[] = {1,2,3};

    tested::ranges::subrange range(values, values+3);

    return tested::ranges::get<0>(range) == values &&
           tested::ranges::get<1>(range) == values+3;
}

static_assert(get_works());

static_assert(
    requires(tested::ranges::subrange<int*>& range)
    {
        get<0>(range);
        get<1>(range);
    }
);

static_assert(
    tested::tuple_size_v<
        tested::ranges::subrange<int*>
    > == 2
);

constexpr bool get_subrange_works()
{
    int values[] = {1, 2, 3};

    tested::ranges::subrange range(values, values + 3);

    return get<0>(range) == values &&
           get<1>(range) == values + 3;
}

static_assert(get_subrange_works());

static_assert(tested::is_same_v<
    typename tested::tuple_element_t<
        0,
        tested::ranges::subrange<int*>
    >,
    int*
>);

static_assert(tested::is_same_v<
    typename tested::tuple_element_t<
        1,
        tested::ranges::subrange<int*>
    >,
    int*
>);

static_assert(
    tested::tuple_size_v<
        tested::ranges::subrange<int*>
    > == 2
);

static_assert(tested::is_same_v<
    tested::tuple_element_t<
        0,
        tested::ranges::subrange<int*>
    >,
    int*
>);

#ifdef FTL_REPLACE_STL

constexpr bool subrange_structured_binding_works()
{
    int values[] = {1, 2, 3};

    tested::ranges::subrange range(values, values + 3);

    auto [first, last] = range;

    return first == values &&
           last == values + 3;
}

static_assert(subrange_structured_binding_works());

#endif


// ref_view const behavior

constexpr bool ref_view_const_behavior_works()
{
    int values[] = {1, 2, 3};

    member_range range{values, values + 3};

    const tested::ranges::ref_view<
        member_range
    > view(range);

    if (view.base().begin() != values)
        return false;

    if (view.begin() != values)
        return false;

    return true;
}

static_assert(ref_view_const_behavior_works());


// owning_view const access

constexpr bool owning_view_const_behavior_works()
{
    int values[] = {1, 2, 3};

    tested::ranges::owning_view<
        member_range
    > view(
        member_range{
            values,
            values + 3
        }
    );

    const auto& constant = view;

    if (constant.begin() != values)
        return false;

    if (constant.end() != values + 3)
        return false;

    return true;
}

static_assert(owning_view_const_behavior_works());


// movable_box move-only lifetime path

constexpr bool movable_box_move_only_path_works()
{
    tested::ranges::single_view<move_only> first(
        move_only{42}
    );

    auto second = static_cast<
        tested::ranges::single_view<move_only>&&
    >(first);

    return second.front().value == 42;
}

static_assert(movable_box_move_only_path_works());

bool ftl_test()
{
    int values[] = {1, 2, 3, 4};

    borrowed_member_range borrowed{values, values + 4};

    if (tested::ranges::begin(
            static_cast<borrowed_member_range&&>(borrowed)
        ) != values)
    {
        return false;
    }

    if (tested::ranges::end(
            static_cast<borrowed_member_range&&>(borrowed)
        ) != values + 4)
    {
        return false;
    }

    reverse_adl_test::range reversed{values, values + 4};

    if (*tested::ranges::rbegin(reversed) != 4)
        return false;

    if (*(tested::ranges::rend(reversed) - 1) != 1)
        return false;

    move_only_range movable{values, values + 4};

    auto owned = tested::ranges::views::all(
        static_cast<move_only_range&&>(movable)
    );

    static_assert(tested::is_same_v<
        decltype(owned),
        tested::ranges::owning_view<move_only_range>
    >);

    if (owned.begin() != values)
        return false;

    if (owned.end() != values + 4)
        return false;

    if (owned.size() != 4)
        return false;

    if (owned.data() != values)
        return false;

    auto moved_base =
            static_cast<decltype(owned)&&>(owned).base();

    if (moved_base.begin() != values)
        return false;

    if (moved_base.end() != values + 4)
        return false;

    tested::ranges::single_view<int> in_place_single(
        tested::in_place,
        7
    );

    if (in_place_single.front() != 7)
        return false;

    if (in_place_single.back() != 7)
        return false;

    tested::ranges::dangling ignored{values, values + 4};
    (void) ignored;

    return ranges_constexpr_works() &&
           range_iterator_operations_work();
}
