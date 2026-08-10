#ifdef FTL_REPLACE_STL
#include <locale>
namespace tested = std;
#else
#include <ftl/locale>
namespace tested = ftl;
#endif

bool ftl_test() {
  const tested::locale &classic = tested::locale::classic();

  using identity = tested::codecvt<char, char, tested::mbstate_t>;

  using utf16 = tested::codecvt<char16_t, char8_t, tested::mbstate_t>;

  using utf32 = tested::codecvt<char32_t, char8_t, tested::mbstate_t>;

  using wide = tested::codecvt<wchar_t, char, tested::mbstate_t>;

  if (!tested::has_facet<identity>(classic))
    return false;

  if (!tested::has_facet<utf16>(classic))
    return false;

  if (!tested::has_facet<utf32>(classic))
    return false;

  if (!tested::has_facet<wide>(classic))
    return false;

  //
  // Identity conversion.
  //
  {
    const auto &facet = tested::use_facet<identity>(classic);

    if (!facet.always_noconv())
      return false;

    if (facet.encoding() != 1)
      return false;

    if (facet.max_length() != 1)
      return false;

    tested::mbstate_t state{};

    const char source[] = "abc";

    if (facet.length(state, source, source + 3, 2) != 2) {
      return false;
    }

    const char *from_next = nullptr;
    char output[3]{};
    char *to_next = nullptr;

    if (facet.out(state, source, source + 3, from_next, output, output + 3,
                  to_next) != tested::codecvt_base::noconv) {
      return false;
    }

    if (to_next != output)
      return false;
  }

  //
  // UTF-32 -> UTF-8 -> UTF-32.
  //
  {
    const auto &facet = tested::use_facet<utf32>(classic);

    if (facet.always_noconv())
      return false;

    if (facet.encoding() != 0)
      return false;

    if (facet.max_length() != 4)
      return false;

    const char32_t source[] = {U'A', static_cast<char32_t>(0x1f600)};

    char8_t encoded[8]{};

    tested::mbstate_t state{};

    const char32_t *from_next = nullptr;
    char8_t *to_next = nullptr;

    if (facet.out(state, source, source + 2, from_next, encoded, encoded + 8,
                  to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (from_next != source + 2)
      return false;

    if (to_next != encoded + 5)
      return false;

    if (encoded[0] != u8'A' || static_cast<unsigned char>(encoded[1]) != 0xf0 ||
        static_cast<unsigned char>(encoded[2]) != 0x9f ||
        static_cast<unsigned char>(encoded[3]) != 0x98 ||
        static_cast<unsigned char>(encoded[4]) != 0x80) {
      return false;
    }

    char32_t decoded[2]{};

    const char8_t *external_next = nullptr;
    char32_t *internal_next = nullptr;

    state = {};

    if (facet.in(state, encoded, encoded + 5, external_next, decoded,
                 decoded + 2, internal_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (external_next != encoded + 5 || internal_next != decoded + 2) {
      return false;
    }

    if (decoded[0] != U'A' || decoded[1] != static_cast<char32_t>(0x1f600)) {
      return false;
    }

    state = {};

    if (facet.length(state, encoded, encoded + 5, 1) != 1) {
      return false;
    }

    state = {};

    if (facet.length(state, encoded, encoded + 5, 2) != 5) {
      return false;
    }
  }

  //
  // UTF-16 surrogate pair.
  //
  {
    const auto &facet = tested::use_facet<utf16>(classic);

    if (facet.max_length() != 3)
      return false;

    const char16_t source[] = {static_cast<char16_t>(0xd83d),
                               static_cast<char16_t>(0xde00)};

    char8_t encoded[4]{};

    tested::mbstate_t state{};

    const char16_t *from_next = nullptr;
    char8_t *to_next = nullptr;

    if (facet.out(state, source, source + 2, from_next, encoded, encoded + 4,
                  to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (from_next != source + 2 || to_next != encoded + 4) {
      return false;
    }

    if (static_cast<unsigned char>(encoded[0]) != 0xf0 ||
        static_cast<unsigned char>(encoded[1]) != 0x9f ||
        static_cast<unsigned char>(encoded[2]) != 0x98 ||
        static_cast<unsigned char>(encoded[3]) != 0x80) {
      return false;
    }

    char16_t decoded[2]{};

    const char8_t *external_next = nullptr;
    char16_t *internal_next = nullptr;

    state = {};

    if (facet.in(state, encoded, encoded + 4, external_next, decoded,
                 decoded + 2, internal_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (decoded[0] != source[0] || decoded[1] != source[1]) {
      return false;
    }

    //
    // One UTF-16 output slot cannot absorb a supplementary scalar.
    //
    state = {};
    external_next = nullptr;
    internal_next = nullptr;

    char16_t one[1]{};

    if (facet.in(state, encoded, encoded + 4, external_next, one, one + 1,
                 internal_next) != tested::codecvt_base::partial) {
      return false;
    }

    if (external_next != encoded || internal_next != one) {
      return false;
    }

    state = {};

    if (facet.length(state, encoded, encoded + 4, 1) != 0) {
      return false;
    }

    state = {};

    if (facet.length(state, encoded, encoded + 4, 2) != 4) {
      return false;
    }
  }

  //
  // Incomplete UTF-8 sequence remains unconsumed.
  //
  {
    const auto &facet = tested::use_facet<utf32>(classic);

    const char8_t incomplete[] = {static_cast<char8_t>(0xf0),
                                  static_cast<char8_t>(0x9f)};

    char32_t output[1]{};

    tested::mbstate_t state{};

    const char8_t *from_next = nullptr;
    char32_t *to_next = nullptr;

    if (facet.in(state, incomplete, incomplete + 2, from_next, output,
                 output + 1, to_next) != tested::codecvt_base::partial) {
      return false;
    }

    if (from_next != incomplete || to_next != output) {
      return false;
    }
  }

  //
  // Invalid Unicode scalar.
  //
  {
    const auto &facet = tested::use_facet<utf32>(classic);

    const char32_t invalid[] = {static_cast<char32_t>(0xd800)};

    char8_t output[4]{};

    tested::mbstate_t state{};

    const char32_t *from_next = nullptr;
    char8_t *to_next = nullptr;

    if (facet.out(state, invalid, invalid + 1, from_next, output, output + 4,
                  to_next) != tested::codecvt_base::error) {
      return false;
    }

    if (from_next != invalid || to_next != output) {
      return false;
    }
  }

  //
  // Insufficient UTF-8 output buffer.
  //
  {
    const auto &facet = tested::use_facet<utf32>(classic);

    const char32_t source[] = {static_cast<char32_t>(0x1f600)};

    char8_t output[3]{};

    tested::mbstate_t state{};

    const char32_t *from_next = nullptr;
    char8_t *to_next = nullptr;

    if (facet.out(state, source, source + 1, from_next, output, output + 3,
                  to_next) != tested::codecvt_base::partial) {
      return false;
    }

    if (from_next != source || to_next != output) {
      return false;
    }
  }

  //
  // FTL classic wchar_t <-> narrow is the current ASCII C locale.
  //
  {
    const auto &facet = tested::use_facet<wide>(classic);

    if (facet.encoding() != 1)
      return false;

    const wchar_t wide_source[] = {L'A', L'z', L'7'};

    char narrow[3]{};

    tested::mbstate_t state{};

    const wchar_t *wide_next = nullptr;
    char *narrow_next = nullptr;

    if (facet.out(state, wide_source, wide_source + 3, wide_next, narrow,
                  narrow + 3, narrow_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (narrow[0] != 'A' || narrow[1] != 'z' || narrow[2] != '7') {
      return false;
    }

    wchar_t roundtrip[3]{};

    const char *external_next = nullptr;
    wchar_t *internal_next = nullptr;

    state = {};

    if (facet.in(state, narrow, narrow + 3, external_next, roundtrip,
                 roundtrip + 3, internal_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (roundtrip[0] != L'A' || roundtrip[1] != L'z' || roundtrip[2] != L'7') {
      return false;
    }

    const char invalid[] = {static_cast<char>(0xff)};

    state = {};
    external_next = nullptr;
    internal_next = nullptr;

    if (facet.in(state, invalid, invalid + 1, external_next, roundtrip,
                 roundtrip + 1, internal_next) != tested::codecvt_base::error) {
      return false;
    }
  }

  //
  // Stateless encodings require no unshift sequence.
  //
  {
    const auto &facet = tested::use_facet<utf32>(classic);

    tested::mbstate_t state{};
    char8_t buffer[4]{};
    char8_t *next = nullptr;

    if (facet.unshift(state, buffer, buffer + 4, next) !=
        tested::codecvt_base::noconv) {
      return false;
    }

    if (next != buffer)
      return false;
  }

  return true;
}
