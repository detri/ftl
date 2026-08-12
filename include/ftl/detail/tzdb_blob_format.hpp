// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_TZDB_BLOB_FORMAT_HEADER
#define FTL_DETAIL_TZDB_BLOB_FORMAT_HEADER

#include <cstddef>
#include <cstdint>

namespace ftl_tzdb_blob_format {

inline constexpr unsigned char magic[8] = {
    'F', 'T', 'L', 'T', 'Z', 'D', 'B', '1',
};

inline constexpr std::uint32_t format_version = 1;
inline constexpr std::uint32_t no_rule_set = 0xffffffffu;

enum class table_id : std::uint32_t {
  strings = 0,
  rules,
  rule_sets,
  eras,
  transitions,
  zones,
  links,
  leaps,
  windows_zones,
  count,
};

namespace header {

inline constexpr std::size_t magic = 0;
inline constexpr std::size_t format_version = 8;
inline constexpr std::size_t total_size = 12;
inline constexpr std::size_t version_string = 16;
inline constexpr std::size_t reserved = 20;
inline constexpr std::size_t precomputed_until = 24;
inline constexpr std::size_t leap_expiration = 32;

inline constexpr std::size_t tables = 40;
inline constexpr std::size_t table_span_size = 8;

inline constexpr std::size_t size =
    tables + static_cast<std::size_t>(table_id::count) * table_span_size;

} // namespace header

namespace rule_record {

inline constexpr std::size_t from_year = 0;
inline constexpr std::size_t to_year = 4;
inline constexpr std::size_t to_max = 8;
inline constexpr std::size_t month = 9;

inline constexpr std::size_t day_kind = 13;
inline constexpr std::size_t weekday = 14;
inline constexpr std::size_t day = 18;

inline constexpr std::size_t at_seconds = 22;
inline constexpr std::size_t at_basis = 26;

inline constexpr std::size_t save_seconds = 27;
inline constexpr std::size_t save_is_daylight = 31;

inline constexpr std::size_t letters = 32;

inline constexpr std::size_t size = 36;

} // namespace rule_record

namespace rule_set_record {

inline constexpr std::size_t name = 0;
inline constexpr std::size_t rule_begin = 4;
inline constexpr std::size_t rule_count = 8;

inline constexpr std::size_t size = 12;

} // namespace rule_set_record

namespace era_record {

inline constexpr std::size_t standard_offset_seconds = 0;

inline constexpr std::size_t rules = 4;
inline constexpr std::size_t rule_set = 5;

inline constexpr std::size_t fixed_save_seconds = 9;
inline constexpr std::size_t fixed_save_is_daylight = 13;

inline constexpr std::size_t format = 14;

inline constexpr std::size_t has_until = 18;
inline constexpr std::size_t until_year = 19;
inline constexpr std::size_t until_month = 23;

inline constexpr std::size_t until_day_kind = 27;
inline constexpr std::size_t until_weekday = 28;
inline constexpr std::size_t until_day = 32;

inline constexpr std::size_t until_seconds = 36;
inline constexpr std::size_t until_basis = 40;

inline constexpr std::size_t size = 41;

} // namespace era_record

namespace transition_record {

inline constexpr std::size_t begin = 0;
inline constexpr std::size_t offset_seconds = 8;
inline constexpr std::size_t save_minutes = 12;
inline constexpr std::size_t abbreviation = 16;

inline constexpr std::size_t size = 20;

} // namespace transition_record

namespace zone_record {

inline constexpr std::size_t name = 0;

inline constexpr std::size_t era_begin = 4;
inline constexpr std::size_t era_count = 8;

inline constexpr std::size_t transition_begin = 12;
inline constexpr std::size_t transition_count = 16;

inline constexpr std::size_t initial_offset_seconds = 20;
inline constexpr std::size_t initial_save_minutes = 24;
inline constexpr std::size_t initial_abbreviation = 28;

inline constexpr std::size_t size = 32;

} // namespace zone_record

namespace link_record {

inline constexpr std::size_t name = 0;
inline constexpr std::size_t target_zone = 4;

inline constexpr std::size_t size = 8;

} // namespace link_record

namespace leap_record {

inline constexpr std::size_t date_seconds = 0;
inline constexpr std::size_t value = 8;

inline constexpr std::size_t size = 12;

} // namespace leap_record

namespace windows_zone_record {

inline constexpr std::size_t windows_name = 0;
inline constexpr std::size_t iana_name = 4;

inline constexpr std::size_t size = 8;

} // namespace windows_zone_record

constexpr std::size_t table_span_offset(table_id table) noexcept {
  return header::tables +
         static_cast<std::size_t>(table) * header::table_span_size;
}

} // namespace ftl_tzdb_blob_format

#endif
