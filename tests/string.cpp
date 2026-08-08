#ifdef FTL_REPLACE_STL
#include <array>
#include <compare>
#include <memory>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/compare>
#include <ftl/memory>
#include <ftl/ranges>
#include <ftl/string>
#include <ftl/string_view>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

using namespace tested::literals::string_literals;

template <class T, class U> inline constexpr bool same_as_v = false;
template <class T> inline constexpr bool same_as_v<T, T> = true;

static_assert(__cpp_lib_constexpr_string >= 201907L);
static_assert(__cpp_lib_string_contains >= 202011L);
static_assert(__cpp_lib_string_resize_and_overwrite >= 202110L);
static_assert(__cpp_lib_containers_ranges >= 202202L);

static_assert(same_as_v<tested::string::value_type, char>);
static_assert(same_as_v<tested::string::traits_type, tested::char_traits<char>>);
static_assert(same_as_v<tested::string::allocator_type,
                        tested::allocator<char>>);
static_assert(same_as_v<tested::string::iterator, char *>);
static_assert(same_as_v<tested::string::const_iterator, const char *>);
static_assert(tested::ranges::contiguous_range<tested::string>);
static_assert(tested::ranges::sized_range<tested::string>);
static_assert(!tested::ranges::borrowed_range<tested::string>);
static_assert(noexcept(tested::string{}));
static_assert(noexcept(tested::string(tested::string{})));
static_assert(noexcept(tested::declval<tested::string &>().clear()));
static_assert(noexcept(tested::declval<tested::string &>().swap(
    tested::declval<tested::string &>())));
static_assert(!tested::is_constructible_v<tested::string, tested::nullptr_t>);
static_assert(!tested::is_assignable_v<tested::string &, tested::nullptr_t>);

constexpr bool construction_and_sso() {
  tested::string empty;
  if (!empty.empty() || empty.data() == nullptr || empty.data()[0] != '\0')
    return false;

  tested::string short_value("short");
  if (short_value != "short" || short_value.capacity() < short_value.size())
    return false;

  const char *short_storage = short_value.data();
  tested::string moved_short(tested::move(short_value));
  if (moved_short != "short" || !short_value.empty())
    return false;
  if (moved_short.data() == short_storage)
    return false; // inline storage belongs to the destination object

#if !defined(_MSC_VER) || defined(FTL_REPLACE_STL)
  tested::string long_value("abcdefghijklmnopqrstuvwxyz0123456789");
  const char *long_storage = long_value.data();
  tested::string moved_long(tested::move(long_value));
  if (moved_long.data() != long_storage || !long_value.empty())
    return false;
#endif

  tested::string counted("a\0b", 3);
  if (counted.size() != 3 || counted[1] != '\0' || counted[2] != 'b')
    return false;

  tested::string copies(4, 'x');
  if (copies != "xxxx")
    return false;

  tested::array<char, 4> range{{'f', 't', 'l', '!'}};
  tested::string from_range(tested::from_range, range);
  tested::basic_string deduced(range.begin(), range.end());
  tested::basic_string deduced_range(tested::from_range, range);
  static_assert(same_as_v<decltype(deduced), tested::string>);
  static_assert(same_as_v<decltype(deduced_range), tested::string>);
  return from_range == "ftl!" && deduced == from_range &&
         deduced_range == from_range;
}
static_assert(construction_and_sso());

constexpr bool capacity_and_access() {
  tested::string value = "abc";
  const auto old_capacity = value.capacity();
#if !defined(_MSC_VER) || defined(FTL_REPLACE_STL)
  value.reserve(old_capacity + 10);
  if (value != "abc" || value.capacity() < old_capacity + 10)
    return false;
#else
  value.reserve(old_capacity);
#endif
  value.resize(6, 'x');
  if (value != "abcxxx" || value.front() != 'a' || value.back() != 'x')
    return false;
  value.resize(2);
  if (value != "ab" || value.data()[2] != '\0')
    return false;
  value.resize_and_overwrite(8, [](char *buffer, tested::size_t count) {
    for (tested::size_t i = 0; i < count; ++i)
      buffer[i] = static_cast<char>('0' + i);
    return tested::size_t{5};
  });
  if (value != "01234")
    return false;
  value.shrink_to_fit();
  value.clear();
  return value.empty() && value.data()[0] == '\0';
}
static_assert(capacity_and_access());

constexpr bool modifiers() {
  tested::string value = "ace";
  value.insert(1, "b");
  value.insert(3, 1, 'd');
  value.push_back('f');
  if (value != "abcdef")
    return false;
  value.replace(1, 3, "XYZ");
  if (value != "aXYZef")
    return false;
  value.erase(1, 3);
  if (value != "aef")
    return false;
  value.assign({'1', '2', '3'});
  tested::array<char, 2> suffix{{'4', '5'}};
  value.append_range(suffix);
  tested::array<char, 2> middle{{'x', 'y'}};
  value.insert_range(value.begin() + 2, middle);
  if (value != "12xy345")
    return false;
  tested::array<char, 2> replacement{{'A', 'B'}};
  value.replace_with_range(value.begin() + 2, value.begin() + 4, replacement);
  value.pop_back();
  if (value != "12AB34")
    return false;
  if (tested::erase(value, 'A') != 1 || value != "12B34")
    return false;
  if (tested::erase_if(value, [](char c) { return c == '3' || c == '4'; }) != 2 ||
      value != "12B")
    return false;
  char copied[3]{};
  return value.copy(copied, 2, 1) == 2 && copied[0] == '2' &&
         copied[1] == 'B';
}
static_assert(modifiers());

constexpr bool operations() {
  tested::string value = "bananas";
  if (value.find("ana") != 1 || value.rfind("ana") != 3)
    return false;
  if (value.find_first_of("ns") != 2 || value.find_last_of("an") != 5)
    return false;
  if (value.find_first_not_of("ab") != 2 ||
      value.find_last_not_of("as") != 4)
    return false;
  if (!value.starts_with("ban") || !value.ends_with('s') ||
      !value.contains("nan"))
    return false;
  if (value.substr(1, 3) != "ana" || value.compare("bananas") != 0)
    return false;
  tested::string joined = '!' + (tested::string("abc") + "def") + '!';
  if (joined != "!abcdef!")
    return false;
  return (tested::string("abc") <=> tested::string("abd")) ==
         tested::strong_ordering::less;
}
static_assert(operations());

static_assert("abc"s == tested::string("abc"));
static_assert(L"abc"s == tested::wstring(L"abc"));
static_assert(u8"abc"s == tested::u8string(u8"abc"));
static_assert(u"abc"s == tested::u16string(u"abc"));
static_assert(U"abc"s == tested::u32string(U"abc"));

struct allocation_state {
  int allocations{};
  int deallocations{};
};

template <class T> struct counting_allocator {
  using value_type = T;
  using propagate_on_container_move_assignment = tested::false_type;
  using propagate_on_container_swap = tested::false_type;
  using is_always_equal = tested::false_type;
  allocation_state *state{};

  counting_allocator() = default;
  explicit counting_allocator(allocation_state &value) : state(&value) {}
  template <class U>
  counting_allocator(const counting_allocator<U> &other) : state(other.state) {}

  T *allocate(tested::size_t count) {
    ++state->allocations;
    return static_cast<T *>(::operator new(count * sizeof(T)));
  }
  void deallocate(T *pointer, tested::size_t) noexcept {
    ++state->deallocations;
    ::operator delete(pointer);
  }
  template <class U> friend struct counting_allocator;
  friend bool operator==(counting_allocator left,
                         counting_allocator right) noexcept {
    return left.state == right.state;
  }
};

bool ftl_test() {
  allocation_state first{};
  allocation_state second{};
  using allocated_string =
      tested::basic_string<char, tested::char_traits<char>,
                           counting_allocator<char>>;
  {
    allocated_string short_value("short", counting_allocator<char>(first));
    if (first.allocations != 0)
      return false;
    allocated_string long_value("abcdefghijklmnopqrstuvwxyz0123456789",
                                counting_allocator<char>(first));
    if (first.allocations != 1)
      return false;
    const char *storage = long_value.data();
    allocated_string moved(tested::move(long_value),
                           counting_allocator<char>(first));
    if (moved.data() != storage)
      return false;
    allocated_string unequal(tested::move(moved),
                             counting_allocator<char>(second));
    if (second.allocations != 1 || unequal.empty())
      return false;
  }
  if (first.allocations != first.deallocations ||
      second.allocations != second.deallocations)
    return false;

  tested::size_t index = 0;
  if (tested::stoi("  -0x10tail", &index, 0) != -16 || index != 7)
    return false;
  if (tested::stoull("18446744073709551615") !=
      18446744073709551615ull)
    return false;
  if (tested::stod("1.25rest", &index) != 1.25 || index != 4)
    return false;
  if (tested::to_string(42) != "42" || tested::to_string(1.5) != "1.500000")
    return false;
  if (tested::to_wstring(-7) != L"-7")
    return false;
  if (tested::hash<tested::string>{}("hash") !=
      tested::hash<tested::string>{}(tested::string("hash")))
    return false;

#if FTL_HAS_EXCEPTIONS
  bool invalid = false;
  try {
    static_cast<void>(tested::stoi("not a number"));
  } catch (const tested::invalid_argument &) {
    invalid = true;
  }
  if (!invalid)
    return false;

  bool range = false;
  try {
    static_cast<void>(tested::string("abc").at(3));
  } catch (const tested::out_of_range &) {
    range = true;
  }
  if (!range)
    return false;
#endif
  return true;
}
