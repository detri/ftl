/*	$OpenBSD: s_ccos.c,v 1.6 2013/07/03 04:46:36 espie Exp $	*/
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

/*							ftl_olm_ccos()
 *
 *	Complex circular cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * ftl_olm_complex<double> ftl_olm_ccos();
 * ftl_olm_complex<double> z, w;
 *
 * w = ftl_olm_ccos (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *
 *     w = ftl_olm_cos x  ftl_olm_cosh y  -  i ftl_olm_sin x ftl_olm_sinh y.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      8400       4.5e-17     1.3e-17
 *    IEEE      -10,+10     30000       3.8e-16     1.0e-16
 */

#include <float.h>
#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

/* calculate ftl_olm_cosh and ftl_olm_sinh */

static void
_cchsh(double x, double *c, double *s)
{
	double e, ei;

	if (ftl_olm_fabs(x) <= 0.5) {
		*c = ftl_olm_cosh(x);
		*s = ftl_olm_sinh(x);
	}
	else {
		e = ftl_olm_exp(x);
		ei = 0.5/e;
		e = 0.5 * e;
		*s = e - ei;
		*c = e + ei;
	}
}

ftl_olm_complex<double>
ftl_olm_ccos(ftl_olm_complex<double> z)
{
	ftl_olm_complex<double> w;
	double ch, sh;

	_cchsh( ftl_olm_cimag(z), &ch, &sh );
	w = ftl_olm_cos(ftl_olm_creal (z)) * ch - (ftl_olm_sin (ftl_olm_creal (z)) * sh) * I;
	return (w);
}

#if	LDBL_MANT_DIG == DBL_MANT_DIG
openlibm_strong_reference(ftl_olm_ccos, ftl_olm_ccosl);
#endif	/* LDBL_MANT_DIG == DBL_MANT_DIG */
