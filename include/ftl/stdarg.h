// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_STDARG_H_HEADER
#define FTL_STDARG_H_HEADER

#ifdef FTL_REPLACE_STL
#include <cstdarg>
#define FTL_STDARG_NAMESPACE std
#else
#include <ftl/cstdarg>
#define FTL_STDARG_NAMESPACE ftl
#endif

using FTL_STDARG_NAMESPACE::va_list;

#undef FTL_STDARG_NAMESPACE

#endif // FTL_STDARG_H_HEADER
