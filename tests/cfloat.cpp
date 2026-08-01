#ifdef FTL_REPLACE_STL
#include <cfloat>
#else
#include <ftl/cfloat>
#endif

static_assert(FLT_RADIX >= 2 && FLT_MANT_DIG >= 6 && DBL_MANT_DIG >= 10);
static_assert(FLT_MAX > 1.0F && DBL_MAX > 1.0 && LDBL_MAX > 1.0L);
static_assert(FLT_MIN > 0.0F && DBL_MIN > 0.0 && LDBL_MIN > 0.0L);
static_assert(FLT_TRUE_MIN > 0.0F && DBL_TRUE_MIN > 0.0 && LDBL_TRUE_MIN > 0.0L);
static_assert(FLT_EPSILON > 0.0F && DBL_EPSILON > 0.0 && LDBL_EPSILON > 0.0L);
static_assert(FLT_DECIMAL_DIG >= 6 && DBL_DECIMAL_DIG >= 10 && DECIMAL_DIG >= 10);
static_assert(FLT_HAS_SUBNORM >= -1 && FLT_HAS_SUBNORM <= 1);

bool ftl_test() { return true; }
