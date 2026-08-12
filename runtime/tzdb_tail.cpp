#include "tzdb_tail.hpp"

#include <cstdint>
#include <cstring>
#include <limits>

namespace ftl_tzdb_tail {
namespace {

namespace runtime = ::ftl_tzdb_runtime_core;

constexpr long long seconds_per_day = 86400LL;
constexpr long long days_per_cycle = 146097LL;
constexpr long long seconds_per_cycle = days_per_cycle * seconds_per_day;

constexpr unsigned max_pending_rules = 64;
constexpr unsigned abbreviation_buffer_count = 64;
constexpr unsigned abbreviation_buffer_size = 64;

thread_local char abbreviation_buffers[abbreviation_buffer_count]
                                      [abbreviation_buffer_size]{};

thread_local unsigned abbreviation_buffer_index = 0;

struct civil_date {
  long long year = 1970;
  unsigned month = 1;
  unsigned day = 1;
};

struct tail_state {
  int save_seconds = 0;
  int offset_seconds = 0;
  int save_minutes = 0;

  bool daylight = false;

  const char *letters = nullptr;

  /*
   * The precomputed seed already owns a resolved
   * abbreviation in the blob.
   *
   * Once a tail Rule applies, resolved_abbreviation
   * becomes null and the abbreviation is produced
   * from FORMAT + LETTER/SAVE on demand.
   */
  const char *resolved_abbreviation = nullptr;
};

struct pending_rule {
  runtime::rule_definition rule;
  long long civil = 0;
  bool done = false;
};

struct rule_analysis {
  bool has_max = false;

  long long settle_year = 0;
  long long last_finite_year = 0;
};

bool checked_add(long long left, long long right, long long &result) noexcept {
  constexpr long long minimum = std::numeric_limits<long long>::min();

  constexpr long long maximum = std::numeric_limits<long long>::max();

  if (right > 0 && left > maximum - right) {
    return false;
  }

  if (right < 0 && left < minimum - right) {
    return false;
  }

  result = left + right;
  return true;
}

bool checked_multiply_days(long long days, long long &result) noexcept {
  constexpr long long minimum = std::numeric_limits<long long>::min();

  constexpr long long maximum = std::numeric_limits<long long>::max();

  if (days > maximum / seconds_per_day || days < minimum / seconds_per_day) {
    return false;
  }

  result = days * seconds_per_day;
  return true;
}

long long floor_div(long long value, long long divisor) noexcept {
  long long quotient = value / divisor;
  const long long remainder = value % divisor;

  if (remainder < 0)
    --quotient;

  return quotient;
}

/*
 * Howard Hinnant-style proleptic Gregorian conversion.
 *
 * The epoch adjustment makes the result count days
 * from 1970-01-01.
 */
long long days_from_civil(long long year, unsigned month,
                          unsigned day) noexcept {
  year -= month <= 2;

  const long long era = (year >= 0 ? year : year - 399) / 400;

  const unsigned year_of_era = static_cast<unsigned>(year - era * 400);

  const unsigned month_prime = month > 2 ? month - 3 : month + 9;

  const unsigned day_of_year = (153 * month_prime + 2) / 5 + day - 1;

  const unsigned day_of_era =
      year_of_era * 365 + year_of_era / 4 - year_of_era / 100 + day_of_year;

  return era * days_per_cycle + static_cast<long long>(day_of_era) - 719468;
}

civil_date civil_from_days(long long days) noexcept {
  days += 719468;

  const long long era = (days >= 0 ? days : days - 146096) / 146097;

  const unsigned day_of_era = static_cast<unsigned>(days - era * 146097);

  const unsigned year_of_era = (day_of_era - day_of_era / 1460 +
                                day_of_era / 36524 - day_of_era / 146096) /
                               365;

  long long year = static_cast<long long>(year_of_era) + era * 400;

  const unsigned day_of_year =
      day_of_era - (365 * year_of_era + year_of_era / 4 - year_of_era / 100);

  const unsigned month_prime = (5 * day_of_year + 2) / 153;

  const unsigned day = day_of_year - (153 * month_prime + 2) / 5 + 1;

  const unsigned month = month_prime < 10 ? month_prime + 3 : month_prime - 9;

  year += month <= 2;

  return {
      year,
      month,
      day,
  };
}

long long civil_year_from_seconds(long long timestamp) noexcept {
  const long long days = floor_div(timestamp, seconds_per_day);

  return civil_from_days(days).year;
}

int weekday_from_days(long long days) noexcept {
  long long result = (days + 4) % 7;

  if (result < 0)
    result += 7;

  return static_cast<int>(result);
}

bool civil_seconds(long long year, int month, const runtime::day_spec &spec,
                   int seconds_from_midnight, long long &result) noexcept {
  if (month < 1 || month > 12)
    return false;

  const unsigned m = static_cast<unsigned>(month);

  long long date = 0;

  switch (spec.kind) {
  case runtime::day_kind::exact:
    date = days_from_civil(year, m, 1) + static_cast<long long>(spec.day) - 1;

    break;

  case runtime::day_kind::last_weekday: {
    long long next_year = year;
    unsigned next_month = m + 1;

    if (next_month == 13) {
      next_month = 1;
      ++next_year;
    }

    const long long last = days_from_civil(next_year, next_month, 1) - 1;

    const int actual = weekday_from_days(last);

    const int delta = (actual - spec.weekday + 7) % 7;

    date = last - delta;
    break;
  }

  case runtime::day_kind::weekday_on_or_after: {
    const long long base =
        days_from_civil(year, m, 1) + static_cast<long long>(spec.day) - 1;

    const int actual = weekday_from_days(base);

    const int delta = (spec.weekday - actual + 7) % 7;

    date = base + delta;
    break;
  }

  case runtime::day_kind::weekday_on_or_before: {
    const long long base =
        days_from_civil(year, m, 1) + static_cast<long long>(spec.day) - 1;

    const int actual = weekday_from_days(base);

    const int delta = (actual - spec.weekday + 7) % 7;

    date = base - delta;
    break;
  }
  }

  long long day_seconds = 0;

  if (!checked_multiply_days(date, day_seconds)) {
    return false;
  }

  return checked_add(day_seconds, seconds_from_midnight, result);
}

bool rule_civil_time(const runtime::rule_definition &rule, long long year,
                     long long &result) noexcept {
  return civil_seconds(year, rule.month, rule.on, rule.at_seconds, result);
}

bool to_sys_time(long long civil, runtime::time_basis basis,
                 int standard_offset, int current_save,
                 long long &result) noexcept {
  switch (basis) {
  case runtime::time_basis::universal:
    result = civil;
    return true;

  case runtime::time_basis::standard:
    return checked_add(civil, -static_cast<long long>(standard_offset), result);

  case runtime::time_basis::wall: {
    long long intermediate = 0;

    if (!checked_add(civil, -static_cast<long long>(standard_offset),
                     intermediate)) {
      return false;
    }

    return checked_add(intermediate, -static_cast<long long>(current_save),
                       result);
  }
  }

  return false;
}

bool rule_applies(const runtime::rule_definition &rule,
                  long long year) noexcept {
  if (year < rule.from_year)
    return false;

  if (rule.to_max)
    return true;

  return year <= rule.to_year;
}

char *next_abbreviation_buffer() noexcept {
  char *result = abbreviation_buffers[abbreviation_buffer_index %
                                      abbreviation_buffer_count];

  ++abbreviation_buffer_index;

  result[0] = '\0';

  return result;
}

bool append_character(char *&output, char *end, char value) noexcept {
  if (output == end)
    return false;

  *output++ = value;
  return true;
}

bool append_text(char *&output, char *end, const char *first,
                 const char *last) noexcept {
  while (first != last) {
    if (!append_character(output, end, *first++)) {
      return false;
    }
  }

  return true;
}

bool append_c_string(char *&output, char *end, const char *value) noexcept {
  if (value == nullptr)
    return true;

  while (*value != '\0') {
    if (!append_character(output, end, *value++)) {
      return false;
    }
  }

  return true;
}

bool append_unsigned(char *&output, char *end,
                     unsigned long long value) noexcept {
  char temporary[32];
  unsigned count = 0;

  do {
    temporary[count++] = static_cast<char>('0' + value % 10);

    value /= 10;
  } while (value != 0);

  while (count != 0) {
    if (!append_character(output, end, temporary[--count])) {
      return false;
    }
  }

  return true;
}

bool append_two_digits(char *&output, char *end, unsigned value) noexcept {
  return append_character(output, end,
                          static_cast<char>('0' + (value / 10) % 10)) &&
         append_character(output, end, static_cast<char>('0' + value % 10));
}

bool append_numeric_offset(char *&output, char *end,
                           int offset_seconds) noexcept {
  const long long signed_value = offset_seconds;

  const bool negative = signed_value < 0;

  const unsigned long long magnitude =
      negative ? static_cast<unsigned long long>(-signed_value)
               : static_cast<unsigned long long>(signed_value);

  unsigned long long value = magnitude;

  const unsigned long long hours = value / 3600;

  value %= 3600;

  const unsigned minutes = static_cast<unsigned>(value / 60);

  const unsigned seconds = static_cast<unsigned>(value % 60);

  if (!append_character(output, end, negative ? '-' : '+')) {
    return false;
  }

  if (hours < 10) {
    if (!append_character(output, end, '0')) {
      return false;
    }
  }

  if (!append_unsigned(output, end, hours)) {
    return false;
  }

  if (minutes != 0 || seconds != 0) {
    if (!append_two_digits(output, end, minutes)) {
      return false;
    }
  }

  if (seconds != 0) {
    if (!append_two_digits(output, end, seconds)) {
      return false;
    }
  }

  return true;
}

const char *make_abbreviation(const runtime::era_definition &era,
                              const tail_state &state) noexcept {
  if (state.resolved_abbreviation != nullptr)
    return state.resolved_abbreviation;

  if (era.format == nullptr)
    return nullptr;

  const char *format_begin = era.format;

  const char *format_end = era.format + std::strlen(era.format);

  const char *slash = nullptr;

  for (const char *scan = format_begin; scan != format_end; ++scan) {
    if (*scan != '/')
      continue;

    if (slash != nullptr)
      return nullptr;

    slash = scan;
  }

  const char *selected_begin = format_begin;

  const char *selected_end = format_end;

  if (slash != nullptr) {
    if (state.daylight) {
      selected_begin = slash + 1;
    } else {
      selected_end = slash;
    }
  }

  char *buffer = next_abbreviation_buffer();

  char *output = buffer;

  char *const end = buffer + abbreviation_buffer_size - 1;

  const char *scan = selected_begin;

  while (scan != selected_end) {
    if (*scan == '%' && scan + 1 != selected_end) {
      if (scan[1] == 's') {
        if (!append_c_string(output, end, state.letters)) {
          return nullptr;
        }

        scan += 2;
        continue;
      }

      if (scan[1] == 'z') {
        if (!append_numeric_offset(output, end, state.offset_seconds)) {
          return nullptr;
        }

        scan += 2;
        continue;
      }
    }

    if (!append_character(output, end, *scan++)) {
      return nullptr;
    }
  }

  *output = '\0';

  return buffer;
}

bool same_public_state(const runtime::era_definition &era,
                       const tail_state &left, const tail_state &right,
                       bool &same) noexcept {
  if (left.offset_seconds != right.offset_seconds ||
      left.save_minutes != right.save_minutes) {
    same = false;
    return true;
  }

  const char *left_abbreviation = make_abbreviation(era, left);

  const char *right_abbreviation = make_abbreviation(era, right);

  if (left_abbreviation == nullptr || right_abbreviation == nullptr) {
    return false;
  }

  same = std::strcmp(left_abbreviation, right_abbreviation) == 0;

  return true;
}

bool seed_state(const runtime::era_definition &era,
                const runtime::zone_interval &seed,
                tail_state &result) noexcept {
  const long long save = static_cast<long long>(seed.offset_seconds) -
                         static_cast<long long>(era.standard_offset_seconds);

  if (save < std::numeric_limits<int>::min() ||
      save > std::numeric_limits<int>::max()) {
    return false;
  }

  result.save_seconds = static_cast<int>(save);

  result.offset_seconds = seed.offset_seconds;

  result.save_minutes = seed.save_minutes;

  result.daylight = seed.save_minutes != 0;

  result.letters = nullptr;

  result.resolved_abbreviation = seed.abbreviation;

  return result.resolved_abbreviation != nullptr;
}

bool state_from_rule(const runtime::era_definition &era,
                     const runtime::rule_definition &rule,
                     tail_state &result) noexcept {
  if (rule.save_is_daylight && rule.save_seconds % 60 != 0) {
    return false;
  }

  const long long offset = static_cast<long long>(era.standard_offset_seconds) +
                           static_cast<long long>(rule.save_seconds);

  if (offset < std::numeric_limits<int>::min() ||
      offset > std::numeric_limits<int>::max()) {
    return false;
  }

  result.save_seconds = rule.save_seconds;

  result.offset_seconds = static_cast<int>(offset);

  result.save_minutes = rule.save_is_daylight ? rule.save_seconds / 60 : 0;

  result.daylight = rule.save_is_daylight;

  result.letters = rule.letters;

  result.resolved_abbreviation = nullptr;

  return result.letters != nullptr;
}

bool analyze_rule_set(const runtime::rule_set_definition &set,
                      long long horizon_year, rule_analysis &result) noexcept {
  if (!set)
    return false;

  result.has_max = false;
  result.settle_year = horizon_year;

  result.last_finite_year = horizon_year - 1;

  for (unsigned index = 0; index < set.rule_count; ++index) {
    const auto rule = runtime::rule_set_rule_at(
        set.rule_begin == runtime::invalid_index ? runtime::invalid_index : 0,
        0);

    (void)rule;
  }

  /*
   * rule_set_rule_at takes a rule-set index rather
   * than a rule_set_definition. This helper is filled
   * by analyze_rule_set_index below.
   */
  return true;
}

bool analyze_rule_set_index(unsigned rule_set_index, long long horizon_year,
                            rule_analysis &result) noexcept {
  const auto set = runtime::rule_set_at(rule_set_index);

  if (!set)
    return false;

  result.has_max = false;
  result.settle_year = horizon_year;

  result.last_finite_year = horizon_year - 1;

  for (unsigned index = 0; index < set.rule_count; ++index) {
    const auto rule = runtime::rule_set_rule_at(rule_set_index, index);

    if (!rule)
      return false;

    if (rule.to_max) {
      result.has_max = true;

      if (rule.from_year > result.settle_year) {
        result.settle_year = rule.from_year;
      }

      continue;
    }

    if (rule.to_year > result.last_finite_year) {
      result.last_finite_year = rule.to_year;
    }

    const long long after = static_cast<long long>(rule.to_year) + 1;

    if (after > result.settle_year) {
      result.settle_year = after;
    }
  }

  return true;
}

bool process_year(const runtime::era_definition &era, unsigned rule_set_index,
                  long long year, long long lower_bound, long long target,
                  tail_state &state, long long &current_begin,
                  bool &begin_unbounded, bool &found_next,
                  long long &next_transition) noexcept {
  const auto set = runtime::rule_set_at(rule_set_index);

  if (!set)
    return false;

  pending_rule pending[max_pending_rules]{};

  unsigned pending_count = 0;

  for (unsigned index = 0; index < set.rule_count; ++index) {
    const auto rule = runtime::rule_set_rule_at(rule_set_index, index);

    if (!rule)
      return false;

    if (!rule_applies(rule, year)) {
      continue;
    }

    if (pending_count == max_pending_rules) {
      return false;
    }

    long long civil = 0;

    if (!rule_civil_time(rule, year, civil)) {
      return false;
    }

    pending[pending_count++] = {
        rule,
        civil,
        false,
    };
  }

  while (true) {
    unsigned best = pending_count;

    long long best_time = 0;

    for (unsigned index = 0; index < pending_count; ++index) {
      if (pending[index].done)
        continue;

      long long candidate = 0;

      if (!to_sys_time(pending[index].civil, pending[index].rule.at_basis,
                       era.standard_offset_seconds, state.save_seconds,
                       candidate)) {
        return false;
      }

      if (best == pending_count || candidate < best_time) {
        best = index;
        best_time = candidate;
      } else if (candidate == best_time) {
        /*
         * Match buildtzdb: two active Rules
         * resolving to one instant make the data
         * unusable rather than being arbitrarily
         * ordered.
         */
        return false;
      }
    }

    if (best == pending_count)
      break;

    pending[best].done = true;

    /*
     * The seed already represents the state active
     * at lower_bound, so Rules before that point
     * must not be replayed into it.
     */
    if (best_time < lower_bound)
      continue;

    tail_state next_state;

    if (!state_from_rule(era, pending[best].rule, next_state)) {
      return false;
    }

    bool changed = false;

    if (!same_public_state(era, state, next_state, changed)) {
      return false;
    }

    /*
     * same_public_state returned equality.
     */
    changed = !changed;

    if (best_time <= target) {
      if (changed) {
        current_begin = best_time;
        begin_unbounded = false;
      }

      state = next_state;
      continue;
    }

    /*
     * A Rule after target can still be a public
     * no-op while changing the SAVE value used to
     * interpret a later wall-time Rule.
     *
     * Apply such no-ops internally and keep looking.
     */
    if (!changed) {
      state = next_state;
      continue;
    }

    found_next = true;
    next_transition = best_time;

    return true;
  }

  return true;
}

bool replay_years(const runtime::era_definition &era, unsigned rule_set_index,
                  long long first_year, long long last_year,
                  long long lower_bound, long long target, tail_state &state,
                  long long &current_begin, bool &begin_unbounded,
                  bool &found_next, long long &next_transition) noexcept {
  if (last_year < first_year)
    return true;

  long long year = first_year;

  for (;;) {
    if (!process_year(era, rule_set_index, year, lower_bound, target, state,
                      current_begin, begin_unbounded, found_next,
                      next_transition)) {
      return false;
    }

    if (found_next)
      return true;

    if (year == last_year)
      break;

    ++year;
  }

  return true;
}

runtime::zone_interval make_interval(const runtime::era_definition &era,
                                     const tail_state &state, long long begin,
                                     bool begin_unbounded,
                                     long long end) noexcept {
  runtime::zone_interval result;

  const char *abbreviation = make_abbreviation(era, state);

  if (abbreviation == nullptr)
    return {};

  result.valid = true;

  result.begin_unbounded = begin_unbounded;

  result.end_is_horizon = false;

  result.begin = begin;
  result.end = end;

  result.offset_seconds = state.offset_seconds;

  result.save_minutes = state.save_minutes;

  result.abbreviation = abbreviation;

  return result;
}

bool shift_interval(runtime::zone_interval &interval,
                    long long shift) noexcept {
  if (shift == 0)
    return true;

  constexpr long long maximum = std::numeric_limits<long long>::max();

  if (!interval.begin_unbounded) {
    if (interval.begin > maximum - shift) {
      return false;
    }

    interval.begin += shift;
  }

  if (interval.end != maximum) {
    if (interval.end > maximum - shift) {
      interval.end = maximum;
    } else {
      interval.end += shift;
    }
  }

  return true;
}

} // namespace

runtime::zone_interval lookup(runtime::zone_ref zone, long long timestamp,
                              const runtime::zone_interval &seed) noexcept {
  if (!zone || !seed)
    return {};

  const auto era = runtime::zone_final_era(zone);

  if (!era)
    return {};

  /*
   * A final Zone era must be open-ended. If this
   * invariant ever stops being true, the generated
   * database and the tail model disagree.
   */
  if (era.has_until)
    return {};

  tail_state initial_state;

  if (!seed_state(era, seed, initial_state)) {
    return {};
  }

  /*
   * No named Rules means the final state is constant
   * forever. The artificial precompute horizon simply
   * disappears.
   */
  if (era.rules != runtime::rules_kind::named) {
    return make_interval(era, initial_state, seed.begin, seed.begin_unbounded,
                         std::numeric_limits<long long>::max());
  }

  if (era.rule_set == runtime::invalid_index) {
    return {};
  }

  const long long horizon = runtime::precomputed_until();

  const long long horizon_year = civil_year_from_seconds(horizon);

  rule_analysis analysis;

  if (!analyze_rule_set_index(era.rule_set, horizon_year, analysis)) {
    return {};
  }

  /*
   * No recurring max Rule:
   *
   * Replay through the final finite Rule and then the
   * resulting state lasts forever.
   */
  if (!analysis.has_max) {
    tail_state state = initial_state;

    long long current_begin = seed.begin;

    bool begin_unbounded = seed.begin_unbounded;

    bool found_next = false;
    long long next_transition = 0;

    const long long final_rule_year = analysis.last_finite_year > horizon_year
                                          ? analysis.last_finite_year
                                          : horizon_year;

    if (!replay_years(era, era.rule_set, horizon_year - 1, final_rule_year + 2,
                      horizon, timestamp, state, current_begin, begin_unbounded,
                      found_next, next_transition)) {
      return {};
    }

    return make_interval(era, state, current_begin, begin_unbounded,
                         found_next ? next_transition
                                    : std::numeric_limits<long long>::max());
  }

  /*
   * Wait one complete recurring year after every
   * finite Rule has expired and every max Rule has
   * started.
   *
   * This makes the state entering cycle_year itself
   * part of the recurring rule regime instead of
   * accidentally carrying a finite historical state
   * into the beginning of our 400-year cycle.
   */
  const long long cycle_year = analysis.settle_year + 1;

  long long cycle_start = 0;

  if (!civil_seconds(cycle_year, 1, runtime::day_spec{}, 0, cycle_start)) {
    return {};
  }

  /*
   * Queries before the repeating-cycle anchor only
   * require the short direct replay.
   *
   * This also handles a query in the last
   * precomputed interval: target may be below horizon
   * while we're merely extending that interval's true
   * end beyond the artificial horizon.
   */
  if (timestamp < cycle_start) {
    tail_state state = initial_state;

    long long current_begin = seed.begin;

    bool begin_unbounded = seed.begin_unbounded;

    bool found_next = false;
    long long next_transition = 0;

    if (!replay_years(era, era.rule_set, horizon_year - 1, cycle_year + 2,
                      horizon, timestamp, state, current_begin, begin_unbounded,
                      found_next, next_transition)) {
      return {};
    }

    return make_interval(era, state, current_begin, begin_unbounded,
                         found_next ? next_transition
                                    : std::numeric_limits<long long>::max());
  }

  /*
   * Establish the exact state immediately before the
   * recurring cycle begins.
   */
  tail_state cycle_state = initial_state;

  long long cycle_begin = seed.begin;

  bool cycle_begin_unbounded = seed.begin_unbounded;

  bool ignored_next = false;
  long long ignored_transition = 0;

  if (!replay_years(era, era.rule_set, horizon_year - 1, cycle_year, horizon,
                    cycle_start - 1, cycle_state, cycle_begin,
                    cycle_begin_unbounded, ignored_next, ignored_transition)) {
    return {};
  }

  const long long target_year = civil_year_from_seconds(timestamp);

  long long cycles = 0;

  if (target_year >= cycle_year + 400) {
    cycles = (target_year - cycle_year) / 400;
  }

  if (cycles > std::numeric_limits<long long>::max() / seconds_per_cycle) {
    return {};
  }

  const long long shift = cycles * seconds_per_cycle;

  const long long mapped_timestamp = timestamp - shift;

  const long long mapped_year = civil_year_from_seconds(mapped_timestamp);

  tail_state state = cycle_state;

  long long current_begin = cycle_begin;

  bool begin_unbounded = cycle_begin_unbounded;

  bool found_next = false;
  long long next_transition = 0;

  if (!replay_years(era, era.rule_set, cycle_year - 1, mapped_year + 2,
                    cycle_start, mapped_timestamp, state, current_begin,
                    begin_unbounded, found_next, next_transition)) {
    return {};
  }

  auto result = make_interval(
      era, state, current_begin, begin_unbounded,
      found_next ? next_transition : std::numeric_limits<long long>::max());

  if (!result)
    return {};

  if (!shift_interval(result, shift)) {
    return {};
  }

  return result;
}

} // namespace ftl_tzdb_tail
