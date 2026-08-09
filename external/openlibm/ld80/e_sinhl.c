/* @(#)e_sinh.c 5.1 93/09/24 */
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

/* ftl_olm_sinhl(x)
 * Method :
 * mathematically ftl_olm_sinh(x) if defined to be (ftl_olm_exp(x)-ftl_olm_exp(-x))/2
 *	1. Replace x by |x| (ftl_olm_sinhl(-x) = -ftl_olm_sinhl(x)).
 *	2.
 *		                                     E + E/(E+1)
 *	    0        <= x <= 25     :  ftl_olm_sinhl(x) := --------------, E=ftl_olm_expm1l(x)
 *			       			         2
 *
 *	    25       <= x <= lnovft :  ftl_olm_sinhl(x) := ftl_olm_expl(x)/2
 *	    lnovft   <= x <= ln2ovft:  ftl_olm_sinhl(x) := ftl_olm_expl(x/2)/2 * ftl_olm_expl(x/2)
 *	    ln2ovft  <  x	    :  ftl_olm_sinhl(x) := x*shuge (overflow)
 *
 * Special cases:
 *	ftl_olm_sinhl(x) is |x| if x is +INF, -INF, or NaN.
 *	only ftl_olm_sinhl(0)=0 is exact for finite x.
 */

#include <openlibm_math.h>

#include "math_private.h"

static const long double one = 1.0, shuge = 1.0e4931L;

long double
ftl_olm_sinhl(long double x)
{
	long double t,w,h;
	u_int32_t jx,ix,i0,i1;

    /* Words of |x|. */
	GET_LDOUBLE_WORDS(jx,i0,i1,x);
	ix = jx&0x7fff;

    /* x is INF or NaN */
	if(ix==0x7fff) return x+x;

	h = 0.5;
	if (jx & 0x8000) h = -h;
    /* |x| in [0,25], return sign(x)*0.5*(E+E/(E+1))) */
	if (ix < 0x4003 || (ix == 0x4003 && i0 <= 0xc8000000)) { /* |x|<25 */
	    if (ix<0x3fdf)		 /* |x|<2**-32 */
		if(shuge+x>one) return x;/* ftl_olm_sinh(tiny) = tiny with inexact */
	    t = ftl_olm_expm1l(ftl_olm_fabsl(x));
	    if(ix<0x3fff) return h*(2.0*t-t*t/(t+one));
	    return h*(t+t/(t+one));
	}

    /* |x| in [25, ftl_olm_log(maxdouble)] return 0.5*ftl_olm_exp(|x|) */
	if (ix < 0x400c || (ix == 0x400c && i0 < 0xb17217f7))
		return h*ftl_olm_expl(ftl_olm_fabsl(x));

    /* |x| in [ftl_olm_log(maxdouble), overflowthreshold] */
	if (ix<0x400c || (ix == 0x400c && (i0 < 0xb174ddc0
					   || (i0 == 0xb174ddc0
					       && i1 <= 0x31aec0ea)))) {
	    w = ftl_olm_expl(0.5*ftl_olm_fabsl(x));
	    t = h*w;
	    return t*w;
	}

    /* |x| > overflowthreshold, ftl_olm_sinhl(x) overflow */
	return x*shuge;
}
