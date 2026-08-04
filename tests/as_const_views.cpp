#ifdef FTL_REPLACE_STL
#include <ranges>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/ranges>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

constexpr bool as_const_works() {
    int values[] = {1, 2, 3};
    auto view = values | tested::ranges::views::as_const;
    static_assert(tested::same_as<tested::ranges::range_reference_t<decltype(view)>, const int&>);
    return view.size() == 3 && *view.begin() == 1 && view[2] == 3;
}

using already_const = decltype(tested::ranges::views::all(tested::declval<const int(&)[3]>()));
static_assert(tested::same_as<
    decltype(tested::ranges::views::as_const(tested::declval<already_const>())),
    already_const>);
static_assert(tested::convertible_to<
    tested::basic_const_iterator<int*>,
    tested::basic_const_iterator<const int*>>);
static_assert(requires(tested::basic_const_iterator<int*> mutable_iterator,
    tested::basic_const_iterator<const int*> const_iterator) {
    { mutable_iterator == const_iterator } -> tested::same_as<bool>;
    { mutable_iterator < const_iterator } -> tested::same_as<bool>;
    mutable_iterator <=> const_iterator;
    mutable_iterator - const_iterator;
});

static_assert(as_const_works());
bool ftl_test() { return as_const_works(); }
