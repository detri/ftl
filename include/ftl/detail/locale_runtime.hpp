// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_LOCALE_RUNTIME_HPP
#define FTL_DETAIL_LOCALE_RUNTIME_HPP

#ifdef FTL_REPLACE_STL
#include <locale.h>
#else
#include <ftl/locale.h>
#endif

#if !defined(_WIN32)
#include <nl_types.h>
#endif

namespace ftl_locale_runtime
{
#if defined(_WIN32)

    struct native_handle
    {
        void* crt = nullptr;
        unsigned int code_page = 0;
        int multibyte_max = 1;
        bool code_page_known = false;

        constexpr native_handle() noexcept = default;

        constexpr native_handle(decltype(nullptr)) noexcept {}

        constexpr explicit operator bool() const noexcept
        {
            return crt != nullptr;
        }

        friend constexpr bool operator==(native_handle value,
                                         decltype(nullptr)) noexcept
        {
            return value.crt == nullptr;
        }

        friend constexpr bool operator==(decltype(nullptr),
                                         native_handle value) noexcept
        {
            return value.crt == nullptr;
        }

        friend constexpr bool operator!=(native_handle value,
                                         decltype(nullptr)) noexcept
        {
            return value.crt != nullptr;
        }

        friend constexpr bool operator!=(decltype(nullptr),
                                         native_handle value) noexcept
        {
            return value.crt != nullptr;
        }
    };

    using native_catalog = void*;

#else

    using native_handle = void*;
    using native_catalog = ::nl_catd;

#endif

    enum class classification
    {
        space,
        print,
        cntrl,
        upper,
        lower,
        alpha,
        digit,
        punct,
        xdigit,
        blank
    };

    struct decoded_wide
    {
        enum class status { complete, partial, error };

        status result;
        wchar_t value;
        decltype(sizeof(0)) consumed;
    };

    struct encoded_wide
    {
        bool valid;
        char bytes[16];
        decltype(sizeof(0)) produced;
    };

    struct decoded_scalar
    {
        decoded_wide::status result;
        char32_t value;
        decltype(sizeof(0)) consumed;
    };

    struct encoded_scalar
    {
        bool valid;
        char bytes[16];
        decltype(sizeof(0)) produced;
    };

    inline bool valid_unicode_scalar(char32_t value) noexcept
    {
        return value <= 0x10ffffu &&
               !(value >= 0xd800u && value <= 0xdfffu);
    }

    struct windows_locale_data_public
    {
        const unsigned short* pctype;
        int mb_cur_max;
        unsigned int code_page;
    };

    struct windows_locale_pointers
    {
        windows_locale_data_public* locinfo;
        void* mbcinfo;
    };

#if defined(_WIN32)

    extern "C"
    {
    void*__cdecl _create_locale(int, const char*);

    void __cdecl _free_locale(void*);

    int __cdecl _isspace_l(int, void*);

    int __cdecl _isprint_l(int, void*);

    int __cdecl _iscntrl_l(int, void*);

    int __cdecl _isupper_l(int, void*);

    int __cdecl _islower_l(int, void*);

    int __cdecl _isalpha_l(int, void*);

    int __cdecl _isdigit_l(int, void*);

    int __cdecl _ispunct_l(int, void*);

    int __cdecl _isxdigit_l(int, void*);

    int __cdecl _isblank_l(int, void*);

    int __cdecl _iswspace_l(unsigned int, void*);

    int __cdecl _iswprint_l(unsigned int, void*);

    int __cdecl _iswcntrl_l(unsigned int, void*);

    int __cdecl _iswupper_l(unsigned int, void*);

    int __cdecl _iswlower_l(unsigned int, void*);

    int __cdecl _iswalpha_l(unsigned int, void*);

    int __cdecl _iswdigit_l(unsigned int, void*);

    int __cdecl _iswpunct_l(unsigned int, void*);

    int __cdecl _iswxdigit_l(unsigned int, void*);

    int __cdecl _iswblank_l(unsigned int, void*);

    int __cdecl _toupper_l(int, void*);

    int __cdecl _tolower_l(int, void*);

    unsigned int __cdecl _towupper_l(unsigned int, void*);

    unsigned int __cdecl _towlower_l(unsigned int, void*);

    decltype(sizeof(0)) __cdecl _strxfrm_l(char*, const char*,
                                           decltype(sizeof(0)), void*);

    decltype(sizeof(0)) __cdecl _wcsxfrm_l(wchar_t*, const wchar_t*,
                                           decltype(sizeof(0)), void*);

    int __cdecl _configthreadlocale(int);

    int __stdcall MultiByteToWideChar(
        unsigned int code_page,
        unsigned long flags,
        const char* input,
        int input_size,
        wchar_t* output,
        int output_size);

    int __stdcall WideCharToMultiByte(
        unsigned int code_page,
        unsigned long flags,
        const wchar_t* input,
        int input_size,
        char* output,
        int output_size,
        const char* default_character,
        int* used_default_character);

    int __stdcall IsDBCSLeadByteEx(
        unsigned int code_page,
        unsigned char value);

    int __stdcall GetLocaleInfoEx(
        const wchar_t* locale_name,
        unsigned long type,
        wchar_t* data,
        int data_count);

    int __stdcall GetUserDefaultLocaleName(
        wchar_t* locale_name,
        int locale_name_count);
    } // extern "C"

    struct resolved_code_page
    {
        unsigned int value = 0;
        bool known = false;
    };

    inline char ascii_lower(char value) noexcept
    {
        if (value >= 'A' && value <= 'Z')
            return static_cast<char>(value - 'A' + 'a');

        return value;
    }

    inline bool ascii_equal_case_insensitive(const char* left,
                                             const char* right) noexcept
    {
        if (left == nullptr || right == nullptr)
            return left == right;

        for (;; ++left, ++right)
        {
            if (ascii_lower(*left) != ascii_lower(*right))
                return false;

            if (*left == '\0')
                return true;
        }
    }

    inline resolved_code_page parse_decimal_code_page(
        const char* text) noexcept
    {
        if (text == nullptr || *text == '\0')
            return {};

        unsigned int value = 0;

        for (; *text != '\0'; ++text)
        {
            if (*text < '0' || *text > '9')
                return {};

            const unsigned int digit =
                    static_cast<unsigned int>(*text - '0');

            if (value > (0xffffffffu - digit) / 10u)
                return {};

            value = value * 10u + digit;
        }

        if (value == 0)
            return {};

        return {value, true};
    }

    inline resolved_code_page parse_wide_decimal_code_page(
        const wchar_t* text) noexcept
    {
        if (text == nullptr || *text == L'\0')
            return {};

        unsigned int value = 0;

        for (; *text != L'\0'; ++text)
        {
            if (*text < L'0' || *text > L'9')
                return {};

            const unsigned int digit =
                    static_cast<unsigned int>(*text - L'0');

            if (value > (0xffffffffu - digit) / 10u)
                return {};

            value = value * 10u + digit;
        }

        return {value, true};
    }

    inline bool copy_ascii_locale_name(
        wchar_t* destination,
        decltype(sizeof(0)) capacity,
        const char* first,
        const char* last) noexcept
    {
        decltype(sizeof(0)) count = 0;

        while (first != last)
        {
            if (count + 1 >= capacity)
                return false;

            const auto byte =
                    static_cast<unsigned char>(*first++);

            /*
             * Windows locale names / BCP-47 language tags are ASCII.
             * Legacy CRT names containing non-ASCII text are deliberately
             * left unresolved rather than guessed.
             */
            if (byte > 0x7f)
                return false;

            destination[count++] =
                    static_cast<wchar_t>(byte);
        }

        destination[count] = L'\0';
        return true;
    }

    inline resolved_code_page query_windows_locale_code_page(
        const wchar_t* locale_name,
        bool oem) noexcept
    {
        /*
         * LOCALE_IDEFAULTANSICODEPAGE = 0x00001004
         * LOCALE_IDEFAULTCODEPAGE     = 0x0000000b
         */
        constexpr unsigned long ansi_code_page = 0x00001004ul;
        constexpr unsigned long oem_code_page = 0x0000000bul;

        wchar_t buffer[16]{};

        const int result =
                GetLocaleInfoEx(
                    locale_name,
                    oem ? oem_code_page : ansi_code_page,
                    buffer,
                    static_cast<int>(sizeof(buffer) / sizeof(buffer[0])));

        if (result == 0)
            return {};

        return parse_wide_decimal_code_page(buffer);
    }

    inline resolved_code_page query_user_code_page(bool oem) noexcept
    {
        /*
         * LOCALE_NAME_MAX_LENGTH is 85 including the terminator.
         */
        wchar_t locale_name[85]{};

        if (GetUserDefaultLocaleName(
                locale_name,
                static_cast<int>(
                    sizeof(locale_name) / sizeof(locale_name[0]))) == 0)
        {
            return {};
        }

        return query_windows_locale_code_page(locale_name, oem);
    }

    inline resolved_code_page resolve_windows_code_page(
        const char* name) noexcept
    {
        if (name == nullptr)
            return {};

        /*
         * FTL deliberately models the portable C execution encoding
         * as ASCII for multibyte conversion.
         */
        if (ascii_equal_case_insensitive(name, "C"))
            return {20127u, true};

        /*
         * Empty locale means the implementation-defined native
         * environment. On Windows, its character encoding is the
         * user's default ANSI code page.
         */
        if (*name == '\0')
            return query_user_code_page(false);

        const char* end = name;

        while (*end != '\0')
            ++end;

        const char* dot = nullptr;

        for (const char* current = name; current != end; ++current)
        {
            if (*current == '.')
                dot = current;
        }

        if (dot != nullptr)
        {
            const char* suffix = dot + 1;

            if (ascii_equal_case_insensitive(suffix, "UTF-8") ||
                ascii_equal_case_insensitive(suffix, "UTF8"))
            {
                return {65001u, true};
            }

            if (const auto numeric =
                        parse_decimal_code_page(suffix);
                numeric.known)
            {
                return numeric;
            }

            const bool ansi =
                    ascii_equal_case_insensitive(suffix, "ACP");

            const bool oem =
                    ascii_equal_case_insensitive(suffix, "OCP");

            if (!ansi && !oem)
                return {};

            /*
             * ".ACP" / ".OCP" use the user's default locale.
             */
            if (dot == name)
                return query_user_code_page(oem);

            /*
             * For an NLS-compatible locale name such as en-US.ACP,
             * query Windows directly. Legacy CRT names such as
             * English_United States.ACP may fail this lookup; in that
             * case code_page_known simply remains false.
             */
            wchar_t locale_name[85]{};

            if (!copy_ascii_locale_name(
                locale_name,
                sizeof(locale_name) / sizeof(locale_name[0]),
                name,
                dot))
            {
                return {};
            }

            return query_windows_locale_code_page(
                locale_name, oem);
        }

        /*
         * Modern locale-name form, e.g. "en-US".
         *
         * UCRT chooses that locale's default ANSI code page when no
         * explicit code page is present.
         */
        wchar_t locale_name[85]{};

        if (!copy_ascii_locale_name(
            locale_name,
            sizeof(locale_name) / sizeof(locale_name[0]),
            name,
            end))
        {
            return {};
        }

        return query_windows_locale_code_page(
            locale_name, false);
    }

    inline native_handle create_native_locale(
        int category,
        const char* name) noexcept
    {
        if (name == nullptr)
            return nullptr;

        void* crt = _create_locale(category, name);

        if (crt == nullptr)
            return nullptr;

        native_handle result{};
        result.crt = crt;

        const int maximum =
                ___mb_cur_max_l_func(crt);

        result.multibyte_max =
                maximum > 0 ? maximum : 1;

        const auto code_page =
                resolve_windows_code_page(name);

        result.code_page = code_page.value;
        result.code_page_known = code_page.known;

        return result;
    }

    inline native_handle create_ctype(const char* name) noexcept
    {
        return create_native_locale(LC_CTYPE, name);
    }

    inline native_handle create_collate(const char* name) noexcept
    {
        return create_native_locale(LC_COLLATE, name);
    }

    inline native_handle create_numeric(const char* name) noexcept
    {
        /*
         * LC_ALL intentionally gives the handle LC_CTYPE as well as
         * LC_NUMERIC. numpunct<wchar_t> needs the locale's multibyte
         * conversion rules to widen punctuation.
         */
        return create_native_locale(LC_ALL, name);
    }

    inline native_handle create_time(const char* name) noexcept
    {
        return create_native_locale(LC_ALL, name);
    }

    inline native_handle create_monetary(const char* name) noexcept
    {
        return create_native_locale(LC_ALL, name);
    }

    inline native_handle create_messages(const char* name) noexcept
    {
        return create_native_locale(LC_ALL, name);
    }

    inline bool message_catalog_valid(native_catalog catalog) noexcept
    {
        return catalog != nullptr;
    }

    inline native_catalog open_message_catalog(native_handle,
                                               const char*) noexcept
    {
        return nullptr;
    }

    inline const char* get_message_catalog(native_catalog, int, int,
                                           const char* fallback) noexcept
    {
        return fallback;
    }

    inline void close_message_catalog(native_catalog) noexcept {}

    inline windows_locale_data_public*
    windows_locale_data(native_handle locale) noexcept
    {
        if (locale == nullptr)
            return nullptr;

        const auto* pointers =
                static_cast<const windows_locale_pointers*>(locale);

        return pointers->locinfo;
    }

    inline unsigned int
    windows_code_page(native_handle locale) noexcept
    {
        const auto* data = windows_locale_data(locale);
        return data != nullptr ? data->code_page : 0u;
    }

    inline int multibyte_max_length(native_handle locale) noexcept
    {
        return locale.multibyte_max > 0
                   ? locale.multibyte_max
                   : 1;
    }

    inline bool multibyte_is_stateful(native_handle) noexcept
    {
        /*
         * UCRT's Windows locale encodings are represented as code-page
         * encodings rather than ISO-2022-style persistent shift machines.
         *
         * Restartability is therefore entirely represented by FTL's
         * pending input bytes.
         */
        return false;
    }

    inline constexpr unsigned int windows_cp_utf7 = 65000u;
    inline constexpr unsigned int windows_cp_utf8 = 65001u;
    inline constexpr unsigned int windows_cp_gb18030 = 54936u;

    inline constexpr unsigned long windows_mb_err_invalid_chars = 0x00000008ul;
    inline constexpr unsigned long windows_wc_err_invalid_chars = 0x00000080ul;
    inline constexpr unsigned long windows_wc_no_best_fit_chars = 0x00000400ul;

    inline bool windows_requires_zero_flags(unsigned int code_page) noexcept
    {
        return code_page == 42u ||
               code_page == windows_cp_utf7 ||
               (code_page >= 50220u && code_page <= 50222u) ||
               code_page == 50225u ||
               code_page == 50227u ||
               code_page == 50229u ||
               (code_page >= 57002u && code_page <= 57011u);
    }

    /*
     * Returns:
     *
     *   > 0  complete UTF-8 sequence length
     *     0  incomplete but valid prefix
     *    -1  invalid prefix
     */
    inline int utf8_sequence_length(const char* input,
                                    decltype(sizeof(0)) available) noexcept
    {
        if (available == 0)
            return 0;

        const auto first =
                static_cast<unsigned char>(input[0]);

        if (first <= 0x7fu)
            return 1;

        unsigned expected = 0;

        if (first >= 0xc2u && first <= 0xdfu)
            expected = 2;
        else if (first >= 0xe0u && first <= 0xefu)
            expected = 3;
        else if (first >= 0xf0u && first <= 0xf4u)
            expected = 4;
        else
            return -1;

        const auto present =
                available < expected
                    ? available
                    : static_cast<decltype(sizeof(0))>(expected);

        for (decltype(sizeof(0)) index = 1; index < present; ++index)
        {
            const auto byte =
                    static_cast<unsigned char>(input[index]);

            if (byte < 0x80u || byte > 0xbfu)
                return -1;
        }

        if (available >= 2)
        {
            const auto second =
                    static_cast<unsigned char>(input[1]);

            if (first == 0xe0u && second < 0xa0u)
                return -1;

            if (first == 0xedu && second > 0x9fu)
                return -1;

            if (first == 0xf0u && second < 0x90u)
                return -1;

            if (first == 0xf4u && second > 0x8fu)
                return -1;
        }

        return available < expected
                   ? 0
                   : static_cast<int>(expected);
    }

    /*
     * Windows GB18030 has one-, two-, and four-byte forms.
     */
    inline int gb18030_sequence_length(
        const char* input,
        decltype(sizeof(0)) available) noexcept
    {
        if (available == 0)
            return 0;

        const auto first =
                static_cast<unsigned char>(input[0]);

        if (first <= 0x7fu)
            return 1;

        if (first < 0x81u || first > 0xfeu)
            return -1;

        if (available < 2)
            return 0;

        const auto second =
                static_cast<unsigned char>(input[1]);

        if (second >= 0x40u &&
            second <= 0xfeu &&
            second != 0x7fu)
        {
            return 2;
        }

        if (second < 0x30u || second > 0x39u)
            return -1;

        if (available < 3)
            return 0;

        const auto third =
                static_cast<unsigned char>(input[2]);

        if (third < 0x81u || third > 0xfeu)
            return -1;

        if (available < 4)
            return 0;

        const auto fourth =
                static_cast<unsigned char>(input[3]);

        if (fourth < 0x30u || fourth > 0x39u)
            return -1;

        return 4;
    }

    inline decoded_scalar windows_decode_exact(
        native_handle locale,
        const char* input,
        decltype(sizeof(0)) count) noexcept
    {
        decoded_scalar result{
            decoded_wide::status::error,
            char32_t{},
            0
        };

        const unsigned int code_page =
                windows_code_page(locale);

        /*
         * UCRT's "C" locale reports code page zero. Do NOT feed that
         * to MultiByteToWideChar: Windows interprets zero as CP_ACP,
         * whereas FTL's C locale is the portable ASCII execution set.
         */
        if (code_page == 0u)
        {
            if (count != 1)
                return result;

            const auto byte =
                    static_cast<unsigned char>(input[0]);

            if (byte > 0x7fu)
                return result;

            result.result = decoded_wide::status::complete;
            result.value = static_cast<char32_t>(byte);
            result.consumed = 1;
            return result;
        }

        wchar_t wide[2]{};

        const unsigned long flags =
                windows_requires_zero_flags(code_page)
                    ? 0ul
                    : windows_mb_err_invalid_chars;

        const int produced =
                MultiByteToWideChar(
                    code_page,
                    flags,
                    input,
                    static_cast<int>(count),
                    wide,
                    2);

        if (produced == 1)
        {
            const auto first =
                    static_cast<unsigned int>(wide[0]);

            if (first >= 0xd800u && first <= 0xdfffu)
                return result;

            result.result = decoded_wide::status::complete;
            result.value = static_cast<char32_t>(first);
            result.consumed = count;
            return result;
        }

        if (produced == 2)
        {
            const auto high =
                    static_cast<unsigned int>(wide[0]);
            const auto low =
                    static_cast<unsigned int>(wide[1]);

            if (high < 0xd800u || high > 0xdbffu ||
                low < 0xdc00u || low > 0xdfffu)
            {
                return result;
            }

            result.result = decoded_wide::status::complete;
            result.value = static_cast<char32_t>(
                0x10000u +
                ((high - 0xd800u) << 10u) +
                (low - 0xdc00u));
            result.consumed = count;
            return result;
        }

        return result;
    }

    inline decoded_scalar decode_scalar(
        native_handle locale,
        const char* first,
        const char* last) noexcept
    {
        if (first == last)
        {
            return {
                decoded_wide::status::partial,
                char32_t{},
                0
            };
        }

        const unsigned int code_page =
                windows_code_page(locale);

        const auto available =
                static_cast<decltype(sizeof(0))>(last - first);

        if (code_page == 0u)
            return windows_decode_exact(locale, first, 1);

        int required = -1;

        if (code_page == windows_cp_utf8)
        {
            required =
                    utf8_sequence_length(first, available);
        } else if (code_page == windows_cp_gb18030)
        {
            required =
                    gb18030_sequence_length(first, available);
        } else
        {
            const int maximum =
                    multibyte_max_length(locale);

            if (maximum <= 1)
            {
                required = 1;
            } else if (maximum == 2)
            {
                const auto byte =
                        static_cast<unsigned char>(first[0]);

                if (IsDBCSLeadByteEx(code_page, byte) != 0)
                    required = available >= 2 ? 2 : 0;
                else
                    required = 1;
            } else
            {
                /*
                 * Unusual Windows encodings. Probe complete prefixes up to
                 * MB_CUR_MAX. Common SBCS, DBCS, UTF-8 and GB18030 never
                 * reach this fallback.
                 */
                const auto limit =
                        available <
                        static_cast<decltype(sizeof(0))>(maximum)
                            ? available
                            : static_cast<decltype(sizeof(0))>(maximum);

                for (decltype(sizeof(0)) count = 1;
                     count <= limit;
                     ++count)
                {
                    const auto candidate =
                            windows_decode_exact(locale, first, count);

                    if (candidate.result ==
                        decoded_wide::status::complete)
                    {
                        return candidate;
                    }
                }

                if (available <
                    static_cast<decltype(sizeof(0))>(maximum))
                {
                    return {
                        decoded_wide::status::partial,
                        char32_t{},
                        0
                    };
                }

                return {
                    decoded_wide::status::error,
                    char32_t{},
                    0
                };
            }
        }

        if (required == 0)
        {
            return {
                decoded_wide::status::partial,
                char32_t{},
                0
            };
        }

        if (required < 0)
        {
            return {
                decoded_wide::status::error,
                char32_t{},
                0
            };
        }

        return windows_decode_exact(
            locale,
            first,
            static_cast<decltype(sizeof(0))>(required));
    }

    inline encoded_scalar encode_scalar(
        native_handle locale,
        char32_t value) noexcept
    {
        encoded_scalar result{};

        if (!valid_unicode_scalar(value))
            return result;

        const unsigned int code_page =
                windows_code_page(locale);

        if (code_page == 0u)
        {
            if (value > 0x7fu)
                return result;

            result.valid = true;
            result.bytes[0] =
                    static_cast<char>(
                        static_cast<unsigned char>(value));
            result.produced = 1;
            return result;
        }

        wchar_t wide[2]{};
        int wide_count = 0;

        if (value <= 0xffffu)
        {
            wide[0] = static_cast<wchar_t>(value);
            wide_count = 1;
        } else
        {
            const auto adjusted =
                    static_cast<unsigned int>(value - 0x10000u);

            wide[0] = static_cast<wchar_t>(
                0xd800u + (adjusted >> 10u));

            wide[1] = static_cast<wchar_t>(
                0xdc00u + (adjusted & 0x3ffu));

            wide_count = 2;
        }

        unsigned long flags = 0;
        int used_default = 0;
        int* used_default_pointer = nullptr;

        if (code_page == windows_cp_utf8 ||
            code_page == windows_cp_gb18030)
        {
            flags = windows_wc_err_invalid_chars;
        } else if (!windows_requires_zero_flags(code_page))
        {
            flags = windows_wc_no_best_fit_chars;
            used_default_pointer = &used_default;
        } else if (code_page != windows_cp_utf7)
        {
            used_default_pointer = &used_default;
        }

        const int produced =
                WideCharToMultiByte(
                    code_page,
                    flags,
                    wide,
                    wide_count,
                    result.bytes,
                    static_cast<int>(sizeof(result.bytes)),
                    nullptr,
                    used_default_pointer);

        if (produced <= 0 || used_default != 0)
            return result;

        /*
         * Zero-flag legacy code pages may silently substitute. Verify
         * the resulting byte sequence decodes to the same scalar.
         */
        if (windows_requires_zero_flags(code_page) &&
            code_page != windows_cp_utf7)
        {
            const auto roundtrip =
                    windows_decode_exact(
                        locale,
                        result.bytes,
                        static_cast<decltype(sizeof(0))>(produced));

            if (roundtrip.result !=
                decoded_wide::status::complete ||
                roundtrip.value != value)
            {
                return encoded_scalar{};
            }
        }

        result.valid = true;
        result.produced =
                static_cast<decltype(sizeof(0))>(produced);

        return result;
    }

    inline decoded_wide decode_wide(
        native_handle locale,
        const char* first,
        const char* last) noexcept
    {
        const auto decoded =
                decode_scalar(locale, first, last);

        if (decoded.result !=
            decoded_wide::status::complete)
        {
            return {
                decoded.result,
                wchar_t{},
                decoded.consumed
            };
        }

        const wchar_t value =
                static_cast<wchar_t>(decoded.value);

        /*
         * wchar_t is UTF-16-sized on Windows. <cuchar> can represent
         * supplementary scalars through char16_t/char32_t, while
         * <cwchar>'s single wchar_t conversion cannot.
         */
        if (static_cast<char32_t>(value) != decoded.value)
        {
            return {
                decoded_wide::status::error,
                wchar_t{},
                0
            };
        }

        return {
            decoded_wide::status::complete,
            value,
            decoded.consumed
        };
    }

    inline encoded_wide encode_wide(
        native_handle locale,
        wchar_t value) noexcept
    {
        encoded_wide result{};

        const auto scalar =
                static_cast<unsigned long long>(value);

        if (scalar > 0x10ffffu)
            return result;

        const auto encoded =
                encode_scalar(
                    locale,
                    static_cast<char32_t>(scalar));

        if (!encoded.valid)
            return result;

        result.valid = true;
        result.produced = encoded.produced;

        for (decltype(sizeof(0)) index = 0;
             index < encoded.produced;
             ++index)
        {
            result.bytes[index] = encoded.bytes[index];
        }

        return result;
    }

    inline wchar_t widen_first(native_handle locale,
                               const char* source,
                               wchar_t fallback) noexcept
    {
        if (source == nullptr || *source == '\0')
            return fallback;

        const int maximum =
                multibyte_max_length(locale);

        decltype(sizeof(0)) available = 0;

        while (available <
               static_cast<decltype(sizeof(0))>(maximum) &&
               source[available] != '\0')
        {
            ++available;
        }

        const auto decoded =
                decode_wide(
                    locale,
                    source,
                    source + available);

        return decoded.result ==
               decoded_wide::status::complete
                   ? decoded.value
                   : fallback;
    }

    inline decltype(sizeof(0)) transform_byte(native_handle locale,
                                              char* destination, const char* source,
                                              decltype(sizeof(0)) count) noexcept
    {
        return _strxfrm_l(destination, source, count, locale.crt);
    }

    inline decltype(sizeof(0)) transform_wide(native_handle locale,
                                              wchar_t* destination,
                                              const wchar_t* source,
                                              decltype(sizeof(0)) count) noexcept
    {
        return _wcsxfrm_l(destination, source, count, locale.crt);
    }

    inline void destroy(native_handle locale) noexcept
    {
        if (locale != nullptr)
            _free_locale(locale);
    }

    inline bool classify_byte(native_handle locale, unsigned char value,
                              classification kind) noexcept
    {
        const int character = static_cast<int>(value);

        switch (kind)
        {
            case classification::space:
                return _isspace_l(character, locale.crt) != 0;
            case classification::print:
                return _isprint_l(character, locale.crt) != 0;
            case classification::cntrl:
                return _iscntrl_l(character, locale.crt) != 0;
            case classification::upper:
                return _isupper_l(character, locale.crt) != 0;
            case classification::lower:
                return _islower_l(character, locale.crt) != 0;
            case classification::alpha:
                return _isalpha_l(character, locale.crt) != 0;
            case classification::digit:
                return _isdigit_l(character, locale.crt) != 0;
            case classification::punct:
                return _ispunct_l(character, locale.crt) != 0;
            case classification::xdigit:
                return _isxdigit_l(character, locale.crt) != 0;
            case classification::blank:
                return _isblank_l(character, locale.crt) != 0;
        }

        return false;
    }

    inline bool classify_wide(native_handle locale, wchar_t value,
                              classification kind) noexcept
    {
        const unsigned int character = static_cast<unsigned int>(value);

        switch (kind)
        {
            case classification::space:
                return _iswspace_l(character, locale.crt) != 0;
            case classification::print:
                return _iswprint_l(character, locale.crt) != 0;
            case classification::cntrl:
                return _iswcntrl_l(character, locale.crt) != 0;
            case classification::upper:
                return _iswupper_l(character, locale.crt) != 0;
            case classification::lower:
                return _iswlower_l(character, locale.crt) != 0;
            case classification::alpha:
                return _iswalpha_l(character, locale.crt) != 0;
            case classification::digit:
                return _iswdigit_l(character, locale.crt) != 0;
            case classification::punct:
                return _iswpunct_l(character, locale.crt) != 0;
            case classification::xdigit:
                return _iswxdigit_l(character, locale.crt) != 0;
            case classification::blank:
                return _iswblank_l(character, locale.crt) != 0;
        }

        return false;
    }

    inline char toupper_byte(native_handle locale, char value) noexcept
    {
        return static_cast<char>(
            _toupper_l(static_cast<unsigned char>(value), locale.crt));
    }

    inline char tolower_byte(native_handle locale, char value) noexcept
    {
        return static_cast<char>(
            _tolower_l(static_cast<unsigned char>(value), locale.crt));
    }

    inline wchar_t toupper_wide(native_handle locale, wchar_t value) noexcept
    {
        return static_cast<wchar_t>(
            _towupper_l(static_cast<unsigned int>(value), locale.crt));
    }

    inline wchar_t tolower_wide(native_handle locale, wchar_t value) noexcept
    {
        return static_cast<wchar_t>(
            _towlower_l(static_cast<unsigned int>(value), locale.crt));
    }

    inline wchar_t widen(native_handle locale,
                         char value) noexcept
    {
        const auto decoded =
                decode_wide(locale, &value, &value + 1);

        if (decoded.result !=
            decoded_wide::status::complete)
        {
            return static_cast<wchar_t>(
                static_cast<unsigned char>(value));
        }

        return decoded.value;
    }

    inline char narrow(native_handle locale,
                       wchar_t value,
                       char fallback) noexcept
    {
        const auto encoded =
                encode_wide(locale, value);

        return encoded.valid && encoded.produced == 1
                   ? encoded.bytes[0]
                   : fallback;
    }

#else

    extern "C"
    {
    void* newlocale(int, const char*, void*);

    void* uselocale(void*);

#if defined(__APPLE__)
    int freelocale(void*);
#else
    void freelocale(void*);
#endif

    int isspace_l(int, void*);
    int isprint_l(int, void*);
    int iscntrl_l(int, void*);
    int isupper_l(int, void*);
    int islower_l(int, void*);
    int isalpha_l(int, void*);
    int isdigit_l(int, void*);
    int ispunct_l(int, void*);
    int isxdigit_l(int, void*);
    int isblank_l(int, void*);

    int iswspace_l(unsigned int, void*);
    int iswprint_l(unsigned int, void*);
    int iswcntrl_l(unsigned int, void*);
    int iswupper_l(unsigned int, void*);
    int iswlower_l(unsigned int, void*);
    int iswalpha_l(unsigned int, void*);
    int iswdigit_l(unsigned int, void*);
    int iswpunct_l(unsigned int, void*);
    int iswxdigit_l(unsigned int, void*);
    int iswblank_l(unsigned int, void*);

    int toupper_l(int, void*);
    int tolower_l(int, void*);

    unsigned int towupper_l(unsigned int, void*);

    unsigned int towlower_l(unsigned int, void*);

    unsigned int btowc(int);
    int wctob(unsigned int);

    decltype(sizeof(0)) strxfrm_l(char*, const char*, decltype(sizeof(0)),
                                  void*);

    decltype(sizeof(0)) wcsxfrm_l(wchar_t*, const wchar_t*, decltype(sizeof(0)),
                                  void*);

    int mbtowc(wchar_t*, const char*, decltype(sizeof(0)));

    int wctomb(char*, wchar_t);

    //
    // The native mbstate_t representation is deliberately opaque
    // to FTL. codecvt_byname only supports stateless native
    // encodings, so the restartable conversion state is always
    // supplied as nullptr.
    //
    decltype(sizeof(0)) mbrtowc(wchar_t*, const char*, decltype(sizeof(0)),
                                void*);

    decltype(sizeof(0)) wcrtomb(char*, wchar_t, void*);

    int mbsinit(const void*);

#if defined(__APPLE__)

    int ___mb_cur_max(void);

    decltype(sizeof(0)) mbrtowc_l(wchar_t*, const char*, decltype(sizeof(0)),
                                  void*, void*);

    decltype(sizeof(0)) wcrtomb_l(char*, wchar_t, void*, void*);

    int mbsinit_l(const void*, void*);

#else

    decltype(sizeof(0)) __ctype_get_mb_cur_max(void) noexcept;

#endif

    } // extern "C"

#if defined(__APPLE__)

    inline constexpr int native_collate_mask = 1 << 0;

    inline constexpr int native_ctype_mask = 1 << 1;

    inline constexpr int native_messages_mask = 1 << 2;

    inline constexpr int native_monetary_mask = 1 << 3;

    inline constexpr int native_numeric_mask = 1 << 4;

    inline constexpr int native_time_mask = 1 << 5;

#else

    inline constexpr int native_ctype_mask = 1 << LC_CTYPE;

    inline constexpr int native_numeric_mask = 1 << LC_NUMERIC;

    inline constexpr int native_time_mask = 1 << LC_TIME;

    inline constexpr int native_collate_mask = 1 << LC_COLLATE;

    inline constexpr int native_monetary_mask = 1 << LC_MONETARY;

    /*
     * glibc/Linux LC_MESSAGES is category 5.
     * It is deliberately private here because LC_MESSAGES is a POSIX
     * extension and therefore isn't part of FTL's ISO C <locale.h>.
     */
    inline constexpr int native_messages_mask = 1 << 5;

#endif

    inline native_handle create_ctype(const char* name) noexcept
    {
        if (name == nullptr)
            return nullptr;

        return newlocale(native_ctype_mask, name, nullptr);
    }

    inline native_handle create_collate(const char* name) noexcept
    {
        if (name == nullptr)
            return nullptr;

        return newlocale(native_collate_mask, name, nullptr);
    }

    inline native_handle create_numeric(const char* name) noexcept
    {
        if (name == nullptr)
            return nullptr;

        return newlocale(native_numeric_mask | native_ctype_mask, name, nullptr);
    }

    inline native_handle create_time(const char* name) noexcept
    {
        if (name == nullptr)
            return nullptr;

        return newlocale(native_time_mask | native_ctype_mask, name, nullptr);
    }

    inline native_handle create_monetary(const char* name) noexcept
    {
        if (name == nullptr)
            return nullptr;

        return newlocale(native_monetary_mask | native_ctype_mask, name, nullptr);
    }

    inline native_handle create_messages(const char* name) noexcept
    {
        if (name == nullptr)
            return nullptr;

        return newlocale(native_messages_mask, name, nullptr);
    }

    inline native_catalog invalid_message_catalog() noexcept
    {
        return reinterpret_cast<native_catalog>(static_cast<__INTPTR_TYPE__>(-1));
    }

    inline bool message_catalog_valid(native_catalog catalog) noexcept
    {
        return catalog != invalid_message_catalog();
    }

    inline native_catalog open_message_catalog(native_handle locale,
                                               const char* name) noexcept
    {
        if (locale == nullptr || name == nullptr)
        {
            return invalid_message_catalog();
        }

        native_handle previous = uselocale(locale);

        if (previous == nullptr)
            return invalid_message_catalog();

        native_catalog result = ::catopen(name, NL_CAT_LOCALE);

        (void) uselocale(previous);

        return result;
    }

    inline const char* get_message_catalog(native_catalog catalog, int set,
                                           int message,
                                           const char* fallback) noexcept
    {
        return ::catgets(catalog, set, message, fallback);
    }

    inline void close_message_catalog(native_catalog catalog) noexcept
    {
        if (message_catalog_valid(catalog))
            (void) ::catclose(catalog);
    }

    inline int multibyte_max_length(native_handle locale) noexcept
    {
        native_handle previous = uselocale(locale);

        if (previous == nullptr)
            return 1;

#if defined(__APPLE__)

    const int native_max = ___mb_cur_max();

    (void)uselocale (previous);

  return native_max> 0 ? native_max : 1;

#else

    const auto native_max = __ctype_get_mb_cur_max();

    (void)uselocale (previous);

  return native_max> 0 ? static_cast<int>(native_max) : 1;

#endif
    }

    inline bool multibyte_is_stateful(native_handle locale) noexcept
    {
        native_handle previous = uselocale(locale);

        if (previous == nullptr)
            return true;

        const int result = mbtowc(nullptr, nullptr, 0);

        (void) uselocale(previous);

        return result != 0;
    }

    inline decltype(sizeof(0))
    decode_wide_native(native_handle locale, wchar_t* value, const char* first,
                       decltype(sizeof(0)) available, void* state) noexcept
    {



#if defined(__APPLE__)
    return mbrtowc_l(value, first, available, state, locale);
#else
    native_handle previous = uselocale(locale);

  if (previous== nullptr)
    return static_cast<decltype(sizeof(0))>(-1);

    const auto result = mbrtowc(value, first, available, state);
    (void)uselocale (previous);
  return result;
#endif
    }

    inline decltype(sizeof(0)) encode_wide_native(native_handle locale,
                                                  char* output, wchar_t value,
                                                  void* state) noexcept
    {



#if defined(__APPLE__)
    return wcrtomb_l(output, value, state, locale);
#else
    native_handle previous = uselocale(locale);

  if (previous== nullptr)
    return static_cast<decltype(sizeof(0))>(-1);

    const auto result = wcrtomb(output, value, state);
    (void)uselocale (previous);
  return result;
#endif
    }

    inline bool native_state_is_initial(native_handle locale,
                                        const void* state) noexcept
    {



#if defined(__APPLE__)
    return mbsinit_l(state, locale)!= 0;
#else
    (void)locale;
  return mbsinit (state)!= 0;
#endif
    }

    inline decoded_wide decode_wide(native_handle locale, const char* first,
                                    const char* last) noexcept
    {
        if (first == last)
        {
            return {decoded_wide::status::partial, wchar_t{}, 0};
        }

        native_handle previous = uselocale(locale);

        if (previous == nullptr)
        {
            return {decoded_wide::status::error, wchar_t{}, 0};
        }

        //
        // codecvt_byname rejects state-dependent native encodings. Give each
        // isolated scalar an independent native state so an incomplete probe never
        // contaminates another conversion through mbrtowc's hidden internal state.
        // This storage exceeds mbstate_t on every supported POSIX target.
        //
        alignas(16) unsigned char conversion_state[128]{};

        wchar_t value{};

        const auto available = static_cast<decltype(sizeof(0))>(last - first);

        const auto result = mbrtowc(&value, first, available, conversion_state);

        (void) uselocale(previous);

        constexpr auto conversion_error = static_cast<decltype(sizeof(0))>(-1);

        constexpr auto conversion_partial = static_cast<decltype(sizeof(0))>(-2);

        if (result == conversion_partial)
        {
            return {decoded_wide::status::partial, wchar_t{}, 0};
        }

        if (result == conversion_error)
        {
            return {decoded_wide::status::error, wchar_t{}, 0};
        }

        if (result == 0)
        {
            return {decoded_wide::status::complete, wchar_t{}, 1};
        }

        return {decoded_wide::status::complete, value, result};
    }

    inline encoded_wide encode_wide(native_handle locale,
                                    wchar_t value) noexcept
    {
        encoded_wide result{};

        alignas(16) unsigned char conversion_state[128]{};

        const auto produced =
                encode_wide_native(
                    locale,
                    result.bytes,
                    value,
                    conversion_state);

        if (produced ==
            static_cast<decltype(sizeof(0))>(-1))
        {
            return result;
        }

        result.valid = true;
        result.produced = produced;
        return result;
    }

    inline wchar_t widen_first(native_handle locale,
                               const char* source,
                               wchar_t fallback) noexcept
    {
        if (source == nullptr || *source == '\0')
            return fallback;

        decltype(sizeof(0)) length = 0;

        while (source[length] != '\0')
            ++length;

        const auto decoded =
                decode_wide(
                    locale,
                    source,
                    source + length);

        return decoded.result ==
               decoded_wide::status::complete
                   ? decoded.value
                   : fallback;
    }

    inline decltype(sizeof(0)) transform_byte(native_handle locale,
                                              char* destination, const char* source,
                                              decltype(sizeof(0)) count) noexcept
    {
        return strxfrm_l(destination, source, count, locale);
    }

    inline decltype(sizeof(0)) transform_wide(native_handle locale,
                                              wchar_t* destination,
                                              const wchar_t* source,
                                              decltype(sizeof(0)) count) noexcept
    {
        return wcsxfrm_l(destination, source, count, locale);
    }

    inline void destroy(native_handle locale) noexcept
    {
        if (locale == nullptr)
            return;

#if defined(__APPLE__)
    (void)freelocale (locale);
#else
    freelocale (locale);
#endif
    }

    inline bool classify_byte(native_handle locale, unsigned char value,
                              classification kind) noexcept
    {
        const int character = static_cast<int>(value);

        switch (kind)
        {
            case classification::space:
                return isspace_l(character, locale) != 0;
            case classification::print:
                return isprint_l(character, locale) != 0;
            case classification::cntrl:
                return iscntrl_l(character, locale) != 0;
            case classification::upper:
                return isupper_l(character, locale) != 0;
            case classification::lower:
                return islower_l(character, locale) != 0;
            case classification::alpha:
                return isalpha_l(character, locale) != 0;
            case classification::digit:
                return isdigit_l(character, locale) != 0;
            case classification::punct:
                return ispunct_l(character, locale) != 0;
            case classification::xdigit:
                return isxdigit_l(character, locale) != 0;
            case classification::blank:
                return isblank_l(character, locale) != 0;
        }

        return false;
    }

    inline bool classify_wide(native_handle locale, wchar_t value,
                              classification kind) noexcept
    {
        const unsigned int character = static_cast<unsigned int>(value);

        switch (kind)
        {
            case classification::space:
                return iswspace_l(character, locale) != 0;
            case classification::print:
                return iswprint_l(character, locale) != 0;
            case classification::cntrl:
                return iswcntrl_l(character, locale) != 0;
            case classification::upper:
                return iswupper_l(character, locale) != 0;
            case classification::lower:
                return iswlower_l(character, locale) != 0;
            case classification::alpha:
                return iswalpha_l(character, locale) != 0;
            case classification::digit:
                return iswdigit_l(character, locale) != 0;
            case classification::punct:
                return iswpunct_l(character, locale) != 0;
            case classification::xdigit:
                return iswxdigit_l(character, locale) != 0;
            case classification::blank:
                return iswblank_l(character, locale) != 0;
        }

        return false;
    }

    inline char toupper_byte(native_handle locale, char value) noexcept
    {
        return static_cast<char>(
            toupper_l(static_cast<unsigned char>(value), locale));
    }

    inline char tolower_byte(native_handle locale, char value) noexcept
    {
        return static_cast<char>(
            tolower_l(static_cast<unsigned char>(value), locale));
    }

    inline wchar_t toupper_wide(native_handle locale, wchar_t value) noexcept
    {
        return static_cast<wchar_t>(
            towupper_l(static_cast<unsigned int>(value), locale));
    }

    inline wchar_t tolower_wide(native_handle locale, wchar_t value) noexcept
    {
        return static_cast<wchar_t>(
            towlower_l(static_cast<unsigned int>(value), locale));
    }

    inline wchar_t widen(native_handle locale, char value) noexcept
    {
        native_handle previous = uselocale(locale);

        const unsigned int result = btowc(static_cast<unsigned char>(value));

        (void) uselocale(previous);

        if (result == static_cast<unsigned int>(-1))
        {
            return static_cast<wchar_t>(static_cast<unsigned char>(value));
        }

        return static_cast<wchar_t>(result);
    }

    inline char narrow(native_handle locale, wchar_t value,
                       char fallback) noexcept
    {
        native_handle previous = uselocale(locale);

        const int result = wctob(static_cast<unsigned int>(value));

        (void) uselocale(previous);

        if (result < 0)
            return fallback;

        return static_cast<char>(result);
    }

#endif

    inline native_handle snapshot_current_ctype() noexcept
    {
        const char* name = ::setlocale(LC_CTYPE, nullptr);
        return create_ctype(name != nullptr ? name : "C");
    }

    template<class State>
    inline decoded_wide decode_wide_restartable(native_handle locale, State& state,
                                                const char* first,
                                                const char* last) noexcept
    {
#if defined(_WIN32)
        constexpr auto capacity = sizeof(state.pending);
        const auto available = static_cast<decltype(sizeof(0))>(last - first);
        const auto maximum = static_cast<decltype(sizeof(0))>(
            multibyte_max_length(locale));
        if (maximum == 0 || maximum > capacity || state.pending_count > maximum)
        {
            state.pending_count = 0;
            return {decoded_wide::status::error, wchar_t{}, 0};
        }

        const auto room = capacity - state.pending_count;
        auto supplied = available < room ? available : room;
        if (supplied > maximum - state.pending_count)
            supplied = maximum - state.pending_count;

        unsigned char buffer[capacity]{};
        for (decltype(sizeof(0)) index = 0; index < state.pending_count; ++index)
            buffer[index] = state.pending[index];
        for (decltype(sizeof(0)) index = 0; index < supplied; ++index)
            buffer[state.pending_count + index] =
                    static_cast<unsigned char>(first[index]);

        const auto previous = state.pending_count;
        const auto decoded = decode_wide(
            locale, reinterpret_cast<const char*>(buffer),
            reinterpret_cast<const char*>(buffer + previous + supplied));

        if (decoded.result == decoded_wide::status::partial)
        {
            /*
             * Windows locale multibyte encodings exposed through _locale_t are
             * stateless. A null byte therefore cannot legitimately occur in the
             * middle of an incomplete character. _mbtowc_l cannot distinguish
             * invalid from incomplete input when fewer than MB_CUR_MAX bytes are
             * supplied, so make that distinction here when the terminator is known.
             */
            for (decltype(sizeof(0)) index = 0; index < supplied; ++index)
            {
                if (first[index] == '\0')
                {
                    state.pending_count = 0;
                    return {decoded_wide::status::error, wchar_t{}, 0};
                }
            }

            for (decltype(sizeof(0)) index = 0; index < supplied; ++index)
                state.pending[previous + index] =
                        static_cast<unsigned char>(first[index]);

            state.pending_count += static_cast<unsigned int>(supplied);
            return {decoded.result, wchar_t{}, supplied};
        }

        state.pending_count = 0;

        if (decoded.result == decoded_wide::status::error)
            return {decoded.result, wchar_t{}, 0};

        return {
            decoded.result, decoded.value,
            decoded.consumed - static_cast<decltype(sizeof(0))>(previous)
        };
#else
        const auto available = static_cast<decltype(sizeof(0))>(last - first);
        wchar_t value{};
        const auto result =
                decode_wide_native(locale, &value, first, available, state.native_state);
        constexpr auto conversion_error = static_cast<decltype(sizeof(0))>(-1);
        constexpr auto conversion_partial = static_cast<decltype(sizeof(0))>(-2);

        if (result == conversion_error)
        {
            for (auto& byte: state.native_state)
                byte = 0;
            state.native_active = 0;
            return {decoded_wide::status::error, wchar_t{}, 0};
        }

        state.native_active =
                native_state_is_initial(locale, state.native_state) ? 0u : 1u;

        if (result == conversion_partial)
            return {decoded_wide::status::partial, wchar_t{}, available};

        return {decoded_wide::status::complete, value, result == 0 ? 1u : result};
#endif
    }

    template<class State>
    inline encoded_wide encode_wide_restartable(native_handle locale, State& state,
                                                wchar_t value) noexcept
    {
#if defined(_WIN32)
        (void) state;
        return encode_wide(locale, value);
#else
        encoded_wide result{};
        const auto produced =
                encode_wide_native(locale, result.bytes, value, state.native_state);

        if (produced == static_cast<decltype(sizeof(0))>(-1))
            return result;

        state.native_active =
                native_state_is_initial(locale, state.native_state) ? 0u : 1u;
        result.valid = true;
        result.produced = produced;
        return result;
#endif
    }

    template<class State>
    inline encoded_wide unshift_wide(native_handle locale, State& state) noexcept
    {
        encoded_wide result{};

#if defined(_WIN32)
        (void) locale;
        (void) state;
        result.valid = true;
#else
        const auto produced =
                encode_wide_native(locale, result.bytes, wchar_t{}, state.native_state);

        if (produced == static_cast<decltype(sizeof(0))>(-1) || produced == 0)
            return result;

        state.native_active =
                native_state_is_initial(locale, state.native_state) ? 0u : 1u;
        result.valid = true;
        result.produced = produced - 1;
#endif

        return result;
    }
} // namespace ftl_locale_runtime

#endif
