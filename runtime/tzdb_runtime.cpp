#include "tzdb_tail.hpp"

#include <ftl/detail/tzdb_blob.hpp>
#include <ftl/detail/tzdb_blob_format.hpp>
#include <ftl/detail/tzdb_core.hpp>

#include <cstddef>
#include <cstdint>
#include <limits>

namespace ftl_tzdb_runtime_core {
namespace {

namespace format = ::ftl_tzdb_blob_format;

struct table_span {
  std::uint32_t offset = 0;
  std::uint32_t count = 0;
};

const unsigned char *blob() noexcept { return ::ftl_tzdb_blob::data(); }

size_type blob_size() noexcept { return ::ftl_tzdb_blob::size(); }

std::uint32_t read_u32(const unsigned char *data) noexcept {
  return static_cast<std::uint32_t>(data[0]) |
         (static_cast<std::uint32_t>(data[1]) << 8) |
         (static_cast<std::uint32_t>(data[2]) << 16) |
         (static_cast<std::uint32_t>(data[3]) << 24);
}

std::int32_t read_i32(const unsigned char *data) noexcept {
  const std::uint32_t bits = read_u32(data);

  if (bits <= 0x7fffffffu)
    return static_cast<std::int32_t>(bits);

  return static_cast<std::int32_t>(-1 - static_cast<std::int32_t>(~bits));
}

std::uint64_t read_u64(const unsigned char *data) noexcept {
  std::uint64_t result = 0;

  for (unsigned byte = 0; byte != 8; ++byte) {
    result |= static_cast<std::uint64_t>(data[byte]) << (byte * 8);
  }

  return result;
}

std::int64_t read_i64(const unsigned char *data) noexcept {
  const std::uint64_t bits = read_u64(data);

  if (bits <= 0x7fffffffffffffffULL) {
    return static_cast<std::int64_t>(bits);
  }

  return -1 - static_cast<std::int64_t>(~bits);
}

table_span table(format::table_id id) noexcept {
  const auto offset = format::table_span_offset(id);

  return {
      read_u32(blob() + offset),
      read_u32(blob() + offset + 4),
  };
}

const unsigned char *record_at(table_span span, std::size_t record_size,
                               unsigned index) noexcept {
  if (index >= span.count)
    return nullptr;

  return blob() + span.offset + static_cast<size_type>(index) * record_size;
}

const char *string_at(std::uint32_t relative_offset) noexcept {
  const auto strings = table(format::table_id::strings);

  if (relative_offset >= strings.count)
    return nullptr;

  return reinterpret_cast<const char *>(blob() + strings.offset +
                                        relative_offset);
}

bool span_valid(table_span span, std::size_t element_size) noexcept {
  if (span.offset > blob_size())
    return false;

  const auto available = blob_size() - span.offset;

  if (element_size == 0)
    return false;

  return span.count <= available / element_size;
}

int compare_text(const char *left, size_type left_size,
                 const char *right) noexcept {
  size_type index = 0;

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

std::uint8_t read_u8(const unsigned char *data) noexcept { return data[0]; }

bool decode_day_kind(std::uint8_t value, day_kind &result) noexcept {
  switch (value) {
  case 0:
    result = day_kind::exact;
    return true;

  case 1:
    result = day_kind::last_weekday;
    return true;

  case 2:
    result = day_kind::weekday_on_or_after;
    return true;

  case 3:
    result = day_kind::weekday_on_or_before;
    return true;

  default:
    return false;
  }
}

bool decode_time_basis(std::uint8_t value, time_basis &result) noexcept {
  switch (value) {
  case 0:
    result = time_basis::wall;
    return true;

  case 1:
    result = time_basis::standard;
    return true;

  case 2:
    result = time_basis::universal;
    return true;

  default:
    return false;
  }
}

bool decode_rules_kind(std::uint8_t value, rules_kind &result) noexcept {
  switch (value) {
  case 0:
    result = rules_kind::none;
    return true;

  case 1:
    result = rules_kind::fixed;
    return true;

  case 2:
    result = rules_kind::named;
    return true;

  default:
    return false;
  }
}

bool decode_day_spec(const unsigned char *record, std::size_t kind_offset,
                     std::size_t weekday_offset, std::size_t day_offset,
                     day_spec &result) noexcept {
  if (!decode_day_kind(read_u8(record + kind_offset), result.kind)) {
    return false;
  }

  result.weekday = read_i32(record + weekday_offset);

  result.day = read_i32(record + day_offset);

  return true;
}

const unsigned char *rule_record_at(unsigned index) noexcept {
  return record_at(table(format::table_id::rules), format::rule_record::size,
                   index);
}

const unsigned char *rule_set_record_at(unsigned index) noexcept {
  return record_at(table(format::table_id::rule_sets),
                   format::rule_set_record::size, index);
}

const unsigned char *era_record_at(unsigned index) noexcept {
  return record_at(table(format::table_id::eras), format::era_record::size,
                   index);
}

const unsigned char *zone_record_at(unsigned index) noexcept {
  return record_at(table(format::table_id::zones), format::zone_record::size,
                   index);
}

const unsigned char *link_record_at(unsigned index) noexcept {
  return record_at(table(format::table_id::links), format::link_record::size,
                   index);
}

const unsigned char *leap_record_at(unsigned index) noexcept {
  return record_at(table(format::table_id::leaps), format::leap_record::size,
                   index);
}

const unsigned char *transition_record_at(unsigned index) noexcept {
  return record_at(table(format::table_id::transitions),
                   format::transition_record::size, index);
}

const unsigned char *windows_record_at(unsigned index) noexcept {
  return record_at(table(format::table_id::windows_zones),
                   format::windows_zone_record::size, index);
}

long long saturating_add(long long value, long long adjustment) noexcept {
  constexpr long long minimum = std::numeric_limits<long long>::min();

  constexpr long long maximum = std::numeric_limits<long long>::max();

  if (adjustment > 0 && value > maximum - adjustment) {
    return maximum;
  }

  if (adjustment < 0 && value < minimum - adjustment) {
    return minimum;
  }

  return value + adjustment;
}

bool zone_offset_bounds(zone_ref zone, long long &minimum,
                        long long &maximum) noexcept {
  if (!zone)
    return false;

  const auto *record = zone_record_at(zone.index);

  if (record == nullptr)
    return false;

  const long long initial =
      read_i32(record + format::zone_record::initial_offset_seconds);

  minimum = initial;
  maximum = initial;

  const unsigned transition_begin =
      read_u32(record + format::zone_record::transition_begin);

  const unsigned transition_count =
      read_u32(record + format::zone_record::transition_count);

  for (unsigned index = 0; index < transition_count; ++index) {
    const auto *transition = transition_record_at(transition_begin + index);

    if (transition == nullptr)
      return false;

    const long long offset =
        read_i32(transition + format::transition_record::offset_seconds);

    if (offset < minimum)
      minimum = offset;

    if (offset > maximum)
      maximum = offset;
  }

  /*
   * Also account for every state the open-ended
   * final era can manufacture after the precomputed
   * transition table ends.
   */
  const auto era = zone_final_era(zone);

  if (!era)
    return false;

  auto include_offset = [&](long long offset) {
    if (offset < minimum)
      minimum = offset;

    if (offset > maximum)
      maximum = offset;
  };

  if (era.rules == rules_kind::none) {
    include_offset(era.standard_offset_seconds);

    return true;
  }

  if (era.rules == rules_kind::fixed) {
    include_offset(static_cast<long long>(era.standard_offset_seconds) +
                   era.fixed_save_seconds);

    return true;
  }

  if (era.rule_set == invalid_index) {
    return false;
  }

  /*
   * SAVE == 0 is a valid possible baseline even if
   * the final rule set happens not to contain an
   * explicit zero-save Rule in some future database.
   */
  include_offset(era.standard_offset_seconds);

  const auto set = rule_set_at(era.rule_set);

  if (!set)
    return false;

  for (unsigned index = 0; index < set.rule_count; ++index) {
    const auto rule = rule_set_rule_at(era.rule_set, index);

    if (!rule)
      return false;

    include_offset(static_cast<long long>(era.standard_offset_seconds) +
                   rule.save_seconds);
  }

  return true;
}

} // namespace

bool valid() noexcept {
  if (blob() == nullptr || blob_size() < format::header::size) {
    return false;
  }

  for (std::size_t index = 0; index < sizeof(format::magic); ++index) {
    if (blob()[format::header::magic + index] != format::magic[index]) {
      return false;
    }
  }

  if (read_u32(blob() + format::header::format_version) !=
      format::format_version) {
    return false;
  }

  if (read_u32(blob() + format::header::total_size) != blob_size()) {
    return false;
  }

  if (!span_valid(table(format::table_id::strings), 1))
    return false;

  if (!span_valid(table(format::table_id::rules), format::rule_record::size))
    return false;

  if (!span_valid(table(format::table_id::rule_sets),
                  format::rule_set_record::size))
    return false;

  if (!span_valid(table(format::table_id::eras), format::era_record::size))
    return false;

  if (!span_valid(table(format::table_id::transitions),
                  format::transition_record::size))
    return false;

  if (!span_valid(table(format::table_id::zones), format::zone_record::size))
    return false;

  if (!span_valid(table(format::table_id::links), format::link_record::size))
    return false;

  if (!span_valid(table(format::table_id::leaps), format::leap_record::size))
    return false;

  if (!span_valid(table(format::table_id::windows_zones),
                  format::windows_zone_record::size))
    return false;

  const auto strings = table(format::table_id::strings);

  const auto version_offset = read_u32(blob() + format::header::version_string);

  if (version_offset >= strings.count)
    return false;

  return true;
}

unsigned zone_count() noexcept { return table(format::table_id::zones).count; }

unsigned link_count() noexcept { return table(format::table_id::links).count; }

unsigned leap_count() noexcept { return table(format::table_id::leaps).count; }

unsigned rule_count() noexcept { return table(format::table_id::rules).count; }

unsigned rule_set_count() noexcept {
  return table(format::table_id::rule_sets).count;
}

unsigned era_count() noexcept { return table(format::table_id::eras).count; }

rule_definition rule_at(unsigned index) noexcept {
  const auto *record = rule_record_at(index);

  if (record == nullptr)
    return {};

  rule_definition result;

  const std::uint8_t to_max = read_u8(record + format::rule_record::to_max);

  const std::uint8_t save_is_daylight =
      read_u8(record + format::rule_record::save_is_daylight);

  if (to_max > 1 || save_is_daylight > 1)
    return {};

  if (!decode_day_spec(record, format::rule_record::day_kind,
                       format::rule_record::weekday, format::rule_record::day,
                       result.on)) {
    return {};
  }

  if (!decode_time_basis(read_u8(record + format::rule_record::at_basis),
                         result.at_basis)) {
    return {};
  }

  result.from_year = read_i32(record + format::rule_record::from_year);

  result.to_year = read_i32(record + format::rule_record::to_year);

  result.to_max = to_max != 0;

  result.month = read_i32(record + format::rule_record::month);

  result.at_seconds = read_i32(record + format::rule_record::at_seconds);

  result.save_seconds = read_i32(record + format::rule_record::save_seconds);

  result.save_is_daylight = save_is_daylight != 0;

  result.letters = string_at(read_u32(record + format::rule_record::letters));

  if (result.letters == nullptr)
    return {};

  result.valid = true;
  return result;
}

rule_set_definition rule_set_at(unsigned index) noexcept {
  const auto *record = rule_set_record_at(index);

  if (record == nullptr)
    return {};

  rule_set_definition result;

  result.name = string_at(read_u32(record + format::rule_set_record::name));

  result.rule_begin = read_u32(record + format::rule_set_record::rule_begin);

  result.rule_count = read_u32(record + format::rule_set_record::rule_count);

  if (result.name == nullptr)
    return {};

  const unsigned total = rule_count();

  if (result.rule_begin > total ||
      result.rule_count > total - result.rule_begin) {
    return {};
  }

  result.valid = true;
  return result;
}

era_definition era_at(unsigned index) noexcept {
  const auto *record = era_record_at(index);

  if (record == nullptr)
    return {};

  era_definition result;

  if (!decode_rules_kind(read_u8(record + format::era_record::rules),
                         result.rules)) {
    return {};
  }

  const unsigned rule_set = read_u32(record + format::era_record::rule_set);

  if (result.rules == rules_kind::named) {
    if (rule_set >= rule_set_count())
      return {};

    result.rule_set = rule_set;
  } else {
    if (rule_set != format::no_rule_set)
      return {};

    result.rule_set = invalid_index;
  }

  const std::uint8_t fixed_save_is_daylight =
      read_u8(record + format::era_record::fixed_save_is_daylight);

  const std::uint8_t has_until =
      read_u8(record + format::era_record::has_until);

  if (fixed_save_is_daylight > 1 || has_until > 1)
    return {};

  result.standard_offset_seconds =
      read_i32(record + format::era_record::standard_offset_seconds);

  result.fixed_save_seconds =
      read_i32(record + format::era_record::fixed_save_seconds);

  result.fixed_save_is_daylight = fixed_save_is_daylight != 0;

  result.format = string_at(read_u32(record + format::era_record::format));

  if (result.format == nullptr)
    return {};

  result.has_until = has_until != 0;

  result.until_year = read_i32(record + format::era_record::until_year);

  result.until_month = read_i32(record + format::era_record::until_month);

  if (!decode_day_spec(record, format::era_record::until_day_kind,
                       format::era_record::until_weekday,
                       format::era_record::until_day, result.until_day)) {
    return {};
  }

  result.until_seconds = read_i32(record + format::era_record::until_seconds);

  if (!decode_time_basis(read_u8(record + format::era_record::until_basis),
                         result.until_basis)) {
    return {};
  }

  result.valid = true;
  return result;
}

rule_definition rule_set_rule_at(unsigned rule_set_index,
                                 unsigned relative_index) noexcept {
  const auto set = rule_set_at(rule_set_index);

  if (!set || relative_index >= set.rule_count)
    return {};

  return rule_at(set.rule_begin + relative_index);
}

unsigned zone_era_count(zone_ref zone) noexcept {
  if (!zone)
    return 0;

  const auto *record = zone_record_at(zone.index);

  if (record == nullptr)
    return 0;

  const unsigned begin = read_u32(record + format::zone_record::era_begin);

  const unsigned count = read_u32(record + format::zone_record::era_count);

  const unsigned total = era_count();

  if (begin > total || count > total - begin)
    return 0;

  return count;
}

era_definition zone_era_at(zone_ref zone, unsigned relative_index) noexcept {
  if (!zone)
    return {};

  const auto *record = zone_record_at(zone.index);

  if (record == nullptr)
    return {};

  const unsigned begin = read_u32(record + format::zone_record::era_begin);

  const unsigned count = zone_era_count(zone);

  if (relative_index >= count)
    return {};

  return era_at(begin + relative_index);
}

era_definition zone_final_era(zone_ref zone) noexcept {
  const unsigned count = zone_era_count(zone);

  if (count == 0)
    return {};

  return zone_era_at(zone, count - 1);
}

long long leap_date_seconds(unsigned index) noexcept {
  const auto *record = leap_record_at(index);

  if (record == nullptr)
    return 0;

  return read_i64(record + format::leap_record::date_seconds);
}

int leap_value(unsigned index) noexcept {
  const auto *record = leap_record_at(index);

  if (record == nullptr)
    return 0;

  return read_i32(record + format::leap_record::value);
}

int leap_elapsed_at_sys(long long timestamp) noexcept {
  int elapsed = 0;

  for (unsigned index = 0; index < leap_count(); ++index) {
    const auto date = leap_date_seconds(index);

    if (timestamp < date)
      break;

    elapsed += leap_value(index);
  }

  return elapsed;
}

long long leap_expiration() noexcept {
  return read_i64(blob() + format::header::leap_expiration);
}

long long precomputed_until() noexcept {
  return read_i64(blob() + format::header::precomputed_until);
}

const char *version() noexcept {
  return string_at(read_u32(blob() + format::header::version_string));
}

unsigned windows_zone_count() noexcept {
  return table(format::table_id::windows_zones).count;
}

const char *windows_zone_target(const char *name, size_type length) noexcept {
  if (name == nullptr)
    return nullptr;

  unsigned first = 0;
  unsigned last = windows_zone_count();

  while (first < last) {
    const unsigned middle = first + (last - first) / 2;

    const auto *record = windows_record_at(middle);

    const char *windows_name =
        string_at(read_u32(record + format::windows_zone_record::windows_name));

    const int order = compare_text(name, length, windows_name);

    if (order < 0) {
      last = middle;
    } else if (order > 0) {
      first = middle + 1;
    } else {
      return string_at(
          read_u32(record + format::windows_zone_record::iana_name));
    }
  }

  return nullptr;
}

const char *windows_zone_target(const char *name) noexcept {
  if (name == nullptr)
    return nullptr;

  size_type length = 0;

  while (name[length] != '\0')
    ++length;

  return windows_zone_target(name, length);
}

zone_ref zone_at(unsigned index) noexcept {
  if (index >= zone_count())
    return {};

  return {index};
}

link_ref link_at(unsigned index) noexcept {
  if (index >= link_count())
    return {};

  return {index};
}

const char *zone_name(zone_ref zone) noexcept {
  if (!zone)
    return nullptr;

  const auto *record = zone_record_at(zone.index);

  if (record == nullptr)
    return nullptr;

  return string_at(read_u32(record + format::zone_record::name));
}

const char *link_name(link_ref link) noexcept {
  if (!link)
    return nullptr;

  const auto *record = link_record_at(link.index);

  if (record == nullptr)
    return nullptr;

  return string_at(read_u32(record + format::link_record::name));
}

zone_ref link_target(link_ref link) noexcept {
  if (!link)
    return {};

  const auto *record = link_record_at(link.index);

  if (record == nullptr)
    return {};

  const unsigned target = read_u32(record + format::link_record::target_zone);

  if (target >= zone_count())
    return {};

  return {target};
}

zone_ref find_zone(const char *name, size_type length) noexcept {
  if (name == nullptr)
    return {};

  unsigned first = 0;
  unsigned last = zone_count();

  while (first < last) {
    const unsigned middle = first + (last - first) / 2;

    const char *candidate = zone_name({middle});

    const int order = compare_text(name, length, candidate);

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

zone_ref find_zone(const char *name) noexcept {
  if (name == nullptr)
    return {};

  size_type length = 0;

  while (name[length] != '\0')
    ++length;

  return find_zone(name, length);
}

link_ref find_link(const char *name, size_type length) noexcept {
  if (name == nullptr)
    return {};

  unsigned first = 0;
  unsigned last = link_count();

  while (first < last) {
    const unsigned middle = first + (last - first) / 2;

    const char *candidate = link_name({middle});

    const int order = compare_text(name, length, candidate);

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

link_ref find_link(const char *name) noexcept {
  if (name == nullptr)
    return {};

  size_type length = 0;

  while (name[length] != '\0')
    ++length;

  return find_link(name, length);
}

zone_ref locate_zone(const char *name, size_type length) noexcept {
  if (const auto zone = find_zone(name, length)) {
    return zone;
  }

  if (const auto link = find_link(name, length)) {
    return link_target(link);
  }

  return {};
}

zone_ref locate_zone(const char *name) noexcept {
  if (name == nullptr)
    return {};

  size_type length = 0;

  while (name[length] != '\0')
    ++length;

  return locate_zone(name, length);
}

zone_interval interval_at(zone_ref zone, unsigned state_index) noexcept {
  if (!zone)
    return {};

  const auto *record = zone_record_at(zone.index);

  if (record == nullptr)
    return {};

  const unsigned transition_begin =
      read_u32(record + format::zone_record::transition_begin);

  const unsigned transition_count =
      read_u32(record + format::zone_record::transition_count);

  if (state_index > transition_count) {
    return {};
  }

  zone_interval result;
  result.valid = true;

  if (state_index == 0) {
    result.begin_unbounded = true;

    result.offset_seconds =
        read_i32(record + format::zone_record::initial_offset_seconds);

    result.save_minutes =
        read_i32(record + format::zone_record::initial_save_minutes);

    result.abbreviation =
        string_at(read_u32(record + format::zone_record::initial_abbreviation));
  } else {
    const auto *transition =
        transition_record_at(transition_begin + state_index - 1);

    if (transition == nullptr)
      return {};

    result.begin = read_i64(transition + format::transition_record::begin);

    result.offset_seconds =
        read_i32(transition + format::transition_record::offset_seconds);

    result.save_minutes =
        read_i32(transition + format::transition_record::save_minutes);

    result.abbreviation = string_at(
        read_u32(transition + format::transition_record::abbreviation));
  }

  if (state_index < transition_count) {
    const auto *next = transition_record_at(transition_begin + state_index);

    if (next == nullptr)
      return {};

    result.end = read_i64(next + format::transition_record::begin);
  } else {
    result.end = precomputed_until();

    result.end_is_horizon = true;
  }

  return result;
}

zone_interval lookup(zone_ref zone, long long timestamp) noexcept {
  if (!zone)
    return {};

  const auto *record = zone_record_at(zone.index);

  if (record == nullptr)
    return {};

  const unsigned transition_begin =
      read_u32(record + format::zone_record::transition_begin);

  const unsigned transition_count =
      read_u32(record + format::zone_record::transition_count);

  const long long horizon = precomputed_until();

  /*
   * Everything before the generated horizon continues
   * to use the exact precomputed transition table.
   *
   * Do not involve the tail evaluator in choosing the
   * state for an already-covered timestamp.
   */
  if (timestamp < horizon) {
    unsigned first = 0;
    unsigned last = transition_count;

    while (first < last) {
      const unsigned middle = first + (last - first) / 2;

      const auto *transition = transition_record_at(transition_begin + middle);

      if (transition == nullptr)
        return {};

      const long long begin =
          read_i64(transition + format::transition_record::begin);

      if (begin <= timestamp)
        first = middle + 1;
      else
        last = middle;
    }

    auto result = interval_at(zone, first);

    if (!result)
      return {};

    /*
     * Normal precomputed intervals are already exact.
     */
    if (!result.end_is_horizon)
      return result;

    /*
     * Only the END of the final precomputed interval is
     * artificial.
     *
     * Ask the tail evaluator what state follows the
     * horizon. If its first interval is still the same
     * state, use that interval's real end while retaining
     * the exact precomputed beginning/state.
     */
    const auto tail = ::ftl_tzdb_tail::lookup(zone, horizon, result);

    if (!tail)
      return {};

    result.end = tail.end;
    result.end_is_horizon = false;

    return result;
  }

  /*
   * Post-horizon lookup starts from the exact state that
   * the generated transition table established at the
   * horizon.
   */
  const auto seed = interval_at(zone, transition_count);

  if (!seed)
    return {};

  return ::ftl_tzdb_tail::lookup(zone, timestamp, seed);
}

local_lookup_result lookup_local(zone_ref zone, long long timestamp) noexcept {
  if (!zone)
    return {};

  long long minimum_offset = 0;
  long long maximum_offset = 0;

  if (!zone_offset_bounds(zone, minimum_offset, maximum_offset)) {
    return {};
  }

  /*
   * For:
   *
   *   local = sys + offset
   *
   * every possible sys interpretation lies in:
   *
   *   [local - max_offset,
   *    local - min_offset]
   *
   * This lets local lookup operate over the normal
   * sys lookup API and therefore works seamlessly on
   * both sides of the precompute horizon.
   */
  long long first_system = saturating_add(timestamp, -maximum_offset);

  long long last_system = saturating_add(timestamp, -minimum_offset);

  if (first_system > last_system) {
    const long long temporary = first_system;

    first_system = last_system;
    last_system = temporary;
  }

  local_lookup_result result;

  unsigned matches = 0;

  bool have_previous = false;
  zone_interval previous;

  bool have_gap = false;
  zone_interval gap_first;
  zone_interval gap_second;

  zone_interval interval = lookup(zone, first_system);

  if (!interval)
    return {};

  for (;;) {
    const long long local_begin =
        interval.begin_unbounded
            ? std::numeric_limits<long long>::min()
            : saturating_add(interval.begin, interval.offset_seconds);

    const long long local_end =
        interval.end == std::numeric_limits<long long>::max()
            ? std::numeric_limits<long long>::max()
            : saturating_add(interval.end, interval.offset_seconds);

    if (timestamp >= local_begin && timestamp < local_end) {
      if (matches == 0) {
        result.first = interval;
      } else if (matches == 1) {
        result.second = interval;
      } else {
        return {};
      }

      ++matches;
    }

    if (have_previous && !interval.begin_unbounded) {
      /*
       * Both sides of a system transition share the
       * same system instant, but map that instant to
       * different local boundaries.
       */
      const long long previous_local_end =
          saturating_add(interval.begin, previous.offset_seconds);

      const long long next_local_begin =
          saturating_add(interval.begin, interval.offset_seconds);

      if (previous_local_end < next_local_begin &&
          timestamp >= previous_local_end && timestamp < next_local_begin) {
        have_gap = true;
        gap_first = previous;
        gap_second = interval;
      }
    }

    if (interval.end == std::numeric_limits<long long>::max()) {
      break;
    }

    /*
     * No later system instant can represent this
     * local timestamp once we've passed the complete
     * offset-derived candidate range.
     */
    if (interval.end > last_system) {
      break;
    }

    previous = interval;
    have_previous = true;

    const long long next_time = interval.end;

    auto next = lookup(zone, next_time);

    if (!next)
      return {};

    /*
     * Corrupted/non-progressing interval topology
     * must not turn this into an infinite loop.
     */
    if (next.end <= next_time &&
        next.end != std::numeric_limits<long long>::max()) {
      return {};
    }

    interval = next;
  }

  if (matches == 1) {
    result.result = local_result_kind::unique;

    return result;
  }

  if (matches == 2) {
    result.result = local_result_kind::ambiguous;

    return result;
  }

  if (have_gap) {
    result.result = local_result_kind::nonexistent;

    result.first = gap_first;
    result.second = gap_second;

    return result;
  }

  return {};
}

} // namespace ftl_tzdb_runtime_core
