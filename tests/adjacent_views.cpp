#ifdef FTL_REPLACE_STL
#include <ranges>
#include <tuple>
namespace tested = std;
#else
#include <ftl/ranges>
#include <ftl/tuple>
namespace tested = ftl;
#endif

struct add_pair { constexpr int operator()(int a, int b) const { return a + b; } };

constexpr bool adjacent_works() {
    int values[] = {1, 2, 3, 4};
    auto pairs = values | tested::ranges::views::adjacent<2>;
    if (pairs.size() != 3) return false;
    auto middle = pairs[1];
    if (tested::get<0>(middle) != 2 || tested::get<1>(middle) != 3) return false;
    tested::get<1>(middle) = 8;
    auto sums = values | tested::ranges::views::adjacent_transform<2>(add_pair{});
    return values[2] == 8 && sums[0] == 3 && sums[1] == 10 && sums.size() == 3;
}

static_assert(adjacent_works());
bool ftl_test() { return adjacent_works(); }
