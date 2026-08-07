#include <cwchar>

namespace {

volatile unsigned runtime_seed = 11;

} // namespace

extern "C" int ftl_entry() {
  wchar_t source[64]{};
  wchar_t destination[64]{};

  const unsigned seed = runtime_seed;

  const std::size_t count = static_cast<std::size_t>((seed & 15u) + 16u);

  for (std::size_t index = 0; index < count; ++index) {
    source[index] = static_cast<wchar_t>(L'a' + ((seed + index) % 26u));
  }

  std::wmemcpy(destination, source, count);

  if (std::wmemcmp(destination, source, count) != 0) {
    return 1;
  }

  std::wmemmove(destination + 1, destination, count);

  if (std::wmemcmp(destination + 1, source, count) != 0) {
    return 2;
  }

  std::wmemset(destination, L'x', count);

  for (std::size_t index = 0; index < count; ++index) {
    if (destination[index] != L'x') {
      return 3;
    }
  }

  wchar_t string[64]{};

  const std::size_t length = static_cast<std::size_t>((seed & 15u) + 1u);

  for (std::size_t index = 0; index < length; ++index) {
    string[index] = L'a';
  }

  string[length] = L'\0';

  if (std::wcslen(string) != length) {
    return 4;
  }

  wchar_t formatted[64]{};

  if (std::swprintf(formatted, 64, L"%08x %.2f", seed, 1.25) <= 0) {
    return 5;
  }

  return 0;
}