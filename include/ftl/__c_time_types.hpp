// Freestanding Template Library
// SPDX-License-Identifier: MIT

#ifndef FTL_C_TIME_TYPES_HEADER
#define FTL_C_TIME_TYPES_HEADER

#if defined(__APPLE__)
#include <sys/_types/_timespec.h>
#endif

namespace std
{
#if defined(__APPLE__)
    using clock_t = unsigned long;
#else
    using clock_t = long;
#endif

#if defined(_WIN32)
    using time_t = long long;
#else
    using time_t = long;
#endif

#if defined(__APPLE__)

    // Darwin owns the C ABI definition of ::timespec. Reuse that type rather than
    // defining an independent std::timespec, because native system headers and
    // frameworks may declare or reference ::timespec in the same translation unit.
    using ::timespec;

#else

    struct timespec
    {
        time_t tv_sec;
        long tv_nsec;
    };

#endif

    struct tm
    {
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
        const char* tm_zone;
#endif
    };
} // namespace std

#endif // FTL_C_TIME_TYPES_HEADER
