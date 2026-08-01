#ifdef FTL_REPLACE_STL
#include <tuple>
#include <array>
namespace tested = std;
#else
#include <ftl/tuple>
#include <ftl/array>
namespace tested = ftl;
#endif

constexpr bool tuple_works() {
    tested::tuple<int, long> value{1, 2};
    tested::get<0>(value) = 3;
    auto made = tested::make_tuple(4, 5L);
    int a = 0, b = 0;
    tested::tie(a, b) = made;
    auto joined = tested::tuple_cat(value, made);
    tested::array<int, 2> source{7, 8};
    tested::tuple<long, long> from_array(source);
    return tested::tuple_size_v<decltype(value)> == 2 &&
           tested::get<int>(value) == 3 && a == 4 && b == 5 &&
           tested::get<2>(joined) == 4 &&
           tested::get<1>(from_array) == 8 &&
           value < tested::tuple<int, long>{4, 0} &&
           tested::apply([](int x, long y) { return x + y; }, value) == 5;
}

static_assert(tuple_works());
bool ftl_test() { return tuple_works(); }
