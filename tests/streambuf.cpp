#ifdef FTL_REPLACE_STL
#include <streambuf>
namespace tested = std;
#else
#include <ftl/streambuf>
namespace tested = ftl;
#endif

struct test_char_traits
    : tested::char_traits<char> {};

using tested_streambuf =
    tested::basic_streambuf<
        char,
        test_char_traits
    >;

class fixed_streambuf : public tested_streambuf {
public:
  fixed_streambuf(char *first, char *last) { setg(first, first, last); }

  [[nodiscard]]
  char *beginning() const noexcept {
    return eback();
  }

  [[nodiscard]]
  char *current() const noexcept {
    return gptr();
  }

  [[nodiscard]]
  char *ending() const noexcept {
    return egptr();
  }

  void advance(int count) { gbump(count); }
};

class chunked_streambuf : public tested_streambuf {
public:
  chunked_streambuf(const char *first, tested::streamsize count) noexcept
      : source_(first), count_(count) {}

protected:
  int_type underflow() override {
    if (index_ >= count_)
      return traits_type::eof();

    current_ = source_[index_];
    ++index_;

    setg(&current_, &current_, &current_ + 1);

    return traits_type::to_int_type(current_);
  }

private:
  const char *source_ = nullptr;
  tested::streamsize count_ = 0;
  tested::streamsize index_ = 0;
  char current_ = '\0';
};

static_assert(
    tested::is_same_v<tested::streambuf, tested::basic_streambuf<char>>);

static_assert(tested::is_same_v<tested::streambuf::char_type, char>);

static_assert(tested::is_same_v<tested::streambuf::traits_type,
                                tested::char_traits<char>>);

static_assert(tested::is_same_v<tested::streambuf::int_type,
                                tested::char_traits<char>::int_type>);

bool fixed_get_area_works() {
  char values[] = {'a', 'b', 'c', 'd'};

  fixed_streambuf buffer{values, values + 4};

  using traits = test_char_traits;

  if (buffer.beginning() != values || buffer.current() != values ||
      buffer.ending() != values + 4 || buffer.in_avail() != 4) {
    return false;
  }

  if (traits::to_char_type(buffer.sgetc()) != 'a' ||
      buffer.current() != values) {
    return false;
  }

  if (traits::to_char_type(buffer.sbumpc()) != 'a' ||
      buffer.current() != values + 1 || buffer.in_avail() != 3) {
    return false;
  }

  /*
   * snextc consumes 'b' and observes 'c'
   * without consuming 'c'.
   */
  if (traits::to_char_type(buffer.snextc()) != 'c' ||
      buffer.current() != values + 2) {
    return false;
  }

  char extracted[2] = {};

  if (buffer.sgetn(extracted, 2) != 2 || extracted[0] != 'c' ||
      extracted[1] != 'd' || buffer.in_avail() != 0) {
    return false;
  }

  return traits::eq_int_type(buffer.sgetc(), traits::eof());
}

bool protected_area_access_works() {
  char values[] = {'x', 'y', 'z'};

  fixed_streambuf buffer{values, values + 3};

  buffer.advance(2);

  return buffer.current() == values + 2 &&
         test_char_traits::to_char_type(buffer.sgetc()) == 'z';
}

bool virtual_underflow_works() {
  constexpr char values[] = {'1', '2', '3'};

  chunked_streambuf buffer{values, 3};

  using traits = test_char_traits;

  if (traits::to_char_type(buffer.sgetc()) != '1') {
    return false;
  }

  /*
   * Repeated sgetc does not consume the
   * character installed by underflow.
   */
  if (traits::to_char_type(buffer.sgetc()) != '1') {
    return false;
  }

  if (traits::to_char_type(buffer.sbumpc()) != '1') {
    return false;
  }

  if (traits::to_char_type(buffer.sbumpc()) != '2') {
    return false;
  }

  if (traits::to_char_type(buffer.sbumpc()) != '3') {
    return false;
  }

  return traits::eq_int_type(buffer.sbumpc(), traits::eof());
}

bool bulk_virtual_read_works() {
  constexpr char values[] = {'a', 'b', 'c', 'd'};

  chunked_streambuf buffer{values, 4};

  char output[6] = {};

  const tested::streamsize extracted = buffer.sgetn(output, 6);

  return extracted == 4 && output[0] == 'a' && output[1] == 'b' &&
         output[2] == 'c' && output[3] == 'd';
}

bool ftl_test() {
  return fixed_get_area_works() && protected_area_access_works() &&
         virtual_underflow_works() && bulk_virtual_read_works();
}
