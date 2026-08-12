// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_CHRONO_IO_HPP
#define FTL_DETAIL_CHRONO_IO_HPP

#ifdef FTL_REPLACE_STL
#include <charconv>
#include <istream>
#include <locale>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#else
#include <ftl/charconv>
#include <ftl/istream>
#include <ftl/locale>
#include <ftl/ostream>
#include <ftl/sstream>
#include <ftl/string>
#include <ftl/type_traits>
#endif

FTL_BEGIN_NAMESPACE

namespace chrono {

namespace chrono_detail {

/*
 * --------------------------------------------------------------------------
 * Stream rendering
 * --------------------------------------------------------------------------
 */

template <class Character, class Traits>
void io_ascii(basic_ostream<Character, Traits> &stream, const char *text) {
  while (*text != '\0') {
    stream.put(static_cast<Character>(static_cast<unsigned char>(*text++)));
  }
}

template <class Character, class Traits>
void io_integer(basic_ostream<Character, Traits> &stream, long long value,
                unsigned minimum_digits = 0, char padding = '0') {
  char buffer[64];

  auto converted = to_chars(buffer, buffer + sizeof(buffer), value);

  if (converted.ec != errc{}) {
    stream.setstate(ios_base::failbit);

    return;
  }

  const char *first = buffer;
  const char *last = converted.ptr;

  bool negative = false;

  if (first != last && *first == '-') {
    negative = true;
    ++first;
  }

  const unsigned digits = static_cast<unsigned>(last - first);

  if (negative) {
    stream.put(static_cast<Character>('-'));
  }

  for (unsigned index = digits; index < minimum_digits; ++index) {
    stream.put(static_cast<Character>(padding));
  }

  while (first != last) {
    stream.put(static_cast<Character>(static_cast<unsigned char>(*first++)));
  }
}

template <class Character, class Traits>
void io_unsigned(basic_ostream<Character, Traits> &stream,
                 unsigned long long value, unsigned minimum_digits = 0,
                 char padding = '0') {
  char buffer[64];

  auto converted = to_chars(buffer, buffer + sizeof(buffer), value);

  if (converted.ec != errc{}) {
    stream.setstate(ios_base::failbit);

    return;
  }

  const unsigned digits = static_cast<unsigned>(converted.ptr - buffer);

  for (unsigned index = digits; index < minimum_digits; ++index) {
    stream.put(static_cast<Character>(padding));
  }

  for (const char *current = buffer; current != converted.ptr; ++current) {
    stream.put(static_cast<Character>(static_cast<unsigned char>(*current)));
  }
}

template <class Character, class Traits, class Function>
basic_ostream<Character, Traits> &
io_buffered(basic_ostream<Character, Traits> &stream, Function &&function) {
  basic_ostringstream<Character, Traits, allocator<Character>> buffer;

  buffer.flags(stream.flags());

  buffer.imbue(stream.getloc());

  buffer.precision(stream.precision());

  buffer.fill(stream.fill());

  function(buffer);

  return stream << buffer.str();
}

template <class Character>
basic_string<Character> io_locale_token(const locale &where, const tm &value,
                                        char conversion) {
  using traits_type = char_traits<Character>;

  using stream_type =
      basic_ostringstream<Character, traits_type, allocator<Character>>;

  using iterator = ostreambuf_iterator<Character, traits_type>;

  stream_type stream;

  stream.imbue(where);

  iterator output(stream);

  const auto &facet = use_facet<time_put<Character, iterator>>(where);

  output = facet.put(output, stream, stream.fill(), &value, conversion);

  if (output.failed()) {
    return {};
  }

  return stream.str();
}

template <class Character, class Traits>
void io_localized_month(basic_ostream<Character, Traits> &stream,
                        unsigned month_value) {
  tm value{};

  value.tm_mon = static_cast<int>(month_value) - 1;

  const auto token = io_locale_token<Character>(stream.getloc(), value, 'b');

  if (token.empty()) {
    stream.setstate(ios_base::failbit);

    return;
  }

  stream.write(token.data(), static_cast<streamsize>(token.size()));
}

template <class Character, class Traits>
void io_localized_weekday(basic_ostream<Character, Traits> &stream,
                          unsigned weekday_value) {
  tm value{};

  value.tm_wday = static_cast<int>(weekday_value);

  const auto token = io_locale_token<Character>(stream.getloc(), value, 'a');

  if (token.empty()) {
    stream.setstate(ios_base::failbit);

    return;
  }

  stream.write(token.data(), static_cast<streamsize>(token.size()));
}

template <class Character, class Traits>
void io_year(basic_ostream<Character, Traits> &stream, int value) {
  io_integer(stream, value, 4);
}

template <class Character, class Traits>
void io_ymd(basic_ostream<Character, Traits> &stream,
            const year_month_day &value) {
  io_year(stream, static_cast<int>(value.year()));

  stream.put(static_cast<Character>('-'));

  io_unsigned(stream, static_cast<unsigned>(value.month()), 2);

  stream.put(static_cast<Character>('-'));

  io_unsigned(stream, static_cast<unsigned>(value.day()), 2);
}

template <class Character, class Traits, class Split>
void io_hms(basic_ostream<Character, Traits> &stream, const Split &value,
            bool negative = false, bool force_leap_second = false) {
  if (negative) {
    stream.put(static_cast<Character>('-'));
  }

  io_integer(stream, static_cast<long long>(value.hours().count()), 2);

  stream.put(static_cast<Character>(':'));

  io_integer(stream, static_cast<long long>(value.minutes().count()), 2);

  stream.put(static_cast<Character>(':'));

  io_integer(
      stream,
      force_leap_second ? 60 : static_cast<long long>(value.seconds().count()),
      2);

  if constexpr (Split::fractional_width != 0) {
    stream.put(use_facet<numpunct<Character>>(stream.getloc()).decimal_point());

    io_integer(stream, static_cast<long long>(value.subseconds().count()),
               Split::fractional_width);
  }
}

template <class Character, class Traits, class Duration>
void io_sys_time(basic_ostream<Character, Traits> &stream,
                 const sys_time<Duration> &value) {
  using precision = common_type_t<Duration, seconds>;

  const sys_time<precision> precise{
      precision{
          value.time_since_epoch(),
      },
  };

  const auto day_point = floor<days>(precise);

  const sys_days date{
      day_point.time_since_epoch(),
  };

  const year_month_day ymd{date};

  io_ymd(stream, ymd);

  stream.put(static_cast<Character>(' '));

  const hh_mm_ss<precision> split{
      precision{
          precise - day_point,
      },
  };

  io_hms(stream, split);
}

template <class Character, class Traits, class Duration>
void io_utc_time(basic_ostream<Character, Traits> &stream,
                 const utc_time<Duration> &value) {
  using precision = common_type_t<Duration, seconds>;

  const utc_time<precision> precise{
      precision{
          value.time_since_epoch(),
      },
  };

  const auto leap = get_leap_second_info(precise);

  if (!leap.is_leap_second) {
    const auto system = utc_clock::to_sys(precise);

    io_sys_time(stream, system);

    return;
  }

  /*
   * utc_clock::to_sys collapses a point inside a
   * positive leap second onto the final representable
   * system-clock tick before the insertion.
   *
   * Adding one precision tick therefore recovers the
   * insertion boundary.
   */
  const auto collapsed = utc_clock::to_sys(precise);

  const auto boundary = collapsed + precision{1};

  const auto civil = boundary - seconds{1};

  const auto day_point = floor<days>(civil);

  const year_month_day ymd{
      sys_days{
          day_point.time_since_epoch(),
      },
  };

  io_ymd(stream, ymd);

  stream.put(static_cast<Character>(' '));

  const hh_mm_ss<precision> base{
      precision{
          civil - day_point,
      },
  };

  const auto leap_end = utc_clock::from_sys(boundary);

  const auto leap_begin = leap_end - seconds{1};

  const precision into_leap{
      precise - leap_begin,
  };

  if constexpr (hh_mm_ss<precision>::fractional_width == 0) {
    io_integer(stream, static_cast<long long>(base.hours().count()), 2);

    stream.put(static_cast<Character>(':'));

    io_integer(stream, static_cast<long long>(base.minutes().count()), 2);

    stream.put(static_cast<Character>(':'));

    io_integer(stream, 60, 2);
  } else {
    io_integer(stream, static_cast<long long>(base.hours().count()), 2);

    stream.put(static_cast<Character>(':'));

    io_integer(stream, static_cast<long long>(base.minutes().count()), 2);

    stream.put(static_cast<Character>(':'));

    io_integer(stream, 60, 2);

    stream.put(use_facet<numpunct<Character>>(stream.getloc()).decimal_point());

    const hh_mm_ss<precision> fractional{
        into_leap,
    };

    io_integer(stream, static_cast<long long>(fractional.subseconds().count()),
               hh_mm_ss<precision>::fractional_width);
  }
}

template <class Character, class Traits, class Duration>
void io_tai_time(basic_ostream<Character, Traits> &stream,
                 const tai_time<Duration> &value) {
  using precision = common_type_t<Duration, seconds>;

  const auto epoch_difference =
      sys_days{year{1970} / January / 1} - sys_days{year{1958} / January / 1};

  const sys_time<precision> civil{
      precision{
          value.time_since_epoch(),
      } - duration_cast<precision>(epoch_difference),
  };

  io_sys_time(stream, civil);
}

template <class Character, class Traits, class Duration>
void io_gps_time(basic_ostream<Character, Traits> &stream,
                 const gps_time<Duration> &value) {
  using precision = common_type_t<Duration, seconds>;

  const auto epoch_difference = sys_days{year{1980} / January / Sunday[1]} -
                                sys_days{year{1970} / January / 1};

  const sys_time<precision> civil{
      precision{
          value.time_since_epoch(),
      } + duration_cast<precision>(epoch_difference),
  };

  io_sys_time(stream, civil);
}

template <class Character, class Traits, class Duration>
void io_file_time(basic_ostream<Character, Traits> &stream,
                  const file_time<Duration> &value) {
  const auto system = file_clock::to_sys(value);

  io_sys_time(stream, system);
}

template <class Character, class Traits, class Period>
void io_duration_suffix(basic_ostream<Character, Traits> &stream) {
  using P = typename Period::type;

  if constexpr (is_same_v<P, atto>) {
    io_ascii(stream, "as");
  } else if constexpr (is_same_v<P, femto>) {
    io_ascii(stream, "fs");
  } else if constexpr (is_same_v<P, pico>) {
    io_ascii(stream, "ps");
  } else if constexpr (is_same_v<P, nano>) {
    io_ascii(stream, "ns");
  } else if constexpr (is_same_v<P, micro>) {
    io_ascii(stream, "us");
  } else if constexpr (is_same_v<P, milli>) {
    io_ascii(stream, "ms");
  } else if constexpr (is_same_v<P, centi>) {
    io_ascii(stream, "cs");
  } else if constexpr (is_same_v<P, deci>) {
    io_ascii(stream, "ds");
  } else if constexpr (is_same_v<P, ratio<1>>) {
    io_ascii(stream, "s");
  } else if constexpr (is_same_v<P, deca>) {
    io_ascii(stream, "das");
  } else if constexpr (is_same_v<P, hecto>) {
    io_ascii(stream, "hs");
  } else if constexpr (is_same_v<P, kilo>) {
    io_ascii(stream, "ks");
  } else if constexpr (is_same_v<P, mega>) {
    io_ascii(stream, "Ms");
  } else if constexpr (is_same_v<P, giga>) {
    io_ascii(stream, "Gs");
  } else if constexpr (is_same_v<P, tera>) {
    io_ascii(stream, "Ts");
  } else if constexpr (is_same_v<P, peta>) {
    io_ascii(stream, "Ps");
  } else if constexpr (is_same_v<P, exa>) {
    io_ascii(stream, "Es");
  } else if constexpr (is_same_v<P, ratio<60>>) {
    io_ascii(stream, "min");
  } else if constexpr (is_same_v<P, ratio<3600>>) {
    io_ascii(stream, "h");
  } else if constexpr (is_same_v<P, ratio<86400>>) {
    io_ascii(stream, "d");
  } else {
    stream.put(static_cast<Character>('['));

    io_integer(stream, P::num);

    if constexpr (P::den != 1) {
      stream.put(static_cast<Character>('/'));

      io_integer(stream, P::den);
    }

    stream.put(static_cast<Character>(']'));

    stream.put(static_cast<Character>('s'));
  }
}

/*
 * --------------------------------------------------------------------------
 * Parsing state
 * --------------------------------------------------------------------------
 */

struct io_parse_fields {
  bool has_year = false;
  bool has_century = false;
  bool has_year2 = false;

  bool has_iso_year = false;
  bool has_iso_year2 = false;

  bool has_month = false;
  bool has_day = false;
  bool has_yday = false;

  bool has_weekday = false;

  bool has_week_sunday = false;
  bool has_week_monday = false;
  bool has_iso_week = false;

  bool has_hour24 = false;
  bool has_hour12 = false;
  bool has_minute = false;
  bool has_second = false;
  bool has_ampm = false;

  bool has_abbrev = false;
  bool has_offset = false;

  int year = 0;
  int century = 0;
  int year2 = 0;

  int iso_year = 0;
  int iso_year2 = 0;

  int month = 0;
  int day = 0;
  int yday = 0;

  int weekday = 0;

  int week_sunday = 0;
  int week_monday = 0;
  int iso_week = 0;

  int hour24 = 0;
  int hour12 = 0;
  int minute = 0;

  long double second = 0;

  bool pm = false;

  int offset_minutes = 0;

  string abbrev;
};

template <class Value>
bool io_set_field(bool &present, Value &destination, Value value) {
  if (present) {
    return destination == value;
  }

  present = true;
  destination = value;

  return true;
}

template <class Character> bool io_ascii_digit(Character value) {
  return value >= static_cast<Character>('0') &&
         value <= static_cast<Character>('9');
}

template <class Character> bool io_ascii_alpha(Character value) {
  return (value >= static_cast<Character>('A') &&
          value <= static_cast<Character>('Z')) ||
         (value >= static_cast<Character>('a') &&
          value <= static_cast<Character>('z'));
}

template <class Character> Character io_ascii_lower(Character value) {
  if (value >= static_cast<Character>('A') &&
      value <= static_cast<Character>('Z')) {
    return static_cast<Character>(value - static_cast<Character>('A') +
                                  static_cast<Character>('a'));
  }

  return value;
}

template <class Character, class Traits> class io_parser {
public:
  using stream_type = basic_istream<Character, Traits>;

  using int_type = typename Traits::int_type;

  explicit io_parser(stream_type &stream) : stream_(stream) {}

  bool parse(const Character *format, io_parse_fields &fields) {
    typename stream_type::sentry guard(stream_, true);

    if (!guard) {
      return false;
    }

    while (*format != Character{}) {
      if (space(*format)) {
        do {
          ++format;
        } while (*format != Character{} && space(*format));

        skip_space();
        continue;
      }

      if (*format != static_cast<Character>('%')) {
        if (!match(*format++))
          return false;

        continue;
      }

      ++format;

      if (*format == Character{}) {
        return false;
      }

      int width = 0;

      while (io_ascii_digit(*format)) {
        width = width * 10 +
                static_cast<int>(*format - static_cast<Character>('0'));

        ++format;
      }

      char modifier = '\0';

      if (*format == static_cast<Character>('E') ||
          *format == static_cast<Character>('O')) {
        modifier = static_cast<char>(*format);

        ++format;
      }

      if (*format == Character{}) {
        return false;
      }

      const char conversion = static_cast<char>(*format++);

      if (!conversion_parse(conversion, modifier, width, fields)) {
        return false;
      }
    }

    return true;
  }

  bool saw_eof() const noexcept { return saw_eof_; }

private:
  bool peek(Character &result) {
    const int_type value = stream_.rdbuf()->sgetc();

    if (Traits::eq_int_type(value, Traits::eof())) {
      saw_eof_ = true;
      return false;
    }

    result = Traits::to_char_type(value);

    return true;
  }

  bool take(Character &result) {
    const int_type value = stream_.rdbuf()->sbumpc();

    if (Traits::eq_int_type(value, Traits::eof())) {
      saw_eof_ = true;
      return false;
    }

    result = Traits::to_char_type(value);

    return true;
  }

  bool match(Character expected) {
    Character actual{};

    if (!peek(actual))
      return false;

    if (!Traits::eq(actual, expected)) {
      return false;
    }

    return take(actual);
  }

  bool space(Character value) const {
    return use_facet<ctype<Character>>(stream_.getloc())
        .is(ctype_base::space, value);
  }

  void skip_space() {
    Character value{};

    while (peek(value) && space(value)) {
      take(value);
    }
  }

  bool one_space() {
    Character value{};

    if (!peek(value) || !space(value)) {
      return false;
    }

    take(value);

    return true;
  }

  void optional_space() {
    Character value{};

    if (peek(value) && space(value)) {
      take(value);
    }
  }

  bool number(long long &result, int maximum, bool allow_sign,
              int minimum = 1) {
    result = 0;

    bool negative = false;

    Character value{};

    int consumed = 0;

    if (allow_sign && peek(value) &&
        (value == static_cast<Character>('-') ||
         value == static_cast<Character>('+'))) {
      negative = value == static_cast<Character>('-');

      take(value);
    }

    while (consumed < maximum && peek(value) && io_ascii_digit(value)) {
      take(value);

      result = result * 10 +
               static_cast<long long>(value - static_cast<Character>('0'));

      ++consumed;
    }

    if (consumed < minimum)
      return false;

    if (negative)
      result = -result;

    return true;
  }

  bool second_number(long double &result, int width) {
    Character value{};

    int integer_digits = 0;
    int total = 0;

    long long integral = 0;

    const int integer_limit = width > 0 && width < 2 ? width : 2;

    while (integer_digits < integer_limit && peek(value) &&
           io_ascii_digit(value)) {
      take(value);

      integral = integral * 10 +
                 static_cast<long long>(value - static_cast<Character>('0'));

      ++integer_digits;
      ++total;
    }

    if (integer_digits == 0)
      return false;

    result = static_cast<long double>(integral);

    if (width > 0 && total >= width) {
      return true;
    }

    if (!peek(value))
      return true;

    const Character decimal =
        use_facet<numpunct<Character>>(stream_.getloc()).decimal_point();

    if (!Traits::eq(value, decimal) &&
        !Traits::eq(value, static_cast<Character>('.'))) {
      return true;
    }

    take(value);
    ++total;

    long double scale = 0.1L;

    bool any = false;

    while ((width <= 0 || total < width) && peek(value) &&
           io_ascii_digit(value)) {
      take(value);

      result +=
          static_cast<long double>(value - static_cast<Character>('0')) * scale;

      scale *= 0.1L;

      ++total;
      any = true;
    }

    return any;
  }

  bool read_word(basic_string<Character> &result) {
    result.clear();

    Character value{};

    const auto &classification = use_facet<ctype<Character>>(stream_.getloc());

    while (peek(value) && classification.is(ctype_base::alpha, value)) {
      take(value);
      result.push_back(value);
    }

    return !result.empty();
  }

  bool equal_case_insensitive(basic_string_view<Character> left,
                              basic_string_view<Character> right) const {
    if (left.size() != right.size()) {
      return false;
    }

    const auto &classification = use_facet<ctype<Character>>(stream_.getloc());

    for (size_t index = 0; index < left.size(); ++index) {
      if (!Traits::eq(classification.tolower(left[index]),
                      classification.tolower(right[index]))) {
        return false;
      }
    }

    return true;
  }

  bool month_name(int &result) {
    basic_string<Character> word;

    if (!read_word(word))
      return false;

    for (int month_index = 0; month_index < 12; ++month_index) {
      tm value{};
      value.tm_mon = month_index;

      const auto abbreviated =
          io_locale_token<Character>(stream_.getloc(), value, 'b');

      const auto full =
          io_locale_token<Character>(stream_.getloc(), value, 'B');

      const basic_string_view<Character> parsed{
          word.data(),
          word.size(),
      };

      if (equal_case_insensitive(
              parsed, basic_string_view<Character>{abbreviated.data(),
                                                   abbreviated.size()}) ||
          equal_case_insensitive(
              parsed, basic_string_view<Character>{full.data(), full.size()})) {
        result = month_index + 1;

        return true;
      }
    }

    return false;
  }

  bool weekday_name(int &result) {
    basic_string<Character> word;

    if (!read_word(word))
      return false;

    for (int weekday_index = 0; weekday_index < 7; ++weekday_index) {
      tm value{};
      value.tm_wday = weekday_index;

      const auto abbreviated =
          io_locale_token<Character>(stream_.getloc(), value, 'a');

      const auto full =
          io_locale_token<Character>(stream_.getloc(), value, 'A');

      const basic_string_view<Character> parsed{
          word.data(),
          word.size(),
      };

      if (equal_case_insensitive(
              parsed, basic_string_view<Character>{abbreviated.data(),
                                                   abbreviated.size()}) ||
          equal_case_insensitive(
              parsed, basic_string_view<Character>{full.data(), full.size()})) {
        result = weekday_index;

        return true;
      }
    }

    return false;
  }

  bool am_pm(bool &pm) {
    basic_string<Character> word;

    if (!read_word(word))
      return false;

    tm am_value{};
    am_value.tm_hour = 1;

    tm pm_value{};
    pm_value.tm_hour = 13;

    const auto am = io_locale_token<Character>(stream_.getloc(), am_value, 'p');

    const auto pm_token =
        io_locale_token<Character>(stream_.getloc(), pm_value, 'p');

    const basic_string_view<Character> parsed{
        word.data(),
        word.size(),
    };

    if (equal_case_insensitive(
            parsed, basic_string_view<Character>{am.data(), am.size()})) {
      pm = false;
      return true;
    }

    if (equal_case_insensitive(parsed, basic_string_view<Character>{
                                           pm_token.data(), pm_token.size()})) {
      pm = true;
      return true;
    }

    /*
     * The classic locale must work even if the
     * platform time facet supplies empty AM/PM text.
     */
    const Character classic_am[] = {
        static_cast<Character>('A'),
        static_cast<Character>('M'),
    };

    const Character classic_pm[] = {
        static_cast<Character>('P'),
        static_cast<Character>('M'),
    };

    if (equal_case_insensitive(parsed,
                               basic_string_view<Character>{classic_am, 2})) {
      pm = false;
      return true;
    }

    if (equal_case_insensitive(parsed,
                               basic_string_view<Character>{classic_pm, 2})) {
      pm = true;
      return true;
    }

    return false;
  }

  bool zone_name(string &result) {
    result.clear();

    Character value{};

    while (peek(value)) {
      const bool accepted = io_ascii_alpha(value) || io_ascii_digit(value) ||
                            value == static_cast<Character>('_') ||
                            value == static_cast<Character>('/') ||
                            value == static_cast<Character>('-') ||
                            value == static_cast<Character>('+');

      if (!accepted)
        break;

      take(value);

      result.push_back(static_cast<char>(value));
    }

    return !result.empty();
  }

  bool offset(int &minutes, bool colon_form) {
    Character value{};

    if (!peek(value))
      return false;

    int sign = 1;

    if (value == static_cast<Character>('-')) {
      sign = -1;
      take(value);
    } else if (value == static_cast<Character>('+')) {
      take(value);
    } else {
      return false;
    }

    long long hour = 0;

    if (colon_form) {
      if (!number(hour, 2, false, 1)) {
        return false;
      }
    } else {
      if (!number(hour, 2, false, 2)) {
        return false;
      }
    }

    if (hour > 23)
      return false;

    long long minute = 0;

    if (colon_form) {
      if (peek(value) && value == static_cast<Character>(':')) {
        take(value);

        if (!number(minute, 2, false, 2)) {
          return false;
        }
      }
    } else {
      Character first{};

      if (peek(first) && io_ascii_digit(first)) {
        if (!number(minute, 2, false, 2)) {
          return false;
        }
      }
    }

    if (minute > 59)
      return false;

    minutes = sign * static_cast<int>(hour * 60 + minute);

    return true;
  }

  bool conversion_parse(char conversion, char modifier, int width,
                        io_parse_fields &fields) {
    long long parsed = 0;

    switch (conversion) {
    case 'a':
    case 'A': {
      int value = 0;

      if (!weekday_name(value))
        return false;

      return io_set_field(fields.has_weekday, fields.weekday, value);
    }

    case 'b':
    case 'B':
    case 'h': {
      int value = 0;

      if (!month_name(value))
        return false;

      return io_set_field(fields.has_month, fields.month, value);
    }

    case 'c':
      return conversion_parse('a', '\0', 0, fields) && (skip_space(), true) &&
             conversion_parse('b', '\0', 0, fields) && (skip_space(), true) &&
             conversion_parse('e', '\0', 0, fields) && (skip_space(), true) &&
             conversion_parse('T', '\0', 0, fields) && (skip_space(), true) &&
             conversion_parse('Y', '\0', 0, fields);

    case 'C':
      if (!number(parsed, width > 0 ? width : 2, true)) {
        return false;
      }

      return io_set_field(fields.has_century, fields.century,
                          static_cast<int>(parsed));

    case 'd':
    case 'e':
      if (conversion == 'e') {
        optional_space();
      }

      if (!number(parsed, width > 0 ? width : 2, false)) {
        return false;
      }

      if (parsed < 1 || parsed > 31) {
        return false;
      }

      return io_set_field(fields.has_day, fields.day, static_cast<int>(parsed));

    case 'D':
      return conversion_parse('m', '\0', 0, fields) &&
             match(static_cast<Character>('/')) &&
             conversion_parse('d', '\0', 0, fields) &&
             match(static_cast<Character>('/')) &&
             conversion_parse('y', '\0', 0, fields);

    case 'F':
      return conversion_parse('Y', '\0', width, fields) &&
             match(static_cast<Character>('-')) &&
             conversion_parse('m', '\0', 0, fields) &&
             match(static_cast<Character>('-')) &&
             conversion_parse('d', '\0', 0, fields);

    case 'g':
      if (!number(parsed, width > 0 ? width : 2, false)) {
        return false;
      }

      if (parsed < 0 || parsed > 99) {
        return false;
      }

      return io_set_field(fields.has_iso_year2, fields.iso_year2,
                          static_cast<int>(parsed));

    case 'G':
      if (!number(parsed, width > 0 ? width : 4, true)) {
        return false;
      }

      return io_set_field(fields.has_iso_year, fields.iso_year,
                          static_cast<int>(parsed));

    case 'H':
      if (!number(parsed, width > 0 ? width : 2, false)) {
        return false;
      }

      if (parsed < 0 || parsed > 23) {
        return false;
      }

      return io_set_field(fields.has_hour24, fields.hour24,
                          static_cast<int>(parsed));

    case 'I':
      if (!number(parsed, width > 0 ? width : 2, false)) {
        return false;
      }

      if (parsed < 1 || parsed > 12) {
        return false;
      }

      return io_set_field(fields.has_hour12, fields.hour12,
                          static_cast<int>(parsed));

    case 'j':
      if (!number(parsed, width > 0 ? width : 3, false)) {
        return false;
      }

      if (parsed < 0 || parsed > 999) {
        return false;
      }

      return io_set_field(fields.has_yday, fields.yday,
                          static_cast<int>(parsed));

    case 'm':
      if (!number(parsed, width > 0 ? width : 2, false)) {
        return false;
      }

      if (parsed < 1 || parsed > 12) {
        return false;
      }

      return io_set_field(fields.has_month, fields.month,
                          static_cast<int>(parsed));

    case 'M':
      if (!number(parsed, width > 0 ? width : 2, false)) {
        return false;
      }

      if (parsed < 0 || parsed > 59) {
        return false;
      }

      return io_set_field(fields.has_minute, fields.minute,
                          static_cast<int>(parsed));

    case 'n':
      return one_space();

    case 'p': {
      bool value = false;

      if (!am_pm(value))
        return false;

      return io_set_field(fields.has_ampm, fields.pm, value);
    }

    case 'r':
      return conversion_parse('I', '\0', 0, fields) &&
             match(static_cast<Character>(':')) &&
             conversion_parse('M', '\0', 0, fields) &&
             match(static_cast<Character>(':')) &&
             conversion_parse('S', '\0', 0, fields) && (skip_space(), true) &&
             conversion_parse('p', '\0', 0, fields);

    case 'R':
      return conversion_parse('H', '\0', 0, fields) &&
             match(static_cast<Character>(':')) &&
             conversion_parse('M', '\0', 0, fields);

    case 'S': {
      long double value = 0;

      if (!second_number(value, width)) {
        return false;
      }

      if (value < 0 || value >= 61) {
        return false;
      }

      return io_set_field(fields.has_second, fields.second, value);
    }

    case 't':
      optional_space();
      return true;

    case 'T':
      return conversion_parse('H', '\0', 0, fields) &&
             match(static_cast<Character>(':')) &&
             conversion_parse('M', '\0', 0, fields) &&
             match(static_cast<Character>(':')) &&
             conversion_parse('S', '\0', 0, fields);

    case 'u':
      if (!number(parsed, width > 0 ? width : 1, false)) {
        return false;
      }

      if (parsed < 1 || parsed > 7) {
        return false;
      }

      return io_set_field(fields.has_weekday, fields.weekday,
                          parsed == 7 ? 0 : static_cast<int>(parsed));

    case 'U':
      if (!number(parsed, width > 0 ? width : 2, false)) {
        return false;
      }

      if (parsed < 0 || parsed > 53) {
        return false;
      }

      return io_set_field(fields.has_week_sunday, fields.week_sunday,
                          static_cast<int>(parsed));

    case 'V':
      if (!number(parsed, width > 0 ? width : 2, false)) {
        return false;
      }

      if (parsed < 1 || parsed > 53) {
        return false;
      }

      return io_set_field(fields.has_iso_week, fields.iso_week,
                          static_cast<int>(parsed));

    case 'w':
      if (!number(parsed, width > 0 ? width : 1, false)) {
        return false;
      }

      if (parsed < 0 || parsed > 6) {
        return false;
      }

      return io_set_field(fields.has_weekday, fields.weekday,
                          static_cast<int>(parsed));

    case 'W':
      if (!number(parsed, width > 0 ? width : 2, false)) {
        return false;
      }

      if (parsed < 0 || parsed > 53) {
        return false;
      }

      return io_set_field(fields.has_week_monday, fields.week_monday,
                          static_cast<int>(parsed));

    case 'x':
      return conversion_parse('D', modifier, width, fields);

    case 'X':
      return conversion_parse('T', modifier, width, fields);

    case 'y':
      if (!number(parsed, width > 0 ? width : 2, false)) {
        return false;
      }

      if (parsed < 0 || parsed > 99) {
        return false;
      }

      return io_set_field(fields.has_year2, fields.year2,
                          static_cast<int>(parsed));

    case 'Y':
      if (!number(parsed, width > 0 ? width : 4, true)) {
        return false;
      }

      return io_set_field(fields.has_year, fields.year,
                          static_cast<int>(parsed));

    case 'z': {
      int value = 0;

      if (!offset(value, modifier == 'E' || modifier == 'O')) {
        return false;
      }

      return io_set_field(fields.has_offset, fields.offset_minutes, value);
    }

    case 'Z': {
      string value;

      if (!zone_name(value))
        return false;

      if (fields.has_abbrev && fields.abbrev != value) {
        return false;
      }

      fields.has_abbrev = true;
      fields.abbrev = move(value);

      return true;
    }

    case '%':
      return match(static_cast<Character>('%'));

    default:
      return false;
    }
  }

  stream_type &stream_;
  bool saw_eof_ = false;
};

inline bool io_resolve_year(const io_parse_fields &fields, int &result) {
  bool have = false;
  int value = 0;

  if (fields.has_year) {
    have = true;
    value = fields.year;
  }

  if (fields.has_century || fields.has_year2) {
    int combined = 0;

    if (fields.has_century) {
      combined = fields.century * 100;

      if (fields.has_year2) {
        if (fields.century < 0) {
          combined -= fields.year2;
        } else {
          combined += fields.year2;
        }
      }
    } else {
      combined = fields.year2 >= 69 ? 1900 + fields.year2 : 2000 + fields.year2;
    }

    if (have && value != combined) {
      return false;
    }

    have = true;
    value = combined;
  }

  if (!have)
    return false;

  result = value;
  return true;
}

inline bool io_resolve_iso_year(const io_parse_fields &fields, int &result) {
  if (fields.has_iso_year) {
    result = fields.iso_year;

    if (fields.has_iso_year2) {
      int remainder = result % 100;

      if (remainder < 0)
        remainder = -remainder;

      if (remainder != fields.iso_year2) {
        return false;
      }
    }

    return true;
  }

  if (fields.has_iso_year2) {
    result = fields.iso_year2 >= 69 ? 1900 + fields.iso_year2
                                    : 2000 + fields.iso_year2;

    return true;
  }

  return io_resolve_year(fields, result);
}

inline unsigned io_week_sunday(const sys_days &date) {
  const year_month_day ymd{
      date,
  };

  const int year_value = static_cast<int>(ymd.year());

  const sys_days first{
      year{year_value} / January / 1,
  };

  const unsigned first_weekday = weekday{first}.c_encoding();

  const long long first_sunday =
      static_cast<long long>((7u - first_weekday) % 7u);

  const long long day = (date - first).count();

  if (day < first_sunday) {
    return 0;
  }

  return static_cast<unsigned>(1 + (day - first_sunday) / 7);
}

inline unsigned io_week_monday(const sys_days &date) {
  const year_month_day ymd{
      date,
  };

  const int year_value = static_cast<int>(ymd.year());

  const sys_days first{
      year{year_value} / January / 1,
  };

  const unsigned first_weekday = weekday{first}.iso_encoding() - 1;

  const long long first_monday =
      static_cast<long long>((7u - first_weekday) % 7u);

  const long long day = (date - first).count();

  if (day < first_monday) {
    return 0;
  }

  return static_cast<unsigned>(1 + (day - first_monday) / 7);
}

struct io_iso_fields {
  int year = 0;
  unsigned week = 0;
};

inline io_iso_fields io_iso(const sys_days &date) {
  const unsigned wd = weekday{date}.iso_encoding();

  const auto thursday = date + days{
                                   4 - static_cast<int>(wd),
                               };

  const int iso_year = static_cast<int>(year_month_day{thursday}.year());

  const sys_days january_fourth{
      year{iso_year} / January / 4,
  };

  const unsigned january_fourth_wd = weekday{january_fourth}.iso_encoding();

  const auto first_monday =
      january_fourth - days{
                           static_cast<int>(january_fourth_wd) - 1,
                       };

  return {
      iso_year,
      static_cast<unsigned>(1 + (date - first_monday).count() / 7),
  };
}

inline bool io_check_date_fields(const io_parse_fields &fields,
                                 const sys_days &date) {
  const year_month_day ymd{
      date,
  };

  if (!ymd.ok())
    return false;

  if (fields.has_month &&
      fields.month != static_cast<int>(static_cast<unsigned>(ymd.month()))) {
    return false;
  }

  if (fields.has_day &&
      fields.day != static_cast<int>(static_cast<unsigned>(ymd.day()))) {
    return false;
  }

  int resolved_year = 0;

  if ((fields.has_year || fields.has_century || fields.has_year2) &&
      (!io_resolve_year(fields, resolved_year) ||
       resolved_year != static_cast<int>(ymd.year()))) {
    return false;
  }

  if (fields.has_weekday &&
      fields.weekday != static_cast<int>(weekday{date}.c_encoding())) {
    return false;
  }

  if (fields.has_yday) {
    const sys_days first{
        ymd.year() / January / 1,
    };

    const int yday = static_cast<int>((date - first).count()) + 1;

    if (fields.yday != yday) {
      return false;
    }
  }

  if (fields.has_week_sunday &&
      fields.week_sunday != static_cast<int>(io_week_sunday(date))) {
    return false;
  }

  if (fields.has_week_monday &&
      fields.week_monday != static_cast<int>(io_week_monday(date))) {
    return false;
  }

  if (fields.has_iso_week || fields.has_iso_year || fields.has_iso_year2) {
    const auto iso = io_iso(date);

    int wanted_year = 0;

    if (!io_resolve_iso_year(fields, wanted_year)) {
      return false;
    }

    if (iso.year != wanted_year) {
      return false;
    }

    if (fields.has_iso_week && static_cast<int>(iso.week) != fields.iso_week) {
      return false;
    }
  }

  return true;
}

inline bool io_resolve_date(const io_parse_fields &fields, sys_days &result) {
  int year_value = 0;

  const bool have_year = io_resolve_year(fields, year_value);

  /*
   * Direct Gregorian date.
   */
  if (have_year && fields.has_month && fields.has_day) {
    const year_month_day ymd{
        year{year_value},
        month{static_cast<unsigned>(fields.month)},
        day{static_cast<unsigned>(fields.day)},
    };

    if (!ymd.ok())
      return false;

    result = sys_days{ymd};

    return io_check_date_fields(fields, result);
  }

  /*
   * Year + ordinal day.
   */
  if (have_year && fields.has_yday) {
    const sys_days first{
        year{year_value} / January / 1,
    };

    const bool leap = year{year_value}.is_leap();

    const int maximum = leap ? 366 : 365;

    if (fields.yday < 1 || fields.yday > maximum) {
      return false;
    }

    result = first + days{
                         fields.yday - 1,
                     };

    return io_check_date_fields(fields, result);
  }

  /*
   * ISO week date.
   */
  if (fields.has_iso_week && fields.has_weekday) {
    int iso_year = 0;

    if (!io_resolve_iso_year(fields, iso_year)) {
      return false;
    }

    const sys_days january_fourth{
        year{iso_year} / January / 4,
    };

    const unsigned january_fourth_wd = weekday{january_fourth}.iso_encoding();

    const auto first_monday =
        january_fourth - days{
                             static_cast<int>(january_fourth_wd) - 1,
                         };

    const int iso_weekday = fields.weekday == 0 ? 7 : fields.weekday;

    result = first_monday + days{
                                (fields.iso_week - 1) * 7 + (iso_weekday - 1),
                            };

    return io_check_date_fields(fields, result);
  }

  /*
   * Sunday-based week number.
   */
  if (have_year && fields.has_week_sunday && fields.has_weekday) {
    const sys_days first{
        year{year_value} / January / 1,
    };

    const unsigned first_wd = weekday{first}.c_encoding();

    const auto first_sunday =
        first + days{
                    static_cast<int>((7u - first_wd) % 7u),
                };

    result = first_sunday + days{
                                (fields.week_sunday - 1) * 7 + fields.weekday,
                            };

    if (fields.week_sunday == 0) {
      result -= days{7};
    }

    return io_check_date_fields(fields, result);
  }

  /*
   * Monday-based week number.
   */
  if (have_year && fields.has_week_monday && fields.has_weekday) {
    const sys_days first{
        year{year_value} / January / 1,
    };

    const unsigned first_wd = weekday{first}.iso_encoding() - 1;

    const auto first_monday =
        first + days{
                    static_cast<int>((7u - first_wd) % 7u),
                };

    const int monday_based = fields.weekday == 0 ? 6 : fields.weekday - 1;

    result = first_monday + days{
                                (fields.week_monday - 1) * 7 + monday_based,
                            };

    if (fields.week_monday == 0) {
      result -= days{7};
    }

    return io_check_date_fields(fields, result);
  }

  return false;
}

inline bool io_time_of_day(const io_parse_fields &fields,
                           bool allow_leap_second, long double &result) {
  int hour = 0;

  if (fields.has_hour24) {
    hour = fields.hour24;

    if (fields.has_hour12) {
      int converted = fields.hour12 % 12;

      if (fields.has_ampm && fields.pm) {
        converted += 12;
      }

      if (!fields.has_ampm || converted != hour) {
        return false;
      }
    }
  } else if (fields.has_hour12) {
    if (!fields.has_ampm)
      return false;

    hour = fields.hour12 % 12;

    if (fields.pm)
      hour += 12;
  } else if (fields.has_ampm) {
    return false;
  }

  const int minute = fields.has_minute ? fields.minute : 0;

  const long double second = fields.has_second ? fields.second : 0.0L;

  if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
    return false;
  }

  if (second < 0.0L)
    return false;

  if (allow_leap_second) {
    if (second >= 61.0L)
      return false;
  } else {
    if (second >= 60.0L)
      return false;
  }

  result = static_cast<long double>(hour * 3600 + minute * 60) + second;

  return true;
}

inline bool io_has_date_only_fields(const io_parse_fields &fields) {
  return fields.has_year || fields.has_century || fields.has_year2 ||
         fields.has_iso_year || fields.has_iso_year2 || fields.has_month ||
         fields.has_day || fields.has_weekday || fields.has_week_sunday ||
         fields.has_week_monday || fields.has_iso_week;
}

template <class Duration>
bool io_duration_from_seconds(long double value, Duration &result) {
  using rep = typename Duration::rep;

  const long double count = value *
                            static_cast<long double>(Duration::period::den) /
                            static_cast<long double>(Duration::period::num);

  if constexpr (numeric_limits<rep>::is_bounded) {
    if (count < static_cast<long double>(numeric_limits<rep>::lowest()) ||
        count > static_cast<long double>(numeric_limits<rep>::max())) {
      return false;
    }
  }

  result = Duration{
      static_cast<rep>(count),
  };

  return true;
}

template <class Duration>
bool io_make_sys_time(const io_parse_fields &fields, sys_time<Duration> &result,
                      bool apply_offset = true) {
  sys_days date;

  if (!io_resolve_date(fields, date)) {
    return false;
  }

  long double tod = 0;

  if (!io_time_of_day(fields, false, tod)) {
    return false;
  }

  long double seconds_value =
      static_cast<long double>(date.time_since_epoch().count()) * 86400.0L +
      tod;

  if (apply_offset && fields.has_offset) {
    seconds_value -= static_cast<long double>(fields.offset_minutes) * 60.0L;
  }

  Duration converted;

  if (!io_duration_from_seconds(seconds_value, converted)) {
    return false;
  }

  result = sys_time<Duration>{
      converted,
  };

  return true;
}

template <class Duration>
bool io_make_local_time(const io_parse_fields &fields,
                        local_time<Duration> &result) {
  sys_time<Duration> civil;

  if (!io_make_sys_time(fields, civil, false)) {
    return false;
  }

  result = local_time<Duration>{
      civil.time_since_epoch(),
  };

  return true;
}

template <class Duration>
bool io_make_utc_time(const io_parse_fields &fields,
                      utc_time<Duration> &result) {
  sys_days date;

  if (!io_resolve_date(fields, date)) {
    return false;
  }

  long double tod = 0;

  if (!io_time_of_day(fields, true, tod)) {
    return false;
  }

  const bool leap = fields.has_second && fields.second >= 60.0L;

  if (!leap) {
    long double seconds_value =
        static_cast<long double>(date.time_since_epoch().count()) * 86400.0L +
        tod;

    if (fields.has_offset) {
      seconds_value -= static_cast<long double>(fields.offset_minutes) * 60.0L;
    }

    using floating_duration = duration<long double>;

    const sys_time<floating_duration> system{
        floating_duration{
            seconds_value,
        },
    };

    const auto utc = utc_clock::from_sys(system);

    Duration converted;

    if (!io_duration_from_seconds(utc.time_since_epoch().count(), converted)) {
      /*
       * utc.time_since_epoch().count() is expressed
       * in floating_duration seconds here.
       */
      const long double utc_seconds =
          duration<long double>{
              utc.time_since_epoch(),
          }
              .count();

      if (!io_duration_from_seconds(utc_seconds, converted)) {
        return false;
      }
    }

    result = utc_time<Duration>{
        converted,
    };

    return true;
  }

  /*
   * sec == 60.x is represented as the positive leap
   * interval immediately before the next civil minute.
   */
  const long double fraction = fields.second - 60.0L;

  const int hour = fields.has_hour24
                       ? fields.hour24
                       : (fields.hour12 % 12) + (fields.pm ? 12 : 0);

  const int minute = fields.has_minute ? fields.minute : 0;

  long double boundary_seconds =
      static_cast<long double>(date.time_since_epoch().count()) * 86400.0L +
      static_cast<long double>(hour * 3600 + minute * 60 + 60);

  if (fields.has_offset) {
    boundary_seconds -= static_cast<long double>(fields.offset_minutes) * 60.0L;
  }

  using floating_duration = duration<long double>;

  const sys_time<floating_duration> boundary{
      floating_duration{
          boundary_seconds,
      },
  };

  const auto leap_end = utc_clock::from_sys(boundary);

  const auto candidate = leap_end - seconds{1} +
                         floating_duration{
                             fraction,
                         };

  if (!get_leap_second_info(candidate).is_leap_second) {
    return false;
  }

  const long double utc_seconds =
      duration<long double>{
          candidate.time_since_epoch(),
      }
          .count();

  Duration converted;

  if (!io_duration_from_seconds(utc_seconds, converted)) {
    return false;
  }

  result = utc_time<Duration>{
      converted,
  };

  return true;
}

template <class Character, class Traits, class Allocator>
void io_commit_auxiliary(const io_parse_fields &fields,
                         basic_string<Character, Traits, Allocator> *abbrev,
                         minutes *offset) {
  if (abbrev != nullptr && fields.has_abbrev) {
    abbrev->clear();

    for (char value : fields.abbrev) {
      abbrev->push_back(
          static_cast<Character>(static_cast<unsigned char>(value)));
    }
  }

  if (offset != nullptr && fields.has_offset) {
    *offset = minutes{
        fields.offset_minutes,
    };
  }
}

template <class Character, class Traits>
bool io_parse(basic_istream<Character, Traits> &stream, const Character *format,
              io_parse_fields &fields) {
  io_parser<Character, Traits> parser(stream);

  const bool result = parser.parse(format, fields);

  if (parser.saw_eof()) {
    stream.setstate(ios_base::eofbit);
  }

  if (!result) {
    stream.setstate(ios_base::failbit);
  }

  return result;
}

} // namespace chrono_detail

/*
 * ==========================================================================
 * Stream insertion
 * ==========================================================================
 */

template <class Character, class Traits, class Rep, class Period>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const duration<Rep, Period> &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    buffer << value.count();

    chrono_detail::io_duration_suffix<Character, Traits, Period>(buffer);
  });
}

template <class Character, class Traits, class Duration>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const hh_mm_ss<Duration> &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    chrono_detail::io_hms(buffer, value, value.is_negative());
  });
}

template <class Character, class Traits, class Duration>
  requires(!treat_as_floating_point_v<typename Duration::rep> &&
           Duration{1} < days{1})
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const sys_time<Duration> &value) {
  return chrono_detail::io_buffered(
      stream, [&](auto &buffer) { chrono_detail::io_sys_time(buffer, value); });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream, const sys_days &value) {
  return stream << year_month_day{
             value,
         };
}

template <class Character, class Traits, class Duration>
  requires requires(basic_ostream<Character, Traits> &output,
                    const sys_time<Duration> &value) { output << value; }
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const local_time<Duration> &value) {
  return stream << sys_time<Duration>{
             value.time_since_epoch(),
         };
}

template <class Character, class Traits, class Duration>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const utc_time<Duration> &value) {
  return chrono_detail::io_buffered(
      stream, [&](auto &buffer) { chrono_detail::io_utc_time(buffer, value); });
}

template <class Character, class Traits, class Duration>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const tai_time<Duration> &value) {
  return chrono_detail::io_buffered(
      stream, [&](auto &buffer) { chrono_detail::io_tai_time(buffer, value); });
}

template <class Character, class Traits, class Duration>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const gps_time<Duration> &value) {
  return chrono_detail::io_buffered(
      stream, [&](auto &buffer) { chrono_detail::io_gps_time(buffer, value); });
}

template <class Character, class Traits, class Duration>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const file_time<Duration> &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    chrono_detail::io_file_time(buffer, value);
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream, const day &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    chrono_detail::io_unsigned(buffer, static_cast<unsigned>(value), 2);

    if (!value.ok()) {
      chrono_detail::io_ascii(buffer, " is not a valid day");
    }
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream, const month &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    if (value.ok()) {
      chrono_detail::io_localized_month(buffer, static_cast<unsigned>(value));
    } else {
      chrono_detail::io_unsigned(buffer, static_cast<unsigned>(value));

      chrono_detail::io_ascii(buffer, " is not a valid month");
    }
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream, const year &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    chrono_detail::io_year(buffer, static_cast<int>(value));

    if (!value.ok()) {
      chrono_detail::io_ascii(buffer, " is not a valid year");
    }
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream, const weekday &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    if (value.ok()) {
      chrono_detail::io_localized_weekday(buffer, value.c_encoding());
    } else {
      chrono_detail::io_unsigned(buffer, value.c_encoding());

      chrono_detail::io_ascii(buffer, " is not a valid weekday");
    }
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const weekday_indexed &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    buffer << value.weekday();

    buffer.put(static_cast<Character>('['));

    chrono_detail::io_unsigned(buffer, value.index());

    if (value.index() < 1 || value.index() > 5) {
      chrono_detail::io_ascii(buffer, " is not a valid index");
    }

    buffer.put(static_cast<Character>(']'));
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const weekday_last &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    buffer << value.weekday();

    chrono_detail::io_ascii(buffer, "[last]");
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream, const month_day &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    buffer << value.month();

    buffer.put(static_cast<Character>('/'));

    buffer << value.day();
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const month_day_last &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    buffer << value.month();

    chrono_detail::io_ascii(buffer, "/last");
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const month_weekday &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    buffer << value.month();

    buffer.put(static_cast<Character>('/'));

    buffer << value.weekday_indexed();
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const month_weekday_last &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    buffer << value.month();

    buffer.put(static_cast<Character>('/'));

    buffer << value.weekday_last();
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream, const year_month &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    buffer << value.year();

    buffer.put(static_cast<Character>('/'));

    buffer << value.month();
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const year_month_day &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    chrono_detail::io_ymd(buffer, value);

    if (!value.ok()) {
      chrono_detail::io_ascii(buffer, " is not a valid date");
    }
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const year_month_day_last &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    buffer << value.year();

    buffer.put(static_cast<Character>('/'));

    buffer << value.month();

    chrono_detail::io_ascii(buffer, "/last");
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const year_month_weekday &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    buffer << value.year();

    buffer.put(static_cast<Character>('/'));

    buffer << value.month();

    buffer.put(static_cast<Character>('/'));

    buffer << value.weekday_indexed();
  });
}

template <class Character, class Traits>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const year_month_weekday_last &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    buffer << value.year();

    buffer.put(static_cast<Character>('/'));

    buffer << value.month();

    buffer.put(static_cast<Character>('/'));

    buffer << value.weekday_last();
  });
}

template <class Character, class Traits, class Duration, class TimeZonePtr>
basic_ostream<Character, Traits> &
operator<<(basic_ostream<Character, Traits> &stream,
           const zoned_time<Duration, TimeZonePtr> &value) {
  return chrono_detail::io_buffered(stream, [&](auto &buffer) {
    buffer << value.get_local_time();

    buffer.put(static_cast<Character>(' '));

    const auto info = value.get_info();

    for (char character : info.abbrev) {
      buffer.put(static_cast<Character>(static_cast<unsigned char>(character)));
    }
  });
}

/*
 * ==========================================================================
 * Exact time-zone exception diagnostics
 * ==========================================================================
 */

namespace chrono_detail {

template <class Duration>
string io_nonexistent_message(const local_time<Duration> &tp,
                              const local_info &info) {
  basic_ostringstream<char, char_traits<char>, allocator<char>> stream;

  stream << tp << " is in a gap between\n"
         << local_seconds{info.first.end.time_since_epoch()} + info.first.offset
         << ' ' << info.first.abbrev << " and\n"
         << local_seconds{info.second.begin.time_since_epoch()} +
                info.second.offset
         << ' ' << info.second.abbrev << " which are both equivalent to\n"
         << info.first.end << " UTC";

  return stream.str();
}

template <class Duration>
string io_ambiguous_message(const local_time<Duration> &tp,
                            const local_info &info) {
  basic_ostringstream<char, char_traits<char>, allocator<char>> stream;

  stream << tp << " is ambiguous. It could be\n"
         << tp << ' ' << info.first.abbrev << " == " << tp - info.first.offset
         << " UTC or\n"
         << tp << ' ' << info.second.abbrev << " == " << tp - info.second.offset
         << " UTC";

  return stream.str();
}

} // namespace chrono_detail

template <class Duration>
nonexistent_local_time::nonexistent_local_time(const local_time<Duration> &tp,
                                               const local_info &info)
    : runtime_error(chrono_detail::io_nonexistent_message(tp, info)) {}

template <class Duration>
ambiguous_local_time::ambiguous_local_time(const local_time<Duration> &tp,
                                           const local_info &info)
    : runtime_error(chrono_detail::io_ambiguous_message(tp, info)) {}

/*
 * ==========================================================================
 * from_stream
 * ==========================================================================
 */

template <class Character, class Traits, class Rep, class Period,
          class Allocator = allocator<Character>>
basic_istream<Character, Traits> &
from_stream(basic_istream<Character, Traits> &stream, const Character *format,
            duration<Rep, Period> &target,
            basic_string<Character, Traits, Allocator> *abbrev = nullptr,
            minutes *offset = nullptr) {
  chrono_detail::io_parse_fields fields;

  if (!chrono_detail::io_parse(stream, format, fields)) {
    return stream;
  }

  /*
   * duration may represent time-of-day fields and
   * %j as a count of days, but not civil calendar
   * fields.
   */
  if (fields.has_year || fields.has_century || fields.has_year2 ||
      fields.has_iso_year || fields.has_iso_year2 || fields.has_month ||
      fields.has_day || fields.has_weekday || fields.has_week_sunday ||
      fields.has_week_monday || fields.has_iso_week) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  long double tod = 0;

  if (!chrono_detail::io_time_of_day(fields, false, tod)) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  bool represented = fields.has_yday || fields.has_hour24 ||
                     fields.has_hour12 || fields.has_minute ||
                     fields.has_second;

  if (!represented) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  long double total = tod;

  if (fields.has_yday) {
    total += static_cast<long double>(fields.yday) * 86400.0L;
  }

  duration<Rep, Period> candidate;

  if (!chrono_detail::io_duration_from_seconds(total, candidate)) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  target = candidate;

  chrono_detail::io_commit_auxiliary(fields, abbrev, offset);

  return stream;
}

template <class Character, class Traits, class Duration,
          class Allocator = allocator<Character>>
basic_istream<Character, Traits> &
from_stream(basic_istream<Character, Traits> &stream, const Character *format,
            sys_time<Duration> &target,
            basic_string<Character, Traits, Allocator> *abbrev = nullptr,
            minutes *offset = nullptr) {
  chrono_detail::io_parse_fields fields;

  if (!chrono_detail::io_parse(stream, format, fields)) {
    return stream;
  }

  sys_time<Duration> candidate;

  if (!chrono_detail::io_make_sys_time(fields, candidate)) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  target = candidate;

  chrono_detail::io_commit_auxiliary(fields, abbrev, offset);

  return stream;
}

template <class Character, class Traits, class Duration,
          class Allocator = allocator<Character>>
basic_istream<Character, Traits> &
from_stream(basic_istream<Character, Traits> &stream, const Character *format,
            local_time<Duration> &target,
            basic_string<Character, Traits, Allocator> *abbrev = nullptr,
            minutes *offset = nullptr) {
  chrono_detail::io_parse_fields fields;

  if (!chrono_detail::io_parse(stream, format, fields)) {
    return stream;
  }

  local_time<Duration> candidate;

  if (!chrono_detail::io_make_local_time(fields, candidate)) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  target = candidate;

  chrono_detail::io_commit_auxiliary(fields, abbrev, offset);

  return stream;
}

template <class Character, class Traits, class Duration,
          class Allocator = allocator<Character>>
basic_istream<Character, Traits> &
from_stream(basic_istream<Character, Traits> &stream, const Character *format,
            utc_time<Duration> &target,
            basic_string<Character, Traits, Allocator> *abbrev = nullptr,
            minutes *offset = nullptr) {
  chrono_detail::io_parse_fields fields;

  if (!chrono_detail::io_parse(stream, format, fields)) {
    return stream;
  }

  utc_time<Duration> candidate;

  if (!chrono_detail::io_make_utc_time(fields, candidate)) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  target = candidate;

  chrono_detail::io_commit_auxiliary(fields, abbrev, offset);

  return stream;
}

template <class Character, class Traits, class Duration,
          class Allocator = allocator<Character>>
basic_istream<Character, Traits> &
from_stream(basic_istream<Character, Traits> &stream, const Character *format,
            tai_time<Duration> &target,
            basic_string<Character, Traits, Allocator> *abbrev = nullptr,
            minutes *offset = nullptr) {
  chrono_detail::io_parse_fields fields;

  if (!chrono_detail::io_parse(stream, format, fields)) {
    return stream;
  }

  using floating = duration<long double>;

  sys_time<floating> civil;

  if (!chrono_detail::io_make_sys_time(fields, civil)) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  const auto epoch_difference =
      sys_days{year{1970} / January / 1} - sys_days{year{1958} / January / 1};

  const floating raw =
      civil.time_since_epoch() + duration_cast<floating>(epoch_difference);

  Duration converted;

  if (!chrono_detail::io_duration_from_seconds(raw.count(), converted)) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  target = tai_time<Duration>{
      converted,
  };

  chrono_detail::io_commit_auxiliary(fields, abbrev, offset);

  return stream;
}

template <class Character, class Traits, class Duration,
          class Allocator = allocator<Character>>
basic_istream<Character, Traits> &
from_stream(basic_istream<Character, Traits> &stream, const Character *format,
            gps_time<Duration> &target,
            basic_string<Character, Traits, Allocator> *abbrev = nullptr,
            minutes *offset = nullptr) {
  chrono_detail::io_parse_fields fields;

  if (!chrono_detail::io_parse(stream, format, fields)) {
    return stream;
  }

  using floating = duration<long double>;

  sys_time<floating> civil;

  if (!chrono_detail::io_make_sys_time(fields, civil)) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  const auto epoch_difference = sys_days{year{1980} / January / Sunday[1]} -
                                sys_days{year{1970} / January / 1};

  const floating raw =
      civil.time_since_epoch() - duration_cast<floating>(epoch_difference);

  Duration converted;

  if (!chrono_detail::io_duration_from_seconds(raw.count(), converted)) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  target = gps_time<Duration>{
      converted,
  };

  chrono_detail::io_commit_auxiliary(fields, abbrev, offset);

  return stream;
}

template <class Character, class Traits, class Duration,
          class Allocator = allocator<Character>>
basic_istream<Character, Traits> &
from_stream(basic_istream<Character, Traits> &stream, const Character *format,
            file_time<Duration> &target,
            basic_string<Character, Traits, Allocator> *abbrev = nullptr,
            minutes *offset = nullptr) {
  chrono_detail::io_parse_fields fields;

  if (!chrono_detail::io_parse(stream, format, fields)) {
    return stream;
  }

  using floating = duration<long double>;

  sys_time<floating> civil;

  if (!chrono_detail::io_make_sys_time(fields, civil)) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  const auto file = file_clock::from_sys(civil);

  const long double seconds_value =
      duration<long double>{
          file.time_since_epoch(),
      }
          .count();

  Duration converted;

  if (!chrono_detail::io_duration_from_seconds(seconds_value, converted)) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  target = file_time<Duration>{
      converted,
  };

  chrono_detail::io_commit_auxiliary(fields, abbrev, offset);

  return stream;
}

/*
 * Calendar from_stream overloads.
 */

template <class Character, class Traits, class Allocator = allocator<Character>>
basic_istream<Character, Traits> &
from_stream(basic_istream<Character, Traits> &stream, const Character *format,
            day &target,
            basic_string<Character, Traits, Allocator> *abbrev = nullptr,
            minutes *offset = nullptr) {
  chrono_detail::io_parse_fields fields;

  if (!chrono_detail::io_parse(stream, format, fields)) {
    return stream;
  }

  if (!fields.has_day) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  day candidate{
      static_cast<unsigned>(fields.day),
  };

  if (!candidate.ok()) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  target = candidate;

  chrono_detail::io_commit_auxiliary(fields, abbrev, offset);

  return stream;
}

template <class Character, class Traits, class Allocator = allocator<Character>>
basic_istream<Character, Traits> &
from_stream(basic_istream<Character, Traits> &stream, const Character *format,
            month &target,
            basic_string<Character, Traits, Allocator> *abbrev = nullptr,
            minutes *offset = nullptr) {
  chrono_detail::io_parse_fields fields;

  if (!chrono_detail::io_parse(stream, format, fields)) {
    return stream;
  }

  if (!fields.has_month) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  month candidate{
      static_cast<unsigned>(fields.month),
  };

  if (!candidate.ok()) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  target = candidate;

  chrono_detail::io_commit_auxiliary(fields, abbrev, offset);

  return stream;
}

template <class Character, class Traits, class Allocator = allocator<Character>>
basic_istream<Character, Traits> &
from_stream(basic_istream<Character, Traits> &stream, const Character *format,
            year &target,
            basic_string<Character, Traits, Allocator> *abbrev = nullptr,
            minutes *offset = nullptr) {
  chrono_detail::io_parse_fields fields;

  if (!chrono_detail::io_parse(stream, format, fields)) {
    return stream;
  }

  int value = 0;

  if (!chrono_detail::io_resolve_year(fields, value)) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  year candidate{
      value,
  };

  if (!candidate.ok()) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  target = candidate;

  chrono_detail::io_commit_auxiliary(fields, abbrev, offset);

  return stream;
}

template <class Character, class Traits, class Allocator = allocator<Character>>
basic_istream<Character, Traits> &
from_stream(basic_istream<Character, Traits> &stream, const Character *format,
            weekday &target,
            basic_string<Character, Traits, Allocator> *abbrev = nullptr,
            minutes *offset = nullptr) {
  chrono_detail::io_parse_fields fields;

  if (!chrono_detail::io_parse(stream, format, fields)) {
    return stream;
  }

  if (!fields.has_weekday) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  weekday candidate{
      static_cast<unsigned>(fields.weekday),
  };

  if (!candidate.ok()) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  target = candidate;

  chrono_detail::io_commit_auxiliary(fields, abbrev, offset);

  return stream;
}

template <class Character, class Traits, class Allocator = allocator<Character>>
basic_istream<Character, Traits> &
from_stream(basic_istream<Character, Traits> &stream, const Character *format,
            month_day &target,
            basic_string<Character, Traits, Allocator> *abbrev = nullptr,
            minutes *offset = nullptr) {
  chrono_detail::io_parse_fields fields;

  if (!chrono_detail::io_parse(stream, format, fields)) {
    return stream;
  }

  if (!fields.has_month || !fields.has_day) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  month_day candidate{
      month{static_cast<unsigned>(fields.month)},
      day{static_cast<unsigned>(fields.day)},
  };

  if (!candidate.ok()) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  target = candidate;

  chrono_detail::io_commit_auxiliary(fields, abbrev, offset);

  return stream;
}

template <class Character, class Traits, class Allocator = allocator<Character>>
basic_istream<Character, Traits> &
from_stream(basic_istream<Character, Traits> &stream, const Character *format,
            year_month &target,
            basic_string<Character, Traits, Allocator> *abbrev = nullptr,
            minutes *offset = nullptr) {
  chrono_detail::io_parse_fields fields;

  if (!chrono_detail::io_parse(stream, format, fields)) {
    return stream;
  }

  int year_value = 0;

  if (!chrono_detail::io_resolve_year(fields, year_value) ||
      !fields.has_month) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  year_month candidate{
      year{year_value},
      month{static_cast<unsigned>(fields.month)},
  };

  if (!candidate.ok()) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  target = candidate;

  chrono_detail::io_commit_auxiliary(fields, abbrev, offset);

  return stream;
}

template <class Character, class Traits, class Allocator = allocator<Character>>
basic_istream<Character, Traits> &
from_stream(basic_istream<Character, Traits> &stream, const Character *format,
            year_month_day &target,
            basic_string<Character, Traits, Allocator> *abbrev = nullptr,
            minutes *offset = nullptr) {
  chrono_detail::io_parse_fields fields;

  if (!chrono_detail::io_parse(stream, format, fields)) {
    return stream;
  }

  sys_days date;

  if (!chrono_detail::io_resolve_date(fields, date)) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  const year_month_day candidate{
      date,
  };

  if (!candidate.ok()) {
    stream.setstate(ios_base::failbit);

    return stream;
  }

  target = candidate;

  chrono_detail::io_commit_auxiliary(fields, abbrev, offset);

  return stream;
}

/*
 * ==========================================================================
 * parse manipulators
 * ==========================================================================
 */

namespace chrono_detail {

template <class Character, class Parsable, class Abbrev>
class io_parse_manipulator {
public:
  io_parse_manipulator(basic_string<Character> format, Parsable &value,
                       Abbrev *abbrev, minutes *offset)
      : format_(move(format)), value_(addressof(value)), abbrev_(abbrev),
        offset_(offset) {}

  io_parse_manipulator(const io_parse_manipulator &) = delete;

  io_parse_manipulator &operator=(const io_parse_manipulator &) = delete;

  io_parse_manipulator(io_parse_manipulator &&) = delete;

  io_parse_manipulator &operator=(io_parse_manipulator &&) = delete;

  template <class Traits>
  friend basic_istream<Character, Traits> &
  operator>>(basic_istream<Character, Traits> &stream,
             io_parse_manipulator &&value) {
    if constexpr (is_void_v<Abbrev>) {
      if (value.offset_ == nullptr) {
        from_stream(stream, value.format_.c_str(), *value.value_);
      } else {
        from_stream(stream, value.format_.c_str(), *value.value_,
                    static_cast<basic_string<Character, Traits,
                                             allocator<Character>> *>(nullptr),
                    value.offset_);
      }
    } else {
      from_stream(stream, value.format_.c_str(), *value.value_, value.abbrev_,
                  value.offset_);
    }

    return stream;
  }

private:
  basic_string<Character> format_;

  Parsable *value_;

  Abbrev *abbrev_;

  minutes *offset_;
};

template <class Character, class Parsable, class Abbrev>
auto io_make_parse_manipulator(const Character *format, Parsable &value,
                               Abbrev *abbrev, minutes *offset) {
  return io_parse_manipulator<Character, Parsable, Abbrev>{
      basic_string<Character>{format},
      value,
      abbrev,
      offset,
  };
}

template <class Character, class FormatTraits, class FormatAllocator,
          class Parsable, class Abbrev>
auto io_make_parse_manipulator(
    const basic_string<Character, FormatTraits, FormatAllocator> &format,
    Parsable &value, Abbrev *abbrev, minutes *offset) {
  return io_parse_manipulator<Character, Parsable, Abbrev>{
      basic_string<Character>{format.data(), format.size()},
      value,
      abbrev,
      offset,
  };
}

} // namespace chrono_detail

template <class Character, class Parsable>
auto parse(const Character *format, Parsable &value) {
  return chrono_detail::io_make_parse_manipulator<Character, Parsable, void>(
      format, value, nullptr, nullptr);
}

template <class Character, class FormatTraits, class FormatAllocator,
          class Parsable>
auto parse(const basic_string<Character, FormatTraits, FormatAllocator> &format,
           Parsable &value) {
  return chrono_detail::io_make_parse_manipulator<
      Character, FormatTraits, FormatAllocator, Parsable, void>(
      format, value, nullptr, nullptr);
}

template <class Character, class Traits, class Allocator, class Parsable>
auto parse(const Character *format, Parsable &value,
           basic_string<Character, Traits, Allocator> &abbrev) {
  return chrono_detail::io_make_parse_manipulator(format, value,
                                                  addressof(abbrev), nullptr);
}

template <class Character, class FormatTraits, class FormatAllocator,
          class Traits, class Allocator, class Parsable>
auto parse(const basic_string<Character, FormatTraits, FormatAllocator> &format,
           Parsable &value,
           basic_string<Character, Traits, Allocator> &abbrev) {
  return chrono_detail::io_make_parse_manipulator(format, value,
                                                  addressof(abbrev), nullptr);
}

template <class Character, class Parsable>
auto parse(const Character *format, Parsable &value, minutes &offset) {
  return chrono_detail::io_make_parse_manipulator<Character, Parsable, void>(
      format, value, nullptr, addressof(offset));
}

template <class Character, class FormatTraits, class FormatAllocator,
          class Parsable>
auto parse(const basic_string<Character, FormatTraits, FormatAllocator> &format,
           Parsable &value, minutes &offset) {
  return chrono_detail::io_make_parse_manipulator<
      Character, FormatTraits, FormatAllocator, Parsable, void>(
      format, value, nullptr, addressof(offset));
}

template <class Character, class Traits, class Allocator, class Parsable>
auto parse(const Character *format, Parsable &value,
           basic_string<Character, Traits, Allocator> &abbrev,
           minutes &offset) {
  return chrono_detail::io_make_parse_manipulator(
      format, value, addressof(abbrev), addressof(offset));
}

template <class Character, class FormatTraits, class FormatAllocator,
          class Traits, class Allocator, class Parsable>
auto parse(const basic_string<Character, FormatTraits, FormatAllocator> &format,
           Parsable &value, basic_string<Character, Traits, Allocator> &abbrev,
           minutes &offset) {
  return chrono_detail::io_make_parse_manipulator(
      format, value, addressof(abbrev), addressof(offset));
}

} // namespace chrono

FTL_END_NAMESPACE

#endif // FTL_DETAIL_CHRONO_IO_HPP
