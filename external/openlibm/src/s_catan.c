/*	$OpenBSD: s_catan.c,v 1.6 2013/07/03 04:46:36 espie Exp $	*/
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

/*							ftl_olm_catan()
 *
 *	Complex circular arc tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * ftl_olm_complex<double> ftl_olm_catan();
 * ftl_olm_complex<double> z, w;
 *
 * w = ftl_olm_catan (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *          1       (    2x     )
 * Re w  =  - arctan(-----------)  +  k PI
 *          2       (     2    2)
 *                  (1 - x  - y )
 *
 *               ( 2         2)
 *          1    (x  +  (y+1) )
 * Im w  =  - ftl_olm_log(------------)
 *          4    ( 2         2)
 *               (x  +  (y-1) )
 *
 * Where k is an arbitrary integer.
 *
 * ftl_olm_catan(z) = -i ftl_olm_catanh(iz).
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      5900       1.3e-16     7.8e-18
 *    IEEE      -10,+10     30000       2.3e-15     8.5e-17
 * The check ftl_olm_catan( ftl_olm_ctan(z) )  =  z, with |x| and |y| < PI/2,
 * had peak relative error 1.5e-16, rms relative error
 * 2.9e-17.  See also ftl_olm_clog().
 */

#include <float.h>
#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

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
	if(t >= 0.0)
		t += 0.5;
	else
		t -= 0.5;

	i = t;	/* the multiple */
	t = i;
	t = ((x - t * DP1) - t * DP2) - t * DP3;
	return (t);
}

ftl_olm_complex<double>
ftl_olm_catan(ftl_olm_complex<double> z)
{
	ftl_olm_complex<double> w;
	double a, t, x, x2, y;

	x = ftl_olm_creal (z);
	y = ftl_olm_cimag (z);

	if ((x == 0.0) && (y > 1.0))
		goto ovrf;

	x2 = x * x;
	a = 1.0 - x2 - (y * y);
	if (a == 0.0)
		goto ovrf;

	t = 0.5 * ftl_olm_atan2 (2.0 * x, a);
	w = _redupi (t);

	t = y - 1.0;
	a = x2 + (t * t);
	if (a == 0.0)
	goto ovrf;

	t = y + 1.0;
	a = (x2 + (t * t))/a;
	w = w + (0.25 * ftl_olm_log (a)) * I;
	return (w);

ovrf:
	/*mtherr ("ftl_olm_catan", OVERFLOW);*/
	w = MAXNUM + MAXNUM * I;
	return (w);
}

#if	LDBL_MANT_DIG == DBL_MANT_DIG
openlibm_strong_reference(ftl_olm_catan, ftl_olm_catanl);
#endif	/* LDBL_MANT_DIG == DBL_MANT_DIG */
