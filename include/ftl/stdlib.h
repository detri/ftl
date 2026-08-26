// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_STDLIB_H_HEADER
#define FTL_STDLIB_H_HEADER

#include <cstdlib>

using std::size_t;

using std::div_t;
using std::ldiv_t;
using std::lldiv_t;

using std::abs;
using std::labs;
using std::llabs;

using std::div;
using std::ldiv;
using std::lldiv;

#if defined(_WIN32)
using std::aligned_alloc;
#endif


#endif // FTL_STDLIB_H_HEADER
