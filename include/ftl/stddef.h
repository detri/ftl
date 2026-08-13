// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_STDDEF_H_HEADER
#define FTL_STDDEF_H_HEADER

#ifdef FTL_REPLACE_STL
#include <cstddef>
#define FTL_STDDEF_NAMESPACE std
#else
#include <ftl/cstddef>
#define FTL_STDDEF_NAMESPACE ftl
#endif

using FTL_STDDEF_NAMESPACE::max_align_t;
using FTL_STDDEF_NAMESPACE::nullptr_t;
using FTL_STDDEF_NAMESPACE::ptrdiff_t;
using FTL_STDDEF_NAMESPACE::size_t;

#undef FTL_STDDEF_NAMESPACE

#endif // FTL_STDDEF_H_HEADER
