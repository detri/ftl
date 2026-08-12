#ifdef FTL_REPLACE_STL
#include <chrono>
#include <detail/tzdb_runtime.hpp>
#include <format>
#include <sstream>
namespace tested = std;
#else
#include <ftl/chrono>
#include <ftl/detail/tzdb_runtime.hpp>
#include <ftl/format>
#include <ftl/sstream>
namespace tested = ftl;
#endif

using namespace tested::chrono;
using namespace tested::chrono::literals;

static_assert(tested::formattable<seconds, char>);

static_assert(tested::formattable<milliseconds, char>);

static_assert(tested::formattable<seconds, wchar_t>);

static_assert(tested::formattable<hh_mm_ss<milliseconds>, char>);

static_assert(tested::formattable<year_month_day, char>);

static_assert(tested::formattable<weekday, char>);

static_assert(tested::formattable<sys_seconds, char>);

static_assert(tested::formattable<utc_seconds, char>);

static_assert(tested::formattable<tai_seconds, char>);

static_assert(tested::formattable<gps_seconds, char>);

static_assert(tested::formattable<file_seconds, char>);

static_assert(tested::formattable<local_seconds, char>);

static_assert(tested::formattable<year_month_day, wchar_t>);

using local_format_seconds = decltype(local_time_format(local_seconds{}));

static_assert(tested::formattable<local_format_seconds, char>);

static_assert(tested::formattable<local_format_seconds, wchar_t>);

static_assert(tested::formattable<zoned_time<seconds>, char>);

static_assert(tested::formattable<zoned_time<seconds>, wchar_t>);

static_assert(
    tested::is_same_v<decltype(tested::declval<const time_zone &>().to_sys(
                          tested::declval<const local_time<milliseconds> &>())),
                      sys_time<milliseconds>>);

static_assert(
    tested::is_same_v<decltype(tested::declval<const time_zone &>().to_local(
                          tested::declval<const sys_time<milliseconds> &>())),
                      local_time<milliseconds>>);

static_assert(
    tested::is_same_v<decltype(tested::declval<const time_zone &>().to_sys(
                          tested::declval<const local_time<minutes> &>())),
                      sys_time<seconds>>);

static_assert(
    tested::is_same_v<decltype(tested::declval<const leap_second &>().date()),
                      sys_seconds>);

static_assert(
    tested::is_same_v<decltype(tested::declval<const leap_second &>().value()),
                      seconds>);

static_assert(noexcept(tested::declval<const leap_second &>().date()));

static_assert(noexcept(tested::declval<const leap_second &>().value()));

static_assert(tested::is_same_v<
              decltype(tested::declval<const time_zone_link &>().name()),
              tested::string_view>);

static_assert(tested::is_same_v<
              decltype(tested::declval<const time_zone_link &>().target()),
              tested::string_view>);

static_assert(noexcept(tested::declval<const time_zone_link &>().name()));

static_assert(noexcept(tested::declval<const time_zone_link &>().target()));

static_assert(
    tested::is_same_v<typename tzdb_list::const_iterator::value_type, tzdb>);

static_assert(tested::is_same_v<typename tzdb_list::const_iterator::reference,
                                const tzdb &>);

static_assert(tested::is_same_v<typename tzdb_list::const_iterator::pointer,
                                const tzdb *>);

static_assert(
    tested::is_same_v<decltype(tested::declval<const tzdb_list &>().front()),
                      const tzdb &>);

static_assert(noexcept(tested::declval<const tzdb_list &>().front()));

static_assert(noexcept(tested::declval<const tzdb_list &>().begin()));

static_assert(noexcept(tested::declval<const tzdb_list &>().end()));

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
// static_assert(utc_clock::from_sys(sys_seconds{seconds{0}}).time_since_epoch()
// ==
//               seconds{0});
static_assert(tai_clock::from_utc(utc_seconds{seconds{0}}).time_since_epoch() ==
              seconds{378691210});
static_assert(gps_clock::from_utc(utc_seconds{seconds{315964809}})
                  .time_since_epoch() == seconds{0});
// static_assert(clock_cast<tai_clock>(sys_seconds{seconds{0}})
//                   .time_since_epoch() == seconds{378691210});
// static_assert(clock_cast<system_clock>(gps_seconds{seconds{0}})
//                   .time_since_epoch() == seconds{315964800});

bool equal_text(const char *first, const char *second) {
  tested::size_t index = 0;

  for (;;) {
    if (first[index] != second[index])
      return false;

    if (first[index] == '\0')
      return true;

    ++index;
  }
}

bool equal_text(const wchar_t *first, const wchar_t *second) {
  tested::size_t index = 0;

  for (;;) {
    if (first[index] != second[index])
      return false;

    if (first[index] == L'\0')
      return true;

    ++index;
  }
}

bool chrono_duration_formatting_works() {
  const auto default_seconds = tested::format("{}", seconds{42});

  if (!equal_text(default_seconds.c_str(), "42s")) {
    return false;
  }

  const auto default_millis = tested::format("{}", milliseconds{1500});

  if (!equal_text(default_millis.c_str(), "1500ms")) {
    return false;
  }

  const auto count = tested::format("{:%Q}", milliseconds{1500});

  if (!equal_text(count.c_str(), "1500")) {
    return false;
  }

  const auto suffix = tested::format("{:%q}", milliseconds{1500});

  if (!equal_text(suffix.c_str(), "ms")) {
    return false;
  }

  const auto time = tested::format("{:%T}", 3h + 2min + 1s);

  if (!equal_text(time.c_str(), "03:02:01")) {
    return false;
  }

  const auto fields =
      tested::format("{:%H hours %M minutes %S seconds}", 3h + 2min + 1s);

  if (!equal_text(fields.c_str(), "03 hours 02 minutes 01 seconds")) {
    return false;
  }

  const auto fractional = tested::format("{:%T}", 1h + 2min + 3004ms);

  if (!equal_text(fractional.c_str(), "01:02:03.004")) {
    return false;
  }

  const auto negative = tested::format("{:%T}", -10000s);

  if (!equal_text(negative.c_str(), "-02:46:40")) {
    return false;
  }

  /*
   * The sign appears immediately before
   * replacement of the first conversion
   * specifier, not necessarily at the
   * beginning of the chrono-spec.
   */
  const auto negative_literal =
      tested::format("{:%M minutes, %H hours, %S seconds}", -10000s);

  if (!equal_text(negative_literal.c_str(),
                  "-46 minutes, 02 hours, 40 seconds")) {
    return false;
  }

  const auto padded = tested::format("{:=>8}", milliseconds{42});

  if (!equal_text(padded.c_str(), "====42ms")) {
    return false;
  }

  const auto wide = tested::format(L"{:%T}", 1h + 2min + 3s);

  if (!equal_text(wide.c_str(), L"01:02:03")) {
    return false;
  }

  using custom_duration = duration<int, tested::ratio<2, 3>>;

  const auto custom_suffix = tested::format("{}", custom_duration{5});

  if (!equal_text(custom_suffix.c_str(), "5[2/3]s")) {
    return false;
  }

  return true;
}

bool chrono_hms_formatting_works() {
  const hh_mm_ss value{4083007ms};

  const auto default_value = tested::format("{}", value);

  if (!equal_text(default_value.c_str(), "01:08:03.007")) {
    return false;
  }

  const auto explicit_value = tested::format("{:%H:%M:%S}", value);

  if (!equal_text(explicit_value.c_str(), "01:08:03.007")) {
    return false;
  }

  const hh_mm_ss negative{-4083007ms};

  const auto negative_value = tested::format("{:%T}", negative);

  if (!equal_text(negative_value.c_str(), "-01:08:03.007")) {
    return false;
  }

  return true;
}

bool chrono_calendar_formatting_works() {
  const auto date = year{2024} / March / 14;

  const auto default_date = tested::format("{}", date);

  if (!equal_text(default_date.c_str(), "2024-03-14")) {
    return false;
  }

  const auto explicit_date = tested::format("{:%Y-%m-%d}", date);

  if (!equal_text(explicit_date.c_str(), "2024-03-14")) {
    return false;
  }

  const auto long_date = tested::format("{:%A, %B %d, %Y}", date);

  if (!equal_text(long_date.c_str(), "Thursday, March 14, 2024")) {
    return false;
  }

  const auto day_of_year = tested::format("{:%j}", date);

  if (!equal_text(day_of_year.c_str(), "074")) {
    return false;
  }

  const auto weekday_numbers = tested::format("{:%u/%w}", date);

  if (!equal_text(weekday_numbers.c_str(), "4/4")) {
    return false;
  }

  const auto iso = tested::format("{:%G-W%V-%u}", year{2024} / January / 1);

  if (!equal_text(iso.c_str(), "2024-W01-1")) {
    return false;
  }

  const auto month_value = tested::format("{}", March);

  if (!equal_text(month_value.c_str(), "Mar")) {
    return false;
  }

  const auto weekday_value = tested::format("{}", Thursday);

  if (!equal_text(weekday_value.c_str(), "Thu")) {
    return false;
  }

  const auto indexed = tested::format("{}", Friday[2]);

  if (!equal_text(indexed.c_str(), "Fri[2]")) {
    return false;
  }

  const auto last_weekday = tested::format("{}", Friday[last]);

  if (!equal_text(last_weekday.c_str(), "Fri[last]")) {
    return false;
  }

  const auto month_day_value = tested::format("{}", March / 14);

  if (!equal_text(month_day_value.c_str(), "Mar/14")) {
    return false;
  }

  const auto year_month_value = tested::format("{}", year{2024} / March);

  if (!equal_text(year_month_value.c_str(), "2024/Mar")) {
    return false;
  }

  const auto wide_date = tested::format(L"{:%F}", date);

  if (!equal_text(wide_date.c_str(), L"2024-03-14")) {
    return false;
  }

  return true;
}

bool chrono_clock_formatting_works() {
  const sys_seconds epoch{seconds{0}};

  const auto system_default = tested::format("{}", epoch);

  if (!equal_text(system_default.c_str(), "1970-01-01 00:00:00")) {
    return false;
  }

  const auto system_zone = tested::format("{:%F %T %Z %z}", epoch);

  if (!equal_text(system_zone.c_str(), "1970-01-01 00:00:00 UTC +0000")) {
    return false;
  }

  const auto system_colon_zone = tested::format("{:%Ez}", epoch);

  if (!equal_text(system_colon_zone.c_str(), "+00:00")) {
    return false;
  }

  const auto utc_epoch = utc_clock::from_sys(epoch);

  const auto utc_text = tested::format("{:%F %T %Z}", utc_epoch);

  if (!equal_text(utc_text.c_str(), "1970-01-01 00:00:00 UTC")) {
    return false;
  }

  const auto date_2000 = sys_seconds{sys_days{year{2000} / January / 1}};

  const auto tai = clock_cast<tai_clock>(date_2000);

  const auto tai_text = tested::format("{:%F %T %Z}", tai);

  if (!equal_text(tai_text.c_str(), "2000-01-01 00:00:32 TAI")) {
    return false;
  }

  const auto gps = clock_cast<gps_clock>(date_2000);

  const auto gps_text = tested::format("{:%F %T %Z}", gps);

  if (!equal_text(gps_text.c_str(), "2000-01-01 00:00:13 GPS")) {
    return false;
  }

  const file_seconds file_epoch{seconds{0}};

  const auto file_text = tested::format("{:%F %T %Z}", file_epoch);

  if (!equal_text(file_text.c_str(), "1970-01-01 00:00:00 UTC")) {
    return false;
  }

  const local_seconds local_epoch{seconds{0}};

  const auto local_text = tested::format("{}", local_epoch);

  if (!equal_text(local_text.c_str(), "1970-01-01 00:00:00")) {
    return false;
  }

  return true;
}

bool chrono_format_errors_work() {
  seconds value{42};

  auto store = tested::make_format_args(value);

  try {
    /*
     * A duration has no calendar year.
     */
    (void)tested::vformat("{:%Y}", tested::format_args(store));

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  try {
    /*
     * Precision is not valid for an
     * integral duration representation.
     */
    (void)tested::vformat("{:.2%Q}", tested::format_args(store));

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  hh_mm_ss hms{1s};

  auto hms_store = tested::make_format_args(hms);

  try {
    /*
     * %q is duration-only.
     */
    (void)tested::vformat("{:%q}", tested::format_args(hms_store));

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  local_seconds local{seconds{0}};

  auto local_store = tested::make_format_args(local);

  try {
    (void)tested::vformat("{:%Z}", tested::format_args(local_store));

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  try {
    (void)tested::vformat("{:%z}", tested::format_args(local_store));

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  month only_month{3};

  auto month_store = tested::make_format_args(only_month);

  try {
    /*
     * A month does not contain
     * enough information for %F.
     */
    (void)tested::vformat("{:%F}", tested::format_args(month_store));

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  return true;
}

bool chrono_tzdb_runtime_works() {
  namespace tz = tested::detail::tzdb_runtime;

  /*
   * Canonical name lookup.
   */
  const auto new_york = tz::locate_zone("America/New_York");

  if (!new_york)
    return false;

  if (!equal_text(tz::zone_name(new_york), "America/New_York")) {
    return false;
  }

  /*
   * backward contains:
   *
   *   Link America/New_York US/Eastern
   *
   * Links resolve to the canonical Zone.
   */
  const auto eastern_link = tz::find_link("US/Eastern");

  if (!eastern_link)
    return false;

  if (!equal_text(tz::link_name(eastern_link), "US/Eastern")) {
    return false;
  }

  const auto eastern_target = tz::link_target(eastern_link);

  if (!eastern_target || eastern_target.index != new_york.index) {
    return false;
  }

  const auto eastern = tz::locate_zone("US/Eastern");

  if (!eastern || eastern.index != new_york.index) {
    return false;
  }

  if (tz::locate_zone("Definitely/Not_A_Zone")) {
    return false;
  }

  /*
   * 2024-03-10 07:00:00 UTC is the
   * New York spring-forward transition.
   */
  const auto before_spring = tz::lookup(new_york, 1710053999LL);

  if (!before_spring)
    return false;

  if (before_spring.offset_seconds != -5 * 60 * 60) {
    return false;
  }

  if (before_spring.save_minutes != 0)
    return false;

  if (!equal_text(before_spring.abbreviation, "EST")) {
    return false;
  }

  if (before_spring.end != 1710054000LL) {
    return false;
  }

  /*
   * Transition equality belongs to the new state.
   */
  const auto spring = tz::lookup(new_york, 1710054000LL);

  if (!spring)
    return false;

  if (spring.begin_unbounded)
    return false;

  if (spring.begin != 1710054000LL) {
    return false;
  }

  if (spring.offset_seconds != -4 * 60 * 60) {
    return false;
  }

  if (spring.save_minutes != 60)
    return false;

  if (!equal_text(spring.abbreviation, "EDT")) {
    return false;
  }

  /*
   * Corresponding fall-back.
   */
  if (spring.end != 1730613600LL) {
    return false;
  }

  const auto fall = tz::lookup(new_york, 1730613600LL);

  if (!fall)
    return false;

  if (fall.begin != 1730613600LL) {
    return false;
  }

  if (fall.offset_seconds != -5 * 60 * 60) {
    return false;
  }

  if (fall.save_minutes != 0)
    return false;

  if (!equal_text(fall.abbreviation, "EST")) {
    return false;
  }

  /*
   * Fixed-zone initial-state path.
   */
  const auto utc = tz::locate_zone("Etc/UTC");

  if (!utc)
    return false;

  const auto utc_epoch = tz::lookup(utc, 0);

  if (!utc_epoch)
    return false;

  if (utc_epoch.offset_seconds != 0)
    return false;

  if (utc_epoch.save_minutes != 0)
    return false;

  if (!equal_text(utc_epoch.abbreviation, "UTC")) {
    return false;
  }

  /*
   * The precomputed horizon is now an internal
   * implementation boundary, not a coverage boundary.
   *
   * A lookup immediately before it must expose the
   * real next transition produced by the tail evaluator.
   */
  const long long horizon = tz::precomputed_until();

  const auto before_horizon = tz::lookup(new_york, horizon - 1);

  if (!before_horizon)
    return false;

  if (before_horizon.end_is_horizon)
    return false;

  if (before_horizon.end <= horizon)
    return false;

  if (before_horizon.offset_seconds != -5 * 60 * 60) {
    return false;
  }

  if (before_horizon.save_minutes != 0)
    return false;

  if (!equal_text(before_horizon.abbreviation, "EST")) {
    return false;
  }

  /*
   * Exact equality with the old artificial horizon
   * must continue seamlessly in the same winter state.
   */
  const auto at_horizon = tz::lookup(new_york, horizon);

  if (!at_horizon)
    return false;

  if (at_horizon.end_is_horizon)
    return false;

  if (at_horizon.offset_seconds != -5 * 60 * 60) {
    return false;
  }

  if (at_horizon.save_minutes != 0)
    return false;

  if (!equal_text(at_horizon.abbreviation, "EST")) {
    return false;
  }

  if (at_horizon.begin != before_horizon.begin) {
    return false;
  }

  if (at_horizon.end != before_horizon.end) {
    return false;
  }

  /*
   * A fixed zone also extends beyond the generated
   * transition horizon indefinitely.
   */
  const auto utc_future = tz::lookup(utc, horizon);

  if (!utc_future)
    return false;

  if (utc_future.offset_seconds != 0)
    return false;

  if (utc_future.save_minutes != 0)
    return false;

  if (!equal_text(utc_future.abbreviation, "UTC")) {
    return false;
  }

  return true;
}

bool chrono_time_zone_public_works() {
  const time_zone *new_york = nullptr;

  try {
    new_york = locate_zone("America/New_York");
  } catch (...) {
    return false;
  }

  if (new_york == nullptr)
    return false;

  if (new_york->name() != "America/New_York") {
    return false;
  }

  /*
   * A string_view need not be NUL terminated.
   */
  const char sliced_name[] = "America/New_York!";

  const time_zone *sliced = nullptr;

  try {
    sliced = locate_zone(tested::string_view{
        sliced_name,
        16,
    });
  } catch (...) {
    return false;
  }

  if (sliced != new_york)
    return false;

  /*
   * Link lookup returns the same canonical object.
   */
  const time_zone *eastern = nullptr;

  try {
    eastern = locate_zone("US/Eastern");
  } catch (...) {
    return false;
  }

  if (eastern != new_york)
    return false;

  if (eastern->name() != "America/New_York") {
    return false;
  }

  /*
   * 2024 spring-forward.
   */
  const auto before_spring = new_york->get_info(sys_seconds{
      seconds{
          1710053999LL,
      },
  });

  if (before_spring.offset != hours{-5}) {
    return false;
  }

  if (before_spring.save != minutes{0}) {
    return false;
  }

  if (!equal_text(before_spring.abbrev.c_str(), "EST")) {
    return false;
  }

  if (before_spring.end != sys_seconds{
                               seconds{
                                   1710054000LL,
                               },
                           }) {
    return false;
  }

  /*
   * Exact transition equality belongs to EDT.
   */
  const auto spring = new_york->get_info(sys_seconds{
      seconds{
          1710054000LL,
      },
  });

  if (spring.begin != sys_seconds{
                          seconds{
                              1710054000LL,
                          },
                      }) {
    return false;
  }

  if (spring.end != sys_seconds{
                        seconds{
                            1730613600LL,
                        },
                    }) {
    return false;
  }

  if (spring.offset != hours{-4})
    return false;

  if (spring.save != minutes{60})
    return false;

  if (!equal_text(spring.abbrev.c_str(), "EDT")) {
    return false;
  }

  /*
   * Subsecond boundary behavior.
   */
  const auto subsecond_before = new_york->get_info(sys_time<milliseconds>{
      milliseconds{
          1710053999999LL,
      },
  });

  if (subsecond_before.offset != hours{-5}) {
    return false;
  }

  if (!equal_text(subsecond_before.abbrev.c_str(), "EST")) {
    return false;
  }

  const auto subsecond_after = new_york->get_info(sys_time<milliseconds>{
      milliseconds{
          1710054000001LL,
      },
  });

  if (subsecond_after.offset != hours{-4}) {
    return false;
  }

  if (!equal_text(subsecond_after.abbrev.c_str(), "EDT")) {
    return false;
  }

  /*
   * Fixed-zone public path.
   */
  const time_zone *utc = nullptr;

  try {
    utc = locate_zone("Etc/UTC");
  } catch (...) {
    return false;
  }

  if (utc == nullptr)
    return false;

  const auto utc_epoch = utc->get_info(sys_seconds{
      seconds{0},
  });

  if (utc_epoch.offset != seconds{0})
    return false;

  if (utc_epoch.save != minutes{0})
    return false;

  if (!equal_text(utc_epoch.abbrev.c_str(), "UTC")) {
    return false;
  }

  /*
   * Unknown names throw.
   */
  try {
    (void)locate_zone("Definitely/Not_A_Zone");

    return false;
  } catch (const tested::runtime_error &) {
  } catch (...) {
    return false;
  }

  /*
   * Arbitrary-year evaluation has now landed.
   *
   * The generated horizon is therefore invisible
   * through the public time_zone API.
   */
  const auto horizon = sys_seconds{
      seconds{
          tested::detail::tzdb_runtime::precomputed_until(),
      },
  };

  sys_info horizon_info;

  try {
    horizon_info = new_york->get_info(horizon);
  } catch (...) {
    return false;
  }

  if (horizon_info.offset != hours{-5}) {
    return false;
  }

  if (horizon_info.save != minutes{0}) {
    return false;
  }

  if (!equal_text(horizon_info.abbrev.c_str(), "EST")) {
    return false;
  }

  if (horizon_info.begin > horizon)
    return false;

  if (horizon_info.end <= horizon)
    return false;

  /*
   * And the same public API works well beyond the
   * formerly generated coverage range.
   */
  const auto future =
      new_york->get_info(sys_days{year{2600} / July / 15} + hours{12});

  if (future.offset != hours{-4})
    return false;

  if (future.save != minutes{60})
    return false;

  if (!equal_text(future.abbrev.c_str(), "EDT")) {
    return false;
  }

  return true;
}

bool chrono_local_info_works() {
  const time_zone *new_york = nullptr;

  try {
    new_york = locate_zone("America/New_York");
  } catch (...) {
    return false;
  }

  if (new_york == nullptr)
    return false;

  /*
   * Normal summer time: 2024-07-01 12:00 local.
   */
  const auto summer = new_york->get_info(local_seconds{seconds{1719835200LL}});

  if (summer.result != local_info::unique)
    return false;

  if (summer.first.offset != hours{-4})
    return false;

  if (summer.first.save != minutes{60})
    return false;

  if (!equal_text(summer.first.abbrev.c_str(), "EDT"))
    return false;

  /*
   * Unique requires second to be zero-initialized.
   */
  if (summer.second.begin != sys_seconds{})
    return false;

  if (summer.second.end != sys_seconds{})
    return false;

  if (summer.second.offset != seconds{})
    return false;

  if (summer.second.save != minutes{})
    return false;

  if (!summer.second.abbrev.empty())
    return false;

  /*
   * Spring forward:
   *
   *   2024-03-10 01:59:59 EST
   *   2024-03-10 03:00:00 EDT
   *
   * Therefore 02:30 local does not exist.
   */
  const auto gap = new_york->get_info(local_seconds{seconds{1710037800LL}});

  if (gap.result != local_info::nonexistent)
    return false;

  if (gap.first.offset != hours{-5})
    return false;

  if (gap.first.save != minutes{0})
    return false;

  if (!equal_text(gap.first.abbrev.c_str(), "EST"))
    return false;

  if (gap.first.end != sys_seconds{seconds{1710054000LL}})
    return false;

  if (gap.second.offset != hours{-4})
    return false;

  if (gap.second.save != minutes{60})
    return false;

  if (!equal_text(gap.second.abbrev.c_str(), "EDT"))
    return false;

  if (gap.second.begin != sys_seconds{seconds{1710054000LL}})
    return false;

  /*
   * The exact lower edge of the gap is nonexistent.
   */
  const auto gap_begin =
      new_york->get_info(local_seconds{seconds{1710036000LL}});

  if (gap_begin.result != local_info::nonexistent)
    return false;

  /*
   * The exact upper edge belongs uniquely to the new state.
   */
  const auto gap_end = new_york->get_info(local_seconds{seconds{1710039600LL}});

  if (gap_end.result != local_info::unique)
    return false;

  if (gap_end.first.offset != hours{-4})
    return false;

  /*
   * Fall back:
   *
   * 01:30 occurs first under EDT and then again under EST.
   */
  const auto overlap = new_york->get_info(local_seconds{seconds{1730597400LL}});

  if (overlap.result != local_info::ambiguous)
    return false;

  /*
   * local_info::first is the earlier system-time interpretation.
   */
  if (overlap.first.offset != hours{-4})
    return false;

  if (overlap.first.save != minutes{60})
    return false;

  if (!equal_text(overlap.first.abbrev.c_str(), "EDT"))
    return false;

  if (overlap.first.end != sys_seconds{seconds{1730613600LL}})
    return false;

  /*
   * local_info::second is the later system-time interpretation.
   */
  if (overlap.second.offset != hours{-5})
    return false;

  if (overlap.second.save != minutes{0})
    return false;

  if (!equal_text(overlap.second.abbrev.c_str(), "EST"))
    return false;

  if (overlap.second.begin != sys_seconds{seconds{1730613600LL}})
    return false;

  /*
   * A fixed zone can never produce a gap or overlap.
   */
  const auto *utc = locate_zone("Etc/UTC");

  const auto utc_local = utc->get_info(local_seconds{seconds{1710037800LL}});

  if (utc_local.result != local_info::unique)
    return false;

  if (utc_local.first.offset != seconds{0})
    return false;

  if (!equal_text(utc_local.first.abbrev.c_str(), "UTC"))
    return false;

  /*
   * Subsecond values inside the spring gap remain nonexistent.
   */
  const auto gap_fraction = new_york->get_info(
      local_time<milliseconds>{milliseconds{1710037800123LL}});

  if (gap_fraction.result != local_info::nonexistent)
    return false;

  /*
   * Likewise a subsecond value inside the repeated fall-back hour
   * remains ambiguous.
   */
  const auto overlap_fraction = new_york->get_info(
      local_time<milliseconds>{milliseconds{1730597400123LL}});

  if (overlap_fraction.result != local_info::ambiguous)
    return false;

  return true;
}

bool chrono_time_zone_conversion_works() {
  const auto *new_york = locate_zone("America/New_York");
  const auto *utc = locate_zone("Etc/UTC");

  if (new_york == nullptr || utc == nullptr)
    return false;

  /*
   * Ordinary local -> sys conversion.
   *
   * 2024-07-01 12:00 EDT == 2024-07-01 16:00 UTC.
   */
  const local_seconds summer_local{seconds{1719835200LL}};

  const auto summer_sys = new_york->to_sys(summer_local);

  if (summer_sys != sys_seconds{seconds{1719849600LL}})
    return false;

  /*
   * sys -> local is the inverse for unique local times.
   */
  if (new_york->to_local(summer_sys) != summer_local)
    return false;

  /*
   * Preserve finer-than-second precision.
   */
  const local_time<milliseconds> precise_local{milliseconds{1719835200123LL}};

  const auto precise_sys = new_york->to_sys(precise_local);

  if (precise_sys != sys_time<milliseconds>{milliseconds{1719849600123LL}})
    return false;

  if (new_york->to_local(precise_sys) != precise_local)
    return false;

  /*
   * Spring forward:
   *
   * 2024-03-10 02:30 does not exist in New York.
   *
   * The throwing overload must report nonexistent_local_time.
   */
  const local_seconds gap{seconds{1710037800LL}};

  try {
    (void)new_york->to_sys(gap);
    return false;
  } catch (const nonexistent_local_time &) {
  } catch (...) {
    return false;
  }

  /*
   * For nonexistent local time, earliest and latest collapse onto the
   * same system transition instant.
   */
  const auto gap_earliest = new_york->to_sys(gap, choose::earliest);

  const auto gap_latest = new_york->to_sys(gap, choose::latest);

  if (gap_earliest != sys_seconds{seconds{1710054000LL}})
    return false;

  if (gap_latest != gap_earliest)
    return false;

  /*
   * Even a fractional point inside the gap collapses to that same
   * transition instant, but retains the required result duration type.
   */
  const local_time<milliseconds> precise_gap{milliseconds{1710037800123LL}};

  const auto precise_gap_sys = new_york->to_sys(precise_gap, choose::earliest);

  if (precise_gap_sys != sys_time<milliseconds>{milliseconds{1710054000000LL}})
    return false;

  /*
   * Fall back:
   *
   * 2024-11-03 01:30 occurs twice.
   */
  const local_seconds overlap{seconds{1730597400LL}};

  try {
    (void)new_york->to_sys(overlap);
    return false;
  } catch (const ambiguous_local_time &) {
  } catch (...) {
    return false;
  }

  /*
   * First occurrence: EDT, UTC-4.
   *
   * 01:30 EDT == 05:30 UTC.
   */
  const auto overlap_earliest = new_york->to_sys(overlap, choose::earliest);

  if (overlap_earliest != sys_seconds{seconds{1730611800LL}})
    return false;

  /*
   * Second occurrence: EST, UTC-5.
   *
   * 01:30 EST == 06:30 UTC.
   */
  const auto overlap_latest = new_york->to_sys(overlap, choose::latest);

  if (overlap_latest != sys_seconds{seconds{1730615400LL}})
    return false;

  if (!(overlap_earliest < overlap_latest))
    return false;

  /*
   * sys -> local maps both interpretations back to the same repeated
   * wall-clock value.
   */
  if (new_york->to_local(overlap_earliest) != overlap)
    return false;

  if (new_york->to_local(overlap_latest) != overlap)
    return false;

  /*
   * Exact transition instant after the spring-forward maps to 03:00 EDT.
   */
  const auto spring_transition_local =
      new_york->to_local(sys_seconds{seconds{1710054000LL}});

  if (spring_transition_local != local_seconds{seconds{1710039600LL}})
    return false;

  /*
   * Preserve subsecond precision across sys -> local as well.
   */
  const auto spring_fraction =
      new_york->to_local(sys_time<milliseconds>{milliseconds{1710054000123LL}});

  if (spring_fraction !=
      local_time<milliseconds>{milliseconds{1710039600123LL}})
    return false;

  /*
   * UTC is a useful negative-time precision sanity check.
   */
  const sys_time<milliseconds> before_epoch{milliseconds{-1}};

  if (utc->to_local(before_epoch) != local_time<milliseconds>{milliseconds{-1}})
    return false;

  if (utc->to_sys(local_time<milliseconds>{milliseconds{-1}}) != before_epoch)
    return false;

  /*
   * The exception classes themselves must derive from runtime_error.
   */
  static_assert(
      tested::is_base_of_v<tested::runtime_error, nonexistent_local_time>);

  static_assert(
      tested::is_base_of_v<tested::runtime_error, ambiguous_local_time>);

  return true;
}

bool chrono_leap_second_works() {
  namespace tz = tested::detail::tzdb_runtime;

  /*
   * The leap data now comes from the vendored IANA 2026c snapshot.
   */
  if (tz::leap_count() != 27)
    return false;

  /*
   * First positive leap:
   *
   * 1972-06-30 23:59:60 UTC
   * -> insertion boundary 1972-07-01 00:00:00 sys
   */
  if (tz::leap_date_seconds(0) != 78796800LL)
    return false;

  if (tz::leap_value(0) != 1)
    return false;

  /*
   * Most recent leap in the snapshot:
   *
   * 2016-12-31 23:59:60 UTC
   * -> insertion boundary 2017-01-01 00:00:00 sys
   */
  const unsigned last = tz::leap_count() - 1;

  if (tz::leap_date_seconds(last) != 1483228800LL)
    return false;

  if (tz::leap_value(last) != 1)
    return false;

  /*
   * The generator also carries the leap-table validity horizon from
   * leap-seconds.list.
   *
   * 2027-06-28 00:00:00 UTC.
   */
  if (tz::leap_expiration() != 1814140800LL)
    return false;

  /*
   * Cumulative system-time correction immediately before and at the
   * 2017 insertion boundary.
   */
  if (tz::leap_elapsed_at_sys(1483228799LL) != 26)
    return false;

  if (tz::leap_elapsed_at_sys(1483228800LL) != 27)
    return false;

  /*
   * utc_clock::from_sys now consumes that same generated table.
   *
   * Immediately before insertion:
   *
   * sys 1483228799 + 26 elapsed seconds.
   */
  const auto before = utc_clock::from_sys(sys_seconds{seconds{1483228799LL}});

  if (before != utc_seconds{seconds{1483228825LL}})
    return false;

  /*
   * At the system boundary the new leap has been inserted:
   *
   * sys 1483228800 + 27 elapsed seconds.
   */
  const auto after = utc_clock::from_sys(sys_seconds{seconds{1483228800LL}});

  if (after != utc_seconds{seconds{1483228827LL}})
    return false;

  /*
   * The missing UTC second between those two values is the leap second.
   */
  const utc_seconds leap{
      seconds{1483228826LL},
  };

  const auto during = get_leap_second_info(leap);

  if (!during.is_leap_second)
    return false;

  /*
   * The currently active insertion counts toward elapsed.
   */
  if (during.elapsed != seconds{27})
    return false;

  /*
   * Immediately before it, only 26 insertions have elapsed.
   */
  const auto before_info =
      get_leap_second_info(utc_seconds{seconds{1483228825LL}});

  if (before_info.is_leap_second)
    return false;

  if (before_info.elapsed != seconds{26})
    return false;

  /*
   * Immediately after it, the 27th correction remains in force but we
   * are no longer inside the insertion itself.
   */
  const auto after_info =
      get_leap_second_info(utc_seconds{seconds{1483228827LL}});

  if (after_info.is_leap_second)
    return false;

  if (after_info.elapsed != seconds{27})
    return false;

  /*
   * Existing utc_clock::to_sys leap behavior remains backed by the
   * generated database as well.
   */
  if (utc_clock::to_sys(leap) != sys_seconds{seconds{1483228799LL}})
    return false;

  /*
   * Exercise the actual public leap_second representation that tzdb will
   * expose in the next slice.
   */
  const auto first =
      tested::chrono::chrono_detail::leap_second_factory::make(0);

  const auto latest =
      tested::chrono::chrono_detail::leap_second_factory::make(last);

  if (first.date() != sys_seconds{seconds{78796800LL}})
    return false;

  if (first.value() != seconds{1})
    return false;

  if (latest.date() != sys_seconds{seconds{1483228800LL}})
    return false;

  if (!(first < latest))
    return false;

  if (!(latest == sys_seconds{seconds{1483228800LL}}))
    return false;

  if (!(first < sys_seconds{seconds{1483228800LL}}))
    return false;

  return true;
}

bool chrono_tzdb_public_works() {
  namespace runtime = tested::detail::tzdb_runtime;

  const tzdb &database = get_tzdb();

  /*
   * get_tzdb() exposes one stable initialized database.
   */
  if (&get_tzdb() != &database)
    return false;

  if (database.version != "2026c")
    return false;

  if (database.zones.size() != runtime::zone_count())
    return false;

  if (database.links.size() != runtime::link_count())
    return false;

  if (database.leap_seconds.size() != runtime::leap_count())
    return false;

  /*
   * N4950 requires every vector in tzdb to be sorted.
   */
  for (tested::size_t index = 1; index < database.zones.size(); ++index) {
    if (!(database.zones[index - 1].name() < database.zones[index].name())) {
      return false;
    }
  }

  for (tested::size_t index = 1; index < database.links.size(); ++index) {
    if (!(database.links[index - 1].name() < database.links[index].name())) {
      return false;
    }
  }

  for (tested::size_t index = 1; index < database.leap_seconds.size();
       ++index) {
    if (!(database.leap_seconds[index - 1] < database.leap_seconds[index])) {
      return false;
    }
  }

  /*
   * Canonical lookup returns a pointer into tzdb::zones.
   */
  const auto *new_york = database.locate_zone("America/New_York");

  if (new_york == nullptr)
    return false;

  if (new_york->name() != "America/New_York")
    return false;

  bool new_york_is_database_element = false;

  for (const auto &zone : database.zones) {
    if (&zone == new_york) {
      new_york_is_database_element = true;
      break;
    }
  }

  if (!new_york_is_database_element)
    return false;

  /*
   * Public namespace lookup delegates to the same tzdb object.
   */
  if (locate_zone("America/New_York") != new_york)
    return false;

  /*
   * Find the actual US/Eastern time_zone_link object.
   */
  const time_zone_link *eastern_link = nullptr;

  for (const auto &link : database.links) {
    if (link.name() == "US/Eastern") {
      eastern_link = &link;
      break;
    }
  }

  if (eastern_link == nullptr)
    return false;

  if (eastern_link->name() != "US/Eastern")
    return false;

  if (eastern_link->target() != "America/New_York")
    return false;

  /*
   * An alias resolves to the canonical object in tzdb::zones.
   */
  const auto *eastern = database.locate_zone("US/Eastern");

  if (eastern != new_york)
    return false;

  if (locate_zone("US/Eastern") != new_york)
    return false;

  /*
   * The leap_second objects exposed by tzdb are the generated IANA data.
   */
  if (database.leap_seconds.empty())
    return false;

  if (database.leap_seconds.front().date() != sys_seconds{seconds{78796800LL}})
    return false;

  if (database.leap_seconds.front().value() != seconds{1})
    return false;

  if (database.leap_seconds.back().date() != sys_seconds{seconds{1483228800LL}})
    return false;

  /*
   * Unknown names fail through tzdb::locate_zone as well as the namespace
   * convenience function.
   */
  try {
    (void)database.locate_zone("Definitely/Not_A_Zone");
    return false;
  } catch (const tested::runtime_error &) {
  } catch (...) {
    return false;
  }

  return true;
}

bool chrono_tzdb_list_works() {
  tzdb_list &list = get_tzdb_list();

  /*
   * There is one unique process-wide tzdb_list object.
   */
  if (&get_tzdb_list() != &list)
    return false;

  /*
   * Initial database construction produces exactly one element.
   */
  auto first = list.begin();

  if (first == list.end())
    return false;

  if (&*first != &list.front())
    return false;

  if (&*first != &get_tzdb())
    return false;

  auto after_first = first;
  ++after_first;

  if (after_first != list.end())
    return false;

  /*
   * const traversal exposes the same singleton contents.
   */
  const tzdb_list &const_list = list;

  if (const_list.cbegin() == const_list.cend())
    return false;

  if (&*const_list.cbegin() != &get_tzdb())
    return false;

  /*
   * Existing public accessors are now rooted in tzdb_list::front().
   */
  const auto *new_york = locate_zone("America/New_York");

  if (new_york == nullptr)
    return false;

  if (new_york != get_tzdb().locate_zone("America/New_York"))
    return false;

  if (get_tzdb().version != "2026c")
    return false;

  return true;
}

bool chrono_tzdb_remote_works() {
  tzdb_list &list = get_tzdb_list();

  /*
   * FTL's deterministic remote database is currently the same vendored
   * snapshot used to initialize the local database.
   */
  if (remote_version() != "2026c")
    return false;

  if (remote_version() != get_tzdb().version)
    return false;

  /*
   * Capture every identity that reload_tzdb() must preserve when the
   * versions are already equal.
   */
  const tzdb *const database_before = &get_tzdb();

  const time_zone *const new_york_before = locate_zone("America/New_York");

  const auto begin_before = list.begin();

  auto after_before = begin_before;
  ++after_before;

  /*
   * With equal local/remote versions reload_tzdb() has no effects.
   */
  const tzdb &reloaded = reload_tzdb();

  if (&reloaded != database_before)
    return false;

  if (&get_tzdb() != database_before)
    return false;

  if (&list.front() != database_before)
    return false;

  /*
   * Existing pointers into the database remain valid.
   */
  if (locate_zone("America/New_York") != new_york_before)
    return false;

  if (new_york_before->name() != "America/New_York")
    return false;

  /*
   * No second database was inserted.
   */
  const auto begin_after = list.begin();

  if (begin_after != begin_before)
    return false;

  auto after_after = begin_after;
  ++after_after;

  if (after_after != list.end())
    return false;

  if (after_before != list.end())
    return false;

  /*
   * Repeated reloads remain idempotent while the remote version does not
   * change.
   */
  if (&reload_tzdb() != database_before)
    return false;

  return true;
}

bool chrono_current_zone_works() {
  namespace runtime = tested::detail::tzdb_runtime;

  /*
   * Validate a couple of generated CLDR mappings
   * regardless of which OS this test is running on.
   */
  const char *eastern = runtime::windows_zone_target("Eastern Standard Time");

  if (eastern == nullptr)
    return false;

  if (tested::string_view{eastern} != "America/New_York")
    return false;

  const char *utc = runtime::windows_zone_target("UTC");

  if (utc == nullptr)
    return false;

  if (tested::string_view{utc} != "Etc/UTC")
    return false;

  /*
   * Integration test against the CI host's actual
   * configured local zone.
   *
   * Windows exercises GetDynamicTimeZoneInformation
   * + CLDR.
   *
   * Linux exercises the zoneinfo/file paths.
   *
   * Darwin exercises the zoneinfo fast path or the
   * CoreFoundation fallback.
   */
  const time_zone *const zone = current_zone();

  if (zone == nullptr)
    return false;

  if (get_tzdb().current_zone() != zone)
    return false;

  /*
   * Whatever the platform reports must resolve back
   * to the same canonical object in this database.
   */
  if (locate_zone(zone->name()) != zone)
    return false;

  return true;
}

bool chrono_tzdb_raw_rules_work() {
  namespace runtime = tested::detail::tzdb_runtime;

  if (!runtime::valid())
    return false;

  if (runtime::rule_count() == 0)
    return false;

  if (runtime::rule_set_count() == 0)
    return false;

  if (runtime::era_count() == 0)
    return false;

  const auto new_york = runtime::locate_zone("America/New_York");

  if (!new_york)
    return false;

  /*
   * The vendored 2026c source has several historical
   * America/New_York eras and ends with:
   *
   *   -5:00 US E%sT
   *
   * with no UNTIL boundary.
   */
  if (runtime::zone_era_count(new_york) < 2) {
    return false;
  }

  const auto first = runtime::zone_era_at(new_york, 0);

  if (!first)
    return false;

  if (!first.has_until)
    return false;

  const auto final = runtime::zone_final_era(new_york);

  if (!final)
    return false;

  if (final.has_until)
    return false;

  if (final.standard_offset_seconds != -5 * 60 * 60) {
    return false;
  }

  if (final.rules != runtime::rules_kind::named) {
    return false;
  }

  if (final.rule_set == runtime::invalid_index) {
    return false;
  }

  if (tested::string_view{final.format} != "E%sT") {
    return false;
  }

  const auto us = runtime::rule_set_at(final.rule_set);

  if (!us)
    return false;

  if (tested::string_view{us.name} != "US") {
    return false;
  }

  if (us.rule_count == 0)
    return false;

  bool saw_open_ended_rule = false;
  bool saw_positive_save = false;
  bool saw_zero_save = false;

  for (unsigned index = 0; index < us.rule_count; ++index) {
    const auto rule = runtime::rule_set_rule_at(final.rule_set, index);

    if (!rule)
      return false;

    if (rule.month < 1 || rule.month > 12) {
      return false;
    }

    if (rule.on.weekday < 0 || rule.on.weekday > 6) {
      return false;
    }

    if (rule.letters == nullptr)
      return false;

    if (rule.to_max)
      saw_open_ended_rule = true;

    if (rule.save_seconds > 0)
      saw_positive_save = true;

    if (rule.save_seconds == 0)
      saw_zero_save = true;
  }

  /*
   * US rules exercise exactly what the tail evaluator
   * needs: recurring max-year rules and transitions
   * both into and out of daylight saving time.
   */
  if (!saw_open_ended_rule)
    return false;

  if (!saw_positive_save)
    return false;

  if (!saw_zero_save)
    return false;

  return true;
}

bool chrono_tzdb_tail_works() {
  namespace runtime = tested::detail::tzdb_runtime;

  const time_zone *const new_york = locate_zone("America/New_York");

  if (new_york == nullptr)
    return false;

  /*
   * The generated transition table stops at 2501-01-01,
   * but that boundary is only an implementation detail.
   *
   * The state active immediately before the horizon must
   * expose its actual next transition.
   */
  const auto before_horizon = sys_days{year{2500} / December / 31} + hours{12};

  const auto horizon_info = new_york->get_info(before_horizon);

  if (horizon_info.end == sys_seconds{
                              seconds{
                                  runtime::precomputed_until(),
                              },
                          }) {
    return false;
  }

  /*
   * Ordinary post-horizon winter state.
   */
  const auto winter_2600 =
      new_york->get_info(sys_days{year{2600} / January / 15} + hours{12});

  if (winter_2600.offset != hours{-5})
    return false;

  if (winter_2600.save != minutes{0})
    return false;

  if (winter_2600.abbrev != "EST")
    return false;

  /*
   * Ordinary post-horizon daylight state.
   */
  const auto summer_2600 =
      new_york->get_info(sys_days{year{2600} / July / 15} + hours{12});

  if (summer_2600.offset != hours{-4})
    return false;

  if (summer_2600.save != minutes{60})
    return false;

  if (summer_2600.abbrev != "EDT")
    return false;

  /*
   * 2600-03-09 is the second Sunday in March.
   *
   * US daylight time begins at 02:00 standard time,
   * which is 07:00 UTC in New York.
   */
  const auto spring = sys_days{year{2600} / March / 9} + hours{7};

  const auto spring_before = new_york->get_info(spring - seconds{1});

  const auto spring_at = new_york->get_info(spring);

  if (spring_before.offset != hours{-5})
    return false;

  if (spring_at.offset != hours{-4})
    return false;

  if (spring_before.end != floor<seconds>(spring)) {
    return false;
  }

  if (spring_at.begin != floor<seconds>(spring)) {
    return false;
  }

  /*
   * 2600-11-02 is the first Sunday in November.
   *
   * The rollback occurs at 02:00 daylight time,
   * which is 06:00 UTC.
   */
  const auto autumn = sys_days{year{2600} / November / 2} + hours{6};

  const auto autumn_before = new_york->get_info(autumn - seconds{1});

  const auto autumn_at = new_york->get_info(autumn);

  if (autumn_before.offset != hours{-4})
    return false;

  if (autumn_at.offset != hours{-5})
    return false;

  /*
   * Future spring-forward gap.
   */
  const auto gap = local_days{year{2600} / March / 9} + hours{2} + minutes{30};

  const auto gap_info = new_york->get_info(gap);

  if (gap_info.result != local_info::nonexistent) {
    return false;
  }

  if (gap_info.first.offset != hours{-5})
    return false;

  if (gap_info.second.offset != hours{-4})
    return false;

  /*
   * Future fall-back overlap.
   */
  const auto overlap =
      local_days{year{2600} / November / 2} + hours{1} + minutes{30};

  const auto overlap_info = new_york->get_info(overlap);

  if (overlap_info.result != local_info::ambiguous) {
    return false;
  }

  if (overlap_info.first.offset != hours{-4})
    return false;

  if (overlap_info.second.offset != hours{-5})
    return false;

  /*
   * Southern-hemisphere recurrence must carry daylight
   * state across the calendar-year boundary.
   */
  const time_zone *const sydney = locate_zone("Australia/Sydney");

  if (sydney == nullptr)
    return false;

  const auto sydney_january =
      sydney->get_info(sys_days{year{2600} / January / 15} + hours{12});

  if (sydney_january.offset != hours{11})
    return false;

  const auto sydney_july =
      sydney->get_info(sys_days{year{2600} / July / 15} + hours{12});

  if (sydney_july.offset != hours{10})
    return false;

  /*
   * Exercise the 400-year Gregorian cycle reduction
   * rather than merely replaying another century.
   */
  const auto winter_30000 =
      new_york->get_info(sys_days{year{30000} / January / 15} + hours{12});

  if (winter_30000.offset != hours{-5})
    return false;

  const auto summer_30000 =
      new_york->get_info(sys_days{year{30000} / July / 15} + hours{12});

  if (summer_30000.offset != hours{-4})
    return false;

  /*
   * A constant final era extends indefinitely too.
   */
  const time_zone *const utc = locate_zone("Etc/UTC");

  if (utc == nullptr)
    return false;

  const auto utc_future = utc->get_info(sys_days{year{30000} / June / 1});

  if (utc_future.offset != seconds{0})
    return false;

  if (utc_future.save != minutes{0})
    return false;

  return true;
}

bool chrono_utc_subsecond_leap_works() {
  /*
   * The final generated positive leap is inserted
   * at 2017-01-01 00:00:00 sys time.
   *
   * Before insertion UTC has accumulated 26 seconds,
   * so the inserted UTC second is:
   *
   *   [1483228826, 1483228827)
   */
  const utc_time<milliseconds> leap_begin{
      milliseconds{
          1483228826000LL,
      },
  };

  const utc_time<milliseconds> leap_middle{
      milliseconds{
          1483228826500LL,
      },
  };

  const utc_time<milliseconds> leap_end{
      milliseconds{
          1483228827000LL,
      },
  };

  /*
   * Every point inside the inserted UTC second maps
   * to the last representable millisecond before the
   * system-clock insertion boundary.
   */
  const auto expected_last = sys_time<milliseconds>{
      milliseconds{
          1483228799999LL,
      },
  };

  if (utc_clock::to_sys(leap_begin) != expected_last) {
    return false;
  }

  if (utc_clock::to_sys(leap_middle) != expected_last) {
    return false;
  }

  /*
   * The first UTC tick after the leap maps normally
   * onto the insertion boundary itself.
   */
  if (utc_clock::to_sys(leap_end) != sys_time<milliseconds>{
                                         milliseconds{
                                             1483228800000LL,
                                         },
                                     }) {
    return false;
  }

  /*
   * from_sys preserves the discontinuity.
   */
  if (utc_clock::from_sys(expected_last) != utc_time<milliseconds>{
                                                milliseconds{
                                                    1483228825999LL,
                                                },
                                            }) {
    return false;
  }

  if (utc_clock::from_sys(sys_time<milliseconds>{
          milliseconds{
              1483228800000LL,
          },
      }) != leap_end) {
    return false;
  }

  return true;
}

bool chrono_zoned_time_works() {
  const time_zone *const utc = locate_zone("UTC");

  const time_zone *const new_york = locate_zone("America/New_York");

  if (utc == nullptr || new_york == nullptr) {
    return false;
  }

  /*
   * Default construction uses zoned_traits'
   * default zone.
   */
  zoned_time default_value;

  if (default_value.get_time_zone() != utc) {
    return false;
  }

  if (default_value.get_sys_time() != sys_seconds{}) {
    return false;
  }

  /*
   * Construct from sys_time with the default zone.
   */
  const sys_seconds source{
      seconds{
          1719849600LL,
      },
  };

  zoned_time default_sys{
      source,
  };

  if (default_sys.get_time_zone() != utc) {
    return false;
  }

  if (default_sys.get_sys_time() != source) {
    return false;
  }

  /*
   * Explicit zone + sys_time.
   */
  zoned_time new_york_sys{
      new_york,
      source,
  };

  if (new_york_sys.get_time_zone() != new_york) {
    return false;
  }

  if (new_york_sys.get_sys_time() != source) {
    return false;
  }

  if (new_york_sys.get_local_time() != local_seconds{
                                           seconds{
                                               1719835200LL,
                                           },
                                       }) {
    return false;
  }

  const auto summer_info = new_york_sys.get_info();

  if (summer_info.offset != hours{-4}) {
    return false;
  }

  if (summer_info.save != minutes{60}) {
    return false;
  }

  if (summer_info.abbrev != "EDT") {
    return false;
  }

  /*
   * Name-based construction.
   */
  zoned_time named{
      "America/New_York",
      source,
  };

  if (named.get_time_zone() != new_york) {
    return false;
  }

  if (!(named == new_york_sys))
    return false;

  /*
   * Unique local-time construction.
   */
  const local_seconds summer_local{
      seconds{
          1719835200LL,
      },
  };

  zoned_time from_local{
      new_york,
      summer_local,
  };

  if (from_local.get_sys_time() != source) {
    return false;
  }

  /*
   * Conversion operators.
   */
  if (static_cast<sys_seconds>(from_local) != source) {
    return false;
  }

  if (static_cast<local_seconds>(from_local) != summer_local) {
    return false;
  }

  /*
   * Assignment preserves the zone.
   */
  zoned_time assigned{
      new_york,
  };

  assigned = source;

  if (assigned.get_time_zone() != new_york) {
    return false;
  }

  if (assigned.get_sys_time() != source) {
    return false;
  }

  assigned = summer_local;

  if (assigned.get_time_zone() != new_york) {
    return false;
  }

  if (assigned.get_sys_time() != source) {
    return false;
  }

  /*
   * Nonexistent local time throws without choose.
   */
  const local_seconds gap{
      seconds{
          1710037800LL,
      },
  };

  try {
    zoned_time invalid{
        new_york,
        gap,
    };

    (void)invalid;
    return false;
  } catch (const nonexistent_local_time &) {
  } catch (...) {
    return false;
  }

  /*
   * choose collapses the gap onto the transition.
   */
  zoned_time gap_earliest{
      new_york,
      gap,
      choose::earliest,
  };

  zoned_time gap_latest{
      new_york,
      gap,
      choose::latest,
  };

  if (gap_earliest.get_sys_time() != sys_seconds{
                                         seconds{
                                             1710054000LL,
                                         },
                                     }) {
    return false;
  }

  if (gap_latest.get_sys_time() != gap_earliest.get_sys_time()) {
    return false;
  }

  /*
   * Ambiguous local time selects distinct system
   * interpretations.
   */
  const local_seconds overlap{
      seconds{
          1730597400LL,
      },
  };

  try {
    zoned_time invalid{
        new_york,
        overlap,
    };

    (void)invalid;
    return false;
  } catch (const ambiguous_local_time &) {
  } catch (...) {
    return false;
  }

  zoned_time overlap_earliest{
      new_york,
      overlap,
      choose::earliest,
  };

  zoned_time overlap_latest{
      new_york,
      overlap,
      choose::latest,
  };

  if (overlap_earliest.get_sys_time() != sys_seconds{
                                             seconds{
                                                 1730611800LL,
                                             },
                                         }) {
    return false;
  }

  if (overlap_latest.get_sys_time() != sys_seconds{
                                           seconds{
                                               1730615400LL,
                                           },
                                       }) {
    return false;
  }

  /*
   * Re-zoning a zoned_time preserves its system
   * time. choose has no effect for this constructor.
   */
  zoned_time rezoned{
      utc,
      overlap_earliest,
  };

  if (rezoned.get_time_zone() != utc) {
    return false;
  }

  if (rezoned.get_sys_time() != overlap_earliest.get_sys_time()) {
    return false;
  }

  zoned_time rezoned_choose{
      utc,
      overlap_earliest,
      choose::latest,
  };

  if (rezoned_choose.get_sys_time() != rezoned.get_sys_time()) {
    return false;
  }

  /*
   * Future tail integration through zoned_time.
   */
  zoned_time future{
      "America/New_York",
      sys_days{year{2600} / July / 15} + hours{12},
  };

  if (future.get_info().offset != hours{-4}) {
    return false;
  }

  if (future.get_info().save != minutes{60}) {
    return false;
  }

  /*
   * CTAD.
   */
  static_assert(tested::is_same_v<decltype(zoned_time{
                                      sys_seconds{},
                                  }),
                                  zoned_time<seconds>>);

  static_assert(tested::is_same_v<decltype(zoned_time{
                                      "America/New_York",
                                      sys_seconds{},
                                  }),
                                  zoned_time<seconds, const time_zone *>>);

  static_assert(tested::is_same_v<decltype(zoned_time{
                                      new_york,
                                      sys_seconds{},
                                  }),
                                  zoned_time<seconds, const time_zone *>>);

  return true;
}

bool chrono_zoned_formatting_works() {
  /*
   * local_time_format supplies time-zone metadata
   * independently of a time_zone object.
   */
  const local_seconds local{
      seconds{
          1719835200LL,
      },
  };

  const tested::string abbreviation{
      "EDT",
  };

  const seconds offset{
      -4 * 60 * 60,
  };

  const auto decorated = local_time_format(local, &abbreviation, &offset);

  /*
   * Its default chrono-spec is:
   *
   *   %F %T %Z
   */
  if (tested::format("{}", decorated) != "2024-07-01 12:00:00 EDT") {
    return false;
  }

  if (tested::format("{:%F %T %z %Z}", decorated) !=
      "2024-07-01 12:00:00 -0400 EDT") {
    return false;
  }

  if (tested::format("{:%F %T %Ez %Z}", decorated) !=
      "2024-07-01 12:00:00 -04:00 EDT") {
    return false;
  }

  /*
   * Metadata is optional unless the format string
   * actually asks for it.
   */
  const auto undecorated = local_time_format(local);

  if (tested::format("{:%F %T}", undecorated) != "2024-07-01 12:00:00") {
    return false;
  }

  try {
    (void)tested::format("{:%Z}", undecorated);

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  try {
    (void)tested::format("{:%z}", undecorated);

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  /*
   * Bare local_time still has no zone metadata.
   */
  try {
    (void)tested::format("{:%Z}", local);

    return false;
  } catch (const tested::format_error &) {
  } catch (...) {
    return false;
  }

  /*
   * zoned_time automatically supplies the metadata
   * from its associated sys_info.
   */
  zoned_time new_york{
      "America/New_York",
      sys_seconds{
          seconds{
              1719849600LL,
          },
      },
  };

  if (tested::format("{}", new_york) != "2024-07-01 12:00:00 EDT") {
    return false;
  }

  if (tested::format("{:%F %T %Z %z}", new_york) !=
      "2024-07-01 12:00:00 EDT -0400") {
    return false;
  }

  if (tested::format("{:%F %T %Z %Ez}", new_york) !=
      "2024-07-01 12:00:00 EDT -04:00") {
    return false;
  }

  /*
   * Winter exercises the other side of the same
   * zone's rules.
   */
  zoned_time winter{
      "America/New_York",
      sys_days{year{2024} / January / 15} + hours{12},
  };

  if (tested::format("{:%F %T %Z %z}", winter) !=
      "2024-01-15 07:00:00 EST -0500") {
    return false;
  }

  /*
   * And, importantly, the formatter consumes the
   * post-horizon tail through ordinary zoned_time
   * get_info(), not a separate formatting path.
   */
  zoned_time future{
      "America/New_York",
      sys_days{year{2600} / July / 15} + hours{12},
  };

  if (tested::format("{:%F %T %Z %z}", future) !=
      "2600-07-15 08:00:00 EDT -0400") {
    return false;
  }

  /*
   * Fixed UTC zone.
   */
  zoned_time utc{
      "Etc/UTC",
      sys_days{year{2600} / July / 15} + hours{12},
  };

  if (tested::format("{}", utc) != "2600-07-15 12:00:00 UTC") {
    return false;
  }

  if (tested::format("{:%F %T %Z %z}", utc) !=
      "2600-07-15 12:00:00 UTC +0000") {
    return false;
  }

  return true;
}

bool chrono_stream_io_works() {
  /*
   * Duration insertion follows the specified
   * count + units-suffix representation.
   */
  {
    tested::ostringstream stream;

    stream << milliseconds{1500};

    if (stream.str() != "1500ms")
      return false;
  }

  /*
   * hh_mm_ss.
   */
  {
    tested::ostringstream stream;

    stream << hh_mm_ss<milliseconds>{
        milliseconds{
            4083007,
        },
    };

    if (stream.str() != "01:08:03.007") {
      return false;
    }
  }

  /*
   * sys_time and local_time.
   */
  {
    tested::ostringstream stream;

    stream << sys_seconds{
        seconds{0},
    };

    if (stream.str() != "1970-01-01 00:00:00") {
      return false;
    }
  }

  {
    tested::ostringstream stream;

    stream << local_seconds{
        seconds{0},
    };

    if (stream.str() != "1970-01-01 00:00:00") {
      return false;
    }
  }

  /*
   * Fractional system time.
   */
  {
    tested::ostringstream stream;

    stream << sys_time<milliseconds>{
        milliseconds{
            1234,
        },
    };

    if (stream.str() != "1970-01-01 00:00:01.234") {
      return false;
    }
  }

  /*
   * Positive UTC leap-second insertion.
   */
  {
    tested::ostringstream stream;

    stream << utc_time<milliseconds>{
        milliseconds{
            1483228826500LL,
        },
    };

    if (stream.str() != "2016-12-31 23:59:60.500") {
      return false;
    }
  }

  /*
   * Calendar stream forms.
   */
  {
    tested::ostringstream stream;

    stream << year{2024} / July / 1;

    if (stream.str() != "2024-07-01") {
      return false;
    }
  }

  {
    tested::ostringstream stream;

    stream << July / day{1};

    if (stream.str() != "Jul/01") {
      return false;
    }
  }

  {
    tested::ostringstream stream;

    stream << Monday;

    if (stream.str() != "Mon")
      return false;
  }

  /*
   * zoned_time uses its local wall clock plus
   * the active abbreviation.
   */
  {
    tested::ostringstream stream;

    const zoned_time value{
        "America/New_York",
        sys_seconds{
            seconds{
                1719849600LL,
            },
        },
    };

    stream << value;

    if (stream.str() != "2024-07-01 12:00:00 EDT") {
      return false;
    }
  }

  /*
   * Direct sys_time from_stream.
   */
  {
    tested::istringstream stream{
        "2024-07-01 12:34:56 -0400 EDT",
    };

    sys_seconds parsed{};

    tested::string abbrev;

    minutes offset{};

    from_stream(stream, "%F %T %z %Z", parsed, &abbrev, &offset);

    if (stream.fail())
      return false;

    const auto expected =
        sys_days{year{2024} / July / 1} + hours{16} + minutes{34} + seconds{56};

    if (parsed != expected)
      return false;

    if (abbrev != "EDT")
      return false;

    if (offset != minutes{-240}) {
      return false;
    }
  }

  /*
   * Colonized offset.
   */
  {
    tested::istringstream stream{
        "2024-07-01 12:00:00 -04:30",
    };

    sys_seconds parsed{};

    minutes offset{};

    from_stream(stream, "%F %T %Ez", parsed,
                static_cast<tested::string *>(nullptr), &offset);

    if (stream.fail())
      return false;

    if (offset != minutes{-270}) {
      return false;
    }

    if (parsed != sys_days{year{2024} / July / 1} + hours{16} + minutes{30}) {
      return false;
    }
  }

  /*
   * local_time captures an offset but does not
   * apply it to the local clock value.
   */
  {
    tested::istringstream stream{
        "2024-07-01 12:34:56 -0400",
    };

    local_seconds parsed{};

    minutes offset{};

    from_stream(stream, "%F %T %z", parsed,
                static_cast<tested::string *>(nullptr), &offset);

    if (stream.fail())
      return false;

    if (parsed != local_days{year{2024} / July / 1} + hours{12} + minutes{34} +
                      seconds{56}) {
      return false;
    }

    if (offset != minutes{-240}) {
      return false;
    }
  }

  /*
   * Calendar target.
   */
  {
    tested::istringstream stream{
        "2024-02-29",
    };

    year_month_day parsed{
        year{2000},
        January,
        day{1},
    };

    from_stream(stream, "%F", parsed);

    if (stream.fail())
      return false;

    if (parsed != year{2024} / February / day{29}) {
      return false;
    }
  }

  /*
   * Localized month/weekday names in the classic
   * locale.
   */
  {
    tested::istringstream stream{
        "Jul Monday",
    };

    month parsed_month{
        January,
    };

    weekday parsed_weekday{
        Sunday,
    };

    from_stream(stream, "%b", parsed_month);

    if (stream.fail())
      return false;

    stream >> tested::ws;

    from_stream(stream, "%A", parsed_weekday);

    if (stream.fail())
      return false;

    if (parsed_month != July)
      return false;

    if (parsed_weekday != Monday)
      return false;
  }

  /*
   * Duration parsing uses time-of-day fields.
   */
  {
    tested::istringstream stream{
        "01:02:03.500",
    };

    milliseconds parsed{};

    from_stream(stream, "%T", parsed);

    if (stream.fail())
      return false;

    if (parsed != milliseconds{
                      3723500,
                  }) {
      return false;
    }
  }

  /*
   * parse() manipulator.
   */
  {
    tested::istringstream stream{
        "2024-07-01 12:34:56",
    };

    sys_seconds parsed{};

    stream >> parse("%F %T", parsed);

    if (stream.fail())
      return false;

    if (parsed != sys_days{year{2024} / July / 1} + hours{12} + minutes{34} +
                      seconds{56}) {
      return false;
    }
  }

  /*
   * parse() with both auxiliary outputs.
   */
  {
    tested::istringstream stream{
        "2024-07-01 12:00:00 EDT -0400",
    };

    sys_seconds parsed{};

    tested::string abbrev;

    minutes offset{};

    stream >> parse("%F %T %Z %z", parsed, abbrev, offset);

    if (stream.fail())
      return false;

    if (abbrev != "EDT")
      return false;

    if (offset != minutes{-240}) {
      return false;
    }
  }

  /*
   * Failure must not modify the destination.
   */
  {
    tested::istringstream stream{
        "2024-02-31",
    };

    year_month_day parsed{
        year{1999},
        December,
        day{31},
    };

    from_stream(stream, "%F", parsed);

    if (!stream.fail())
      return false;

    if (parsed != year{1999} / December / day{31}) {
      return false;
    }
  }

  /*
   * Conflicting redundant fields fail.
   *
   * 2024-07-01 is Monday, not Tuesday.
   */
  {
    tested::istringstream stream{
        "2024-07-01 Tuesday",
    };

    year_month_day parsed{
        year{1999},
        December,
        day{31},
    };

    from_stream(stream, "%F %A", parsed);

    if (!stream.fail())
      return false;

    if (parsed != year{1999} / December / day{31}) {
      return false;
    }
  }

  /*
   * UTC leap second parsing.
   */
  {
    tested::istringstream stream{
        "2016-12-31 23:59:60.500",
    };

    utc_time<milliseconds> parsed{};

    from_stream(stream, "%F %T", parsed);

    if (stream.fail())
      return false;

    if (!get_leap_second_info(parsed).is_leap_second) {
      return false;
    }

    tested::ostringstream rendered;

    rendered << parsed;

    if (rendered.str() != "2016-12-31 23:59:60.500") {
      return false;
    }
  }

  /*
   * Exact nonexistent_local_time diagnostic.
   */
  {
    const auto *zone = locate_zone("America/New_York");

    if (zone == nullptr)
      return false;

    const local_seconds gap =
        local_days{year{2024} / March / 10} + hours{2} + minutes{30};

    try {
      (void)zone->to_sys(gap);

      return false;
    } catch (const nonexistent_local_time &error) {
      if (tested::string{error.what()} !=
          "2024-03-10 02:30:00 is in a gap between\n"
          "2024-03-10 02:00:00 EST and\n"
          "2024-03-10 03:00:00 EDT which are both equivalent to\n"
          "2024-03-10 07:00:00 UTC") {
        return false;
      }
    } catch (...) {
      return false;
    }
  }

  /*
   * Exact ambiguous_local_time diagnostic.
   */
  {
    const auto *zone = locate_zone("America/New_York");

    if (zone == nullptr)
      return false;

    const local_seconds overlap =
        local_days{year{2024} / November / 3} + hours{1} + minutes{30};

    try {
      (void)zone->to_sys(overlap);

      return false;
    } catch (const ambiguous_local_time &error) {
      if (tested::string{error.what()} !=
          "2024-11-03 01:30:00 is ambiguous. It could be\n"
          "2024-11-03 01:30:00 EDT == 2024-11-03 05:30:00 UTC or\n"
          "2024-11-03 01:30:00 EST == 2024-11-03 06:30:00 UTC") {
        return false;
      }
    } catch (...) {
      return false;
    }
  }

  return true;
}

bool ftl_test() {
  if (!(system_clock::now().time_since_epoch() > seconds{0})) {
    return false;
  }

  if (!(steady_clock::now().time_since_epoch() > nanoseconds{0})) {
    return false;
  }

  if (!chrono_duration_formatting_works())
    return false;

  if (!chrono_hms_formatting_works())
    return false;

  if (!chrono_format_errors_work())
    return false;

  if (!chrono_calendar_formatting_works())
    return false;

  if (!chrono_clock_formatting_works())
    return false;

  if (!chrono_tzdb_runtime_works())
    return false;

  if (!chrono_tzdb_raw_rules_work())
    return false;

  if (!chrono_tzdb_tail_works())
    return false;

  if (!chrono_zoned_time_works())
    return false;

  if (!chrono_zoned_formatting_works())
    return false;

  if (!chrono_stream_io_works())
    return false;

  if (!chrono_time_zone_public_works())
    return false;

  if (!chrono_local_info_works())
    return false;

  if (!chrono_time_zone_conversion_works())
    return false;

  if (!chrono_leap_second_works())
    return false;

  if (!chrono_utc_subsecond_leap_works())
    return false;

  if (!chrono_tzdb_public_works())
    return false;

  if (!chrono_tzdb_list_works())
    return false;

  if (!chrono_tzdb_remote_works())
    return false;

  if (!chrono_current_zone_works())
    return false;

  return true;
}
