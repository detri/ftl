// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_C_FILE_TYPES_HEADER
#define FTL_C_FILE_TYPES_HEADER

struct ftl_file;

using FILE = ftl_file;
namespace std {
using FILE = ::ftl_file;
}
#define FTL_C_FILE_TYPE FILE

#endif // FTL_C_FILE_TYPES_HEADER
