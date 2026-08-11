// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_STDIO_RUNTIME_HEADER
#define FTL_DETAIL_STDIO_RUNTIME_HEADER

#ifdef FTL_REPLACE_STL
#include <__c_file_types.hpp>
#include <detail/native_io.hpp>
#else
#include <ftl/__c_file_types.hpp>
#include <ftl/detail/native_io.hpp>
#endif

namespace ftl_stdio_runtime {
using size_type = decltype(sizeof(0));
using file_type = ::ftl_file;

size_type fwrite(const void *, size_type, size_type, file_type *);
int fflush(file_type *);

[[nodiscard]] file_type *input_stream() noexcept;
[[nodiscard]] file_type *output_stream() noexcept;
[[nodiscard]] file_type *error_stream() noexcept;
[[nodiscard]] ftl::detail::native_file_handle
native_handle(file_type *) noexcept;
[[nodiscard]] bool is_terminal(file_type *) noexcept;
int orient(file_type *, int) noexcept;
void lock_file(file_type *) noexcept;
void unlock_file(file_type *) noexcept;
size_type write_bytes_locked(const void *, size_type, file_type *) noexcept;
size_type read_bytes_locked(void *, size_type, file_type *) noexcept;
int unget_byte_locked(int, file_type *) noexcept;
int read_wide_byte_locked(file_type *) noexcept;
int write_wide_byte_locked(int, file_type *) noexcept;
int unget_wide_byte_locked(int, file_type *) noexcept;
int read_wide_byte(file_type *) noexcept;
int write_wide_byte(int, file_type *) noexcept;
int unget_wide_byte(int, file_type *) noexcept;
} // namespace ftl_stdio_runtime

#endif // FTL_DETAIL_STDIO_RUNTIME_HEADER
