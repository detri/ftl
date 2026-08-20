#ifdef FTL_REPLACE_STL
#include <iomanip>
#include <sstream>
namespace tested = std;
#else
#include <ftl/iomanip>
#include <ftl/sstream>
namespace tested = ftl;
#endif

#if FTL_HAS_EXCEPTIONS
struct streambuf_exception {};
class throwing_input : public tested::streambuf {
protected:
  int_type underflow() override { throw streambuf_exception{}; }
};
class throwing_output : public tested::streambuf {
protected:
  int_type overflow(int_type) override { throw streambuf_exception{}; }
  tested::streamsize xsputn(const char *, tested::streamsize) override {
    throw streambuf_exception{};
  }
};

template <class Manipulator>
bool input_exception_state(Manipulator manipulator) {
  throwing_input buffer;
  tested::istream stream(&buffer);
  try { stream >> manipulator; } catch (...) { return false; }
  if (!stream.bad()) return false;
  throwing_input rethrow_buffer;
  tested::istream rethrow_stream(&rethrow_buffer);
  rethrow_stream.exceptions(tested::ios_base::badbit);
  try { rethrow_stream >> manipulator; }
  catch (const streambuf_exception &) { return rethrow_stream.bad(); }
  catch (...) {}
  return false;
}

template <class Manipulator>
bool output_exception_state(Manipulator manipulator) {
  throwing_output buffer;
  tested::ostream stream(&buffer);
  try { stream << manipulator; } catch (...) { return false; }
  if (!stream.bad()) return false;
  throwing_output rethrow_buffer;
  tested::ostream rethrow_stream(&rethrow_buffer);
  rethrow_stream.exceptions(tested::ios_base::badbit);
  try { rethrow_stream << manipulator; }
  catch (const streambuf_exception &) { return rethrow_stream.bad(); }
  catch (...) {}
  return false;
}
#endif

bool ftl_test() {
  tested::ostringstream out;
  out << tested::setbase(16) << tested::setfill('0') << tested::setw(4) << 42;
  if (out.str() != "002a")
    return false;
  tested::string source = "a\\\"b";
  tested::ostringstream quoted;
  quoted << tested::quoted(source);
  if (quoted.str() != "\"a\\\\\\\"b\"")
    return false;
  tested::istringstream input(quoted.str());
  tested::string restored;
  input >> tested::quoted(restored);
  if (restored != source)
    return false;
#if FTL_HAS_EXCEPTIONS
  long double money = 0;
  tested::tm time{};
  if (!input_exception_state(tested::get_money(money)) ||
      !input_exception_state(tested::get_time(&time, "%Y")) ||
      !output_exception_state(tested::put_money(42.0L)) ||
      !output_exception_state(tested::put_time(&time, "%Y")))
    return false;
#endif
  return true;
}
