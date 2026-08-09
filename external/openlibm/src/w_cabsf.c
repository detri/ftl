/*
 * ftl_olm_cabsf() wrapper for ftl_olm_hypotf().
 *
 * Written by J.T. Conklin, <jtc@wimsey.com>
 * Placed into the Public Domain, 1994.
 */

#include <openlibm_complex.h>
#include <openlibm_math.h>

#include "math_private.h"

OLM_DLLEXPORT float
ftl_olm_cabsf(ftl_olm_complex<float> z)
{

	return ftl_olm_hypotf(ftl_olm_crealf(z), ftl_olm_cimagf(z));
}
