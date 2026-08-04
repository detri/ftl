#ifdef FTL_REPLACE_STL
#include <ranges>
namespace tested = std;
#else
#include <ftl/ranges>
namespace tested = ftl;
#endif

constexpr bool slide_works() {
    int values[] = {1, 2, 3, 4};
    auto view = values | tested::ranges::views::slide(3);
    if (view.size() != 2) return false;
    auto first = view[0];
    auto second = view[1];
    return first.size() == 3 && first[0] == 1 && first[2] == 3 &&
        second[0] == 2 && second[2] == 4;
}

static_assert(slide_works());
bool ftl_test() { return slide_works(); }
