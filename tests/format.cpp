#ifdef FTL_REPLACE_STL
#include <format>
#include <limits>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/format>
#include <ftl/limits>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct handled_value {
  char character;
};

#ifdef FTL_REPLACE_STL

template <> struct std::formatter<handled_value, char> {
  constexpr auto parse(std::format_parse_context &context) {
    return context.begin();
  }

  template <class FormatContext>
  typename FormatContext::iterator format(const handled_value &value,
                                          FormatContext &context) const {
    auto output = context.out();

    *output = value.character;
    ++output;

    return output;
  }
};

#else

template <> struct ftl::formatter<handled_value, char> {
  constexpr auto parse(ftl::format_parse_context &context) {
    return context.begin();
  }

  template <class FormatContext>
  typename FormatContext::iterator format(const handled_value &value,
                                          FormatContext &context) const {
    auto output = context.out();

    *output = value.character;
    ++output;

    return output;
  }
};

#endif

struct unavailable_format_type {};

static_assert(!tested::is_copy_constructible_v<tested::format_parse_context>);

static_assert(!tested::is_copy_assignable_v<tested::format_parse_context>);

static_assert(
    tested::is_base_of_v<tested::runtime_error, tested::format_error>);

static_assert(
    !tested::is_default_constructible_v<tested::formatter<void, char>>);

static_assert(tested::is_default_constructible_v<tested::formatter<int, char>>);

static_assert(tested::is_default_constructible_v<
              tested::formatter<unsigned long long, wchar_t>>);

static_assert(
    tested::is_default_constructible_v<tested::formatter<float, char>>);

static_assert(
    tested::is_default_constructible_v<tested::formatter<double, char>>);

static_assert(tested::is_default_constructible_v<
              tested::formatter<long double, wchar_t>>);

static_assert(
    tested::is_default_constructible_v<tested::formatter<bool, char>>);

static_assert(
    tested::is_default_constructible_v<tested::formatter<char, char>>);

static_assert(
    tested::is_default_constructible_v<tested::formatter<char, wchar_t>>);

static_assert(
    tested::is_default_constructible_v<tested::formatter<wchar_t, wchar_t>>);

static_assert(
    !tested::is_default_constructible_v<tested::formatter<wchar_t, char>>);

static_assert(tested::is_default_constructible_v<
              tested::formatter<tested::string, char>>);

static_assert(tested::is_default_constructible_v<
              tested::formatter<tested::string_view, char>>);

static_assert(
    tested::is_default_constructible_v<tested::formatter<const char *, char>>);

static_assert(!tested::is_default_constructible_v<
              tested::formatter<const char *, wchar_t>>);

static_assert(
    tested::is_default_constructible_v<tested::formatter<void *, char>>);

static_assert(
    tested::is_default_constructible_v<tested::formatter<const void *, char>>);

static_assert(tested::is_default_constructible_v<
              tested::formatter<tested::nullptr_t, char>>);

static_assert(tested::range_format::disabled != tested::range_format::sequence);

static_assert(tested::formattable<int, char>);

static_assert(tested::formattable<double, char>);

static_assert(tested::formattable<long double, wchar_t>);

static_assert(tested::formattable<bool, char>);

static_assert(tested::formattable<char, char>);

static_assert(tested::formattable<char, wchar_t>);

static_assert(tested::formattable<tested::string, char>);

static_assert(tested::formattable<tested::string_view, char>);

static_assert(tested::formattable<const char *, char>);

static_assert(tested::formattable<handled_value, char>);

static_assert(!tested::formattable<unavailable_format_type, char>);

template <class Character> struct sink_iterator {
  using difference_type = tested::ptrdiff_t;

  Character *current = nullptr;

  sink_iterator &operator*() noexcept { return *this; }

  sink_iterator &operator++() noexcept { return *this; }

  sink_iterator operator++(int) noexcept { return *this; }

  sink_iterator &operator=(Character value) noexcept {
    *current++ = value;
    return *this;
  }
};

template <class Character> struct sink_context {
  using iterator = sink_iterator<Character>;

  using char_type = Character;

  template <class T> using formatter_type = tested::formatter<T, Character>;

  iterator output;

  tested::basic_format_arg<sink_context> arg(tested::size_t) const noexcept {
    return {};
  }

  tested::locale locale() { return {}; }

  iterator out() { return output; }

  void advance_to(iterator next) { output = next; }
};

bool equal_text(const char *first, const char *second) {
  tested::size_t index = 0;

  for (;;) {
    if (first[index] != second[index])
      return false;

    if (first[index] == '\0')
      return true;

    ++index;
  }
}

bool equal_text(const wchar_t *first, const wchar_t *second) {
  tested::size_t index = 0;

  for (;;) {
    if (first[index] != second[index])
      return false;

    if (first[index] == L'\0')
      return true;

    ++index;
  }
}

template <class Value>
bool format_value_case(const char *specification, const Value &value,
                       const char *expected) {
  using formatter_type = tested::formatter<tested::remove_const_t<Value>, char>;

  formatter_type formatter;

  tested::format_parse_context parse_context(specification);

  const auto parsed = formatter.parse(parse_context);

  if (parsed != parse_context.end() && *parsed != '}') {
    return false;
  }

  char buffer[512]{};

  sink_context<char> context{sink_iterator<char>{buffer}};

  auto result = formatter.format(value, context);

  *result.current = '\0';

  return equal_text(buffer, expected);
}

template <class Value>
bool format_wide_value_case(const wchar_t *specification, const Value &value,
                            const wchar_t *expected) {
  using formatter_type =
      tested::formatter<tested::remove_const_t<Value>, wchar_t>;

  formatter_type formatter;

  tested::wformat_parse_context parse_context(specification);

  const auto parsed = formatter.parse(parse_context);

  if (parsed != parse_context.end() && *parsed != L'}') {
    return false;
  }

  wchar_t buffer[512]{};

  sink_context<wchar_t> context{sink_iterator<wchar_t>{buffer}};

  auto result = formatter.format(value, context);

  *result.current = L'\0';

  return equal_text(buffer, expected);
}

struct test_numpunct : tested::numpunct<char> {
  explicit test_numpunct(tested::size_t refs = 0)
      : tested::numpunct<char>(refs) {}

protected:
  char do_decimal_point() const override { return ';'; }

  char do_thousands_sep() const override { return '_'; }

  tested::string do_grouping() const override {
    return tested::string(1, static_cast<char>(3));
  }

  tested::string do_truename() const override { return tested::string("yes"); }

  tested::string do_falsename() const override { return tested::string("no"); }
};

bool automatic_indexing() {
  tested::format_parse_context context("}", 3);

  return context.next_arg_id() == 0 && context.next_arg_id() == 1 &&
         context.next_arg_id() == 2;
}

bool manual_indexing() {
  tested::format_parse_context context("}", 3);

  context.check_arg_id(2);
  context.check_arg_id(0);

  return true;
}

bool automatic_then_manual_throws() {
  try {
    tested::format_parse_context context("}", 2);

    (void)context.next_arg_id();

    context.check_arg_id(1);
  } catch (const tested::format_error &) {
    return true;
  } catch (...) {
    return false;
  }

  return false;
}

bool manual_then_automatic_throws() {
  try {
    tested::format_parse_context context("}", 2);

    context.check_arg_id(1);

    (void)context.next_arg_id();
  } catch (const tested::format_error &) {
    return true;
  } catch (...) {
    return false;
  }

  return false;
}

bool invalid_presentation_throws() {
  try {
    tested::formatter<int, char> formatter;

    tested::format_parse_context context("q}");

    (void)formatter.parse(context);
  } catch (const tested::format_error &) {
    return true;
  } catch (...) {
    return false;
  }

  return false;
}

bool precision_throws() {
  try {
    tested::formatter<int, char> formatter;

    tested::format_parse_context context(".3d}");

    (void)formatter.parse(context);
  } catch (const tested::format_error &) {
    return true;
  } catch (...) {
    return false;
  }

  return false;
}

bool integer_character_range_throws() {
  try {
    tested::formatter<int, char> formatter;

    tested::format_parse_context parse_context("c}");

    (void)formatter.parse(parse_context);

    char buffer[16]{};

    sink_context<char> context{sink_iterator<char>{buffer}};

    (void)formatter.format(256, context);
  } catch (const tested::format_error &) {
    return true;
  } catch (...) {
    return false;
  }

  return false;
}

bool format_arguments_work() {
  short signed_value = -7;

  unsigned short unsigned_value = 9;

  auto store = tested::make_format_args(signed_value, unsigned_value);

  tested::format_args arguments = store;

  const bool signed_ok = tested::visit_format_arg(
      [](auto value) -> bool {
        using value_type = tested::remove_cvref_t<decltype(value)>;

        if constexpr (tested::is_same_v<value_type, int>) {
          return value == -7;
        } else {
          return false;
        }
      },
      arguments.get(0));

  if (!signed_ok)
    return false;

  const bool unsigned_ok = tested::visit_format_arg(
      [](auto value) -> bool {
        using value_type = tested::remove_cvref_t<decltype(value)>;

        if constexpr (tested::is_same_v<value_type, unsigned int>) {
          return value == 9u;
        } else {
          return false;
        }
      },
      arguments.get(1));

  if (!unsigned_ok)
    return false;

  return !arguments.get(2);
}

bool floating_arguments_work() {
  float float_value = 1.5f;
  double double_value = 2.5;
  long double long_value = 3.5L;

  auto store = tested::make_format_args(float_value, double_value, long_value);

  tested::format_args arguments = store;

  const bool float_ok = tested::visit_format_arg(
      [](auto value) -> bool {
        using value_type = tested::remove_cvref_t<decltype(value)>;

        if constexpr (tested::is_same_v<value_type, float>) {
          return value == 1.5f;
        } else {
          return false;
        }
      },
      arguments.get(0));

  const bool double_ok = tested::visit_format_arg(
      [](auto value) -> bool {
        using value_type = tested::remove_cvref_t<decltype(value)>;

        if constexpr (tested::is_same_v<value_type, double>) {
          return value == 2.5;
        } else {
          return false;
        }
      },
      arguments.get(1));

  const bool long_ok = tested::visit_format_arg(
      [](auto value) -> bool {
        using value_type = tested::remove_cvref_t<decltype(value)>;

        if constexpr (tested::is_same_v<value_type, long double>) {
          return value == 3.5L;
        } else {
          return false;
        }
      },
      arguments.get(2));

  return float_ok && double_ok && long_ok;
}

bool string_argument_erasure_works() {
  const char text[] = "abc";

  auto store = tested::make_format_args(text);

  tested::format_args arguments = store;

  return tested::visit_format_arg(
      [](auto value) -> bool {
        using value_type = tested::remove_cvref_t<decltype(value)>;

        if constexpr (tested::is_same_v<value_type, const char *>) {
          return equal_text(value, "abc");
        } else {
          return false;
        }
      },
      arguments.get(0));
}

bool custom_format_argument_handle_works() {
  handled_value value{'Q'};

  auto store = tested::make_format_args(value);

  tested::format_args arguments = store;

  using argument_type = tested::basic_format_arg<tested::format_context>;

  using handle_type = typename argument_type::handle;

  return tested::visit_format_arg(
      [&](auto erased) -> bool {
        using erased_type = tested::remove_cvref_t<decltype(erased)>;

        if constexpr (tested::is_same_v<erased_type, handle_type>) {
          tested::string output;

          tested::format_args empty_arguments;

          tested::format_context context(tested::back_inserter(output),
                                         empty_arguments);

          tested::format_parse_context parse_context("}");

          erased.format(parse_context, context);

          return equal_text(output.c_str(), "Q");
        } else {
          return false;
        }
      },
      arguments.get(0));
}

bool dynamic_width_formats() {
  int value = 42;
  int width = 6;

  auto store = tested::make_format_args(value, width);

  tested::format_args arguments = store;

  tested::string output;

  tested::format_context context(tested::back_inserter(output), arguments);

  tested::formatter<int, char> formatter;

  tested::format_parse_context parse_context("{1}d}", 2);

  (void)formatter.parse(parse_context);

  auto result = formatter.format(value, context);

  context.advance_to(result);

  return equal_text(output.c_str(), "    42");
}

bool negative_dynamic_width_throws() {
  int value = 42;
  int width = -1;

  auto store = tested::make_format_args(value, width);

  tested::format_args arguments = store;

  tested::string output;

  tested::format_context context(tested::back_inserter(output), arguments);

  tested::formatter<int, char> formatter;

  tested::format_parse_context parse_context("{1}d}", 2);

  (void)formatter.parse(parse_context);

  try {
    (void)formatter.format(value, context);
  } catch (const tested::format_error &) {
    return true;
  } catch (...) {
    return false;
  }

  return false;
}

bool dynamic_string_precision_formats() {
  tested::string value("abcdef");

  int precision = 3;

  auto store = tested::make_format_args(value, precision);

  tested::format_args arguments = store;

  tested::string output;

  tested::format_context context(tested::back_inserter(output), arguments);

  tested::formatter<tested::string, char> formatter;

  tested::format_parse_context parse_context(".{1}}", 2);

  (void)formatter.parse(parse_context);

  auto result = formatter.format(value, context);

  context.advance_to(result);

  return equal_text(output.c_str(), "abc");
}

bool dynamic_float_precision_formats() {
  double value = 1.2345;
  int precision = 2;

  auto store = tested::make_format_args(value, precision);

  tested::format_args arguments = store;

  tested::string output;

  tested::format_context context(tested::back_inserter(output), arguments);

  tested::formatter<double, char> formatter;

  tested::format_parse_context parse_context(".{1}f}", 2);

  (void)formatter.parse(parse_context);

  auto result = formatter.format(value, context);

  context.advance_to(result);

  return equal_text(output.c_str(), "1.23");
}

bool negative_dynamic_float_precision_throws() {
  double value = 1.25;
  int precision = -1;

  auto store = tested::make_format_args(value, precision);

  tested::format_args arguments = store;

  tested::string output;

  tested::format_context context(tested::back_inserter(output), arguments);

  tested::formatter<double, char> formatter;

  tested::format_parse_context parse_context(".{1}f}", 2);

  (void)formatter.parse(parse_context);

  try {
    (void)formatter.format(value, context);
  } catch (const tested::format_error &) {
    return true;
  } catch (...) {
    return false;
  }

  return false;
}

bool localized_integer_formats() {
  int value = 1234567;

  auto store = tested::make_format_args(value);

  tested::format_args arguments = store;

  tested::locale custom(tested::locale::classic(), new test_numpunct);

  tested::string output;

  tested::format_context context(tested::back_inserter(output), arguments,
                                 &custom);

  tested::formatter<int, char> formatter;

  tested::format_parse_context parse_context("L}", 1);

  (void)formatter.parse(parse_context);

  auto result = formatter.format(value, context);

  context.advance_to(result);

  return equal_text(output.c_str(), "1_234_567");
}

bool localized_float_formats() {
  double value = 12345.5;

  auto store = tested::make_format_args(value);

  tested::format_args arguments = store;

  tested::locale custom(tested::locale::classic(), new test_numpunct);

  tested::string output;

  tested::format_context context(tested::back_inserter(output), arguments,
                                 &custom);

  tested::formatter<double, char> formatter;

  tested::format_parse_context parse_context("L}", 1);

  (void)formatter.parse(parse_context);

  auto result = formatter.format(value, context);

  context.advance_to(result);

  return equal_text(output.c_str(), "12_345;5");
}

bool localized_bool_formats() {
  bool value = true;

  auto store = tested::make_format_args(value);

  tested::format_args arguments = store;

  tested::locale custom(tested::locale::classic(), new test_numpunct);

  tested::string output;

  tested::format_context context(tested::back_inserter(output), arguments,
                                 &custom);

  tested::formatter<bool, char> formatter;

  tested::format_parse_context parse_context("L}", 1);

  (void)formatter.parse(parse_context);

  auto result = formatter.format(value, context);

  context.advance_to(result);

  return equal_text(output.c_str(), "yes");
}

bool invalid_float_presentation_throws() {
  try {
    tested::formatter<double, char> formatter;

    tested::format_parse_context context("q}");

    (void)formatter.parse(context);
  } catch (const tested::format_error &) {
    return true;
  } catch (...) {
    return false;
  }

  return false;
}

bool invalid_string_sign_throws() {
  try {
    tested::formatter<tested::string, char> formatter;

    tested::format_parse_context context("+}");

    (void)formatter.parse(context);
  } catch (const tested::format_error &) {
    return true;
  } catch (...) {
    return false;
  }

  return false;
}

bool invalid_pointer_precision_throws() {
  try {
    tested::formatter<const void *, char> formatter;

    tested::format_parse_context context(".3}");

    (void)formatter.parse(context);
  } catch (const tested::format_error &) {
    return true;
  } catch (...) {
    return false;
  }

  return false;
}

bool ftl_test() {
  if (!automatic_indexing())
    return false;

  if (!manual_indexing())
    return false;

  if (!automatic_then_manual_throws())
    return false;

  if (!manual_then_automatic_throws())
    return false;

  if (!invalid_presentation_throws())
    return false;

  if (!precision_throws())
    return false;

  if (!integer_character_range_throws())
    return false;

  if (!format_arguments_work())
    return false;

  if (!floating_arguments_work())
    return false;

  if (!string_argument_erasure_works())
    return false;

  if (!custom_format_argument_handle_works())
    return false;

  if (!dynamic_width_formats())
    return false;

  if (!negative_dynamic_width_throws())
    return false;

  if (!dynamic_string_precision_formats())
    return false;

  if (!dynamic_float_precision_formats())
    return false;

  if (!negative_dynamic_float_precision_throws())
    return false;

  if (!localized_integer_formats())
    return false;

  if (!localized_float_formats())
    return false;

  if (!localized_bool_formats())
    return false;

  if (!invalid_float_presentation_throws())
    return false;

  if (!invalid_string_sign_throws())
    return false;

  if (!invalid_pointer_precision_throws())
    return false;

  if (!format_value_case("}", 42, "42"))
    return false;

  if (!format_value_case("x}", 42, "2a"))
    return false;

  if (!format_value_case("#x}", 42, "0x2a"))
    return false;

  if (!format_value_case("06d}", -42, "-00042"))
    return false;

  if (!format_value_case("*^7d}", 42, "**42***"))
    return false;

  if (!format_value_case("c}", 65, "A"))
    return false;

  if (!format_wide_value_case(L"X}", 42, L"2A"))
    return false;

  /*
   * Floating default and precision forms.
   */
  if (!format_value_case("}", 1.5, "1.5"))
    return false;

  if (!format_value_case("f}", 1.5, "1.500000"))
    return false;

  if (!format_value_case(".2f}", 1.25, "1.25"))
    return false;

  if (!format_value_case("e}", 1.25, "1.250000e+00"))
    return false;

  if (!format_value_case("E}", 1.25, "1.250000E+00"))
    return false;

  if (!format_value_case("g}", 1.25, "1.25"))
    return false;

  if (!format_value_case(".4g}", 12.0, "12"))
    return false;

  if (!format_value_case("a}", 1.5, "1.8p+0"))
    return false;

  if (!format_value_case("A}", 1.5, "1.8P+0"))
    return false;

  /*
   * Alternate floating form.
   */
  if (!format_value_case("#.0f}", 1.0, "1."))
    return false;

  if (!format_value_case("#.4g}", 12.0, "12.00"))
    return false;

  if (!format_value_case("#.3e}", 1.0, "1.000e+00"))
    return false;

  if (!format_value_case("#a}", 1.0, "1.p+0"))
    return false;

  /*
   * Floating sign, padding, alignment.
   */
  if (!format_value_case("+f}", 1.5, "+1.500000"))
    return false;

  if (!format_value_case(" f}", 1.5, " 1.500000"))
    return false;

  if (!format_value_case("010.2f}", -1.5, "-000001.50"))
    return false;

  if (!format_value_case("*<10.2f}", 1.5, "1.50******"))
    return false;

  if (!format_value_case("*^10.2f}", 1.5, "***1.50***"))
    return false;

  if (!format_wide_value_case(L".2f}", 1.5, L"1.50"))
    return false;

  /*
   * Infinity/NaN case and 0 behavior.
   */
  const double infinity = tested::numeric_limits<double>::infinity();

  if (!format_value_case("F}", infinity, "INF"))
    return false;

  if (!format_value_case("+F}", infinity, "+INF"))
    return false;

  if (!format_value_case("08f}", infinity, "     inf"))
    return false;

  if (!format_value_case("}", 'x', "x"))
    return false;

  if (!format_value_case("6d}", 'x', "   120"))
    return false;

  if (!format_value_case("?}", '\n', "'\\n'"))
    return false;

  if (!format_value_case("}", true, "true"))
    return false;

  if (!format_value_case("d}", true, "1"))
    return false;

  if (!format_value_case("#b}", true, "0b1"))
    return false;

  if (!format_value_case("}", "hello", "hello"))
    return false;

  if (!format_value_case("*<8.3}", "abcdef", "abc*****"))
    return false;

  if (!format_value_case("?}", "a\n", "\"a\\n\""))
    return false;

  tested::string string_value("world");

  if (!format_value_case("}", string_value, "world"))
    return false;

  const void *null_pointer = nullptr;

  if (!format_value_case("}", null_pointer, "0x0"))
    return false;

  tested::nullptr_t null_value = nullptr;

  if (!format_value_case("p}", null_value, "0x0"))
    return false;

  return true;
}
