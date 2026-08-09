/* @(#)e_acosh.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/* ftl_olm_acoshl(x)
 * Method :
 *	Based on
 *		ftl_olm_acoshl(x) = ftl_olm_logl [ x + ftl_olm_sqrtl(x*x-1) ]
 *	we have
 *		ftl_olm_acoshl(x) := ftl_olm_logl(x)+ln2,	if x is large; else
 *		ftl_olm_acoshl(x) := ftl_olm_logl(2x-1/(ftl_olm_sqrtl(x*x-1)+x)) if x>2; else
 *		ftl_olm_acoshl(x) := ftl_olm_log1pl(t+ftl_olm_sqrtl(2.0*t+t*t)); where t=x-1.
 *
 * Special cases:
 *	ftl_olm_acoshl(x) is NaN with signal if x<1.
 *	ftl_olm_acoshl(NaN) is NaN without signal.
 */

#include <openlibm_math.h>

#include "math_private.h"

static const long double
one	= 1.0,
ln2	= 6.931471805599453094287e-01L; /* 0x3FFE, 0xB17217F7, 0xD1CF79AC */

long double
ftl_olm_acoshl(long double x)
{
	long double t;
	u_int32_t se,i0,i1;
	GET_LDOUBLE_WORDS(se,i0,i1,x);
	if(se<0x3fff || se & 0x8000) {	/* x < 1 */
	    return (x-x)/(x-x);
	} else if(se >=0x401d) {	/* x > 2**30 */
	    if(se >=0x7fff) {		/* x is inf of NaN */
		return x+x;
	    } else
		return ftl_olm_logl(x)+ln2;	/* ftl_olm_acoshl(huge)=ftl_olm_logl(2x) */
	} else if(((se-0x3fff)|i0|i1)==0) {
	    return 0.0;			/* ftl_olm_acosh(1) = 0 */
	} else if (se > 0x4000) {	/* 2**28 > x > 2 */
	    t=x*x;
	    return ftl_olm_logl(2.0*x-one/(x+ftl_olm_sqrtl(t-one)));
	} else {			/* 1<x<2 */
	    t = x-one;
	    return ftl_olm_log1pl(t+ftl_olm_sqrtl(2.0*t+t*t));
	}
}
