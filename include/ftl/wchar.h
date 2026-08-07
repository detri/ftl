#ifdef FTL_REPLACE_STL
#include <cwchar>
#else
#include <ftl/cwchar>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_WCHAR_C_NAMESPACE std
#else
#define FTL_WCHAR_C_NAMESPACE ftl
#endif

using FTL_WCHAR_C_NAMESPACE::mbstate_t;
using FTL_WCHAR_C_NAMESPACE::size_t;
using FTL_WCHAR_C_NAMESPACE::tm;
using FTL_WCHAR_C_NAMESPACE::wint_t;

using FTL_WCHAR_C_NAMESPACE::fwprintf;
using FTL_WCHAR_C_NAMESPACE::fwscanf;
using FTL_WCHAR_C_NAMESPACE::swprintf;
using FTL_WCHAR_C_NAMESPACE::swscanf;
using FTL_WCHAR_C_NAMESPACE::vfwprintf;
using FTL_WCHAR_C_NAMESPACE::vfwscanf;
using FTL_WCHAR_C_NAMESPACE::vswprintf;
using FTL_WCHAR_C_NAMESPACE::vswscanf;
using FTL_WCHAR_C_NAMESPACE::vwprintf;
using FTL_WCHAR_C_NAMESPACE::vwscanf;
using FTL_WCHAR_C_NAMESPACE::wprintf;
using FTL_WCHAR_C_NAMESPACE::wscanf;

using FTL_WCHAR_C_NAMESPACE::fgetwc;
using FTL_WCHAR_C_NAMESPACE::fgetws;
using FTL_WCHAR_C_NAMESPACE::fputwc;
using FTL_WCHAR_C_NAMESPACE::fputws;
using FTL_WCHAR_C_NAMESPACE::fwide;
using FTL_WCHAR_C_NAMESPACE::getwc;
using FTL_WCHAR_C_NAMESPACE::getwchar;
using FTL_WCHAR_C_NAMESPACE::putwc;
using FTL_WCHAR_C_NAMESPACE::putwchar;
using FTL_WCHAR_C_NAMESPACE::ungetwc;

using FTL_WCHAR_C_NAMESPACE::wcstod;
using FTL_WCHAR_C_NAMESPACE::wcstof;
using FTL_WCHAR_C_NAMESPACE::wcstol;
using FTL_WCHAR_C_NAMESPACE::wcstold;
using FTL_WCHAR_C_NAMESPACE::wcstoll;
using FTL_WCHAR_C_NAMESPACE::wcstoul;
using FTL_WCHAR_C_NAMESPACE::wcstoull;

using FTL_WCHAR_C_NAMESPACE::wcscat;
using FTL_WCHAR_C_NAMESPACE::wcschr;
using FTL_WCHAR_C_NAMESPACE::wcscmp;
using FTL_WCHAR_C_NAMESPACE::wcscoll;
using FTL_WCHAR_C_NAMESPACE::wcscpy;
using FTL_WCHAR_C_NAMESPACE::wcscspn;
using FTL_WCHAR_C_NAMESPACE::wcsftime;
using FTL_WCHAR_C_NAMESPACE::wcslen;
using FTL_WCHAR_C_NAMESPACE::wcsncat;
using FTL_WCHAR_C_NAMESPACE::wcsncmp;
using FTL_WCHAR_C_NAMESPACE::wcsncpy;
using FTL_WCHAR_C_NAMESPACE::wcspbrk;
using FTL_WCHAR_C_NAMESPACE::wcsrchr;
using FTL_WCHAR_C_NAMESPACE::wcsspn;
using FTL_WCHAR_C_NAMESPACE::wcsstr;
using FTL_WCHAR_C_NAMESPACE::wcstok;
using FTL_WCHAR_C_NAMESPACE::wcsxfrm;
using FTL_WCHAR_C_NAMESPACE::wmemchr;
using FTL_WCHAR_C_NAMESPACE::wmemcmp;
using FTL_WCHAR_C_NAMESPACE::wmemcpy;
using FTL_WCHAR_C_NAMESPACE::wmemmove;
using FTL_WCHAR_C_NAMESPACE::wmemset;

using FTL_WCHAR_C_NAMESPACE::btowc;
using FTL_WCHAR_C_NAMESPACE::mbrlen;
using FTL_WCHAR_C_NAMESPACE::mbrtowc;
using FTL_WCHAR_C_NAMESPACE::mbsinit;
using FTL_WCHAR_C_NAMESPACE::mbsrtowcs;
using FTL_WCHAR_C_NAMESPACE::wcrtomb;
using FTL_WCHAR_C_NAMESPACE::wcsrtombs;
using FTL_WCHAR_C_NAMESPACE::wctob;

#undef FTL_WCHAR_C_NAMESPACE
