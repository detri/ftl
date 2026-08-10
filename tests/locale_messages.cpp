#ifdef FTL_REPLACE_STL
#include <locale>
#include <string>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/locale>
#include <ftl/string>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct custom_messages final : tested::messages<char> {
  mutable int closes = 0;

  custom_messages() : tested::messages<char>() {}

  ~custom_messages() override = default;

protected:
  catalog do_open(const tested::string &filename,
                  const tested::locale &) const override {
    return filename == "known" ? static_cast<catalog>(7)
                               : static_cast<catalog>(-1);
  }

  string_type do_get(catalog opened, int set, int message,
                     const string_type &fallback) const override {
    if (opened == 7 && set == 2 && message == 3) {
      return "translated";
    }

    return fallback;
  }

  void do_close(catalog opened) const override {
    if (opened == 7)
      ++closes;
  }
};

bool ftl_test() {
  using narrow_messages = tested::messages<char>;

  using wide_messages = tested::messages<wchar_t>;

  static_assert(tested::is_signed_v<tested::messages_base::catalog>);

  const tested::locale &classic = tested::locale::classic();

  if (!tested::has_facet<narrow_messages>(classic)) {
    return false;
  }

  if (!tested::has_facet<wide_messages>(classic)) {
    return false;
  }

  //
  // Default implementation: nonexistent catalog.
  //
  {
    const auto &facet = tested::use_facet<narrow_messages>(classic);

    const auto opened = facet.open("ftl_catalog_that_should_not_exist_"
                                   "4ef243db",
                                   classic);

    if (opened >= 0)
      return false;
  }

  //
  // Virtual dispatch.
  //
  {
    auto *custom = new custom_messages;

    tested::locale value(classic, custom);

    const auto &facet = tested::use_facet<narrow_messages>(value);

    const auto opened = facet.open("known", value);

    if (opened != 7)
      return false;

    if (facet.get(opened, 2, 3, "fallback") != "translated") {
      return false;
    }

    if (facet.get(opened, 9, 9, "fallback") != "fallback") {
      return false;
    }

    facet.close(opened);

    if (custom->closes != 1)
      return false;
  }

  //
  // The conversion path used by messages<wchar_t>.
  //
  {
    const tested::wstring fallback{L"fallback"};

    const auto converted = tested::detail::locale_convert_message<wchar_t>(
        "Hello", classic, fallback);

    if (converted != L"Hello")
      return false;

    const char invalid[] = {static_cast<char>(0xff), '\0'};

    const auto failed = tested::detail::locale_convert_message<wchar_t>(
        invalid, classic, fallback);

    if (failed != fallback)
      return false;
  }

  //
  // Named C locale.
  //
  {
    tested::locale value(classic, new tested::messages_byname<char>("C"));

    const auto &facet = tested::use_facet<narrow_messages>(value);

    const auto opened = facet.open("ftl_catalog_that_should_not_exist_"
                                   "81db162d",
                                   value);

    if (opened >= 0)
      return false;
  }

  //
  // string constructor.
  //
  {
    tested::string name{"C"};

    tested::locale value(classic, new tested::messages_byname<wchar_t>(name));

    if (!tested::has_facet<wide_messages>(value)) {
      return false;
    }
  }

  //
  // Required environment-selected locale spelling.
  //
  {
    tested::locale narrow_value(classic, new tested::messages_byname<char>(""));

    tested::locale wide_value(classic,
                              new tested::messages_byname<wchar_t>(""));

    if (!tested::has_facet<narrow_messages>(narrow_value)) {
      return false;
    }

    if (!tested::has_facet<wide_messages>(wide_value)) {
      return false;
    }
  }

  return true;
}
