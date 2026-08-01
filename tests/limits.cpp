#ifdef FTL_REPLACE_STL
#include <limits>
namespace tested = std;
#else
#include <ftl/limits>
namespace tested = ftl;
#endif

static_assert(!tested::numeric_limits<void>::is_specialized);
static_assert(tested::numeric_limits<bool>::min() == false);
static_assert(tested::numeric_limits<bool>::max() == true);
static_assert(tested::numeric_limits<unsigned char>::digits == 8);
static_assert(tested::numeric_limits<int>::is_signed);
static_assert(tested::numeric_limits<int>::min() < 0);
static_assert(tested::numeric_limits<unsigned>::min() == 0);
static_assert(tested::numeric_limits<unsigned>::max() > 0);
static_assert(tested::numeric_limits<const long long>::is_specialized);
static_assert(tested::numeric_limits<float>::is_iec559);
static_assert(tested::numeric_limits<float>::epsilon() > 0);
static_assert(tested::numeric_limits<double>::infinity() > tested::numeric_limits<double>::max());

bool ftl_test() { return true; }
