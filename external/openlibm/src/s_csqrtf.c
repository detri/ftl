/*-
 * Copyright (c) 2007 David Schultz <das@FreeBSD.ORG>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/s_csqrtf.c,v 1.3 2008/08/08 00:15:16 das Exp $");

#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

/*
 * gcc doesn't implement complex multiplication or division correctly,
 * so we need to handle infinities specially. We turn on this pragma to
 * notify conforming c99 compilers that the fast-but-incorrect code that
 * gcc generates is acceptable, since the special cases have already been
 * handled.
 */
#ifndef __GNUC__
#pragma	STDC CX_LIMITED_RANGE	ON
#endif

OLM_DLLEXPORT ftl_olm_complex<float>
ftl_olm_csqrtf(ftl_olm_complex<float> z)
{
	float a = ftl_olm_crealf(z), b = ftl_olm_cimagf(z);
	double t;

	/* Handle special cases. */
	if (z == 0)
		return (CMPLXF(0, b));
	if (ftl_olm_isinf(b))
		return (CMPLXF(INFINITY, b));
	if (ftl_olm_isnan(a)) {
		t = (b - b) / (b - b);	/* raise invalid if b is not a NaN */
		return (CMPLXF(a, t));	/* return NaN + NaN i */
	}
	if (ftl_olm_isinf(a)) {
		/*
		 * ftl_olm_csqrtf(inf + NaN i)  = inf +  NaN i
		 * ftl_olm_csqrtf(inf + y i)    = inf +  0 i
		 * ftl_olm_csqrtf(-inf + NaN i) = NaN +- inf i
		 * ftl_olm_csqrtf(-inf + y i)   = 0   +  inf i
		 */
		if (signbit(a))
			return (CMPLXF(ftl_olm_fabsf(b - b), ftl_olm_copysignf(a, b)));
		else
			return (CMPLXF(a, ftl_olm_copysignf(b - b, b)));
	}
	/*
	 * The remaining special case (b is NaN) is handled just fine by
	 * the normal code path below.
	 */

	/*
	 * We compute t in double precision to avoid overflow and to
	 * provide correct rounding in nearly all cases.
	 * This is Algorithm 312, CACM vol 10, Oct 1967.
	 */
	if (a >= 0) {
		t = ftl_olm_sqrt((a + ftl_olm_hypot(a, b)) * 0.5);
		return (CMPLXF(t, b / (2.0 * t)));
	} else {
		t = ftl_olm_sqrt((-a + ftl_olm_hypot(a, b)) * 0.5);
		return (CMPLXF(ftl_olm_fabsf(b) / (2.0 * t), ftl_olm_copysignf(t, b)));
	}
}
