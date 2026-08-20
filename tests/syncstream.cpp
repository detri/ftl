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
  int synchronizations = 0;

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
  int sync() override {
    ++synchronizations;
    return 0;
  }
};

#if FTL_HAS_EXCEPTIONS
struct sink_exception {};
class throwing_sink : public tested::streambuf {
protected:
  tested::streamsize xsputn(const char *, tested::streamsize) override {
    throw sink_exception{};
  }
};
#endif

static_assert(!noexcept(tested::declval<tested::osyncstream &>() =
                        tested::declval<tested::osyncstream &&>()));

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
  if (target.value != "alphabeta")
    return false;
  target.synchronizations = 0;

  {
    tested::osyncstream stream(&target);
    stream << "one" << tested::emit_on_flush << tested::flush;
    if (target.value != "alphabetaone" || target.synchronizations != 1)
      return false;
    stream << tested::noemit_on_flush << "two" << tested::flush;
    if (target.value != "alphabetaone")
      return false;
    stream << tested::flush_emit;
    if (target.value != "alphabetaonetwo" || target.synchronizations != 2)
      return false;
  }

  {
    tested::syncbuf first(&target);
    tested::syncbuf second(&target);
    const tested::locale first_locale = tested::locale::classic();
    const tested::locale second_locale(first_locale, new tested::ctype<char>);
    first.pubimbue(first_locale);
    second.pubimbue(second_locale);
    first.swap(second);
    if (!(first.getloc() == second_locale) || !(second.getloc() == first_locale))
      return false;
  }

#if FTL_HAS_EXCEPTIONS
  {
    throwing_sink target;
    tested::osyncstream stream(&target);
    stream << "x";
    try { stream.emit(); } catch (...) { return false; }
    if (!stream.bad()) return false;
  }
  {
    throwing_sink target;
    tested::osyncstream stream(&target);
    stream << "x";
    stream.exceptions(tested::ios_base::badbit);
    try {
      stream.emit();
    } catch (const sink_exception &) {
      if (!stream.bad()) return false;
    } catch (...) { return false; }
  }
#endif

  return true;
}
