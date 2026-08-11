// Freestanding Template Library
// SPDX-License-Identifier: MIT
#include <ftl/cstdio>
#include <ftl/detail/native_io.hpp>

struct ftl_file {
  ftl::detail::native_file_handle handle{};
  unsigned char internal_buffer[4096]{};
  unsigned char *buffer = internal_buffer;
  decltype(sizeof(0)) buffer_capacity = sizeof(internal_buffer);
  decltype(sizeof(0)) buffered = 0;
  volatile long lock = 0;
  int pushback = -1;
  unsigned buffering = _IOFBF;
  unsigned readable : 1 = 0;
  unsigned writable : 1 = 0;
  unsigned append : 1 = 0;
  unsigned binary : 1 = 0;
  unsigned eof : 1 = 0;
  unsigned failed : 1 = 0;
  unsigned owned_handle : 1 = 0;
  unsigned occupied : 1 = 0;
  unsigned io_started : 1 = 0;
  int orientation = 0;
};

namespace {
using size_type = decltype(sizeof(0));
using namespace ftl::detail;

#if defined(_WIN32)
#if !defined(__clang__)
extern "C" long __cdecl _InterlockedCompareExchange(long volatile *, long,
                                                    long);
extern "C" long __cdecl _InterlockedExchange(long volatile *, long);
extern "C" long __cdecl _InterlockedIncrement(long volatile *);
#endif
extern "C" int *_errno();
#if !defined(__clang__)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedIncrement)
#endif
int &runtime_errno() { return *_errno(); }
bool acquire(volatile long &value) {
#if defined(__clang__)
  long expected = 0;
  return __atomic_compare_exchange_n(&value, &expected, 1, false,
                                     __ATOMIC_ACQUIRE, __ATOMIC_RELAXED);
#else
  return _InterlockedCompareExchange(&value, 1, 0) == 0;
#endif
}
void release(volatile long &value) {
#if defined(__clang__)
  __atomic_store_n(&value, 0, __ATOMIC_RELEASE);
#else
  _InterlockedExchange(&value, 0);
#endif
}
#elif defined(__APPLE__)
extern "C" int *__error();
int &runtime_errno() { return *__error(); }
bool acquire(volatile long &value) {
  long expected = 0;
  return __atomic_compare_exchange_n(&value, &expected, 1, false,
                                     __ATOMIC_ACQUIRE, __ATOMIC_RELAXED);
}
void release(volatile long &value) {
  __atomic_store_n(&value, 0, __ATOMIC_RELEASE);
}
#else
extern "C" int *__errno_location() noexcept;
int &runtime_errno() { return *__errno_location(); }
bool acquire(volatile long &value) {
  long expected = 0;
  return __atomic_compare_exchange_n(&value, &expected, 1, false,
                                     __ATOMIC_ACQUIRE, __ATOMIC_RELAXED);
}
void release(volatile long &value) {
  __atomic_store_n(&value, 0, __ATOMIC_RELEASE);
}
#endif

struct file_guard {
  explicit file_guard(ftl_file *stream) : stream(stream) {
    while (!acquire(stream->lock)) {
    }
  }
  ~file_guard() { release(stream->lock); }
  ftl_file *stream;
};

constexpr unsigned stream_count = FOPEN_MAX < 32 ? 32 : FOPEN_MAX;
ftl_file streams[stream_count];
bool standard_stream_initialized[3]{};
volatile long stream_table_lock = 0;
volatile unsigned long temporary_counter = 0;

ftl_file *standard_stream(unsigned index) {
  auto &stream = streams[index];
  while (!acquire(stream_table_lock)) {
  }
  if (!standard_stream_initialized[index]) {
    stream.handle = index == 0   ? native_standard_input()
                    : index == 1 ? native_standard_output()
                                 : native_standard_error();
    stream.readable = index == 0;
    stream.writable = index != 0;
    stream.owned_handle = true;
    stream.buffering = index == 2 ? _IONBF : _IOLBF;
    stream.occupied = true;
    standard_stream_initialized[index] = true;
  }
  release(stream_table_lock);
  return &stream;
}

int stdio_error_value(native_io_error error) {
#if defined(_WIN32)
  switch (error.value) {
  case 2:
  case 3:
    return 2;
  case 5:
  case 32:
  case 33:
    return 13;
  case 6:
    return 9;
  case 80:
  case 183:
    return 17;
  case 87:
    return 22;
  case 112:
    return 28;
  case 206:
    return 36;
  default:
    return 5;
  }
#else
  return error.value != 0 ? error.value : 5;
#endif
}

void set_error(ftl_file *stream, native_io_error error) {
  stream->failed = true;
  runtime_errno() = stdio_error_value(error);
}

void set_runtime_error(native_io_error error) {
  runtime_errno() = stdio_error_value(error);
}

ftl_file *allocate_stream() {
  while (!acquire(stream_table_lock)) {
  }
  ftl_file *result = nullptr;
  for (unsigned index = 3; index != stream_count; ++index) {
    if (!streams[index].occupied) {
      streams[index] = {};
      streams[index].occupied = true;
      result = &streams[index];
      break;
    }
  }
  release(stream_table_lock);
  if (!result)
    runtime_errno() = 24;
  return result;
}

void release_stream(ftl_file *stream) {
  while (!acquire(stream_table_lock)) {
  }
  stream->occupied = false;
  release(stream_table_lock);
}

bool parse_mode(const char *mode, native_open_options &options, bool &readable,
                bool &writable, bool &binary) {
  if (!mode || (*mode != 'r' && *mode != 'w' && *mode != 'a'))
    return false;
  char operation = *mode++;
  bool update = false, exclusive = false;
  binary = false;
  for (; *mode; ++mode) {
    if (*mode == '+') {
      if (update)
        return false;
      update = true;
    } else if (*mode == 'b') {
      binary = true;
    } else if (*mode == 'x') {
      exclusive = true;
    } else {
      return false;
    }
  }
  if (exclusive && operation != 'w')
    return false;
  readable = operation == 'r' || update;
  writable = operation != 'r' || update;
  options.access = update     ? native_file_access::read_write
                   : readable ? native_file_access::read
                              : native_file_access::write;
  options.append = operation == 'a';
  options.creation = operation == 'r'   ? native_file_creation::open_existing
                     : exclusive        ? native_file_creation::create_new
                     : operation == 'w' ? native_file_creation::create_always
                                        : native_file_creation::open_or_create;
  return true;
}

bool flush_unlocked(ftl_file *stream) {
  if (!stream->writable || stream->buffered == 0)
    return true;
  native_io_size transferred = 0;
  native_io_error error;
  bool okay = native_write_file(stream->handle, stream->buffer,
                                stream->buffered, transferred, error) &&
              transferred == stream->buffered;
  if (!okay) {
    set_error(stream, error);
    return false;
  }
  stream->buffered = 0;
  return true;
}

size_type write_physical_unlocked(ftl_file *stream, const unsigned char *data,
                                  size_type count) {
  if (!stream->writable) {
    stream->failed = true;
    runtime_errno() = 9;
    return 0;
  }
  stream->io_started = true;
  size_type written = 0;
  while (written != count) {
    if (stream->buffering == _IONBF) {
      native_io_size transferred = 0;
      native_io_error error;
      if (!native_write_file(stream->handle, data + written, count - written,
                             transferred, error)) {
        set_error(stream, error);
        return written;
      }
      written += transferred;
      if (transferred == 0)
        return written;
      continue;
    }
    if (stream->buffered == stream->buffer_capacity && !flush_unlocked(stream))
      return written;
    unsigned char value = data[written++];
    stream->buffer[stream->buffered++] = value;
    if (stream->buffering == _IOLBF && value == '\n' && !flush_unlocked(stream))
      return written - 1;
  }
  return written;
}

size_type write_unlocked(ftl_file *stream, const unsigned char *data,
                         size_type count) {
#if defined(_WIN32)
  if (!stream->binary) {
    size_type written = 0;
    while (written != count) {
      if (data[written] == '\n') {
        static constexpr unsigned char newline[] = {'\r', '\n'};
        if (write_physical_unlocked(stream, newline, 2) != 2)
          break;
      } else if (write_physical_unlocked(stream, data + written, 1) != 1) {
        break;
      }
      ++written;
    }
    return written;
  }
#endif
  return write_physical_unlocked(stream, data, count);
}

size_type read_physical_unlocked(ftl_file *stream, unsigned char *data,
                                 size_type count) {
  if (!stream->readable) {
    stream->failed = true;
    runtime_errno() = 9;
    return 0;
  }
  stream->io_started = true;
  size_type read_count = 0;
  if (stream->pushback != -1 && count != 0) {
    data[read_count++] = static_cast<unsigned char>(stream->pushback);
    stream->pushback = -1;
  }
  while (read_count != count) {
    native_io_size transferred = 0;
    native_io_error error;
    if (!native_read_file(stream->handle, data + read_count, count - read_count,
                          transferred, error)) {
      set_error(stream, error);
      return read_count;
    }
    if (transferred == 0) {
      stream->eof = true;
      break;
    }
    read_count += transferred;
  }
  return read_count;
}

size_type read_unlocked(ftl_file *stream, unsigned char *data,
                        size_type count) {
#if defined(_WIN32)
  if (!stream->binary) {
    size_type read_count = 0;
    while (read_count != count) {
      unsigned char value;
      if (read_physical_unlocked(stream, &value, 1) != 1)
        break;
      if (value == 0x1a) {
        stream->eof = true;
        break;
      }
      if (value == '\r') {
        unsigned char next;
        if (read_physical_unlocked(stream, &next, 1) == 1) {
          if (next == '\n')
            value = '\n';
          else
            stream->pushback = next;
        }
      }
      data[read_count++] = value;
    }
    return read_count;
  }
#endif
  return read_physical_unlocked(stream, data, count);
}

void copy_text(char *destination, const char *source) {
  while ((*destination++ = *source++) != 0) {
  }
}

void append_decimal(char *&output, unsigned long value) {
  char digits[32];
  unsigned count = 0;
  do {
    digits[count++] = static_cast<char>('0' + value % 10);
    value /= 10;
  } while (value != 0);
  while (count)
    *output++ = digits[--count];
}

const char *error_text(int value) {
  switch (value) {
  case 2:
    return "No such file or directory";
  case 9:
    return "Bad file descriptor";
  case 12:
    return "Not enough memory";
  case 13:
    return "Permission denied";
  case 17:
    return "File exists";
  case 22:
    return "Invalid argument";
  case 24:
    return "Too many open files";
  case 28:
    return "No space left on device";
  case 34:
    return "Result out of range";
  case 36:
    return "File name too long";
  default:
    return "I/O error";
  }
}

} // namespace

namespace ftl_stdio_runtime {

file_type *input_stream() noexcept { return standard_stream(0); }
file_type *output_stream() noexcept { return standard_stream(1); }
file_type *error_stream() noexcept { return standard_stream(2); }
native_file_handle native_handle(file_type *stream) noexcept {
  return stream ? stream->handle : native_file_handle{};
}
bool is_terminal(file_type *stream) noexcept {
  return stream && native_file_is_terminal(stream->handle);
}
int orient(file_type *stream, int mode) noexcept {
  if (!stream)
    return 0;
  file_guard guard(stream);
  if (stream->orientation == 0 && mode != 0)
    stream->orientation = mode < 0 ? -1 : 1;
  return stream->orientation;
}
void lock_file(file_type *stream) noexcept {
  if (stream)
    while (!acquire(stream->lock)) {
    }
}
void unlock_file(file_type *stream) noexcept {
  if (stream)
    release(stream->lock);
}
size_type write_bytes_locked(const void *data, size_type count,
                             file_type *stream) noexcept {
  if (!stream || stream->orientation > 0)
    return 0;
  stream->orientation = -1;
  return write_unlocked(stream, static_cast<const unsigned char *>(data),
                        count);
}
size_type read_bytes_locked(void *data, size_type count,
                            file_type *stream) noexcept {
  if (!stream || stream->orientation > 0)
    return 0;
  stream->orientation = -1;
  return read_unlocked(stream, static_cast<unsigned char *>(data), count);
}
int unget_byte_locked(int value, file_type *stream) noexcept {
  if (!stream || value == EOF || stream->orientation > 0 || !stream->readable ||
      stream->pushback != -1)
    return EOF;
  stream->orientation = -1;
  stream->pushback = static_cast<unsigned char>(value);
  stream->eof = false;
  return stream->pushback;
}
int read_wide_byte_locked(file_type *stream) noexcept {
  if (!stream || stream->orientation < 0)
    return EOF;
  stream->orientation = 1;
  unsigned char value;
  return read_unlocked(stream, &value, 1) == 1 ? value : EOF;
}
int write_wide_byte_locked(int value, file_type *stream) noexcept {
  if (!stream || stream->orientation < 0)
    return EOF;
  stream->orientation = 1;
  unsigned char byte = static_cast<unsigned char>(value);
  return write_unlocked(stream, &byte, 1) == 1 ? byte : EOF;
}
int unget_wide_byte_locked(int value, file_type *stream) noexcept {
  if (!stream || value == EOF || stream->orientation < 0 || !stream->readable ||
      stream->pushback != -1)
    return EOF;
  stream->orientation = 1;
  stream->pushback = static_cast<unsigned char>(value);
  stream->eof = false;
  return stream->pushback;
}
int read_wide_byte(file_type *stream) noexcept {
  if (!stream)
    return EOF;
  file_guard guard(stream);
  return read_wide_byte_locked(stream);
}
int write_wide_byte(int value, file_type *stream) noexcept {
  if (!stream)
    return EOF;
  file_guard guard(stream);
  return write_wide_byte_locked(value, stream);
}
int unget_wide_byte(int value, file_type *stream) noexcept {
  if (!stream || value == EOF)
    return EOF;
  file_guard guard(stream);
  return unget_wide_byte_locked(value, stream);
}

size_type fwrite(const void *data, size_type size, size_type count,
                 file_type *stream) {
  if (!stream || size == 0 || count == 0)
    return 0;
  if (count > static_cast<size_type>(-1) / size) {
    runtime_errno() = 34;
    return 0;
  }
  file_guard guard(stream);
  return write_bytes_locked(data, size * count, stream) / size;
}

int fflush(file_type *stream) {
  if (stream) {
    file_guard guard(stream);
    return flush_unlocked(stream) ? 0 : EOF;
  }
  int result = 0;
  while (!acquire(stream_table_lock)) {
  }
  for (unsigned index = 0; index != stream_count; ++index) {
    auto &candidate = streams[index];
    if (candidate.occupied && candidate.writable) {
      file_guard guard(&candidate);
      if (!flush_unlocked(&candidate))
        result = EOF;
    }
  }
  release(stream_table_lock);
  return result;
}

} // namespace ftl_stdio_runtime

namespace ftl_cstdio_runtime {

using namespace ftl::detail;

int remove(const char *path) {
  native_io_error error;
  if (!native_remove_file(path, error)) {
    set_runtime_error(error);
    return -1;
  }
  return 0;
}

int rename(const char *old_path, const char *new_path) {
  native_io_error error;
  if (!native_rename_file(old_path, new_path, error)) {
    set_runtime_error(error);
    return -1;
  }
  return 0;
}

FILE *fopen(const char *path, const char *mode) {
  native_open_options options;
  bool readable, writable, binary;
  if (!parse_mode(mode, options, readable, writable, binary)) {
    runtime_errno() = 22;
    return nullptr;
  }
  FILE *stream = allocate_stream();
  if (!stream)
    return nullptr;
  native_io_error error;
  if (!native_open_file(path, options, stream->handle, error)) {
    release_stream(stream);
    set_runtime_error(error);
    return nullptr;
  }
  stream->readable = readable;
  stream->writable = writable;
  stream->append = options.append;
  stream->binary = binary;
  stream->owned_handle = true;
  return stream;
}

FILE *freopen(const char *path, const char *mode, FILE *stream) {
  if (!stream)
    return nullptr;
  native_open_options options;
  bool readable, writable, binary;
  if (!parse_mode(mode, options, readable, writable, binary)) {
    runtime_errno() = 22;
    return nullptr;
  }
  while (!acquire(stream_table_lock)) {
  }
  while (!acquire(stream->lock)) {
  }
  (void)flush_unlocked(stream);
  if (stream->owned_handle) {
    native_io_error ignored;
    (void)native_close_file(stream->handle, ignored);
  }
  native_io_error error;
  if (!native_open_file(path, options, stream->handle, error)) {
    stream->failed = true;
    stream->occupied = false;
    set_runtime_error(error);
    release(stream->lock);
    release(stream_table_lock);
    return nullptr;
  }
  stream->readable = readable;
  stream->writable = writable;
  stream->append = options.append;
  stream->binary = binary;
  stream->owned_handle = true;
  stream->eof = stream->failed = false;
  stream->pushback = -1;
  stream->buffered = 0;
  stream->orientation = 0;
  stream->io_started = false;
  release(stream->lock);
  release(stream_table_lock);
  return stream;
}

int fclose(FILE *stream) {
  if (!stream)
    return EOF;
  while (!acquire(stream_table_lock)) {
  }
  while (!acquire(stream->lock)) {
  }
  bool okay = flush_unlocked(stream);
  if (stream->owned_handle) {
    native_io_error error;
    if (!native_close_file(stream->handle, error)) {
      set_error(stream, error);
      okay = false;
    }
  }
  stream->occupied = false;
  release(stream->lock);
  release(stream_table_lock);
  return okay ? 0 : EOF;
}

void setbuf(FILE *stream, char *buffer) {
  setvbuf(stream, buffer, buffer ? _IOFBF : _IONBF, BUFSIZ);
}

int setvbuf(FILE *stream, char *buffer, int mode, size_t size) {
  if (!stream || (mode != _IOFBF && mode != _IOLBF && mode != _IONBF) ||
      (mode != _IONBF && size == 0))
    return -1;
  file_guard guard(stream);
  if (stream->io_started)
    return -1;
  stream->buffering = static_cast<unsigned>(mode);
  if (mode != _IONBF) {
    stream->buffer = buffer ? reinterpret_cast<unsigned char *>(buffer)
                            : stream->internal_buffer;
    stream->buffer_capacity = buffer ? size : sizeof(stream->internal_buffer);
  }
  return 0;
}

size_t fread(void *data, size_t size, size_t count, FILE *stream) {
  if (!stream || size == 0 || count == 0)
    return 0;
  if (count > static_cast<size_t>(-1) / size) {
    runtime_errno() = 34;
    return 0;
  }
  file_guard guard(stream);
  return ftl_stdio_runtime::read_bytes_locked(data, size * count, stream) /
         size;
}

int fgetc(FILE *stream) {
  unsigned char value;
  return fread(&value, 1, 1, stream) == 1 ? value : EOF;
}
int getc(FILE *stream) { return fgetc(stream); }
int getchar() { return fgetc(ftl_stdio_runtime::input_stream()); }

char *fgets(char *destination, int count, FILE *stream) {
  if (!destination || count <= 0 || !stream)
    return nullptr;
  if (count == 1) {
    destination[0] = 0;
    return destination;
  }
  ftl_stdio_runtime::lock_file(stream);
  int used = 0;
  while (used + 1 < count) {
    unsigned char value;
    if (ftl_stdio_runtime::read_bytes_locked(&value, 1, stream) != 1)
      break;
    destination[used++] = static_cast<char>(value);
    if (value == '\n')
      break;
  }
  ftl_stdio_runtime::unlock_file(stream);
  if (used == 0)
    return nullptr;
  destination[used] = 0;
  return destination;
}

int fputc(int value, FILE *stream) {
  unsigned char byte = static_cast<unsigned char>(value);
  return ftl_stdio_runtime::fwrite(&byte, 1, 1, stream) == 1 ? byte : EOF;
}
int putc(int value, FILE *stream) { return fputc(value, stream); }
int putchar(int value) {
  return fputc(value, ftl_stdio_runtime::output_stream());
}
int fputs(const char *text, FILE *stream) {
  size_t length = 0;
  while (text[length])
    ++length;
  return ftl_stdio_runtime::fwrite(text, 1, length, stream) == length ? 0 : EOF;
}
int puts(const char *text) {
  FILE *stream = ftl_stdio_runtime::output_stream();
  ftl_stdio_runtime::lock_file(stream);
  size_t length = 0;
  while (text[length])
    ++length;
  bool okay =
      ftl_stdio_runtime::write_bytes_locked(text, length, stream) == length &&
      ftl_stdio_runtime::write_bytes_locked("\n", 1, stream) == 1;
  ftl_stdio_runtime::unlock_file(stream);
  return okay ? 0 : EOF;
}

int ungetc(int value, FILE *stream) {
  if (!stream || value == EOF)
    return EOF;
  file_guard guard(stream);
  return ftl_stdio_runtime::unget_byte_locked(value, stream);
}

int fseek(FILE *stream, long offset, int origin) {
  if (!stream ||
      (origin != SEEK_SET && origin != SEEK_CUR && origin != SEEK_END))
    return -1;
  file_guard guard(stream);
  if (!flush_unlocked(stream))
    return -1;
  native_io_offset position;
  native_io_error error;
  native_seek_origin native_origin =
      origin == SEEK_SET   ? native_seek_origin::begin
      : origin == SEEK_CUR ? native_seek_origin::current
                           : native_seek_origin::end;
  native_io_offset adjusted_offset = offset;
  if (origin == SEEK_CUR && stream->pushback != -1)
    --adjusted_offset;
  if (!native_seek_file(stream->handle, adjusted_offset, native_origin,
                        position, error)) {
    set_error(stream, error);
    return -1;
  }
  stream->pushback = -1;
  stream->eof = false;
  return 0;
}

long ftell(FILE *stream) {
  if (!stream)
    return -1;
  file_guard guard(stream);
  native_io_offset position;
  native_io_error error;
  if (!native_seek_file(stream->handle, 0, native_seek_origin::current,
                        position, error)) {
    set_error(stream, error);
    return -1;
  }
  position += static_cast<native_io_offset>(stream->buffered);
  if (stream->pushback != -1)
    --position;
  if constexpr (sizeof(long) == 4) {
    if (position < -2147483647LL - 1 || position > 2147483647LL) {
      runtime_errno() = 34;
      return -1;
    }
  }
  return static_cast<long>(position);
}

void rewind(FILE *stream) {
  fseek(stream, 0, SEEK_SET);
  clearerr(stream);
}

int fgetpos(FILE *stream, fpos_t *position) {
  if (!stream || !position)
    return -1;
  file_guard guard(stream);
  native_io_offset value;
  native_io_error error;
  if (!native_seek_file(stream->handle, 0, native_seek_origin::current, value,
                        error)) {
    set_error(stream, error);
    return -1;
  }
  value += static_cast<native_io_offset>(stream->buffered);
  if (stream->pushback != -1)
    --value;
  position->position = value;
  position->state = {};
  return 0;
}

int fsetpos(FILE *stream, const fpos_t *position) {
  if (!stream || !position)
    return -1;
  file_guard guard(stream);
  if (!flush_unlocked(stream))
    return -1;
  native_io_offset result;
  native_io_error error;
  if (!native_seek_file(stream->handle, position->position,
                        native_seek_origin::begin, result, error)) {
    set_error(stream, error);
    return -1;
  }
  stream->pushback = -1;
  stream->eof = false;
  return 0;
}

void clearerr(FILE *stream) {
  if (stream) {
    file_guard guard(stream);
    stream->eof = stream->failed = false;
  }
}
int feof(FILE *stream) {
  if (!stream)
    return 0;
  file_guard guard(stream);
  return stream->eof;
}
int ferror(FILE *stream) {
  if (!stream)
    return 0;
  file_guard guard(stream);
  return stream->failed;
}

char *tmpnam(char *destination) {
  static char shared[L_tmpnam];
  if (!destination)
    destination = shared;
  for (unsigned attempt = 0; attempt != TMP_MAX; ++attempt) {
    char *output = destination;
    copy_text(output, "ftl-tmp-");
    while (*output)
      ++output;
#if defined(_WIN32)
#if defined(__clang__)
    unsigned long number =
        __atomic_add_fetch(&temporary_counter, 1, __ATOMIC_RELAXED);
#else
    unsigned long number = static_cast<unsigned long>(_InterlockedIncrement(
        reinterpret_cast<volatile long *>(&temporary_counter)));
#endif
#else
    unsigned long number =
        __atomic_add_fetch(&temporary_counter, 1, __ATOMIC_RELAXED);
#endif
    append_decimal(output, number);
    copy_text(output, ".tmp");
    native_open_options options;
    options.access = native_file_access::write;
    options.creation = native_file_creation::create_new;
    native_file_handle handle;
    native_io_error error;
    if (!native_open_file(destination, options, handle, error))
      continue;
    native_io_error ignored;
    bool closed = native_close_file(handle, ignored);
    bool removed = native_remove_file(destination, ignored);
    if (closed && removed)
      return destination;
  }
  return nullptr;
}

FILE *tmpfile() {
  native_open_options options;
  options.access = native_file_access::read_write;
  options.creation = native_file_creation::create_new;
  options.temporary = true;
  for (unsigned attempt = 0; attempt != TMP_MAX; ++attempt) {
    char path[L_tmpnam];
    if (!tmpnam(path))
      return nullptr;
    FILE *stream = allocate_stream();
    if (!stream)
      return nullptr;
    native_io_error error;
    if (!native_open_file(path, options, stream->handle, error)) {
      release_stream(stream);
      continue;
    }
    stream->readable = true;
    stream->writable = true;
    stream->binary = true;
    stream->owned_handle = true;
#if !defined(_WIN32)
    native_io_error ignored;
    (void)native_remove_file(path, ignored);
#endif
    return stream;
  }
  runtime_errno() = 17;
  return nullptr;
}

void perror(const char *prefix) {
  int saved_error = runtime_errno();
  FILE *stream = ftl_stdio_runtime::error_stream();
  ftl_stdio_runtime::lock_file(stream);
  if (prefix && *prefix) {
    size_t length = 0;
    while (prefix[length])
      ++length;
    (void)ftl_stdio_runtime::write_bytes_locked(prefix, length, stream);
    (void)ftl_stdio_runtime::write_bytes_locked(": ", 2, stream);
  }
  const char *message = error_text(saved_error);
  size_t length = 0;
  while (message[length])
    ++length;
  (void)ftl_stdio_runtime::write_bytes_locked(message, length, stream);
  (void)ftl_stdio_runtime::write_bytes_locked("\n", 1, stream);
  ftl_stdio_runtime::unlock_file(stream);
}

} // namespace ftl_cstdio_runtime
