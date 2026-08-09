#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/s_lrintf.c,v 1.1 2005/01/11 23:12:55 das Exp $");

#define type		float
#define	roundit		ftl_olm_rintf
#define dtype		long
#define	fn		ftl_olm_lrintf

#include "s_lrint.c"
