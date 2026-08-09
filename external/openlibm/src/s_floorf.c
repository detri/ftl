/* s_floorf.c -- float version of s_floor.c.
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
//__FBSDID("$FreeBSD: src/lib/msun/src/s_floorf.c,v 1.8 2008/02/22 02:30:35 das Exp $");

/*
 * ftl_olm_floorf(x)
 * Return x rounded toward -inf to integral value
 * Method:
 *	Bit twiddling.
 * Exception:
 *	Inexact flag raised if x not equal to ftl_olm_floorf(x).
 */

#include <openlibm_math.h>

#include "math_private.h"

static const float huge = 1.0e30;

OLM_DLLEXPORT float
ftl_olm_floorf(float x)
{
	int32_t i0,ftl_olm_j0;
	u_int32_t i;
	GET_FLOAT_WORD(i0,x);
	ftl_olm_j0 = ((i0>>23)&0xff)-0x7f;
	if(ftl_olm_j0<23) {
	    if(ftl_olm_j0<0) { 	/* raise inexact if x != 0 */
		if(huge+x>(float)0.0) {/* return 0*sign(x) if |x|<1 */
		    if(i0>=0) {i0=0;}
		    else if((i0&0x7fffffff)!=0)
			{ i0=0xbf800000;}
		}
	    } else {
		i = (0x007fffff)>>ftl_olm_j0;
		if((i0&i)==0) return x; /* x is integral */
		if(huge+x>(float)0.0) {	/* raise inexact flag */
		    if(i0<0) i0 += (0x00800000)>>ftl_olm_j0;
		    i0 &= (~i);
		}
	    }
	} else {
	    if(ftl_olm_j0==0x80) return x+x;	/* inf or NaN */
	    else return x;		/* x is integral */
	}
	SET_FLOAT_WORD(x,i0);
	return x;
}
