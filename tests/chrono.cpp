#ifdef FTL_REPLACE_STL
#include <chrono>
#include <format>
namespace tested = std;
#else
#include <ftl/chrono>
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

  return true;
}
