#ifdef FTL_REPLACE_STL
#include <ranges>
namespace tested = std;
#else
#include <ftl/ranges>
namespace tested = ftl;
#endif

struct sum {
    constexpr int operator()(int left, int right) const { return left + right; }
};

constexpr bool zip_transform_works() {
    int left[] = {1, 2, 3};
    int right[] = {10, 20};
    auto view = tested::ranges::views::zip_transform(sum{}, left, right);
    return view.size() == 2 && view[0] == 11 && view[1] == 22;
}

static_assert(zip_transform_works());
bool ftl_test() { return zip_transform_works(); }
