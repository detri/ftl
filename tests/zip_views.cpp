#ifdef FTL_REPLACE_STL
#include <ranges>
#include <tuple>
namespace tested = std;
#else
#include <ftl/ranges>
#include <ftl/tuple>
namespace tested = ftl;
#endif

constexpr bool zip_works() {
    int left[] = {1, 2, 3};
    char right[] = {'a', 'b'};
    auto view = tested::ranges::views::zip(left, right);
    if (view.size() != 2) return false;
    auto first = *view.begin();
    if (tested::get<0>(first) != 1 || tested::get<1>(first) != 'a') return false;
    tested::get<0>(first) = 7;
    auto iterator = view.begin();
    ++iterator;
    return left[0] == 7 && tested::get<0>(*iterator) == 2 && ++iterator == view.end();
}

static_assert(zip_works());
bool ftl_test() { return zip_works(); }
