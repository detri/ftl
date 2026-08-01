#ifdef FTL_REPLACE_STL
#include <utility>
namespace tested = std;
#else
#include <ftl/utility>
namespace tested = ftl;
#endif

#if __cpp_lib_forward_like != 202207L || __cpp_lib_unreachable != 202202L
#error invalid <utility> feature-test macros
#endif

enum class value : unsigned char { one = 1 };

static_assert(tested::to_underlying(value::one) == 1);
static_assert(tested::cmp_equal(-1, -1LL));
static_assert(!tested::cmp_equal(-1, 1U));
static_assert(tested::cmp_less(-1, 0U));
static_assert(tested::cmp_not_equal(1, 2U));
static_assert(tested::cmp_greater(2U, 1));
static_assert(tested::cmp_less_equal(2, 2U));
static_assert(tested::cmp_greater_equal(2U, 2));
static_assert(tested::in_range<unsigned char>(255));
static_assert(!tested::in_range<unsigned char>(256));
static_assert(!tested::in_range<unsigned>(-1));
static_assert(tested::in_range<signed char>(-128));
static_assert(!tested::in_range<signed char>(128));
static_assert(tested::make_index_sequence<4>::size() == 4);
static_assert(tested::is_same_v<tested::make_index_sequence<3>,
                                tested::index_sequence<0, 1, 2>>);

constexpr int forward_like_value = 4;
static_assert(tested::is_same_v<
              decltype(tested::forward_like<const int&>(forward_like_value)),
              const int&>);
static_assert(tested::is_same_v<decltype(tested::move(forward_like_value)),
                                const int&&>);
static_assert(tested::is_same_v<decltype(tested::as_const(forward_like_value)),
                                const int&>);
static_assert(tested::is_same_v<decltype(tested::declval<int&>()), int&>);

struct throwing_move {
    throwing_move(const throwing_move&) noexcept = default;
    throwing_move(throwing_move&&) noexcept(false) {}
};
static_assert(tested::is_same_v<
              decltype(tested::move_if_noexcept(
                  tested::declval<throwing_move&>())),
              const throwing_move&>);
static_assert(tested::is_same_v<decltype(tested::in_place),
                                const tested::in_place_t>);
static_assert(tested::is_same_v<decltype(tested::in_place_type<int>),
                                const tested::in_place_type_t<int>>);
static_assert(tested::is_same_v<decltype(tested::in_place_index<1>),
                                const tested::in_place_index_t<1>>);
static_assert(tested::is_same_v<decltype(tested::piecewise_construct),
                                const tested::piecewise_construct_t>);
static_assert(tested::tuple_size_v<tested::pair<int, long>> == 2);
static_assert(tested::is_same_v<
              tested::tuple_element_t<1, const tested::pair<int, long>>,
              const long>);
static_assert(tested::is_same_v<
              tested::common_type_t<tested::pair<int, short>,
                                    tested::pair<long, int>>,
              tested::pair<long, int>>);
static_assert(tested::is_constructible_v<tested::pair<int&, int&>,
                                         tested::pair<int, int>&>);
static_assert(!tested::is_constructible_v<tested::pair<int&, int&>,
                                          const tested::pair<int, int>&>);

constexpr bool pair_works() {
    tested::pair<int, long> first{1, 2};
    tested::get<long>(first) = 4;
    tested::pair<long, long> second = first;
    tested::get<0>(second) = 3;
    auto made = tested::make_pair(5L, 6L);
    tested::swap(second, made);
    return second == tested::pair<long, long>{5, 6} &&
           made == tested::pair<long, long>{3, 4} &&
           second > made;
}
static_assert(pair_works());

constexpr bool utility_works() {
    int a = 1, b = 2;
    tested::swap(a, b);
    return a == 2 && b == 1 && tested::exchange(a, 3) == 2 && a == 3;
}

static_assert(utility_works());

bool ftl_test() { return utility_works(); }
