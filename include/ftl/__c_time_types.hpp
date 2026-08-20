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

struct timespec {
  time_t tv_sec;
  long tv_nsec;
};

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
#if !defined(_WIN32)
  // glibc, musl, and Darwin all expose the POSIX/BSD extension slots in the
  // native ABI even when their source-level spellings are feature-macro
  // controlled. Keep this object layout usable by native mktime/strftime.
  long tm_gmtoff;
  const char *tm_zone;
#endif
};

} // namespace std or ftl

#endif // FTL_C_TIME_TYPES_HEADER
