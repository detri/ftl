#ifdef FTL_REPLACE_STL
#include <cinttypes>
namespace tested = std;
#else
#include <ftl/cinttypes>
namespace tested = ftl;
#endif

static_assert(tested::imaxabs(tested::intmax_t{-7}) == 7);
static_assert(tested::imaxdiv(7, 3).quot == 2 && tested::imaxdiv(7, 3).rem == 1);
static_assert(sizeof(PRId8) > 1 && sizeof(PRId16) > 1 && sizeof(PRId32) > 1);
static_assert(sizeof(PRId64) > 1 && sizeof(PRIdMAX) > 1 && sizeof(PRIdPTR) > 1);
static_assert(sizeof(SCNd8) > 1 && sizeof(SCNd16) > 1 && sizeof(SCNd32) > 1);
static_assert(sizeof(SCNd64) > 1 && sizeof(SCNdMAX) > 1 && sizeof(SCNdPTR) > 1);

bool ftl_test() { return true; }
