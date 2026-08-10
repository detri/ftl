// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_LOCALE_RUNTIME_HPP
#define FTL_DETAIL_LOCALE_RUNTIME_HPP

#ifdef FTL_REPLACE_STL
#include <locale.h>
#else
#include <ftl/locale.h>
#endif

namespace ftl_locale_runtime {

using native_handle = void *;

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

} // extern "C"

inline native_handle create_ctype(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  constexpr int ctype_mask = 1 << LC_CTYPE;

  return newlocale(ctype_mask, name, nullptr);
}

inline native_handle create_collate(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  constexpr int collate_mask = 1 << LC_COLLATE;

  return newlocale(collate_mask, name, nullptr);
}

inline native_handle create_numeric(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  constexpr int numeric_and_ctype_mask = (1 << LC_NUMERIC) | (1 << LC_CTYPE);

  return newlocale(numeric_and_ctype_mask, name, nullptr);
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
