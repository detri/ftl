/*	$OpenBSD: s_cexpl.c,v 1.2 2011/07/20 19:28:33 martynas Exp $	*/

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

/*							ftl_olm_cexpl()
 *
 *	Complex exponential function
 *
 *
 *
 * SYNOPSIS:
 *
 * ftl_olm_complex<long double> ftl_olm_cexpl();
 * ftl_olm_complex<long double> z, w;
 *
 * w = ftl_olm_cexpl( z );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the exponential of the complex argument z
 * into the complex result w.
 *
 * If
 *     z = x + iy,
 *     r = ftl_olm_exp(x),
 *
 * then
 *
 *     w = r ftl_olm_cos y + i r ftl_olm_sin y.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      8700       3.7e-17     1.1e-17
 *    IEEE      -10,+10     30000       3.0e-16     8.7e-17
 *
 */

#include <openlibm_complex.h>
#include <openlibm_math.h>

ftl_olm_complex<long double>
ftl_olm_cexpl(ftl_olm_complex<long double> z)
{
	ftl_olm_complex<long double> w;
	long double r;

	r = ftl_olm_expl(ftl_olm_creall(z));
	w = r * ftl_olm_cosl(ftl_olm_cimagl(z)) + (r * ftl_olm_sinl(ftl_olm_cimagl(z))) * I;
	return (w);
}
