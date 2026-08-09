/*	$OpenBSD: s_csinf.c,v 1.2 2010/07/18 18:42:26 guenther Exp $	*/
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

/*							ftl_olm_csinf()
 *
 *	Complex circular sine
 *
 *
 *
 * SYNOPSIS:
 *
 * void ftl_olm_csinf();
 * cmplxf z, w;
 *
 * ftl_olm_csinf( &z, &w );
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
 *     w = ftl_olm_sin x  ftl_olm_cosh y  +  i ftl_olm_cos x ftl_olm_sinh y.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       1.9e-7      5.5e-8
 *
 */

#include <openlibm_complex.h>
#include <openlibm_math.h>

/* calculate ftl_olm_cosh and ftl_olm_sinh */

static void
cchshf(float xx, float *c, float *s)
{
	float x, e, ei;

	x = xx;
	if(ftl_olm_fabsf(x) <= 0.5f) {
		*c = ftl_olm_coshf(x);
		*s = ftl_olm_sinhf(x);
	}
	else {
		e = ftl_olm_expf(x);
		ei = 0.5f/e;
		e = 0.5f * e;
		*s = e - ei;
		*c = e + ei;
	}
}

ftl_olm_complex<float>
ftl_olm_csinf(ftl_olm_complex<float> z)
{
	ftl_olm_complex<float> w;
	float ch, sh;

	cchshf(ftl_olm_cimagf(z), &ch, &sh);
	w = ftl_olm_sinf(ftl_olm_crealf(z)) * ch  + (ftl_olm_cosf(ftl_olm_crealf(z)) * sh) * I;
	return (w);
}
