#ifdef FTL_REPLACE_STL
#include <charconv>
#include <limits>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/charconv>
#include <ftl/limits>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

#if __cpp_lib_to_chars < 201611L
#error <charconv> must advertise primitive numeric conversions
#endif

#if __cpp_lib_constexpr_charconv < 202207L
#error <charconv> must advertise constexpr integer conversion
#endif

enum charconv_unscoped_enum { charconv_enum_value = 42 };

enum class charconv_scoped_enum { value = 42 };

template <class T>
concept has_integer_to_chars = requires(char *first, char *last, T value) {
  tested::to_chars(first, last, value);
};

template <class T>
concept has_integer_from_chars =
    requires(const char *first, const char *last, T &value) {
      tested::from_chars(first, last, value);
    };

template <class Float>
concept has_exact_floating_charconv_overloads = requires {
  static_cast<tested::to_chars_result (*)(char *, char *, Float)>(
      &tested::to_chars);

  static_cast<tested::to_chars_result (*)(
      char *, char *, Float, tested::chars_format)>(&tested::to_chars);

  static_cast<tested::to_chars_result (*)(
      char *, char *, Float, tested::chars_format, int)>(&tested::to_chars);

  static_cast<tested::from_chars_result (*)(const char *, const char *, Float &,
                                            tested::chars_format)>(
      &tested::from_chars);
};

static_assert(has_exact_floating_charconv_overloads<float>);
static_assert(has_exact_floating_charconv_overloads<double>);
static_assert(has_exact_floating_charconv_overloads<long double>);

#ifdef __STDCPP_FLOAT16_T__
static_assert(has_exact_floating_charconv_overloads<decltype(0.0f16)>);
#endif

#ifdef __STDCPP_FLOAT32_T__
static_assert(has_exact_floating_charconv_overloads<decltype(0.0f32)>);
#endif

#ifdef __STDCPP_FLOAT64_T__
static_assert(has_exact_floating_charconv_overloads<decltype(0.0f64)>);
#endif

#ifdef __STDCPP_FLOAT128_T__
static_assert(has_exact_floating_charconv_overloads<decltype(0.0f128)>);
#endif

#ifdef __STDCPP_BFLOAT16_T__
static_assert(has_exact_floating_charconv_overloads<decltype(0.0bf16)>);
#endif

static_assert(has_integer_to_chars<char>);
static_assert(has_integer_to_chars<signed char>);
static_assert(has_integer_to_chars<unsigned char>);
static_assert(has_integer_to_chars<short>);
static_assert(has_integer_to_chars<unsigned short>);
static_assert(has_integer_to_chars<int>);
static_assert(has_integer_to_chars<unsigned int>);
static_assert(has_integer_to_chars<long>);
static_assert(has_integer_to_chars<unsigned long>);
static_assert(has_integer_to_chars<long long>);
static_assert(has_integer_to_chars<unsigned long long>);

static_assert(has_integer_from_chars<char>);
static_assert(has_integer_from_chars<signed char>);
static_assert(has_integer_from_chars<unsigned char>);
static_assert(has_integer_from_chars<short>);
static_assert(has_integer_from_chars<unsigned short>);
static_assert(has_integer_from_chars<int>);
static_assert(has_integer_from_chars<unsigned int>);
static_assert(has_integer_from_chars<long>);
static_assert(has_integer_from_chars<unsigned long>);
static_assert(has_integer_from_chars<long long>);
static_assert(has_integer_from_chars<unsigned long long>);

static_assert(!has_integer_to_chars<bool>);
static_assert(has_integer_to_chars<wchar_t>);
static_assert(has_integer_to_chars<char8_t>);
static_assert(has_integer_to_chars<char16_t>);
static_assert(has_integer_to_chars<char32_t>);

static_assert(!has_integer_from_chars<bool>);
static_assert(!has_integer_from_chars<wchar_t>);
static_assert(!has_integer_from_chars<char8_t>);
static_assert(!has_integer_from_chars<char16_t>);
static_assert(!has_integer_from_chars<char32_t>);

static_assert(!has_integer_from_chars<const int>);
static_assert(!has_integer_from_chars<volatile int>);
static_assert(!has_integer_from_chars<const volatile int>);

static_assert(has_integer_to_chars<charconv_unscoped_enum>);
static_assert(!has_integer_to_chars<charconv_scoped_enum>);

static_assert(!has_integer_from_chars<charconv_unscoped_enum>);
static_assert(!has_integer_from_chars<charconv_scoped_enum>);

#if defined(__SIZEOF_INT128__)
static_assert(has_integer_to_chars<__int128>);
static_assert(has_integer_to_chars<unsigned __int128>);
static_assert(has_integer_from_chars<__int128>);
static_assert(has_integer_from_chars<unsigned __int128>);

constexpr bool extended_integer_charconv_works() {
  char buffer[64]{};
  const __int128 original = (static_cast<__int128>(1) << 100) + 17;
  auto written = tested::to_chars(buffer, buffer + 64, original);
  __int128 parsed{};
  auto read = tested::from_chars(buffer, written.ptr, parsed);
  return written.ec == tested::errc{} && read.ec == tested::errc{} &&
         read.ptr == written.ptr && parsed == original;
}
static_assert(extended_integer_charconv_works());
#endif

constexpr bool equal_text(const char *first, const char *last,
                          const char *expected) {
  while (first != last && *expected != '\0') {
    if (*first++ != *expected++)
      return false;
  }

  return first == last && *expected == '\0';
}

constexpr bool integer_to_chars_tests() {
  char buffer[128]{};

  {
    auto result = tested::to_chars(buffer, buffer + 128, 0);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "0"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 128, -12345);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "-12345"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 128, 255u, 16);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "ff"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 128, 35u, 36);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "z"))
      return false;
  }

  {
    constexpr auto minimum = tested::numeric_limits<long long>::min();

    auto result = tested::to_chars(buffer, buffer + 128, minimum);

    long long round_trip = 0;

    auto parsed = tested::from_chars(buffer, result.ptr, round_trip);

    if (parsed.ec != tested::errc{} || parsed.ptr != result.ptr ||
        round_trip != minimum)
      return false;
  }

  {
    char tiny[1]{};

    auto result = tested::to_chars(tiny, tiny + 1, 42);

    if (result.ptr != tiny + 1 || result.ec != tested::errc::value_too_large)
      return false;
  }

  /*
   * N4950 specifies a real overload set rather than
   * a deduced integral template. An unscoped enum can
   * therefore reach the appropriate integer overload
   * through integral promotion.
   */
  {
    auto result = tested::to_chars(buffer, buffer + 128, charconv_enum_value);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "42"))
      return false;
  }

  return true;
}

static_assert(integer_to_chars_tests());

constexpr bool integer_from_chars_tests() {
  {
    int value = 99;
    constexpr char text[] = "123xyz";

    auto result = tested::from_chars(text, text + 6, value);

    if (result.ec != tested::errc{} || result.ptr != text + 3 || value != 123)
      return false;
  }

  {
    int value = 99;
    constexpr char text[] = "-42";

    auto result = tested::from_chars(text, text + 3, value);

    if (result.ec != tested::errc{} || result.ptr != text + 3 || value != -42)
      return false;
  }

  {
    unsigned value = 99;
    constexpr char text[] = "-1";

    auto result = tested::from_chars(text, text + 2, value);

    if (result.ec != tested::errc::invalid_argument || result.ptr != text ||
        value != 99)
      return false;
  }

  {
    int value = 99;
    constexpr char text[] = "+1";

    auto result = tested::from_chars(text, text + 2, value);

    if (result.ec != tested::errc::invalid_argument || result.ptr != text ||
        value != 99)
      return false;
  }

  {
    int value = 99;
    constexpr char text[] = " 1";

    auto result = tested::from_chars(text, text + 2, value);

    if (result.ec != tested::errc::invalid_argument || result.ptr != text ||
        value != 99)
      return false;
  }

  {
    int value = 99;
    constexpr char text[] = "0x12";

    auto result = tested::from_chars(text, text + 4, value, 16);

    /*
     * <charconv> deliberately does not recognize 0x.
     * Only the leading zero matches.
     */
    if (result.ec != tested::errc{} || result.ptr != text + 1 || value != 0)
      return false;
  }

  {
    int value = 99;
    constexpr char text[] = "7fffffff";

    auto result = tested::from_chars(text, text + 8, value, 16);

    if (result.ec != tested::errc{} || result.ptr != text + 8 ||
        value != tested::numeric_limits<int>::max())
      return false;
  }

  {
    int value = 99;
    constexpr char text[] = "999999999999999999999tail";

    auto result = tested::from_chars(text, text + 25, value);

    /*
     * On overflow the complete matching digit
     * sequence is still consumed.
     */
    if (result.ec != tested::errc::result_out_of_range ||
        result.ptr != text + 21 || value != 99)
      return false;
  }

  {
    int value = 99;
    constexpr char text[] = "999999999999999999999";

    auto result = tested::from_chars(text, text + 21, value);

    if (result.ec != tested::errc::result_out_of_range ||
        result.ptr != text + 21 || value != 99)
      return false;
  }

  return true;
}

static_assert(integer_from_chars_tests());

static_assert(tested::to_chars_result{nullptr, tested::errc{}} ==
              tested::to_chars_result{nullptr, tested::errc{}});

static_assert(tested::from_chars_result{nullptr, tested::errc{}} ==
              tested::from_chars_result{nullptr, tested::errc{}});

bool floating_from_chars_tests() {
  /*
   * General decimal conversion and partial
   * consumption.
   */
  {
    float value = 99.0f;

    constexpr char text[] = "1.5xyz";

    const auto result = tested::from_chars(text, text + 6, value);

    if (result.ec != tested::errc{} || result.ptr != text + 3 || value != 1.5f)
      return false;
  }

  {
    double value = 99.0;

    constexpr char text[] = "-1.5e2tail";

    const auto result = tested::from_chars(text, text + 10, value);

    if (result.ec != tested::errc{} || result.ptr != text + 6 ||
        value != -150.0)
      return false;
  }

  /*
   * General mode accepts an exponent, but an
   * incomplete exponent is not part of the
   * matched sequence.
   */
  {
    double value = 99.0;

    constexpr char text[] = "1e+";

    const auto result = tested::from_chars(text, text + 3, value,
                                           tested::chars_format::general);

    if (result.ec != tested::errc{} || result.ptr != text + 1 || value != 1.0)
      return false;
  }

  /*
   * Fixed mode prohibits the exponent part.
   */
  {
    double value = 99.0;

    constexpr char text[] = "1.25e3";

    const auto result =
        tested::from_chars(text, text + 6, value, tested::chars_format::fixed);

    if (result.ec != tested::errc{} || result.ptr != text + 4 || value != 1.25)
      return false;
  }

  /*
   * Scientific-only mode requires the exponent.
   */
  {
    double value = 99.0;

    constexpr char text[] = "1.25";

    const auto result = tested::from_chars(text, text + 4, value,
                                           tested::chars_format::scientific);

    if (result.ec != tested::errc::invalid_argument || result.ptr != text ||
        value != 99.0)
      return false;
  }

  {
    double value = 99.0;

    constexpr char text[] = "1.25e3x";

    const auto result = tested::from_chars(text, text + 7, value,
                                           tested::chars_format::scientific);

    if (result.ec != tested::errc{} || result.ptr != text + 6 ||
        value != 1250.0)
      return false;
  }

  /*
   * Leading '+' is forbidden.
   */
  {
    float value = 99.0f;

    constexpr char text[] = "+1.0";

    const auto result = tested::from_chars(text, text + 4, value);

    if (result.ec != tested::errc::invalid_argument || result.ptr != text ||
        value != 99.0f)
      return false;
  }

  /*
   * No whitespace skipping.
   */
  {
    float value = 99.0f;

    constexpr char text[] = " 1.0";

    const auto result = tested::from_chars(text, text + 4, value);

    if (result.ec != tested::errc::invalid_argument || result.ptr != text ||
        value != 99.0f)
      return false;
  }

  /*
   * Numeric overflow consumes the entire matched
   * sequence and leaves the destination unchanged.
   */
  {
    double value = 99.0;

    constexpr char text[] = "1e309";

    const auto result = tested::from_chars(text, text + 5, value);

    if (result.ec != tested::errc::result_out_of_range ||
        result.ptr != text + 5 || value != 99.0)
      return false;
  }

  /*
   * Same contract for underflow.
   */
  {
    double value = 99.0;

    constexpr char text[] = "1e-400";

    const auto result = tested::from_chars(text, text + 6, value);

    if (result.ec != tested::errc::result_out_of_range ||
        result.ptr != text + 6 || value != 99.0)
      return false;
  }

  /*
   * Exercise a non-exact decimal through the
   * public binary32 path.
   */
  {
    float value = 0.0f;

    constexpr char text[] = "0.1";

    const auto result = tested::from_chars(text, text + 3, value);

    if (result.ec != tested::errc{} || result.ptr != text + 3 || value != 0.1f)
      return false;
  }

  /*
   * Hexadecimal input does not consume a 0x
   * prefix. The prefix is assumed by the format.
   *
   * Therefore "0x123" successfully converts only
   * the initial zero and stops at 'x'.
   */
  {
    double value = 99.0;

    constexpr char text[] = "0x123";

    const auto result =
        tested::from_chars(text, text + 5, value, tested::chars_format::hex);

    if (result.ec != tested::errc{} || result.ptr != text + 1 || value != 0.0)
      return false;
  }

  /*
   * 0x1.8p+1 == 3.0, with the 0x omitted from
   * the actual character sequence.
   */
  {
    double value = 99.0;

    constexpr char text[] = "1.8p+1";

    const auto result =
        tested::from_chars(text, text + 6, value, tested::chars_format::hex);

    if (result.ec != tested::errc{} || result.ptr != text + 6 || value != 3.0)
      return false;
  }

  {
    float value = 99.0f;

    constexpr char text[] = "-1.8p+1x";

    const auto result =
        tested::from_chars(text, text + 8, value, tested::chars_format::hex);

    if (result.ec != tested::errc{} || result.ptr != text + 7 || value != -3.0f)
      return false;
  }

  /*
   * The binary exponent is optional for
   * chars_format::hex.
   *
   * 1.8 hexadecimal == 1.5 decimal.
   */
  {
    double value = 99.0;

    constexpr char text[] = "1.8";

    const auto result =
        tested::from_chars(text, text + 3, value, tested::chars_format::hex);

    if (result.ec != tested::errc{} || result.ptr != text + 3 || value != 1.5)
      return false;
  }

  /*
   * An incomplete optional binary exponent is
   * excluded from the matched sequence.
   */
  {
    double value = 99.0;

    constexpr char text[] = "1p+";

    const auto result =
        tested::from_chars(text, text + 3, value, tested::chars_format::hex);

    if (result.ec != tested::errc{} || result.ptr != text + 1 || value != 1.0)
      return false;
  }

  /*
   * Exact minimum binary32 subnormal.
   */
  {
    float value = 99.0f;

    constexpr char text[] = "1p-149";

    const auto result =
        tested::from_chars(text, text + 6, value, tested::chars_format::hex);

    if (result.ec != tested::errc{} || result.ptr != text + 6)
      return false;

    if (value != tested::numeric_limits<float>::denorm_min())
      return false;
  }

  /*
   * Exactly halfway between zero and the
   * minimum subnormal rounds to zero, which is
   * outside the representable nonzero range and
   * therefore reported as result_out_of_range.
   *
   * The destination remains unchanged.
   */
  {
    float value = 99.0f;

    constexpr char text[] = "1p-150";

    const auto result =
        tested::from_chars(text, text + 6, value, tested::chars_format::hex);

    if (result.ec != tested::errc::result_out_of_range ||
        result.ptr != text + 6 || value != 99.0f)
      return false;
  }

  /*
   * Exact halfway rounding at 1.0f:
   *
   *   0x1.000001p0
   *
   * is halfway between 1.0f and the next float.
   * 1.0 has the even significand.
   */
  {
    float value = 99.0f;

    constexpr char text[] = "1.000001p0";

    const auto result =
        tested::from_chars(text, text + 10, value, tested::chars_format::hex);

    if (result.ec != tested::errc{} || result.ptr != text + 10 || value != 1.0f)
      return false;
  }

  /*
   * Leading '+' and whitespace remain invalid
   * in hexadecimal mode too.
   */
  {
    double value = 99.0;

    constexpr char text[] = "+1";

    const auto result =
        tested::from_chars(text, text + 2, value, tested::chars_format::hex);

    if (result.ec != tested::errc::invalid_argument || result.ptr != text ||
        value != 99.0)
      return false;
  }

  {
    double value = 99.0;

    constexpr char text[] = " 1";

    const auto result =
        tested::from_chars(text, text + 2, value, tested::chars_format::hex);

    if (result.ec != tested::errc::invalid_argument || result.ptr != text ||
        value != 99.0)
      return false;
  }

  {
    float value = 0.0f;

    constexpr char text[] = "INFtail";

    const auto result = tested::from_chars(text, text + 7, value);

    if (result.ec != tested::errc{} || result.ptr != text + 3 ||
        value != tested::numeric_limits<float>::infinity())
      return false;
  }

  {
    double value = 0.0;

    constexpr char text[] = "-infinity!";

    const auto result = tested::from_chars(text, text + 10, value);

    if (result.ec != tested::errc{} || result.ptr != text + 9 ||
        value != -tested::numeric_limits<double>::infinity())
      return false;
  }

  {
    double value = 0.0;

    constexpr char text[] = "NaN(payload_123)x";

    const auto result = tested::from_chars(text, text + 17, value);

    if (result.ec != tested::errc{} || result.ptr != text + 16)
      return false;

    /*
     * NaN is the only floating value unequal to
     * itself.
     */
    if (value == value)
      return false;
  }

  /*
   * Malformed optional payload still leaves the
   * shorter valid "nan" subject sequence.
   */
  {
    double value = 0.0;

    constexpr char text[] = "nan(payload";

    const auto result = tested::from_chars(text, text + 11, value);

    if (result.ec != tested::errc{} || result.ptr != text + 3 || value == value)
      return false;
  }

  /*
   * Special forms remain valid regardless of the
   * requested numeric notation.
   */
  {
    double value = 0.0;

    constexpr char text[] = "inf";

    const auto result = tested::from_chars(text, text + 3, value,
                                           tested::chars_format::scientific);

    if (result.ec != tested::errc{} || result.ptr != text + 3 ||
        value != tested::numeric_limits<double>::infinity())
      return false;
  }

  {
    double value = 0.0;

    constexpr char text[] = "nan";

    const auto result =
        tested::from_chars(text, text + 3, value, tested::chars_format::hex);

    if (result.ec != tested::errc{} || result.ptr != text + 3 || value == value)
      return false;
  }

  {
    double value = 123.0;

    constexpr char text[] = "+inf";

    const auto result = tested::from_chars(text, text + 4, value);

    if (result.ec != tested::errc::invalid_argument || result.ptr != text ||
        value != 123.0)
      return false;
  }

  {
    long double value = 99.0L;

    constexpr char text[] = "1.5tail";

    const auto result = tested::from_chars(text, text + 7, value);

    if (result.ec != tested::errc{} || result.ptr != text + 3 || value != 1.5L)
      return false;
  }

  {
    long double value = 99.0L;

    constexpr char text[] = "0.1";

    const auto result = tested::from_chars(text, text + 3, value);

    if (result.ec != tested::errc{} || result.ptr != text + 3 || value != 0.1L)
      return false;
  }

  {
    long double value = 99.0L;

    constexpr char text[] = "-1.25e3x";

    const auto result = tested::from_chars(text, text + 8, value,
                                           tested::chars_format::scientific);

    if (result.ec != tested::errc{} || result.ptr != text + 7 ||
        value != -1250.0L)
      return false;
  }

  {
    long double value = 99.0L;

    constexpr char text[] = "1.25e3";

    const auto result =
        tested::from_chars(text, text + 6, value, tested::chars_format::fixed);

    if (result.ec != tested::errc{} || result.ptr != text + 4 || value != 1.25L)
      return false;
  }

  {
    long double value = 99.0L;

    constexpr char text[] = "1.8p+1x";

    const auto result =
        tested::from_chars(text, text + 7, value, tested::chars_format::hex);

    if (result.ec != tested::errc{} || result.ptr != text + 6 || value != 3.0L)
      return false;
  }

  {
    long double value = 99.0L;

    char text[32]{};

    /*
     * Construct:
     *
     *   1p<minimum_significand_exponent>
     *
     * without relying on formatting facilities.
     */
    char *current = text;

    *current++ = '1';
    *current++ = 'p';

    int exponent = tested::numeric_limits<long double>::min_exponent -
                   tested::numeric_limits<long double>::digits;

    if (exponent < 0) {
      *current++ = '-';
      exponent = -exponent;
    }

    char reversed[16]{};
    unsigned count = 0;

    do {
      reversed[count++] = static_cast<char>('0' + exponent % 10);

      exponent /= 10;
    } while (exponent != 0);

    while (count != 0)
      *current++ = reversed[--count];

    const auto result =
        tested::from_chars(text, current, value, tested::chars_format::hex);

    if (result.ec != tested::errc{} || result.ptr != current ||
        value != tested::numeric_limits<long double>::denorm_min())
      return false;
  }

  {
    long double value = 0.0L;

    constexpr char text[] = "-INFINITY!";

    const auto result = tested::from_chars(text, text + 10, value);

    if (result.ec != tested::errc{} || result.ptr != text + 9 ||
        value != -tested::numeric_limits<long double>::infinity())
      return false;
  }

  {
    long double value = 0.0L;

    constexpr char text[] = "nan(payload)x";

    const auto result = tested::from_chars(text, text + 13, value);

    if (result.ec != tested::errc{} || result.ptr != text + 12)
      return false;

    if (value == value)
      return false;
  }

  {
    long double value = 99.0L;

    constexpr char text[] = "1e100000";

    const auto result = tested::from_chars(text, text + 8, value);

    if (result.ec != tested::errc::result_out_of_range ||
        result.ptr != text + 8 || value != 99.0L)
      return false;
  }

  {
    long double value = 99.0L;

    constexpr char text[] = "1e-100000";

    const auto result = tested::from_chars(text, text + 9, value);

    if (result.ec != tested::errc::result_out_of_range ||
        result.ptr != text + 9 || value != 99.0L)
      return false;
  }

  return true;
}

bool floating_to_chars_precision_tests() {
  char buffer[256]{};

  {
    auto result = tested::to_chars(buffer, buffer + 256, 1.25,
                                   tested::chars_format::fixed, 2);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "1.25"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 256, -1.5f,
                                   tested::chars_format::fixed, 3);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "-1.500"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 256, 1.5,
                                   tested::chars_format::scientific, 3);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.500e+00"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 256, 12345.0,
                                   tested::chars_format::general, 3);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.23e+04"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 256, 0.1,
                                   tested::chars_format::hex, 3);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.99ap-4"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 256, 1.5L,
                                   tested::chars_format::hex, 4);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.8000p+0"))
      return false;
  }

  /*
   * General precision zero means one significant digit.
   */
  {
    auto result = tested::to_chars(buffer, buffer + 256, 12.5,
                                   tested::chars_format::general, 0);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "1e+01"))
      return false;
  }

  /*
   * Negative precision is treated as omitted.
   */
  {
    auto result = tested::to_chars(buffer, buffer + 256, 1.25,
                                   tested::chars_format::fixed, -1);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.250000"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 256, 1.5,
                                   tested::chars_format::scientific, -1);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.500000e+00"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 256, 1.25,
                                   tested::chars_format::general, -1);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "1.25"))
      return false;
  }

  /*
   * Omitted hexadecimal precision is exact but does not retain redundant
   * trailing fractional zeroes.
   */
  {
    auto result = tested::to_chars(buffer, buffer + 256, 1.5,
                                   tested::chars_format::hex, -1);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.8p+0"))
      return false;
  }

  /*
   * This also verifies the omitted-hex implementation checks capacity
   * against the trimmed output, not its full scratch representation.
   */
  {
    char exact[6]{};

    auto result =
        tested::to_chars(exact, exact + 6, 1.5, tested::chars_format::hex, -1);

    if (result.ec != tested::errc{} || result.ptr != exact + 6 ||
        !equal_text(exact, result.ptr, "1.8p+0"))
      return false;
  }

  /*
   * %g capacity is checked against the final,
   * trailing-zero-trimmed representation.
   */
  {
    char exact[4]{};

    auto result = tested::to_chars(exact, exact + 4, 1.25,
                                   tested::chars_format::general, 6);

    if (result.ec != tested::errc{} || result.ptr != exact + 4 ||
        !equal_text(exact, result.ptr, "1.25"))
      return false;
  }

  /*
   * Same rule when rounding itself creates the
   * removable trailing zero.
   */
  {
    char exact[5]{};

    auto result = tested::to_chars(exact, exact + 5, 999.0,
                                   tested::chars_format::general, 2);

    if (result.ec != tested::errc{} || result.ptr != exact + 5 ||
        !equal_text(exact, result.ptr, "1e+03"))
      return false;
  }

  /*
   * Enormous precision does not imply enormous
   * output once %g trailing-zero suppression is
   * applied.
   */
  {
    char exact[4]{};

    auto result =
        tested::to_chars(exact, exact + 4, 1.25, tested::chars_format::general,
                         tested::numeric_limits<int>::max());

    if (result.ec != tested::errc{} || result.ptr != exact + 4 ||
        !equal_text(exact, result.ptr, "1.25"))
      return false;
  }

  /*
   * Standard failure contract: ptr == last and value_too_large.
   */
  {
    char tiny[3]{};

    auto result =
        tested::to_chars(tiny, tiny + 3, 1.25, tested::chars_format::fixed, 2);

    if (result.ptr != tiny + 3 || result.ec != tested::errc::value_too_large)
      return false;
  }

  return true;
}

bool floating_to_chars_shortest_tests() {
  char buffer[1024]{};

  /*
   * Ordinary shortest decimal.
   */
  {
    auto result = tested::to_chars(buffer, buffer + 1024, 0.1);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "0.1"))
      return false;

    double parsed = 0.0;
    auto round_trip = tested::from_chars(buffer, result.ptr, parsed);

    if (round_trip.ec != tested::errc{} || round_trip.ptr != result.ptr ||
        parsed != 0.1)
      return false;
  }

  /*
   * Fixed and scientific shortest formatting use the
   * same shortest decimal significand.
   */
  {
    auto result = tested::to_chars(buffer, buffer + 1024, 1.5,
                                   tested::chars_format::fixed);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "1.5"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 1024, 1.5,
                                   tested::chars_format::scientific);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.5e+00"))
      return false;
  }

  /*
   * General without explicit precision uses the
   * default %g selection precision of six.
   */
  {
    auto result = tested::to_chars(buffer, buffer + 1024, 100000.0,
                                   tested::chars_format::general);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "100000"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 1024, 1000000.0,
                                   tested::chars_format::general);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "1e+06"))
      return false;
  }

  /*
   * The no-format overload is deliberately different
   * from chars_format::general: it chooses the shorter
   * of the shortest fixed and scientific forms.
   */
  {
    auto result = tested::to_chars(buffer, buffer + 1024, 100000.0);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "1e+05"))
      return false;
  }

  /*
   * Here fixed beats scientific even though %g chooses
   * scientific because the exponent is well above six.
   */
  {
    constexpr double value = 123456789012345680.0;

    auto result = tested::to_chars(buffer, buffer + 1024, value);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "123456789012345680"))
      return false;
  }

  {
    constexpr double value = 123456789012345680.0;

    auto result = tested::to_chars(buffer, buffer + 1024, value,
                                   tested::chars_format::general);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.2345678901234568e+17"))
      return false;
  }

  /*
   * Classic shortest-roundtrip anomaly.
   *
   * 2^-44 exactly is:
   *
   *   5.684341886080801486968994140625e-14
   *
   * The nearest 16-digit decimal ends in ...801 and
   * does NOT round-trip. The next decimal, ...802, does.
   */
  {
    constexpr double value = 0x1p-44;

    auto result = tested::to_chars(buffer, buffer + 1024, value,
                                   tested::chars_format::scientific);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "5.684341886080802e-14"))
      return false;

    double parsed = 0.0;

    auto round_trip = tested::from_chars(buffer, result.ptr, parsed,
                                         tested::chars_format::scientific);

    if (round_trip.ec != tested::errc{} || round_trip.ptr != result.ptr ||
        parsed != value)
      return false;
  }

  {
    constexpr double value = 0x1p-44;

    auto result = tested::to_chars(buffer, buffer + 1024, value,
                                   tested::chars_format::fixed);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "0.00000000000005684341886080802"))
      return false;
  }

  /*
   * Smallest binary64 subnormal.
   */
  {
    const double value = tested::numeric_limits<double>::denorm_min();

    auto result = tested::to_chars(buffer, buffer + 1024, value);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "5e-324"))
      return false;

    double parsed = 0.0;

    auto round_trip = tested::from_chars(buffer, result.ptr, parsed);

    if (round_trip.ec != tested::errc{} || round_trip.ptr != result.ptr ||
        parsed != value)
      return false;
  }

  /*
   * Shortest hexadecimal is normalized even for
   * subnormals. It is not the anchored precision form
   * used by format_hexadecimal_precision().
   */
  {
    const double value = tested::numeric_limits<double>::denorm_min();

    auto result = tested::to_chars(buffer, buffer + 1024, value,
                                   tested::chars_format::hex);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1p-1074"))
      return false;

    double parsed = 0.0;

    auto round_trip = tested::from_chars(buffer, result.ptr, parsed,
                                         tested::chars_format::hex);

    if (round_trip.ec != tested::errc{} || round_trip.ptr != result.ptr ||
        parsed != value)
      return false;
  }

  {
    const float value = tested::numeric_limits<float>::denorm_min();

    auto result = tested::to_chars(buffer, buffer + 1024, value,
                                   tested::chars_format::hex);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1p-149"))
      return false;
  }

  {
    auto result =
        tested::to_chars(buffer, buffer + 1024, 1.5, tested::chars_format::hex);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.8p+0"))
      return false;
  }

  {
    auto result =
        tested::to_chars(buffer, buffer + 1024, 0.1, tested::chars_format::hex);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.999999999999ap-4"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 1024,
                                   tested::numeric_limits<double>::max(),
                                   tested::chars_format::hex);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.fffffffffffffp+1023"))
      return false;
  }

  /*
   * Signed zero must remain signed through the shortest
   * representation.
   */
  {
    auto result = tested::to_chars(buffer, buffer + 1024, -0.0);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "-0"))
      return false;

    double parsed = 1.0;

    auto round_trip = tested::from_chars(buffer, result.ptr, parsed);

    if (round_trip.ec != tested::errc{} || round_trip.ptr != result.ptr)
      return false;

    /*
     * IEEE signed zero: division distinguishes the sign
     * without pulling an internal FTL implementation header
     * into this public-header test.
     */
    if (1.0 / parsed != -tested::numeric_limits<double>::infinity())
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 1024, -0.0,
                                   tested::chars_format::hex);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "-0p+0"))
      return false;
  }

  /*
   * Long double goes through the same generic shortest
   * decimal engine on binary64, x87, and binary128.
   */
  {
    auto result = tested::to_chars(buffer, buffer + 1024, 1.5L);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "1.5"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 1024, 1.5L,
                                   tested::chars_format::scientific);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.5e+00"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 1024, 1.5L,
                                   tested::chars_format::hex);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.8p+0"))
      return false;
  }

  /*
   * Specials remain canonical and format-independent.
   */
  {
    auto result = tested::to_chars(buffer, buffer + 1024,
                                   tested::numeric_limits<double>::infinity());

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "inf"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 1024,
                                   -tested::numeric_limits<double>::infinity(),
                                   tested::chars_format::hex);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "-inf"))
      return false;
  }

  {
    auto result = tested::to_chars(buffer, buffer + 1024,
                                   tested::numeric_limits<double>::quiet_NaN());

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "nan"))
      return false;
  }

  /*
   * Buffer failure contract.
   */
  {
    char tiny[2]{};

    auto result = tested::to_chars(tiny, tiny + 2, 0.1);

    if (result.ptr != tiny + 2 || result.ec != tested::errc::value_too_large)
      return false;
  }

  {
    char tiny[5]{};

    auto result =
        tested::to_chars(tiny, tiny + 5, 1.5, tested::chars_format::scientific);

    if (result.ptr != tiny + 5 || result.ec != tested::errc::value_too_large)
      return false;
  }

  return true;
}

template <class Float> bool extended_floating_charconv_tests_for() {
  char buffer[256]{};

  /*
   * Shortest no-format conversion.
   */
  {
    const Float value = static_cast<Float>(1.5);

    const auto result = tested::to_chars(buffer, buffer + 256, value);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "1.5")) {
      return false;
    }

    Float parsed{};

    const auto round_trip = tested::from_chars(buffer, result.ptr, parsed);

    if (round_trip.ec != tested::errc{} || round_trip.ptr != result.ptr ||
        parsed != value) {
      return false;
    }
  }

  /*
   * Format-constrained shortest scientific form.
   */
  {
    const Float value = static_cast<Float>(1.5);

    const auto result = tested::to_chars(buffer, buffer + 256, value,
                                         tested::chars_format::scientific);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.5e+00")) {
      return false;
    }
  }

  /*
   * Precision overload and %g trimming.
   */
  {
    const Float value = static_cast<Float>(1.25);

    const auto result = tested::to_chars(buffer, buffer + 256, value,
                                         tested::chars_format::general, 6);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.25")) {
      return false;
    }
  }

  /*
   * Exact hexadecimal conversion.
   */
  {
    const Float value = static_cast<Float>(1.5);

    const auto result = tested::to_chars(buffer, buffer + 256, value,
                                         tested::chars_format::hex);

    if (result.ec != tested::errc{} ||
        !equal_text(buffer, result.ptr, "1.8p+0")) {
      return false;
    }

    Float parsed{};

    const auto round_trip = tested::from_chars(buffer, result.ptr, parsed,
                                               tested::chars_format::hex);

    if (round_trip.ec != tested::errc{} || round_trip.ptr != result.ptr ||
        parsed != value) {
      return false;
    }
  }

  /*
   * Smallest subnormal exercises the complete
   * exponent range and shortest normalized hex path.
   */
  {
    const Float value = tested::numeric_limits<Float>::denorm_min();

    const auto result = tested::to_chars(buffer, buffer + 256, value,
                                         tested::chars_format::hex);

    if (result.ec != tested::errc{})
      return false;

    Float parsed{};

    const auto round_trip = tested::from_chars(buffer, result.ptr, parsed,
                                               tested::chars_format::hex);

    if (round_trip.ec != tested::errc{} || round_trip.ptr != result.ptr ||
        parsed != value) {
      return false;
    }
  }

  /*
   * Decimal parsing through the type-specific path.
   */
  {
    Float value = static_cast<Float>(99.0);

    constexpr char text[] = "1.5";

    const auto result = tested::from_chars(text, text + 3, value);

    if (result.ec != tested::errc{} || result.ptr != text + 3 ||
        value != static_cast<Float>(1.5)) {
      return false;
    }
  }

  /*
   * Extreme decimal exponents must classify as range
   * errors without modifying the destination.
   */
  {
    const Float original = static_cast<Float>(1.5);
    Float value = original;

    constexpr char text[] = "1e100000";

    const auto result = tested::from_chars(text, text + 8, value);

    if (result.ec != tested::errc::result_out_of_range ||
        result.ptr != text + 8 || value != original) {
      return false;
    }
  }

  {
    const Float original = static_cast<Float>(1.5);
    Float value = original;

    constexpr char text[] = "1e-100000";

    const auto result = tested::from_chars(text, text + 9, value);

    if (result.ec != tested::errc::result_out_of_range ||
        result.ptr != text + 9 || value != original) {
      return false;
    }
  }

  /*
   * Specials exercise extended-type composition.
   */
  {
    const Float value = tested::numeric_limits<Float>::infinity();

    const auto result = tested::to_chars(buffer, buffer + 256, value);

    if (result.ec != tested::errc{} || !equal_text(buffer, result.ptr, "inf")) {
      return false;
    }

    Float parsed{};

    const auto round_trip = tested::from_chars(buffer, result.ptr, parsed);

    if (round_trip.ec != tested::errc{} || round_trip.ptr != result.ptr ||
        parsed != value) {
      return false;
    }
  }

  return true;
}

bool extended_floating_charconv_tests() {
#ifdef __STDCPP_FLOAT16_T__
  if (!extended_floating_charconv_tests_for<decltype(0.0f16)>())
    return false;
#endif

#ifdef __STDCPP_FLOAT32_T__
  if (!extended_floating_charconv_tests_for<decltype(0.0f32)>())
    return false;
#endif

#ifdef __STDCPP_FLOAT64_T__
  if (!extended_floating_charconv_tests_for<decltype(0.0f64)>())
    return false;
#endif

#ifdef __STDCPP_FLOAT128_T__
  if (!extended_floating_charconv_tests_for<decltype(0.0f128)>())
    return false;
#endif

#ifdef __STDCPP_BFLOAT16_T__
  if (!extended_floating_charconv_tests_for<decltype(0.0bf16)>())
    return false;
#endif

  return true;
}

bool ftl_test() {
  return floating_from_chars_tests() && floating_to_chars_precision_tests() &&
         floating_to_chars_shortest_tests() &&
         extended_floating_charconv_tests();
}
