#ifdef FTL_REPLACE_STL
#include <ranges>
namespace tested = std;
#else
#include <ftl/ranges>
namespace tested = ftl;
#endif

constexpr bool stride_works() {
    int values[] = {0, 1, 2, 3, 4, 5, 6};
    auto view = values | tested::ranges::views::stride(3);
    if (view.size() != 3 || view[0] != 0 || view[1] != 3 || view[2] != 6) return false;
    auto iterator = view.end();
    if (*--iterator != 6 || *--iterator != 3 || *--iterator != 0) return false;
    return view.end() - view.begin() == 3;
}

static_assert(stride_works());
bool ftl_test() { return stride_works(); }
