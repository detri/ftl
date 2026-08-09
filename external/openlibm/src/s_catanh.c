/*	$OpenBSD: s_catanh.c,v 1.6 2013/07/03 04:46:36 espie Exp $	*/
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

/*							ftl_olm_catanh
 *
 *	Complex inverse hyperbolic tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * ftl_olm_complex<double> ftl_olm_catanh();
 * ftl_olm_complex<double> z, w;
 *
 * w = ftl_olm_catanh (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * Inverse ftl_olm_tanh, equal to  -i ftl_olm_catan (iz);
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       2.3e-16     6.2e-17
 *
 */

#include <float.h>
#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

ftl_olm_complex<double>
ftl_olm_catanh(ftl_olm_complex<double> z)
{
	ftl_olm_complex<double> w;

	w = -1.0 * I * ftl_olm_catan (z * I);
	return (w);
}

#if	LDBL_MANT_DIG == DBL_MANT_DIG
openlibm_strong_reference(ftl_olm_catanh, ftl_olm_catanhl);
#endif	/* LDBL_MANT_DIG == DBL_MANT_DIG */
