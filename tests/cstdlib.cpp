#ifdef FTL_REPLACE_STL
#include <cstdlib>
namespace tested = std;
#else
#include <ftl/cstdlib>
namespace tested = ftl;
#endif

static_assert(sizeof(tested::size_t) == sizeof(void*));
static_assert(tested::abs(-7) == 7 && tested::labs(-8L) == 8L);
static_assert(tested::llabs(-9LL) == 9LL);
static_assert(tested::div(-7, 3).quot == -2 && tested::div(-7, 3).rem == -1);
static_assert(tested::ldiv(7L, 3L).quot == 2);
static_assert(tested::lldiv(7LL, 3LL).rem == 1);
static_assert(EXIT_SUCCESS != EXIT_FAILURE && RAND_MAX >= 32767);

bool ftl_test() {
    void* memory = tested::malloc(16);
    tested::free(memory);
    return MB_CUR_MAX >= 1 && tested::atoi("42") == 42 &&
           tested::strtol("17", nullptr, 10) == 17;
}
