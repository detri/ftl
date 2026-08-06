#ifdef FTL_REPLACE_STL
#include <ios>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/ios>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct test_char_traits : tested::char_traits<char> {};

using tested_streambuf = tested::basic_streambuf<char, test_char_traits>;

using tested_ios = tested::basic_ios<char, test_char_traits>;

class fixed_streambuf : public tested_streambuf {
public:
  fixed_streambuf(char *first, char *last) { setg(first, first, last); }
};

class default_constructed_ios : public tested_ios {
public:
  default_constructed_ios() = default;

  void initialize(tested_streambuf *buffer) { init(buffer); }

  void replace_without_clearing(tested_streambuf *buffer) { set_rdbuf(buffer); }
};

static_assert(tested::is_same_v<tested::ios_base::iostate, unsigned int>);

static_assert(tested::ios_base::goodbit == 0);

static_assert((tested::ios_base::badbit & tested::ios_base::eofbit) == 0);

static_assert((tested::ios_base::badbit & tested::ios_base::failbit) == 0);

static_assert((tested::ios_base::eofbit & tested::ios_base::failbit) == 0);

static_assert(tested::is_base_of_v<tested::ios_base, tested_ios>);

static_assert(tested::is_same_v<tested_ios::char_type, char>);

static_assert(tested::is_same_v<tested_ios::traits_type, test_char_traits>);

static_assert(
    tested::is_same_v<tested_ios::int_type, test_char_traits::int_type>);

static_assert(tested::is_same_v<tested::ios, tested::basic_ios<char>>);

bool initial_state_works() {
  char characters[] = {'a', 'b', 'c'};

  fixed_streambuf buffer{characters, characters + 3};

  tested_ios stream{&buffer};

  return stream.rdbuf() == &buffer &&
         stream.rdstate() == tested::ios_base::goodbit && stream.good() &&
         !stream.eof() && !stream.fail() && !stream.bad() &&
         static_cast<bool>(stream) && !stream.operator!();
}

bool state_flags_work() {
  char character = 'x';

  fixed_streambuf buffer{&character, &character + 1};

  tested_ios stream{&buffer};

  stream.setstate(tested::ios_base::eofbit);

  if (!stream.eof() || stream.fail() || !static_cast<bool>(stream)) {
    return false;
  }

  stream.setstate(tested::ios_base::failbit);

  if (!stream.eof() || !stream.fail() || stream.bad() ||
      static_cast<bool>(stream) || !stream.operator!()) {
    return false;
  }

  stream.clear();

  if (!stream.good() || stream.rdstate() != tested::ios_base::goodbit) {
    return false;
  }

  stream.setstate(tested::ios_base::badbit);

  return stream.bad() && stream.fail() && !static_cast<bool>(stream);
}

bool null_buffer_state_works() {
  tested_ios stream{nullptr};

  if (stream.rdbuf() != nullptr || !stream.bad() || !stream.fail() ||
      static_cast<bool>(stream)) {
    return false;
  }

  stream.clear(tested::ios_base::goodbit);

  return stream.rdstate() == tested::ios_base::badbit && stream.bad();
}

bool buffer_replacement_works() {
  char first_character = 'a';
  char second_character = 'b';

  fixed_streambuf first{&first_character, &first_character + 1};

  fixed_streambuf second{&second_character, &second_character + 1};

  tested_ios stream{&first};

  stream.setstate(tested::ios_base::failbit);

  auto *previous = stream.rdbuf(&second);

  if (previous != &first || stream.rdbuf() != &second || !stream.good()) {
    return false;
  }

  previous = stream.rdbuf(nullptr);

  return previous == &second && stream.rdbuf() == nullptr && stream.bad() &&
         stream.fail();
}

bool protected_initialization_works() {
  char character = 'q';

  fixed_streambuf buffer{&character, &character + 1};

  default_constructed_ios stream;

  if (stream.rdbuf() != nullptr || !stream.bad()) {
    return false;
  }

  stream.initialize(&buffer);

  if (stream.rdbuf() != &buffer || !stream.good()) {
    return false;
  }

  stream.setstate(tested::ios_base::eofbit);

  stream.replace_without_clearing(nullptr);

  return stream.rdbuf() == nullptr &&
         stream.rdstate() == tested::ios_base::eofbit;
}

bool ftl_test() {
  return initial_state_works() && state_flags_work() &&
         null_buffer_state_works() && buffer_replacement_works() &&
         protected_initialization_works();
}