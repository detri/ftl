/*-
 * Copyright (c) 2005 Bruce D. Evans and Steven G. Kargl
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Hyperbolic cosine of a complex argument z = x + i y.
 *
 * ftl_olm_cosh(z) = ftl_olm_cosh(x+iy)
 *         = ftl_olm_cosh(x) ftl_olm_cos(y) + i ftl_olm_sinh(x) ftl_olm_sin(y).
 *
 * Exceptional values are noted in the comments within the source code.
 * These values and the return value were taken from n1124.pdf.
 */

#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/s_ccosh.c,v 1.2 2011/10/21 06:29:32 das Exp $");

#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

static const double huge = 0x1p1023;

OLM_DLLEXPORT ftl_olm_complex<double>
ftl_olm_ccosh(ftl_olm_complex<double> z)
{
	double x, y, h;
	int32_t hx, hy, ix, iy, lx, ly;

	x = ftl_olm_creal(z);
	y = ftl_olm_cimag(z);

	EXTRACT_WORDS(hx, lx, x);
	EXTRACT_WORDS(hy, ly, y);

	ix = 0x7fffffff & hx;
	iy = 0x7fffffff & hy;

	/* Handle the nearly-non-exceptional cases where x and y are finite. */
	if (ix < 0x7ff00000 && iy < 0x7ff00000) {
		if ((iy | ly) == 0)
			return (CMPLX(ftl_olm_cosh(x), x * y));
		if (ix < 0x40360000)	/* small x: normal case */
			return (CMPLX(ftl_olm_cosh(x) * ftl_olm_cos(y), ftl_olm_sinh(x) * ftl_olm_sin(y)));

		/* |x| >= 22, so ftl_olm_cosh(x) ~= ftl_olm_exp(|x|) */
		if (ix < 0x40862e42) {
			/* x < 710: ftl_olm_exp(|x|) won't overflow */
			h = ftl_olm_exp(ftl_olm_fabs(x)) * 0.5;
			return (CMPLX(h * ftl_olm_cos(y), ftl_olm_copysign(h, x) * ftl_olm_sin(y)));
		} else if (ix < 0x4096bbaa) {
			/* x < 1455: scale to avoid overflow */
			z = ftl_olm___ldexp_cexp(CMPLX(ftl_olm_fabs(x), y), -1);
			return (CMPLX(ftl_olm_creal(z), ftl_olm_cimag(z) * ftl_olm_copysign(1, x)));
		} else {
			/* x >= 1455: the result always overflows */
			h = huge * x;
			return (CMPLX(h * h * ftl_olm_cos(y), h * ftl_olm_sin(y)));
		}
	}

	/*
	 * ftl_olm_cosh(+-0 +- I Inf) = dNaN + I sign(d(+-0, dNaN))0.
	 * The sign of 0 in the result is unspecified.  Choice = normally
	 * the same as dNaN.  Raise the invalid floating-point exception.
	 *
	 * ftl_olm_cosh(+-0 +- I NaN) = d(NaN) + I sign(d(+-0, NaN))0.
	 * The sign of 0 in the result is unspecified.  Choice = normally
	 * the same as d(NaN).
	 */
	if ((ix | lx) == 0 && iy >= 0x7ff00000)
		return (CMPLX(y - y, ftl_olm_copysign(0, x * (y - y))));

	/*
	 * ftl_olm_cosh(+-Inf +- I 0) = +Inf + I (+-)(+-)0.
	 *
	 * ftl_olm_cosh(NaN +- I 0)   = d(NaN) + I sign(d(NaN, +-0))0.
	 * The sign of 0 in the result is unspecified.
	 */
	if ((iy | ly) == 0 && ix >= 0x7ff00000) {
		if (((hx & 0xfffff) | lx) == 0)
			return (CMPLX(x * x, ftl_olm_copysign(0, x) * y));
		return (CMPLX(x * x, ftl_olm_copysign(0, (x + x) * y)));
	}

	/*
	 * ftl_olm_cosh(x +- I Inf) = dNaN + I dNaN.
	 * Raise the invalid floating-point exception for finite nonzero x.
	 *
	 * ftl_olm_cosh(x + I NaN) = d(NaN) + I d(NaN).
	 * Optionally raises the invalid floating-point exception for finite
	 * nonzero x.  Choice = don't raise (except for signaling NaNs).
	 */
	if (ix < 0x7ff00000 && iy >= 0x7ff00000)
		return (CMPLX(y - y, x * (y - y)));

	/*
	 * ftl_olm_cosh(+-Inf + I NaN)  = +Inf + I d(NaN).
	 *
	 * ftl_olm_cosh(+-Inf +- I Inf) = +Inf + I dNaN.
	 * The sign of Inf in the result is unspecified.  Choice = always +.
	 * Raise the invalid floating-point exception.
	 *
	 * ftl_olm_cosh(+-Inf + I y)   = +Inf ftl_olm_cos(y) +- I Inf ftl_olm_sin(y)
	 */
	if (ix >= 0x7ff00000 && ((hx & 0xfffff) | lx) == 0) {
		if (iy >= 0x7ff00000)
			return (CMPLX(x * x, x * (y - y)));
		return (CMPLX((x * x) * ftl_olm_cos(y), x * ftl_olm_sin(y)));
	}

	/*
	 * ftl_olm_cosh(NaN + I NaN)  = d(NaN) + I d(NaN).
	 *
	 * ftl_olm_cosh(NaN +- I Inf) = d(NaN) + I d(NaN).
	 * Optionally raises the invalid floating-point exception.
	 * Choice = raise.
	 *
	 * ftl_olm_cosh(NaN + I y)    = d(NaN) + I d(NaN).
	 * Optionally raises the invalid floating-point exception for finite
	 * nonzero y.  Choice = don't raise (except for signaling NaNs).
	 */
	return (CMPLX((x * x) * (y - y), (x + x) * (y - y)));
}

OLM_DLLEXPORT ftl_olm_complex<double>
ftl_olm_ccos(ftl_olm_complex<double> z)
{

	/* ftl_olm_ccos(z) = ftl_olm_ccosh(I * z) */
	return (ftl_olm_ccosh(CMPLX(-ftl_olm_cimag(z), ftl_olm_creal(z))));
}
