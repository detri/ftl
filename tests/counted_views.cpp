#ifdef FTL_REPLACE_STL
#include <ranges>
namespace tested = std;
#else
#include <ftl/ranges>
namespace tested = ftl;
#endif

constexpr bool counted_works() {
    int values[] = {1, 2, 3, 4};
    auto view = tested::ranges::views::counted(values + 1, 2);
    auto iterator = view.begin();
    if (*iterator++ != 2 || *iterator++ != 3 || iterator != view.end()) return false;
    return view.size() == 2;
}

static_assert(counted_works());

bool ftl_test() { return counted_works(); }
