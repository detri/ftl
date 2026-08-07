// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_C_FILE_TYPES_HEADER
#define FTL_C_FILE_TYPES_HEADER

#if defined(_WIN32)

struct _iobuf;
using FILE = _iobuf;

#elif defined(__APPLE__)

struct __sFILE;
using FILE = __sFILE;

#else

struct _IO_FILE;
using FILE = _IO_FILE;

#endif

#endif // FTL_C_FILE_TYPES_HEADER
