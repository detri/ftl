/* @(#)e_cosh.c 5.1 93/09/24 */
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

/* ftl_olm_coshl(x)
 * Method :
 * mathematically ftl_olm_coshl(x) if defined to be (ftl_olm_exp(x)+ftl_olm_exp(-x))/2
 *	1. Replace x by |x| (ftl_olm_coshl(x) = ftl_olm_coshl(-x)).
 *	2.
 *		                                        [ ftl_olm_exp(x) - 1 ]^2
 *	    0        <= x <= ln2/2  :  ftl_olm_coshl(x) := 1 + -------------------
 *			       			           2*ftl_olm_exp(x)
 *
 *		                                   ftl_olm_exp(x) +  1/ftl_olm_exp(x)
 *	    ln2/2    <= x <= 22     :  ftl_olm_coshl(x) := -------------------
 *			       			           2
 *	    22       <= x <= lnovft :  ftl_olm_coshl(x) := ftl_olm_expl(x)/2
 *	    lnovft   <= x <= ln2ovft:  ftl_olm_coshl(x) := ftl_olm_expl(x/2)/2 * ftl_olm_expl(x/2)
 *	    ln2ovft  <  x	    :  ftl_olm_coshl(x) := huge*huge (overflow)
 *
 * Special cases:
 *	ftl_olm_coshl(x) is |x| if x is +INF, -INF, or NaN.
 *	only ftl_olm_coshl(0)=1 is exact for finite x.
 */

#include <openlibm_math.h>

#include "math_private.h"

static const long double one = 1.0, half=0.5, huge = 1.0e4900L;

long double
ftl_olm_coshl(long double x)
{
	long double t,w;
	int32_t ex;
	u_int32_t mx,lx;

    /* High word of |x|. */
	GET_LDOUBLE_WORDS(ex,mx,lx,x);
	ex &= 0x7fff;

    /* x is INF or NaN */
	if(ex==0x7fff) return x*x;

    /* |x| in [0,0.5*ln2], return 1+ftl_olm_expm1l(|x|)^2/(2*ftl_olm_expl(|x|)) */
	if(ex < 0x3ffd || (ex == 0x3ffd && mx < 0xb17217f7u)) {
	    t = ftl_olm_expm1l(ftl_olm_fabsl(x));
	    w = one+t;
	    if (ex<0x3fbc) return w;	/* ftl_olm_cosh(tiny) = 1 */
	    return one+(t*t)/(w+w);
	}

    /* |x| in [0.5*ln2,22], return (ftl_olm_exp(|x|)+1/ftl_olm_exp(|x|)/2; */
	if (ex < 0x4003 || (ex == 0x4003 && mx < 0xb0000000u)) {
		t = ftl_olm_expl(ftl_olm_fabsl(x));
		return half*t+half/t;
	}

    /* |x| in [22, ln(maxdouble)] return half*ftl_olm_exp(|x|) */
	if (ex < 0x400c || (ex == 0x400c && mx < 0xb1700000u))
		return half*ftl_olm_expl(ftl_olm_fabsl(x));

    /* |x| in [ftl_olm_log(maxdouble), ftl_olm_log(2*maxdouble)) */
	if (ex == 0x400c && (mx < 0xb174ddc0u
			     || (mx == 0xb174ddc0u && lx < 0x31aec0ebu)))
	{
	    w = ftl_olm_expl(half*ftl_olm_fabsl(x));
	    t = half*w;
	    return t*w;
	}

    /* |x| >= ftl_olm_log(2*maxdouble), ftl_olm_cosh(x) overflow */
	return huge*huge;
}
