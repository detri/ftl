/*
 * ftl_olm_cabs() wrapper for ftl_olm_hypot().
 *
 * Written by J.T. Conklin, <jtc@wimsey.com>
 * Placed into the Public Domain, 1994.
 */

#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/w_cabs.c,v 1.7 2008/03/30 20:03:06 das Exp $");

#include <float.h>
#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

OLM_DLLEXPORT double
ftl_olm_cabs(ftl_olm_complex<double> z)
{
	return ftl_olm_hypot(ftl_olm_creal(z), ftl_olm_cimag(z));
}

#if LDBL_MANT_DIG == 53
openlibm_weak_reference(ftl_olm_cabs, ftl_olm_cabsl);
#endif
