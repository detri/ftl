#include <ftl/detail/tzdb_blob.hpp>
#include <ftl/detail/tzdb_blob_format.hpp>
#include <ftl/detail/tzdb_core.hpp>

#include <cstdint>

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
  if (!zone || timestamp >= precomputed_until()) {
    return {};
  }

  const auto *record = zone_record_at(zone.index);

  if (record == nullptr)
    return {};

  const unsigned transition_begin =
      read_u32(record + format::zone_record::transition_begin);

  const unsigned transition_count =
      read_u32(record + format::zone_record::transition_count);

  unsigned first = 0;
  unsigned last = transition_count;

  while (first < last) {
    const unsigned middle = first + (last - first) / 2;

    const auto *transition = transition_record_at(transition_begin + middle);

    const long long begin =
        read_i64(transition + format::transition_record::begin);

    if (begin <= timestamp)
      first = middle + 1;
    else
      last = middle;
  }

  return interval_at(zone, first);
}

local_lookup_result lookup_local(zone_ref zone, long long timestamp) noexcept {
  if (!zone)
    return {};

  const auto *record = zone_record_at(zone.index);

  if (record == nullptr)
    return {};

  const unsigned transition_count =
      read_u32(record + format::zone_record::transition_count);

  local_lookup_result result;

  unsigned matches = 0;

  for (unsigned state = 0; state <= transition_count; ++state) {
    const auto interval = interval_at(zone, state);

    if (!interval)
      return {};

    const bool after_begin =
        interval.begin_unbounded ||
        timestamp >= interval.begin + interval.offset_seconds;

    const bool before_end = timestamp < interval.end + interval.offset_seconds;

    if (!after_begin || !before_end) {
      continue;
    }

    if (matches == 0) {
      result.first = interval;
    } else if (matches == 1) {
      result.second = interval;
    } else {
      return {};
    }

    ++matches;
  }

  if (matches == 1) {
    result.result = local_result_kind::unique;

    return result;
  }

  if (matches == 2) {
    result.result = local_result_kind::ambiguous;

    return result;
  }

  for (unsigned state = 1; state <= transition_count; ++state) {
    const auto previous = interval_at(zone, state - 1);

    const auto next = interval_at(zone, state);

    if (!previous || !next)
      return {};

    const long long previous_local_end = next.begin + previous.offset_seconds;

    const long long next_local_begin = next.begin + next.offset_seconds;

    if (previous_local_end >= next_local_begin) {
      continue;
    }

    if (timestamp >= previous_local_end && timestamp < next_local_begin) {
      result.result = local_result_kind::nonexistent;

      result.first = previous;
      result.second = next;

      return result;
    }
  }

  return {};
}

} // namespace ftl_tzdb_runtime_core
