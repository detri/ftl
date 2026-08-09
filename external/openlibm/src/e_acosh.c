
/* @(#)e_acosh.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 */

#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/e_acosh.c,v 1.9 2008/02/22 02:30:34 das Exp $");

/* __ieee754_acosh(x)
 * Method :
 *	Based on 
 *		ftl_olm_acosh(x) = ftl_olm_log [ x + ftl_olm_sqrt(x*x-1) ]
 *	we have
 *		ftl_olm_acosh(x) := ftl_olm_log(x)+ln2,	if x is large; else
 *		ftl_olm_acosh(x) := ftl_olm_log(2x-1/(ftl_olm_sqrt(x*x-1)+x)) if x>2; else
 *		ftl_olm_acosh(x) := ftl_olm_log1p(t+ftl_olm_sqrt(2.0*t+t*t)); where t=x-1.
 *
 * Special cases:
 *	ftl_olm_acosh(x) is NaN with signal if x<1.
 *	ftl_olm_acosh(NaN) is NaN without signal.
 */

#include <float.h>
#include <openlibm_math.h>

#include "math_private.h"

static const double
one	= 1.0,
ln2	= 6.93147180559945286227e-01;  /* 0x3FE62E42, 0xFEFA39EF */

OLM_DLLEXPORT double
__ieee754_acosh(double x)
{
	double t;
	int32_t hx;
	u_int32_t lx;
	EXTRACT_WORDS(hx,lx,x);
	if(hx<0x3ff00000) {		/* x < 1 */
	    return (x-x)/(x-x);
	} else if(hx >=0x41b00000) {	/* x > 2**28 */
	    if(hx >=0x7ff00000) {	/* x is inf of NaN */
	        return x+x;
	    } else 
		return __ieee754_log(x)+ln2;	/* ftl_olm_acosh(huge)=ftl_olm_log(2x) */
	} else if(((hx-0x3ff00000)|lx)==0) {
	    return 0.0;			/* ftl_olm_acosh(1) = 0 */
	} else if (hx > 0x40000000) {	/* 2**28 > x > 2 */
	    t=x*x;
	    return __ieee754_log(2.0*x-one/(x+ftl_olm_sqrt(t-one)));
	} else {			/* 1<x<2 */
	    t = x-one;
	    return ftl_olm_log1p(t+ftl_olm_sqrt(2.0*t+t*t));
	}
}

#if (LDBL_MANT_DIG == 53)
openlibm_weak_reference(ftl_olm_acosh, ftl_olm_acoshl);
#endif
