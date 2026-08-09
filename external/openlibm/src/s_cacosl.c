/*	$OpenBSD: s_cacosl.c,v 1.3 2011/07/20 21:02:51 martynas Exp $	*/

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

/*							ftl_olm_cacosl()
 *
 *	Complex circular arc cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * ftl_olm_complex<long double> ftl_olm_cacosl();
 * ftl_olm_complex<long double> z, w;
 *
 * w = ftl_olm_cacosl( z );
 *
 *
 *
 * DESCRIPTION:
 *
 *
 * w = arccos z  =  PI/2 - arcsin z.
 *
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      5200      1.6e-15      2.8e-16
 *    IEEE      -10,+10     30000      1.8e-14      2.2e-15
 */

#include <openlibm_complex.h>
#include <openlibm_math.h>

static const long double PIO2L = 1.570796326794896619231321691639751442098585L;

ftl_olm_complex<long double>
ftl_olm_cacosl(ftl_olm_complex<long double> z)
{
	ftl_olm_complex<long double> w;

	w = ftl_olm_casinl(z);
	w = (PIO2L - ftl_olm_creall(w)) - ftl_olm_cimagl(w) * I;
	return (w);
}
