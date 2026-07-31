#ifdef FTL_REPLACE_STL
#include <compare>
namespace tested = std;
#else
#include <ftl/compare>
namespace tested = ftl;
#endif

static_assert(tested::compare_three_way{}(2, 1) > 0);
static_assert(tested::strong_ordering::equal == 0);

bool ftl_test() { return true; }
