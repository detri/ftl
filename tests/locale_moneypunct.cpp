#ifdef FTL_REPLACE_STL
#include <locale>
#include <string>
namespace tested = std;
#else
#include <ftl/locale>
#include <ftl/string>
namespace tested = ftl;
#endif

struct custom_moneypunct final : tested::moneypunct<char> {
  custom_moneypunct() : tested::moneypunct<char>() {}

  ~custom_moneypunct() override = default;

protected:
  char do_decimal_point() const override { return ':'; }

  char do_thousands_sep() const override { return '_'; }

  tested::string do_grouping() const override {
    tested::string result;

    result.push_back(static_cast<char>(3));

    return result;
  }

  tested::string do_curr_symbol() const override { return "FTL"; }

  tested::string do_positive_sign() const override { return "+"; }

  tested::string do_negative_sign() const override { return "-"; }

  int do_frac_digits() const override { return 3; }

  pattern do_pos_format() const override {
    return pattern{{static_cast<char>(sign), static_cast<char>(symbol),
                    static_cast<char>(space), static_cast<char>(value)}};
  }

  pattern do_neg_format() const override {
    return pattern{{static_cast<char>(sign), static_cast<char>(value),
                    static_cast<char>(none), static_cast<char>(symbol)}};
  }
};

bool pattern_equals(tested::money_base::pattern pattern,
                    tested::money_base::part first,
                    tested::money_base::part second,
                    tested::money_base::part third,
                    tested::money_base::part fourth) {
  return pattern.field[0] == static_cast<char>(first) &&
         pattern.field[1] == static_cast<char>(second) &&
         pattern.field[2] == static_cast<char>(third) &&
         pattern.field[3] == static_cast<char>(fourth);
}

bool ftl_test() {
  const tested::locale &classic = tested::locale::classic();

  using local_char = tested::moneypunct<char, false>;

  using local_wchar = tested::moneypunct<wchar_t, false>;

  using intl_char = tested::moneypunct<char, true>;

  using intl_wchar = tested::moneypunct<wchar_t, true>;

  static_assert(!local_char::intl);
  static_assert(intl_char::intl);

  if (!tested::has_facet<local_char>(classic)) {
    return false;
  }

  if (!tested::has_facet<local_wchar>(classic)) {
    return false;
  }

  if (!tested::has_facet<intl_char>(classic)) {
    return false;
  }

  if (!tested::has_facet<intl_wchar>(classic)) {
    return false;
  }

  //
  // Classic required behavior.
  //
  {
    const auto &facet = tested::use_facet<local_char>(classic);

    if (facet.decimal_point() != '.')
      return false;

    if (facet.thousands_sep() != ',')
      return false;

    if (!facet.grouping().empty())
      return false;

    if (!facet.curr_symbol().empty())
      return false;

    if (!facet.positive_sign().empty())
      return false;

    if (!facet.negative_sign().empty())
      return false;

    if (facet.frac_digits() != 0)
      return false;

    if (!pattern_equals(facet.pos_format(), tested::money_base::symbol,
                        tested::money_base::sign, tested::money_base::none,
                        tested::money_base::value)) {
      return false;
    }

    if (!pattern_equals(facet.neg_format(), tested::money_base::symbol,
                        tested::money_base::sign, tested::money_base::none,
                        tested::money_base::value)) {
      return false;
    }
  }

  {
    const auto &facet = tested::use_facet<local_wchar>(classic);

    if (facet.decimal_point() != L'.')
      return false;

    if (facet.thousands_sep() != L',')
      return false;

    if (!facet.curr_symbol().empty())
      return false;

    if (facet.frac_digits() != 0)
      return false;
  }

  {
    const auto &facet = tested::use_facet<intl_char>(classic);

    if (!facet.curr_symbol().empty())
      return false;

    if (facet.frac_digits() != 0)
      return false;
  }

  //
  // Virtual dispatch.
  //
  {
    tested::locale value(classic, new custom_moneypunct);

    const auto &facet = tested::use_facet<local_char>(value);

    if (facet.decimal_point() != ':')
      return false;

    if (facet.thousands_sep() != '_')
      return false;

    if (facet.curr_symbol() != "FTL")
      return false;

    if (facet.positive_sign() != "+")
      return false;

    if (facet.negative_sign() != "-")
      return false;

    if (facet.frac_digits() != 3)
      return false;

    if (!pattern_equals(facet.pos_format(), tested::money_base::sign,
                        tested::money_base::symbol, tested::money_base::space,
                        tested::money_base::value)) {
      return false;
    }

    if (!pattern_equals(facet.neg_format(), tested::money_base::sign,
                        tested::money_base::value, tested::money_base::none,
                        tested::money_base::symbol)) {
      return false;
    }
  }

  //
  // Directly exercise the lconv -> money_base::pattern
  // translation without relying on an installed host locale.
  //
  {
    const auto pattern = tested::detail::locale_monetary_pattern(1, 1, 1);

    if (!pattern_equals(pattern, tested::money_base::sign,
                        tested::money_base::symbol, tested::money_base::space,
                        tested::money_base::value)) {
      return false;
    }
  }

  {
    const auto pattern = tested::detail::locale_monetary_pattern(0, 2, 3);

    if (!pattern_equals(pattern, tested::money_base::value,
                        tested::money_base::sign, tested::money_base::space,
                        tested::money_base::symbol)) {
      return false;
    }
  }

  {
    const auto pattern = tested::detail::locale_monetary_pattern(1, 2, 4);

    if (!pattern_equals(pattern, tested::money_base::symbol,
                        tested::money_base::space, tested::money_base::sign,
                        tested::money_base::value)) {
      return false;
    }
  }

  //
  // Invalid/unavailable C locale positioning values fall
  // back to the required base moneypunct pattern.
  //
  {
    const auto pattern = tested::detail::locale_monetary_pattern(255, 255, 255);

    if (!pattern_equals(pattern, tested::money_base::symbol,
                        tested::money_base::sign, tested::money_base::none,
                        tested::money_base::value)) {
      return false;
    }
  }

  //
  // Named C locale must be equivalent to classic semantics.
  //
  {
    tested::locale value(classic,
                         new tested::moneypunct_byname<char, false>("C"));

    const auto &facet = tested::use_facet<local_char>(value);

    if (facet.decimal_point() != '.')
      return false;

    if (facet.thousands_sep() != ',')
      return false;

    if (!facet.grouping().empty())
      return false;

    if (!facet.curr_symbol().empty())
      return false;

    if (facet.frac_digits() != 0)
      return false;

    if (!pattern_equals(facet.pos_format(), tested::money_base::symbol,
                        tested::money_base::sign, tested::money_base::none,
                        tested::money_base::value)) {
      return false;
    }
  }

  //
  // International C locale.
  //
  {
    tested::string name{"C"};

    tested::locale value(classic,
                         new tested::moneypunct_byname<wchar_t, true>(name));

    const auto &facet = tested::use_facet<intl_wchar>(value);

    if (facet.decimal_point() != L'.')
      return false;

    if (facet.thousands_sep() != L',')
      return false;

    if (!facet.curr_symbol().empty())
      return false;

    if (facet.frac_digits() != 0)
      return false;
  }

  //
  // Environment-selected locale must construct. Its monetary
  // punctuation itself is intentionally not asserted because it
  // depends on the machine.
  //
  {
    tested::locale local_value(classic,
                               new tested::moneypunct_byname<char, false>(""));

    tested::locale intl_value(classic,
                              new tested::moneypunct_byname<wchar_t, true>(""));

    const auto &local = tested::use_facet<local_char>(local_value);

    const auto &international = tested::use_facet<intl_wchar>(intl_value);

    if (local.decimal_point() == '\0')
      return false;

    if (international.decimal_point() == L'\0') {
      return false;
    }
  }

  //
  // Exercise native-handle ownership.
  //
  for (int iteration = 0; iteration < 32; ++iteration) {
    tested::locale value(classic,
                         new tested::moneypunct_byname<char, false>("C"));

    const auto &facet = tested::use_facet<local_char>(value);

    if (facet.decimal_point() != '.')
      return false;
  }

  return true;
}
