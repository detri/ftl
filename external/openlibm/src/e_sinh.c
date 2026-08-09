
/* @(#)e_sinh.c 1.3 95/01/18 */
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
//__FBSDID("$FreeBSD: src/lib/msun/src/e_sinh.c,v 1.11 2011/10/21 06:28:47 das Exp $");

/* __ieee754_sinh(x)
 * Method : 
 * mathematically ftl_olm_sinh(x) if defined to be (ftl_olm_exp(x)-ftl_olm_exp(-x))/2
 *	1. Replace x by |x| (ftl_olm_sinh(-x) = -ftl_olm_sinh(x)). 
 *	2. 
 *		                                    E + E/(E+1)
 *	    0        <= x <= 22     :  ftl_olm_sinh(x) := --------------, E=ftl_olm_expm1(x)
 *			       			        2
 *
 *	    22       <= x <= lnovft :  ftl_olm_sinh(x) := ftl_olm_exp(x)/2 
 *	    lnovft   <= x <= ln2ovft:  ftl_olm_sinh(x) := ftl_olm_exp(x/2)/2 * ftl_olm_exp(x/2)
 *	    ln2ovft  <  x	    :  ftl_olm_sinh(x) := x*shuge (overflow)
 *
 * Special cases:
 *	ftl_olm_sinh(x) is |x| if x is +INF, -INF, or NaN.
 *	only ftl_olm_sinh(0)=0 is exact for finite x.
 */

#include <float.h>
#include <openlibm_math.h>

#include "math_private.h"

static const double one = 1.0, shuge = 1.0e307;

OLM_DLLEXPORT double
__ieee754_sinh(double x)
{
	double t,h;
	int32_t ix,jx;

    /* High word of |x|. */
	GET_HIGH_WORD(jx,x);
	ix = jx&0x7fffffff;

    /* x is INF or NaN */
	if(ix>=0x7ff00000) return x+x;	

	h = 0.5;
	if (jx<0) h = -h;
    /* |x| in [0,22], return sign(x)*0.5*(E+E/(E+1))) */
	if (ix < 0x40360000) {		/* |x|<22 */
	    if (ix<0x3e300000) 		/* |x|<2**-28 */
		if(shuge+x>one) return x;/* ftl_olm_sinh(tiny) = tiny with inexact */
	    t = ftl_olm_expm1(ftl_olm_fabs(x));
	    if(ix<0x3ff00000) return h*(2.0*t-t*t/(t+one));
	    return h*(t+t/(t+one));
	}

    /* |x| in [22, ftl_olm_log(maxdouble)] return 0.5*ftl_olm_exp(|x|) */
	if (ix < 0x40862E42)  return h*__ieee754_exp(ftl_olm_fabs(x));

    /* |x| in [ftl_olm_log(maxdouble), overflowthresold] */
	if (ix<=0x408633CE)
	    return h*2.0*ftl_olm___ldexp_exp(ftl_olm_fabs(x), -1);

    /* |x| > overflowthresold, ftl_olm_sinh(x) overflow */
	return x*shuge;
}

#if (LDBL_MANT_DIG == 53)
openlibm_weak_reference(ftl_olm_sinh, ftl_olm_sinhl);
#endif
