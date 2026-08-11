// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_STDIO_RUNTIME_HEADER
#define FTL_DETAIL_STDIO_RUNTIME_HEADER

#ifdef FTL_REPLACE_STL
#include <__c_file_types.hpp>
#else
#include <ftl/__c_file_types.hpp>
#endif

namespace ftl_stdio_runtime {
using size_type = decltype(sizeof(0));
extern "C" {
size_type fwrite(const void *, size_type, size_type, FILE *);
int fflush(FILE *);
#if defined(_WIN32)
FILE *__cdecl __acrt_iob_func(unsigned);
#elif defined(__APPLE__)
extern FILE *__stdinp;
extern FILE *__stdoutp;
extern FILE *__stderrp;
#else
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;
#endif
}
[[nodiscard]] inline FILE *input_stream() noexcept {
#if defined(_WIN32)
  return __acrt_iob_func(0);
#elif defined(__APPLE__)
  return __stdinp;
#else
  return stdin;
#endif
}
[[nodiscard]] inline FILE *output_stream() noexcept {
#if defined(_WIN32)
  return __acrt_iob_func(1);
#elif defined(__APPLE__)
  return __stdoutp;
#else
  return stdout;
#endif
}
[[nodiscard]] inline FILE *error_stream() noexcept {
#if defined(_WIN32)
  return __acrt_iob_func(2);
#elif defined(__APPLE__)
  return __stderrp;
#else
  return stderr;
#endif
}
} // namespace ftl_stdio_runtime
#endif
