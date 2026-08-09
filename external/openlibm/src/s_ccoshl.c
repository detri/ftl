/*	$OpenBSD: s_ccoshl.c,v 1.2 2011/07/20 19:28:33 martynas Exp $	*/

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

/*							ftl_olm_ccoshl
 *
 *	Complex hyperbolic cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * ftl_olm_complex<long double> ftl_olm_ccoshl();
 * ftl_olm_complex<long double> z, w;
 *
 * w = ftl_olm_ccoshl (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * ftl_olm_ccosh(z) = ftl_olm_cosh x  ftl_olm_cos y + i ftl_olm_sinh x ftl_olm_sin y .
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       2.9e-16     8.1e-17
 *
 */

#include <openlibm_complex.h>
#include <openlibm_math.h>

ftl_olm_complex<long double>
ftl_olm_ccoshl(ftl_olm_complex<long double> z)
{
	ftl_olm_complex<long double> w;
	long double x, y;

	x = ftl_olm_creall(z);
	y = ftl_olm_cimagl(z);
	w = ftl_olm_coshl(x) * ftl_olm_cosl(y) + (ftl_olm_sinhl(x) * ftl_olm_sinl(y)) * I;
	return (w);
}
