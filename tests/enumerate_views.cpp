#ifdef FTL_REPLACE_STL
#include <ranges>
#include <tuple>
namespace tested = std;
#else
#include <ftl/ranges>
#include <ftl/tuple>
namespace tested = ftl;
#endif

constexpr bool enumerate_works() {
    int values[] = {4, 5, 6};
    auto view = values | tested::ranges::views::enumerate;
    if (view.size() != 3) return false;
    auto iterator = view.begin();
    auto first = *iterator++;
    auto second = *iterator;
    tested::get<1>(second) = 9;
    return tested::get<0>(first) == 0 && tested::get<1>(first) == 4 &&
        tested::get<0>(second) == 1 && values[1] == 9;
}

static_assert(enumerate_works());
bool ftl_test() { return enumerate_works(); }
