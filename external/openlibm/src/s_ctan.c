/*	$OpenBSD: s_ctan.c,v 1.6 2013/07/03 04:46:36 espie Exp $	*/
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

/*							ftl_olm_ctan()
 *
 *	Complex circular tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * ftl_olm_complex<double> ftl_olm_ctan();
 * ftl_olm_complex<double> z, w;
 *
 * w = ftl_olm_ctan (z);
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
 *           ftl_olm_sin 2x  +  i ftl_olm_sinh 2y
 *     w  =  --------------------.
 *            ftl_olm_cos 2x  +  ftl_olm_cosh 2y
 *
 * On the real axis the denominator is zero at odd multiples
 * of PI/2.  The denominator is evaluated by its Taylor
 * series near these points.
 *
 * ftl_olm_ctan(z) = -i ftl_olm_ctanh(iz).
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      5200       7.1e-17     1.6e-17
 *    IEEE      -10,+10     30000       7.2e-16     1.2e-16
 * Also tested by ftl_olm_ctan * ccot = 1 and ftl_olm_catan(ftl_olm_ctan(z))  =  z.
 */

#include <float.h>
#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

#define MACHEP 1.1e-16
#define MAXNUM 1.0e308

static const double DP1 = 3.14159265160560607910E0;
static const double DP2 = 1.98418714791870343106E-9;
static const double DP3 = 1.14423774522196636802E-17;

static double
_redupi(double x)
{
	double t;
	long i;

	t = x/M_PI;
	if (t >= 0.0)
		t += 0.5;
	else
		t -= 0.5;

	i = t;	/* the multiple */
	t = i;
	t = ((x - t * DP1) - t * DP2) - t * DP3;
	return (t);
}

/*  Taylor series expansion for ftl_olm_cosh(2y) - ftl_olm_cos(2x)	*/

static double
_ctans(ftl_olm_complex<double> z)
{
	double f, x, x2, y, y2, rn, t;
	double d;

	x = ftl_olm_fabs (2.0 * ftl_olm_creal (z));
	y = ftl_olm_fabs (2.0 * ftl_olm_cimag(z));

	x = _redupi(x);

	x = x * x;
	y = y * y;
	x2 = 1.0;
	y2 = 1.0;
	f = 1.0;
	rn = 0.0;
	d = 0.0;
	do {
		rn += 1.0;
		f *= rn;
		rn += 1.0;
		f *= rn;
		x2 *= x;
		y2 *= y;
		t = y2 + x2;
		t /= f;
		d += t;

		rn += 1.0;
		f *= rn;
		rn += 1.0;
		f *= rn;
		x2 *= x;
		y2 *= y;
		t = y2 - x2;
		t /= f;
		d += t;
	}
	while (ftl_olm_fabs(t/d) > MACHEP)
		;
	return (d);
}

ftl_olm_complex<double>
ftl_olm_ctan(ftl_olm_complex<double> z)
{
	ftl_olm_complex<double> w;
	double d;

	d = ftl_olm_cos (2.0 * ftl_olm_creal (z)) + ftl_olm_cosh (2.0 * ftl_olm_cimag (z));

	if (ftl_olm_fabs(d) < 0.25)
		d = _ctans (z);

	if (d == 0.0) {
		/*mtherr ("ftl_olm_ctan", OVERFLOW);*/
		w = MAXNUM + MAXNUM * I;
		return (w);
	}

	w = ftl_olm_sin (2.0 * ftl_olm_creal(z)) / d + (ftl_olm_sinh (2.0 * ftl_olm_cimag(z)) / d) * I;
	return (w);
}

#if	LDBL_MANT_DIG == DBL_MANT_DIG
openlibm_strong_reference(ftl_olm_ctan, ftl_olm_ctanl);
#endif	/* LDBL_MANT_DIG == DBL_MANT_DIG */
