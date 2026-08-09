// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_THREAD_HEADER
#define FTL_DETAIL_THREAD_HEADER

#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <cstdint>
#else
#include <ftl/cstddef>
#include <ftl/cstdint>
#endif

#if defined(__linux__) || defined(__APPLE__)
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_THREAD_DETAIL_BEGIN_NAMESPACE namespace std::detail {
#define FTL_THREAD_DETAIL_END_NAMESPACE }
#else
#define FTL_THREAD_DETAIL_BEGIN_NAMESPACE namespace ftl::detail {
#define FTL_THREAD_DETAIL_END_NAMESPACE }
#endif

FTL_THREAD_DETAIL_BEGIN_NAMESPACE

struct thread_start_state {
  using run_type = void (*)(thread_start_state *) noexcept;

  explicit thread_start_state(run_type run) noexcept : run_(run) {}

  run_type run_;
};

#if defined(_WIN32)

using native_thread_handle = void *;
using native_thread_id = uintptr_t;

using windows_thread_entry = unsigned long(__stdcall *)(void *);

extern "C" __declspec(dllimport) void *__stdcall
CreateThread(void *, size_t, windows_thread_entry, void *, unsigned long,
             unsigned long *);

extern "C" __declspec(dllimport) unsigned long __stdcall
WaitForSingleObject(void *, unsigned long);

extern "C" __declspec(dllimport) int __stdcall CloseHandle(void *);

extern "C" __declspec(dllimport) unsigned long __stdcall GetCurrentThreadId();

extern "C" __declspec(dllimport) unsigned long __stdcall GetLastError();

extern "C" __declspec(dllimport) int __stdcall SwitchToThread();

extern "C" __declspec(dllimport) void __stdcall Sleep(unsigned long);

extern "C" __declspec(dllimport) unsigned long __stdcall
GetActiveProcessorCount(unsigned short);

inline unsigned long __stdcall windows_thread_start(void *argument) noexcept {
  auto *state = static_cast<thread_start_state *>(argument);

  state->run_(state);
  return 0;
}

inline bool native_thread_create(native_thread_handle &handle,
                                 native_thread_id &id,
                                 thread_start_state *state) noexcept {
  unsigned long thread_id = 0;

  void *created =
      CreateThread(nullptr, 0, &windows_thread_start, state, 0, &thread_id);

  if (created == nullptr)
    return false;

  handle = created;
  id = static_cast<native_thread_id>(thread_id);

  return true;
}

inline bool native_thread_join(native_thread_handle handle) noexcept {
  constexpr unsigned long infinite = 0xffffffffUL;

  constexpr unsigned long object_signaled = 0;

  const unsigned long result = WaitForSingleObject(handle, infinite);

  if (result != object_signaled)
    return false;

  (void)CloseHandle(handle);
  return true;
}

inline bool native_thread_detach(native_thread_handle handle) noexcept {
  return CloseHandle(handle) != 0;
}

inline native_thread_id native_this_thread_id() noexcept {
  return static_cast<native_thread_id>(GetCurrentThreadId());
}

inline void native_thread_yield() noexcept { (void)SwitchToThread(); }

inline void native_thread_sleep(uint64_t nanoseconds) noexcept {
  constexpr uint64_t nanoseconds_per_millisecond = 1000000ULL;

  constexpr uint64_t maximum_sleep = 0xfffffffeULL;

  uint64_t milliseconds = nanoseconds / nanoseconds_per_millisecond;

  if (nanoseconds % nanoseconds_per_millisecond != 0)
    ++milliseconds;

  while (milliseconds > maximum_sleep) {
    Sleep(static_cast<unsigned long>(maximum_sleep));

    milliseconds -= maximum_sleep;
  }

  if (milliseconds != 0) {
    Sleep(static_cast<unsigned long>(milliseconds));
  }
}

inline unsigned int native_thread_hardware_concurrency() noexcept {
  constexpr unsigned short all_processor_groups = 0xffff;

  return static_cast<unsigned int>(
      GetActiveProcessorCount(all_processor_groups));
}

#elif defined(__linux__) || defined(__APPLE__)

using native_thread_handle = pthread_t;
using native_thread_id = uintptr_t;

inline native_thread_id
native_thread_id_from_handle(native_thread_handle handle) noexcept {
#if defined(__APPLE__)
  return reinterpret_cast<native_thread_id>(handle);
#else
  return static_cast<native_thread_id>(handle);
#endif
}

inline void *posix_thread_start(void *argument) noexcept {
  auto *state = static_cast<thread_start_state *>(argument);

  state->run_(state);
  return nullptr;
}

inline bool native_thread_create(native_thread_handle &handle,
                                 native_thread_id &id,
                                 thread_start_state *state) noexcept {
  native_thread_handle created{};

  const int result =
      pthread_create(&created, nullptr, &posix_thread_start, state);

  if (result != 0)
    return false;

  handle = created;
  id = native_thread_id_from_handle(created);

  return true;
}

inline bool native_thread_join(native_thread_handle handle) noexcept {
  return pthread_join(handle, nullptr) == 0;
}

inline bool native_thread_detach(native_thread_handle handle) noexcept {
  return pthread_detach(handle) == 0;
}

inline native_thread_id native_this_thread_id() noexcept {
  return native_thread_id_from_handle(pthread_self());
}

inline void native_thread_yield() noexcept { (void)sched_yield(); }

inline void native_thread_sleep(uint64_t nanoseconds) noexcept {
  constexpr uint64_t nanoseconds_per_second = 1000000000ULL;

  timespec requested{static_cast<time_t>(nanoseconds / nanoseconds_per_second),
                     static_cast<long>(nanoseconds % nanoseconds_per_second)};

  timespec remaining{};

  while (nanosleep(&requested, &remaining) != 0) {
    if (errno != EINTR)
      return;

    requested = remaining;
  }
}

inline unsigned int native_thread_hardware_concurrency() noexcept {
  const long count = sysconf(_SC_NPROCESSORS_ONLN);

  return count > 0 ? static_cast<unsigned int>(count) : 0u;
}

#else

#error FTL thread backend is not implemented for this platform

#endif

FTL_THREAD_DETAIL_END_NAMESPACE

#undef FTL_THREAD_DETAIL_BEGIN_NAMESPACE
#undef FTL_THREAD_DETAIL_END_NAMESPACE

#endif // FTL_DETAIL_THREAD_HEADER
