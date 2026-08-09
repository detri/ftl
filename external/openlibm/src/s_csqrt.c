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
//__FBSDID("$FreeBSD: src/lib/msun/src/s_csqrt.c,v 1.4 2008/08/08 00:15:16 das Exp $");

#include <float.h>
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

/* We risk spurious overflow for components >= DBL_MAX / (1 + ftl_olm_sqrt(2)). */
#define	THRESH	0x1.a827999fcef32p+1022

OLM_DLLEXPORT ftl_olm_complex<double>
ftl_olm_csqrt(ftl_olm_complex<double> z)
{
	ftl_olm_complex<double> result;
	double a, b;
	double t;
	int scale;

	a = ftl_olm_creal(z);
	b = ftl_olm_cimag(z);

	/* Handle special cases. */
	if (z == 0)
		return (CMPLX(0, b));
	if (ftl_olm_isinf(b))
		return (CMPLX(INFINITY, b));
	if (ftl_olm_isnan(a)) {
		t = (b - b) / (b - b);	/* raise invalid if b is not a NaN */
		return (CMPLX(a, t));	/* return NaN + NaN i */
	}
	if (ftl_olm_isinf(a)) {
		/*
		 * ftl_olm_csqrt(inf + NaN i)  = inf +  NaN i
		 * ftl_olm_csqrt(inf + y i)    = inf +  0 i
		 * ftl_olm_csqrt(-inf + NaN i) = NaN +- inf i
		 * ftl_olm_csqrt(-inf + y i)   = 0   +  inf i
		 */
		if (signbit(a))
			return (CMPLX(ftl_olm_fabs(b - b), ftl_olm_copysign(a, b)));
		else
			return (CMPLX(a, ftl_olm_copysign(b - b, b)));
	}
	/*
	 * The remaining special case (b is NaN) is handled just fine by
	 * the normal code path below.
	 */

	/* Scale to avoid overflow. */
	if (ftl_olm_fabs(a) >= THRESH || ftl_olm_fabs(b) >= THRESH) {
		a *= 0.25;
		b *= 0.25;
		scale = 1;
	} else {
		scale = 0;
	}

	/* Algorithm 312, CACM vol 10, Oct 1967. */
	if (a >= 0) {
		t = ftl_olm_sqrt((a + ftl_olm_hypot(a, b)) * 0.5);
		result = CMPLX(t, b / (2 * t));
	} else {
		t = ftl_olm_sqrt((-a + ftl_olm_hypot(a, b)) * 0.5);
		result = CMPLX(ftl_olm_fabs(b) / (2 * t), ftl_olm_copysign(t, b));
	}

	/* Rescale. */
	if (scale)
		return (result * 2);
	else
		return (result);
}

#if LDBL_MANT_DIG == 53
openlibm_weak_reference(ftl_olm_csqrt, ftl_olm_csqrtl);
#endif
