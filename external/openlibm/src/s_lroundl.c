#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/s_lroundl.c,v 1.1 2005/04/08 01:24:08 das Exp $");

#define type		long double
#define	roundit		ftl_olm_roundl
#define dtype		long
#define	DTYPE_MIN	LONG_MIN
#define	DTYPE_MAX	LONG_MAX
#define	fn		ftl_olm_lroundl

#include "s_lround.c"
