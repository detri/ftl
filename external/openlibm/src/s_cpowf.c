/*	$OpenBSD: s_cpowf.c,v 1.2 2010/07/18 18:42:26 guenther Exp $	*/
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

/*							ftl_olm_cpowf
 *
 *	Complex power function
 *
 *
 *
 * SYNOPSIS:
 *
 * ftl_olm_complex<float> ftl_olm_cpowf();
 * ftl_olm_complex<float> a, z, w;
 *
 * w = ftl_olm_cpowf (a, z);
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

OLM_DLLEXPORT ftl_olm_complex<float>
ftl_olm_cpowf(ftl_olm_complex<float> a, ftl_olm_complex<float> z)
{
	ftl_olm_complex<float> w;
	float x, y, r, theta, absa, arga;

	x = ftl_olm_crealf(z);
	y = ftl_olm_cimagf(z);
	absa = ftl_olm_cabsf (a);
	if (absa == 0.0f) {
		return (0.0f + 0.0f * I);
	}
	arga = ftl_olm_cargf (a);
	r = ftl_olm_powf (absa, x);
	theta = x * arga;
	if (y != 0.0f) {
		r = r * ftl_olm_expf (-y * arga);
		theta = theta + y * ftl_olm_logf (absa);
	}
	w = r * ftl_olm_cosf (theta) + (r * ftl_olm_sinf (theta)) * I;
	return (w);
}
