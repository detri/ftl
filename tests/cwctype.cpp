#ifdef FTL_REPLACE_STL
#include <cwctype>
namespace tested = std;
#else
#include <ftl/cwctype>
namespace tested = ftl;
#endif

#ifndef WEOF
#error "<cwctype> must define WEOF"
#endif

bool ftl_test() {
  if (!tested::iswalnum(L'A')) {
    return false;
  }

  if (!tested::iswalnum(L'7')) {
    return false;
  }

  if (tested::iswalnum(L'!')) {
    return false;
  }

  if (!tested::iswalpha(L'A') || !tested::iswalpha(L'z') ||
      tested::iswalpha(L'0')) {
    return false;
  }

  if (!tested::iswblank(L' ') || !tested::iswblank(L'\t') ||
      tested::iswblank(L'\n')) {
    return false;
  }

  if (!tested::iswcntrl(L'\0') || !tested::iswcntrl(L'\n') ||
      tested::iswcntrl(L'A')) {
    return false;
  }

  if (!tested::iswdigit(L'0') || !tested::iswdigit(L'9') ||
      tested::iswdigit(L'a')) {
    return false;
  }

  if (!tested::iswgraph(L'!') || !tested::iswgraph(L'A') ||
      tested::iswgraph(L' ')) {
    return false;
  }

  if (!tested::iswlower(L'a') || tested::iswlower(L'A')) {
    return false;
  }

  if (!tested::iswprint(L' ') || !tested::iswprint(L'~') ||
      tested::iswprint(L'\n')) {
    return false;
  }

  if (!tested::iswpunct(L'!') || !tested::iswpunct(L'_') ||
      tested::iswpunct(L'A') || tested::iswpunct(L'1')) {
    return false;
  }

  if (!tested::iswspace(L' ') || !tested::iswspace(L'\t') ||
      !tested::iswspace(L'\n') || tested::iswspace(L'A')) {
    return false;
  }

  if (!tested::iswupper(L'A') || tested::iswupper(L'a')) {
    return false;
  }

  if (!tested::iswxdigit(L'0') || !tested::iswxdigit(L'9') ||
      !tested::iswxdigit(L'a') || !tested::iswxdigit(L'F') ||
      tested::iswxdigit(L'g')) {
    return false;
  }

  const auto alpha = tested::wctype("alpha");

  if (alpha == 0) {
    return false;
  }

  if (!tested::iswctype(L'A', alpha) || tested::iswctype(L'0', alpha)) {
    return false;
  }

  if (tested::wctype("does-not-exist") != 0) {
    return false;
  }

  if (tested::towlower(L'A') != L'a' || tested::towlower(L'!') != L'!') {
    return false;
  }

  if (tested::towupper(L'a') != L'A' || tested::towupper(L'!') != L'!') {
    return false;
  }

  const auto lower = tested::wctrans("tolower");

  const auto upper = tested::wctrans("toupper");

  if (lower == 0 || upper == 0) {
    return false;
  }

  if (tested::towctrans(L'A', lower) != L'a' ||
      tested::towctrans(L'a', upper) != L'A') {
    return false;
  }

  if (tested::wctrans("nope") != 0) {
    return false;
  }

  if (tested::iswalnum(static_cast<tested::wint_t>(WEOF))) {
    return false;
  }

  return true;
}
