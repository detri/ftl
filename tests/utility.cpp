#ifdef FTL_REPLACE_STL
#include <utility>
namespace tested = std;
#else
#include <ftl/utility>
namespace tested = ftl;
#endif

enum class value : unsigned char { one = 1 };

static_assert(tested::to_underlying(value::one) == 1);

constexpr bool utility_works() {
    int a = 1, b = 2;
    tested::swap(a, b);
    return a == 2 && b == 1 && tested::exchange(a, 3) == 2 && a == 3;
}

static_assert(utility_works());

bool ftl_test() { return utility_works(); }
