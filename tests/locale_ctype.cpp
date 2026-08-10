#ifdef FTL_REPLACE_STL
#include <locale>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/locale>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(tested::ctype<char>::table_size >= 256);

static_assert(tested::ctype_base::alnum ==
              (tested::ctype_base::alpha | tested::ctype_base::digit));

static_assert(tested::ctype_base::graph ==
              (tested::ctype_base::alnum | tested::ctype_base::punct));

struct overriding_char_ctype : tested::ctype<char> {
  overriding_char_ctype() : tested::ctype<char>() {}

  ~overriding_char_ctype() override = default;

protected:
  char do_toupper(char value) const override {
    return value == 'q' ? '!' : tested::ctype<char>::do_toupper(value);
  }
};

struct overriding_wide_ctype : tested::ctype<wchar_t> {
  overriding_wide_ctype() : tested::ctype<wchar_t>() {}

  ~overriding_wide_ctype() override = default;

protected:
  bool do_is(mask requested, wchar_t value) const override {
    if (value == L'@')
      return (requested & alpha) != 0;

    return tested::ctype<wchar_t>::do_is(requested, value);
  }

  wchar_t do_toupper(wchar_t value) const override {
    return value == L'q' ? L'!' : tested::ctype<wchar_t>::do_toupper(value);
  }
};

struct owning_table_ctype : tested::ctype<char> {
  owning_table_ctype(const mask *table, int *destructions)
      : tested::ctype<char>(table, true), destructions_(destructions) {}

  ~owning_table_ctype() override { ++*destructions_; }

private:
  int *destructions_;
};

bool ftl_test() {
  const tested::locale &classic = tested::locale::classic();

  if (!tested::has_facet<tested::ctype<char>>(classic))
    return false;

  if (!tested::has_facet<tested::ctype<wchar_t>>(classic))
    return false;

  const auto &characters = tested::use_facet<tested::ctype<char>>(classic);

  const auto &wide = tested::use_facet<tested::ctype<wchar_t>>(classic);

  if (characters.table() != tested::ctype<char>::classic_table())
    return false;

  if (!characters.is(tested::ctype_base::alpha, 'A'))
    return false;

  if (!characters.is(tested::ctype_base::upper, 'A'))
    return false;

  if (characters.is(tested::ctype_base::lower, 'A'))
    return false;

  if (!characters.is(tested::ctype_base::lower, 'z'))
    return false;

  if (!characters.is(tested::ctype_base::digit, '7'))
    return false;

  if (!characters.is(tested::ctype_base::xdigit, 'F'))
    return false;

  if (!characters.is(tested::ctype_base::xdigit, 'f'))
    return false;

  if (!characters.is(tested::ctype_base::space, '\n'))
    return false;

  if (!characters.is(tested::ctype_base::blank, '\t'))
    return false;

  if (!characters.is(tested::ctype_base::punct, '!'))
    return false;

  if (!characters.is(tested::ctype_base::print, ' '))
    return false;

  if (characters.is(tested::ctype_base::graph, ' '))
    return false;

  if (!characters.is(tested::ctype_base::cntrl, '\n'))
    return false;

  if (characters.is(tested::ctype_base::alpha, static_cast<char>(0xff)))
    return false;

  const char classified[] = {'A', '7', ' '};
  tested::ctype_base::mask masks[3]{};

  if (characters.is(classified, classified + 3, masks) != classified + 3) {
    return false;
  }

  if ((masks[0] & tested::ctype_base::alpha) == 0)
    return false;

  if ((masks[1] & tested::ctype_base::digit) == 0)
    return false;

  if ((masks[2] & tested::ctype_base::space) == 0)
    return false;

  const char scan_alpha[] = {'1', '2', 'A', '3'};

  if (characters.scan_is(tested::ctype_base::alpha, scan_alpha,
                         scan_alpha + 4) != scan_alpha + 2) {
    return false;
  }

  if (characters.scan_not(tested::ctype_base::digit, scan_alpha,
                          scan_alpha + 4) != scan_alpha + 2) {
    return false;
  }

  if (characters.toupper('a') != 'A')
    return false;

  if (characters.tolower('Z') != 'z')
    return false;

  char upper_range[] = {'a', 'B', 'z'};

  if (characters.toupper(upper_range, upper_range + 3) != upper_range + 3) {
    return false;
  }

  if (upper_range[0] != 'A' || upper_range[1] != 'B' || upper_range[2] != 'Z') {
    return false;
  }

  char lower_range[] = {'A', 'b', 'Z'};

  characters.tolower(lower_range, lower_range + 3);

  if (lower_range[0] != 'a' || lower_range[1] != 'b' || lower_range[2] != 'z') {
    return false;
  }

  if (characters.widen('A') != 'A')
    return false;

  if (characters.narrow('A', '?') != 'A')
    return false;

  //
  // wchar_t specialization.
  //

  if (!wide.is(tested::ctype_base::alpha, L'A'))
    return false;

  if (!wide.is(tested::ctype_base::digit, L'9'))
    return false;

  if (!wide.is(tested::ctype_base::space, L' '))
    return false;

  if (wide.toupper(L'a') != L'A')
    return false;

  if (wide.tolower(L'Z') != L'z')
    return false;

  if (wide.widen('A') != L'A')
    return false;

  if (wide.narrow(L'A', '?') != 'A')
    return false;

  if (wide.narrow(L'\u03a9', '?') != '?')
    return false;

  //
  // Convenience interfaces.
  //

  if (!tested::isspace(' ', classic))
    return false;

  if (!tested::isprint('A', classic))
    return false;

  if (!tested::iscntrl('\n', classic))
    return false;

  if (!tested::isupper('A', classic))
    return false;

  if (!tested::islower('a', classic))
    return false;

  if (!tested::isalpha('x', classic))
    return false;

  if (!tested::isdigit('4', classic))
    return false;

  if (!tested::ispunct('!', classic))
    return false;

  if (!tested::isxdigit('e', classic))
    return false;

  if (!tested::isalnum('8', classic))
    return false;

  if (!tested::isgraph('@', classic))
    return false;

  if (!tested::isblank('\t', classic))
    return false;

  if (tested::toupper('a', classic) != 'A')
    return false;

  if (tested::tolower('Z', classic) != 'z')
    return false;

  if (!tested::isalpha(L'A', classic))
    return false;

  if (tested::toupper(L'a', classic) != L'A')
    return false;

  //
  // A custom char table replaces classification wholesale.
  //

  int table_destructions = 0;

  {
    using mask = tested::ctype_base::mask;

    auto *table = new mask[tested::ctype<char>::table_size]{};

    table[static_cast<unsigned char>('@')] =
        tested::ctype_base::alpha | tested::ctype_base::upper;

    tested::locale custom(classic,
                          new owning_table_ctype(table, &table_destructions));

    const auto &facet = tested::use_facet<tested::ctype<char>>(custom);

    if (facet.table() != table)
      return false;

    if (!facet.is(tested::ctype_base::alpha, '@'))
      return false;

    if (!facet.is(tested::ctype_base::upper, '@'))
      return false;

    if (facet.is(tested::ctype_base::alpha, 'A'))
      return false;
  }

  if (table_destructions != 1)
    return false;

  //
  // char classification stays table-driven, but conversion hooks are virtual.
  //

  {
    tested::locale custom(classic, new overriding_char_ctype);

    const auto &facet = tested::use_facet<tested::ctype<char>>(custom);

    if (!facet.is(tested::ctype_base::alpha, 'q'))
      return false;

    if (facet.toupper('q') != '!')
      return false;
  }

  //
  // wchar_t classification itself is virtual.
  //

  {
    tested::locale custom(classic, new overriding_wide_ctype);

    const auto &facet = tested::use_facet<tested::ctype<wchar_t>>(custom);

    if (!facet.is(tested::ctype_base::alpha, L'@'))
      return false;

    if (facet.toupper(L'q') != L'!')
      return false;
  }

  return true;
}
