#ifdef FTL_REPLACE_STL
#include <locale>
#include <string>
namespace tested = std;
#else
#include <ftl/locale>
#include <ftl/string>
namespace tested = ftl;
#endif

struct reverse_collate final : tested::collate<char> {
  reverse_collate() : tested::collate<char>() {}

  ~reverse_collate() override = default;

protected:
  int do_compare(const char *low1, const char *high1, const char *low2,
                 const char *high2) const override {
    return -tested::collate<char>::do_compare(low1, high1, low2, high2);
  }
};

bool ftl_test() {
  const tested::locale &classic = tested::locale::classic();

  if (!tested::has_facet<tested::collate<char>>(classic)) {
    return false;
  }

  if (!tested::has_facet<tested::collate<wchar_t>>(classic)) {
    return false;
  }

  const auto &narrow = tested::use_facet<tested::collate<char>>(classic);

  const auto &wide = tested::use_facet<tested::collate<wchar_t>>(classic);

  const char alpha[] = "alpha";
  const char beta[] = "beta";

  if (narrow.compare(alpha, alpha + 5, beta, beta + 4) >= 0) {
    return false;
  }

  if (narrow.compare(beta, beta + 4, alpha, alpha + 5) <= 0) {
    return false;
  }

  if (narrow.compare(alpha, alpha + 5, alpha, alpha + 5) != 0) {
    return false;
  }

  const auto alpha_key = narrow.transform(alpha, alpha + 5);

  if (alpha_key != tested::string{"alpha"})
    return false;

  if (narrow.hash(alpha, alpha + 5) != narrow.hash(alpha, alpha + 5)) {
    return false;
  }

  //
  // Ordinary collate is range-based, including embedded NULs.
  //
  const char embedded_left[] = {'a', '\0', 'b'};

  const char embedded_right[] = {'a', '\0', 'c'};

  if (narrow.compare(embedded_left, embedded_left + 3, embedded_right,
                     embedded_right + 3) >= 0) {
    return false;
  }

  const auto embedded_left_key =
      narrow.transform(embedded_left, embedded_left + 3);

  const auto embedded_right_key =
      narrow.transform(embedded_right, embedded_right + 3);

  if (!(embedded_left_key < embedded_right_key)) {
    return false;
  }

  //
  // wchar_t specialization.
  //
  const wchar_t wide_alpha[] = L"alpha";
  const wchar_t wide_beta[] = L"beta";

  if (wide.compare(wide_alpha, wide_alpha + 5, wide_beta, wide_beta + 4) >= 0) {
    return false;
  }

  const auto wide_key = wide.transform(wide_alpha, wide_alpha + 5);

  if (wide_key != tested::wstring{L"alpha"})
    return false;

  //
  // locale::operator()
  //
  tested::string first{"alpha"};
  tested::string second{"beta"};

  if (!classic(first, second))
    return false;

  if (classic(second, first))
    return false;

  //
  // Virtual dispatch through a custom facet.
  //
  {
    tested::locale reversed(classic, new reverse_collate);

    if (!reversed(second, first))
      return false;

    if (reversed(first, second))
      return false;
  }

  //
  // Named C locale, char.
  //
  {
    tested::locale value(classic, new tested::collate_byname<char>("C"));

    const auto &facet = tested::use_facet<tested::collate<char>>(value);

    if (facet.compare(alpha, alpha + 5, beta, beta + 4) >= 0) {
      return false;
    }

    const auto left_key = facet.transform(alpha, alpha + 5);

    const auto right_key = facet.transform(beta, beta + 4);

    if (!(left_key < right_key))
      return false;

    if (facet.hash(alpha, alpha + 5) != facet.hash(alpha, alpha + 5)) {
      return false;
    }

    //
    // Make sure named collation doesn't silently truncate
    // C++ ranges at embedded NULs.
    //
    const auto nul_left_key = facet.transform(embedded_left, embedded_left + 3);

    const auto nul_right_key =
        facet.transform(embedded_right, embedded_right + 3);

    if (!(nul_left_key < nul_right_key))
      return false;

    if (facet.compare(embedded_left, embedded_left + 3, embedded_right,
                      embedded_right + 3) >= 0) {
      return false;
    }
  }

  //
  // string name constructor + wchar_t.
  //
  {
    tested::string name{"C"};

    tested::locale value(classic, new tested::collate_byname<wchar_t>(name));

    const auto &facet = tested::use_facet<tested::collate<wchar_t>>(value);

    if (facet.compare(wide_alpha, wide_alpha + 5, wide_beta, wide_beta + 4) >=
        0) {
      return false;
    }

    const auto left_key = facet.transform(wide_alpha, wide_alpha + 5);

    const auto right_key = facet.transform(wide_beta, wide_beta + 4);

    if (!(left_key < right_key))
      return false;
  }

  //
  // Environment-selected locale must also construct.
  // Only portable basic ordering properties are tested.
  //
  {
    tested::locale value(classic, new tested::collate_byname<char>(""));

    const auto &facet = tested::use_facet<tested::collate<char>>(value);

    if (facet.compare(alpha, alpha + 5, alpha, alpha + 5) != 0) {
      return false;
    }

    if (facet.hash(alpha, alpha + 5) != facet.hash(alpha, alpha + 5)) {
      return false;
    }
  }

  return true;
}
