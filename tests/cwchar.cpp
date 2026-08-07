#ifdef FTL_REPLACE_STL
#include <cwchar>
namespace tested = std;
#else
#include <ftl/cwchar>
namespace tested = ftl;
#endif

bool ftl_test() {
  // Foundational types/macros.
  {
    tested::mbstate_t state{};
    static_cast<void>(state);

    tested::wint_t eof = WEOF;

    if (eof == static_cast<tested::wint_t>(L'A')) {
      return false;
    }

    if (WCHAR_MIN > L'A' || WCHAR_MAX < L'A') {
      return false;
    }
  }

  // wcscpy.
  {
    wchar_t destination[8]{};

    if (tested::wcscpy(destination, L"ftl") != destination) {
      return false;
    }

    if (tested::wcscmp(destination, L"ftl") != 0) {
      return false;
    }
  }

  // wcsncpy: zero padding.
  {
    wchar_t destination[6] = {L'x', L'x', L'x', L'x', L'x', L'x'};

    if (tested::wcsncpy(destination, L"hi", 5) != destination) {
      return false;
    }

    if (destination[0] != L'h' || destination[1] != L'i' ||
        destination[2] != L'\0' || destination[3] != L'\0' ||
        destination[4] != L'\0' || destination[5] != L'x') {
      return false;
    }
  }

  // wcsncpy: truncation does not terminate.
  {
    wchar_t destination[4] = {L'x', L'x', L'x', L'x'};

    tested::wcsncpy(destination, L"abcdef", 3);

    if (destination[0] != L'a' || destination[1] != L'b' ||
        destination[2] != L'c' || destination[3] != L'x') {
      return false;
    }
  }

  // wmemcpy.
  {
    const wchar_t source[] = {L'a', L'b', L'c', L'd'};
    wchar_t destination[4]{};

    if (tested::wmemcpy(destination, source, 4) != destination) {
      return false;
    }

    if (tested::wmemcmp(destination, source, 4) != 0) {
      return false;
    }
  }

  // wmemmove: overlapping toward higher addresses.
  {
    wchar_t value[] = L"abcdef";

    tested::wmemmove(value + 2, value, 4);

    if (tested::wcscmp(value, L"ababcd") != 0) {
      return false;
    }
  }

  // wmemmove: overlapping toward lower addresses.
  {
    wchar_t value[] = L"abcdef";

    tested::wmemmove(value, value + 2, 4);

    if (tested::wcscmp(value, L"cdefef") != 0) {
      return false;
    }
  }

  // wcscat.
  {
    wchar_t value[16] = L"ft";

    if (tested::wcscat(value, L"l") != value) {
      return false;
    }

    if (tested::wcscmp(value, L"ftl") != 0) {
      return false;
    }
  }

  // wcsncat.
  {
    wchar_t value[16] = L"ab";

    if (tested::wcsncat(value, L"cdef", 2) != value) {
      return false;
    }

    if (tested::wcscmp(value, L"abcd") != 0) {
      return false;
    }
  }

  // Comparison.
  {
    if (tested::wcscmp(L"abc", L"abc") != 0) {
      return false;
    }

    if (tested::wcscmp(L"abc", L"abd") >= 0) {
      return false;
    }

    if (tested::wcscmp(L"abd", L"abc") <= 0) {
      return false;
    }

    if (tested::wcsncmp(L"abcdef", L"abcxyz", 3) != 0) {
      return false;
    }

    if (tested::wcsncmp(L"abcdef", L"abcxyz", 4) >= 0) {
      return false;
    }
  }

  // C-locale collation.
  {
    if (tested::wcscoll(L"abc", L"abd") >= 0) {
      return false;
    }

    if (tested::wcscoll(L"same", L"same") != 0) {
      return false;
    }
  }

  // C-locale transformation.
  {
    wchar_t transformed[8]{};

    if (tested::wcsxfrm(transformed, L"abc", 8) != 3) {
      return false;
    }

    if (tested::wcscmp(transformed, L"abc") != 0) {
      return false;
    }
  }

  // wmemcmp.
  {
    const wchar_t lower[] = {L'a', L'b'};
    const wchar_t equal[] = {L'a', L'b'};
    const wchar_t higher[] = {L'a', L'c'};

    if (tested::wmemcmp(lower, equal, 2) != 0) {
      return false;
    }

    if (tested::wmemcmp(lower, higher, 2) >= 0) {
      return false;
    }

    if (tested::wmemcmp(higher, lower, 2) <= 0) {
      return false;
    }
  }

  // Wide integer conversion.
  {
    wchar_t *end = nullptr;

    if (tested::wcstol(L"123", &end, 10) != 123) {
      return false;
    }

    if (end == nullptr || *end != L'\0') {
      return false;
    }
  }

  {
    const wchar_t input[] = L" \t-42xyz";
    wchar_t *end = nullptr;

    if (tested::wcstol(input, &end, 10) != -42) {
      return false;
    }

    if (end != input + 5 || *end != L'x') {
      return false;
    }
  }

  // Automatic base selection.
  {
    wchar_t *end = nullptr;

    if (tested::wcstol(L"077", &end, 0) != 63) {
      return false;
    }

    if (end == nullptr || *end != L'\0') {
      return false;
    }

    if (tested::wcstol(L"0x2a", &end, 0) != 42) {
      return false;
    }

    if (end == nullptr || *end != L'\0') {
      return false;
    }
  }

  // Explicit hexadecimal prefix.
  {
    wchar_t *end = nullptr;

    if (tested::wcstoll(L"0X7f!", &end, 16) != 127) {
      return false;
    }

    if (end == nullptr || *end != L'!') {
      return false;
    }
  }

  // Bases through 36.
  {
    wchar_t *end = nullptr;

    if (tested::wcstoull(L"z", &end, 36) != 35ull) {
      return false;
    }

    if (end == nullptr || *end != L'\0') {
      return false;
    }
  }

  // Unsigned conversion accepts a leading minus and negates
  // the converted value in the result type.
  {
    wchar_t *end = nullptr;

    if (tested::wcstoul(L"-1", &end, 10) != ~0ul) {
      return false;
    }

    if (end == nullptr || *end != L'\0') {
      return false;
    }
  }

  // No conversion leaves endptr at the original input.
  {
    const wchar_t input[] = L"xyz";
    wchar_t *end = nullptr;

    if (tested::wcstol(input, &end, 10) != 0) {
      return false;
    }

    if (end != input) {
      return false;
    }
  }

  // "0x" without a following hexadecimal digit consumes only
  // the leading zero under base autodetection.
  {
    const wchar_t input[] = L"0x";
    wchar_t *end = nullptr;

    if (tested::wcstol(input, &end, 0) != 0) {
      return false;
    }

    if (end != input + 1 || *end != L'x') {
      return false;
    }
  }

  // wcschr.
  {
    wchar_t mutable_value[] = L"banana";
    const wchar_t const_value[] = L"banana";

    wchar_t *mutable_result = tested::wcschr(mutable_value, L'n');
    const wchar_t *const_result = tested::wcschr(const_value, L'n');

    if (mutable_result != mutable_value + 2) {
      return false;
    }

    if (const_result != const_value + 2) {
      return false;
    }

    if (tested::wcschr(const_value, L'\0') != const_value + 6) {
      return false;
    }
  }

  // wcscspn.
  {
    if (tested::wcscspn(L"abc123", L"0123456789") != 3) {
      return false;
    }
  }

  // wcspbrk.
  {
    wchar_t mutable_value[] = L"hello";
    const wchar_t const_value[] = L"hello";

    if (tested::wcspbrk(mutable_value, L"xyzol") != mutable_value + 2) {
      return false;
    }

    if (tested::wcspbrk(const_value, L"xyzol") != const_value + 2) {
      return false;
    }

    if (tested::wcspbrk(const_value, L"xyz") != nullptr) {
      return false;
    }
  }

  // wcsrchr.
  {
    wchar_t mutable_value[] = L"banana";
    const wchar_t const_value[] = L"banana";

    if (tested::wcsrchr(mutable_value, L'a') != mutable_value + 5) {
      return false;
    }

    if (tested::wcsrchr(const_value, L'a') != const_value + 5) {
      return false;
    }

    if (tested::wcsrchr(const_value, L'\0') != const_value + 6) {
      return false;
    }
  }

  // wcsspn.
  {
    if (tested::wcsspn(L"aaab", L"a") != 3) {
      return false;
    }

    if (tested::wcsspn(L"abc123", L"abc") != 3) {
      return false;
    }
  }

  // wcsstr.
  {
    wchar_t mutable_value[] = L"banana";
    const wchar_t const_value[] = L"banana";

    if (tested::wcsstr(mutable_value, L"ana") != mutable_value + 1) {
      return false;
    }

    if (tested::wcsstr(const_value, L"ana") != const_value + 1) {
      return false;
    }

    if (tested::wcsstr(const_value, L"") != const_value) {
      return false;
    }

    if (tested::wcsstr(const_value, L"xyz") != nullptr) {
      return false;
    }
  }

  // wcstok.
  {
    wchar_t value[] = L"::alpha:beta::gamma";
    wchar_t *context = nullptr;

    wchar_t *token = tested::wcstok(value, L":", &context);

    if (token == nullptr || tested::wcscmp(token, L"alpha") != 0) {
      return false;
    }

    token = tested::wcstok(nullptr, L":", &context);

    if (token == nullptr || tested::wcscmp(token, L"beta") != 0) {
      return false;
    }

    token = tested::wcstok(nullptr, L":", &context);

    if (token == nullptr || tested::wcscmp(token, L"gamma") != 0) {
      return false;
    }

    if (tested::wcstok(nullptr, L":", &context) != nullptr) {
      return false;
    }
  }

  // wmemchr.
  {
    wchar_t mutable_value[] = {L'a', L'b', L'c', L'd'};
    const wchar_t const_value[] = {L'a', L'b', L'c', L'd'};

    if (tested::wmemchr(mutable_value, L'c', 4) != mutable_value + 2) {
      return false;
    }

    if (tested::wmemchr(const_value, L'c', 4) != const_value + 2) {
      return false;
    }

    if (tested::wmemchr(const_value, L'x', 4) != nullptr) {
      return false;
    }
  }

  // wcslen.
  {
    if (tested::wcslen(L"") != 0) {
      return false;
    }

    if (tested::wcslen(L"ftl") != 3) {
      return false;
    }
  }

  // wmemset.
  {
    wchar_t value[5]{};

    if (tested::wmemset(value, L'x', 5) != value) {
      return false;
    }

    for (wchar_t character : value) {
      if (character != L'x') {
        return false;
      }
    }
  }

  return true;
}