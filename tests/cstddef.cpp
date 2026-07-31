#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct empty {};
struct compressed {
    FTL_NO_UNIQUE_ADDRESS empty value;
    int payload;
};

static_assert(sizeof(compressed) == sizeof(int));
static_assert(tested::is_same_v<decltype(sizeof(0)), tested::size_t>);
static_assert(tested::to_integer<unsigned>(tested::byte{3} << 1) == 6);
static_assert(alignof(tested::max_align_t) >= alignof(long double));

bool ftl_test()
{
    return true;
}