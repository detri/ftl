#ifdef FTL_REPLACE_STL
#include <locale>
#include <string>
namespace tested = std;
#else
#include <ftl/locale>
#include <ftl/string>
namespace tested = ftl;
#endif

bool ftl_test() {
  const tested::locale &classic = tested::locale::classic();

  //
  // const char* constructor.
  //
  {
    tested::locale value(classic, new tested::ctype_byname<char>("C"));

    const auto &facet = tested::use_facet<tested::ctype<char>>(value);

    if (!facet.is(tested::ctype_base::alpha, 'A'))
      return false;

    if (!facet.is(tested::ctype_base::lower, 'z'))
      return false;

    if (!facet.is(tested::ctype_base::digit, '5'))
      return false;

    if (!facet.is(tested::ctype_base::space, '\n'))
      return false;

    if (!facet.is(tested::ctype_base::punct, '!'))
      return false;

    if (facet.toupper('a') != 'A')
      return false;

    if (facet.tolower('Z') != 'z')
      return false;
  }

  //
  // string constructor.
  //
  {
    tested::string name{"C"};

    tested::locale value(classic, new tested::ctype_byname<wchar_t>(name));

    const auto &facet = tested::use_facet<tested::ctype<wchar_t>>(value);

    if (!facet.is(tested::ctype_base::alpha, L'A'))
      return false;

    if (!facet.is(tested::ctype_base::digit, L'7'))
      return false;

    if (!facet.is(tested::ctype_base::space, L' '))
      return false;

    if (facet.toupper(L'a') != L'A')
      return false;

    if (facet.tolower(L'Z') != L'z')
      return false;

    if (facet.widen('A') != L'A')
      return false;

    if (facet.narrow(L'A', '?') != 'A')
      return false;
  }

  //
  // The environment-selected locale name is a required valid
  // standard locale spelling. We only assert portable basic-character
  // behavior because the actual selected locale differs by machine.
  //
  {
    tested::locale narrow(classic, new tested::ctype_byname<char>(""));

    tested::locale wide(classic, new tested::ctype_byname<wchar_t>(""));

    const auto &narrow_facet = tested::use_facet<tested::ctype<char>>(narrow);

    const auto &wide_facet = tested::use_facet<tested::ctype<wchar_t>>(wide);

    if (!narrow_facet.is(tested::ctype_base::digit, '3')) {
      return false;
    }

    if (!wide_facet.is(tested::ctype_base::digit, L'3')) {
      return false;
    }

    if (narrow_facet.toupper('a') != 'A')
      return false;

    if (wide_facet.toupper(L'a') != L'A')
      return false;
  }

  //
  // Repeated construction/destruction exercises native-handle
  // ownership rather than leaking one locale object per facet.
  //
  for (int iteration = 0; iteration < 32; ++iteration) {
    tested::locale value(classic, new tested::ctype_byname<char>("C"));

    if (!tested::isalpha('x', value))
      return false;
  }

  return true;
}
