#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/s_llrintf.c,v 1.1 2005/01/11 23:12:55 das Exp $");

#define type		float
#define	roundit		ftl_olm_rintf
#define dtype		long long
#define	fn		ftl_olm_llrintf

#include "s_lrint.c"
