// Freestanding Template Library
// SPDX-License-Identifier: MIT
#include <ftl/charconv>
#include <ftl/clocale>
#include <ftl/cstdint>
#include <ftl/cstdio>
#include <ftl/limits>
#include <ftl/type_traits>
#include <ftl/vector>

namespace {
using size_type = decltype(sizeof(0));
using file_type = ::ftl_file;
using va_list_type = ftl::va_list;

struct output_sink {
  file_type *file{};
  char *buffer{};
  size_type capacity{};
  size_type count{};
  bool failed{};
  bool locked{};

  void put(char value) {
    if (buffer && count + 1 < capacity)
      buffer[count] = value;
    if (file) {
      size_type written =
          locked ? ::ftl_stdio_runtime::write_bytes_locked(&value, 1, file)
                 : ::ftl_stdio_runtime::fwrite(&value, 1, 1, file);
      if (written != 1)
        failed = true;
    }
    ++count;
  }
  void write(const char *text, size_type size) {
    for (size_type index = 0; index != size; ++index)
      put(text[index]);
  }
  void finish() {
    if (buffer && capacity)
      buffer[count < capacity ? count : capacity - 1] = 0;
  }
};

enum class length_type { none, hh, h, l, ll, j, z, t, big_l };

struct conversion {
  bool left{}, plus{}, space{}, alternate{}, zero{};
  int width{};
  int precision{-1};
  length_type length{};
  char type{};
};

bool digit(char value) { return value >= '0' && value <= '9'; }

const char *parse_conversion(const char *format, conversion &item,
                             va_list_type &arguments) {
  for (;; ++format) {
    if (*format == '-')
      item.left = true;
    else if (*format == '+')
      item.plus = true;
    else if (*format == ' ')
      item.space = true;
    else if (*format == '#')
      item.alternate = true;
    else if (*format == '0')
      item.zero = true;
    else
      break;
  }
  if (*format == '*') {
    item.width = va_arg(arguments, int);
    ++format;
    if (item.width < 0) {
      item.left = true;
      item.width = -item.width;
    }
  } else {
    while (digit(*format))
      item.width = item.width * 10 + (*format++ - '0');
  }
  if (*format == '.') {
    ++format;
    item.precision = 0;
    if (*format == '*') {
      item.precision = va_arg(arguments, int);
      ++format;
      if (item.precision < 0)
        item.precision = -1;
    } else {
      while (digit(*format))
        item.precision = item.precision * 10 + (*format++ - '0');
    }
  }
  if (*format == 'h') {
    item.length =
        *++format == 'h' ? (++format, length_type::hh) : length_type::h;
  } else if (*format == 'l') {
    item.length =
        *++format == 'l' ? (++format, length_type::ll) : length_type::l;
  } else if (*format == 'j') {
    item.length = length_type::j;
    ++format;
  } else if (*format == 'z') {
    item.length = length_type::z;
    ++format;
  } else if (*format == 't') {
    item.length = length_type::t;
    ++format;
  } else if (*format == 'L') {
    item.length = length_type::big_l;
    ++format;
  }
  item.type = *format;
  return *format ? format + 1 : format;
}

void padded(output_sink &sink, const conversion &item, const char *prefix,
            size_type prefix_size, const char *text, size_type text_size,
            int leading_zeroes = 0) {
  size_type total =
      prefix_size + static_cast<size_type>(leading_zeroes) + text_size;
  int padding = item.width > static_cast<int>(total)
                    ? item.width - static_cast<int>(total)
                    : 0;
  char pad = item.zero && !item.left && item.precision < 0 ? '0' : ' ';
  if (!item.left && pad == ' ')
    while (padding--)
      sink.put(' ');
  sink.write(prefix, prefix_size);
  if (!item.left && pad == '0')
    while (padding--)
      sink.put('0');
  while (leading_zeroes--)
    sink.put('0');
  sink.write(text, text_size);
  if (item.left)
    while (padding--)
      sink.put(' ');
}

unsigned long long unsigned_argument(va_list_type &args, length_type length) {
  switch (length) {
  case length_type::ll:
    return va_arg(args, unsigned long long);
  case length_type::l:
    return va_arg(args, unsigned long);
  case length_type::j:
    return va_arg(args, ftl::uintmax_t);
  case length_type::z:
    return va_arg(args, size_type);
  case length_type::t:
    return static_cast<unsigned long long>(
        va_arg(args, ftl::make_unsigned_t<decltype((char *)0 - (char *)0)>));
  case length_type::hh:
    return static_cast<unsigned char>(va_arg(args, unsigned int));
  case length_type::h:
    return static_cast<unsigned short>(va_arg(args, unsigned int));
  default:
    return va_arg(args, unsigned int);
  }
}

long long signed_argument(va_list_type &args, length_type length) {
  switch (length) {
  case length_type::ll:
    return va_arg(args, long long);
  case length_type::l:
    return va_arg(args, long);
  case length_type::j:
    return va_arg(args, ftl::intmax_t);
  case length_type::z:
  case length_type::t:
    return va_arg(args, decltype((char *)0 - (char *)0));
  case length_type::hh:
    return static_cast<signed char>(va_arg(args, int));
  case length_type::h:
    return static_cast<short>(va_arg(args, int));
  default:
    return va_arg(args, int);
  }
}

void integer(output_sink &sink, conversion item, unsigned long long magnitude,
             bool negative, unsigned base, bool upper) {
  char digits[65];
  auto result = ftl::to_chars(digits, digits + sizeof(digits), magnitude,
                              static_cast<int>(base));
  if (result.ec != ftl::errc{}) {
    sink.failed = true;
    return;
  }
  size_type size = static_cast<size_type>(result.ptr - digits);
  if (item.precision == 0 && magnitude == 0)
    size = 0;
  if (upper)
    for (size_type index = 0; index != size; ++index)
      if (digits[index] >= 'a' && digits[index] <= 'f')
        digits[index] = static_cast<char>(digits[index] - 'a' + 'A');
  char prefix[3];
  size_type prefix_size = 0;
  if (negative)
    prefix[prefix_size++] = '-';
  else if (item.plus)
    prefix[prefix_size++] = '+';
  else if (item.space)
    prefix[prefix_size++] = ' ';
  if (item.alternate && base == 8 &&
      (size == 0 ||
       (digits[0] != '0' && item.precision <= static_cast<int>(size))))
    prefix[prefix_size++] = '0';
  else if (item.alternate && base == 16 && magnitude != 0) {
    prefix[prefix_size++] = '0';
    prefix[prefix_size++] = upper ? 'X' : 'x';
  }
  int zeroes = item.precision > static_cast<int>(size)
                   ? item.precision - static_cast<int>(size)
                   : 0;
  padded(sink, item, prefix, prefix_size, digits, size, zeroes);
}

void floating(output_sink &sink, conversion item, va_list_type &args) {
  long double value = item.length == length_type::big_l
                          ? va_arg(args, long double)
                          : va_arg(args, double);
  auto format =
      item.type == 'f' || item.type == 'F'   ? ftl::chars_format::fixed
      : item.type == 'e' || item.type == 'E' ? ftl::chars_format::scientific
      : item.type == 'a' || item.type == 'A' ? ftl::chars_format::hex
                                             : ftl::chars_format::general;
  if (item.precision < 0) {
    item.precision =
        item.type == 'a' || item.type == 'A'
            ? (item.length == length_type::big_l
                   ? (ftl::numeric_limits<long double>::digits - 1 + 3) / 4
                   : (ftl::numeric_limits<double>::digits - 1 + 3) / 4)
            : 6;
  } else if ((item.type == 'g' || item.type == 'G') && item.precision == 0) {
    item.precision = 1;
  }
  const size_type requested = static_cast<size_type>(item.precision);
  if (requested > static_cast<size_type>(-1) - 8192) {
    sink.failed = true;
    return;
  }
  ftl::vector<char> storage(requested + 8192);
  char *text = storage.data();
  const size_type text_capacity = storage.size();
  auto result = item.precision >= 0
                    ? ftl::to_chars(text, text + text_capacity, value, format,
                                    item.precision)
                    : ftl::to_chars(text, text + text_capacity, value, format);
  if (result.ec != ftl::errc{}) {
    sink.failed = true;
    return;
  }
  size_type size = static_cast<size_type>(result.ptr - text);
  if (item.type >= 'A' && item.type <= 'Z')
    for (size_type index = 0; index != size; ++index)
      if (text[index] >= 'a' && text[index] <= 'z')
        text[index] = static_cast<char>(text[index] - 'a' + 'A');
  size_type number_start = size && (text[0] == '-' || text[0] == '+') ? 1 : 0;
  bool special = number_start < size &&
                 (text[number_start] == 'i' || text[number_start] == 'I' ||
                  text[number_start] == 'n' || text[number_start] == 'N');
  if (item.alternate && !special) {
    size_type insertion = size;
    for (size_type index = 0; index != size; ++index) {
      if (text[index] == '.') {
        insertion = size;
        break;
      }
      if (text[index] == 'e' || text[index] == 'E' || text[index] == 'p' ||
          text[index] == 'P') {
        insertion = index;
        break;
      }
    }
    bool has_point = false;
    for (size_type index = 0; index != insertion; ++index)
      if (text[index] == '.')
        has_point = true;
    if (!has_point && size + 1 < text_capacity) {
      for (size_type index = size; index != insertion; --index)
        text[index] = text[index - 1];
      text[insertion] = '.';
      ++size;
    }
  }
  if (item.alternate && !special && (item.type == 'g' || item.type == 'G')) {
    size_type exponent = size;
    for (size_type index = number_start; index != size; ++index)
      if (text[index] == 'e' || text[index] == 'E') {
        exponent = index;
        break;
      }
    int significant = 0;
    bool nonzero_seen = false;
    for (size_type index = number_start; index != exponent; ++index) {
      if (text[index] < '0' || text[index] > '9')
        continue;
      if (text[index] != '0')
        nonzero_seen = true;
      if (nonzero_seen)
        ++significant;
    }
    if (!nonzero_seen)
      significant = 1;
    int trailing_zeroes = item.precision - significant;
    if (trailing_zeroes > 0) {
      if (size + static_cast<size_type>(trailing_zeroes) >= text_capacity) {
        sink.failed = true;
        return;
      }
      for (size_type index = size; index != exponent; --index)
        text[index + trailing_zeroes - 1] = text[index - 1];
      for (int index = 0; index != trailing_zeroes; ++index)
        text[exponent + static_cast<size_type>(index)] = '0';
      size += static_cast<size_type>(trailing_zeroes);
    }
  }
  const lconv *numeric = ::localeconv();
  const char *radix = numeric != nullptr ? numeric->decimal_point : nullptr;
  if (!special && radix != nullptr && radix[0] != '\0' &&
      !(radix[0] == '.' && radix[1] == '\0')) {
    size_type radix_size = 0;
    while (radix[radix_size] != '\0')
      ++radix_size;
    for (size_type index = 0; index != size; ++index) {
      if (text[index] != '.')
        continue;
      if (size + radix_size - 1 >= text_capacity) {
        sink.failed = true;
        return;
      }
      for (size_type tail = size; tail != index + 1; --tail)
        text[tail + radix_size - 2] = text[tail - 1];
      for (size_type part = 0; part != radix_size; ++part)
        text[index + part] = radix[part];
      size += radix_size - 1;
      break;
    }
  }
  char prefix[3];
  size_type prefix_size = 0;
  if (size && text[0] == '-') {
    prefix[prefix_size++] = '-';
    for (size_type index = 0; index + 1 < size; ++index)
      text[index] = text[index + 1];
    --size;
  } else if (item.plus)
    prefix[prefix_size++] = '+';
  else if (item.space)
    prefix[prefix_size++] = ' ';
  if (!special && (item.type == 'a' || item.type == 'A')) {
    prefix[prefix_size++] = '0';
    prefix[prefix_size++] = item.type == 'A' ? 'X' : 'x';
  }
  padded(sink, item, prefix, prefix_size, text, size);
}

int format_to(output_sink &sink, const char *format, va_list_type &arguments) {
  while (*format) {
    if (*format != '%') {
      sink.put(*format++);
      continue;
    }
    ++format;
    if (*format == '%') {
      sink.put(*format++);
      continue;
    }
    conversion item;
    format = parse_conversion(format, item, arguments);
    if (!item.type)
      return -1;
    if (item.type == 'd' || item.type == 'i') {
      long long value = signed_argument(arguments, item.length);
      auto magnitude = value < 0 ? 0ULL - static_cast<unsigned long long>(value)
                                 : static_cast<unsigned long long>(value);
      integer(sink, item, magnitude, value < 0, 10, false);
    } else if (item.type == 'u' || item.type == 'o' || item.type == 'x' ||
               item.type == 'X') {
      unsigned base = item.type == 'o' ? 8 : item.type == 'u' ? 10 : 16;
      integer(sink, item, unsigned_argument(arguments, item.length), false,
              base, item.type == 'X');
    } else if (item.type == 'c') {
      auto raw = va_arg(arguments, int);
      if (item.length == length_type::l &&
          static_cast<unsigned int>(raw) > 0x7f) {
        sink.failed = true;
        continue;
      }
      char value = static_cast<char>(raw);
      padded(sink, item, nullptr, 0, &value, 1);
    } else if (item.type == 's') {
      if (item.length == length_type::l) {
        const wchar_t *value = va_arg(arguments, const wchar_t *);
        size_type size = 0;
        if (value)
          while (value[size] && (item.precision < 0 ||
                                 size < static_cast<size_type>(item.precision)))
            ++size;
        int padding = item.width > static_cast<int>(size)
                          ? item.width - static_cast<int>(size)
                          : 0;
        if (!item.left)
          while (padding--)
            sink.put(' ');
        for (size_type index = 0; index != size; ++index) {
          if (static_cast<unsigned long long>(value[index]) > 0x7f) {
            sink.failed = true;
            break;
          }
          sink.put(static_cast<char>(value[index]));
        }
        if (item.left)
          while (padding--)
            sink.put(' ');
      } else {
        const char *value = va_arg(arguments, const char *);
        if (!value)
          value = "(null)";
        size_type size = 0;
        while (value[size] && (item.precision < 0 ||
                               size < static_cast<size_type>(item.precision)))
          ++size;
        padded(sink, item, nullptr, 0, value, size);
      }
    } else if (item.type == 'p') {
      item.alternate = true;
      integer(sink, item,
              reinterpret_cast<ftl::uintptr_t>(va_arg(arguments, void *)),
              false, 16, false);
    } else if (item.type == 'n') {
      if (item.length == length_type::hh)
        *va_arg(arguments, signed char *) =
            static_cast<signed char>(sink.count);
      else if (item.length == length_type::h)
        *va_arg(arguments, short *) = static_cast<short>(sink.count);
      else if (item.length == length_type::l)
        *va_arg(arguments, long *) = static_cast<long>(sink.count);
      else if (item.length == length_type::ll)
        *va_arg(arguments, long long *) = static_cast<long long>(sink.count);
      else if (item.length == length_type::j)
        *va_arg(arguments, ftl::intmax_t *) =
            static_cast<ftl::intmax_t>(sink.count);
      else if (item.length == length_type::z)
        *va_arg(arguments, ftl::make_signed_t<size_type> *) =
            static_cast<ftl::make_signed_t<size_type>>(sink.count);
      else if (item.length == length_type::t)
        *va_arg(arguments, decltype((char *)0 - (char *)0) *) =
            static_cast<decltype((char *)0 - (char *)0)>(sink.count);
      else
        *va_arg(arguments, int *) = static_cast<int>(sink.count);
    } else if ((item.type >= 'a' && item.type <= 'g') ||
               (item.type >= 'A' && item.type <= 'G')) {
      floating(sink, item, arguments);
    } else {
      return -1;
    }
  }
  sink.finish();
  return sink.failed || sink.count > static_cast<size_type>(0x7fffffff)
             ? -1
             : static_cast<int>(sink.count);
}
} // namespace

namespace {
struct input_source {
  file_type *file{};
  const char *text{};
  const wchar_t *wide_text{};
  size_type position{};
  size_type consumed{};
  bool last_was_eof{};
  bool wide_file{};
  bool locked{};

  int get() {
    int value = file ? wide_file
                           ? locked ? ::ftl_stdio_runtime::read_wide_byte_locked(file)
                                    : ::ftl_stdio_runtime::read_wide_byte(file)
                           : locked ? [&] {
                               unsigned char byte;
                               return ::ftl_stdio_runtime::read_bytes_locked(
                                          &byte, 1, file) == 1
                                          ? static_cast<int>(byte)
                                          : EOF;
                             }()
                                    : ::ftl_cstdio_runtime::fgetc(file)
                     : wide_text ? static_cast<int>(wide_text[position])
                                 : static_cast<unsigned char>(text[position]);
    if (!file && value != 0)
      ++position;
    last_was_eof = value == EOF || value == 0;
    if (value != EOF && value != 0)
      ++consumed;
    return value == 0 ? EOF : value;
  }
  void unget(int value) {
    if (value == EOF)
      return;
    if (file)
      (void)(wide_file
                 ? locked ? ::ftl_stdio_runtime::unget_wide_byte_locked(value,
                                                                        file)
                          : ::ftl_stdio_runtime::unget_wide_byte(value, file)
             : locked ? ::ftl_stdio_runtime::unget_byte_locked(value, file)
                      : ::ftl_cstdio_runtime::ungetc(value, file));
    else
      --position;
    --consumed;
  }
};

bool space_character(int value) {
  return value == ' ' || value == '\t' || value == '\n' || value == '\r' ||
         value == '\f' || value == '\v';
}

void skip_space(input_source &source) {
  int value;
  do {
    value = source.get();
  } while (space_character(value));
  source.unget(value);
}

void store_signed(va_list_type &args, length_type length, long long value) {
  if (length == length_type::hh)
    *va_arg(args, signed char *) = static_cast<signed char>(value);
  else if (length == length_type::h)
    *va_arg(args, short *) = static_cast<short>(value);
  else if (length == length_type::l)
    *va_arg(args, long *) = static_cast<long>(value);
  else if (length == length_type::ll)
    *va_arg(args, long long *) = value;
  else if (length == length_type::j)
    *va_arg(args, ftl::intmax_t *) = static_cast<ftl::intmax_t>(value);
  else if (length == length_type::z || length == length_type::t)
    *va_arg(args, decltype((char *)0 - (char *)0) *) =
        static_cast<decltype((char *)0 - (char *)0)>(value);
  else
    *va_arg(args, int *) = static_cast<int>(value);
}

void store_unsigned(va_list_type &args, length_type length,
                    unsigned long long value) {
  if (length == length_type::hh)
    *va_arg(args, unsigned char *) = static_cast<unsigned char>(value);
  else if (length == length_type::h)
    *va_arg(args, unsigned short *) = static_cast<unsigned short>(value);
  else if (length == length_type::l)
    *va_arg(args, unsigned long *) = static_cast<unsigned long>(value);
  else if (length == length_type::ll)
    *va_arg(args, unsigned long long *) = value;
  else if (length == length_type::j)
    *va_arg(args, ftl::uintmax_t *) = static_cast<ftl::uintmax_t>(value);
  else if (length == length_type::z)
    *va_arg(args, size_type *) = static_cast<size_type>(value);
  else if (length == length_type::t)
    *va_arg(args, ftl::make_unsigned_t<decltype((char *)0 - (char *)0)> *) =
        static_cast<ftl::make_unsigned_t<decltype((char *)0 - (char *)0)>>(
            value);
  else
    *va_arg(args, unsigned int *) = static_cast<unsigned int>(value);
}

int digit_value(int value) {
  if (value >= '0' && value <= '9')
    return value - '0';
  if (value >= 'a' && value <= 'z')
    return value - 'a' + 10;
  if (value >= 'A' && value <= 'Z')
    return value - 'A' + 10;
  return -1;
}

bool nan_sequence_character(int value) {
  return digit(value) || (value >= 'a' && value <= 'z') ||
         (value >= 'A' && value <= 'Z') || value == '_';
}

bool scan_integer(input_source &source, conversion item, va_list_type &args,
                  bool suppress) {
  int remaining = item.width > 0 ? item.width : 0x7fffffff;
  auto next = [&] {
    if (remaining == 0)
      return EOF;
    --remaining;
    return source.get();
  };
  int value = next();
  bool negative = false;
  if (value == '+' || value == '-') {
    negative = value == '-';
    value = next();
  }
  unsigned base = item.type == 'o' ? 8
                  : item.type == 'x' || item.type == 'X' || item.type == 'p'
                      ? 16
                      : 10;
  if (item.type == 'i' && value == '0') {
    base = 8;
    if (remaining != 0) {
      int following = next();
      if (following == 'x' || following == 'X') {
        base = 16;
        value = next();
      } else {
        source.unget(following);
        ++remaining;
      }
    }
  } else if (base == 16 && value == '0' && remaining != 0) {
    int following = next();
    if (following == 'x' || following == 'X') {
      value = next();
    } else {
      source.unget(following);
      ++remaining;
    }
  }
  unsigned long long result = 0;
  int digits = 0;
  while (value != EOF) {
    int converted = digit_value(value);
    if (converted < 0 || static_cast<unsigned>(converted) >= base)
      break;
    result = result * base + static_cast<unsigned>(converted);
    ++digits;
    value = next();
  }
  source.unget(value);
  if (!digits)
    return false;
  if (!suppress) {
    if (item.type == 'd' || item.type == 'i')
      store_signed(args, item.length,
                   negative ? -static_cast<long long>(result)
                            : static_cast<long long>(result));
    else if (item.type == 'p')
      *va_arg(args, void **) =
          reinterpret_cast<void *>(static_cast<ftl::uintptr_t>(result));
    else
      store_unsigned(args, item.length, negative ? 0ULL - result : result);
  }
  return true;
}

bool scan_float(input_source &source, conversion item, va_list_type &args,
                bool suppress) {
  char buffer[512];
  int limit =
      item.width > 0 && item.width < static_cast<int>(sizeof(buffer) - 1)
          ? item.width
          : static_cast<int>(sizeof(buffer) - 1);
  int count = 0;
  bool hexadecimal_input = false;
  bool point_seen = false;
  bool exponent_seen = false;
  const lconv *numeric = ::localeconv();
  const int decimal =
      numeric != nullptr && numeric->decimal_point != nullptr &&
              numeric->decimal_point[0] != '\0'
          ? static_cast<unsigned char>(numeric->decimal_point[0])
          : '.';
  while (count != limit) {
    int value = source.get();
    int start = count && (buffer[0] == '+' || buffer[0] == '-') ? 1 : 0;
    bool infinity =
        count > start && (buffer[start] == 'i' || buffer[start] == 'I');
    bool not_a_number =
        count > start && (buffer[start] == 'n' || buffer[start] == 'N');
    bool valid = false;
    if (count == 0 && (value == '+' || value == '-')) {
      valid = true;
    } else if (count == start &&
               (value == 'i' || value == 'I' || value == 'n' || value == 'N')) {
      valid = true;
    } else if (infinity) {
      static constexpr char spelling[] = "infinity";
      int index = count - start;
      valid = index < 8 && (value == spelling[index] ||
                            value == spelling[index] - 'a' + 'A');
    } else if (not_a_number && count < start + 3) {
      static constexpr char spelling[] = "nan";
      int index = count - start;
      valid = index < 3 && (value == spelling[index] ||
                            value == spelling[index] - 'a' + 'A');
    } else if (not_a_number && count == start + 3) {
      valid = value == '(';
    } else if (not_a_number && count > start + 3 && buffer[start + 3] == '(' &&
               buffer[count - 1] != ')') {
      valid = nan_sequence_character(value) || value == ')';
    } else if (digit(value) || (hexadecimal_input && digit_value(value) >= 10 &&
                                digit_value(value) < 16)) {
      valid = true;
    } else if (!point_seen && !exponent_seen && value == decimal) {
      point_seen = true;
      valid = true;
    } else if (!hexadecimal_input && !exponent_seen &&
               (value == 'e' || value == 'E')) {
      exponent_seen = true;
      valid = true;
    } else if (hexadecimal_input && !exponent_seen &&
               (value == 'p' || value == 'P')) {
      exponent_seen = true;
      valid = true;
    } else if (exponent_seen && count != 0 &&
               (buffer[count - 1] == 'e' || buffer[count - 1] == 'E' ||
                buffer[count - 1] == 'p' || buffer[count - 1] == 'P') &&
               (value == '+' || value == '-')) {
      valid = true;
    } else if (!hexadecimal_input && !point_seen && !exponent_seen &&
               count == start + 1 && buffer[start] == '0' &&
               (value == 'x' || value == 'X')) {
      hexadecimal_input = true;
      valid = true;
    }
    if (!valid) {
      source.unget(value);
      break;
    }
    buffer[count++] = static_cast<char>(value == decimal ? '.' : value);
  }
  if (!count)
    return false;
  long double result{};
  int prefix = (buffer[0] == '+' || buffer[0] == '-') ? 1 : 0;
  bool hexadecimal = count >= prefix + 2 && buffer[prefix] == '0' &&
                     (buffer[prefix + 1] == 'x' || buffer[prefix + 1] == 'X');
  if (hexadecimal) {
    char adjusted[512];
    int adjusted_count = 0;
    if (prefix && buffer[0] == '-')
      adjusted[adjusted_count++] = buffer[0];
    for (int index = prefix + 2; index != count; ++index)
      adjusted[adjusted_count++] = buffer[index];
    auto parsed = ftl::from_chars(adjusted, adjusted + adjusted_count, result,
                                  ftl::chars_format::hex);
    if (parsed.ptr != adjusted + adjusted_count)
      return false;
  } else {
    const char *first = buffer[0] == '+' ? buffer + 1 : buffer;
    auto parsed = ftl::from_chars(first, buffer + count, result,
                                  ftl::chars_format::general);
    if (parsed.ptr != buffer + count)
      return false;
  }
  if (!suppress) {
    if (item.length == length_type::big_l)
      *va_arg(args, long double *) = result;
    else if (item.length == length_type::l)
      *va_arg(args, double *) = static_cast<double>(result);
    else
      *va_arg(args, float *) = static_cast<float>(result);
  }
  return true;
}

template <class FormatCharacter>
int scan_from(input_source &source, const FormatCharacter *format,
              va_list_type &args) {
  int assignments = 0;
  while (*format) {
    if (space_character(*format)) {
      while (space_character(*format))
        ++format;
      skip_space(source);
      continue;
    }
    if (*format != '%') {
      int value = source.get();
      if (value != static_cast<unsigned char>(*format++)) {
        source.unget(value);
        return assignments ? assignments : (source.last_was_eof ? EOF : 0);
      }
      continue;
    }
    ++format;
    if (*format == '%') {
      int value = source.get();
      if (value != '%') {
        source.unget(value);
        return assignments ? assignments : (source.last_was_eof ? EOF : 0);
      }
      ++format;
      continue;
    }
    bool suppress = *format == '*';
    if (suppress)
      ++format;
    conversion item;
    while (digit(*format))
      item.width = item.width * 10 + (*format++ - '0');
    if (*format == 'h')
      item.length =
          *++format == 'h' ? (++format, length_type::hh) : length_type::h;
    else if (*format == 'l')
      item.length =
          *++format == 'l' ? (++format, length_type::ll) : length_type::l;
    else if (*format == 'j')
      item.length = (++format, length_type::j);
    else if (*format == 'z')
      item.length = (++format, length_type::z);
    else if (*format == 't')
      item.length = (++format, length_type::t);
    else if (*format == 'L')
      item.length = (++format, length_type::big_l);
    item.type = *format++;
    if (item.type != 'c' && item.type != '[' && item.type != 'n')
      skip_space(source);
    bool matched = true;
    if (item.type == 'd' || item.type == 'i' || item.type == 'o' ||
        item.type == 'u' || item.type == 'x' || item.type == 'X' ||
        item.type == 'p') {
      matched = scan_integer(source, item, args, suppress);
    } else if (item.type == 'f' || item.type == 'F' || item.type == 'e' ||
               item.type == 'E' || item.type == 'g' || item.type == 'G' ||
               item.type == 'a' || item.type == 'A') {
      matched = scan_float(source, item, args, suppress);
    } else if (item.type == 's') {
      char *destination = nullptr;
      wchar_t *wide_destination = nullptr;
      if (!suppress) {
        if (item.length == length_type::l)
          wide_destination = va_arg(args, wchar_t *);
        else
          destination = va_arg(args, char *);
      }
      int limit = item.width > 0 ? item.width : 0x7fffffff;
      int count = 0, value;
      while (count != limit && (value = source.get()) != EOF &&
             !space_character(value)) {
        if (!suppress) {
          if (wide_destination)
            wide_destination[count] = static_cast<wchar_t>(value);
          else
            destination[count] = static_cast<char>(value);
        }
        ++count;
      }
      if (count != limit)
        source.unget(value);
      if (!suppress) {
        if (wide_destination)
          wide_destination[count] = L'\0';
        else
          destination[count] = 0;
      }
      matched = count != 0;
    } else if (item.type == 'c') {
      char *destination = nullptr;
      wchar_t *wide_destination = nullptr;
      if (!suppress) {
        if (item.length == length_type::l)
          wide_destination = va_arg(args, wchar_t *);
        else
          destination = va_arg(args, char *);
      }
      int count = item.width > 0 ? item.width : 1;
      for (int index = 0; index != count; ++index) {
        int value = source.get();
        if (value == EOF) {
          matched = false;
          break;
        }
        if (!suppress) {
          if (wide_destination)
            wide_destination[index] = static_cast<wchar_t>(value);
          else
            destination[index] = static_cast<char>(value);
        }
      }
    } else if (item.type == '[') {
      bool accepted[256]{};
      bool invert = *format == '^';
      if (invert)
        ++format;
      int previous = -1;
      if (*format == ']') {
        accepted[static_cast<unsigned char>(*format++)] = true;
        previous = ']';
      }
      while (*format && *format != ']') {
        unsigned char current = static_cast<unsigned char>(*format++);
        if (current == '-' && previous >= 0 && *format && *format != ']') {
          unsigned char last = static_cast<unsigned char>(*format++);
          if (previous <= last)
            for (int member = previous; member <= last; ++member)
              accepted[member] = true;
          else
            accepted[current] = true;
          previous = last;
          accepted[last] = true;
        } else {
          accepted[current] = true;
          previous = current;
        }
      }
      if (*format == ']')
        ++format;
      else
        return assignments;
      char *destination = nullptr;
      wchar_t *wide_destination = nullptr;
      if (!suppress) {
        if (item.length == length_type::l)
          wide_destination = va_arg(args, wchar_t *);
        else
          destination = va_arg(args, char *);
      }
      int limit = item.width > 0 ? item.width : 0x7fffffff;
      int count = 0;
      while (count != limit) {
        int value = source.get();
        if (value == EOF ||
            (accepted[static_cast<unsigned char>(value)] == invert)) {
          source.unget(value);
          break;
        }
        if (!suppress) {
          if (wide_destination)
            wide_destination[count] = static_cast<wchar_t>(value);
          else
            destination[count] = static_cast<char>(value);
        }
        ++count;
      }
      if (!suppress) {
        if (wide_destination)
          wide_destination[count] = L'\0';
        else
          destination[count] = 0;
      }
      matched = count != 0;
    } else if (item.type == 'n') {
      if (!suppress)
        store_signed(args, item.length,
                     static_cast<long long>(source.consumed));
      continue;
    } else {
      return assignments;
    }
    if (!matched)
      return assignments ? assignments : (source.last_was_eof ? EOF : 0);
    if (!suppress)
      ++assignments;
  }
  return assignments;
}
} // namespace

namespace ftl_cstdio_runtime {
int vfprintf(FILE *stream, const char *format, ftl::va_list args) {
  output_sink sink{stream};
  sink.locked = true;
  ftl::va_list arguments;
  va_copy(arguments, args);
  ::ftl_stdio_runtime::lock_file(stream);
  int result = format_to(sink, format, arguments);
  ::ftl_stdio_runtime::unlock_file(stream);
  va_end(arguments);
  return result;
}
int fprintf(FILE *stream, const char *format, ...) {
  ftl::va_list args;
  va_start(args, format);
  int result = vfprintf(stream, format, args);
  va_end(args);
  return result;
}
int vprintf(const char *format, ftl::va_list args) {
  return vfprintf(::ftl_stdio_runtime::output_stream(), format, args);
}
int printf(const char *format, ...) {
  ftl::va_list args;
  va_start(args, format);
  int result = vprintf(format, args);
  va_end(args);
  return result;
}
int vsnprintf(char *buffer, size_t size, const char *format,
              ftl::va_list args) {
  if (!buffer && size != 0)
    return -1;
  output_sink sink{nullptr, buffer, size};
  ftl::va_list arguments;
  va_copy(arguments, args);
  int result = format_to(sink, format, arguments);
  va_end(arguments);
  return result;
}
int snprintf(char *buffer, size_t size, const char *format, ...) {
  ftl::va_list args;
  va_start(args, format);
  int result = vsnprintf(buffer, size, format, args);
  va_end(args);
  return result;
}
int vsprintf(char *buffer, const char *format, ftl::va_list args) {
  output_sink sink{nullptr, buffer, static_cast<size_t>(-1)};
  ftl::va_list arguments;
  va_copy(arguments, args);
  int result = format_to(sink, format, arguments);
  va_end(arguments);
  return result;
}
int sprintf(char *buffer, const char *format, ...) {
  ftl::va_list args;
  va_start(args, format);
  int result = vsprintf(buffer, format, args);
  va_end(args);
  return result;
}
int vfscanf(FILE *stream, const char *format, ftl::va_list args) {
  input_source source{stream};
  source.locked = true;
  ftl::va_list arguments;
  va_copy(arguments, args);
  ::ftl_stdio_runtime::lock_file(stream);
  int result = scan_from(source, format, arguments);
  ::ftl_stdio_runtime::unlock_file(stream);
  va_end(arguments);
  return result;
}
int fscanf(FILE *stream, const char *format, ...) {
  ftl::va_list args;
  va_start(args, format);
  int result = vfscanf(stream, format, args);
  va_end(args);
  return result;
}
int vscanf(const char *format, ftl::va_list args) {
  return vfscanf(::ftl_stdio_runtime::input_stream(), format, args);
}
int scanf(const char *format, ...) {
  ftl::va_list args;
  va_start(args, format);
  int result = vscanf(format, args);
  va_end(args);
  return result;
}
int vsscanf(const char *buffer, const char *format, ftl::va_list args) {
  input_source source{nullptr, buffer};
  ftl::va_list arguments;
  va_copy(arguments, args);
  int result = scan_from(source, format, arguments);
  va_end(arguments);
  return result;
}
int sscanf(const char *buffer, const char *format, ...) {
  ftl::va_list args;
  va_start(args, format);
  int result = vsscanf(buffer, format, args);
  va_end(args);
  return result;
}
} // namespace ftl_cstdio_runtime

namespace ftl_wstdio_runtime {
int vfwscanf(file_type *stream, const wchar_t *format, ftl::va_list args) {
  if (::ftl_stdio_runtime::orient(stream, 1) < 0)
    return EOF;
  input_source source;
  source.file = stream;
  source.wide_file = true;
  source.locked = true;
  ftl::va_list arguments;
  va_copy(arguments, args);
  ::ftl_stdio_runtime::lock_file(stream);
  int result = scan_from(source, format, arguments);
  ::ftl_stdio_runtime::unlock_file(stream);
  va_end(arguments);
  return result;
}

int vswscanf(const wchar_t *buffer, const wchar_t *format, ftl::va_list args) {
  input_source source;
  source.wide_text = buffer;
  ftl::va_list arguments;
  va_copy(arguments, args);
  int result = scan_from(source, format, arguments);
  va_end(arguments);
  return result;
}
} // namespace ftl_wstdio_runtime
