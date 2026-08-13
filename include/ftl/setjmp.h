// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_SETJMP_H_HEADER
#define FTL_SETJMP_H_HEADER

#ifdef FTL_REPLACE_STL
#include <csetjmp>
#define FTL_SETJMP_NAMESPACE std
#else
#include <ftl/csetjmp>
#define FTL_SETJMP_NAMESPACE ftl
#endif

using FTL_SETJMP_NAMESPACE::jmp_buf;

#undef FTL_SETJMP_NAMESPACE

#endif // FTL_SETJMP_H_HEADER
