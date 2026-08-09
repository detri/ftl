#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/s_lrintl.c,v 1.1 2008/01/14 02:12:06 das Exp $");

#define type		long double
#define	roundit		ftl_olm_rintl
#define dtype		long
#define	fn		ftl_olm_lrintl

#include "s_lrint.c"
