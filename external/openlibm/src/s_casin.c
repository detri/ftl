/*	$OpenBSD: s_casin.c,v 1.6 2013/07/03 04:46:36 espie Exp $	*/
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

/*							ftl_olm_casin()
 *
 *	Complex circular arc sine
 *
 *
 *
 * SYNOPSIS:
 *
 * ftl_olm_complex<double> ftl_olm_casin();
 * ftl_olm_complex<double> z, w;
 *
 * w = ftl_olm_casin (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * Inverse complex sine:
 *
 *                               2
 * w = -i ftl_olm_clog( iz + ftl_olm_csqrt( 1 - z ) ).
 *
 * ftl_olm_casin(z) = -i ftl_olm_casinh(iz)
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10     10100       2.1e-15     3.4e-16
 *    IEEE      -10,+10     30000       2.2e-14     2.7e-15
 * Larger relative error can be observed for z near zero.
 * Also tested by ftl_olm_csin(ftl_olm_casin(z)) = z.
 */

#include <float.h>
#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

ftl_olm_complex<double>
ftl_olm_casin(ftl_olm_complex<double> z)
{
	ftl_olm_complex<double> w;
	static ftl_olm_complex<double> ca, ct, zz, z2;
	double x, y;

	x = ftl_olm_creal (z);
	y = ftl_olm_cimag (z);

	if (y == 0.0) {
		if (ftl_olm_fabs(x) > 1.0) {
			w = M_PI_2 + 0.0 * I;
			/*mtherr ("ftl_olm_casin", DOMAIN);*/
		}
		else {
			w = ftl_olm_asin (x) + 0.0 * I;
		}
		return (w);
	}

	/* Power series expansion */
	/*
	b = ftl_olm_cabs(z);
	if( b < 0.125 ) {
		z2.r = (x - y) * (x + y);
		z2.i = 2.0 * x * y;

		cn = 1.0;
		n = 1.0;
		ca.r = x;
		ca.i = y;
		sum.r = x;
		sum.i = y;
		do {
			ct.r = z2.r * ca.r  -  z2.i * ca.i;
			ct.i = z2.r * ca.i  +  z2.i * ca.r;
			ca.r = ct.r;
			ca.i = ct.i;

			cn *= n;
			n += 1.0;
			cn /= n;
			n += 1.0;
			b = cn/n;

			ct.r *= b;
			ct.i *= b;
			sum.r += ct.r;
			sum.i += ct.i;
			b = ftl_olm_fabs(ct.r) + ftl_olm_fabs(ct.i);
		}
		while( b > MACHEP );
		w->r = sum.r;
		w->i = sum.i;
		return;
	}
	*/

	ca = x + y * I;
	ct = ca * I;
	/* ftl_olm_sqrt( 1 - z*z) */
	/* cmul( &ca, &ca, &zz ) */
	/*x * x  -  y * y */
	zz = (x - y) * (x + y) + (2.0 * x * y) * I;

	zz = 1.0 - ftl_olm_creal(zz) - ftl_olm_cimag(zz) * I;
	z2 = ftl_olm_csqrt (zz);

	zz = ct + z2;
	zz = ftl_olm_clog (zz);
	/* multiply by 1/i = -i */
	w = zz * (-1.0 * I);
	return (w);
}

#if	LDBL_MANT_DIG == DBL_MANT_DIG
openlibm_strong_reference(ftl_olm_casin, ftl_olm_casinl);
#endif	/* LDBL_MANT_DIG == DBL_MANT_DIG */
