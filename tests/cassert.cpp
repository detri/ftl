#ifdef FTL_REPLACE_STL
#include <cassert>
#else
#include <ftl/cassert>
#endif

constexpr bool assertions_are_constant() {
    assert(true);
    return true;
}
static_assert(assertions_are_constant());

bool ftl_test() {
    int evaluations = 0;
    assert(++evaluations == 1);
#ifdef NDEBUG
    return evaluations == 0;
#else
    return evaluations == 1;
#endif
}
