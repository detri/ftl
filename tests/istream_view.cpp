#ifdef FTL_REPLACE_STL
#include <istream>
#include <ranges>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/istream>
#include <ftl/ranges>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct test_char_traits : tested::char_traits<char> {};

using tested_streambuf = tested::basic_streambuf<char, test_char_traits>;

using tested_istream = tested::basic_istream<char, test_char_traits>;

class fixed_streambuf : public tested_streambuf {
public:
  fixed_streambuf(char *first, char *last) { setg(first, first, last); }
};

class derived_istream : public tested_istream {
public:
  using tested_istream::tested_istream;
};

struct extracted_value {
  char value = '\0';
};

tested_istream &operator>>(tested_istream &stream, extracted_value &value) {
  return stream.get(value.value);
}

using tested_view = tested::ranges::basic_istream_view<struct extracted_value,
                                                       char, test_char_traits>;

/*
 * These declarations are enough to validate
 * the default narrow and wide aliases without
 * instantiating hosted-runtime stream symbols.
 */
struct alias_value {};

tested::istream &operator>>(tested::istream &, alias_value &);

tested::wistream &operator>>(tested::wistream &, alias_value &);

static_assert(
    tested::is_same_v<tested::ranges::istream_view<alias_value>,
                      tested::ranges::basic_istream_view<alias_value, char>>);

static_assert(tested::is_same_v<
              tested::ranges::wistream_view<alias_value>,
              tested::ranges::basic_istream_view<alias_value, wchar_t>>);

static_assert(tested::ranges::view<tested_view>);

static_assert(tested::ranges::input_range<tested_view>);

static_assert(!tested::ranges::forward_range<tested_view>);

static_assert(!tested::ranges::common_range<tested_view>);

using tested_iterator = tested::ranges::iterator_t<tested_view>;

static_assert(tested::is_same_v<typename tested_iterator::iterator_concept,
                                tested::input_iterator_tag>);

static_assert(
    tested::is_same_v<typename tested_iterator::value_type, extracted_value>);

static_assert(!tested::is_copy_constructible_v<tested_iterator>);

static_assert(tested::is_move_constructible_v<tested_iterator>);

bool direct_view_works() {
  char characters[] = {'a', 'b', 'c'};

  fixed_streambuf buffer{characters, characters + 3};

  tested_istream stream{&buffer};

  tested_view view{stream};

  auto iterator = view.begin();
  const auto sentinel = view.end();

  if (iterator == sentinel || (*iterator).value != 'a') {
    return false;
  }

  extracted_value *storage = &*iterator;

  ++iterator;

  if (iterator == sentinel || &*iterator != storage ||
      (*iterator).value != 'b') {
    return false;
  }

  iterator++;

  if (iterator == sentinel || &*iterator != storage ||
      (*iterator).value != 'c') {
    return false;
  }

  ++iterator;

  return iterator == sentinel && stream.eof() && stream.fail();
}

bool empty_stream_works() {
  fixed_streambuf buffer{nullptr, nullptr};

  tested_istream stream{&buffer};

  tested_view view{stream};

  auto iterator = view.begin();

  return iterator == view.end() && stream.eof() && stream.fail();
}

bool customization_point_works() {
  char characters[] = {'x', 'y'};

  fixed_streambuf buffer{characters, characters + 2};

  tested_istream stream{&buffer};

  auto view = tested::views::istream<extracted_value>(stream);

  static_assert(tested::is_same_v<decltype(view), tested_view>);

  auto iterator = view.begin();

  if ((*iterator).value != 'x')
    return false;

  ++iterator;

  if ((*iterator).value != 'y')
    return false;

  ++iterator;

  return iterator == view.end();
}

bool derived_stream_works() {
  char character = 'q';

  fixed_streambuf buffer{&character, &character + 1};

  derived_istream stream{&buffer};

  auto view = tested::views::istream<extracted_value>(stream);

  static_assert(tested::is_same_v<decltype(view), tested_view>);

  auto iterator = view.begin();

  if ((*iterator).value != 'q')
    return false;

  ++iterator;

  return iterator == view.end();
}

bool ftl_test() {
  return direct_view_works() && empty_stream_works() &&
         customization_point_works() && derived_stream_works();
}
