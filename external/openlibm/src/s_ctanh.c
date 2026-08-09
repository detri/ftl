/*-
 * Copyright (c) 2011 David Schultz
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
 * Hyperbolic tangent of a complex argument z = x + i y.
 *
 * The algorithm is from:
 *
 *   W. Kahan.  Branch Cuts for Complex Elementary Functions or Much
 *   Ado About Nothing's Sign Bit.  In The State of the Art in
 *   Numerical Analysis, pp. 165 ff.  Iserles and Powell, eds., 1987.
 *
 * Method:
 *
 *   Let t    = ftl_olm_tan(x)
 *       beta = 1/ftl_olm_cos^2(y)
 *       s    = ftl_olm_sinh(x)
 *       rho  = ftl_olm_cosh(x)
 *
 *   We have:
 *
 *   ftl_olm_tanh(z) = ftl_olm_sinh(z) / ftl_olm_cosh(z)
 *
 *             ftl_olm_sinh(x) ftl_olm_cos(y) + i ftl_olm_cosh(x) ftl_olm_sin(y)
 *           = ---------------------------------
 *             ftl_olm_cosh(x) ftl_olm_cos(y) + i ftl_olm_sinh(x) ftl_olm_sin(y)
 *
 *             ftl_olm_cosh(x) ftl_olm_sinh(x) / ftl_olm_cos^2(y) + i ftl_olm_tan(y)
 *           = -------------------------------------
 *                    1 + ftl_olm_sinh^2(x) / ftl_olm_cos^2(y)
 *
 *             beta rho s + i t
 *           = ----------------
 *               1 + beta s^2
 *
 * Modifications:
 *
 *   I omitted the original algorithm's handling of overflow in ftl_olm_tan(x) after
 *   verifying with nearpi.c that this can't happen in IEEE single or double
 *   precision.  I also handle large x differently.
 */

#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/s_ctanh.c,v 1.2 2011/10/21 06:30:16 das Exp $");

#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

OLM_DLLEXPORT ftl_olm_complex<double>
ftl_olm_ctanh(ftl_olm_complex<double> z)
{
	double x, y;
	double t, beta, s, rho, denom;
	u_int32_t hx, ix, lx;

	x = ftl_olm_creal(z);
	y = ftl_olm_cimag(z);

	EXTRACT_WORDS(hx, lx, x);
	ix = hx & 0x7fffffff;

	/*
	 * ftl_olm_ctanh(NaN + i 0) = NaN + i 0
	 *
	 * ftl_olm_ctanh(NaN + i y) = NaN + i NaN		for y != 0
	 *
	 * The imaginary part has the sign of x*ftl_olm_sin(2*y), but there's no
	 * special effort to get this right.
	 *
	 * ftl_olm_ctanh(+-Inf +- i Inf) = +-1 +- 0
	 *
	 * ftl_olm_ctanh(+-Inf + i y) = +-1 + 0 ftl_olm_sin(2y)		for y finite
	 *
	 * The imaginary part of the sign is unspecified.  This special
	 * case is only needed to avoid a spurious invalid exception when
	 * y is infinite.
	 */
	if (ix >= 0x7ff00000) {
		if ((ix & 0xfffff) | lx)	/* x is NaN */
			return (CMPLX(x, (y == 0 ? y : x * y)));
		SET_HIGH_WORD(x, hx - 0x40000000);	/* x = ftl_olm_copysign(1, x) */
		return (CMPLX(x, ftl_olm_copysign(0, ftl_olm_isinf(y) ? y : ftl_olm_sin(y) * ftl_olm_cos(y))));
	}

	/*
	 * ftl_olm_ctanh(x + i NAN) = NaN + i NaN
	 * ftl_olm_ctanh(x +- i Inf) = NaN + i NaN
	 */
	if (!isfinite(y))
		return (CMPLX(y - y, y - y));

	/*
	 * ftl_olm_ctanh(+-huge + i +-y) ~= +-1 +- i 2sin(2y)/ftl_olm_exp(2x), using the
	 * approximation ftl_olm_sinh^2(huge) ~= ftl_olm_exp(2*huge) / 4.
	 * We use a modified formula to avoid spurious overflow.
	 */
	if (ix >= 0x40360000) {	/* x >= 22 */
		double exp_mx = ftl_olm_exp(-ftl_olm_fabs(x));
		return (CMPLX(ftl_olm_copysign(1, x),
		    4 * ftl_olm_sin(y) * ftl_olm_cos(y) * exp_mx * exp_mx));
	}

	/* Kahan's algorithm */
	t = ftl_olm_tan(y);
	beta = 1.0 + t * t;	/* = 1 / ftl_olm_cos^2(y) */
	s = ftl_olm_sinh(x);
	rho = ftl_olm_sqrt(1 + s * s);	/* = ftl_olm_cosh(x) */
	denom = 1 + beta * s * s;
	return (CMPLX((beta * rho * s) / denom, t / denom));
}

OLM_DLLEXPORT ftl_olm_complex<double>
ftl_olm_ctan(ftl_olm_complex<double> z)
{

	/* ftl_olm_ctan(z) = -I * ftl_olm_ctanh(I * z) */
	z = ftl_olm_ctanh(CMPLX(-ftl_olm_cimag(z), ftl_olm_creal(z)));
	return (CMPLX(ftl_olm_cimag(z), -ftl_olm_creal(z)));
}
