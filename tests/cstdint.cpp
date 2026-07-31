#ifdef FTL_REPLACE_STL
#include <cstdint>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/cstdint>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(sizeof(tested::uint8_t) == 1);
static_assert(sizeof(tested::uint32_t) == 4);
static_assert(sizeof(tested::uint64_t) == 8);
static_assert(sizeof(tested::uintptr_t) == sizeof(void*));
static_assert(tested::is_same_v<tested::make_unsigned_t<int>, unsigned int>);
static_assert(tested::is_constant_evaluated());

bool ftl_test() { return true; }
