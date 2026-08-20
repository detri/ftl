#ifdef FTL_REPLACE_STL
#include <ctime>
#include <limits>
namespace tested = std;
#else
#include <ftl/ctime>
#include <ftl/limits>
namespace tested = ftl;
#endif

static_assert(TIME_UTC > 0);

static_assert(requires(tested::timespec value) {
  value.tv_sec;
  value.tv_nsec;
});

bool ftl_test() {
  if (tested::difftime(tested::numeric_limits<tested::time_t>::max(),
                       tested::numeric_limits<tested::time_t>::min()) <= 0.0)
    return false;

  tested::time_t before_epoch = -1;
  const auto *before = tested::gmtime(&before_epoch);
  if (!before || before->tm_year != 69 || before->tm_mon != 11 ||
      before->tm_mday != 31 || before->tm_hour != 23 ||
      before->tm_min != 59 || before->tm_sec != 59)
    return false;
#if !defined(_WIN32)
  if (before->tm_gmtoff != 0 || before->tm_zone == nullptr ||
      before->tm_zone[0] != 'U' || before->tm_zone[1] != 'T' ||
      before->tm_zone[2] != 'C' || before->tm_zone[3] != '\0')
    return false;
#endif

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
