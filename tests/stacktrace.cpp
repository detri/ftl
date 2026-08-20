#ifdef FTL_REPLACE_STL
#include <stacktrace>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/stacktrace>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(tested::is_default_constructible_v<tested::stacktrace_entry>);
static_assert(tested::is_nothrow_copy_constructible_v<tested::stacktrace_entry>);
static_assert(tested::is_same_v<tested::stacktrace::value_type,
                                tested::stacktrace_entry>);
static_assert(noexcept(tested::stacktrace::current()));

class stacktrace_output_buffer : public tested::streambuf {
public:
  tested::string text;
protected:
  int_type overflow(int_type value) override {
    if (traits_type::eq_int_type(value, traits_type::eof()))
      return traits_type::not_eof(value);
    text.push_back(traits_type::to_char_type(value));
    return value;
  }
};

bool ftl_test() {
  tested::stacktrace empty;
  if (!empty.empty() || empty.begin() != empty.end() || !tested::to_string(empty).empty())
    return false;

  const auto trace = tested::stacktrace::current(0, 8);
  if (trace.size() > 8 || (!trace.empty() && !trace[0]))
    return false;
  if (!trace.empty() && tested::to_string(trace[0]).empty())
    return false;

  const auto skipped = tested::stacktrace::current(1, 3);
  const auto full_tail = tested::stacktrace::current(0, 128);
  const auto skipped_tail = tested::stacktrace::current(2, 128);
  const auto expected_tail_size = full_tail.size() > 2 ? full_tail.size() - 2 : 0;
  if (skipped_tail.size() != expected_tail_size)
    return false;
  stacktrace_output_buffer buffer;
  tested::ostream stream(&buffer);
  stream << trace;
  if (buffer.text != tested::to_string(trace) ||
      tested::format("{}", trace) != tested::to_string(trace))
    return false;
  if (!trace.empty() && tested::format("{:>32}", trace[0]).size() < 32)
    return false;
  return skipped.size() <= 3 && (trace == trace) &&
         tested::hash<tested::stacktrace>{}(trace) ==
             tested::hash<tested::stacktrace>{}(trace);
}
