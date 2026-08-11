#include "transitions.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ftl_tzdb_tool {
namespace {

using seconds64 = std::int64_t;

seconds64 civil_seconds(int year_value, int month_value, const day_spec &spec,
                        int seconds_from_midnight) {

  using namespace std::chrono;

  const year y{year_value};
  const month m{static_cast<unsigned>(month_value)};

  if (!y.ok() || !m.ok())
    throw std::runtime_error("invalid civil year/month");

  sys_days date;

  switch (spec.kind) {
  case day_kind::exact: {
    date = sys_days{y / m / day{1}} + days{spec.day - 1};
    break;
  }

  case day_kind::last_weekday: {
    const auto last = sys_days{y / m / std::chrono::last};

    const weekday actual{last};

    const int delta =
        (static_cast<int>(actual.c_encoding()) - spec.weekday + 7) % 7;

    date = last - days{delta};
    break;
  }

  case day_kind::weekday_on_or_after: {
    auto base = sys_days{y / m / day{1}} + days{spec.day - 1};

    const weekday actual{base};

    const int delta =
        (spec.weekday - static_cast<int>(actual.c_encoding()) + 7) % 7;

    date = base + days{delta};
    break;
  }

  case day_kind::weekday_on_or_before: {
    auto base = sys_days{y / m / day{1}} + days{spec.day - 1};

    const weekday actual{base};

    const int delta =
        (static_cast<int>(actual.c_encoding()) - spec.weekday + 7) % 7;

    date = base - days{delta};
    break;
  }
  }

  return sys_seconds{date}.time_since_epoch().count() + seconds_from_midnight;
}

seconds64 civil_seconds(int year_value, int month_value, int day_value,
                        int seconds_from_midnight = 0) {

  return civil_seconds(year_value, month_value,
                       day_spec{
                           day_kind::exact,
                           0,
                           day_value,
                       },
                       seconds_from_midnight);
}

seconds64 rule_civil_time(const rule &r, int year_value) {

  return civil_seconds(year_value, r.month, r.on, r.at.seconds);
}

seconds64 until_civil_time(const until_spec &u) {

  return civil_seconds(u.year, u.month, u.day, u.time.seconds);
}

seconds64 to_sys_time(seconds64 civil, time_basis basis, int standard_offset,
                      int current_save) {

  switch (basis) {
  case time_basis::universal:
    return civil;

  case time_basis::standard:
    return civil - standard_offset;

  case time_basis::wall:
    return civil - standard_offset - current_save;
  }

  throw std::runtime_error("invalid time basis");
}

bool rule_applies(const rule &r, int year_value) {

  if (year_value < r.from_year)
    return false;

  if (r.to_max)
    return true;

  return year_value <= r.to_year;
}

std::string numeric_abbreviation_offset(int offset_seconds) {

  const bool negative = offset_seconds < 0;

  std::int64_t value = negative ? -static_cast<std::int64_t>(offset_seconds)
                                : static_cast<std::int64_t>(offset_seconds);

  const auto hours = value / 3600;
  value %= 3600;

  const auto minutes = value / 60;
  const auto seconds = value % 60;

  std::string result;

  result.push_back(negative ? '-' : '+');

  auto append_two = [&](std::int64_t n) {
    result.push_back(static_cast<char>('0' + (n / 10) % 10));

    result.push_back(static_cast<char>('0' + n % 10));
  };

  if (hours < 10)
    result.push_back('0');

  result += std::to_string(hours);

  if (minutes != 0 || seconds != 0)
    append_two(minutes);

  if (seconds != 0)
    append_two(seconds);

  return result;
}

void replace_all(std::string &value, std::string_view needle,
                 std::string_view replacement) {

  if (needle.empty())
    return;

  std::size_t pos = 0;

  while ((pos = value.find(needle, pos)) != std::string::npos) {

    value.replace(pos, needle.size(), replacement);

    pos += replacement.size();
  }
}

std::string make_abbreviation(const std::string &format,
                              std::string_view letters, bool daylight,
                              int total_offset_seconds) {

  std::string result = format;

  if (const auto slash = result.find('/'); slash != std::string::npos) {

    if (result.find('/', slash + 1) != std::string::npos) {

      throw std::runtime_error(
          "time zone abbreviation format has multiple '/' separators: " +
          format);
    }

    if (daylight)
      result.erase(0, slash + 1);
    else
      result.erase(slash);
  }

  replace_all(result, "%s", letters);

  replace_all(result, "%z", numeric_abbreviation_offset(total_offset_seconds));

  return result;
}

compiled_state make_state(const zone_era &era, int save_seconds, bool daylight,
                          std::string_view letters) {

  if (daylight && save_seconds % 60 != 0) {

    throw std::runtime_error("daylight SAVE cannot be represented exactly by "
                             "std::chrono::sys_info::save minutes");
  }

  compiled_state result;

  result.offset_seconds = era.standard_offset_seconds + save_seconds;

  /*
   * N4950's sys_info::save describes daylight-saving adjustment,
   * not merely the arithmetic difference from STDOFF.
   *
   * IANA permits a nonzero SAVE explicitly marked as standard time,
   * so preserve the total offset above but expose save == 0min for
   * that state.
   */
  result.save_minutes = daylight ? save_seconds / 60 : 0;

  result.abbreviation =
      make_abbreviation(era.format, letters, daylight, result.offset_seconds);

  return result;
}

std::string default_rule_letters(const std::vector<const rule *> &rules) {

  const rule *best = nullptr;

  for (const auto *r : rules) {
    if (r->save.daylight)
      continue;

    if (!best) {
      best = r;
      continue;
    }

    const auto a = std::tuple{
        r->from_year, r->month,      static_cast<int>(r->on.kind),
        r->on.day,    r->on.weekday, r->at.seconds,
    };

    const auto b = std::tuple{
        best->from_year, best->month,      static_cast<int>(best->on.kind),
        best->on.day,    best->on.weekday, best->at.seconds,
    };

    if (a < b)
      best = r;
  }

  return best ? best->letters : std::string{};
}

void add_transition(compiled_zone &zone, std::int64_t at, compiled_state state,
                    std::int64_t horizon) {

  if (at >= horizon)
    return;

  if (!zone.transitions.empty()) {
    auto &previous = zone.transitions.back();

    if (at < previous.at) {
      throw std::runtime_error("non-monotonic transition sequence in zone " +
                               zone.name);
    }

    if (at == previous.at) {
      previous.state = std::move(state);
      return;
    }

    if (previous.state == state)
      return;

  } else if (zone.initial == state) {
    return;
  }

  zone.transitions.push_back({
      at,
      std::move(state),
  });
}

int database_min_year(const database &db) {

  int result = 1970;

  for (const auto &r : db.rules)
    result = std::min(result, r.from_year);

  for (const auto &z : db.zones) {
    for (const auto &era : z.eras) {
      if (era.until)
        result = std::min(result, era.until->year);
    }
  }

  /*
   * One year of breathing room matters because ON expressions and
   * negative/greater-than-24-hour AT values are allowed to cross a
   * calendar-year boundary.
   */
  if (result > std::numeric_limits<int>::min())
    --result;

  return result;
}

std::unordered_map<std::string, std::vector<const rule *>>
build_rule_sets(const database &db) {

  std::unordered_map<std::string, std::vector<const rule *>> result;

  for (const auto &r : db.rules)
    result[r.name].push_back(&r);

  return result;
}

compiled_zone compile_zone(
    const zone &source,
    const std::unordered_map<std::string, std::vector<const rule *>> &rule_sets,
    int min_year, int through_year, std::int64_t horizon) {

  if (source.eras.empty()) {
    throw std::runtime_error("zone has no eras: " + source.name);
  }

  compiled_zone result;
  result.name = source.name;
  result.precomputed_until = horizon;

  std::int64_t start_time = std::numeric_limits<std::int64_t>::min();

  bool have_initial = false;

  for (std::size_t era_index = 0; era_index < source.eras.size(); ++era_index) {

    const auto &era = source.eras[era_index];

    const bool use_start = era_index != 0;

    const bool use_until = era.until.has_value();

    if (start_time >= horizon)
      break;

    /*
     * An era with no named rule set has a single fixed state.
     */
    if (era.rules != rules_kind::named) {
      const int save_seconds =
          era.rules == rules_kind::fixed ? era.fixed_save.seconds : 0;

      const bool daylight =
          era.rules == rules_kind::fixed ? era.fixed_save.daylight : false;

      auto state = make_state(era, save_seconds, daylight, {});

      if (!have_initial) {
        result.initial = std::move(state);

        have_initial = true;
      } else {
        add_transition(result, start_time, std::move(state), horizon);
      }

      if (use_until) {
        start_time =
            to_sys_time(until_civil_time(*era.until), era.until->time.basis,
                        era.standard_offset_seconds, save_seconds);
      }

      continue;
    }

    /*
     * Named-rule era.
     */
    const auto rules_it = rule_sets.find(era.rule_name);

    if (rules_it == rule_sets.end()) {
      throw std::runtime_error("zone '" + source.name +
                               "' references unknown rule set '" +
                               era.rule_name + "'");
    }

    const auto &rules = rules_it->second;

    int current_save = 0;
    bool current_daylight = false;

    std::string current_letters = default_rule_letters(rules);

    /*
     * The first zone era conceptually extends to the beginning of
     * representable time, so its initial state becomes the zone's
     * initial state.
     */
    if (!have_initial) {
      result.initial =
          make_state(era, current_save, current_daylight, current_letters);

      have_initial = true;
    }

    /*
     * For later eras we must determine which Rule state is active
     * immediately after start_time.
     *
     * We do that by replaying rules from before the era boundary.
     * Once the first rule strictly after start_time is encountered,
     * emit the era-start state BEFORE emitting that later rule.
     *
     * If a rule occurs exactly at start_time, that rule itself
     * supplies the state beginning at the boundary.
     */
    bool need_start_transition = use_start;

    bool era_done = false;

    int last_year = through_year;

    if (use_until) {
      last_year = std::min(last_year, era.until->year + 1);
    }

    for (int year_value = min_year; year_value <= last_year && !era_done;
         ++year_value) {

      struct pending_rule {
        const rule *source = nullptr;
        std::int64_t civil = 0;
        bool done = false;
      };

      std::vector<pending_rule> pending;

      for (const auto *r : rules) {
        if (!rule_applies(*r, year_value)) {
          continue;
        }

        pending.push_back({
            r,
            rule_civil_time(*r, year_value),
            false,
        });
      }

      while (true) {
        /*
         * UNTIL expressed as wall time depends on the SAVE
         * state in effect immediately before the boundary, so
         * recompute it whenever current_save changes.
         */
        std::int64_t until_time = std::numeric_limits<std::int64_t>::max();

        if (use_until) {
          until_time =
              to_sys_time(until_civil_time(*era.until), era.until->time.basis,
                          era.standard_offset_seconds, current_save);
        }

        std::size_t best = pending.size();

        std::int64_t best_time = 0;

        /*
         * Rule times expressed as wall time also depend on the
         * currently active SAVE value. Recompute every pending
         * candidate after each applied rule and choose the
         * earliest resulting system time.
         */
        for (std::size_t i = 0; i < pending.size(); ++i) {

          if (pending[i].done)
            continue;

          const auto *r = pending[i].source;

          const auto candidate =
              to_sys_time(pending[i].civil, r->at.basis,
                          era.standard_offset_seconds, current_save);

          if (best == pending.size() || candidate < best_time) {

            best = i;
            best_time = candidate;

          } else if (candidate == best_time) {

            throw std::runtime_error("two rules for the same instant in zone " +
                                     source.name + " using rule set " +
                                     era.rule_name);
          }
        }

        if (best == pending.size())
          break;

        pending[best].done = true;

        const auto &r = *pending[best].source;

        /*
         * The Zone continuation boundary wins a tie with a Rule.
         * The Rule therefore belongs to neither the previous era
         * nor its state calculation after UNTIL.
         */
        if (use_until && best_time >= until_time) {

          era_done = true;
          break;
        }

        /*
         * We've finished replaying rules that precede the era.
         *
         * The currently active state is therefore the state at
         * start_time. Emit it NOW, before the later rule, so the
         * generated transition stream remains monotonically
         * ordered.
         */
        if (need_start_transition && best_time > start_time) {

          add_transition(
              result, start_time,
              make_state(era, current_save, current_daylight, current_letters),
              horizon);

          need_start_transition = false;
        }

        /*
         * Apply the Rule.
         */
        current_save = r.save.seconds;

        current_daylight = r.save.daylight;

        current_letters = r.letters;

        /*
         * A Rule exactly at the era boundary supplies the
         * boundary state itself, so no separate start transition
         * is necessary.
         */
        if (need_start_transition && best_time == start_time) {

          need_start_transition = false;
        }

        if (best_time >= start_time) {
          add_transition(
              result, best_time,
              make_state(era, current_save, current_daylight, current_letters),
              horizon);
        }
      }
    }

    /*
     * There may have been no Rule after the era boundary at all.
     * In that case the replayed state is still the state beginning
     * at start_time.
     */
    if (need_start_transition) {
      add_transition(
          result, start_time,
          make_state(era, current_save, current_daylight, current_letters),
          horizon);
    }

    /*
     * Compute the next era's boundary using the SAVE state actually
     * in force immediately before UNTIL.
     */
    if (use_until) {
      start_time =
          to_sys_time(until_civil_time(*era.until), era.until->time.basis,
                      era.standard_offset_seconds, current_save);
    }
  }

  if (!have_initial) {
    throw std::runtime_error("failed to establish initial state for zone " +
                             source.name);
  }

  return result;
}

} // namespace

compiled_database compile_transitions(const database &db, int through_year) {

  if (through_year < 1970 || through_year >= 32767) {

    throw std::runtime_error("invalid transition precomputation horizon");
  }

  const auto rule_sets = build_rule_sets(db);

  const int min_year = database_min_year(db);

  const auto horizon = civil_seconds(through_year + 1, 1, 1);

  compiled_database result;
  result.precomputed_until = horizon;

  result.zones.reserve(db.zones.size());

  for (const auto &z : db.zones) {
    result.zones.push_back(
        compile_zone(z, rule_sets, min_year, through_year, horizon));
  }

  std::ranges::sort(result.zones,
                    [](const compiled_zone &a, const compiled_zone &b) {
                      return a.name < b.name;
                    });

  return result;
}

} // namespace ftl_tzdb_tool
