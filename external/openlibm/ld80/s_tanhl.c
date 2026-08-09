/* @(#)s_tanh.c 5.1 93/09/24 */
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

/* ftl_olm_tanhl(x)
 * Return the Hyperbolic Tangent of x
 *
 * Method :
 *				        x    -x
 *				       e  - e
 *	0. ftl_olm_tanhl(x) is defined to be -----------
 *				        x    -x
 *				       e  + e
 *	1. reduce x to non-negative by ftl_olm_tanhl(-x) = -ftl_olm_tanhl(x).
 *	2.  0      <= x <= 2**-55 : ftl_olm_tanhl(x) := x*(one+x)
 *					         -t
 *	    2**-55 <  x <=  1     : ftl_olm_tanhl(x) := -----; t = ftl_olm_expm1l(-2x)
 *					        t + 2
 *						      2
 *	    1      <= x <=  23.0  : ftl_olm_tanhl(x) := 1-  ----- ; t=ftl_olm_expm1l(2x)
 *						    t + 2
 *	    23.0   <  x <= INF    : ftl_olm_tanhl(x) := 1.
 *
 * Special cases:
 *	ftl_olm_tanhl(NaN) is NaN;
 *	only ftl_olm_tanhl(0)=0 is exact for finite argument.
 */

#include <openlibm_math.h>

#include "math_private.h"

static const long double one=1.0, two=2.0, tiny = 1.0e-4900L;

long double
ftl_olm_tanhl(long double x)
{
	long double t,z;
	int32_t se;
	u_int32_t jj0,jj1,ix;

    /* High word of |x|. */
	GET_LDOUBLE_WORDS(se,jj0,jj1,x);
	ix = se&0x7fff;

    /* x is INF or NaN */
	if(ix==0x7fff) {
	    /* for NaN it's not important which branch: ftl_olm_tanhl(NaN) = NaN */
	    if (se&0x8000) return one/x-one;	/* ftl_olm_tanhl(-inf)= -1; */
	    else	   return one/x+one;	/* ftl_olm_tanhl(+inf)=+1 */
	}

    /* |x| < 23 */
	if (ix < 0x4003 || (ix == 0x4003 && jj0 < 0xb8000000u)) {/* |x|<23 */
	    if ((ix|jj0|jj1) == 0)
		return x;		/* x == +- 0 */
	    if (ix<0x3fc8)		/* |x|<2**-55 */
		return x*(one+tiny);	/* ftl_olm_tanh(small) = small */
	    if (ix>=0x3fff) {	/* |x|>=1  */
		t = ftl_olm_expm1l(two*ftl_olm_fabsl(x));
		z = one - two/(t+two);
	    } else {
		t = ftl_olm_expm1l(-two*ftl_olm_fabsl(x));
		z= -t/(t+two);
	    }
    /* |x| > 23, return +-1 */
	} else {
	    z = one - tiny;		/* raised inexact flag */
	}
	return (se&0x8000)? -z: z;
}
