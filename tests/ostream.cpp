#ifdef FTL_REPLACE_STL
#include <ostream>
#include <string>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/ostream>
#include <ftl/string>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

class output_buffer : public tested::streambuf {
public:
  tested::string text;
  int synchronizations = 0;

protected:
  int_type overflow(int_type value) override {
    if (traits_type::eq_int_type(value, traits_type::eof()))
      return traits_type::not_eof(value);
    text.push_back(traits_type::to_char_type(value));
    return value;
  }

  tested::streamsize xsputn(const char *source,
                            tested::streamsize count) override {
    text.append(source, static_cast<tested::size_t>(count));
    return count;
  }

  int sync() override {
    ++synchronizations;
    return 0;
  }
};

#if FTL_HAS_EXCEPTIONS
struct streambuf_exception {};

class throwing_output_buffer : public tested::streambuf {
protected:
  int_type overflow(int_type) override { throw streambuf_exception{}; }
  tested::streamsize xsputn(const char *, tested::streamsize) override {
    throw streambuf_exception{};
  }
  int sync() override { throw streambuf_exception{}; }
  pos_type seekoff(off_type, tested::ios_base::seekdir,
                   tested::ios_base::openmode) override {
    throw streambuf_exception{};
  }
  pos_type seekpos(pos_type, tested::ios_base::openmode) override {
    throw streambuf_exception{};
  }
};

class throwing_source_buffer : public tested::streambuf {
protected:
  int_type underflow() override { throw streambuf_exception{}; }
};

template <class Operation>
bool records_badbit(Operation operation) {
  throwing_output_buffer buffer;
  tested::ostream stream(&buffer);
  try { operation(stream); } catch (...) { return false; }
  if (!stream.bad()) return false;

  throwing_output_buffer rethrow_buffer;
  tested::ostream rethrow_stream(&rethrow_buffer);
  rethrow_stream.exceptions(tested::ios_base::badbit);
  try {
    operation(rethrow_stream);
  } catch (const streambuf_exception &) {
    return rethrow_stream.bad();
  } catch (...) {}
  return false;
}

bool exception_state_works() {
  if (!records_badbit([](tested::ostream &s) { s.put('x'); }) ||
      !records_badbit([](tested::ostream &s) { s.write("x", 1); }) ||
      !records_badbit([](tested::ostream &s) { s.flush(); }) ||
      !records_badbit([](tested::ostream &s) { s << 'x'; }) ||
      !records_badbit([](tested::ostream &s) { s << 42; }))
    return false;

  output_buffer destination;
  throwing_source_buffer source;
  tested::ostream stream(&destination);
  try { stream << &source; } catch (...) { return false; }
  if (!stream.fail() || stream.bad()) return false;

  output_buffer rethrow_destination;
  throwing_source_buffer rethrow_source;
  tested::ostream rethrow_stream(&rethrow_destination);
  rethrow_stream.exceptions(tested::ios_base::failbit);
  try {
    rethrow_stream << &rethrow_source;
  } catch (const streambuf_exception &) {
    return rethrow_stream.fail() && !rethrow_stream.bad();
  } catch (...) {}
  return false;
}
#endif

static_assert(tested::is_same_v<tested::ostream,
                                tested::basic_ostream<char>>);
static_assert(tested::is_same_v<
              decltype(tested::declval<tested::ostream &>() << 1),
              tested::ostream &>);
static_assert(tested::is_same_v<
              decltype(tested::declval<tested::ostream &>().put('x')),
              tested::ostream &>);
static_assert(tested::is_same_v<
              decltype(tested::declval<tested::ostream &>().tellp()),
              tested::ostream::pos_type>);

bool formatted_output_works() {
  output_buffer buffer;
  tested::ostream stream(&buffer);
  stream << tested::boolalpha << true << ' ' << tested::showbase
         << tested::hex << 42 << ' ' << 1.5;
  return buffer.text == "true 0x2a 1.5" && stream.good();
}

bool padding_and_width_reset_work() {
  output_buffer buffer;
  tested::ostream stream(&buffer);
  stream.fill('_');
  stream.width(5);
  stream << tested::right << "xy";
  if (buffer.text != "___xy" || stream.width() != 0)
    return false;
  stream.width(4);
  stream << tested::left << 'z';
  return buffer.text == "___xyz___" && stream.width() == 0;
}

bool unformatted_output_and_manipulators_work() {
  output_buffer buffer;
  tested::ostream stream(&buffer);
  const char text[] = {'b', 'c'};
  stream.put('a').write(text, 2);
  tested::endl(stream);
  tested::ends(stream);
  return buffer.text.size() == 5 && buffer.text[0] == 'a' &&
         buffer.text[1] == 'b' && buffer.text[2] == 'c' &&
         buffer.text[3] == '\n' && buffer.text[4] == '\0' &&
         buffer.synchronizations == 1 && stream.good();
}

bool unitbuf_and_failure_state_work() {
  output_buffer buffer;
  tested::ostream stream(&buffer);
  stream << tested::unitbuf << 'x';
  if (buffer.synchronizations != 1)
    return false;
  tested::ostream missing(nullptr);
  missing.put('x');
  return missing.bad() && missing.fail();
}

bool ftl_test() {
  return formatted_output_works() && padding_and_width_reset_work() &&
         unformatted_output_and_manipulators_work() &&
         unitbuf_and_failure_state_work()
#if FTL_HAS_EXCEPTIONS
         && exception_state_works()
#endif
         ;
}
