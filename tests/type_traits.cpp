#ifdef FTL_REPLACE_STL
#include <type_traits>
namespace tested = std;
#else
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(tested::is_same_v<tested::remove_cvref_t<const int&>, int>);

bool ftl_test()
{
    return true;
}