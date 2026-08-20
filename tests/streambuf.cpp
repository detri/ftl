#ifdef FTL_REPLACE_STL
#include <streambuf>
namespace tested = std;
#else
#include <ftl/streambuf>
namespace tested = ftl;
#endif

struct test_char_traits : tested::char_traits<char> {};

using tested_streambuf = tested::basic_streambuf<char, test_char_traits>;

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

class writable_streambuf : public tested_streambuf {
public:
  writable_streambuf(char *first, char *last) { setp(first, last); }

  char *beginning() const noexcept { return pbase(); }
  char *current() const noexcept { return pptr(); }
  char *ending() const noexcept { return epptr(); }

  void rewind(int count) { pbump(-count); }

  void swap_with(writable_streambuf &other) { swap(other); }

protected:
  pos_type seekoff(off_type offset, tested::ios_base::seekdir direction,
                   tested::ios_base::openmode) override {
    if (direction != tested::ios_base::beg)
      return pos_type(off_type(-1));
    return pos_type(offset);
  }

  int sync() override { return 0; }
};

static_assert(
    tested::is_same_v<tested::streambuf, tested::basic_streambuf<char>>);

static_assert(tested::is_same_v<tested::streambuf::char_type, char>);

static_assert(tested::is_same_v<tested::streambuf::traits_type,
                                tested::char_traits<char>>);

static_assert(tested::is_same_v<tested::streambuf::int_type,
                                tested::char_traits<char>::int_type>);

struct marker_facet : tested::locale::facet {
  static tested::locale::id id;
};

tested::locale::id marker_facet::id;

bool swap_preserves_locale_state() {
  char left_storage[1]{};
  char right_storage[1]{};

  writable_streambuf left(left_storage, left_storage + 1);
  writable_streambuf right(right_storage, right_storage + 1);

  const tested::locale classic = tested::locale::classic();
  const tested::locale marked(classic, new marker_facet);

  left.pubimbue(classic);
  right.pubimbue(marked);

  left.swap_with(right);

  return left.getloc() == marked && right.getloc() == classic;
}

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

bool put_area_and_positioning_work() {
  char storage[6]{};
  writable_streambuf buffer(storage, storage + 6);
  if (buffer.sputc('a') != test_char_traits::to_int_type('a') ||
      buffer.sputn("bc", 2) != 2 || buffer.current() != storage + 3)
    return false;
  buffer.rewind(1);
  if (buffer.sputc('d') != test_char_traits::to_int_type('d') ||
      storage[0] != 'a' || storage[1] != 'b' || storage[2] != 'd')
    return false;
  return buffer.pubseekoff(4, tested::ios_base::beg, tested::ios_base::out) ==
             tested::streampos(4) &&
         buffer.pubsync() == 0;
}

bool locale_state_works() {
  char storage[1]{};
  writable_streambuf buffer(storage, storage + 1);
  tested::locale previous = buffer.pubimbue(tested::locale::classic());
  return previous == tested::locale() &&
         buffer.getloc() == tested::locale::classic();
}

bool ftl_test() {
  return fixed_get_area_works() && protected_area_access_works() &&
         virtual_underflow_works() && bulk_virtual_read_works() &&
         put_area_and_positioning_work() && locale_state_works() &&
         swap_preserves_locale_state();
}
