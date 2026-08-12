// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_TZDB_CORE_HEADER
#define FTL_DETAIL_TZDB_CORE_HEADER

namespace ftl_tzdb_runtime_core {

using size_type = decltype(sizeof(0));

inline constexpr unsigned invalid_index = ~0u;

struct zone_ref {
  unsigned index = invalid_index;

  constexpr explicit operator bool() const noexcept {
    return index != invalid_index;
  }
};

struct link_ref {
  unsigned index = invalid_index;

  constexpr explicit operator bool() const noexcept {
    return index != invalid_index;
  }
};

enum class day_kind : unsigned char {
  exact,
  last_weekday,
  weekday_on_or_after,
  weekday_on_or_before,
};

enum class time_basis : unsigned char {
  wall,
  standard,
  universal,
};

enum class rules_kind : unsigned char {
  none,
  fixed,
  named,
};

struct day_spec {
  day_kind kind = day_kind::exact;
  int weekday = 0;
  int day = 1;
};

struct rule_definition {
  bool valid = false;

  int from_year = 0;
  int to_year = 0;
  bool to_max = false;

  int month = 1;
  day_spec on;

  int at_seconds = 0;
  time_basis at_basis = time_basis::wall;

  int save_seconds = 0;
  bool save_is_daylight = false;

  const char *letters = nullptr;

  constexpr explicit operator bool() const noexcept { return valid; }
};

struct rule_set_definition {
  bool valid = false;

  const char *name = nullptr;

  unsigned rule_begin = 0;
  unsigned rule_count = 0;

  constexpr explicit operator bool() const noexcept { return valid; }
};

struct era_definition {
  bool valid = false;

  int standard_offset_seconds = 0;

  rules_kind rules = rules_kind::none;
  unsigned rule_set = invalid_index;

  int fixed_save_seconds = 0;
  bool fixed_save_is_daylight = false;

  const char *format = nullptr;

  bool has_until = false;

  int until_year = 0;
  int until_month = 1;
  day_spec until_day;

  int until_seconds = 0;
  time_basis until_basis = time_basis::wall;

  constexpr explicit operator bool() const noexcept { return valid; }
};

struct zone_interval {
  bool valid = false;

  bool begin_unbounded = false;
  bool end_is_horizon = false;

  long long begin = 0;
  long long end = 0;

  int offset_seconds = 0;
  int save_minutes = 0;

  const char *abbreviation = nullptr;

  constexpr explicit operator bool() const noexcept { return valid; }
};

enum class local_result_kind : unsigned char {
  invalid,
  unique,
  nonexistent,
  ambiguous,
};

struct local_lookup_result {
  local_result_kind result = local_result_kind::invalid;

  zone_interval first;
  zone_interval second;

  constexpr explicit operator bool() const noexcept {
    return result != local_result_kind::invalid;
  }
};

bool valid() noexcept;

unsigned zone_count() noexcept;
unsigned link_count() noexcept;
unsigned leap_count() noexcept;

unsigned rule_count() noexcept;
unsigned rule_set_count() noexcept;
unsigned era_count() noexcept;

rule_definition rule_at(unsigned index) noexcept;

rule_set_definition rule_set_at(unsigned index) noexcept;

era_definition era_at(unsigned index) noexcept;

rule_definition rule_set_rule_at(unsigned rule_set_index,
                                 unsigned relative_index) noexcept;

unsigned zone_era_count(zone_ref zone) noexcept;

era_definition zone_era_at(zone_ref zone, unsigned relative_index) noexcept;

era_definition zone_final_era(zone_ref zone) noexcept;

long long leap_date_seconds(unsigned index) noexcept;
int leap_value(unsigned index) noexcept;
int leap_elapsed_at_sys(long long timestamp) noexcept;

long long leap_expiration() noexcept;
long long precomputed_until() noexcept;

const char *version() noexcept;

unsigned windows_zone_count() noexcept;

const char *windows_zone_target(const char *name, size_type length) noexcept;

const char *windows_zone_target(const char *name) noexcept;

zone_ref zone_at(unsigned index) noexcept;
link_ref link_at(unsigned index) noexcept;

const char *zone_name(zone_ref zone) noexcept;
const char *link_name(link_ref link) noexcept;

zone_ref link_target(link_ref link) noexcept;

zone_ref find_zone(const char *name, size_type length) noexcept;

zone_ref find_zone(const char *name) noexcept;

link_ref find_link(const char *name, size_type length) noexcept;

link_ref find_link(const char *name) noexcept;

zone_ref locate_zone(const char *name, size_type length) noexcept;

zone_ref locate_zone(const char *name) noexcept;

zone_interval interval_at(zone_ref zone, unsigned state_index) noexcept;

zone_interval lookup(zone_ref zone, long long timestamp) noexcept;

local_lookup_result lookup_local(zone_ref zone, long long timestamp) noexcept;

} // namespace ftl_tzdb_runtime_core

#endif
