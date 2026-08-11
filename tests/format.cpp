#ifdef FTL_REPLACE_STL
#include <array>
#include <format>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/format>
#include <ftl/limits>
#include <ftl/tuple>
#include <ftl/type_traits>
#include <ftl/utility>
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

struct sequence_range {
  int values[3]{1, 2, 3};

  int *begin() noexcept { return values; }

  int *end() noexcept { return values + 3; }

  const int *begin() const noexcept { return values; }

  const int *end() const noexcept { return values + 3; }
};

struct set_range {
  using key_type = int;

  int values[3]{1, 2, 3};

  int *begin() noexcept { return values; }

  int *end() noexcept { return values + 3; }

  const int *begin() const noexcept { return values; }

  const int *end() const noexcept { return values + 3; }
};

struct map_range {
  using key_type = int;
  using mapped_type = int;

  tested::pair<int, int> values[2]{{1, 2}, {3, 4}};

  tested::pair<int, int> *begin() noexcept { return values; }

  tested::pair<int, int> *end() noexcept { return values + 2; }

  const tested::pair<int, int> *begin() const noexcept { return values; }

  const tested::pair<int, int> *end() const noexcept { return values + 2; }
};

struct pair_sequence_range {
  tested::pair<int, int> values[2]{{1, 2}, {3, 4}};

  tested::pair<int, int> *begin() noexcept { return values; }

  tested::pair<int, int> *end() noexcept { return values + 2; }

  const tested::pair<int, int> *begin() const noexcept { return values; }

  const tested::pair<int, int> *end() const noexcept { return values + 2; }
};

struct char_sequence_range {
  char values[3]{'H', 'i', '!'};

  char *begin() noexcept { return values; }

  char *end() noexcept { return values + 3; }

  const char *begin() const noexcept { return values; }

  const char *end() const noexcept { return values + 3; }
};

struct string_kind_range {
  char values[2]{'o', 'k'};

  char *begin() noexcept { return values; }

  char *end() noexcept { return values + 2; }

  const char *begin() const noexcept { return values; }

  const char *end() const noexcept { return values + 2; }
};

struct debug_string_kind_range {
  char values[2]{'a', '\n'};

  char *begin() noexcept { return values; }

  char *end() noexcept { return values + 2; }

  const char *begin() const noexcept { return values; }

  const char *end() const noexcept { return values + 2; }
};

/*
 * Deliberately exposes a different element type
 * through const iteration.
 *
 * Non-const: char
 * const:     int
 *
 * N4950 requires the generic range formatter to
 * prefer the const range when it is formattable,
 * so "{}" must format the integer range.
 */
struct const_switch_range {
  char mutable_values[2]{'A', 'B'};

  int const_values[2]{1, 2};

  char *begin() noexcept { return mutable_values; }

  char *end() noexcept { return mutable_values + 2; }

  const int *begin() const noexcept { return const_values; }

  const int *end() const noexcept { return const_values + 2; }
};

struct mutable_only_range {
  int values[2]{7, 8};

  int *begin() noexcept { return values; }

  int *end() noexcept { return values + 2; }
};

#ifdef FTL_REPLACE_STL

namespace std {

template <>
inline constexpr range_format format_kind<string_kind_range> =
    range_format::string;

template <>
inline constexpr range_format format_kind<debug_string_kind_range> =
    range_format::debug_string;

} // namespace std

#else

namespace ftl {

template <>
inline constexpr range_format format_kind<string_kind_range> =
    range_format::string;

template <>
inline constexpr range_format format_kind<debug_string_kind_range> =
    range_format::debug_string;

} // namespace ftl

#endif

static_assert(tested::format_kind<sequence_range> ==
              tested::range_format::sequence);

static_assert(tested::format_kind<set_range> == tested::range_format::set);

static_assert(tested::format_kind<map_range> == tested::range_format::map);

static_assert(tested::format_kind<string_kind_range> ==
              tested::range_format::string);

static_assert(tested::format_kind<debug_string_kind_range> ==
              tested::range_format::debug_string);

static_assert(tested::formattable<sequence_range, char>);

static_assert(tested::formattable<map_range, char>);

static_assert(tested::formattable<tested::pair<int, int>, char>);

static_assert(tested::formattable<tested::tuple<int, int, int>, char>);

static_assert(tested::format_kind<const_switch_range> ==
              tested::range_format::sequence);

static_assert(tested::formattable<const_switch_range, char>);

static_assert(tested::formattable<mutable_only_range, char>);

using tested_range_formatter = tested::range_formatter<int, char>;

static_assert(noexcept(tested::declval<tested_range_formatter &>()
                           .set_separator(tested::string_view{})));

static_assert(noexcept(tested::declval<tested_range_formatter &>().set_brackets(
    tested::string_view{}, tested::string_view{})));

static_assert(
    noexcept(tested::declval<tested_range_formatter &>().underlying()));

static_assert(
    noexcept(tested::declval<const tested_range_formatter &>().underlying()));

using tested_pair_formatter = tested::formatter<tested::pair<int, int>, char>;

static_assert(noexcept(tested::declval<tested_pair_formatter &>().set_separator(
    tested::string_view{})));

static_assert(noexcept(tested::declval<tested_pair_formatter &>().set_brackets(
    tested::string_view{}, tested::string_view{})));

using tested_sequence_formatter = tested::formatter<sequence_range, char>;

static_assert(noexcept(tested::declval<tested_sequence_formatter &>()
                           .set_separator(tested::string_view{})));

static_assert(noexcept(tested::declval<tested_sequence_formatter &>()
                           .set_brackets(tested::string_view{},
                                         tested::string_view{})));

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

bool range_formatting_works() {
  sequence_range sequence;

  const auto normal = tested::format("{}", sequence);

  if (!equal_text(normal.c_str(), "[1, 2, 3]")) {
    return false;
  }

  const auto no_brackets = tested::format("{:n}", sequence);

  if (!equal_text(no_brackets.c_str(), "1, 2, 3")) {
    return false;
  }

  const auto elements = tested::format("{::#x}", sequence);

  if (!equal_text(elements.c_str(), "[0x1, 0x2, 0x3]")) {
    return false;
  }

  const auto padded = tested::format("{:*^15}", sequence);

  if (!equal_text(padded.c_str(), "***[1, 2, 3]***")) {
    return false;
  }

  set_range set;

  const auto set_text = tested::format("{}", set);

  if (!equal_text(set_text.c_str(), "{1, 2, 3}")) {
    return false;
  }

  map_range map;

  const auto map_text = tested::format("{}", map);

  if (!equal_text(map_text.c_str(), "{1: 2, 3: 4}")) {
    return false;
  }

  const auto map_no_brackets = tested::format("{:n}", map);

  if (!equal_text(map_no_brackets.c_str(), "1: 2, 3: 4")) {
    return false;
  }

  pair_sequence_range pairs;

  const auto explicit_map = tested::format("{:m}", pairs);

  if (!equal_text(explicit_map.c_str(), "{1: 2, 3: 4}")) {
    return false;
  }

  char_sequence_range characters;

  const auto default_chars = tested::format("{}", characters);

  if (!equal_text(default_chars.c_str(), "['H', 'i', '!']")) {
    return false;
  }

  const auto as_string = tested::format("{:s}", characters);

  if (!equal_text(as_string.c_str(), "Hi!")) {
    return false;
  }

  const auto as_debug_string = tested::format("{:?s}", characters);

  if (!equal_text(as_debug_string.c_str(), "\"Hi!\"")) {
    return false;
  }

  string_kind_range string_kind;

  const auto string_default = tested::format("{}", string_kind);

  if (!equal_text(string_default.c_str(), "ok")) {
    return false;
  }

  debug_string_kind_range debug_kind;

  const auto debug_default = tested::format("{}", debug_kind);

  if (!equal_text(debug_default.c_str(), "\"a\\n\"")) {
    return false;
  }

  const_switch_range const_switch;

  const auto const_selected = tested::format("{}", const_switch);

  /*
   * If the formatter incorrectly uses the
   * non-const range reference type this would
   * instead become ['A', 'B'].
   */
  if (!equal_text(const_selected.c_str(), "[1, 2]")) {
    return false;
  }

  mutable_only_range mutable_only;

  const auto mutable_selected = tested::format("{}", mutable_only);

  if (!equal_text(mutable_selected.c_str(), "[7, 8]")) {
    return false;
  }

  return true;
}

bool tuple_formatting_works() {
  tested::pair<int, tested::string> pair_value{42, tested::string("hello")};

  const auto normal_pair = tested::format("{}", pair_value);

  if (!equal_text(normal_pair.c_str(), "(42, \"hello\")")) {
    return false;
  }

  const auto map_pair = tested::format("{:m}", pair_value);

  if (!equal_text(map_pair.c_str(), "42: \"hello\"")) {
    return false;
  }

  const auto naked_pair = tested::format("{:n}", pair_value);

  if (!equal_text(naked_pair.c_str(), "42, \"hello\"")) {
    return false;
  }

  tested::pair<int, int> numeric_pair{1, 2};

  const auto padded = tested::format("{:*^12}", numeric_pair);

  if (!equal_text(padded.c_str(), "***(1, 2)***")) {
    return false;
  }

  tested::tuple<int, tested::string, char> tuple_value{1, tested::string("x"),
                                                       '\n'};

  const auto tuple_text = tested::format("{}", tuple_value);

  if (!equal_text(tuple_text.c_str(), "(1, \"x\", '\\n')")) {
    return false;
  }

  return true;
}

bool range_tuple_runtime_errors_work() {
  sequence_range sequence;

  auto sequence_store = tested::make_format_args(sequence);

  try {
    (void)tested::vformat("{:s}", tested::format_args(sequence_store));

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  tested::tuple<int, int, int> triple{1, 2, 3};

  auto tuple_store = tested::make_format_args(triple);

  try {
    (void)tested::vformat("{:m}", tested::format_args(tuple_store));

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  return true;
}

bool unicode_formatting_works() {
  /*
   * U+1F921 CLOWN FACE, UTF-8:
   *
   *   f0 9f a4 a1
   *
   * Its C++ field width is 2.
   */
  const char clown[] = "\xf0\x9f\xa4\xa1";

  const char three_clowns[] = "\xf0\x9f\xa4\xa1"
                              "\xf0\x9f\xa4\xa1"
                              "\xf0\x9f\xa4\xa1";

  const auto wide_argument = tested::format("{:*^6}", three_clowns);

  if (!equal_text(wide_argument.c_str(), three_clowns)) {
    return false;
  }

  /*
   * A multibyte Unicode scalar is one fill
   * character, not four independent bytes.
   */
  const auto unicode_fill = tested::format("{:\xf0\x9f\xa4\xa1^6}", "x");

  const char expected_fill[] = "\xf0\x9f\xa4\xa1"
                               "\xf0\x9f\xa4\xa1"
                               "x"
                               "\xf0\x9f\xa4\xa1"
                               "\xf0\x9f\xa4\xa1"
                               "\xf0\x9f\xa4\xa1";

  if (!equal_text(unicode_fill.c_str(), expected_fill)) {
    return false;
  }

  /*
   * "A🤡B" has field widths:
   *
   *   A  = 1
   *   🤡 = 2
   *   B  = 1
   *
   * Precision 3 therefore retains A🤡.
   */
  const char mixed_width[] = "A"
                             "\xf0\x9f\xa4\xa1"
                             "B";

  const auto precision_three = tested::format("{:.3}", mixed_width);

  const char expected_three[] = "A"
                                "\xf0\x9f\xa4\xa1";

  if (!equal_text(precision_three.c_str(), expected_three)) {
    return false;
  }

  const auto precision_two = tested::format("{:.2}", mixed_width);

  if (!equal_text(precision_two.c_str(), "A")) {
    return false;
  }

  /*
   * e + U+0301 COMBINING ACUTE ACCENT is
   * retained as one width-1 cluster for our
   * Grapheme_Extend handling.
   */
  const char composed_cluster[] = "e"
                                  "\xcc\x81";

  const auto combining_precision = tested::format("{:.1}", composed_cluster);

  if (!equal_text(combining_precision.c_str(), composed_cluster)) {
    return false;
  }

  /*
   * N4950 debug example family:
   * control characters become Unicode escapes
   * unless one of the dedicated short escapes.
   */
  const char debug_controls_value[] = {'\0', ' ',    '\n', ' ',    '\t',
                                       ' ',  '\x02', ' ',  '\x1b', '\0'};

  tested::string debug_controls_input(debug_controls_value, 9);

  const auto debug_controls = tested::format("{:?}", debug_controls_input);

  if (!equal_text(debug_controls.c_str(),
                  "\"\\u{0} \\n \\t \\u{2} \\u{1b}\"")) {
    return false;
  }

  /*
   * Ill-formed UTF-8:
   *
   * c3 is an incomplete lead byte because the
   * next unit is '(' rather than a continuation.
   */
  const char invalid_utf8_value[] = {static_cast<char>(0xc3), '('};

  tested::string invalid_utf8(invalid_utf8_value, 2);

  const auto escaped_invalid = tested::format("{:?}", invalid_utf8);

  if (!equal_text(escaped_invalid.c_str(), "\"\\x{c3}(\"")) {
    return false;
  }

  /*
   * A standalone Grapheme_Extend character is
   * escaped.
   */
  const char standalone_combining[] = "\xcc\x81";

  const auto escaped_combining = tested::format("{:?}", standalone_combining);

  if (!equal_text(escaped_combining.c_str(), "\"\\u{301}\"")) {
    return false;
  }

  /*
   * But after an untranslated base character,
   * Grapheme_Extend characters remain untranslated.
   */
  const char base_and_combining[] = "e"
                                    "\xcc\x81"
                                    "\xcc\xa3";

  const auto preserved_combining = tested::format("{:?}", base_and_combining);

  const char expected_combining[] = "\"e"
                                    "\xcc\x81"
                                    "\xcc\xa3"
                                    "\"";

  if (!equal_text(preserved_combining.c_str(), expected_combining)) {
    return false;
  }

  /*
   * U+00A0 NO-BREAK SPACE is General_Category Zs.
   */
  const auto escaped_separator = tested::format("{:?}", "\xc2\xa0");

  if (!equal_text(escaped_separator.c_str(), "\"\\u{a0}\"")) {
    return false;
  }

  /*
   * U+200D ZERO WIDTH JOINER is General_Category Cf.
   */
  const auto escaped_format_character = tested::format("{:?}", "\xe2\x80\x8d");

  if (!equal_text(escaped_format_character.c_str(), "\"\\u{200d}\"")) {
    return false;
  }

  /*
   * U+E000 private-use character is General_Category Co.
   */
  const auto escaped_private_use = tested::format("{:?}", "\xee\x80\x80");

  if (!equal_text(escaped_private_use.c_str(), "\"\\u{e000}\"")) {
    return false;
  }

  /*
   * U+0378 is unassigned and therefore General_Category Cn.
   */
  const auto escaped_unassigned = tested::format("{:?}", "\xcd\xb8");

  if (!equal_text(escaped_unassigned.c_str(), "\"\\u{378}\"")) {
    return false;
  }

  /*
   * Wide strings exercise UTF-16 on Windows and
   * UTF-32 on the usual Unix ABI.
   */
  const auto wide_cjk = tested::format(L"{:*^3}", L"\u4e00");

  if (!equal_text(wide_cjk.c_str(), L"\u4e00*")) {
    return false;
  }

  const auto wide_combining = tested::format(L"{:?}", L"\u0301");

  if (!equal_text(wide_combining.c_str(), L"\"\\u{301}\"")) {
    return false;
  }

  (void)clown;

  return true;
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

  if (!range_formatting_works())
    return false;

  if (!tuple_formatting_works())
    return false;

  if (!range_tuple_runtime_errors_work())
    return false;

  if (!unicode_formatting_works())
    return false;

  return true;
}
