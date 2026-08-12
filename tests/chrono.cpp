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

  return true;
}
