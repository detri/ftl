#ifdef FTL_REPLACE_STL
#include <climits>
#else
#include <ftl/climits>
#endif

static_assert(CHAR_BIT >= 8 && SCHAR_MIN < 0 && SCHAR_MAX > 0);
static_assert(UCHAR_MAX >= SCHAR_MAX && CHAR_MIN <= CHAR_MAX);
static_assert(SHRT_MIN < 0 && SHRT_MAX > 0 && USHRT_MAX >= SHRT_MAX);
static_assert(INT_MIN < 0 && INT_MAX > 0 && UINT_MAX >= INT_MAX);
static_assert(LONG_MIN < 0 && LONG_MAX > 0 && ULONG_MAX >= LONG_MAX);
static_assert(LLONG_MIN < 0 && LLONG_MAX > 0 && ULLONG_MAX >= LLONG_MAX);
static_assert(MB_LEN_MAX >= 1);

bool ftl_test() { return true; }
