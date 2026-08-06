#ifdef FTL_REPLACE_STL
#include <iosfwd>
namespace tested = std;
#else
#include <ftl/iosfwd>
namespace tested = ftl;
#endif

static_assert(tested::is_same_v<tested::streamoff, tested::ptrdiff_t>);

static_assert(tested::is_same_v<tested::streamsize, tested::ptrdiff_t>);

static_assert(tested::is_same_v<tested::istream, tested::basic_istream<char>>);

static_assert(
    tested::is_same_v<tested::wistream, tested::basic_istream<wchar_t>>);

static_assert(
    tested::is_same_v<tested::streambuf, tested::basic_streambuf<char>>);

static_assert(tested::is_same_v<tested::char_traits<char>::char_type, char>);

static_assert(
    tested::is_same_v<tested::char_traits<char>::off_type, tested::streamoff>);

constexpr bool char_traits_works() {
  using traits = tested::char_traits<char>;

  char copied[4] = {};
  traits::copy(copied, "abc", 3);

  if (copied[0] != 'a' || copied[1] != 'b' || copied[2] != 'c') {
    return false;
  }

  char assigned[4] = {};
  traits::assign(assigned, 3, 'x');

  if (assigned[0] != 'x' || assigned[1] != 'x' || assigned[2] != 'x') {
    return false;
  }

  return traits::length("hello") == 5 &&
         traits::compare("abc", "abc", 3) == 0 &&
         traits::compare("abc", "abd", 3) < 0 &&
         traits::find("abc", 3, 'b') != nullptr &&
         traits::eq(traits::to_char_type(traits::to_int_type('q')), 'q') &&
         traits::eq_int_type(traits::not_eof(traits::eof()), 0);
}

static_assert(char_traits_works());

bool ftl_test() { return char_traits_works(); }
