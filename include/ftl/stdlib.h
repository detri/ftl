// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_STDLIB_H_HEADER
#define FTL_STDLIB_H_HEADER

#ifdef FTL_REPLACE_STL
#include <cstdlib>
#define FTL_STDLIB_NAMESPACE std
#else
#include <ftl/cstdlib>
#define FTL_STDLIB_NAMESPACE ftl
#endif

using FTL_STDLIB_NAMESPACE::size_t;

using FTL_STDLIB_NAMESPACE::div_t;
using FTL_STDLIB_NAMESPACE::ldiv_t;
using FTL_STDLIB_NAMESPACE::lldiv_t;

using FTL_STDLIB_NAMESPACE::abs;
using FTL_STDLIB_NAMESPACE::labs;
using FTL_STDLIB_NAMESPACE::llabs;

using FTL_STDLIB_NAMESPACE::div;
using FTL_STDLIB_NAMESPACE::ldiv;
using FTL_STDLIB_NAMESPACE::lldiv;

#if defined(_WIN32)
using FTL_STDLIB_NAMESPACE::aligned_alloc;
#endif

#undef FTL_STDLIB_NAMESPACE

#endif // FTL_STDLIB_H_HEADER
