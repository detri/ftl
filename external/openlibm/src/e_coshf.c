/* e_coshf.c -- float version of e_cosh.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */

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

#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/e_coshf.c,v 1.9 2011/10/21 06:28:47 das Exp $");

#include <openlibm_math.h>

#include "math_private.h"

static const float one = 1.0, half=0.5, huge = 1.0e30;

OLM_DLLEXPORT float
__ieee754_coshf(float x)
{
	float t,w;
	int32_t ix;

	GET_FLOAT_WORD(ix,x);
	ix &= 0x7fffffff;

    /* x is INF or NaN */
	if(ix>=0x7f800000) return x*x;

    /* |x| in [0,0.5*ln2], return 1+ftl_olm_expm1(|x|)^2/(2*ftl_olm_exp(|x|)) */
	if(ix<0x3eb17218) {
	    t = ftl_olm_expm1f(ftl_olm_fabsf(x));
	    w = one+t;
	    if (ix<0x39800000) return one;	/* ftl_olm_cosh(tiny) = 1 */
	    return one+(t*t)/(w+w);
	}

    /* |x| in [0.5*ln2,9], return (ftl_olm_exp(|x|)+1/ftl_olm_exp(|x|))/2; */
	if (ix < 0x41100000) {
		t = __ieee754_expf(ftl_olm_fabsf(x));
		return half*t+half/t;
	}

    /* |x| in [9, ftl_olm_log(maxfloat)] return half*ftl_olm_exp(|x|) */
	if (ix < 0x42b17217)  return half*__ieee754_expf(ftl_olm_fabsf(x));

    /* |x| in [ftl_olm_log(maxfloat), overflowthresold] */
	if (ix<=0x42b2d4fc)
	    return ftl_olm___ldexp_expf(ftl_olm_fabsf(x), -1);

    /* |x| > overflowthresold, ftl_olm_cosh(x) overflow */
	return huge*huge;
}
