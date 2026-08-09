
/* @(#)e_cosh.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/e_cosh.c,v 1.10 2011/10/21 06:28:47 das Exp $");

/* __ieee754_cosh(x)
 * Method : 
 * mathematically ftl_olm_cosh(x) if defined to be (ftl_olm_exp(x)+ftl_olm_exp(-x))/2
 *	1. Replace x by |x| (ftl_olm_cosh(x) = ftl_olm_cosh(-x)). 
 *	2. 
 *		                                        [ ftl_olm_exp(x) - 1 ]^2 
 *	    0        <= x <= ln2/2  :  ftl_olm_cosh(x) := 1 + -------------------
 *			       			           2*ftl_olm_exp(x)
 *
 *		                                  ftl_olm_exp(x) +  1/ftl_olm_exp(x)
 *	    ln2/2    <= x <= 22     :  ftl_olm_cosh(x) := -------------------
 *			       			          2
 *	    22       <= x <= lnovft :  ftl_olm_cosh(x) := ftl_olm_exp(x)/2 
 *	    lnovft   <= x <= ln2ovft:  ftl_olm_cosh(x) := ftl_olm_exp(x/2)/2 * ftl_olm_exp(x/2)
 *	    ln2ovft  <  x	    :  ftl_olm_cosh(x) := huge*huge (overflow)
 *
 * Special cases:
 *	ftl_olm_cosh(x) is |x| if x is +INF, -INF, or NaN.
 *	only ftl_olm_cosh(0)=1 is exact for finite x.
 */

#include <float.h>
#include <openlibm_math.h>

#include "math_private.h"

static const double one = 1.0, half=0.5, huge = 1.0e300;

OLM_DLLEXPORT double
__ieee754_cosh(double x)
{
	double t,w;
	int32_t ix;

    /* High word of |x|. */
	GET_HIGH_WORD(ix,x);
	ix &= 0x7fffffff;

    /* x is INF or NaN */
	if(ix>=0x7ff00000) return x*x;	

    /* |x| in [0,0.5*ln2], return 1+ftl_olm_expm1(|x|)^2/(2*ftl_olm_exp(|x|)) */
	if(ix<0x3fd62e43) {
	    t = ftl_olm_expm1(ftl_olm_fabs(x));
	    w = one+t;
	    if (ix<0x3c800000) return w;	/* ftl_olm_cosh(tiny) = 1 */
	    return one+(t*t)/(w+w);
	}

    /* |x| in [0.5*ln2,22], return (ftl_olm_exp(|x|)+1/ftl_olm_exp(|x|)/2; */
	if (ix < 0x40360000) {
		t = __ieee754_exp(ftl_olm_fabs(x));
		return half*t+half/t;
	}

    /* |x| in [22, ftl_olm_log(maxdouble)] return half*ftl_olm_exp(|x|) */
	if (ix < 0x40862E42)  return half*__ieee754_exp(ftl_olm_fabs(x));

    /* |x| in [ftl_olm_log(maxdouble), overflowthresold] */
	if (ix<=0x408633CE)
	    return ftl_olm___ldexp_exp(ftl_olm_fabs(x), -1);

    /* |x| > overflowthresold, ftl_olm_cosh(x) overflow */
	return huge*huge;
}

#if (LDBL_MANT_DIG == 53)
openlibm_weak_reference(ftl_olm_cosh, ftl_olm_coshl);
#endif
