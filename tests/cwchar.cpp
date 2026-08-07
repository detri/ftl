#ifdef FTL_REPLACE_STL
#include <cwchar>
namespace tested = std;
#else
#include <ftl/cwchar>
namespace tested = ftl;
#endif

int test_vswprintf(wchar_t *destination, tested::size_t capacity,
                   const wchar_t *format, ...) {
  tested::va_list arguments;
  va_start(arguments, format);

  const int result =
      tested::vswprintf(destination, capacity, format, arguments);

  va_end(arguments);
  return result;
}

bool ftl_test() {
  // Foundational types/macros.
  {
    tested::mbstate_t state{};
    static_cast<void>(state);

    tested::wint_t eof = WEOF;

    if (eof == static_cast<tested::wint_t>(L'A')) {
      return false;
    }

    if (WCHAR_MIN > L'A' || WCHAR_MAX < L'A') {
      return false;
    }
  }

  // wcscpy.
  {
    wchar_t destination[8]{};

    if (tested::wcscpy(destination, L"ftl") != destination) {
      return false;
    }

    if (tested::wcscmp(destination, L"ftl") != 0) {
      return false;
    }
  }

  // wcsncpy: zero padding.
  {
    wchar_t destination[6] = {L'x', L'x', L'x', L'x', L'x', L'x'};

    if (tested::wcsncpy(destination, L"hi", 5) != destination) {
      return false;
    }

    if (destination[0] != L'h' || destination[1] != L'i' ||
        destination[2] != L'\0' || destination[3] != L'\0' ||
        destination[4] != L'\0' || destination[5] != L'x') {
      return false;
    }
  }

  // wcsncpy: truncation does not terminate.
  {
    wchar_t destination[4] = {L'x', L'x', L'x', L'x'};

    tested::wcsncpy(destination, L"abcdef", 3);

    if (destination[0] != L'a' || destination[1] != L'b' ||
        destination[2] != L'c' || destination[3] != L'x') {
      return false;
    }
  }

  // wmemcpy.
  {
    const wchar_t source[] = {L'a', L'b', L'c', L'd'};
    wchar_t destination[4]{};

    if (tested::wmemcpy(destination, source, 4) != destination) {
      return false;
    }

    if (tested::wmemcmp(destination, source, 4) != 0) {
      return false;
    }
  }

  // wmemmove: overlapping toward higher addresses.
  {
    wchar_t value[] = L"abcdef";

    tested::wmemmove(value + 2, value, 4);

    if (tested::wcscmp(value, L"ababcd") != 0) {
      return false;
    }
  }

  // wmemmove: overlapping toward lower addresses.
  {
    wchar_t value[] = L"abcdef";

    tested::wmemmove(value, value + 2, 4);

    if (tested::wcscmp(value, L"cdefef") != 0) {
      return false;
    }
  }

  // wcscat.
  {
    wchar_t value[16] = L"ft";

    if (tested::wcscat(value, L"l") != value) {
      return false;
    }

    if (tested::wcscmp(value, L"ftl") != 0) {
      return false;
    }
  }

  // wcsncat.
  {
    wchar_t value[16] = L"ab";

    if (tested::wcsncat(value, L"cdef", 2) != value) {
      return false;
    }

    if (tested::wcscmp(value, L"abcd") != 0) {
      return false;
    }
  }

  // Comparison.
  {
    if (tested::wcscmp(L"abc", L"abc") != 0) {
      return false;
    }

    if (tested::wcscmp(L"abc", L"abd") >= 0) {
      return false;
    }

    if (tested::wcscmp(L"abd", L"abc") <= 0) {
      return false;
    }

    if (tested::wcsncmp(L"abcdef", L"abcxyz", 3) != 0) {
      return false;
    }

    if (tested::wcsncmp(L"abcdef", L"abcxyz", 4) >= 0) {
      return false;
    }
  }

  // C-locale collation.
  {
    if (tested::wcscoll(L"abc", L"abd") >= 0) {
      return false;
    }

    if (tested::wcscoll(L"same", L"same") != 0) {
      return false;
    }
  }

  // C-locale transformation.
  {
    wchar_t transformed[8]{};

    if (tested::wcsxfrm(transformed, L"abc", 8) != 3) {
      return false;
    }

    if (tested::wcscmp(transformed, L"abc") != 0) {
      return false;
    }
  }

  // wmemcmp.
  {
    const wchar_t lower[] = {L'a', L'b'};
    const wchar_t equal[] = {L'a', L'b'};
    const wchar_t higher[] = {L'a', L'c'};

    if (tested::wmemcmp(lower, equal, 2) != 0) {
      return false;
    }

    if (tested::wmemcmp(lower, higher, 2) >= 0) {
      return false;
    }

    if (tested::wmemcmp(higher, lower, 2) <= 0) {
      return false;
    }
  }

  // Wide integer conversion.
  {
    wchar_t *end = nullptr;

    if (tested::wcstol(L"123", &end, 10) != 123) {
      return false;
    }

    if (end == nullptr || *end != L'\0') {
      return false;
    }
  }

  {
    const wchar_t input[] = L" \t-42xyz";
    wchar_t *end = nullptr;

    if (tested::wcstol(input, &end, 10) != -42) {
      return false;
    }

    if (end != input + 5 || *end != L'x') {
      return false;
    }
  }

  // Decimal floating conversion.
  {
    const wchar_t input[] = L"  -12.5e2tail";
    wchar_t *end = nullptr;

    const double value = tested::wcstod(input, &end);

    if (value != -1250.0) {
      return false;
    }

    if (end != input + 9 || *end != L't') {
      return false;
    }
  }

  // Fraction without an integer part.
  {
    wchar_t *end = nullptr;

    if (tested::wcstof(L".25", &end) != 0.25f) {
      return false;
    }

    if (end == nullptr || *end != L'\0') {
      return false;
    }
  }

  // Decimal exponent is consumed only when complete.
  {
    const wchar_t input[] = L"1e+";
    wchar_t *end = nullptr;

    if (tested::wcstod(input, &end) != 1.0) {
      return false;
    }

    if (end != input + 1 || *end != L'e') {
      return false;
    }
  }

  // Hexadecimal floating conversion.
  {
    const wchar_t input[] = L"0x1.8p2!";
    wchar_t *end = nullptr;

    if (tested::wcstod(input, &end) != 6.0) {
      return false;
    }

    if (end != input + 7 || *end != L'!') {
      return false;
    }
  }

  // The strtod/wcstod family permits hexadecimal input without
  // an explicit binary exponent.
  {
    wchar_t *end = nullptr;

    if (tested::wcstod(L"0x10", &end) != 16.0) {
      return false;
    }

    if (end == nullptr || *end != L'\0') {
      return false;
    }
  }

  // A failed hexadecimal prefix falls back to the valid
  // decimal prefix.
  {
    const wchar_t input[] = L"0x";
    wchar_t *end = nullptr;

    if (tested::wcstod(input, &end) != 0.0) {
      return false;
    }

    if (end != input + 1 || *end != L'x') {
      return false;
    }
  }

  // Infinity.
  {
    const wchar_t input[] = L"-INFINITY!";
    wchar_t *end = nullptr;

    const double value = tested::wcstod(input, &end);

    if (value != -tested::numeric_limits<double>::infinity()) {
      return false;
    }

    if (end != input + 9 || *end != L'!') {
      return false;
    }
  }

  {
    const wchar_t input[] = L"inf!";
    wchar_t *end = nullptr;

    if (tested::wcstod(input, &end) !=
        tested::numeric_limits<double>::infinity()) {
      return false;
    }

    if (end != input + 3 || *end != L'!') {
      return false;
    }
  }

  // NaN, including an implementation-defined payload.
  {
    const wchar_t input[] = L"NaN(ftl_23)!";
    wchar_t *end = nullptr;

    const double value = tested::wcstod(input, &end);

    if (value == value) {
      return false;
    }

    if (end != input + 11 || *end != L'!') {
      return false;
    }
  }

  // Malformed NAN(payload) is not accepted as a valid NAN
  // subject sequence.
  {
    const wchar_t input[] = L"nan(bad";
    wchar_t *end = nullptr;

    if (tested::wcstod(input, &end) != 0.0) {
      return false;
    }

    if (end != input) {
      return false;
    }
  }

  // No conversion.
  {
    const wchar_t input[] = L"xyz";
    wchar_t *end = nullptr;

    if (tested::wcstold(input, &end) != 0.0L) {
      return false;
    }

    if (end != input) {
      return false;
    }
  }

  // A mathematically zero significand remains zero regardless
  // of an absurd exponent.
  {
    wchar_t *end = nullptr;

    if (tested::wcstod(L"0.0e99999", &end) != 0.0) {
      return false;
    }

    if (end == nullptr || *end != L'\0') {
      return false;
    }
  }

  // Automatic base selection.
  {
    wchar_t *end = nullptr;

    if (tested::wcstol(L"077", &end, 0) != 63) {
      return false;
    }

    if (end == nullptr || *end != L'\0') {
      return false;
    }

    if (tested::wcstol(L"0x2a", &end, 0) != 42) {
      return false;
    }

    if (end == nullptr || *end != L'\0') {
      return false;
    }
  }

  // Explicit hexadecimal prefix.
  {
    wchar_t *end = nullptr;

    if (tested::wcstoll(L"0X7f!", &end, 16) != 127) {
      return false;
    }

    if (end == nullptr || *end != L'!') {
      return false;
    }
  }

  // Bases through 36.
  {
    wchar_t *end = nullptr;

    if (tested::wcstoull(L"z", &end, 36) != 35ull) {
      return false;
    }

    if (end == nullptr || *end != L'\0') {
      return false;
    }
  }

  // Unsigned conversion accepts a leading minus and negates
  // the converted value in the result type.
  {
    wchar_t *end = nullptr;

    if (tested::wcstoul(L"-1", &end, 10) != ~0ul) {
      return false;
    }

    if (end == nullptr || *end != L'\0') {
      return false;
    }
  }

  // No conversion leaves endptr at the original input.
  {
    const wchar_t input[] = L"xyz";
    wchar_t *end = nullptr;

    if (tested::wcstol(input, &end, 10) != 0) {
      return false;
    }

    if (end != input) {
      return false;
    }
  }

  // "0x" without a following hexadecimal digit consumes only
  // the leading zero under base autodetection.
  {
    const wchar_t input[] = L"0x";
    wchar_t *end = nullptr;

    if (tested::wcstol(input, &end, 0) != 0) {
      return false;
    }

    if (end != input + 1 || *end != L'x') {
      return false;
    }
  }

  // wcschr.
  {
    wchar_t mutable_value[] = L"banana";
    const wchar_t const_value[] = L"banana";

    wchar_t *mutable_result = tested::wcschr(mutable_value, L'n');
    const wchar_t *const_result = tested::wcschr(const_value, L'n');

    if (mutable_result != mutable_value + 2) {
      return false;
    }

    if (const_result != const_value + 2) {
      return false;
    }

    if (tested::wcschr(const_value, L'\0') != const_value + 6) {
      return false;
    }
  }

  // wcscspn.
  {
    if (tested::wcscspn(L"abc123", L"0123456789") != 3) {
      return false;
    }
  }

  // wcspbrk.
  {
    wchar_t mutable_value[] = L"hello";
    const wchar_t const_value[] = L"hello";

    if (tested::wcspbrk(mutable_value, L"xyzol") != mutable_value + 2) {
      return false;
    }

    if (tested::wcspbrk(const_value, L"xyzol") != const_value + 2) {
      return false;
    }

    if (tested::wcspbrk(const_value, L"xyz") != nullptr) {
      return false;
    }
  }

  // wcsrchr.
  {
    wchar_t mutable_value[] = L"banana";
    const wchar_t const_value[] = L"banana";

    if (tested::wcsrchr(mutable_value, L'a') != mutable_value + 5) {
      return false;
    }

    if (tested::wcsrchr(const_value, L'a') != const_value + 5) {
      return false;
    }

    if (tested::wcsrchr(const_value, L'\0') != const_value + 6) {
      return false;
    }
  }

  // wcsspn.
  {
    if (tested::wcsspn(L"aaab", L"a") != 3) {
      return false;
    }

    if (tested::wcsspn(L"abc123", L"abc") != 3) {
      return false;
    }
  }

  // wcsstr.
  {
    wchar_t mutable_value[] = L"banana";
    const wchar_t const_value[] = L"banana";

    if (tested::wcsstr(mutable_value, L"ana") != mutable_value + 1) {
      return false;
    }

    if (tested::wcsstr(const_value, L"ana") != const_value + 1) {
      return false;
    }

    if (tested::wcsstr(const_value, L"") != const_value) {
      return false;
    }

    if (tested::wcsstr(const_value, L"xyz") != nullptr) {
      return false;
    }
  }

  // wcstok.
  {
    wchar_t value[] = L"::alpha:beta::gamma";
    wchar_t *context = nullptr;

    wchar_t *token = tested::wcstok(value, L":", &context);

    if (token == nullptr || tested::wcscmp(token, L"alpha") != 0) {
      return false;
    }

    token = tested::wcstok(nullptr, L":", &context);

    if (token == nullptr || tested::wcscmp(token, L"beta") != 0) {
      return false;
    }

    token = tested::wcstok(nullptr, L":", &context);

    if (token == nullptr || tested::wcscmp(token, L"gamma") != 0) {
      return false;
    }

    if (tested::wcstok(nullptr, L":", &context) != nullptr) {
      return false;
    }
  }

  // wmemchr.
  {
    wchar_t mutable_value[] = {L'a', L'b', L'c', L'd'};
    const wchar_t const_value[] = {L'a', L'b', L'c', L'd'};

    if (tested::wmemchr(mutable_value, L'c', 4) != mutable_value + 2) {
      return false;
    }

    if (tested::wmemchr(const_value, L'c', 4) != const_value + 2) {
      return false;
    }

    if (tested::wmemchr(const_value, L'x', 4) != nullptr) {
      return false;
    }
  }

  // wcslen.
  {
    if (tested::wcslen(L"") != 0) {
      return false;
    }

    if (tested::wcslen(L"ftl") != 3) {
      return false;
    }
  }

  // wmemset.
  {
    wchar_t value[5]{};

    if (tested::wmemset(value, L'x', 5) != value) {
      return false;
    }

    for (wchar_t character : value) {
      if (character != L'x') {
        return false;
      }
    }
  }

  // Single-byte/wide conversion in FTL's C locale.
  {
    if (tested::btowc('A') != static_cast<tested::wint_t>(L'A')) {
      return false;
    }

    if (tested::btowc(0x80) != static_cast<tested::wint_t>(WEOF)) {
      return false;
    }

    if (tested::wctob(static_cast<tested::wint_t>(L'A')) != 'A') {
      return false;
    }

    if (tested::wctob(static_cast<tested::wint_t>(0x80)) != -1) {
      return false;
    }
  }

  // Initial conversion state.
  {
    tested::mbstate_t state{};

    if (tested::mbsinit(&state) == 0) {
      return false;
    }

    if (tested::mbsinit(nullptr) == 0) {
      return false;
    }
  }

  // mbrtowc.
  {
    tested::mbstate_t state{};
    wchar_t result = L'\0';

    if (tested::mbrtowc(&result, "A", 1, &state) != 1) {
      return false;
    }

    if (result != L'A') {
      return false;
    }

    if (tested::mbsinit(&state) == 0) {
      return false;
    }
  }

  // Null character conversion returns zero.
  {
    tested::mbstate_t state{};
    wchar_t result = L'x';

    if (tested::mbrtowc(&result, "", 1, &state) != 0) {
      return false;
    }

    if (result != L'\0') {
      return false;
    }
  }

  // Zero bytes available means potentially incomplete.
  {
    tested::mbstate_t state{};

    if (tested::mbrtowc(nullptr, "A", 0, &state) !=
        static_cast<tested::size_t>(-2)) {
      return false;
    }
  }

  // Non-C-locale byte is an encoding error.
  {
    tested::mbstate_t state{};

    const char invalid[] = {static_cast<char>(0x80), '\0'};

    if (tested::mbrtowc(nullptr, invalid, 1, &state) !=
        static_cast<tested::size_t>(-1)) {
      return false;
    }

    if (tested::mbsinit(&state) == 0) {
      return false;
    }
  }

  // mbrlen.
  {
    tested::mbstate_t state{};

    if (tested::mbrlen("A", 1, &state) != 1) {
      return false;
    }

    if (tested::mbrlen("", 1, &state) != 0) {
      return false;
    }

    if (tested::mbrlen("A", 0, &state) != static_cast<tested::size_t>(-2)) {
      return false;
    }
  }

  // wcrtomb.
  {
    tested::mbstate_t state{};
    char output[2]{};

    if (tested::wcrtomb(output, L'A', &state) != 1) {
      return false;
    }

    if (output[0] != 'A') {
      return false;
    }

    if (tested::mbsinit(&state) == 0) {
      return false;
    }
  }

  // wcrtomb rejects characters outside the C-locale
  // execution character set.
  {
    tested::mbstate_t state{};
    char output[2]{};

    if (tested::wcrtomb(output, static_cast<wchar_t>(0x80), &state) !=
        static_cast<tested::size_t>(-1)) {
      return false;
    }
  }

  // Null destination resets wcrtomb state and returns the
  // length of the null conversion.
  {
    tested::mbstate_t state{};

    state.state = 1;
    state.value = 123;

    if (tested::wcrtomb(nullptr, L'x', &state) != 1) {
      return false;
    }

    if (tested::mbsinit(&state) == 0) {
      return false;
    }
  }

  // mbsrtowcs.
  {
    const char text[] = "ftl";
    const char *source = text;
    wchar_t output[4]{};
    tested::mbstate_t state{};

    if (tested::mbsrtowcs(output, &source, 4, &state) != 3) {
      return false;
    }

    if (source != nullptr) {
      return false;
    }

    if (tested::wcscmp(output, L"ftl") != 0) {
      return false;
    }
  }

  // mbsrtowcs stops when destination capacity is exhausted.
  {
    const char text[] = "abcd";
    const char *source = text;
    wchar_t output[2]{};
    tested::mbstate_t state{};

    if (tested::mbsrtowcs(output, &source, 2, &state) != 2) {
      return false;
    }

    if (source != text + 2) {
      return false;
    }

    if (output[0] != L'a' || output[1] != L'b') {
      return false;
    }
  }

  // mbsrtowcs sizing pass does not move source.
  {
    const char text[] = "ftl";
    const char *source = text;
    tested::mbstate_t state{};

    if (tested::mbsrtowcs(nullptr, &source, 0, &state) != 3) {
      return false;
    }

    if (source != text) {
      return false;
    }
  }

  // wcsrtombs.
  {
    const wchar_t text[] = L"ftl";
    const wchar_t *source = text;
    char output[4]{};
    tested::mbstate_t state{};

    if (tested::wcsrtombs(output, &source, 4, &state) != 3) {
      return false;
    }

    if (source != nullptr) {
      return false;
    }

    if (output[0] != 'f' || output[1] != 't' || output[2] != 'l' ||
        output[3] != '\0') {
      return false;
    }
  }

  // wcsrtombs stops at byte capacity.
  {
    const wchar_t text[] = L"abcd";
    const wchar_t *source = text;
    char output[2]{};
    tested::mbstate_t state{};

    if (tested::wcsrtombs(output, &source, 2, &state) != 2) {
      return false;
    }

    if (source != text + 2) {
      return false;
    }

    if (output[0] != 'a' || output[1] != 'b') {
      return false;
    }
  }

  // wcsrtombs sizing pass does not move source.
  {
    const wchar_t text[] = L"ftl";
    const wchar_t *source = text;
    tested::mbstate_t state{};

    if (tested::wcsrtombs(nullptr, &source, 0, &state) != 3) {
      return false;
    }

    if (source != text) {
      return false;
    }
  }

  // wcsftime basic calendar and clock fields.
  {
    tested::tm value{};

    value.tm_sec = 6;
    value.tm_min = 5;
    value.tm_hour = 16;
    value.tm_mday = 1;
    value.tm_mon = 0;
    value.tm_year = 121; // 2021
    value.tm_wday = 5;   // Friday
    value.tm_yday = 0;

    wchar_t output[128]{};

    const auto length =
        tested::wcsftime(output, 128, L"%Y-%m-%d %H:%M:%S", &value);

    if (length != 19) {
      return false;
    }

    if (tested::wcscmp(output, L"2021-01-01 16:05:06") != 0) {
      return false;
    }
  }

  // C-locale names and 12-hour clock.
  {
    tested::tm value{};

    value.tm_sec = 6;
    value.tm_min = 5;
    value.tm_hour = 16;
    value.tm_mday = 1;
    value.tm_mon = 0;
    value.tm_year = 121;
    value.tm_wday = 5;
    value.tm_yday = 0;

    wchar_t output[128]{};

    tested::wcsftime(output, 128, L"%a|%A|%b|%B|%I|%p", &value);

    if (tested::wcscmp(output, L"Fri|Friday|Jan|January|04|PM") != 0) {
      return false;
    }
  }

  // Composite C-locale formats.
  {
    tested::tm value{};

    value.tm_sec = 6;
    value.tm_min = 5;
    value.tm_hour = 16;
    value.tm_mday = 1;
    value.tm_mon = 0;
    value.tm_year = 121;
    value.tm_wday = 5;
    value.tm_yday = 0;

    wchar_t output[256]{};

    tested::wcsftime(output, 256, L"%c|%x|%X|%D|%F|%r|%R|%T", &value);

    if (tested::wcscmp(output, L"Fri Jan  1 16:05:06 2021|"
                               L"01/01/21|"
                               L"16:05:06|"
                               L"01/01/21|"
                               L"2021-01-01|"
                               L"04:05:06 PM|"
                               L"16:05|"
                               L"16:05:06") != 0) {
      return false;
    }
  }

  // ISO week-year boundary:
  // 2021-01-01 belongs to ISO week 53 of 2020.
  {
    tested::tm value{};

    value.tm_mday = 1;
    value.tm_mon = 0;
    value.tm_year = 121;
    value.tm_wday = 5;
    value.tm_yday = 0;

    wchar_t output[64]{};

    tested::wcsftime(output, 64, L"%G-W%V-%u %g", &value);

    if (tested::wcscmp(output, L"2020-W53-5 20") != 0) {
      return false;
    }
  }

  // Ordinal day and conventional week numbers.
  {
    tested::tm value{};

    value.tm_mday = 1;
    value.tm_mon = 0;
    value.tm_year = 121;
    value.tm_wday = 5;
    value.tm_yday = 0;

    wchar_t output[64]{};

    tested::wcsftime(output, 64, L"%j %U %W %u %w", &value);

    if (tested::wcscmp(output, L"001 00 00 5 5") != 0) {
      return false;
    }
  }

  // Leap-day ordinal.
  {
    tested::tm value{};

    value.tm_mday = 29;
    value.tm_mon = 1;
    value.tm_year = 124; // 2024
    value.tm_wday = 4;   // Thursday
    value.tm_yday = 59;

    wchar_t output[64]{};

    tested::wcsftime(output, 64, L"%F %j", &value);

    if (tested::wcscmp(output, L"2024-02-29 060") != 0) {
      return false;
    }
  }

  // E/O modifiers reduce to their ordinary C-locale forms.
  {
    tested::tm value{};

    value.tm_mday = 9;
    value.tm_mon = 7;
    value.tm_year = 126;
    value.tm_wday = 0;
    value.tm_yday = 220;

    wchar_t output[64]{};

    tested::wcsftime(output, 64, L"%EY %Om %Od", &value);

    if (tested::wcscmp(output, L"2026 08 09") != 0) {
      return false;
    }
  }

  // No timezone information is available in FTL's portable tm.
  {
    tested::tm value{};
    wchar_t output[16]{};

    tested::wcsftime(output, 16, L"A%zB%ZC", &value);

    if (tested::wcscmp(output, L"ABC") != 0) {
      return false;
    }
  }

  // Newline, tab, and literal percent.
  {
    tested::tm value{};
    wchar_t output[16]{};

    tested::wcsftime(output, 16, L"a%n%tb%%", &value);

    if (tested::wcscmp(output, L"a\n\tb%") != 0) {
      return false;
    }
  }

  // Insufficient destination capacity returns zero.
  {
    tested::tm value{};

    value.tm_year = 126;
    value.tm_mon = 7;
    value.tm_mday = 7;

    wchar_t output[5]{};

    if (tested::wcsftime(output, 5, L"%F", &value) != 0) {
      return false;
    }
  }

  // swprintf ordinary output and percent escape.
  {
    wchar_t output[64]{};

    const int result = tested::swprintf(output, 64, L"hello %% %d", 42);

    if (result != 10) {
      return false;
    }

    if (tested::wcscmp(output, L"hello % 42") != 0) {
      return false;
    }
  }

  // Signed flags and field width.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"|%+6d|%-6d|% 6d|", 42, 42, 42);

    if (tested::wcscmp(output, L"|   +42|42    |    42|") != 0) {
      return false;
    }
  }

  // Zero padding occurs after the sign.
  {
    wchar_t output[32]{};

    tested::swprintf(output, 32, L"%+06d", 42);

    if (tested::wcscmp(output, L"+00042") != 0) {
      return false;
    }
  }

  // Integer precision.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"|%.5d|%.0d|", 42, 0);

    if (tested::wcscmp(output, L"|00042||") != 0) {
      return false;
    }
  }

  // Explicit integer precision disables zero field padding.
  {
    wchar_t output[32]{};

    tested::swprintf(output, 32, L"%08.5d", 42);

    if (tested::wcscmp(output, L"   00042") != 0) {
      return false;
    }
  }

  // Alternate octal and hexadecimal.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%#o %#x %#X", 9u, 42u, 42u);

    if (tested::wcscmp(output, L"011 0x2a 0X2A") != 0) {
      return false;
    }
  }

  // %#.0o must still produce one zero.
  {
    wchar_t output[16]{};

    tested::swprintf(output, 16, L"%#.0o", 0u);

    if (tested::wcscmp(output, L"0") != 0) {
      return false;
    }
  }

  // Dynamic width and precision.
  {
    wchar_t output[32]{};

    tested::swprintf(output, 32, L"%*.*d", -8, 4, 23);

    if (tested::wcscmp(output, L"0023    ") != 0) {
      return false;
    }
  }

  // Integer length modifiers.
  {
    wchar_t output[128]{};

    const signed char hh = -12;
    const short h = -123;
    const long l = -1234;
    const long long ll = -12345;

    tested::swprintf(output, 128, L"%hhd %hd %ld %lld", hh, h, l, ll);

    if (tested::wcscmp(output, L"-12 -123 -1234 -12345") != 0) {
      return false;
    }
  }

  // Narrow and wide character conversion.
  {
    wchar_t output[32]{};

    tested::swprintf(output, 32, L"%c %lc", 'A',
                     static_cast<tested::wint_t>(L'Z'));

    if (tested::wcscmp(output, L"A Z") != 0) {
      return false;
    }
  }

  // Narrow and wide strings.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"|%6s|%-6ls|", "ftl", L"wide");

    if (tested::wcscmp(output, L"|   ftl|wide  |") != 0) {
      return false;
    }
  }

  // String precision counts output wide characters.
  {
    wchar_t output[32]{};

    tested::swprintf(output, 32, L"%.2s %.3ls", "ftl", L"wide");

    if (tested::wcscmp(output, L"ft wid") != 0) {
      return false;
    }
  }

  // %n stores output count without producing characters.
  {
    wchar_t output[32]{};
    int count = -1;

    tested::swprintf(output, 32, L"abc%nxyz", &count);

    if (count != 3) {
      return false;
    }

    if (tested::wcscmp(output, L"abcxyz") != 0) {
      return false;
    }
  }

  // Pointer representation is deliberately FTL-defined.
  {
    wchar_t output[64]{};
    int value = 0;

    if (tested::swprintf(output, 64, L"%p", static_cast<void *>(&value)) <= 2) {
      return false;
    }

    if (output[0] != L'0' || output[1] != L'x') {
      return false;
    }
  }

  // vswprintf follows the same engine.
  {
    wchar_t output[64]{};

    const int result = test_vswprintf(output, 64, L"%04x %ls", 42u, L"ftl");

    if (result != 8) {
      return false;
    }

    if (tested::wcscmp(output, L"002a ftl") != 0) {
      return false;
    }
  }

  // Insufficient destination space is a formatting failure,
  // not snprintf-style length reporting.
  {
    wchar_t output[4]{};

    if (tested::swprintf(output, 4, L"abcdef") >= 0) {
      return false;
    }
  }

  // %f default precision.
  {
    wchar_t output[64]{};

    if (tested::swprintf(output, 64, L"%f", 1.5) != 8) {
      return false;
    }

    if (tested::wcscmp(output, L"1.500000") != 0) {
      return false;
    }
  }

  // Explicit fixed precision.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%.2f", 12.345);

    if (tested::wcscmp(output, L"12.35") != 0) {
      return false;
    }
  }

  // Exact halfway values use round-to-nearest-even.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%.0f %.0f", 2.5, 3.5);

    if (tested::wcscmp(output, L"2 4") != 0) {
      return false;
    }
  }

  // Rounding can carry into a new integer digit.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%.2f", 9.999);

    if (tested::wcscmp(output, L"10.00") != 0) {
      return false;
    }
  }

  // Fractional leading zeroes.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%.4f", 0.00125);

    if (tested::wcscmp(output, L"0.0013") != 0) {
      return false;
    }
  }

  // Alternate form forces the radix point.
  {
    wchar_t output[32]{};

    tested::swprintf(output, 32, L"%#.0f", 2.0);

    if (tested::wcscmp(output, L"2.") != 0) {
      return false;
    }
  }

  // Sign and zero field padding.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%+010.2f", 12.5);

    if (tested::wcscmp(output, L"+000012.50") != 0) {
      return false;
    }
  }

  // Left alignment disables zero padding.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%-010.2f", 12.5);

    if (tested::wcscmp(output, L"12.50     ") != 0) {
      return false;
    }
  }

  // Negative zero retains its sign.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%.1f", -0.0);

    if (tested::wcscmp(output, L"-0.0") != 0) {
      return false;
    }
  }

  // Infinity and NaN.
  {
    wchar_t output[64]{};

    const double infinity = tested::numeric_limits<double>::infinity();

    const double nan = tested::numeric_limits<double>::quiet_NaN();

    tested::swprintf(output, 64, L"%f %F", infinity, nan);

    if (tested::wcscmp(output, L"inf NAN") != 0) {
      return false;
    }
  }

  // Signs apply to non-finite signed conversions too.
  {
    wchar_t output[64]{};

    const double infinity = tested::numeric_limits<double>::infinity();

    tested::swprintf(output, 64, L"%+f", infinity);

    if (tested::wcscmp(output, L"+inf") != 0) {
      return false;
    }
  }

  // Long double.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%.3Lf", static_cast<long double>(1.25L));

    if (tested::wcscmp(output, L"1.250") != 0) {
      return false;
    }
  }

  // Small values round cleanly to zero.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%.3f", 0.0001);

    if (tested::wcscmp(output, L"0.000") != 0) {
      return false;
    }
  }

  // Scientific notation.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%e", 1.25);

    if (tested::wcscmp(output, L"1.250000e+00") != 0) {
      return false;
    }
  }

  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%.2E", 1234.0);

    if (tested::wcscmp(output, L"1.23E+03") != 0) {
      return false;
    }
  }

  // Scientific rounding can advance the exponent.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%.2e", 9.999);

    if (tested::wcscmp(output, L"1.00e+01") != 0) {
      return false;
    }
  }

  // %g removes insignificant trailing zeroes.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%g", 123.45);

    if (tested::wcscmp(output, L"123.45") != 0) {
      return false;
    }
  }

  // %g switches to scientific notation.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%g", 1234567.0);

    if (tested::wcscmp(output, L"1.23457e+06") != 0) {
      return false;
    }
  }

  // Exponent -4 remains fixed.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%.4g", 0.0001234);

    if (tested::wcscmp(output, L"0.0001234") != 0) {
      return false;
    }
  }

  // Below -4 switches to scientific.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%.4g", 0.00001234);

    if (tested::wcscmp(output, L"1.234e-05") != 0) {
      return false;
    }
  }

  // Alternate %g retains trailing zeroes.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%#.4g", 12.0);

    if (tested::wcscmp(output, L"12.00") != 0) {
      return false;
    }
  }

  // Hexadecimal floating notation.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%.1a", 1.5);

    if (tested::wcscmp(output, L"0x1.8p+0") != 0) {
      return false;
    }
  }

  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%.2A", 10.5);

    if (tested::wcscmp(output, L"0X1.50P+3") != 0) {
      return false;
    }
  }

  // Alternate hexadecimal form forces the radix point.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%#.0a", 1.0);

    if (tested::wcscmp(output, L"0x1.p+0") != 0) {
      return false;
    }
  }

  // Hexadecimal halfway rounding.
  {
    wchar_t output[64]{};

    tested::swprintf(output, 64, L"%.0a", 1.5);

    if (tested::wcscmp(output, L"0x2p+0") != 0) {
      return false;
    }
  }

  return true;
}