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

template <class T>
concept has_integer_to_chars =
    requires(char* first, char* last, T value) {
  tested::to_chars(first, last, value);
    };

template <class T>
concept has_integer_from_chars =
    requires(const char* first, const char* last, T& value) {
  tested::from_chars(first, last, value);
    };

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
static_assert(!has_integer_to_chars<wchar_t>);
static_assert(!has_integer_to_chars<char8_t>);
static_assert(!has_integer_to_chars<char16_t>);
static_assert(!has_integer_to_chars<char32_t>);

static_assert(!has_integer_from_chars<bool>);
static_assert(!has_integer_from_chars<wchar_t>);
static_assert(!has_integer_from_chars<char8_t>);
static_assert(!has_integer_from_chars<char16_t>);
static_assert(!has_integer_from_chars<char32_t>);

static_assert(!has_integer_from_chars<const int>);
static_assert(!has_integer_from_chars<volatile int>);
static_assert(!has_integer_from_chars<const volatile int>);

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

  return true;
}

static_assert(integer_from_chars_tests());

static_assert(tested::to_chars_result{nullptr, tested::errc{}} ==
              tested::to_chars_result{nullptr, tested::errc{}});

static_assert(tested::from_chars_result{nullptr, tested::errc{}} ==
              tested::from_chars_result{nullptr, tested::errc{}});

bool ftl_test() { return true; }
