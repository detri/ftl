#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/s_llroundl.c,v 1.1 2005/04/08 01:24:08 das Exp $");

#define type		long double
#define	roundit		ftl_olm_roundl
#define dtype		long long
#define	DTYPE_MIN	LLONG_MIN
#define	DTYPE_MAX	LLONG_MAX
#define	fn		ftl_olm_llroundl

#include "s_lround.c"
