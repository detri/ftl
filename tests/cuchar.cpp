#ifdef FTL_REPLACE_STL
#include <cerrno>
#include <clocale>
#include <cuchar>
namespace tested = std;
#else
#include <ftl/cerrno>
#include <ftl/clocale>
#include <ftl/cuchar>
namespace tested = ftl;
#endif

bool ftl_test()
{
    constexpr tested::size_t error = static_cast<tested::size_t>(-1);

    constexpr tested::size_t incomplete = static_cast<tested::size_t>(-2);

    // ASCII narrow -> UTF-8.
    {
        tested::mbstate_t state{};
        char8_t output{};

        if (tested::mbrtoc8(&output, "A", 1, &state) != 1)
        {
            return false;
        }

        if (output != u8'A')
        {
            return false;
        }
    }

    // ASCII narrow -> UTF-16.
    {
        tested::mbstate_t state{};
        char16_t output{};

        if (tested::mbrtoc16(&output, "B", 1, &state) != 1)
        {
            return false;
        }

        if (output != u'B')
        {
            return false;
        }
    }

    // ASCII narrow -> UTF-32.
    {
        tested::mbstate_t state{};
        char32_t output{};

        if (tested::mbrtoc32(&output, "C", 1, &state) != 1)
        {
            return false;
        }

        if (output != U'C')
        {
            return false;
        }
    }

    // Null character conversions return zero.
    {
        tested::mbstate_t state{};
        char8_t c8 = u8'x';
        char16_t c16 = u'x';
        char32_t c32 = U'x';

        if (tested::mbrtoc8(&c8, "", 1, &state) != 0 || c8 != u8'\0')
        {
            return false;
        }

        if (tested::mbrtoc16(&c16, "", 1, &state) != 0 || c16 != u'\0')
        {
            return false;
        }

        if (tested::mbrtoc32(&c32, "", 1, &state) != 0 || c32 != U'\0')
        {
            return false;
        }
    }

    // Zero input length means incomplete input.
    {
        tested::mbstate_t state{};

        if (tested::mbrtoc8(nullptr, "A", 0, &state) != incomplete)
        {
            return false;
        }

        if (tested::mbrtoc16(nullptr, "A", 0, &state) != incomplete)
        {
            return false;
        }

        if (tested::mbrtoc32(nullptr, "A", 0, &state) != incomplete)
        {
            return false;
        }
    }

    // Null source is equivalent to converting "".
    {
        tested::mbstate_t state{};

        if (tested::mbrtoc8(nullptr, nullptr, 0, &state) != 0)
        {
            return false;
        }

        if (tested::mbrtoc16(nullptr, nullptr, 0, &state) != 0)
        {
            return false;
        }

        if (tested::mbrtoc32(nullptr, nullptr, 0, &state) != 0)
        {
            return false;
        }
    }

    // Non-ASCII narrow input is invalid in FTL's C locale.
    {
        const char input[] = {static_cast<char>(0x80), '\0'};

        tested::mbstate_t state{};

        errno = 0;

        if (tested::mbrtoc8(nullptr, input, 1, &state) != error ||
            errno != EILSEQ)
        {
            return false;
        }

        errno = 0;

        if (tested::mbrtoc16(nullptr, input, 1, &state) != error ||
            errno != EILSEQ)
        {
            return false;
        }

        errno = 0;

        if (tested::mbrtoc32(nullptr, input, 1, &state) != error ||
            errno != EILSEQ)
        {
            return false;
        }
    }

    // UTF code units -> ASCII narrow encoding.
    {
        tested::mbstate_t state{};
        char output = '\0';

        if (tested::c8rtomb(&output, u8'A', &state) != 1 || output != 'A')
        {
            return false;
        }

        if (tested::c16rtomb(&output, u'B', &state) != 1 || output != 'B')
        {
            return false;
        }

        if (tested::c32rtomb(&output, U'C', &state) != 1 || output != 'C')
        {
            return false;
        }
    }

    // Null output pointer requests reset conversion.
    {
        tested::mbstate_t state{};

        if (tested::c8rtomb(nullptr, u8'X', &state) != 1)
        {
            return false;
        }

        if (tested::c16rtomb(nullptr, u'X', &state) != 1)
        {
            return false;
        }

        if (tested::c32rtomb(nullptr, U'X', &state) != 1)
        {
            return false;
        }
    }

    // A UTF-8 leading code unit is accepted into state.
    // The completed non-ASCII scalar then fails because the
    // C locale cannot represent it.
    {
        tested::mbstate_t state{};
        char output = '\0';

        if (tested::c8rtomb(&output, static_cast<char8_t>(0xc2), &state) != 0)
        {
            return false;
        }

        errno = 0;

        if (tested::c8rtomb(&output, static_cast<char8_t>(0xa2), &state) != error ||
            errno != EILSEQ)
        {
            return false;
        }
    }

    // Invalid UTF-8 sequences fail.
    {
        tested::mbstate_t state{};
        char output = '\0';

        if (tested::c8rtomb(&output, static_cast<char8_t>(0xe2), &state) != 0)
        {
            return false;
        }

        errno = 0;

        if (tested::c8rtomb(&output, u8'A', &state) != error || errno != EILSEQ)
        {
            return false;
        }
    }

    // UTF-16 surrogate state is preserved across calls.
    {
        tested::mbstate_t state{};
        char output = '\0';

        if (tested::c16rtomb(&output, static_cast<char16_t>(0xd83c), &state) != 0)
        {
            return false;
        }

        errno = 0;

        if (tested::c16rtomb(&output, static_cast<char16_t>(0xdf4c), &state) !=
            error ||
            errno != EILSEQ)
        {
            return false;
        }
    }

    // Lone low surrogate is invalid.
    {
        tested::mbstate_t state{};

        errno = 0;

        if (tested::c16rtomb(nullptr, static_cast<char16_t>(0xdc00), &state) != 1)
        {
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
            errno != EILSEQ)
        {
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
            errno != EILSEQ)
        {
            return false;
        }

        errno = 0;

        if (tested::c32rtomb(&output, U'\u00a2', &state) != error ||
            errno != EILSEQ)
        {
            return false;
        }
    }


#if defined(_WIN32)

    //
    // Windows UCRT's .UTF-8 locale exercises the FTL-owned scalar codec rather
    // than the ASCII-only C locale. Save and restore LC_CTYPE so this test does
    // not leak locale state into any future work in this process.
    //
    char previous_ctype[128]{};

    {
        const char* current = tested::setlocale(LC_CTYPE, nullptr);

        if (current == nullptr)
            return false;

        tested::size_t index = 0;

        for (; current[index] != '\0'; ++index)
        {
            if (index + 1 >= sizeof(previous_ctype))
                return false;

            previous_ctype[index] = current[index];
        }

        previous_ctype[index] = '\0';
    }

    if (tested::setlocale(LC_CTYPE, ".UTF-8") == nullptr)
        return false;

    //
    // UTF-8 selected as the execution multibyte encoding: UTF-32 scalars
    // round-trip through 2-, 3-, and 4-byte sequences.
    //
    {
        struct scalar_case
        {
            char32_t scalar;
            unsigned char bytes[4];
            tested::size_t count;
        };

        const scalar_case cases[] = {
            {U'\u00a2', {0xc2, 0xa2, 0x00, 0x00}, 2},
            {U'\u20ac', {0xe2, 0x82, 0xac, 0x00}, 3},
            {U'\U0001f600', {0xf0, 0x9f, 0x98, 0x80}, 4},
        };

        for (const auto& current: cases)
        {
            tested::mbstate_t encode_state{};
            char encoded[8]{};

            const tested::size_t produced =
                    tested::c32rtomb(encoded, current.scalar, &encode_state);

            if (produced != current.count)
                return false;

            for (tested::size_t index = 0; index < current.count; ++index)
            {
                if (static_cast<unsigned char>(encoded[index]) != current.bytes[index])
                    return false;
            }

            tested::mbstate_t decode_state{};
            char32_t decoded{};
            char input[4]{};

            for (tested::size_t index = 0; index < current.count; ++index)
                input[index] = static_cast<char>(current.bytes[index]);

            const tested::size_t consumed =
                    tested::mbrtoc32(&decoded, input, current.count, &decode_state);

            if (consumed != current.count || decoded != current.scalar)
                return false;
        }
    }

    //
    // Restartable decoding must retain an incomplete multibyte character in
    // mbstate_t, and ordinary copies of mbstate_t must be independently usable.
    //
    {
        const char first[] = {static_cast<char>(0xf0), static_cast<char>(0x9f)};
        const char second[] = {static_cast<char>(0x98), static_cast<char>(0x80)};

        tested::mbstate_t state{};
        char32_t output{};

        if (tested::mbrtoc32(&output, first, 2, &state) != incomplete)
            return false;

        tested::mbstate_t copied = state;

        if (tested::mbrtoc32(&output, second, 2, &state) != 2 ||
            output != U'\U0001f600')
        {
            return false;
        }

        output = U'\0';

        if (tested::mbrtoc32(&output, second, 2, &copied) != 2 ||
            output != U'\U0001f600')
        {
            return false;
        }
    }

    //
    // mbrtoc8 consumes the complete multibyte character once, emits the first
    // UTF-8 code unit, then returns -3 for each remaining code unit without
    // processing input.
    //
    {
        constexpr tested::size_t continuation =
                static_cast<tested::size_t>(-3);

        const char input[] = {
            static_cast<char>(0xf0), static_cast<char>(0x9f),
            static_cast<char>(0x98), static_cast<char>(0x80)
        };

        const char ignored_until_complete[] = "X";

        tested::mbstate_t state{};
        char8_t output{};

        if (tested::mbrtoc8(&output, input, 4, &state) != 4 ||
            output != static_cast<char8_t>(0xf0))
        {
            return false;
        }

        if (tested::mbrtoc8(&output, ignored_until_complete, 1, &state) !=
            continuation ||
            output != static_cast<char8_t>(0x9f))
        {
            return false;
        }

        if (tested::mbrtoc8(&output, ignored_until_complete, 1, &state) !=
            continuation ||
            output != static_cast<char8_t>(0x98))
        {
            return false;
        }

        if (tested::mbrtoc8(&output, ignored_until_complete, 1, &state) !=
            continuation ||
            output != static_cast<char8_t>(0x80))
        {
            return false;
        }

        if (tested::mbrtoc8(&output, ignored_until_complete, 1, &state) != 1 ||
            output != u8'X')
        {
            return false;
        }
    }

    //
    // mbrtoc16 consumes the four UTF-8 input bytes when producing the high
    // surrogate. The low surrogate is emitted on the next call with -3 and no
    // input consumption.
    //
    {
        constexpr tested::size_t continuation =
                static_cast<tested::size_t>(-3);

        const char input[] = {
            static_cast<char>(0xf0), static_cast<char>(0x9f),
            static_cast<char>(0x98), static_cast<char>(0x80)
        };

        const char ignored_until_complete[] = "X";

        tested::mbstate_t state{};
        char16_t output{};

        if (tested::mbrtoc16(&output, input, 4, &state) != 4 ||
            output != static_cast<char16_t>(0xd83d))
        {
            return false;
        }

        if (tested::mbrtoc16(&output, ignored_until_complete, 1, &state) !=
            continuation ||
            output != static_cast<char16_t>(0xde00))
        {
            return false;
        }

        if (tested::mbrtoc16(&output, ignored_until_complete, 1, &state) != 1 ||
            output != u'X')
        {
            return false;
        }
    }

    //
    // c8rtomb stages a complete UTF-8 scalar. Intermediate code units produce
    // no output; the final code unit emits the execution-locale multibyte
    // representation.
    //
    {
        tested::mbstate_t state{};
        char output[8];

        for (char& byte: output)
            byte = static_cast<char>(0x55);

        if (tested::c8rtomb(output, static_cast<char8_t>(0xf0), &state) != 0 ||
            static_cast<unsigned char>(output[0]) != 0x55)
        {
            return false;
        }

        if (tested::c8rtomb(output, static_cast<char8_t>(0x9f), &state) != 0 ||
            static_cast<unsigned char>(output[0]) != 0x55)
        {
            return false;
        }

        if (tested::c8rtomb(output, static_cast<char8_t>(0x98), &state) != 0 ||
            static_cast<unsigned char>(output[0]) != 0x55)
        {
            return false;
        }

        if (tested::c8rtomb(output, static_cast<char8_t>(0x80), &state) != 4)
            return false;

        const unsigned char expected[] = {0xf0, 0x9f, 0x98, 0x80};

        for (tested::size_t index = 0; index < 4; ++index)
        {
            if (static_cast<unsigned char>(output[index]) != expected[index])
                return false;
        }
    }

    //
    // c16rtomb performs the analogous UTF-16 surrogate staging.
    //
    {
        tested::mbstate_t state{};
        char output[8];

        for (char& byte: output)
            byte = static_cast<char>(0x55);

        if (tested::c16rtomb(output, static_cast<char16_t>(0xd83d), &state) != 0 ||
            static_cast<unsigned char>(output[0]) != 0x55)
        {
            return false;
        }

        if (tested::c16rtomb(output, static_cast<char16_t>(0xde00), &state) != 4)
            return false;

        const unsigned char expected[] = {0xf0, 0x9f, 0x98, 0x80};

        for (tested::size_t index = 0; index < 4; ++index)
        {
            if (static_cast<unsigned char>(output[index]) != expected[index])
                return false;
        }
    }

    //
    // Strict UTF-8 decoding rejects malformed sequences rather than accepting a
    // replacement character or best-fit conversion.
    //
    {
        const char invalid[] = {
            static_cast<char>(0xc0),
            static_cast<char>(0xaf)
        };

        tested::mbstate_t state{};
        char32_t output{};

        errno = 0;

        if (tested::mbrtoc32(&output, invalid, 2, &state) != error ||
            errno != EILSEQ)
        {
            return false;
        }
    }

    if (tested::setlocale(LC_CTYPE, previous_ctype) == nullptr)
        return false;

#endif

    return true;
}
