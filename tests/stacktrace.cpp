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
  return skipped.size() <= 3 && (trace == trace) &&
         tested::hash<tested::stacktrace>{}(trace) ==
             tested::hash<tested::stacktrace>{}(trace);
}
