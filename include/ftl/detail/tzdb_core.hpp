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
