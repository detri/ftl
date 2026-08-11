// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_NATIVE_IO_HEADER
#define FTL_DETAIL_NATIVE_IO_HEADER

namespace ftl::detail {

using native_io_size = decltype(sizeof(0));
using native_io_offset = long long;

struct native_io_error {
  int value{};
};

struct native_file_handle {
#if defined(_WIN32)
  void *value = reinterpret_cast<void *>(-1);
#else
  int value = -1;
#endif

  [[nodiscard]] bool valid() const noexcept {
#if defined(_WIN32)
    return value != reinterpret_cast<void *>(-1) && value != nullptr;
#else
    return value >= 0;
#endif
  }
};

enum class native_file_access : unsigned {
  read = 1,
  write = 2,
  read_write = 3,
};

enum class native_file_creation : unsigned {
  open_existing,
  create_always,
  open_or_create,
  create_new,
};

enum class native_seek_origin : unsigned { begin, current, end };

struct native_open_options {
  native_file_access access = native_file_access::read;
  native_file_creation creation = native_file_creation::open_existing;
  bool append = false;
  bool temporary = false;
};

[[nodiscard]] bool native_open_file(const char *path,
                                    const native_open_options &options,
                                    native_file_handle &result,
                                    native_io_error &error) noexcept;

#if defined(_WIN32)
[[nodiscard]] bool native_open_file(const wchar_t *path,
                                    const native_open_options &options,
                                    native_file_handle &result,
                                    native_io_error &error) noexcept;
[[nodiscard]] bool native_remove_file(const wchar_t *path,
                                      native_io_error &error) noexcept;
[[nodiscard]] bool native_rename_file(const wchar_t *old_path,
                                      const wchar_t *new_path,
                                      native_io_error &error) noexcept;
#endif

[[nodiscard]] bool native_read_file(native_file_handle handle, void *buffer,
                                    native_io_size capacity,
                                    native_io_size &transferred,
                                    native_io_error &error) noexcept;

[[nodiscard]] bool native_write_file(native_file_handle handle,
                                     const void *buffer, native_io_size count,
                                     native_io_size &transferred,
                                     native_io_error &error) noexcept;

[[nodiscard]] bool native_seek_file(native_file_handle handle,
                                    native_io_offset offset,
                                    native_seek_origin origin,
                                    native_io_offset &position,
                                    native_io_error &error) noexcept;

[[nodiscard]] bool native_flush_file(native_file_handle handle,
                                     native_io_error &error) noexcept;

[[nodiscard]] bool native_close_file(native_file_handle handle,
                                     native_io_error &error) noexcept;

[[nodiscard]] native_file_handle native_standard_input() noexcept;
[[nodiscard]] native_file_handle native_standard_output() noexcept;
[[nodiscard]] native_file_handle native_standard_error() noexcept;

[[nodiscard]] bool native_remove_file(const char *path,
                                      native_io_error &error) noexcept;
[[nodiscard]] bool native_rename_file(const char *old_path,
                                      const char *new_path,
                                      native_io_error &error) noexcept;
[[nodiscard]] bool native_file_is_terminal(native_file_handle handle) noexcept;

} // namespace ftl::detail

#endif // FTL_DETAIL_NATIVE_IO_HEADER
