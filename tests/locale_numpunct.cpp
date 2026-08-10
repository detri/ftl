#ifdef FTL_REPLACE_STL
#include <locale>
#include <string>
namespace tested = std;
#else
#include <ftl/locale>
#include <ftl/string>
namespace tested = ftl;
#endif

struct custom_numpunct final : tested::numpunct<char> {
  custom_numpunct() : tested::numpunct<char>() {}

  ~custom_numpunct() override = default;

protected:
  char do_decimal_point() const override { return ':'; }

  char do_thousands_sep() const override { return '_'; }

  tested::string do_grouping() const override {
    tested::string result;
    result.push_back(static_cast<char>(3));
    return result;
  }

  tested::string do_truename() const override { return "yes"; }

  tested::string do_falsename() const override { return "no"; }
};

bool ftl_test() {
  const tested::locale &classic = tested::locale::classic();

  if (!tested::has_facet<tested::numpunct<char>>(classic)) {
    return false;
  }

  if (!tested::has_facet<tested::numpunct<wchar_t>>(classic)) {
    return false;
  }

  const auto &narrow = tested::use_facet<tested::numpunct<char>>(classic);

  const auto &wide = tested::use_facet<tested::numpunct<wchar_t>>(classic);

  //
  // Required classic semantics.
  //
  if (narrow.decimal_point() != '.')
    return false;

  if (narrow.thousands_sep() != ',')
    return false;

  if (!narrow.grouping().empty())
    return false;

  if (narrow.truename() != "true")
    return false;

  if (narrow.falsename() != "false")
    return false;

  if (wide.decimal_point() != L'.')
    return false;

  if (wide.thousands_sep() != L',')
    return false;

  if (!wide.grouping().empty())
    return false;

  if (wide.truename() != L"true")
    return false;

  if (wide.falsename() != L"false")
    return false;

  //
  // Virtual dispatch.
  //
  {
    tested::locale custom(classic, new custom_numpunct);

    const auto &facet = tested::use_facet<tested::numpunct<char>>(custom);

    if (facet.decimal_point() != ':')
      return false;

    if (facet.thousands_sep() != '_')
      return false;

    const tested::string grouping = facet.grouping();

    if (grouping.size() != 1)
      return false;

    if (static_cast<unsigned char>(grouping[0]) != 3) {
      return false;
    }

    if (facet.truename() != "yes")
      return false;

    if (facet.falsename() != "no")
      return false;
  }

  //
  // Named C locale.
  //
  {
    tested::locale value(classic, new tested::numpunct_byname<char>("C"));

    const auto &facet = tested::use_facet<tested::numpunct<char>>(value);

    if (facet.decimal_point() != '.')
      return false;

    if (!facet.grouping().empty())
      return false;

    if (facet.truename() != "true")
      return false;

    if (facet.falsename() != "false")
      return false;
  }

  //
  // wchar_t named C locale.
  //
  {
    tested::string name{"C"};

    tested::locale value(classic, new tested::numpunct_byname<wchar_t>(name));

    const auto &facet = tested::use_facet<tested::numpunct<wchar_t>>(value);

    if (facet.decimal_point() != L'.')
      return false;

    if (!facet.grouping().empty())
      return false;

    if (facet.truename() != L"true")
      return false;

    if (facet.falsename() != L"false")
      return false;
  }

  //
  // Environment-selected locale. Exact punctuation is host-specific,
  // but construction and the invariant boolean names are portable.
  //
  {
    tested::locale narrow_value(classic, new tested::numpunct_byname<char>(""));

    tested::locale wide_value(classic,
                              new tested::numpunct_byname<wchar_t>(""));

    const auto &narrow_facet =
        tested::use_facet<tested::numpunct<char>>(narrow_value);

    const auto &wide_facet =
        tested::use_facet<tested::numpunct<wchar_t>>(wide_value);

    if (narrow_facet.decimal_point() == '\0')
      return false;

    if (wide_facet.decimal_point() == L'\0')
      return false;

    if (narrow_facet.truename() != "true")
      return false;

    if (narrow_facet.falsename() != "false")
      return false;

    if (wide_facet.truename() != L"true")
      return false;

    if (wide_facet.falsename() != L"false")
      return false;
  }

  //
  // Exercise native locale lifetime repeatedly.
  //
  for (int iteration = 0; iteration < 32; ++iteration) {
    tested::locale value(classic, new tested::numpunct_byname<char>("C"));

    const auto &facet = tested::use_facet<tested::numpunct<char>>(value);

    if (facet.decimal_point() != '.')
      return false;
  }

  return true;
}
