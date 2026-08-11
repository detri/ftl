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

  return true;
}
