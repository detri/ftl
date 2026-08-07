#ifdef FTL_REPLACE_STL
#include <cerrno>
#include <cuchar>
namespace tested = std;
#else
#include <ftl/cerrno>
#include <ftl/cuchar>
namespace tested = ftl;
#endif

bool ftl_test() {
  constexpr tested::size_t error = static_cast<tested::size_t>(-1);

  constexpr tested::size_t incomplete = static_cast<tested::size_t>(-2);

  // ASCII narrow -> UTF-8.
  {
    tested::mbstate_t state{};
    char8_t output{};

    if (tested::mbrtoc8(&output, "A", 1, &state) != 1) {
      return false;
    }

    if (output != u8'A') {
      return false;
    }
  }

  // ASCII narrow -> UTF-16.
  {
    tested::mbstate_t state{};
    char16_t output{};

    if (tested::mbrtoc16(&output, "B", 1, &state) != 1) {
      return false;
    }

    if (output != u'B') {
      return false;
    }
  }

  // ASCII narrow -> UTF-32.
  {
    tested::mbstate_t state{};
    char32_t output{};

    if (tested::mbrtoc32(&output, "C", 1, &state) != 1) {
      return false;
    }

    if (output != U'C') {
      return false;
    }
  }

  // Null character conversions return zero.
  {
    tested::mbstate_t state{};
    char8_t c8 = u8'x';
    char16_t c16 = u'x';
    char32_t c32 = U'x';

    if (tested::mbrtoc8(&c8, "", 1, &state) != 0 || c8 != u8'\0') {
      return false;
    }

    if (tested::mbrtoc16(&c16, "", 1, &state) != 0 || c16 != u'\0') {
      return false;
    }

    if (tested::mbrtoc32(&c32, "", 1, &state) != 0 || c32 != U'\0') {
      return false;
    }
  }

  // Zero input length means incomplete input.
  {
    tested::mbstate_t state{};

    if (tested::mbrtoc8(nullptr, "A", 0, &state) != incomplete) {
      return false;
    }

    if (tested::mbrtoc16(nullptr, "A", 0, &state) != incomplete) {
      return false;
    }

    if (tested::mbrtoc32(nullptr, "A", 0, &state) != incomplete) {
      return false;
    }
  }

  // Null source is equivalent to converting "".
  {
    tested::mbstate_t state{};

    if (tested::mbrtoc8(nullptr, nullptr, 0, &state) != 0) {
      return false;
    }

    if (tested::mbrtoc16(nullptr, nullptr, 0, &state) != 0) {
      return false;
    }

    if (tested::mbrtoc32(nullptr, nullptr, 0, &state) != 0) {
      return false;
    }
  }

  // Non-ASCII narrow input is invalid in FTL's C locale.
  {
    const char input[] = {static_cast<char>(0x80), '\0'};

    tested::mbstate_t state{};

    errno = 0;

    if (tested::mbrtoc8(nullptr, input, 1, &state) != error ||
        errno != EILSEQ) {
      return false;
    }

    errno = 0;

    if (tested::mbrtoc16(nullptr, input, 1, &state) != error ||
        errno != EILSEQ) {
      return false;
    }

    errno = 0;

    if (tested::mbrtoc32(nullptr, input, 1, &state) != error ||
        errno != EILSEQ) {
      return false;
    }
  }

  // UTF code units -> ASCII narrow encoding.
  {
    tested::mbstate_t state{};
    char output = '\0';

    if (tested::c8rtomb(&output, u8'A', &state) != 1 || output != 'A') {
      return false;
    }

    if (tested::c16rtomb(&output, u'B', &state) != 1 || output != 'B') {
      return false;
    }

    if (tested::c32rtomb(&output, U'C', &state) != 1 || output != 'C') {
      return false;
    }
  }

  // Null output pointer requests reset conversion.
  {
    tested::mbstate_t state{};

    if (tested::c8rtomb(nullptr, u8'X', &state) != 1) {
      return false;
    }

    if (tested::c16rtomb(nullptr, u'X', &state) != 1) {
      return false;
    }

    if (tested::c32rtomb(nullptr, U'X', &state) != 1) {
      return false;
    }
  }

  // A UTF-8 leading code unit is accepted into state.
  // The completed non-ASCII scalar then fails because the
  // C locale cannot represent it.
  {
    tested::mbstate_t state{};
    char output = '\0';

    if (tested::c8rtomb(&output, static_cast<char8_t>(0xc2), &state) != 0) {
      return false;
    }

    errno = 0;

    if (tested::c8rtomb(&output, static_cast<char8_t>(0xa2), &state) != error ||
        errno != EILSEQ) {
      return false;
    }
  }

  // Invalid UTF-8 sequences fail.
  {
    tested::mbstate_t state{};
    char output = '\0';

    if (tested::c8rtomb(&output, static_cast<char8_t>(0xe2), &state) != 0) {
      return false;
    }

    errno = 0;

    if (tested::c8rtomb(&output, u8'A', &state) != error || errno != EILSEQ) {
      return false;
    }
  }

  // UTF-16 surrogate state is preserved across calls.
  {
    tested::mbstate_t state{};
    char output = '\0';

    if (tested::c16rtomb(&output, static_cast<char16_t>(0xd83c), &state) != 0) {
      return false;
    }

    errno = 0;

    if (tested::c16rtomb(&output, static_cast<char16_t>(0xdf4c), &state) !=
            error ||
        errno != EILSEQ) {
      return false;
    }
  }

  // Lone low surrogate is invalid.
  {
    tested::mbstate_t state{};

    errno = 0;

    if (tested::c16rtomb(nullptr, static_cast<char16_t>(0xdc00), &state) != 1) {
      /*
       * s == nullptr ignores c16 and behaves as though
       * u'\0' was supplied.
       */
      return false;
    }

    char output{};

    errno = 0;

    if (tested::c16rtomb(&output, static_cast<char16_t>(0xdc00), &state) !=
            error ||
        errno != EILSEQ) {
      return false;
    }
  }

  // Valid non-ASCII UTF-16/UTF-32 scalars cannot be encoded
  // by the current C locale.
  {
    tested::mbstate_t state{};
    char output{};

    errno = 0;

    if (tested::c16rtomb(&output, u'\u00a2', &state) != error ||
        errno != EILSEQ) {
      return false;
    }

    errno = 0;

    if (tested::c32rtomb(&output, U'\u00a2', &state) != error ||
        errno != EILSEQ) {
      return false;
    }
  }

  return true;
}
