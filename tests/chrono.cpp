#ifdef FTL_REPLACE_STL
#include <chrono>
#include <detail/tzdb_runtime.hpp>
#include <format>
namespace tested = std;
#else
#include <ftl/chrono>
#include <ftl/detail/tzdb_runtime.hpp>
#include <ftl/format>
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

  if (!equal_text(tz::zone_name(new_york), "America/New_York"))
    return false;

  /*
   * backward contains:
   *
   *   Link America/New_York US/Eastern
   *
   * buildtzdb resolves links to final Zone indices, so both names must
   * identify the same generated zone.
   */
  const auto eastern_link = tz::find_link("US/Eastern");

  if (!eastern_link)
    return false;

  if (!equal_text(tz::link_name(eastern_link), "US/Eastern"))
    return false;

  const auto eastern_target = tz::link_target(eastern_link);

  if (!eastern_target || eastern_target.index != new_york.index)
    return false;

  const auto eastern = tz::locate_zone("US/Eastern");

  if (!eastern || eastern.index != new_york.index)
    return false;

  if (tz::locate_zone("Definitely/Not_A_Zone"))
    return false;

  /*
   * 2024-03-10 07:00:00 UTC is the New York spring-forward
   * transition.
   *
   * The second immediately before it must still be EST.
   */
  const auto before_spring = tz::lookup(new_york, 1710053999LL);

  if (!before_spring)
    return false;

  if (before_spring.offset_seconds != -5 * 60 * 60)
    return false;

  if (before_spring.save_minutes != 0)
    return false;

  if (!equal_text(before_spring.abbreviation, "EST"))
    return false;

  if (before_spring.end != 1710054000LL)
    return false;

  /*
   * Exact equality with transition.begin belongs to the new state.
   */
  const auto spring = tz::lookup(new_york, 1710054000LL);

  if (!spring)
    return false;

  if (spring.begin_unbounded)
    return false;

  if (spring.begin != 1710054000LL)
    return false;

  if (spring.offset_seconds != -4 * 60 * 60)
    return false;

  if (spring.save_minutes != 60)
    return false;

  if (!equal_text(spring.abbreviation, "EDT"))
    return false;

  /*
   * 2024-11-03 06:00:00 UTC is the corresponding fall-back.
   */
  if (spring.end != 1730613600LL)
    return false;

  const auto fall = tz::lookup(new_york, 1730613600LL);

  if (!fall)
    return false;

  if (fall.begin != 1730613600LL)
    return false;

  if (fall.offset_seconds != -5 * 60 * 60)
    return false;

  if (fall.save_minutes != 0)
    return false;

  if (!equal_text(fall.abbreviation, "EST"))
    return false;

  /*
   * A no-DST zone exercises the generated initial-state path.
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

  if (!equal_text(utc_epoch.abbreviation, "UTC"))
    return false;

  /*
   * The current runtime slice must never pretend that the generated
   * transition table knows anything past its coverage horizon.
   */
  const auto last_precomputed =
      tz::lookup(new_york, tz::precomputed_until() - 1);

  if (!last_precomputed)
    return false;

  if (!last_precomputed.end_is_horizon)
    return false;

  if (last_precomputed.end != tz::precomputed_until())
    return false;

  if (tz::lookup(new_york, tz::precomputed_until()))
    return false;

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

  if (new_york->name() != "America/New_York")
    return false;

  /*
   * A string_view is not required to be null-terminated.
   * Make sure public lookup does not accidentally fall back to C-string
   * semantics.
   */
  const char sliced_name[] = "America/New_York!";

  const time_zone *sliced = nullptr;

  try {
    sliced = locate_zone(tested::string_view{sliced_name, 16});
  } catch (...) {
    return false;
  }

  if (sliced != new_york)
    return false;

  /*
   * backward contains:
   *
   *   Link America/New_York US/Eastern
   *
   * Public lookup of the Link must return the same canonical time_zone
   * object, not merely an equivalent object.
   */
  const time_zone *eastern = nullptr;

  try {
    eastern = locate_zone("US/Eastern");
  } catch (...) {
    return false;
  }

  if (eastern != new_york)
    return false;

  if (eastern->name() != "America/New_York")
    return false;

  /*
   * 2024-03-10 07:00:00 UTC is New York's spring-forward transition.
   */
  const auto before_spring =
      new_york->get_info(sys_seconds{seconds{1710053999LL}});

  if (before_spring.offset != hours{-5})
    return false;

  if (before_spring.save != minutes{0})
    return false;

  if (!equal_text(before_spring.abbrev.c_str(), "EST"))
    return false;

  if (before_spring.end != sys_seconds{seconds{1710054000LL}})
    return false;

  /*
   * Exact transition equality belongs to the new state.
   */
  const auto spring = new_york->get_info(sys_seconds{seconds{1710054000LL}});

  if (spring.begin != sys_seconds{seconds{1710054000LL}})
    return false;

  if (spring.end != sys_seconds{seconds{1730613600LL}})
    return false;

  if (spring.offset != hours{-4})
    return false;

  if (spring.save != minutes{60})
    return false;

  if (!equal_text(spring.abbrev.c_str(), "EDT"))
    return false;

  /*
   * Subsecond timestamps immediately before the transition still belong
   * to the old state. This specifically catches truncation-vs-floor bugs
   * in the sys_time -> generated-second bridge.
   */
  const auto subsecond_before =
      new_york->get_info(sys_time<milliseconds>{milliseconds{1710053999999LL}});

  if (subsecond_before.offset != hours{-5})
    return false;

  if (!equal_text(subsecond_before.abbrev.c_str(), "EST"))
    return false;

  const auto subsecond_after =
      new_york->get_info(sys_time<milliseconds>{milliseconds{1710054000001LL}});

  if (subsecond_after.offset != hours{-4})
    return false;

  if (!equal_text(subsecond_after.abbrev.c_str(), "EDT"))
    return false;

  /*
   * A fixed zone exercises the generated initial-state -> sys_info path.
   */
  const time_zone *utc = nullptr;

  try {
    utc = locate_zone("Etc/UTC");
  } catch (...) {
    return false;
  }

  if (utc == nullptr)
    return false;

  const auto utc_epoch = utc->get_info(sys_seconds{seconds{0}});

  if (utc_epoch.offset != seconds{0})
    return false;

  if (utc_epoch.save != minutes{0})
    return false;

  if (!equal_text(utc_epoch.abbrev.c_str(), "UTC"))
    return false;

  /*
   * Unknown names are required to fail rather than produce a null public
   * time_zone pointer.
   */
  try {
    (void)locate_zone("Definitely/Not_A_Zone");
    return false;
  } catch (const tested::runtime_error &) {
  } catch (...) {
    return false;
  }

  /*
   * Temporary FTL limitation until arbitrary-year rule evaluation lands:
   * never silently claim generated coverage beyond the precomputed horizon.
   */
  try {
    (void)new_york->get_info(sys_seconds{
        seconds{tested::detail::tzdb_runtime::precomputed_until()}});
    return false;
  } catch (const tested::runtime_error &) {
  } catch (...) {
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

  if (!chrono_time_zone_public_works())
    return false;

  if (!chrono_local_info_works())
    return false;

  if (!chrono_time_zone_conversion_works())
    return false;

  if (!chrono_leap_second_works())
    return false;

  if (!chrono_tzdb_public_works())
    return false;

  if (!chrono_tzdb_list_works())
    return false;

  if (!chrono_tzdb_remote_works())
    return false;

  return true;
}
using