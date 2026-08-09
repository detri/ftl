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
 * Hyperbolic sine of a complex argument z.  See s_csinh.c for details.
 */

#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/s_csinhf.c,v 1.2 2011/10/21 06:29:32 das Exp $");

#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

static const float huge = 0x1p127;

OLM_DLLEXPORT ftl_olm_complex<float>
ftl_olm_csinhf(ftl_olm_complex<float> z)
{
	float x, y, h;
	int32_t hx, hy, ix, iy;

	x = ftl_olm_crealf(z);
	y = ftl_olm_cimagf(z);

	GET_FLOAT_WORD(hx, x);
	GET_FLOAT_WORD(hy, y);

	ix = 0x7fffffff & hx;
	iy = 0x7fffffff & hy;

	if (ix < 0x7f800000 && iy < 0x7f800000) {
		if (iy == 0)
			return (CMPLXF(ftl_olm_sinhf(x), y));
		if (ix < 0x41100000)	/* small x: normal case */
			return (CMPLXF(ftl_olm_sinhf(x) * ftl_olm_cosf(y), ftl_olm_coshf(x) * ftl_olm_sinf(y)));

		/* |x| >= 9, so ftl_olm_cosh(x) ~= ftl_olm_exp(|x|) */
		if (ix < 0x42b17218) {
			/* x < 88.7: ftl_olm_expf(|x|) won't overflow */
			h = ftl_olm_expf(ftl_olm_fabsf(x)) * 0.5f;
			return (CMPLXF(ftl_olm_copysignf(h, x) * ftl_olm_cosf(y), h * ftl_olm_sinf(y)));
		} else if (ix < 0x4340b1e7) {
			/* x < 192.7: scale to avoid overflow */
			z = ftl_olm___ldexp_cexpf(CMPLXF(ftl_olm_fabsf(x), y), -1);
			return (CMPLXF(ftl_olm_crealf(z) * ftl_olm_copysignf(1, x), ftl_olm_cimagf(z)));
		} else {
			/* x >= 192.7: the result always overflows */
			h = huge * x;
			return (CMPLXF(h * ftl_olm_cosf(y), h * h * ftl_olm_sinf(y)));
		}
	}

	if (ix == 0 && iy >= 0x7f800000)
		return (CMPLXF(ftl_olm_copysignf(0, x * (y - y)), y - y));

	if (iy == 0 && ix >= 0x7f800000) {
		if ((hx & 0x7fffff) == 0)
			return (CMPLXF(x, y));
		return (CMPLXF(x, ftl_olm_copysignf(0, y)));
	}

	if (ix < 0x7f800000 && iy >= 0x7f800000)
		return (CMPLXF(y - y, x * (y - y)));

	if (ix >= 0x7f800000 && (hx & 0x7fffff) == 0) {
		if (iy >= 0x7f800000)
			return (CMPLXF(x * x, x * (y - y)));
		return (CMPLXF(x * ftl_olm_cosf(y), INFINITY * ftl_olm_sinf(y)));
	}

	return (CMPLXF((x * x) * (y - y), (x + x) * (y - y)));
}

OLM_DLLEXPORT ftl_olm_complex<float>
ftl_olm_csinf(ftl_olm_complex<float> z)
{

	z = ftl_olm_csinhf(CMPLXF(-ftl_olm_cimagf(z), ftl_olm_crealf(z)));
	return (CMPLXF(ftl_olm_cimagf(z), -ftl_olm_crealf(z)));
}
