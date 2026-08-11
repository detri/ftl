// Freestanding Template Library
// SPDX-License-Identifier: MIT
#include <ftl/detail/native_filesystem.hpp>

#include <cerrno>
#include <cstdlib>
#include <cstring>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace {
using namespace ftl::detail;

bool wide_path(const char *source, wchar_t *result, int capacity,
               native_io_error &error) noexcept {
  for (const char *current = source; *current; ++current) {
    if (*current == '\\') {
      error.value = ERROR_INVALID_NAME;
      return false;
    }
  }
  int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, source, -1, result,
                              capacity);
  if (!n) {
    error.value = static_cast<int>(GetLastError());
    return false;
  }
  for (int i = 0; i != n; ++i)
    if (result[i] == L'/')
      result[i] = L'\\';
  return true;
}

bool utf8(const wchar_t *source, char *result, int capacity, int &size,
          native_io_error &error) noexcept {
  size = WideCharToMultiByte(CP_UTF8, 0, source, -1, result, capacity, nullptr,
                             nullptr);
  if (!size) {
    error.value = static_cast<int>(GetLastError());
    return false;
  }
  --size;
  for (int i = 0; i != size; ++i)
    if (result[i] == '\\')
      result[i] = '/';
  return true;
}

native_file_kind kind(DWORD attributes) noexcept {
  if (attributes & FILE_ATTRIBUTE_REPARSE_POINT)
    return native_file_kind::symlink;
  if (attributes & FILE_ATTRIBUTE_DIRECTORY)
    return native_file_kind::directory;
  return native_file_kind::regular;
}

long long ticks(FILETIME t) noexcept {
  constexpr long long windows_epoch = 116444736000000000LL;
  auto value = static_cast<long long>(
      (static_cast<unsigned long long>(t.dwHighDateTime) << 32) |
      t.dwLowDateTime);
  return (value - windows_epoch) * 100;
}
} // namespace

namespace ftl::detail {
bool native_status(const char *p, bool follow, native_file_info &out,
                   native_io_error &error) noexcept {
  wchar_t path[32768];
  if (!wide_path(p, path, 32768, error))
    return false;
  WIN32_FILE_ATTRIBUTE_DATA data{};
  if (!GetFileAttributesExW(path, GetFileExInfoStandard, &data)) {
    auto e = GetLastError();
    if (e == ERROR_FILE_NOT_FOUND || e == ERROR_PATH_NOT_FOUND ||
        e == ERROR_INVALID_NAME) {
      out.kind = native_file_kind::not_found;
      return true;
    }
    error.value = static_cast<int>(e);
    return false;
  }
  out.kind = kind(data.dwFileAttributes);
  out.permissions =
      (data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? 0444u : 0666u;
  if (out.kind == native_file_kind::directory)
    out.permissions |= 0111u;
  out.size = (static_cast<unsigned long long>(data.nFileSizeHigh) << 32) |
             data.nFileSizeLow;
  out.write_time = ticks(data.ftLastWriteTime);
  if (follow || out.kind != native_file_kind::symlink) {
    HANDLE h = CreateFileW(
        path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (h != INVALID_HANDLE_VALUE) {
      BY_HANDLE_FILE_INFORMATION i{};
      if (GetFileInformationByHandle(h, &i)) {
        out.kind = kind(i.dwFileAttributes);
        out.links = i.nNumberOfLinks;
        out.device = i.dwVolumeSerialNumber;
        out.inode = (static_cast<unsigned long long>(i.nFileIndexHigh) << 32) |
                    i.nFileIndexLow;
      }
      CloseHandle(h);
    }
  }
  return true;
}

bool native_current_path(char *out, native_io_size cap, native_io_size &n,
                         native_io_error &error) noexcept {
  wchar_t path[32768];
  DWORD got = GetCurrentDirectoryW(32768, path);
  if (!got || got >= 32768) {
    error.value = GetLastError();
    return false;
  }
  int size{};
  if (!utf8(path, out, static_cast<int>(cap), size, error))
    return false;
  n = static_cast<native_io_size>(size);
  return true;
}
bool native_set_current_path(const char *p, native_io_error &e) noexcept {
  wchar_t path[32768];
  if (!wide_path(p, path, 32768, e))
    return false;
  if (!SetCurrentDirectoryW(path)) {
    e.value = GetLastError();
    return false;
  }
  return true;
}
bool native_create_directory(const char *p, native_io_error &e) noexcept {
  wchar_t path[32768];
  if (!wide_path(p, path, 32768, e))
    return false;
  if (!CreateDirectoryW(path, nullptr)) {
    e.value = GetLastError();
    return false;
  }
  return true;
}
bool native_create_directory_from(const char *p, const char *attributes,
                                  native_io_error &e) noexcept {
  wchar_t path[32768], source[32768];
  if (!wide_path(p, path, 32768, e) || !wide_path(attributes, source, 32768, e))
    return false;
  if (!CreateDirectoryExW(source, path, nullptr)) {
    e.value = GetLastError();
    return false;
  }
  return true;
}
bool native_remove(const char *p, bool &removed, native_io_error &e) noexcept {
  wchar_t path[32768];
  if (!wide_path(p, path, 32768, e))
    return false;
  DWORD a = GetFileAttributesW(path);
  if (a == INVALID_FILE_ATTRIBUTES) {
    auto x = GetLastError();
    if (x == ERROR_FILE_NOT_FOUND || x == ERROR_PATH_NOT_FOUND) {
      removed = false;
      return true;
    }
    e.value = x;
    return false;
  }
  BOOL ok = (a & FILE_ATTRIBUTE_DIRECTORY) ? RemoveDirectoryW(path)
                                           : DeleteFileW(path);
  if (!ok) {
    e.value = GetLastError();
    return false;
  }
  removed = true;
  return true;
}
bool native_resize_file(const char *p, unsigned long long size,
                        native_io_error &e) noexcept {
  wchar_t path[32768];
  if (!wide_path(p, path, 32768, e))
    return false;
  HANDLE h = CreateFileW(path, GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                         nullptr, OPEN_EXISTING, 0, nullptr);
  if (h == INVALID_HANDLE_VALUE) {
    e.value = GetLastError();
    return false;
  }
  LARGE_INTEGER x;
  x.QuadPart = size;
  bool ok = SetFilePointerEx(h, x, nullptr, FILE_BEGIN) && SetEndOfFile(h);
  if (!ok)
    e.value = GetLastError();
  CloseHandle(h);
  return ok;
}
bool native_set_write_time(const char *p, long long value,
                           native_io_error &e) noexcept {
  wchar_t path[32768];
  if (!wide_path(p, path, 32768, e))
    return false;
  HANDLE h =
      CreateFileW(path, FILE_WRITE_ATTRIBUTES,
                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                  nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
  if (h == INVALID_HANDLE_VALUE) {
    e.value = GetLastError();
    return false;
  }
  constexpr long long windows_epoch = 116444736000000000LL;
  ULARGE_INTEGER u;
  u.QuadPart = static_cast<unsigned long long>(value / 100 + windows_epoch);
  FILETIME t{u.LowPart, u.HighPart};
  bool ok = SetFileTime(h, nullptr, nullptr, &t) != 0;
  if (!ok)
    e.value = GetLastError();
  CloseHandle(h);
  return ok;
}
bool native_set_permissions(const char *p, unsigned bits, bool add, bool,
                            native_io_error &e) noexcept {
  wchar_t path[32768];
  if (!wide_path(p, path, 32768, e))
    return false;
  DWORD a = GetFileAttributesW(path);
  if (a == INVALID_FILE_ATTRIBUTES) {
    e.value = GetLastError();
    return false;
  }
  bool writable = (bits & 0222) != 0;
  if (add && writable)
    a &= ~FILE_ATTRIBUTE_READONLY;
  else if (!add && !writable)
    a |= FILE_ATTRIBUTE_READONLY;
  if (!SetFileAttributesW(path, a)) {
    e.value = GetLastError();
    return false;
  }
  return true;
}
bool native_create_hard_link(const char *to, const char *link,
                             native_io_error &e) noexcept {
  wchar_t a[32768], b[32768];
  if (!wide_path(to, a, 32768, e) || !wide_path(link, b, 32768, e))
    return false;
  if (!CreateHardLinkW(b, a, nullptr)) {
    e.value = GetLastError();
    return false;
  }
  return true;
}
bool native_create_symlink(const char *to, const char *link, bool directory,
                           native_io_error &e) noexcept {
  wchar_t a[32768], b[32768];
  if (!wide_path(to, a, 32768, e) || !wide_path(link, b, 32768, e))
    return false;
  DWORD f = directory ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0;
  f |= SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;
  if (!CreateSymbolicLinkW(b, a, f)) {
    e.value = GetLastError();
    return false;
  }
  return true;
}
bool native_read_symlink(const char *p, char *out, native_io_size cap,
                         native_io_size &n, native_io_error &e) noexcept {
  wchar_t path[32768], wide[32768];
  if (!wide_path(p, path, 32768, e))
    return false;
  HANDLE h = CreateFileW(
      path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
      OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS,
      nullptr);
  if (h == INVALID_HANDLE_VALUE) {
    e.value = GetLastError();
    return false;
  }
  struct reparse_data {
    DWORD tag;
    WORD data_length;
    WORD reserved;
    WORD substitute_offset;
    WORD substitute_length;
    WORD print_offset;
    WORD print_length;
    DWORD flags;
    wchar_t buffer[8192];
  } data{};
  DWORD got{};
  BOOL ok = DeviceIoControl(h, 0x000900A8UL, nullptr, 0, &data, sizeof data,
                            &got, nullptr);
  CloseHandle(h);
  if (!ok || data.tag != IO_REPARSE_TAG_SYMLINK) {
    e.value = GetLastError();
    return false;
  }
  WORD offset = data.print_length ? data.print_offset : data.substitute_offset;
  WORD bytes = data.print_length ? data.print_length : data.substitute_length;
  auto count = static_cast<size_t>(bytes / sizeof(wchar_t));
  if (count >= 32768) {
    e.value = ERROR_INSUFFICIENT_BUFFER;
    return false;
  }
  const wchar_t *source = reinterpret_cast<const wchar_t *>(
      reinterpret_cast<const unsigned char *>(data.buffer) + offset);
  memcpy(wide, source, count * sizeof(wchar_t));
  wide[count] = 0;
  const wchar_t *s = wide;
  if (!data.print_length && wcsncmp(s, L"\\??\\", 4) == 0)
    s += 4;
  int z;
  if (!utf8(s, out, static_cast<int>(cap), z, e))
    return false;
  n = z;
  return true;
}
bool native_list_directory(const char *p, native_directory_callback cb,
                           void *ctx, native_io_error &e) noexcept {
  wchar_t path[32768];
  if (!wide_path(p, path, 32766, e))
    return false;
  size_t n = wcslen(path);
  if (n && path[n - 1] != L'\\')
    path[n++] = L'\\';
  path[n++] = L'*';
  path[n] = 0;
  WIN32_FIND_DATAW d{};
  HANDLE h = FindFirstFileW(path, &d);
  if (h == INVALID_HANDLE_VALUE) {
    e.value = GetLastError();
    return false;
  }
  bool ok = true;
  do {
    if (wcscmp(d.cFileName, L".") && wcscmp(d.cFileName, L"..")) {
      char name[32768];
      int z;
      if (!utf8(d.cFileName, name, 32768, z, e) ||
          !cb(name, kind(d.dwFileAttributes), ctx)) {
        ok = false;
        break;
      }
    }
  } while (FindNextFileW(h, &d));
  DWORD last = GetLastError();
  FindClose(h);
  if (ok && last != ERROR_NO_MORE_FILES) {
    e.value = last;
    return false;
  }
  return ok;
}
bool native_space(const char *p, native_space_info &o,
                  native_io_error &e) noexcept {
  wchar_t path[32768];
  if (!wide_path(p, path, 32768, e))
    return false;
  ULARGE_INTEGER a, b, c;
  if (!GetDiskFreeSpaceExW(path, &a, &b, &c)) {
    e.value = GetLastError();
    return false;
  }
  o.available = a.QuadPart;
  o.capacity = b.QuadPart;
  o.free = c.QuadPart;
  return true;
}
bool native_copy_file(const char *a, const char *b, bool overwrite,
                      native_io_error &e) noexcept {
  wchar_t x[32768], y[32768];
  if (!wide_path(a, x, 32768, e) || !wide_path(b, y, 32768, e))
    return false;
  if (!CopyFileW(x, y, !overwrite)) {
    e.value = GetLastError();
    return false;
  }
  return true;
}
bool native_absolute_path(const char *p, char *out, native_io_size cap,
                          native_io_size &n, native_io_error &e) noexcept {
  wchar_t path[32768], full[32768];
  if (!wide_path(p, path, 32768, e))
    return false;
  DWORD got = GetFullPathNameW(path, 32768, full, nullptr);
  if (!got || got >= 32768) {
    e.value = GetLastError();
    return false;
  }
  int z;
  if (!utf8(full, out, static_cast<int>(cap), z, e))
    return false;
  n = z;
  return true;
}
bool native_canonical_path(const char *p, char *out, native_io_size cap,
                           native_io_size &n, native_io_error &e) noexcept {
  wchar_t path[32768], full[32768];
  if (!wide_path(p, path, 32768, e))
    return false;
  HANDLE handle = CreateFileW(
      path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
      OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
  if (handle == INVALID_HANDLE_VALUE) {
    e.value = GetLastError();
    return false;
  }
  DWORD got =
      GetFinalPathNameByHandleW(handle, full, 32768, FILE_NAME_NORMALIZED);
  CloseHandle(handle);
  if (!got || got >= 32768) {
    e.value = GetLastError();
    return false;
  }
  const wchar_t *value = wcsncmp(full, L"\\\\?\\", 4) == 0 ? full + 4 : full;
  int size;
  if (!utf8(value, out, static_cast<int>(cap), size, e))
    return false;
  n = static_cast<native_io_size>(size);
  return true;
}
bool native_temp_directory(char *out, native_io_size cap, native_io_size &n,
                           native_io_error &e) noexcept {
  wchar_t path[32768];
  DWORD got = GetTempPathW(32768, path);
  if (!got || got >= 32768) {
    e.value = GetLastError();
    return false;
  }
  int size;
  if (!utf8(path, out, static_cast<int>(cap), size, e))
    return false;
  n = static_cast<native_io_size>(size);
  return true;
}
} // namespace ftl::detail

#else
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>

namespace {
ftl::detail::native_file_kind kind(mode_t m) {
  using K = ftl::detail::native_file_kind;
  if (S_ISREG(m))
    return K::regular;
  if (S_ISDIR(m))
    return K::directory;
  if (S_ISLNK(m))
    return K::symlink;
  if (S_ISBLK(m))
    return K::block;
  if (S_ISCHR(m))
    return K::character;
  if (S_ISFIFO(m))
    return K::fifo;
  if (S_ISSOCK(m))
    return K::socket;
  return K::unknown;
}
} // namespace
namespace ftl::detail {
bool native_status(const char *p, bool follow, native_file_info &o,
                   native_io_error &e) noexcept {
  struct stat s{};
  if ((follow ? stat(p, &s) : lstat(p, &s))) {
    if (errno == ENOENT || errno == ENOTDIR) {
      o.kind = native_file_kind::not_found;
      return true;
    }
    e.value = errno;
    return false;
  }
  o.kind = kind(s.st_mode);
  o.permissions = s.st_mode & 07777;
  o.size = s.st_size;
  o.links = s.st_nlink;
  o.device = s.st_dev;
  o.inode = s.st_ino;
#if defined(__APPLE__)
  o.write_time = s.st_mtimespec.tv_sec * 1000000000LL + s.st_mtimespec.tv_nsec;
#else
  o.write_time = s.st_mtim.tv_sec * 1000000000LL + s.st_mtim.tv_nsec;
#endif
  return true;
}
bool native_current_path(char *out, native_io_size cap, native_io_size &n,
                         native_io_error &e) noexcept {
  if (!getcwd(out, cap)) {
    e.value = errno;
    return false;
  }
  n = strlen(out);
  return true;
}
bool native_set_current_path(const char *p, native_io_error &e) noexcept {
  if (chdir(p)) {
    e.value = errno;
    return false;
  }
  return true;
}
bool native_create_directory(const char *p, native_io_error &e) noexcept {
  if (mkdir(p, 0777)) {
    e.value = errno;
    return false;
  }
  return true;
}
bool native_create_directory_from(const char *p, const char *attributes,
                                  native_io_error &e) noexcept {
  struct stat info{};
  if (stat(attributes, &info) || mkdir(p, info.st_mode)) {
    e.value = errno;
    return false;
  }
  return true;
}
bool native_remove(const char *p, bool &r, native_io_error &e) noexcept {
  struct stat s{};
  if (lstat(p, &s)) {
    if (errno == ENOENT) {
      r = false;
      return true;
    }
    e.value = errno;
    return false;
  }
  if ((S_ISDIR(s.st_mode) ? rmdir(p) : unlink(p))) {
    e.value = errno;
    return false;
  }
  r = true;
  return true;
}
bool native_resize_file(const char *p, unsigned long long n,
                        native_io_error &e) noexcept {
  if (truncate(p, n)) {
    e.value = errno;
    return false;
  }
  return true;
}
bool native_set_write_time(const char *p, long long n,
                           native_io_error &e) noexcept {
  long long seconds = n / 1000000000LL;
  long long nanoseconds = n % 1000000000LL;
  if (nanoseconds < 0) {
    nanoseconds += 1000000000LL;
    --seconds;
  }
  timespec t[2]{{0, UTIME_OMIT}, {seconds, nanoseconds}};
  if (utimensat(AT_FDCWD, p, t, 0)) {
    e.value = errno;
    return false;
  }
  return true;
}
bool native_set_permissions(const char *p, unsigned bits, bool add,
                            bool nofollow, native_io_error &e) noexcept {
  struct stat s{};
  if ((nofollow ? lstat(p, &s) : stat(p, &s))) {
    e.value = errno;
    return false;
  }
  mode_t m = add ? (s.st_mode | bits) : (bits);
#if defined(AT_SYMLINK_NOFOLLOW)
  int result = fchmodat(AT_FDCWD, p, m, nofollow ? AT_SYMLINK_NOFOLLOW : 0);
#else
  int result = chmod(p, m);
#endif
  if (result) {
    e.value = errno;
    return false;
  }
  return true;
}
bool native_create_hard_link(const char *a, const char *b,
                             native_io_error &e) noexcept {
  if (link(a, b)) {
    e.value = errno;
    return false;
  }
  return true;
}
bool native_create_symlink(const char *a, const char *b, bool,
                           native_io_error &e) noexcept {
  if (symlink(a, b)) {
    e.value = errno;
    return false;
  }
  return true;
}
bool native_read_symlink(const char *p, char *out, native_io_size cap,
                         native_io_size &n, native_io_error &e) noexcept {
  auto z = readlink(p, out, cap);
  if (z < 0) {
    e.value = errno;
    return false;
  }
  if (static_cast<native_io_size>(z) == cap) {
    e.value = ENAMETOOLONG;
    return false;
  }
  out[z] = 0;
  n = z;
  return true;
}
bool native_list_directory(const char *p, native_directory_callback cb,
                           void *ctx, native_io_error &e) noexcept {
  DIR *d = opendir(p);
  if (!d) {
    e.value = errno;
    return false;
  }
  bool ok = true;
  errno = 0;
  while (auto *x = readdir(d)) {
    if (!strcmp(x->d_name, ".") || !strcmp(x->d_name, ".."))
      continue;
    native_file_kind k = native_file_kind::unknown;
#ifdef DT_REG
    if (x->d_type == DT_REG)
      k = native_file_kind::regular;
    else if (x->d_type == DT_DIR)
      k = native_file_kind::directory;
    else if (x->d_type == DT_LNK)
      k = native_file_kind::symlink;
#endif
    if (!cb(x->d_name, k, ctx)) {
      ok = false;
      break;
    }
  }
  int saved = errno;
  closedir(d);
  if (ok && saved) {
    e.value = saved;
    return false;
  }
  return ok;
}
bool native_space(const char *p, native_space_info &o,
                  native_io_error &e) noexcept {
  struct statvfs s{};
  if (statvfs(p, &s)) {
    e.value = errno;
    return false;
  }
  o.capacity = s.f_blocks * s.f_frsize;
  o.free = s.f_bfree * s.f_frsize;
  o.available = s.f_bavail * s.f_frsize;
  return true;
}
bool native_copy_file(const char *a, const char *b, bool overwrite,
                      native_io_error &e) noexcept {
  int in = open(a, O_RDONLY);
  if (in < 0) {
    e.value = errno;
    return false;
  }
  int flags = O_WRONLY | O_CREAT | O_TRUNC | (overwrite ? 0 : O_EXCL);
  int out = open(b, flags, 0666);
  if (out < 0) {
    e.value = errno;
    close(in);
    return false;
  }
  char buf[65536];
  bool ok = true;
  for (;;) {
    ssize_t n = read(in, buf, sizeof buf);
    if (n == 0)
      break;
    if (n < 0) {
      e.value = errno;
      ok = false;
      break;
    }
    char *q = buf;
    while (n) {
      ssize_t z = write(out, q, n);
      if (z <= 0) {
        e.value = errno;
        ok = false;
        break;
      }
      q += z;
      n -= z;
    }
    if (!ok)
      break;
  }
  close(out);
  close(in);
  return ok;
}
bool native_absolute_path(const char *p, char *out, native_io_size cap,
                          native_io_size &n, native_io_error &e) noexcept {
  if (p[0] == '/') {
    n = strlen(p);
    if (n >= cap) {
      e.value = ENAMETOOLONG;
      return false;
    }
    memcpy(out, p, n + 1);
    return true;
  }
  if (!getcwd(out, cap)) {
    e.value = errno;
    return false;
  }
  n = strlen(out);
  auto m = strlen(p);
  if (n + 1 + m >= cap) {
    e.value = ENAMETOOLONG;
    return false;
  }
  if (n && out[n - 1] != '/')
    out[n++] = '/';
  memcpy(out + n, p, m + 1);
  n += m;
  return true;
}
bool native_canonical_path(const char *p, char *out, native_io_size cap,
                           native_io_size &n, native_io_error &e) noexcept {
  char *value = realpath(p, nullptr);
  if (!value) {
    e.value = errno;
    return false;
  }
  n = strlen(value);
  if (n >= cap) {
    free(value);
    e.value = ENAMETOOLONG;
    return false;
  }
  memcpy(out, value, n + 1);
  free(value);
  return true;
}
bool native_temp_directory(char *out, native_io_size cap, native_io_size &n,
                           native_io_error &e) noexcept {
  const char *value = getenv("TMPDIR");
  if (!value || !*value)
    value = "/tmp";
  n = strlen(value);
  if (n >= cap) {
    e.value = ENAMETOOLONG;
    return false;
  }
  memcpy(out, value, n + 1);
  return true;
}
} // namespace ftl::detail
#endif
