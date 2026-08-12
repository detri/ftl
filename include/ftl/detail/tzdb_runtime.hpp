// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_TZDB_RUNTIME_HEADER
#define FTL_DETAIL_TZDB_RUNTIME_HEADER

#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <detail/tzdb_data.hpp>
#include <type_traits>
#else
#include <ftl/cstddef>
#include <ftl/detail/tzdb_data.hpp>
#include <ftl/type_traits>
#endif

FTL_BEGIN_NAMESPACE

namespace detail::tzdb_runtime {

namespace data = ::ftl_tzdb_data;

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

  /*
   * begin_unbounded means the interval is represented by the generated
   * zone's initial state and conceptually begins at system-clock minimum.
   *
   * end_is_horizon means no later precomputed transition exists. The
   * supplied end value is the generated-data coverage horizon, NOT
   * necessarily the true end of the timezone interval. The tail evaluator
   * will take over from there later.
   */
  bool begin_unbounded = false;
  bool end_is_horizon = false;

  long long begin = 0;
  long long end = 0;

  int offset_seconds = 0;
  int save_minutes = 0;

  const char *abbreviation = nullptr;

  constexpr explicit operator bool() const noexcept { return valid; }
};

constexpr unsigned zone_count() noexcept {
  return static_cast<unsigned>(sizeof(data::zones) / sizeof(data::zones[0]));
}

constexpr unsigned link_count() noexcept {
  return static_cast<unsigned>(sizeof(data::links) / sizeof(data::links[0]));
}

constexpr unsigned leap_count() noexcept {
  return static_cast<unsigned>(sizeof(data::leaps) / sizeof(data::leaps[0]));
}

constexpr long long precomputed_until() noexcept {
  return data::precomputed_until;
}

constexpr const char *version() noexcept { return data::version; }

constexpr int compare_text(const char *left, size_t left_size,
                           const char *right) noexcept {
  size_t index = 0;

  while (index < left_size && right[index] != '\0') {
    const auto l = static_cast<unsigned char>(left[index]);
    const auto r = static_cast<unsigned char>(right[index]);

    if (l < r)
      return -1;

    if (l > r)
      return 1;

    ++index;
  }

  if (index == left_size)
    return right[index] == '\0' ? 0 : -1;

  return 1;
}

constexpr int compare_text(const char *left, const char *right) noexcept {
  size_t length = 0;

  while (left[length] != '\0')
    ++length;

  return compare_text(left, length, right);
}

constexpr zone_ref zone_at(unsigned index) noexcept {
  if (index >= zone_count())
    return {};

  return {index};
}

constexpr link_ref link_at(unsigned index) noexcept {
  if (index >= link_count())
    return {};

  return {index};
}

constexpr const char *zone_name(zone_ref zone) noexcept {
  if (!zone)
    return nullptr;

  return data::string_at(data::zones[zone.index].name);
}

constexpr const char *link_name(link_ref link) noexcept {
  if (!link)
    return nullptr;

  return data::string_at(data::links[link.index].name);
}

constexpr zone_ref link_target(link_ref link) noexcept {
  if (!link)
    return {};

  const auto target = data::links[link.index].target_zone;

  if (target >= zone_count())
    return {};

  return {target};
}

constexpr zone_ref find_zone(const char *name, size_t length) noexcept {
  if (name == nullptr)
    return {};

  unsigned first = 0;
  unsigned last = zone_count();

  while (first < last) {
    const unsigned middle = first + (last - first) / 2;

    const auto &zone = data::zones[middle];

    const int order = compare_text(name, length, data::string_at(zone.name));

    if (order < 0) {
      last = middle;
    } else if (order > 0) {
      first = middle + 1;
    } else {
      return {middle};
    }
  }

  return {};
}

constexpr zone_ref find_zone(const char *name) noexcept {
  if (name == nullptr)
    return {};

  size_t length = 0;

  while (name[length] != '\0')
    ++length;

  return find_zone(name, length);
}

constexpr link_ref find_link(const char *name, size_t length) noexcept {
  if (name == nullptr)
    return {};

  unsigned first = 0;
  unsigned last = link_count();

  while (first < last) {
    const unsigned middle = first + (last - first) / 2;

    const auto &link = data::links[middle];

    const int order = compare_text(name, length, data::string_at(link.name));

    if (order < 0) {
      last = middle;
    } else if (order > 0) {
      first = middle + 1;
    } else {
      return {middle};
    }
  }

  return {};
}

constexpr link_ref find_link(const char *name) noexcept {
  if (name == nullptr)
    return {};

  size_t length = 0;

  while (name[length] != '\0')
    ++length;

  return find_link(name, length);
}

/*
 * Locate either a canonical Zone or a Link name.
 *
 * Links were completely resolved by buildtzdb, so this operation never
 * needs to chase an alias chain at runtime.
 */
constexpr zone_ref locate_zone(const char *name, size_t length) noexcept {
  if (const auto zone = find_zone(name, length))
    return zone;

  if (const auto link = find_link(name, length))
    return link_target(link);

  return {};
}

constexpr zone_ref locate_zone(const char *name) noexcept {
  if (name == nullptr)
    return {};

  size_t length = 0;

  while (name[length] != '\0')
    ++length;

  return locate_zone(name, length);
}

/*
 * Find the precomputed state in effect at the supplied system-clock second.
 *
 * transitions[] stores state changes, not intervals. We therefore perform
 * an upper_bound over the zone's transition slice:
 *
 *   transition.begin <= timestamp
 *
 * The transition immediately preceding that upper bound owns timestamp.
 * Consequently an exact transition instant belongs to the NEW state.
 */
constexpr zone_interval lookup(zone_ref zone, long long timestamp) noexcept {
  if (!zone || timestamp >= data::precomputed_until)
    return {};

  const auto &record = data::zones[zone.index];

  unsigned first = 0;
  unsigned last = record.transition_count;

  while (first < last) {
    const unsigned middle = first + (last - first) / 2;

    const auto &transition =
        data::transitions[record.transition_begin + middle];

    if (transition.begin <= timestamp)
      first = middle + 1;
    else
      last = middle;
  }

  /*
   * first is now the number of transitions whose begin <= timestamp.
   */
  zone_interval result;
  result.valid = true;

  if (first == 0) {
    result.begin_unbounded = true;

    result.offset_seconds = record.initial_offset_seconds;
    result.save_minutes = record.initial_save_minutes;
    result.abbreviation = data::string_at(record.initial_abbreviation);
  } else {
    const auto &transition =
        data::transitions[record.transition_begin + first - 1];

    result.begin = transition.begin;

    result.offset_seconds = transition.offset_seconds;
    result.save_minutes = transition.save_minutes;
    result.abbreviation = data::string_at(transition.abbreviation);
  }

  if (first < record.transition_count) {
    result.end = data::transitions[record.transition_begin + first].begin;
  } else {
    result.end = data::precomputed_until;
    result.end_is_horizon = true;
  }

  return result;
}

} // namespace detail::tzdb_runtime

FTL_END_NAMESPACE

#endif
