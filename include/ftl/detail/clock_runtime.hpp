// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_CLOCK_RUNTIME_HEADER
#define FTL_DETAIL_CLOCK_RUNTIME_HEADER

#ifdef FTL_REPLACE_STL
#include <cstdint>
#else
#include <ftl/cstdint>
#endif

namespace ftl_clock_runtime {
#if defined(_WIN32)
struct file_time {
  unsigned long low, high;
};
extern "C" __declspec(dllimport) void __stdcall
GetSystemTimePreciseAsFileTime(file_time *);
extern "C" __declspec(dllimport) int __stdcall
QueryPerformanceCounter(long long *);
extern "C" __declspec(dllimport) int __stdcall
QueryPerformanceFrequency(long long *);
inline long long system_nanoseconds() noexcept {
  file_time value{};
  GetSystemTimePreciseAsFileTime(&value);
  const auto ticks =
      (static_cast<unsigned long long>(value.high) << 32) | value.low;
  return static_cast<long long>((ticks - 116444736000000000ULL) * 100);
}
inline long long steady_nanoseconds() noexcept {
  long long value{}, frequency{};
  QueryPerformanceCounter(&value);
  QueryPerformanceFrequency(&frequency);
  return value / frequency * 1000000000LL +
         value % frequency * 1000000000LL / frequency;
}
#elif defined(__APPLE__)
struct timebase_info {
  unsigned numer, denom;
};
extern "C" unsigned long long mach_absolute_time();
extern "C" int mach_timebase_info(timebase_info *);
extern "C" int clock_gettime(int, void *);
struct timespec {
  long tv_sec, tv_nsec;
};
inline long long system_nanoseconds() noexcept {
  timespec value{};
  clock_gettime(0, &value);
  return value.tv_sec * 1000000000LL + value.tv_nsec;
}
inline long long steady_nanoseconds() noexcept {
  timebase_info info{};
  mach_timebase_info(&info);
  const auto ticks = mach_absolute_time();
  return static_cast<long long>(ticks / info.denom * info.numer +
                                ticks % info.denom * info.numer / info.denom);
}
#else
struct timespec {
  long tv_sec, tv_nsec;
};
extern "C" int clock_gettime(int, timespec *);
inline long long clock_nanoseconds(int id) noexcept {
  timespec value{};
  clock_gettime(id, &value);
  return value.tv_sec * 1000000000LL + value.tv_nsec;
}
inline long long system_nanoseconds() noexcept { return clock_nanoseconds(0); }
inline long long steady_nanoseconds() noexcept { return clock_nanoseconds(1); }
#endif
} // namespace ftl_clock_runtime
#endif
