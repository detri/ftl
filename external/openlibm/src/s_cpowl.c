/*	$OpenBSD: s_cpowl.c,v 1.2 2011/07/20 19:28:33 martynas Exp $	*/

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

/*							ftl_olm_cpowl
 *
 *	Complex power function
 *
 *
 *
 * SYNOPSIS:
 *
 * ftl_olm_complex<long double> ftl_olm_cpowl();
 * ftl_olm_complex<long double> a, z, w;
 *
 * w = ftl_olm_cpowl (a, z);
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

#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

OLM_DLLEXPORT ftl_olm_complex<long double>
ftl_olm_cpowl(ftl_olm_complex<long double> a, ftl_olm_complex<long double> z)
{
	ftl_olm_complex<long double> w;
	long double x, y, r, theta, absa, arga;

	x = ftl_olm_creall(z);
	y = ftl_olm_cimagl(z);
	absa = ftl_olm_cabsl(a);
	if (absa == 0.0L) {
		return (0.0L + 0.0L * I);
	}
	arga = ftl_olm_cargl(a);
	r = ftl_olm_powl(absa, x);
	theta = x * arga;
	if (y != 0.0L) {
		r = r * ftl_olm_expl(-y * arga);
		theta = theta + y * ftl_olm_logl(absa);
	}
	w = r * ftl_olm_cosl(theta) + (r * ftl_olm_sinl(theta)) * I;
	return (w);
}
