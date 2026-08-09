// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_TIME_CORE_HEADER
#define FTL_DETAIL_TIME_CORE_HEADER

#ifdef FTL_REPLACE_STL
#include <cstdint>
#include <type_traits>
#else
#include <ftl/cstdint>
#include <ftl/type_traits>
#endif

FTL_BEGIN_NAMESPACE
namespace detail {

struct civil_date {
  int year;
  unsigned month, day;
};

constexpr long long days_from_civil(int year, unsigned month,
                                    unsigned day) noexcept {
  year -= month <= 2;
  const int era = (year >= 0 ? year : year - 399) / 400;
  const unsigned yoe = unsigned(year - era * 400);
  const unsigned doy =
      (153 * (month > 2 ? month - 3 : month + 9) + 2) / 5 + day - 1;
  const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097LL + static_cast<long long>(doe) - 719468;
}

constexpr civil_date civil_from_days(long long days) noexcept {
  days += 719468;
  const long long era = (days >= 0 ? days : days - 146096) / 146097;
  const unsigned doe = unsigned(days - era * 146097);
  const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  int year = int(yoe) + int(era * 400);
  const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  const unsigned mp = (5 * doy + 2) / 153;
  const unsigned day = doy - (153 * mp + 2) / 5 + 1;
  const unsigned month = mp < 10 ? mp + 3 : mp - 9;
  year += month <= 2;
  return {year, month, day};
}

constexpr bool is_leap(int year) noexcept {
  return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}
constexpr unsigned last_day(int year, unsigned month) noexcept {
  constexpr unsigned lengths[] = {31, 28, 31, 30, 31, 30,
                                  31, 31, 30, 31, 30, 31};
  return month == 2 ? 28 + is_leap(year) : lengths[month - 1];
}
constexpr long long floor_div(long long value, long long divisor) noexcept {
  const auto quotient = value / divisor;
  const auto remainder = value % divisor;
  return quotient - (remainder < 0);
}

} // namespace detail
FTL_END_NAMESPACE
#endif
