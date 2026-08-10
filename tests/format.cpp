#ifdef FTL_REPLACE_STL
#include <format>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/format>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(!tested::is_copy_constructible_v<tested::format_parse_context>);

static_assert(!tested::is_copy_assignable_v<tested::format_parse_context>);

static_assert(
    tested::is_base_of_v<tested::runtime_error, tested::format_error>);

static_assert(
    !tested::is_default_constructible_v<tested::formatter<void, char>>);

static_assert(tested::is_default_constructible_v<tested::formatter<int, char>>);

static_assert(tested::is_default_constructible_v<
              tested::formatter<unsigned long long, wchar_t>>);

static_assert(tested::range_format::disabled != tested::range_format::sequence);

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

  iterator output;

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

template <class Integer>
bool format_integer_case(const char *specification, Integer value,
                         const char *expected) {
  tested::formatter<Integer, char> formatter;

  tested::format_parse_context parse_context(specification);

  const auto parsed = formatter.parse(parse_context);

  if (parsed != parse_context.end() && *parsed != '}')
    return false;

  char buffer[128]{};

  sink_context<char> context{sink_iterator<char>{buffer}};

  auto result = formatter.format(value, context);

  *result.current = '\0';

  return equal_text(buffer, expected);
}

bool format_wide_integer_case(const wchar_t *specification, int value,
                              const wchar_t *expected) {
  tested::formatter<int, wchar_t> formatter;

  tested::wformat_parse_context parse_context(specification);

  const auto parsed = formatter.parse(parse_context);

  if (parsed != parse_context.end() && *parsed != L'}')
    return false;

  wchar_t buffer[128]{};

  sink_context<wchar_t> context{sink_iterator<wchar_t>{buffer}};

  auto result = formatter.format(value, context);

  *result.current = L'\0';

  return equal_text(buffer, expected);
}

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

bool dynamic_width_parses() {
  tested::formatter<int, char> formatter;

  tested::format_parse_context context("{}}", 1);

  const auto parsed = formatter.parse(context);

  return parsed != context.end() && *parsed == '}';
}

bool manual_dynamic_width_parses() {
  tested::formatter<int, char> formatter;

  tested::format_parse_context context("{1}}", 2);

  const auto parsed = formatter.parse(context);

  return parsed != context.end() && *parsed == '}';
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

  if (!dynamic_width_parses())
    return false;

  if (!manual_dynamic_width_parses())
    return false;

  if (!format_integer_case("}", 42, "42"))
    return false;

  if (!format_integer_case("d}", 42, "42"))
    return false;

  if (!format_integer_case("x}", 42, "2a"))
    return false;

  if (!format_integer_case("X}", 42, "2A"))
    return false;

  if (!format_integer_case("b}", 42, "101010"))
    return false;

  if (!format_integer_case("o}", 42, "52"))
    return false;

  if (!format_integer_case("#x}", 42, "0x2a"))
    return false;

  if (!format_integer_case("#X}", 42, "0X2A"))
    return false;

  if (!format_integer_case("#B}", 5, "0B101"))
    return false;

  if (!format_integer_case("#o}", 42, "052"))
    return false;

  if (!format_integer_case("#o}", 0, "0"))
    return false;

  if (!format_integer_case("+d}", 42, "+42"))
    return false;

  if (!format_integer_case(" d}", 42, " 42"))
    return false;

  if (!format_integer_case("06d}", -42, "-00042"))
    return false;

  if (!format_integer_case("#08x}", 42, "0x00002a"))
    return false;

  if (!format_integer_case("6d}", 42, "    42"))
    return false;

  if (!format_integer_case("<6d}", 42, "42    "))
    return false;

  if (!format_integer_case("+6d}", 42, "   +42"))
    return false;

  if (!format_integer_case("*^7d}", 42, "**42***"))
    return false;

  if (!format_integer_case("*<6d}", 42, "42****"))
    return false;

  if (!format_integer_case("*>6d}", 42, "****42"))
    return false;

  if (!format_wide_integer_case(L"X}", 42, L"2A"))
    return false;

  if (!format_wide_integer_case(L"#08x}", 42, L"0x00002a"))
    return false;

  return true;
}
