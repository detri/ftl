#ifdef FTL_REPLACE_STL
#include <istream>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/istream>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

using test_char_traits = tested::char_traits<char>;

using tested_streambuf = tested::basic_streambuf<char, test_char_traits>;

using tested_istream = tested::basic_istream<char, test_char_traits>;

class fixed_streambuf : public tested_streambuf {
public:
  fixed_streambuf(char *first, char *last) { setg(first, first, last); }
};

struct extracted_character {
  char value = '\0';
};

tested_istream &operator>>(tested_istream &stream, extracted_character &value) {
  return stream.get(value.value);
}

static_assert(tested::is_base_of_v<tested::basic_ios<char, test_char_traits>,
                                   tested_istream>);

static_assert(tested::is_same_v<tested_istream::char_type, char>);

static_assert(tested::is_same_v<tested_istream::traits_type, test_char_traits>);

static_assert(
    tested::is_same_v<tested_istream::int_type, test_char_traits::int_type>);

static_assert(tested::is_same_v<tested::istream, tested::basic_istream<char>>);
static_assert(tested::is_base_of_v<tested::istream, tested::iostream>);
static_assert(tested::is_base_of_v<tested::ostream, tested::iostream>);

bool get_works() {
  char characters[] = {'a', 'b'};

  fixed_streambuf buffer{characters, characters + 2};

  tested_istream stream{&buffer};

  const auto first = stream.get();

  if (test_char_traits::to_char_type(first) != 'a' || stream.gcount() != 1 ||
      !stream.good()) {
    return false;
  }

  char second = '\0';

  stream.get(second);

  if (second != 'b' || stream.gcount() != 1 || !stream.good()) {
    return false;
  }

  const auto end = stream.get();

  return test_char_traits::eq_int_type(end, test_char_traits::eof()) &&
         stream.gcount() == 0 && stream.eof() && stream.fail() &&
         !static_cast<bool>(stream);
}

bool exact_read_works() {
  char characters[] = {'1', '2', '3', '4'};

  fixed_streambuf buffer{characters, characters + 4};

  tested_istream stream{&buffer};

  char destination[4] = {};

  stream.read(destination, 4);

  return destination[0] == '1' && destination[1] == '2' &&
         destination[2] == '3' && destination[3] == '4' &&
         stream.gcount() == 4 && stream.good();
}

bool short_read_sets_state() {
  char characters[] = {'x', 'y'};

  fixed_streambuf buffer{characters, characters + 2};

  tested_istream stream{&buffer};

  char destination[4] = {};

  stream.read(destination, 4);

  return destination[0] == 'x' && destination[1] == 'y' &&
         stream.gcount() == 2 && stream.eof() && stream.fail() && !stream.bad();
}

bool null_buffer_works() {
  tested_istream stream{nullptr};

  char value = '\0';

  stream.get(value);

  return stream.gcount() == 0 && stream.bad() && stream.fail() &&
         !static_cast<bool>(stream);
}

bool user_defined_extraction_works() {
  char characters[] = {'p', 'q'};

  fixed_streambuf buffer{characters, characters + 2};

  tested_istream stream{&buffer};

  extracted_character first;
  extracted_character second;
  extracted_character end;

  stream >> first;
  stream >> second;

  if (first.value != 'p' || second.value != 'q' || !stream) {
    return false;
  }

  stream >> end;

  return stream.eof() && stream.fail() && !stream;
}

bool formatted_arithmetic_and_string_work() {
  char characters[] = {' ', '-', '4', '2', ' ', 't', 'r', 'u', 'e', ' ',
                       'w', 'o', 'r', 'd'};
  fixed_streambuf buffer{characters, characters + sizeof(characters)};
  tested_istream stream{&buffer};
  long number = 0;
  bool boolean = false;
  tested::basic_string<char, test_char_traits> word;
  stream >> number >> tested::boolalpha >> boolean >> word;
  return number == -42 && boolean && word == "word" && stream.eof() &&
         !stream.fail();
}

bool character_array_and_ws_work() {
  char characters[] = {' ', '\t', 'a', 'b', 'c', ' ', 'x'};
  fixed_streambuf buffer{characters, characters + sizeof(characters)};
  tested_istream stream{&buffer};
  tested::ws(stream);
  char text[4]{};
  stream >> text;
  char separator = '\0';
  stream.get(separator);
  return tested::string(text) == "abc" && separator == ' ' &&
         stream.gcount() == 1;
}

bool getline_and_ignore_work() {
  char characters[] = {'a', 'b', ',', 'c', 'd', '\n', 'x'};
  fixed_streambuf buffer{characters, characters + sizeof(characters)};
  tested_istream stream{&buffer};
  char first[4]{};
  stream.getline(first, 4, ',');
  if (tested::string(first) != "ab" || stream.gcount() != 3 || stream.fail())
    return false;
  tested::basic_string<char, test_char_traits> second;
  tested::getline(stream, second);
  if (second != "cd" || stream.fail())
    return false;
  stream.ignore(2);
  return stream.gcount() == 1 && stream.eof();
}

bool ftl_test() {
  return get_works() && exact_read_works() && short_read_sets_state() &&
         null_buffer_works() && user_defined_extraction_works() &&
         formatted_arithmetic_and_string_work() &&
         character_array_and_ws_work() && getline_and_ignore_work();
}
