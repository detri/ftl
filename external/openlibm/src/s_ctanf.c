/*	$OpenBSD: s_ctanf.c,v 1.2 2011/07/20 19:28:33 martynas Exp $	*/
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

/*							ftl_olm_ctanf()
 *
 *	Complex circular tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * void ftl_olm_ctanf();
 * cmplxf z, w;
 *
 * ftl_olm_ctanf( &z, &w );
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
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       3.3e-7       5.1e-8
 */

#include <openlibm_complex.h>
#include <openlibm_math.h>

#define MACHEPF 3.0e-8
#define MAXNUMF 1.0e38f

static const double DP1 = 3.140625;
static const double DP2 = 9.67502593994140625E-4;
static const double DP3 = 1.509957990978376432E-7;

static float
_redupif(float xx)
{
	float x, t;
	long i;

	x = xx;
	t = x/(float)M_PI;
	if(t >= 0.0)
		t += 0.5;
	else
		t -= 0.5;

	i = t;	/* the multiple */
	t = i;
	t = ((x - t * DP1) - t * DP2) - t * DP3;
	return(t);
}

/*  Taylor series expansion for ftl_olm_cosh(2y) - ftl_olm_cos(2x)	*/

static float
_ctansf(ftl_olm_complex<float> z)
{
	float f, x, x2, y, y2, rn, t, d;

	x = ftl_olm_fabsf(2.0f * ftl_olm_crealf(z));
	y = ftl_olm_fabsf(2.0f * ftl_olm_cimagf(z));

	x = _redupif(x);

	x = x * x;
	y = y * y;
	x2 = 1.0f;
	y2 = 1.0f;
	f = 1.0f;
	rn = 0.0f;
	d = 0.0f;
	do {
		rn += 1.0f;
		f *= rn;
		rn += 1.0f;
		f *= rn;
		x2 *= x;
		y2 *= y;
		t = y2 + x2;
		t /= f;
		d += t;

		rn += 1.0f;
		f *= rn;
		rn += 1.0f;
		f *= rn;
		x2 *= x;
		y2 *= y;
		t = y2 - x2;
		t /= f;
		d += t;
	}
	while (ftl_olm_fabsf(t/d) > MACHEPF)
		;
	return(d);
}

ftl_olm_complex<float>
ftl_olm_ctanf(ftl_olm_complex<float> z)
{
	ftl_olm_complex<float> w;
	float d;

	d = ftl_olm_cosf( 2.0f * ftl_olm_crealf(z) ) + ftl_olm_coshf( 2.0f * ftl_olm_cimagf(z) );

	if(ftl_olm_fabsf(d) < 0.25f)
		d = _ctansf(z);

	if (d == 0.0f) {
		/*mtherr( "ftl_olm_ctanf", OVERFLOW );*/
		w = MAXNUMF + MAXNUMF * I;
		return (w);
	}
	w = ftl_olm_sinf (2.0f * ftl_olm_crealf(z)) / d + (ftl_olm_sinhf (2.0f * ftl_olm_cimagf(z)) / d) * I;
	return (w);
}
