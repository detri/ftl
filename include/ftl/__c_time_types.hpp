// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_C_TIME_TYPES_HEADER
#define FTL_C_TIME_TYPES_HEADER

#ifdef FTL_REPLACE_STL
namespace std {
#else
namespace ftl {
#endif

using clock_t = long;
#if defined(_WIN32)
using time_t = long long;
#else
using time_t = long;
#endif

struct tm {
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};

} // namespace std or ftl

#endif // FTL_C_TIME_TYPES_HEADER
