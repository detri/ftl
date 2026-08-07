#ifdef FTL_REPLACE_STL
#include <cwchar>
namespace tested = std;
#else
#include <ftl/cwchar>
namespace tested = ftl;
#endif

#ifndef NULL
#error "<cwchar> must define NULL"
#endif

#ifndef WCHAR_MIN
#error "<cwchar> must define WCHAR_MIN"
#endif

#ifndef WCHAR_MAX
#error "<cwchar> must define WCHAR_MAX"
#endif

#ifndef WEOF
#error "<cwchar> must define WEOF"
#endif

template <class T, class U> inline constexpr bool same_as_v = false;

template <class T> inline constexpr bool same_as_v<T, T> = true;

static_assert(same_as_v<tested::size_t, decltype(sizeof(0))>);

static_assert(sizeof(tested::mbstate_t) != 0);
static_assert(sizeof(tested::wint_t) != 0);
static_assert(sizeof(tested::tm) != 0);

using fwprintf_type = int (*)(FILE *, const wchar_t *, ...);

using fwscanf_type = int (*)(FILE *, const wchar_t *, ...);

using swprintf_type = int (*)(wchar_t *, tested::size_t, const wchar_t *, ...);

using swscanf_type = int (*)(const wchar_t *, const wchar_t *, ...);

using vfwprintf_type = int (*)(FILE *, const wchar_t *, tested::va_list);

using vfwscanf_type = int (*)(FILE *, const wchar_t *, tested::va_list);

using vswprintf_type = int (*)(wchar_t *, tested::size_t, const wchar_t *,
                               tested::va_list);

using vswscanf_type = int (*)(const wchar_t *, const wchar_t *,
                              tested::va_list);

using vwprintf_type = int (*)(const wchar_t *, tested::va_list);

using vwscanf_type = int (*)(const wchar_t *, tested::va_list);

using wprintf_type = int (*)(const wchar_t *, ...);

using wscanf_type = int (*)(const wchar_t *, ...);

using fgetwc_type = tested::wint_t (*)(FILE *);

using fgetws_type = wchar_t *(*)(wchar_t *, int, FILE *);

using fputwc_type = tested::wint_t (*)(wchar_t, FILE *);

using fputws_type = int (*)(const wchar_t *, FILE *);

using fwide_type = int (*)(FILE *, int);

using getwc_type = tested::wint_t (*)(FILE *);

using getwchar_type = tested::wint_t (*)();

using putwc_type = tested::wint_t (*)(wchar_t, FILE *);

using putwchar_type = tested::wint_t (*)(wchar_t);

using ungetwc_type = tested::wint_t (*)(tested::wint_t, FILE *);

using wcstod_type = double (*)(const wchar_t *, wchar_t **);

using wcstof_type = float (*)(const wchar_t *, wchar_t **);

using wcstold_type = long double (*)(const wchar_t *, wchar_t **);

using wcstol_type = long (*)(const wchar_t *, wchar_t **, int);

using wcstoll_type = long long (*)(const wchar_t *, wchar_t **, int);

using wcstoul_type = unsigned long (*)(const wchar_t *, wchar_t **, int);

using wcstoull_type = unsigned long long (*)(const wchar_t *, wchar_t **, int);

using wcscpy_type = wchar_t *(*)(wchar_t *, const wchar_t *);

using wcsncpy_type = wchar_t *(*)(wchar_t *, const wchar_t *, tested::size_t);

using wmemcpy_type = wchar_t *(*)(wchar_t *, const wchar_t *, tested::size_t);

using wmemmove_type = wchar_t *(*)(wchar_t *, const wchar_t *, tested::size_t);

using wcscat_type = wchar_t *(*)(wchar_t *, const wchar_t *);

using wcsncat_type = wchar_t *(*)(wchar_t *, const wchar_t *, tested::size_t);

using wcscmp_type = int (*)(const wchar_t *, const wchar_t *);

using wcscoll_type = int (*)(const wchar_t *, const wchar_t *);

using wcsncmp_type = int (*)(const wchar_t *, const wchar_t *, tested::size_t);

using wcsxfrm_type = tested::size_t (*)(wchar_t *, const wchar_t *,
                                        tested::size_t);

using wmemcmp_type = int (*)(const wchar_t *, const wchar_t *, tested::size_t);

using const_wcschr_type = const wchar_t *(*)(const wchar_t *, wchar_t);

using mutable_wcschr_type = wchar_t *(*)(wchar_t *, wchar_t);

using wcscspn_type = tested::size_t (*)(const wchar_t *, const wchar_t *);

using const_wcspbrk_type = const wchar_t *(*)(const wchar_t *, const wchar_t *);

using mutable_wcspbrk_type = wchar_t *(*)(wchar_t *, const wchar_t *);

using const_wcsrchr_type = const wchar_t *(*)(const wchar_t *, wchar_t);

using mutable_wcsrchr_type = wchar_t *(*)(wchar_t *, wchar_t);

using wcsspn_type = tested::size_t (*)(const wchar_t *, const wchar_t *);

using const_wcsstr_type = const wchar_t *(*)(const wchar_t *, const wchar_t *);

using mutable_wcsstr_type = wchar_t *(*)(wchar_t *, const wchar_t *);

using wcstok_type = wchar_t *(*)(wchar_t *, const wchar_t *, wchar_t **);

using const_wmemchr_type = const wchar_t *(*)(const wchar_t *, wchar_t,
                                              tested::size_t);

using mutable_wmemchr_type = wchar_t *(*)(wchar_t *, wchar_t, tested::size_t);

using wcslen_type = tested::size_t (*)(const wchar_t *);

using wmemset_type = wchar_t *(*)(wchar_t *, wchar_t, tested::size_t);

using wcsftime_type = tested::size_t (*)(wchar_t *, tested::size_t,
                                         const wchar_t *, const tested::tm *);

using btowc_type = tested::wint_t (*)(int);

using wctob_type = int (*)(tested::wint_t);

using mbsinit_type = int (*)(const tested::mbstate_t *);

using mbrlen_type = tested::size_t (*)(const char *, tested::size_t,
                                       tested::mbstate_t *);

using mbrtowc_type = tested::size_t (*)(wchar_t *, const char *, tested::size_t,
                                        tested::mbstate_t *);

using wcrtomb_type = tested::size_t (*)(char *, wchar_t, tested::mbstate_t *);

using mbsrtowcs_type = tested::size_t (*)(wchar_t *, const char **,
                                          tested::size_t, tested::mbstate_t *);

using wcsrtombs_type = tested::size_t (*)(char *, const wchar_t **,
                                          tested::size_t, tested::mbstate_t *);

#define FTL_CHECK_FUNCTION(name, type)                                         \
  [[maybe_unused]] constexpr type name##_pointer = &tested::name

FTL_CHECK_FUNCTION(fwprintf, fwprintf_type);
FTL_CHECK_FUNCTION(fwscanf, fwscanf_type);
FTL_CHECK_FUNCTION(swprintf, swprintf_type);
FTL_CHECK_FUNCTION(swscanf, swscanf_type);
FTL_CHECK_FUNCTION(vfwprintf, vfwprintf_type);
FTL_CHECK_FUNCTION(vfwscanf, vfwscanf_type);
FTL_CHECK_FUNCTION(vswprintf, vswprintf_type);
FTL_CHECK_FUNCTION(vswscanf, vswscanf_type);
FTL_CHECK_FUNCTION(vwprintf, vwprintf_type);
FTL_CHECK_FUNCTION(vwscanf, vwscanf_type);
FTL_CHECK_FUNCTION(wprintf, wprintf_type);
FTL_CHECK_FUNCTION(wscanf, wscanf_type);
FTL_CHECK_FUNCTION(fgetwc, fgetwc_type);
FTL_CHECK_FUNCTION(fgetws, fgetws_type);
FTL_CHECK_FUNCTION(fputwc, fputwc_type);
FTL_CHECK_FUNCTION(fputws, fputws_type);
FTL_CHECK_FUNCTION(fwide, fwide_type);
FTL_CHECK_FUNCTION(getwc, getwc_type);
FTL_CHECK_FUNCTION(getwchar, getwchar_type);
FTL_CHECK_FUNCTION(putwc, putwc_type);
FTL_CHECK_FUNCTION(putwchar, putwchar_type);
FTL_CHECK_FUNCTION(ungetwc, ungetwc_type);
FTL_CHECK_FUNCTION(wcstod, wcstod_type);
FTL_CHECK_FUNCTION(wcstof, wcstof_type);
FTL_CHECK_FUNCTION(wcstold, wcstold_type);
FTL_CHECK_FUNCTION(wcstol, wcstol_type);
FTL_CHECK_FUNCTION(wcstoll, wcstoll_type);
FTL_CHECK_FUNCTION(wcstoul, wcstoul_type);
FTL_CHECK_FUNCTION(wcstoull, wcstoull_type);
FTL_CHECK_FUNCTION(wcscpy, wcscpy_type);
FTL_CHECK_FUNCTION(wcsncpy, wcsncpy_type);
FTL_CHECK_FUNCTION(wmemcpy, wmemcpy_type);
FTL_CHECK_FUNCTION(wmemmove, wmemmove_type);
FTL_CHECK_FUNCTION(wcscat, wcscat_type);
FTL_CHECK_FUNCTION(wcsncat, wcsncat_type);
FTL_CHECK_FUNCTION(wcscmp, wcscmp_type);
FTL_CHECK_FUNCTION(wcscoll, wcscoll_type);
FTL_CHECK_FUNCTION(wcsncmp, wcsncmp_type);
FTL_CHECK_FUNCTION(wcsxfrm, wcsxfrm_type);
FTL_CHECK_FUNCTION(wmemcmp, wmemcmp_type);
FTL_CHECK_FUNCTION(wcscspn, wcscspn_type);
FTL_CHECK_FUNCTION(wcsspn, wcsspn_type);
FTL_CHECK_FUNCTION(wcstok, wcstok_type);
FTL_CHECK_FUNCTION(wcslen, wcslen_type);
FTL_CHECK_FUNCTION(wmemset, wmemset_type);
FTL_CHECK_FUNCTION(wcsftime, wcsftime_type);
FTL_CHECK_FUNCTION(btowc, btowc_type);
FTL_CHECK_FUNCTION(wctob, wctob_type);
FTL_CHECK_FUNCTION(mbsinit, mbsinit_type);
FTL_CHECK_FUNCTION(mbrlen, mbrlen_type);
FTL_CHECK_FUNCTION(mbrtowc, mbrtowc_type);
FTL_CHECK_FUNCTION(wcrtomb, wcrtomb_type);
FTL_CHECK_FUNCTION(mbsrtowcs, mbsrtowcs_type);
FTL_CHECK_FUNCTION(wcsrtombs, wcsrtombs_type);

#undef FTL_CHECK_FUNCTION

[[maybe_unused]]
constexpr const_wcschr_type const_wcschr_pointer =
    static_cast<const_wcschr_type>(&tested::wcschr);

[[maybe_unused]]
constexpr mutable_wcschr_type mutable_wcschr_pointer =
    static_cast<mutable_wcschr_type>(&tested::wcschr);

[[maybe_unused]]
constexpr const_wcspbrk_type const_wcspbrk_pointer =
    static_cast<const_wcspbrk_type>(&tested::wcspbrk);

[[maybe_unused]]
constexpr mutable_wcspbrk_type mutable_wcspbrk_pointer =
    static_cast<mutable_wcspbrk_type>(&tested::wcspbrk);

[[maybe_unused]]
constexpr const_wcsrchr_type const_wcsrchr_pointer =
    static_cast<const_wcsrchr_type>(&tested::wcsrchr);

[[maybe_unused]]
constexpr mutable_wcsrchr_type mutable_wcsrchr_pointer =
    static_cast<mutable_wcsrchr_type>(&tested::wcsrchr);

[[maybe_unused]]
constexpr const_wcsstr_type const_wcsstr_pointer =
    static_cast<const_wcsstr_type>(&tested::wcsstr);

[[maybe_unused]]
constexpr mutable_wcsstr_type mutable_wcsstr_pointer =
    static_cast<mutable_wcsstr_type>(&tested::wcsstr);

[[maybe_unused]]
constexpr const_wmemchr_type const_wmemchr_pointer =
    static_cast<const_wmemchr_type>(&tested::wmemchr);

[[maybe_unused]]
constexpr mutable_wmemchr_type mutable_wmemchr_pointer =
    static_cast<mutable_wmemchr_type>(&tested::wmemchr);

bool ftl_test() {
  void *null_pointer = NULL;

  return null_pointer == nullptr && WCHAR_MIN <= L'A' && WCHAR_MAX >= L'A' &&
         static_cast<tested::wint_t>(WEOF) != static_cast<tested::wint_t>(L'A');
}
