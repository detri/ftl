// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_NATIVE_FILESYSTEM_HEADER
#define FTL_DETAIL_NATIVE_FILESYSTEM_HEADER

#ifdef FTL_REPLACE_STL
#include <detail/native_io.hpp>
#else
#include <ftl/detail/native_io.hpp>
#endif

namespace ftl::detail {

enum class native_file_kind : signed char {
  none = 0,
  not_found = -1,
  regular = 1,
  directory = 2,
  symlink = 3,
  block = 4,
  character = 5,
  fifo = 6,
  socket = 7,
  unknown = 8
};

struct native_file_info {
  native_file_kind kind = native_file_kind::none;
  unsigned permissions{};
  unsigned long long size{};
  unsigned long long links{};
  long long write_time{};
  unsigned long long device{};
  unsigned long long inode{};
};

struct native_space_info {
  unsigned long long capacity{}, free{}, available{};
};

using native_directory_callback = bool (*)(const char *, native_file_kind,
                                           void *) noexcept;

[[nodiscard]] bool native_status(const char *, bool follow, native_file_info &,
                                 native_io_error &) noexcept;
[[nodiscard]] bool native_current_path(char *, native_io_size, native_io_size &,
                                       native_io_error &) noexcept;
[[nodiscard]] bool native_set_current_path(const char *,
                                           native_io_error &) noexcept;
[[nodiscard]] bool native_create_directory(const char *,
                                           native_io_error &) noexcept;
[[nodiscard]] bool native_create_directory_from(const char *, const char *,
                                                native_io_error &) noexcept;
[[nodiscard]] bool native_remove(const char *, bool &,
                                 native_io_error &) noexcept;
[[nodiscard]] bool native_resize_file(const char *, unsigned long long,
                                      native_io_error &) noexcept;
[[nodiscard]] bool native_set_write_time(const char *, long long,
                                         native_io_error &) noexcept;
[[nodiscard]] bool native_set_permissions(const char *, unsigned, bool, bool,
                                          native_io_error &) noexcept;
[[nodiscard]] bool native_create_hard_link(const char *, const char *,
                                           native_io_error &) noexcept;
[[nodiscard]] bool native_create_symlink(const char *, const char *, bool,
                                         native_io_error &) noexcept;
[[nodiscard]] bool native_read_symlink(const char *, char *, native_io_size,
                                       native_io_size &,
                                       native_io_error &) noexcept;
[[nodiscard]] bool native_list_directory(const char *,
                                         native_directory_callback, void *,
                                         native_io_error &) noexcept;
[[nodiscard]] bool native_space(const char *, native_space_info &,
                                native_io_error &) noexcept;
[[nodiscard]] bool native_copy_file(const char *, const char *, bool,
                                    native_io_error &) noexcept;
[[nodiscard]] bool native_absolute_path(const char *, char *, native_io_size,
                                        native_io_size &,
                                        native_io_error &) noexcept;
[[nodiscard]] bool native_canonical_path(const char *, char *, native_io_size,
                                         native_io_size &,
                                         native_io_error &) noexcept;
[[nodiscard]] bool native_temp_directory(char *, native_io_size,
                                         native_io_size &,
                                         native_io_error &) noexcept;

} // namespace ftl::detail
#endif
