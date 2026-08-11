// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_C_FILE_TYPES_HEADER
#define FTL_C_FILE_TYPES_HEADER

struct ftl_file;

namespace ftl {
using FILE = ::ftl_file;
}

#ifdef FTL_REPLACE_STL
using FILE = ftl_file;
namespace std {
using FILE = ::ftl_file;
}
#define FTL_C_FILE_TYPE FILE
#else
#define FTL_C_FILE_TYPE ::ftl_file
#endif

#endif // FTL_C_FILE_TYPES_HEADER
