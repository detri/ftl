#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ftl_tzdb_tool {

enum class day_kind : std::uint8_t {
  exact,
  last_weekday,
  weekday_on_or_after,
  weekday_on_or_before,
};

enum class time_basis : std::uint8_t {
  wall,
  standard,
  universal,
};

enum class rules_kind : std::uint8_t {
  none,
  fixed,
  named,
};

struct day_spec {
  day_kind kind = day_kind::exact;
  int weekday = 0; // 0 = Sunday
  int day = 1;
};

struct clock_time {
  int seconds = 0;
  time_basis basis = time_basis::wall;
};

struct save_spec {
  int seconds = 0;
  bool daylight = false;
};

struct rule {
  std::string name;

  int from_year = 0;

  int to_year = 0;
  bool to_max = false;

  int month = 1;
  day_spec on;
  clock_time at;

  save_spec save;
  std::string letters;
};

struct until_spec {
  int year = 0;
  int month = 1;
  day_spec day;
  clock_time time;
};

struct zone_era {
  int standard_offset_seconds = 0;

  rules_kind rules = rules_kind::none;
  std::string rule_name;
  save_spec fixed_save;

  std::string format;
  std::optional<until_spec> until;
};

struct zone {
  std::string name;
  std::vector<zone_era> eras;
};

struct link {
  std::string target;
  std::string name;
};

struct leap {
  std::int64_t date_seconds = 0;
  int correction = 0;
  bool rolling = false;
};

struct database {
  std::string version;

  std::vector<rule> rules;
  std::vector<zone> zones;
  std::vector<link> links;
  std::vector<leap> leaps;

  std::optional<std::int64_t> leap_expiration;
};

} // namespace ftl_tzdb_tool
