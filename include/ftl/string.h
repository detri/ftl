// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_STRING_H_HEADER
#define FTL_STRING_H_HEADER

#ifdef FTL_REPLACE_STL
#include <cstring>
#define FTL_STRING_C_NAMESPACE std
#else
#include <ftl/cstring>
#define FTL_STRING_C_NAMESPACE ftl
#endif

using FTL_STRING_C_NAMESPACE::size_t;

using FTL_STRING_C_NAMESPACE::memcpy;
using FTL_STRING_C_NAMESPACE::memmove;
using FTL_STRING_C_NAMESPACE::strcpy;
using FTL_STRING_C_NAMESPACE::strncpy;

using FTL_STRING_C_NAMESPACE::strcat;
using FTL_STRING_C_NAMESPACE::strncat;

using FTL_STRING_C_NAMESPACE::memcmp;
using FTL_STRING_C_NAMESPACE::strcmp;
using FTL_STRING_C_NAMESPACE::strcoll;
using FTL_STRING_C_NAMESPACE::strncmp;
using FTL_STRING_C_NAMESPACE::strxfrm;

using FTL_STRING_C_NAMESPACE::memchr;
using FTL_STRING_C_NAMESPACE::strchr;
using FTL_STRING_C_NAMESPACE::strcspn;
using FTL_STRING_C_NAMESPACE::strpbrk;
using FTL_STRING_C_NAMESPACE::strrchr;
using FTL_STRING_C_NAMESPACE::strspn;
using FTL_STRING_C_NAMESPACE::strstr;
using FTL_STRING_C_NAMESPACE::strtok;

using FTL_STRING_C_NAMESPACE::memset;
using FTL_STRING_C_NAMESPACE::strerror;
using FTL_STRING_C_NAMESPACE::strlen;

#undef FTL_STRING_C_NAMESPACE

#endif // FTL_STRING_H_HEADER
