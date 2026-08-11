#ifdef FTL_REPLACE_STL
#include <string>
#include <syncstream>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/string>
#include <ftl/syncstream>
#include <ftl/type_traits>
namespace tested = ftl;
#endif
#if __cpp_lib_syncbuf != 201803L
#error bad syncbuf feature macro
#endif
class sink : public tested::streambuf {
public:
  tested::string value;

protected:
  tested::streamsize xsputn(const char *s, tested::streamsize n) override {
    value.append(s, static_cast<tested::size_t>(n));
    return n;
  }
  int_type overflow(int_type c) override {
    if (!traits_type::eq_int_type(c, traits_type::eof()))
      value.push_back(traits_type::to_char_type(c));
    return c;
  }
};
bool ftl_test() {
  sink target;
  {
    tested::osyncstream first(&target);
    first << "alpha";
    if (!target.value.empty())
      return false;
    first.emit();
    if (target.value != "alpha")
      return false;
  }
  {
    tested::syncbuf buffer(&target);
    buffer.sputn("beta", 4);
    buffer.set_emit_on_sync(true);
    if (buffer.pubsync() != 0)
      return false;
  }
  return target.value == "alphabeta";
}
