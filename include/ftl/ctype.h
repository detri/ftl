// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_CTYPE_H_HEADER
#define FTL_CTYPE_H_HEADER

#ifdef FTL_REPLACE_STL
#include <cctype>
#else
#include <ftl/cctype>
#endif

/*
 * <cctype> above deliberately declares the underlying C functions in the
 * global namespace before importing them into std/ftl, so the required
 * <ctype.h> global declarations are already present here.
 */

#endif // FTL_CTYPE_H_HEADER
