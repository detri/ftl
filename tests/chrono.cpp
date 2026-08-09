#ifdef FTL_REPLACE_STL
#include <chrono>
namespace tested = std;
#else
#include <ftl/chrono>
namespace tested = ftl;
#endif

using namespace tested::chrono;
using namespace tested::chrono::literals;
static_assert(duration_cast<seconds>(1500ms).count() == 1);
static_assert(is_clock_v<system_clock> && is_clock_v<steady_clock>);
static_assert(!is_clock_v<int>);
static_assert((++time_point<system_clock, seconds>{seconds{1}})
                  .time_since_epoch() == seconds{2});
static_assert(floor<seconds>(-1500ms).count() == -2);
static_assert(ceil<seconds>(1500ms).count() == 2);
static_assert(round<seconds>(2500ms).count() == 2);
static_assert(year{2000}.is_leap() && !year{1900}.is_leap());
static_assert(year_month_day{sys_days{days{0}}} == year{1970} / January / 1);
static_assert(sys_days{year{2000} / February / 29}.time_since_epoch().count() ==
              11016);
static_assert(weekday{sys_days{days{0}}} == Thursday);
static_assert((year{2024} / February / last).day() == day{29});
static_assert((year{2024} / March / Friday[1]).ok());
static_assert(weekday{local_days{days{0}}} == Thursday);
static_assert(year{2024} / March + months{10} == year{2025} / January);
static_assert(year{2020} / January - year{2019} / December == months{1});
static_assert(hh_mm_ss<milliseconds>::fractional_width == 3);
static_assert(hh_mm_ss<duration<int, tested::ratio<1, 3>>>::fractional_width ==
              6);
static_assert(hh_mm_ss{1501ms}.subseconds() == milliseconds{501});
static_assert(2024y / February / 29d == year{2024} / February / day{29});
static_assert(noexcept(day{1}) && noexcept(++tested::declval<day &>()));
static_assert(noexcept(month{1}) && noexcept(year{2024}.is_leap()));
static_assert(
    noexcept(tested::declval<const hh_mm_ss<milliseconds> &>().to_duration()));
static_assert(
    tested::is_same_v<file_time<seconds>, time_point<file_clock, seconds>>);
static_assert(utc_clock::from_sys(sys_seconds{seconds{0}}).time_since_epoch() ==
              seconds{0});
static_assert(tai_clock::from_utc(utc_seconds{seconds{0}}).time_since_epoch() ==
              seconds{378691210});
static_assert(gps_clock::from_utc(utc_seconds{seconds{315964809}})
                  .time_since_epoch() == seconds{0});
static_assert(clock_cast<tai_clock>(sys_seconds{seconds{0}})
                  .time_since_epoch() == seconds{378691210});
static_assert(clock_cast<system_clock>(gps_seconds{seconds{0}})
                  .time_since_epoch() == seconds{315964800});

bool ftl_test() {
  return system_clock::now().time_since_epoch() > seconds{0} &&
         steady_clock::now().time_since_epoch() > nanoseconds{0};
}
