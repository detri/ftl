/*	$OpenBSD: s_clog.c,v 1.6 2013/07/03 04:46:36 espie Exp $	*/
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

/*							ftl_olm_clog.c
 *
 *	Complex natural logarithm
 *
 *
 *
 * SYNOPSIS:
 *
 * ftl_olm_complex<double> ftl_olm_clog();
 * ftl_olm_complex<double> z, w;
 *
 * w = ftl_olm_clog (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns complex logarithm to the base e (2.718...) of
 * the complex argument x.
 *
 * If z = x + iy, r = ftl_olm_sqrt( x**2 + y**2 ),
 * then
 *       w = ftl_olm_log(r) + i arctan(y/x).
 *
 * The arctangent ranges from -PI to +PI.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      7000       8.5e-17     1.9e-17
 *    IEEE      -10,+10     30000       5.0e-15     1.1e-16
 *
 * Larger relative error can be observed for z near 1 +i0.
 * In IEEE arithmetic the peak absolute error is 5.2e-16, rms
 * absolute error 1.0e-16.
 */

#include <float.h>
#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

ftl_olm_complex<double>
ftl_olm_clog(ftl_olm_complex<double> z)
{
	ftl_olm_complex<double> w;
	double p, rr;

	/*rr = ftl_olm_sqrt( z->r * z->r  +  z->i * z->i );*/
	rr = ftl_olm_cabs(z);
	p = ftl_olm_log(rr);
	rr = ftl_olm_atan2 (ftl_olm_cimag (z), ftl_olm_creal (z));
	w = p + rr * I;
	return (w);
}

#if	LDBL_MANT_DIG == DBL_MANT_DIG
openlibm_strong_reference(ftl_olm_clog, ftl_olm_clogl);
#endif	/* LDBL_MANT_DIG == DBL_MANT_DIG */
