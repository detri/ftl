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

namespace ftl_locale_runtime {

using native_handle = void *;

#if defined(_WIN32)
using native_catalog = void *;
#else
using native_catalog = ::nl_catd;
#endif

enum class classification {
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

struct decoded_wide {
  enum class status { complete, partial, error };

  status result;
  wchar_t value;
  decltype(sizeof(0)) consumed;
};

struct encoded_wide {
  bool valid;
  char bytes[16];
  decltype(sizeof(0)) produced;
};

#if defined(_WIN32)

extern "C" {

void *__cdecl _create_locale(int, const char *);
void __cdecl _free_locale(void *);

int __cdecl _isspace_l(int, void *);
int __cdecl _isprint_l(int, void *);
int __cdecl _iscntrl_l(int, void *);
int __cdecl _isupper_l(int, void *);
int __cdecl _islower_l(int, void *);
int __cdecl _isalpha_l(int, void *);
int __cdecl _isdigit_l(int, void *);
int __cdecl _ispunct_l(int, void *);
int __cdecl _isxdigit_l(int, void *);
int __cdecl _isblank_l(int, void *);

int __cdecl _iswspace_l(unsigned int, void *);
int __cdecl _iswprint_l(unsigned int, void *);
int __cdecl _iswcntrl_l(unsigned int, void *);
int __cdecl _iswupper_l(unsigned int, void *);
int __cdecl _iswlower_l(unsigned int, void *);
int __cdecl _iswalpha_l(unsigned int, void *);
int __cdecl _iswdigit_l(unsigned int, void *);
int __cdecl _iswpunct_l(unsigned int, void *);
int __cdecl _iswxdigit_l(unsigned int, void *);
int __cdecl _iswblank_l(unsigned int, void *);

int __cdecl _toupper_l(int, void *);
int __cdecl _tolower_l(int, void *);

unsigned int __cdecl _towupper_l(unsigned int, void *);
unsigned int __cdecl _towlower_l(unsigned int, void *);

int __cdecl _mbtowc_l(wchar_t *, const char *, decltype(sizeof(0)), void *);

int __cdecl _wctomb_l(char *, wchar_t, void *);

decltype(sizeof(0)) __cdecl _strxfrm_l(char *, const char *,
                                       decltype(sizeof(0)), void *);

decltype(sizeof(0)) __cdecl _wcsxfrm_l(wchar_t *, const wchar_t *,
                                       decltype(sizeof(0)), void *);

int __cdecl _configthreadlocale(int);

int __cdecl ___mb_cur_max_l_func(void *);

} // extern "C"

inline native_handle create_ctype(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  return _create_locale(LC_CTYPE, name);
}

inline native_handle create_collate(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  return _create_locale(LC_COLLATE, name);
}

inline native_handle create_numeric(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  //
  // LC_ALL intentionally gives the handle LC_CTYPE as well as
  // LC_NUMERIC. numpunct<wchar_t> needs the locale's multibyte
  // conversion rules to turn the CRT punctuation string into a
  // wide character.
  //
  return _create_locale(LC_ALL, name);
}

inline native_handle create_time(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  return _create_locale(LC_ALL, name);
}

inline native_handle create_monetary(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  //
  // We need LC_MONETARY for the actual facet data and
  // LC_CTYPE for widening currency/sign strings.
  //
  return _create_locale(LC_ALL, name);
}

inline native_handle create_messages(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  //
  // Windows has no POSIX message-catalog CRT facility.
  // Still create a native locale so messages_byname validates
  // the requested locale consistently with the other facets.
  //
  return _create_locale(LC_ALL, name);
}

inline bool message_catalog_valid(native_catalog catalog) noexcept {
  return catalog != nullptr;
}

inline native_catalog open_message_catalog(native_handle,
                                           const char *) noexcept {
  return nullptr;
}

inline const char *get_message_catalog(native_catalog, int, int,
                                       const char *fallback) noexcept {
  return fallback;
}

inline void close_message_catalog(native_catalog) noexcept {}

inline int multibyte_max_length(native_handle locale) noexcept {
  const int result = ___mb_cur_max_l_func(locale);

  return result > 0 ? result : 1;
}

inline bool multibyte_is_stateful(native_handle) noexcept {
  //
  // The Windows CRT code-page based multibyte encodings supported
  // through _locale_t do not expose a shift-state mechanism through
  // the _l conversion API.
  //
  return false;
}

inline decoded_wide decode_wide(native_handle locale, const char *first,
                                const char *last) noexcept {
  if (first == last) {
    return {decoded_wide::status::partial, wchar_t{}, 0};
  }

  wchar_t value{};

  const auto available = static_cast<decltype(sizeof(0))>(last - first);

  const int result = _mbtowc_l(&value, first, available, locale);

  if (result > 0) {
    return {decoded_wide::status::complete, value,
            static_cast<decltype(sizeof(0))>(result)};
  }

  if (result == 0) {
    return {decoded_wide::status::complete, wchar_t{}, 1};
  }

  //
  // Microsoft's non-restartable locale-specific conversion API
  // unfortunately does not distinguish invalid input from an
  // incomplete sequence. If fewer than the locale's maximum bytes
  // are available, preserve the source and request more input.
  //
  if (available <
      static_cast<decltype(sizeof(0))>(multibyte_max_length(locale))) {
    return {decoded_wide::status::partial, wchar_t{}, 0};
  }

  return {decoded_wide::status::error, wchar_t{}, 0};
}

inline encoded_wide encode_wide(native_handle locale, wchar_t value) noexcept {
  encoded_wide result{};

  const int produced = _wctomb_l(result.bytes, value, locale);

  if (produced < 0)
    return result;

  result.valid = true;

  result.produced = static_cast<decltype(sizeof(0))>(produced);

  return result;
}

inline wchar_t widen_first(native_handle locale, const char *source,
                           wchar_t fallback) noexcept {
  if (source == nullptr || *source == '\0')
    return fallback;

  decltype(sizeof(0)) length = 0;

  while (source[length] != '\0')
    ++length;

  wchar_t result{};

  if (_mbtowc_l(&result, source, length, locale) <= 0) {
    return fallback;
  }

  return result;
}

inline decltype(sizeof(0)) transform_byte(native_handle locale,
                                          char *destination, const char *source,
                                          decltype(sizeof(0)) count) noexcept {
  return _strxfrm_l(destination, source, count, locale);
}

inline decltype(sizeof(0)) transform_wide(native_handle locale,
                                          wchar_t *destination,
                                          const wchar_t *source,
                                          decltype(sizeof(0)) count) noexcept {
  return _wcsxfrm_l(destination, source, count, locale);
}

inline void destroy(native_handle locale) noexcept {
  if (locale != nullptr)
    _free_locale(locale);
}

inline bool classify_byte(native_handle locale, unsigned char value,
                          classification kind) noexcept {
  const int character = static_cast<int>(value);

  switch (kind) {
  case classification::space:
    return _isspace_l(character, locale) != 0;
  case classification::print:
    return _isprint_l(character, locale) != 0;
  case classification::cntrl:
    return _iscntrl_l(character, locale) != 0;
  case classification::upper:
    return _isupper_l(character, locale) != 0;
  case classification::lower:
    return _islower_l(character, locale) != 0;
  case classification::alpha:
    return _isalpha_l(character, locale) != 0;
  case classification::digit:
    return _isdigit_l(character, locale) != 0;
  case classification::punct:
    return _ispunct_l(character, locale) != 0;
  case classification::xdigit:
    return _isxdigit_l(character, locale) != 0;
  case classification::blank:
    return _isblank_l(character, locale) != 0;
  }

  return false;
}

inline bool classify_wide(native_handle locale, wchar_t value,
                          classification kind) noexcept {
  const unsigned int character = static_cast<unsigned int>(value);

  switch (kind) {
  case classification::space:
    return _iswspace_l(character, locale) != 0;
  case classification::print:
    return _iswprint_l(character, locale) != 0;
  case classification::cntrl:
    return _iswcntrl_l(character, locale) != 0;
  case classification::upper:
    return _iswupper_l(character, locale) != 0;
  case classification::lower:
    return _iswlower_l(character, locale) != 0;
  case classification::alpha:
    return _iswalpha_l(character, locale) != 0;
  case classification::digit:
    return _iswdigit_l(character, locale) != 0;
  case classification::punct:
    return _iswpunct_l(character, locale) != 0;
  case classification::xdigit:
    return _iswxdigit_l(character, locale) != 0;
  case classification::blank:
    return _iswblank_l(character, locale) != 0;
  }

  return false;
}

inline char toupper_byte(native_handle locale, char value) noexcept {
  return static_cast<char>(
      _toupper_l(static_cast<unsigned char>(value), locale));
}

inline char tolower_byte(native_handle locale, char value) noexcept {
  return static_cast<char>(
      _tolower_l(static_cast<unsigned char>(value), locale));
}

inline wchar_t toupper_wide(native_handle locale, wchar_t value) noexcept {
  return static_cast<wchar_t>(
      _towupper_l(static_cast<unsigned int>(value), locale));
}

inline wchar_t tolower_wide(native_handle locale, wchar_t value) noexcept {
  return static_cast<wchar_t>(
      _towlower_l(static_cast<unsigned int>(value), locale));
}

inline wchar_t widen(native_handle locale, char value) noexcept {
  wchar_t output{};

  const int result = _mbtowc_l(&output, &value, 1, locale);

  if (result < 0) {
    return static_cast<wchar_t>(static_cast<unsigned char>(value));
  }

  return output;
}

inline char narrow(native_handle locale, wchar_t value,
                   char fallback) noexcept {
  char buffer[16]{};

  const int result = _wctomb_l(buffer, value, locale);

  // ctype::narrow produces one char, not a multibyte sequence.
  return result == 1 ? buffer[0] : fallback;
}

#else

extern "C" {

void *newlocale(int, const char *, void *);
void *uselocale(void *);

#if defined(__APPLE__)
int freelocale(void *);
#else
void freelocale(void *);
#endif

int isspace_l(int, void *);
int isprint_l(int, void *);
int iscntrl_l(int, void *);
int isupper_l(int, void *);
int islower_l(int, void *);
int isalpha_l(int, void *);
int isdigit_l(int, void *);
int ispunct_l(int, void *);
int isxdigit_l(int, void *);
int isblank_l(int, void *);

int iswspace_l(unsigned int, void *);
int iswprint_l(unsigned int, void *);
int iswcntrl_l(unsigned int, void *);
int iswupper_l(unsigned int, void *);
int iswlower_l(unsigned int, void *);
int iswalpha_l(unsigned int, void *);
int iswdigit_l(unsigned int, void *);
int iswpunct_l(unsigned int, void *);
int iswxdigit_l(unsigned int, void *);
int iswblank_l(unsigned int, void *);

int toupper_l(int, void *);
int tolower_l(int, void *);

unsigned int towupper_l(unsigned int, void *);

unsigned int towlower_l(unsigned int, void *);

unsigned int btowc(int);
int wctob(unsigned int);

decltype(sizeof(0)) strxfrm_l(char *, const char *, decltype(sizeof(0)),
                              void *);

decltype(sizeof(0)) wcsxfrm_l(wchar_t *, const wchar_t *, decltype(sizeof(0)),
                              void *);

int mbtowc(wchar_t *, const char *, decltype(sizeof(0)));

int wctomb(char *, wchar_t);

//
// The native mbstate_t representation is deliberately opaque
// to FTL. codecvt_byname only supports stateless native
// encodings, so the restartable conversion state is always
// supplied as nullptr.
//
decltype(sizeof(0)) mbrtowc(wchar_t *, const char *, decltype(sizeof(0)),
                            void *);

#if defined(__APPLE__)

int ___mb_cur_max(void);

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

inline native_handle create_ctype(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  return newlocale(native_ctype_mask, name, nullptr);
}

inline native_handle create_collate(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  return newlocale(native_collate_mask, name, nullptr);
}

inline native_handle create_numeric(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  return newlocale(native_numeric_mask | native_ctype_mask, name, nullptr);
}

inline native_handle create_time(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  return newlocale(native_time_mask | native_ctype_mask, name, nullptr);
}

inline native_handle create_monetary(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  return newlocale(native_monetary_mask | native_ctype_mask, name, nullptr);
}

inline native_handle create_messages(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  return newlocale(native_messages_mask, name, nullptr);
}

inline native_catalog invalid_message_catalog() noexcept {
  return reinterpret_cast<native_catalog>(static_cast<__INTPTR_TYPE__>(-1));
}

inline bool message_catalog_valid(native_catalog catalog) noexcept {
  return catalog != invalid_message_catalog();
}

inline native_catalog open_message_catalog(native_handle locale,
                                           const char *name) noexcept {
  if (locale == nullptr || name == nullptr) {
    return invalid_message_catalog();
  }

  native_handle previous = uselocale(locale);

  if (previous == nullptr)
    return invalid_message_catalog();

  native_catalog result = ::catopen(name, NL_CAT_LOCALE);

  (void)uselocale(previous);

  return result;
}

inline const char *get_message_catalog(native_catalog catalog, int set,
                                       int message,
                                       const char *fallback) noexcept {
  return ::catgets(catalog, set, message, fallback);
}

inline void close_message_catalog(native_catalog catalog) noexcept {
  if (message_catalog_valid(catalog))
    (void)::catclose(catalog);
}

inline int multibyte_max_length(native_handle locale) noexcept {
  native_handle previous = uselocale(locale);

  if (previous == nullptr)
    return 1;

#if defined(__APPLE__)

  const int native_max = ___mb_cur_max();

  (void)uselocale(previous);

  return native_max > 0 ? native_max : 1;

#else

  const auto native_max = __ctype_get_mb_cur_max();

  (void)uselocale(previous);

  return native_max > 0 ? static_cast<int>(native_max) : 1;

#endif
}

inline bool multibyte_is_stateful(native_handle locale) noexcept {
  native_handle previous = uselocale(locale);

  if (previous == nullptr)
    return true;

  const int result = mbtowc(nullptr, nullptr, 0);

  (void)uselocale(previous);

  return result != 0;
}

inline decoded_wide decode_wide(native_handle locale, const char *first,
                                const char *last) noexcept {
  if (first == last) {
    return {decoded_wide::status::partial, wchar_t{}, 0};
  }

  native_handle previous = uselocale(locale);

  if (previous == nullptr) {
    return {decoded_wide::status::error, wchar_t{}, 0};
  }

  //
  // codecvt_byname rejects state-dependent native encodings,
  // so using mbrtowc's internal state is sufficient. Reset it
  // around each isolated scalar so a partial/error result never
  // contaminates the next call.
  //
  const char reset_sequence[] = "";

  (void)mbrtowc(nullptr, reset_sequence, 1, nullptr);

  wchar_t value{};

  const auto available = static_cast<decltype(sizeof(0))>(last - first);

  const auto result = mbrtowc(&value, first, available, nullptr);

  (void)mbrtowc(nullptr, reset_sequence, 1, nullptr);

  (void)uselocale(previous);

  constexpr auto conversion_error = static_cast<decltype(sizeof(0))>(-1);

  constexpr auto conversion_partial = static_cast<decltype(sizeof(0))>(-2);

  if (result == conversion_partial) {
    return {decoded_wide::status::partial, wchar_t{}, 0};
  }

  if (result == conversion_error) {
    return {decoded_wide::status::error, wchar_t{}, 0};
  }

  if (result == 0) {
    return {decoded_wide::status::complete, wchar_t{}, 1};
  }

  return {decoded_wide::status::complete, value, result};
}

inline encoded_wide encode_wide(native_handle locale, wchar_t value) noexcept {
  encoded_wide result{};

  native_handle previous = uselocale(locale);

  if (previous == nullptr)
    return result;

  (void)wctomb(nullptr, wchar_t{});

  const int produced = wctomb(result.bytes, value);

  (void)uselocale(previous);

  if (produced < 0)
    return result;

  result.valid = true;

  result.produced = static_cast<decltype(sizeof(0))>(produced);

  return result;
}

inline wchar_t widen_first(native_handle locale, const char *source,
                           wchar_t fallback) noexcept {
  if (source == nullptr || *source == '\0')
    return fallback;

  native_handle previous = uselocale(locale);

  if (previous == nullptr)
    return fallback;

  decltype(sizeof(0)) length = 0;

  while (source[length] != '\0')
    ++length;

  //
  // Reset mbtowc's conversion state before decoding this isolated
  // punctuation character.
  //
  (void)mbtowc(nullptr, nullptr, 0);

  wchar_t result{};

  const int converted = mbtowc(&result, source, length);

  (void)uselocale(previous);

  return converted > 0 ? result : fallback;
}

inline decltype(sizeof(0)) transform_byte(native_handle locale,
                                          char *destination, const char *source,
                                          decltype(sizeof(0)) count) noexcept {
  return strxfrm_l(destination, source, count, locale);
}

inline decltype(sizeof(0)) transform_wide(native_handle locale,
                                          wchar_t *destination,
                                          const wchar_t *source,
                                          decltype(sizeof(0)) count) noexcept {
  return wcsxfrm_l(destination, source, count, locale);
}

inline void destroy(native_handle locale) noexcept {
  if (locale == nullptr)
    return;

#if defined(__APPLE__)
  (void)freelocale(locale);
#else
  freelocale(locale);
#endif
}

inline bool classify_byte(native_handle locale, unsigned char value,
                          classification kind) noexcept {
  const int character = static_cast<int>(value);

  switch (kind) {
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
                          classification kind) noexcept {
  const unsigned int character = static_cast<unsigned int>(value);

  switch (kind) {
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

inline char toupper_byte(native_handle locale, char value) noexcept {
  return static_cast<char>(
      toupper_l(static_cast<unsigned char>(value), locale));
}

inline char tolower_byte(native_handle locale, char value) noexcept {
  return static_cast<char>(
      tolower_l(static_cast<unsigned char>(value), locale));
}

inline wchar_t toupper_wide(native_handle locale, wchar_t value) noexcept {
  return static_cast<wchar_t>(
      towupper_l(static_cast<unsigned int>(value), locale));
}

inline wchar_t tolower_wide(native_handle locale, wchar_t value) noexcept {
  return static_cast<wchar_t>(
      towlower_l(static_cast<unsigned int>(value), locale));
}

inline wchar_t widen(native_handle locale, char value) noexcept {
  native_handle previous = uselocale(locale);

  const unsigned int result = btowc(static_cast<unsigned char>(value));

  (void)uselocale(previous);

  if (result == static_cast<unsigned int>(-1)) {
    return static_cast<wchar_t>(static_cast<unsigned char>(value));
  }

  return static_cast<wchar_t>(result);
}

inline char narrow(native_handle locale, wchar_t value,
                   char fallback) noexcept {
  native_handle previous = uselocale(locale);

  const int result = wctob(static_cast<unsigned int>(value));

  (void)uselocale(previous);

  if (result < 0)
    return fallback;

  return static_cast<char>(result);
}

#endif

} // namespace ftl_locale_runtime

#endif
