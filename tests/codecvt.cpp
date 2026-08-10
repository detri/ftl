#ifdef FTL_REPLACE_STL
#include <codecvt>
namespace tested = std;
#else
#include <ftl/codecvt>
namespace tested = ftl;
#endif

bool ftl_test() {
  static_assert(tested::little_endian == 1);

  static_assert(tested::generate_header == 2);

  static_assert(tested::consume_header == 4);

  //
  // codecvt_utf8<char32_t>
  //
  {
    tested::codecvt_utf8<char32_t> facet;

    if (facet.always_noconv())
      return false;

    if (facet.encoding() != 0)
      return false;

    if (facet.max_length() != 4)
      return false;

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

    if (external_next != encoded + 5 || internal_next != roundtrip + 2) {
      return false;
    }

    if (roundtrip[0] != U'A' ||
        roundtrip[1] != static_cast<char32_t>(0x1f600)) {
      return false;
    }
  }

  //
  // UCS-2 behavior for 16-bit Elem.
  //
  {
    tested::codecvt_utf8<char16_t> facet;

    const char source[] = {static_cast<char>(0xf0), static_cast<char>(0x9f),
                           static_cast<char>(0x98), static_cast<char>(0x80)};

    char16_t output[2]{};

    tested::mbstate_t state{};

    const char *from_next = nullptr;
    char16_t *to_next = nullptr;

    if (facet.in(state, source, source + 4, from_next, output, output + 2,
                 to_next) != tested::codecvt_base::error) {
      return false;
    }

    if (from_next != source)
      return false;
  }

  //
  // Maxcode.
  //
  {
    tested::codecvt_utf8<char32_t, 0x7f> facet;

    const char32_t source[] = {static_cast<char32_t>(0x00a2)};

    char output[4]{};

    tested::mbstate_t state{};

    const char32_t *from_next = nullptr;
    char *to_next = nullptr;

    if (facet.out(state, source, source + 1, from_next, output, output + 4,
                  to_next) != tested::codecvt_base::error) {
      return false;
    }

    if (facet.encoding() != 1)
      return false;

    if (facet.max_length() != 1)
      return false;
  }

  //
  // UTF-8 BOM generation.
  //
  {
    using facet_type = tested::codecvt_utf8<char32_t, 0x10ffff,
                                            static_cast<tested::codecvt_mode>(
                                                tested::generate_header)>;

    facet_type facet;

    if (facet.encoding() != -1)
      return false;

    const char32_t source[] = {U'A'};

    char output[4]{};

    tested::mbstate_t state{};

    const char32_t *from_next = nullptr;
    char *to_next = nullptr;

    if (facet.out(state, source, source + 1, from_next, output, output + 4,
                  to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (to_next != output + 4)
      return false;

    if (static_cast<unsigned char>(output[0]) != 0xef ||
        static_cast<unsigned char>(output[1]) != 0xbb ||
        static_cast<unsigned char>(output[2]) != 0xbf || output[3] != 'A') {
      return false;
    }

    //
    // Same state: the header must not be generated twice.
    //
    const char32_t second[] = {U'B'};

    char second_output[1]{};

    from_next = nullptr;
    to_next = nullptr;

    if (facet.out(state, second, second + 1, from_next, second_output,
                  second_output + 1, to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (second_output[0] != 'B')
      return false;
  }

  //
  // UTF-8 BOM consumption.
  //
  {
    using facet_type = tested::codecvt_utf8<char32_t, 0x10ffff,
                                            static_cast<tested::codecvt_mode>(
                                                tested::consume_header)>;

    facet_type facet;

    if (facet.max_length() != 7)
      return false;

    const char source[] = {static_cast<char>(0xef), static_cast<char>(0xbb),
                           static_cast<char>(0xbf), 'A'};

    char32_t output[1]{};

    tested::mbstate_t state{};

    const char *from_next = nullptr;
    char32_t *to_next = nullptr;

    if (facet.in(state, source, source + 4, from_next, output, output + 1,
                 to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (output[0] != U'A')
      return false;

    state = {};

    if (facet.length(state, source, source + 4, 1) != 4) {
      return false;
    }
  }

  //
  // codecvt_utf16 default big-endian.
  //
  {
    tested::codecvt_utf16<char32_t> facet;

    if (facet.encoding() != 0)
      return false;

    if (facet.max_length() != 4)
      return false;

    const char32_t source[] = {U'A', static_cast<char32_t>(0x1f600)};

    char output[6]{};

    tested::mbstate_t state{};

    const char32_t *from_next = nullptr;
    char *to_next = nullptr;

    if (facet.out(state, source, source + 2, from_next, output, output + 6,
                  to_next) != tested::codecvt_base::ok) {
      return false;
    }

    const unsigned char expected[] = {0x00, 0x41, 0xd8, 0x3d, 0xde, 0x00};

    for (int index = 0; index < 6; ++index) {
      if (static_cast<unsigned char>(output[index]) != expected[index]) {
        return false;
      }
    }

    char32_t roundtrip[2]{};

    const char *external_next = nullptr;
    char32_t *internal_next = nullptr;

    state = {};

    if (facet.in(state, output, output + 6, external_next, roundtrip,
                 roundtrip + 2, internal_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (roundtrip[0] != U'A' ||
        roundtrip[1] != static_cast<char32_t>(0x1f600)) {
      return false;
    }
  }

  //
  // UTF-16 little-endian.
  //
  {
    using facet_type = tested::codecvt_utf16<char32_t, 0x10ffff,
                                             static_cast<tested::codecvt_mode>(
                                                 tested::little_endian)>;

    facet_type facet;

    const char32_t source[] = {U'A', static_cast<char32_t>(0x1f600)};

    char output[6]{};

    tested::mbstate_t state{};

    const char32_t *from_next = nullptr;
    char *to_next = nullptr;

    if (facet.out(state, source, source + 2, from_next, output, output + 6,
                  to_next) != tested::codecvt_base::ok) {
      return false;
    }

    const unsigned char expected[] = {0x41, 0x00, 0x3d, 0xd8, 0x00, 0xde};

    for (int index = 0; index < 6; ++index) {
      if (static_cast<unsigned char>(output[index]) != expected[index]) {
        return false;
      }
    }
  }

  //
  // UTF-16 generated little-endian BOM.
  //
  {
    using facet_type = tested::codecvt_utf16<char32_t, 0x10ffff,
                                             static_cast<tested::codecvt_mode>(
                                                 tested::generate_header |
                                                 tested::little_endian)>;

    facet_type facet;

    const char32_t source[] = {U'A'};

    char output[4]{};

    tested::mbstate_t state{};

    const char32_t *from_next = nullptr;
    char *to_next = nullptr;

    if (facet.out(state, source, source + 1, from_next, output, output + 4,
                  to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (static_cast<unsigned char>(output[0]) != 0xff ||
        static_cast<unsigned char>(output[1]) != 0xfe ||
        static_cast<unsigned char>(output[2]) != 0x41 ||
        static_cast<unsigned char>(output[3]) != 0x00) {
      return false;
    }
  }

  //
  // UTF-16 BOM can override the preferred endian.
  //
  {
    using facet_type = tested::codecvt_utf16<char32_t, 0x10ffff,
                                             static_cast<tested::codecvt_mode>(
                                                 tested::consume_header)>;

    facet_type facet;

    const char source[] = {static_cast<char>(0xff), static_cast<char>(0xfe),
                           0x41, 0x00};

    char32_t output[1]{};

    tested::mbstate_t state{};

    const char *from_next = nullptr;
    char32_t *to_next = nullptr;

    if (facet.in(state, source, source + 4, from_next, output, output + 1,
                 to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (output[0] != U'A')
      return false;

    if (facet.max_length() != 6)
      return false;
  }

  //
  // codecvt_utf8_utf16 is UTF-16 internally, even when
  // Elem itself is wider than 16 bits.
  //
  {
    tested::codecvt_utf8_utf16<char32_t> facet;

    if (facet.max_length() != 3)
      return false;

    const char source[] = {static_cast<char>(0xf0), static_cast<char>(0x9f),
                           static_cast<char>(0x98), static_cast<char>(0x80)};

    char32_t output[2]{};

    tested::mbstate_t state{};

    const char *from_next = nullptr;
    char32_t *to_next = nullptr;

    if (facet.in(state, source, source + 4, from_next, output, output + 2,
                 to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (to_next != output + 2)
      return false;

    if (output[0] != static_cast<char32_t>(0xd83d) ||
        output[1] != static_cast<char32_t>(0xde00)) {
      return false;
    }

    char roundtrip[4]{};

    const char32_t *internal_next = nullptr;
    char *external_next = nullptr;

    state = {};

    if (facet.out(state, output, output + 2, internal_next, roundtrip,
                  roundtrip + 4, external_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (static_cast<unsigned char>(roundtrip[0]) != 0xf0 ||
        static_cast<unsigned char>(roundtrip[1]) != 0x9f ||
        static_cast<unsigned char>(roundtrip[2]) != 0x98 ||
        static_cast<unsigned char>(roundtrip[3]) != 0x80) {
      return false;
    }

    //
    // One internal UTF-16 slot cannot absorb this scalar.
    //
    state = {};

    if (facet.length(state, source, source + 4, 1) != 0) {
      return false;
    }

    state = {};

    if (facet.length(state, source, source + 4, 2) != 4) {
      return false;
    }
  }

  //
  // UTF-8/UTF-16 BOM mode.
  //
  {
    using facet_type =
        tested::codecvt_utf8_utf16<char16_t, 0x10ffff,
                                   static_cast<tested::codecvt_mode>(
                                       tested::consume_header)>;

    facet_type facet;

    if (facet.max_length() != 6)
      return false;

    const char source[] = {static_cast<char>(0xef), static_cast<char>(0xbb),
                           static_cast<char>(0xbf), 'A'};

    char16_t output[1]{};

    tested::mbstate_t state{};

    const char *from_next = nullptr;
    char16_t *to_next = nullptr;

    if (facet.in(state, source, source + 4, from_next, output, output + 1,
                 to_next) != tested::codecvt_base::ok) {
      return false;
    }

    if (output[0] != u'A')
      return false;
  }

  //
  // Stateless conversions require no unshift sequence.
  //
  {
    tested::codecvt_utf8<char32_t> facet;

    tested::mbstate_t state{};

    char output[4]{};
    char *to_next = nullptr;

    if (facet.unshift(state, output, output + 4, to_next) !=
        tested::codecvt_base::noconv) {
      return false;
    }

    if (to_next != output)
      return false;
  }

  return true;
}
