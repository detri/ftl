#ifdef FTL_REPLACE_STL
#include <initializer_list>
namespace tested = std;
#else
#include <ftl/initializer_list>
namespace tested = ftl;
#endif

constexpr int sum(tested::initializer_list<int> values) {
    int result = 0;
    for (int value : values)
        result += value;
    return result;
}

static_assert(sum({1, 2, 3}) == 6);

bool ftl_test() { return true; }
