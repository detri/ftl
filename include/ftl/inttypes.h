// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_INTTYPES_H_HEADER
#define FTL_INTTYPES_H_HEADER

#ifdef FTL_REPLACE_STL
#include <stdint.h>
#include <cinttypes>
#define FTL_INTTYPES_NAMESPACE std
#else
#include <ftl/stdint.h>
#include <ftl/cinttypes>
#define FTL_INTTYPES_NAMESPACE ftl
#endif

using FTL_INTTYPES_NAMESPACE::imaxdiv_t;
using FTL_INTTYPES_NAMESPACE::imaxabs;
using FTL_INTTYPES_NAMESPACE::imaxdiv;

/*
 * strtoimax, strtoumax, wcstoimax, and wcstoumax are already declared
 * in the global namespace by <cinttypes>.
 */

#undef FTL_INTTYPES_NAMESPACE

#endif // FTL_INTTYPES_H_HEADER
