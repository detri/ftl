// Freestanding Template Library
// SPDX-License-Identifier: MIT
#include <ftl/detail/native_io.hpp>

#if defined(_WIN32)

extern "C" {
__declspec(dllimport) void *__stdcall CreateFileA(const char *, unsigned long,
                                                  unsigned long, void *,
                                                  unsigned long, unsigned long,
                                                  void *);
__declspec(dllimport) void *__stdcall CreateFileW(const wchar_t *, unsigned long,
                                                  unsigned long, void *,
                                                  unsigned long, unsigned long,
                                                  void *);
__declspec(dllimport) int __stdcall ReadFile(void *, void *, unsigned long,
                                             unsigned long *, void *);
__declspec(dllimport) int __stdcall WriteFile(void *, const void *,
                                              unsigned long, unsigned long *,
                                              void *);
__declspec(dllimport) int __stdcall SetFilePointerEx(void *, long long,
                                                     long long *, unsigned long);
__declspec(dllimport) int __stdcall FlushFileBuffers(void *);
__declspec(dllimport) int __stdcall CloseHandle(void *);
__declspec(dllimport) void *__stdcall GetStdHandle(unsigned long);
__declspec(dllimport) unsigned long __stdcall GetLastError();
__declspec(dllimport) int __stdcall DeleteFileA(const char *);
__declspec(dllimport) int __stdcall MoveFileExA(const char *, const char *,
                                                unsigned long);
__declspec(dllimport) int __stdcall GetConsoleMode(void *, unsigned long *);
}

namespace {
constexpr unsigned long generic_read = 0x80000000UL;
constexpr unsigned long generic_write = 0x40000000UL;
constexpr unsigned long file_append_data = 0x00000004UL;
constexpr unsigned long share_all = 0x00000007UL;
constexpr unsigned long create_new = 1;
constexpr unsigned long create_always = 2;
constexpr unsigned long open_existing = 3;
constexpr unsigned long open_always = 4;
constexpr unsigned long file_attribute_normal = 0x00000080UL;
constexpr unsigned long file_attribute_temporary = 0x00000100UL;
constexpr unsigned long file_flag_delete_on_close = 0x04000000UL;
constexpr unsigned long move_replace_existing = 0x1UL;
constexpr unsigned long std_input_handle = static_cast<unsigned long>(-10);
constexpr unsigned long std_output_handle = static_cast<unsigned long>(-11);
constexpr unsigned long std_error_handle = static_cast<unsigned long>(-12);

unsigned long access_for(ftl::detail::native_file_access access, bool append) {
  switch (access) {
  case ftl::detail::native_file_access::read:
    return generic_read;
  case ftl::detail::native_file_access::write:
    return append ? file_append_data : generic_write;
  default:
    return generic_read | (append ? file_append_data : generic_write);
  }
}

unsigned long creation_for(ftl::detail::native_file_creation creation) {
  switch (creation) {
  case ftl::detail::native_file_creation::create_always:
    return create_always;
  case ftl::detail::native_file_creation::open_or_create:
    return open_always;
  case ftl::detail::native_file_creation::create_new:
    return create_new;
  default:
    return open_existing;
  }
}
} // namespace

namespace ftl::detail {

template <class Character>
bool open_windows(const Character *path, const native_open_options &options,
                  native_file_handle &result, native_io_error &error) noexcept {
  unsigned long attributes = file_attribute_normal;
  if (options.temporary)
    attributes |= file_attribute_temporary | file_flag_delete_on_close;
  void *handle;
  if constexpr (__is_same(Character, char))
    handle = CreateFileA(path, access_for(options.access, options.append), share_all, nullptr,
                         creation_for(options.creation), attributes, nullptr);
  else
    handle = CreateFileW(path, access_for(options.access, options.append), share_all, nullptr,
                         creation_for(options.creation), attributes, nullptr);
  result.value = handle;
  if (!result.valid()) {
    error.value = static_cast<int>(GetLastError());
    return false;
  }
  return true;
}

bool native_open_file(const char *path, const native_open_options &options,
                      native_file_handle &result,
                      native_io_error &error) noexcept {
  return open_windows(path, options, result, error);
}

bool native_open_file(const wchar_t *path, const native_open_options &options,
                      native_file_handle &result,
                      native_io_error &error) noexcept {
  return open_windows(path, options, result, error);
}

bool native_read_file(native_file_handle handle, void *buffer,
                      native_io_size capacity, native_io_size &transferred,
                      native_io_error &error) noexcept {
  transferred = 0;
  while (capacity != 0) {
    unsigned long chunk = capacity > 0x7fffffffUL
                              ? 0x7fffffffUL
                              : static_cast<unsigned long>(capacity);
    unsigned long done = 0;
    if (!ReadFile(handle.value, buffer, chunk, &done, nullptr)) {
      error.value = static_cast<int>(GetLastError());
      return false;
    }
    transferred += done;
    if (done != chunk)
      break;
    buffer = static_cast<unsigned char *>(buffer) + done;
    capacity -= done;
  }
  return true;
}

bool native_write_file(native_file_handle handle, const void *buffer,
                       native_io_size count, native_io_size &transferred,
                       native_io_error &error) noexcept {
  transferred = 0;
  while (count != 0) {
    unsigned long chunk = count > 0x7fffffffUL
                              ? 0x7fffffffUL
                              : static_cast<unsigned long>(count);
    unsigned long done = 0;
    if (!WriteFile(handle.value, buffer, chunk, &done, nullptr)) {
      error.value = static_cast<int>(GetLastError());
      return false;
    }
    transferred += done;
    if (done == 0)
      break;
    buffer = static_cast<const unsigned char *>(buffer) + done;
    count -= done;
  }
  return transferred != 0 || count == 0;
}

bool native_seek_file(native_file_handle handle, native_io_offset offset,
                      native_seek_origin origin, native_io_offset &position,
                      native_io_error &error) noexcept {
  unsigned long method = origin == native_seek_origin::begin     ? 0
                         : origin == native_seek_origin::current ? 1
                                                                 : 2;
  if (!SetFilePointerEx(handle.value, offset, &position, method)) {
    error.value = static_cast<int>(GetLastError());
    return false;
  }
  return true;
}

bool native_flush_file(native_file_handle handle,
                       native_io_error &error) noexcept {
  if (!FlushFileBuffers(handle.value)) {
    error.value = static_cast<int>(GetLastError());
    return false;
  }
  return true;
}

bool native_close_file(native_file_handle handle,
                       native_io_error &error) noexcept {
  if (!CloseHandle(handle.value)) {
    error.value = static_cast<int>(GetLastError());
    return false;
  }
  return true;
}

native_file_handle native_standard_input() noexcept {
  return {GetStdHandle(std_input_handle)};
}
native_file_handle native_standard_output() noexcept {
  return {GetStdHandle(std_output_handle)};
}
native_file_handle native_standard_error() noexcept {
  return {GetStdHandle(std_error_handle)};
}

bool native_remove_file(const char *path, native_io_error &error) noexcept {
  if (!DeleteFileA(path)) {
    error.value = static_cast<int>(GetLastError());
    return false;
  }
  return true;
}

bool native_rename_file(const char *old_path, const char *new_path,
                        native_io_error &error) noexcept {
  if (!MoveFileExA(old_path, new_path, move_replace_existing)) {
    error.value = static_cast<int>(GetLastError());
    return false;
  }
  return true;
}

bool native_file_is_terminal(native_file_handle handle) noexcept {
  unsigned long mode;
  return GetConsoleMode(handle.value, &mode) != 0;
}

} // namespace ftl::detail

#else

extern "C" {
int open(const char *, int, ...);
long long lseek(int, long long, int);
long long read(int, void *, decltype(sizeof(0)));
long long write(int, const void *, decltype(sizeof(0)));
int fsync(int);
int close(int);
int unlink(const char *);
int rename(const char *, const char *);
int isatty(int);
#if defined(__APPLE__)
int *__error();
#else
int *__errno_location();
#endif
}

namespace {
int current_errno() {
#if defined(__APPLE__)
  return *__error();
#else
  return *__errno_location();
#endif
}
constexpr int open_read_only = 0;
constexpr int open_write_only = 1;
constexpr int open_read_write = 2;
constexpr int open_create = 0100;
constexpr int open_exclusive = 0200;
constexpr int open_truncate = 01000;
constexpr int open_append = 02000;
constexpr int owner_read_write = 0600;
} // namespace

namespace ftl::detail {

bool native_open_file(const char *path, const native_open_options &options,
                      native_file_handle &result,
                      native_io_error &error) noexcept {
  int flags = options.access == native_file_access::read         ? open_read_only
              : options.access == native_file_access::write     ? open_write_only
                                                                 : open_read_write;
  switch (options.creation) {
  case native_file_creation::create_always:
    flags |= open_create | open_truncate;
    break;
  case native_file_creation::open_or_create:
    flags |= open_create;
    break;
  case native_file_creation::create_new:
    flags |= open_create | open_exclusive;
    break;
  default:
    break;
  }
  if (options.append)
    flags |= open_append;
  result.value = open(path, flags, owner_read_write);
  if (!result.valid()) {
    error.value = current_errno();
    return false;
  }
  return true;
}

bool native_read_file(native_file_handle handle, void *buffer,
                      native_io_size capacity, native_io_size &transferred,
                      native_io_error &error) noexcept {
  auto result = read(handle.value, buffer, capacity);
  if (result < 0) {
    transferred = 0;
    error.value = current_errno();
    return false;
  }
  transferred = static_cast<native_io_size>(result);
  return true;
}

bool native_write_file(native_file_handle handle, const void *buffer,
                       native_io_size count, native_io_size &transferred,
                       native_io_error &error) noexcept {
  transferred = 0;
  while (transferred != count) {
    auto result = write(handle.value,
                        static_cast<const unsigned char *>(buffer) + transferred,
                        count - transferred);
    if (result <= 0) {
      error.value = current_errno();
      return false;
    }
    transferred += static_cast<native_io_size>(result);
  }
  return true;
}

bool native_seek_file(native_file_handle handle, native_io_offset offset,
                      native_seek_origin origin, native_io_offset &position,
                      native_io_error &error) noexcept {
  int whence = origin == native_seek_origin::begin     ? 0
               : origin == native_seek_origin::current ? 1
                                                       : 2;
  auto result = lseek(handle.value, offset, whence);
  if (result < 0) {
    error.value = current_errno();
    return false;
  }
  position = result;
  return true;
}

bool native_flush_file(native_file_handle handle,
                       native_io_error &error) noexcept {
  if (fsync(handle.value) != 0) {
    error.value = current_errno();
    return false;
  }
  return true;
}

bool native_close_file(native_file_handle handle,
                       native_io_error &error) noexcept {
  if (close(handle.value) != 0) {
    error.value = current_errno();
    return false;
  }
  return true;
}

native_file_handle native_standard_input() noexcept { return {0}; }
native_file_handle native_standard_output() noexcept { return {1}; }
native_file_handle native_standard_error() noexcept { return {2}; }

bool native_remove_file(const char *path, native_io_error &error) noexcept {
  if (unlink(path) != 0) {
    error.value = current_errno();
    return false;
  }
  return true;
}

bool native_rename_file(const char *old_path, const char *new_path,
                        native_io_error &error) noexcept {
  if (rename(old_path, new_path) != 0) {
    error.value = current_errno();
    return false;
  }
  return true;
}

bool native_file_is_terminal(native_file_handle handle) noexcept {
  return isatty(handle.value) != 0;
}

} // namespace ftl::detail

#endif
