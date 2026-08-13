#ifdef FTL_REPLACE_STL
#include <ctime>
namespace tested = std;
#else
#include <ftl/ctime>
namespace tested = ftl;
#endif

static_assert(TIME_UTC > 0);

static_assert(requires(tested::timespec value) {
  value.tv_sec;
  value.tv_nsec;
});

bool ftl_test() {
  tested::timespec current{};

  if (tested::timespec_get(&current, TIME_UTC) != TIME_UTC)
    return false;

  if (current.tv_nsec < 0 || current.tv_nsec >= 1000000000L)
    return false;

  if (tested::timespec_get(&current, TIME_UTC + 1) != 0)
    return false;

  tested::time_t epoch = 0;

  const auto *utc = tested::gmtime(&epoch);
  if (!utc || utc->tm_year != 70 || utc->tm_mon != 0 || utc->tm_mday != 1 ||
      utc->tm_wday != 4)
    return false;

  const auto *text = tested::asctime(utc);
  constexpr char expected[] = "Thu Jan  1 00:00:00 1970\n";

  if (!text)
    return false;

  for (unsigned i = 0; i < sizeof(expected); ++i)
    if (text[i] != expected[i])
      return false;

  return true;
}
