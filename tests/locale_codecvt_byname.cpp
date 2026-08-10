#ifdef FTL_REPLACE_STL
#include <locale>
#include <string>
namespace tested = std;
#else
#include <ftl/locale>
#include <ftl/string>
namespace tested = ftl;
#endif

bool ftl_test() {
  const tested::locale &classic = tested::locale::classic();

  using char_codec = tested::codecvt<char, char, tested::mbstate_t>;

  using utf16_u8 = tested::codecvt<char16_t, char8_t, tested::mbstate_t>;

  using utf32_u8 = tested::codecvt<char32_t, char8_t, tested::mbstate_t>;

  using wide_codec = tested::codecvt<wchar_t, char, tested::mbstate_t>;

  using deprecated_utf16 = tested::codecvt<char16_t, char, tested::mbstate_t>;

  using deprecated_utf32 = tested::codecvt<char32_t, char, tested::mbstate_t>;

  //
  // C++23 still requires these two deprecated classic facets.
  //
  if (!tested::has_facet<deprecated_utf16>(classic)) {
    return false;
  }

  if (!tested::has_facet<deprecated_utf32>(classic)) {
    return false;
  }

  //
  // Deprecated UTF-32 <-> UTF-8-in-char.
  //
  {
    const auto &facet = tested::use_facet<deprecated_utf32>(classic);

    const char32_t source[] = {U'A', static_cast<char32_t>(0x1f600)};

    char encoded[8]{};

    tested::mbstate_t state{};

    const char32_t *from_next = nullptr;
    char *to_next = nullptr;

    if (facet.out(state, source, source + 2, from_next, encoded, encoded + 8,
                  to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (from_next != source + 2 || to_next != encoded + 5) {
      return false;
    }

    if (encoded[0] != 'A' || static_cast<unsigned char>(encoded[1]) != 0xf0 ||
        static_cast<unsigned char>(encoded[2]) != 0x9f ||
        static_cast<unsigned char>(encoded[3]) != 0x98 ||
        static_cast<unsigned char>(encoded[4]) != 0x80) {
      return false;
    }

    char32_t roundtrip[2]{};

    const char *external_next = nullptr;
    char32_t *internal_next = nullptr;

    state = {};

    if (facet.in(state, encoded, encoded + 5, external_next, roundtrip,
                 roundtrip + 2, internal_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (roundtrip[0] != U'A' ||
        roundtrip[1] != static_cast<char32_t>(0x1f600)) {
      return false;
    }
  }

  //
  // Deprecated UTF-16 <-> UTF-8-in-char.
  //
  {
    const auto &facet = tested::use_facet<deprecated_utf16>(classic);

    const char16_t source[] = {static_cast<char16_t>(0xd83d),
                               static_cast<char16_t>(0xde00)};

    char encoded[4]{};

    tested::mbstate_t state{};

    const char16_t *from_next = nullptr;
    char *to_next = nullptr;

    if (facet.out(state, source, source + 2, from_next, encoded, encoded + 4,
                  to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (from_next != source + 2 || to_next != encoded + 4) {
      return false;
    }
  }

  //
  // Named char/char remains a degenerate conversion.
  //
  {
    tested::locale value(
        classic,
        new tested::codecvt_byname<char, char, tested::mbstate_t>("C"));

    const auto &facet = tested::use_facet<char_codec>(value);

    if (!facet.always_noconv())
      return false;

    if (facet.encoding() != 1)
      return false;
  }

  //
  // Fixed UTF facets keep the same Unicode semantics under byname.
  //
  {
    tested::locale value(
        classic,
        new tested::codecvt_byname<char32_t, char8_t, tested::mbstate_t>("C"));

    const auto &facet = tested::use_facet<utf32_u8>(value);

    const char32_t source[] = {static_cast<char32_t>(0x1f600)};

    char8_t encoded[4]{};

    tested::mbstate_t state{};

    const char32_t *from_next = nullptr;
    char8_t *to_next = nullptr;

    if (facet.out(state, source, source + 1, from_next, encoded, encoded + 4,
                  to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (to_next != encoded + 4)
      return false;
  }

  {
    tested::string name{"C"};

    tested::locale value(
        classic,
        new tested::codecvt_byname<char16_t, char8_t, tested::mbstate_t>(name));

    const auto &facet = tested::use_facet<utf16_u8>(value);

    const char8_t source[] = {static_cast<char8_t>(0xc2),
                              static_cast<char8_t>(0xa2)};

    char16_t output[1]{};

    tested::mbstate_t state{};

    const char8_t *from_next = nullptr;
    char16_t *to_next = nullptr;

    if (facet.in(state, source, source + 2, from_next, output, output + 1,
                 to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (output[0] != static_cast<char16_t>(0x00a2)) {
      return false;
    }
  }

  //
  // Named native wchar conversion.
  //
  {
    tested::locale value(
        classic,
        new tested::codecvt_byname<wchar_t, char, tested::mbstate_t>("C"));

    const auto &facet = tested::use_facet<wide_codec>(value);

    //
    // The C locale is a fixed one-byte native encoding.
    //
    if (facet.encoding() != 1)
      return false;

    if (facet.max_length() != 1)
      return false;

    const char source[] = "Az7";

    wchar_t wide[3]{};

    tested::mbstate_t state{};

    const char *from_next = nullptr;
    wchar_t *to_next = nullptr;

    if (facet.in(state, source, source + 3, from_next, wide, wide + 3,
                 to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (wide[0] != L'A' || wide[1] != L'z' || wide[2] != L'7') {
      return false;
    }

    char roundtrip[3]{};

    const wchar_t *wide_next = nullptr;
    char *external_next = nullptr;

    state = {};

    if (facet.out(state, wide, wide + 3, wide_next, roundtrip, roundtrip + 3,
                  external_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (roundtrip[0] != 'A' || roundtrip[1] != 'z' || roundtrip[2] != '7') {
      return false;
    }

#if defined(__APPLE__)

    //
    // Darwin's C locale uses its stateless "NONE" encoding, in which
    // every nonzero byte is a valid one-byte character.
    //
    const char extended[] = {static_cast<char>(0xff)};

    wchar_t extended_output[1]{};

    state = {};

    from_next = nullptr;
    to_next = nullptr;

    if (facet.in(state, extended, extended + 1, from_next, extended_output,
                 extended_output + 1, to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (from_next != extended + 1 || to_next != extended_output + 1 ||
        extended_output[0] != static_cast<wchar_t>(0xff)) {
      return false;
    }

    state = {};

    if (facet.length(state, extended, extended + 1, 1) != 1) {
      return false;
    }

#elif !defined(_WIN32)

    //
    // The glibc C locale rejects bytes outside its native
    // single-byte character repertoire.
    //
    const char invalid[] = {static_cast<char>(0xff)};

    wchar_t invalid_output[1]{};

    state = {};

    from_next = nullptr;
    to_next = nullptr;

    if (facet.in(state, invalid, invalid + 1, from_next, invalid_output,
                 invalid_output + 1, to_next) != tested::codecvt_base::error) {
      return false;
    }

    if (from_next != invalid || to_next != invalid_output) {
      return false;
    }

    state = {};

    if (facet.length(state, invalid, invalid + 1, 1) != 0) {
      return false;
    }

#endif
  }

  //
  // The required environment-selected locale spelling.
  //
  {
    tested::locale value(
        classic,
        new tested::codecvt_byname<wchar_t, char, tested::mbstate_t>(""));

    const auto &facet = tested::use_facet<wide_codec>(value);

    const char source[] = "A";

    wchar_t output[1]{};

    tested::mbstate_t state{};

    const char *from_next = nullptr;
    wchar_t *to_next = nullptr;

    if (facet.in(state, source, source + 1, from_next, output, output + 1,
                 to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (output[0] != L'A')
      return false;
  }

  //
  // The deprecated byname UTF facets are also required.
  //
  {
    tested::locale value(
        classic,
        new tested::codecvt_byname<char32_t, char, tested::mbstate_t>("C"));

    const auto &facet = tested::use_facet<deprecated_utf32>(value);

    const char32_t source[] = {U'\u00a2'};
    char output[2]{};

    tested::mbstate_t state{};

    const char32_t *from_next = nullptr;
    char *to_next = nullptr;

    if (facet.out(state, source, source + 1, from_next, output, output + 2,
                  to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (static_cast<unsigned char>(output[0]) != 0xc2 ||
        static_cast<unsigned char>(output[1]) != 0xa2) {
      return false;
    }
  }

  return true;
}
