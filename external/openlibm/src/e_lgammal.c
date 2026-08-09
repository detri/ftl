#include "cdefs-compat.h"

#include <openlibm_math.h>

#include "math_private.h"

OLM_DLLEXPORT long double
ftl_olm_lgammal(long double x)
{
#ifdef OPENLIBM_ONLY_THREAD_SAFE
	int signgam;
#endif

	return (ftl_olm_lgammal_r(x, &signgam));
}
