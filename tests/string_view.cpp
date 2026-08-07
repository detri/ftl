#ifdef FTL_REPLACE_STL
#include <array>
#include <compare>
#include <functional>
#include <ranges>
#include <string_view>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/compare>
#include <ftl/functional>
#include <ftl/ranges>
#include <ftl/string_view>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

using namespace tested::literals::string_view_literals;

template <class T, class U> inline constexpr bool same_as_v = false;

template <class T> inline constexpr bool same_as_v<T, T> = true;

static_assert(tested::is_trivially_copyable_v<tested::string_view>);

static_assert(same_as_v<tested::string_view::iterator,
                        tested::string_view::const_iterator>);

static_assert(same_as_v<tested::string_view::reverse_iterator,
                        tested::string_view::const_reverse_iterator>);

static_assert(tested::ranges::enable_view<tested::string_view>);

static_assert(tested::ranges::enable_borrowed_range<tested::string_view>);

static_assert(tested::ranges::contiguous_range<tested::string_view>);

static_assert(tested::ranges::sized_range<tested::string_view>);

static_assert(tested::ranges::borrowed_range<tested::string_view>);

constexpr bool constructor_tests() {
  tested::string_view empty;

  if (empty.data() != nullptr || !empty.empty())
    return false;

  constexpr char text[] = "freestanding";

  tested::string_view from_pointer(text);

  if (from_pointer.size() != 12)
    return false;

  tested::string_view from_count(text, 4);

  if (from_count != "free")
    return false;

  tested::string_view from_iterators(text, text + 4);

  if (from_iterators != "free")
    return false;

  tested::array<char, 4> range{{'f', 't', 'l', '!'}};
  tested::string_view from_range(range);

  if (from_range != "ftl!")
    return false;

  tested::basic_string_view deduced(text, text + 4);

  static_assert(same_as_v<decltype(deduced), tested::basic_string_view<char>>);

  tested::basic_string_view deduced_range(range);

  static_assert(
      same_as_v<decltype(deduced_range), tested::basic_string_view<char>>);

  return true;
}

static_assert(constructor_tests());

constexpr bool iterator_tests() {
  constexpr tested::string_view value = "abcd";

  if (*value.begin() != 'a')
    return false;

  if (*(value.end() - 1) != 'd')
    return false;

  if (*value.rbegin() != 'd')
    return false;

  if (*(value.rend() - 1) != 'a')
    return false;

  return true;
}

static_assert(iterator_tests());

constexpr bool access_and_modifier_tests() {
  tested::string_view value = "abcdef";

  if (value[2] != 'c')
    return false;

  if (value.at(3) != 'd')
    return false;

  if (value.front() != 'a' || value.back() != 'f')
    return false;

  value.remove_prefix(1);
  value.remove_suffix(1);

  if (value != "bcde")
    return false;

  tested::string_view other = "xyz";
  value.swap(other);

  return value == "xyz" && other == "bcde";
}

static_assert(access_and_modifier_tests());

constexpr bool operation_tests() {
  constexpr tested::string_view value = "freestanding";

  char buffer[8]{};

  if (value.copy(buffer, 5, 4) != 5)
    return false;

  if (tested::string_view(buffer, 5) != "stand")
    return false;

  if (value.substr(4, 5) != "stand")
    return false;

  if (value.compare("freestanding") != 0)
    return false;

  if (value.compare("freestandinh") >= 0)
    return false;

  if (value.compare("freestandinf") <= 0)
    return false;

  if (value.compare(4, 5, "stand") != 0)
    return false;

  if (value.compare(4, 5, "xxstandyy", 2, 5) != 0)
    return false;

  if (value.compare(4, 5, "standalone", 5) != 0)
    return false;

  if (!value.starts_with("free"))
    return false;

  if (!value.starts_with('f'))
    return false;

  if (!value.ends_with("standing"))
    return false;

  if (!value.ends_with('g'))
    return false;

  if (!value.contains("stand"))
    return false;

  if (!value.contains('s'))
    return false;

  return true;
}

static_assert(operation_tests());

constexpr bool search_tests() {
  constexpr tested::string_view value = "bananas";

  if (value.find("ana") != 1)
    return false;

  if (value.find("ana", 2) != 3)
    return false;

  if (value.find('n') != 2)
    return false;

  if (value.find("nan", 0, 3) != 2)
    return false;

  if (value.find("", value.size()) != value.size())
    return false;

  if (value.find("", value.size() + 1) != tested::string_view::npos)
    return false;

  if (value.rfind("ana") != 3)
    return false;

  if (value.rfind('a') != 5)
    return false;

  if (value.rfind("", 3) != 3)
    return false;

  if (value.find_first_of("xyzna") != 1)
    return false;

  if (value.find_first_of('n') != 2)
    return false;

  if (value.find_last_of("ab") != 5)
    return false;

  if (value.find_last_of('a') != 5)
    return false;

  if (value.find_first_not_of("ab") != 2)
    return false;

  if (value.find_first_not_of('b') != 1)
    return false;

  if (value.find_last_not_of("as") != 4)
    return false;

  if (value.find_last_not_of('s') != 5)
    return false;

  return true;
}

static_assert(search_tests());

constexpr bool comparison_tests() {
  tested::string_view a = "abc";
  tested::string_view b = "abc";
  tested::string_view c = "abd";

  if (!(a == b))
    return false;

  if (a == c)
    return false;

  if (!(a < c))
    return false;

  if (!(c > a))
    return false;

  if (!("abc" == a))
    return false;

  if (!(a == "abc"))
    return false;

  if (!(a <=> b == tested::strong_ordering::equal))
    return false;

  return true;
}

static_assert(comparison_tests());

static_assert("abc"sv == tested::string_view("abc"));
static_assert(L"abc"sv == tested::wstring_view(L"abc"));
static_assert(u8"abc"sv == tested::u8string_view(u8"abc"));
static_assert(u"abc"sv == tested::u16string_view(u"abc"));
static_assert(U"abc"sv == tested::u32string_view(U"abc"));

constexpr bool hash_tests() {
  constexpr tested::string_view first = "hash me";
  constexpr tested::string_view second = "hash me";

  if (tested::hash<tested::string_view>{}(first) !=
      tested::hash<tested::string_view>{}(second))
    return false;

  constexpr tested::wstring_view wide = L"hash me";
  constexpr tested::u8string_view utf8 = u8"hash me";
  constexpr tested::u16string_view utf16 = u"hash me";
  constexpr tested::u32string_view utf32 = U"hash me";

  static_cast<void>(tested::hash<tested::wstring_view>{}(wide));

  static_cast<void>(tested::hash<tested::u8string_view>{}(utf8));

  static_cast<void>(tested::hash<tested::u16string_view>{}(utf16));

  static_cast<void>(tested::hash<tested::u32string_view>{}(utf32));

  return true;
}

static_assert(hash_tests());

bool ftl_test() {
#if FTL_HAS_EXCEPTIONS
  {
    bool threw = false;

    try {
      static_cast<void>(tested::string_view("abc").at(3));
    } catch (const tested::out_of_range &) {
      threw = true;
    }

    if (!threw)
      return false;
  }

  {
    bool threw = false;

    try {
      static_cast<void>(tested::string_view("abc").substr(4));
    } catch (const tested::out_of_range &) {
      threw = true;
    }

    if (!threw)
      return false;
  }

  {
    char destination[4]{};
    bool threw = false;

    try {
      static_cast<void>(tested::string_view("abc").copy(destination, 1, 4));
    } catch (const tested::out_of_range &) {
      threw = true;
    }

    if (!threw)
      return false;
  }
#endif

  return true;
}
