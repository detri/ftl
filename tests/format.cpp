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
    !tested::is_default_constructible_v<tested::formatter<wchar_t, char>>);

static_assert(tested::is_default_constructible_v<
              tested::formatter<tested::string, char>>);

static_assert(
    tested::is_default_constructible_v<tested::formatter<const char *, char>>);

static_assert(
    tested::is_default_constructible_v<tested::formatter<const void *, char>>);

static_assert(tested::is_default_constructible_v<
              tested::formatter<tested::nullptr_t, char>>);

static_assert(tested::formattable<int, char>);

static_assert(tested::formattable<double, char>);

static_assert(tested::formattable<bool, char>);

static_assert(tested::formattable<char, char>);

static_assert(tested::formattable<tested::string, char>);

static_assert(tested::formattable<handled_value, char>);

static_assert(!tested::formattable<unavailable_format_type, char>);

constexpr tested::format_string<int> checked_integer_format("{:04x}");

static_assert(checked_integer_format.get().size() == 6);

constexpr tested::wformat_string<int> checked_wide_format(L"{:x}");

static_assert(checked_wide_format.get().size() == 4);

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

bool parse_context_works() {
  tested::format_parse_context automatic("}", 3);

  if (automatic.next_arg_id() != 0 || automatic.next_arg_id() != 1 ||
      automatic.next_arg_id() != 2) {
    return false;
  }

  tested::format_parse_context manual("}", 3);

  manual.check_arg_id(2);
  manual.check_arg_id(0);

  try {
    tested::format_parse_context mixed("}", 2);

    (void)mixed.next_arg_id();
    mixed.check_arg_id(1);

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  try {
    tested::format_parse_context mixed("}", 2);

    mixed.check_arg_id(1);
    (void)mixed.next_arg_id();

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  return true;
}

bool argument_erasure_works() {
  short signed_value = -7;

  unsigned short unsigned_value = 9;

  float float_value = 1.5f;

  auto store =
      tested::make_format_args(signed_value, unsigned_value, float_value);

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

  const bool float_ok = tested::visit_format_arg(
      [](auto value) -> bool {
        using value_type = tested::remove_cvref_t<decltype(value)>;

        if constexpr (tested::is_same_v<value_type, float>) {
          return value == 1.5f;
        } else {
          return false;
        }
      },
      arguments.get(2));

  return signed_ok && unsigned_ok && float_ok && !arguments.get(3);
}

bool custom_handle_works() {
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

bool raw_formatter_tests() {
  if (!format_value_case("}", 42, "42"))
    return false;

  if (!format_value_case("#08x}", 42, "0x00002a"))
    return false;

  if (!format_value_case("c}", 65, "A"))
    return false;

  if (!format_value_case("}", 1.5, "1.5"))
    return false;

  if (!format_value_case(".2f}", 1.25, "1.25"))
    return false;

  if (!format_value_case("E}", 1.25, "1.250000E+00"))
    return false;

  if (!format_value_case("#.4g}", 12.0, "12.00"))
    return false;

  if (!format_value_case("010.2f}", -1.5, "-000001.50"))
    return false;

  if (!format_value_case("}", true, "true"))
    return false;

  if (!format_value_case("d}", true, "1"))
    return false;

  if (!format_value_case("?}", '\n', "'\\n'"))
    return false;

  if (!format_value_case("*<8.3}", "abcdef", "abc*****"))
    return false;

  if (!format_value_case("?}", "a\n", "\"a\\n\""))
    return false;

  if (!format_wide_value_case(L".2f}", 1.5, L"1.50"))
    return false;

  return true;
}

bool public_format_works() {
  const auto simple = tested::format("hello {}", 42);

  if (!equal_text(simple.c_str(), "hello 42")) {
    return false;
  }

  const auto multiple = tested::format("{} + {} = {}", 2, 3, 5);

  if (!equal_text(multiple.c_str(), "2 + 3 = 5")) {
    return false;
  }

  const auto manual = tested::format("{1}:{0}", "left", "right");

  if (!equal_text(manual.c_str(), "right:left")) {
    return false;
  }

  const auto escaped = tested::format("{{{}}}", 42);

  if (!equal_text(escaped.c_str(), "{42}")) {
    return false;
  }

  const auto integer = tested::format("{:#08x}", 42);

  if (!equal_text(integer.c_str(), "0x00002a")) {
    return false;
  }

  const auto floating = tested::format("{:.2f}", 3.14159);

  if (!equal_text(floating.c_str(), "3.14")) {
    return false;
  }

  const auto text = tested::format("{:*^9}", "abc");

  if (!equal_text(text.c_str(), "***abc***")) {
    return false;
  }

  const auto custom = tested::format("{}", handled_value{'Q'});

  if (!equal_text(custom.c_str(), "Q")) {
    return false;
  }

  return true;
}

bool nested_format_arguments_work() {
  const auto automatic = tested::format("{:{}}", 42, 6);

  if (!equal_text(automatic.c_str(), "    42")) {
    return false;
  }

  const auto manual = tested::format("{0:{1}}", 42, 6);

  if (!equal_text(manual.c_str(), "    42")) {
    return false;
  }

  const auto precision = tested::format("{0:.{1}f}", 3.14159, 3);

  if (!equal_text(precision.c_str(), "3.142")) {
    return false;
  }

  return true;
}

bool wide_format_works() {
  const auto value = tested::format(L"{} {:.2f}", 42, 1.5);

  return equal_text(value.c_str(), L"42 1.50");
}

bool runtime_vformat_works() {
  int first = 10;
  int second = 20;

  auto store = tested::make_format_args(first, second);

  const auto value = tested::vformat("{}:{}", tested::format_args(store));

  if (!equal_text(value.c_str(), "10:20")) {
    return false;
  }

  int number = 42;
  int width = 6;

  auto nested_store = tested::make_format_args(number, width);

  const auto nested =
      tested::vformat("{0:{1}}", tested::format_args(nested_store));

  if (!equal_text(nested.c_str(), "    42")) {
    return false;
  }

  return true;
}

bool runtime_format_errors_work() {
  int first = 1;
  int second = 2;

  auto store = tested::make_format_args(first, second);

  tested::format_args args(store);

  try {
    (void)tested::vformat("{0} {}", args);

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  try {
    (void)tested::vformat("{", args);

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  try {
    (void)tested::vformat("}", args);

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  try {
    (void)tested::vformat("{2}", args);

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  try {
    (void)tested::vformat("{01}", args);

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  return true;
}

bool format_to_works() {
  char buffer[128]{};

  auto result = tested::format_to(sink_iterator<char>{buffer}, "{}-{}", 12, 34);

  *result.current = '\0';

  return result.current == buffer + 5 && equal_text(buffer, "12-34");
}

bool vformat_to_works() {
  int value = 42;

  auto store = tested::make_format_args(value);

  char buffer[128]{};

  auto result = tested::vformat_to(sink_iterator<char>{buffer}, "value={}",
                                   tested::format_args(store));

  *result.current = '\0';

  return equal_text(buffer, "value=42");
}

bool format_to_n_works() {
  char buffer[128]{};

  const auto result =
      tested::format_to_n(sink_iterator<char>{buffer}, 4, "{}", "abcdef");

  *result.out.current = '\0';

  if (!equal_text(buffer, "abcd")) {
    return false;
  }

  if (result.out.current != buffer + 4) {
    return false;
  }

  if (result.size != 6)
    return false;

  char untouched[8]{};

  const auto negative =
      tested::format_to_n(sink_iterator<char>{untouched}, -1, "{}", 123);

  if (negative.out.current != untouched) {
    return false;
  }

  return negative.size == 3;
}

bool formatted_size_works() {
  if (tested::formatted_size("{}:{}", 12, 345) != 6) {
    return false;
  }

  if (tested::formatted_size(L"{:.2f}", 1.5) != 4) {
    return false;
  }

  return true;
}

bool locale_frontend_works() {
  tested::locale custom(tested::locale::classic(), new test_numpunct);

  const auto integer = tested::format(custom, "{:L}", 1234567);

  if (!equal_text(integer.c_str(), "1_234_567")) {
    return false;
  }

  const auto floating = tested::format(custom, "{:L}", 12345.5);

  if (!equal_text(floating.c_str(), "12_345;5")) {
    return false;
  }

  const auto boolean = tested::format(custom, "{:L}", true);

  if (!equal_text(boolean.c_str(), "yes")) {
    return false;
  }

  char buffer[64]{};

  auto result =
      tested::format_to(sink_iterator<char>{buffer}, custom, "{:L}", 1234);

  *result.current = '\0';

  return equal_text(buffer, "1_234");
}

bool format_string_get_works() {
  constexpr tested::format_string<int> value("value={}");

  const auto view = value.get();

  return view.size() == 8 && view[0] == 'v' && view[7] == '}';
}

bool ftl_test() {
  if (!parse_context_works())
    return false;

  if (!argument_erasure_works())
    return false;

  if (!custom_handle_works())
    return false;

  if (!raw_formatter_tests())
    return false;

  if (!public_format_works())
    return false;

  if (!nested_format_arguments_work())
    return false;

  if (!wide_format_works())
    return false;

  if (!runtime_vformat_works())
    return false;

  if (!runtime_format_errors_work())
    return false;

  if (!format_to_works())
    return false;

  if (!vformat_to_works())
    return false;

  if (!format_to_n_works())
    return false;

  if (!formatted_size_works())
    return false;

  if (!locale_frontend_works())
    return false;

  if (!format_string_get_works())
    return false;

  return true;
}
