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
 * Hyperbolic tangent of a complex argument z.  See s_ctanh.c for details.
 */

#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/s_ctanhf.c,v 1.2 2011/10/21 06:30:16 das Exp $");

#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

OLM_DLLEXPORT ftl_olm_complex<float>
ftl_olm_ctanhf(ftl_olm_complex<float> z)
{
	float x, y;
	float t, beta, s, rho, denom;
	u_int32_t hx, ix;

	x = ftl_olm_crealf(z);
	y = ftl_olm_cimagf(z);

	GET_FLOAT_WORD(hx, x);
	ix = hx & 0x7fffffff;

	if (ix >= 0x7f800000) {
		if (ix & 0x7fffff)
			return (CMPLXF(x, (y == 0 ? y : x * y)));
		SET_FLOAT_WORD(x, hx - 0x40000000);
		return (CMPLXF(x,
		    ftl_olm_copysignf(0, ftl_olm_isinf(y) ? y : ftl_olm_sinf(y) * ftl_olm_cosf(y))));
	}

	if (!isfinite(y))
		return (CMPLXF(y - y, y - y));

	if (ix >= 0x41300000) {	/* x >= 11 */
		float exp_mx = ftl_olm_expf(-ftl_olm_fabsf(x));
		return (CMPLXF(ftl_olm_copysignf(1, x),
		    4 * ftl_olm_sinf(y) * ftl_olm_cosf(y) * exp_mx * exp_mx));
	}

	t = ftl_olm_tanf(y);
	beta = 1.0 + t * t;
	s = ftl_olm_sinhf(x);
	rho = ftl_olm_sqrtf(1 + s * s);
	denom = 1 + beta * s * s;
	return (CMPLXF((beta * rho * s) / denom, t / denom));
}

OLM_DLLEXPORT ftl_olm_complex<float>
ftl_olm_ctanf(ftl_olm_complex<float> z)
{

	z = ftl_olm_ctanhf(CMPLXF(-ftl_olm_cimagf(z), ftl_olm_crealf(z)));
	return (CMPLXF(ftl_olm_cimagf(z), -ftl_olm_crealf(z)));
}

