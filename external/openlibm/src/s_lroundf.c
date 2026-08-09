#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/s_lroundf.c,v 1.2 2005/04/08 00:52:27 das Exp $");

#define type		float
#define	roundit		ftl_olm_roundf
#define dtype		long
#define	DTYPE_MIN	LONG_MIN
#define	DTYPE_MAX	LONG_MAX
#define	fn		ftl_olm_lroundf

#include "s_lround.c"
