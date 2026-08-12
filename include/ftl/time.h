// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_TIME_H_HEADER
#define FTL_TIME_H_HEADER

#ifdef FTL_REPLACE_STL
#include <ctime>
#define FTL_TIME_NAMESPACE std
#else
#include <ftl/ctime>
#define FTL_TIME_NAMESPACE ftl
#endif

using FTL_TIME_NAMESPACE::clock_t;
using FTL_TIME_NAMESPACE::size_t;
using FTL_TIME_NAMESPACE::time_t;
using FTL_TIME_NAMESPACE::tm;

using FTL_TIME_NAMESPACE::asctime;
using FTL_TIME_NAMESPACE::clock;
using FTL_TIME_NAMESPACE::ctime;
using FTL_TIME_NAMESPACE::difftime;
using FTL_TIME_NAMESPACE::gmtime;
using FTL_TIME_NAMESPACE::localtime;
using FTL_TIME_NAMESPACE::mktime;
using FTL_TIME_NAMESPACE::strftime;
using FTL_TIME_NAMESPACE::time;

#undef FTL_TIME_NAMESPACE

#endif // FTL_TIME_H_HEADER
