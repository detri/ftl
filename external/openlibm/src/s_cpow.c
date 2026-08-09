/*	$OpenBSD: s_cpow.c,v 1.6 2013/07/03 04:46:36 espie Exp $	*/
/*
 * Copyright (c) 2008 Stephen L. Moshier <steve@moshier.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*							ftl_olm_cpow
 *
 *	Complex power function
 *
 *
 *
 * SYNOPSIS:
 *
 * ftl_olm_complex<double> ftl_olm_cpow();
 * ftl_olm_complex<double> a, z, w;
 *
 * w = ftl_olm_cpow (a, z);
 *
 *
 *
 * DESCRIPTION:
 *
 * Raises complex A to the complex Zth power.
 * Definition is per AMS55 # 4.2.8,
 * analytically equivalent to ftl_olm_cpow(a,z) = ftl_olm_cexp(z ftl_olm_clog(a)).
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       9.4e-15     1.5e-15
 *
 */

#include <float.h>
#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

OLM_DLLEXPORT ftl_olm_complex<double>
ftl_olm_cpow(ftl_olm_complex<double> a, ftl_olm_complex<double> z)
{
	ftl_olm_complex<double> w;
	double x, y, r, theta, absa, arga;

	x = ftl_olm_creal (z);
	y = ftl_olm_cimag (z);
	absa = ftl_olm_cabs (a);
	if (absa == 0.0) {
		return (0.0 + 0.0 * I);
	}
	arga = ftl_olm_carg (a);
	r = ftl_olm_pow (absa, x);
	theta = x * arga;
	if (y != 0.0) {
		r = r * ftl_olm_exp (-y * arga);
		theta = theta + y * ftl_olm_log (absa);
	}
	w = r * ftl_olm_cos (theta) + (r * ftl_olm_sin (theta)) * I;
	return (w);
}

#if	LDBL_MANT_DIG == DBL_MANT_DIG
openlibm_strong_reference(ftl_olm_cpow, ftl_olm_cpowl);
#endif	/* LDBL_MANT_DIG == DBL_MANT_DIG */
