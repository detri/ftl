// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_UCHAR_H_HEADER
#define FTL_UCHAR_H_HEADER

#ifdef FTL_REPLACE_STL
#include <cuchar>
#define FTL_UCHAR_NAMESPACE std
#else
#include <ftl/cuchar>
#define FTL_UCHAR_NAMESPACE ftl
#endif

using FTL_UCHAR_NAMESPACE::mbstate_t;
using FTL_UCHAR_NAMESPACE::size_t;

using FTL_UCHAR_NAMESPACE::c16rtomb;
using FTL_UCHAR_NAMESPACE::c32rtomb;
using FTL_UCHAR_NAMESPACE::c8rtomb;
using FTL_UCHAR_NAMESPACE::mbrtoc16;
using FTL_UCHAR_NAMESPACE::mbrtoc32;
using FTL_UCHAR_NAMESPACE::mbrtoc8;

#undef FTL_UCHAR_NAMESPACE

#endif // FTL_UCHAR_H_HEADER
