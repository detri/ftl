#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/s_llroundf.c,v 1.2 2005/04/08 00:52:27 das Exp $");

#define type		float
#define	roundit		ftl_olm_roundf
#define dtype		long long
#define	DTYPE_MIN	LLONG_MIN
#define	DTYPE_MAX	LLONG_MAX
#define	fn		ftl_olm_llroundf

#include "s_lround.c"
