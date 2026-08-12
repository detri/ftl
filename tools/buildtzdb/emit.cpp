#include "emit.hpp"

#include <ftl/detail/tzdb_blob_format.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ftl_tzdb_tool {
namespace {

namespace blob_format = ::ftl_tzdb_blob_format;

struct table_span {
  std::uint32_t offset = 0;
  std::uint32_t count = 0;
};

class string_table {
public:
  void add(const std::string &value) { values_.insert(value); }

  void build() {
    unsigned offset = 0;

    for (const auto &value : values_) {
      offsets_.emplace(value, offset);

      data_.append(value);
      data_.push_back('\0');

      offset += static_cast<unsigned>(value.size() + 1);
    }
  }

  unsigned offset(const std::string &value) const {
    const auto it = offsets_.find(value);

    if (it == offsets_.end()) {
      throw std::runtime_error("string was not interned: " + value);
    }

    return it->second;
  }

  const std::string &data() const noexcept { return data_; }

private:
  std::set<std::string> values_;

  std::unordered_map<std::string, unsigned> offsets_;

  std::string data_;
};

class blob_writer {
public:
  explicit blob_writer(std::size_t initial_size) : bytes_(initial_size, 0) {}

  std::size_t size() const noexcept { return bytes_.size(); }

  const std::vector<unsigned char> &bytes() const noexcept { return bytes_; }

  void align(std::size_t alignment) {
    while ((bytes_.size() % alignment) != 0)
      bytes_.push_back(0);
  }

  void write_u8(std::uint8_t value) { bytes_.push_back(value); }

  void write_u32(std::uint32_t value) {
    bytes_.push_back(static_cast<unsigned char>(value & 0xffu));

    bytes_.push_back(static_cast<unsigned char>((value >> 8) & 0xffu));

    bytes_.push_back(static_cast<unsigned char>((value >> 16) & 0xffu));

    bytes_.push_back(static_cast<unsigned char>((value >> 24) & 0xffu));
  }

  void write_i32(std::int32_t value) {
    write_u32(static_cast<std::uint32_t>(value));
  }

  void write_u64(std::uint64_t value) {
    for (unsigned shift = 0; shift != 64; shift += 8) {
      bytes_.push_back(static_cast<unsigned char>((value >> shift) & 0xffu));
    }
  }

  void write_i64(std::int64_t value) {
    write_u64(static_cast<std::uint64_t>(value));
  }

  void write_bytes(const void *data, std::size_t size) {
    const auto *first = static_cast<const unsigned char *>(data);

    bytes_.insert(bytes_.end(), first, first + size);
  }

  void patch_u32(std::size_t offset, std::uint32_t value) {
    if (offset + 4 > bytes_.size())
      throw std::runtime_error("blob u32 patch outside buffer");

    for (unsigned byte = 0; byte != 4; ++byte) {
      bytes_[offset + byte] =
          static_cast<unsigned char>((value >> (byte * 8)) & 0xffu);
    }
  }

  void patch_i64(std::size_t offset, std::int64_t value) {
    const auto bits = static_cast<std::uint64_t>(value);

    if (offset + 8 > bytes_.size())
      throw std::runtime_error("blob i64 patch outside buffer");

    for (unsigned byte = 0; byte != 8; ++byte) {
      bytes_[offset + byte] =
          static_cast<unsigned char>((bits >> (byte * 8)) & 0xffu);
    }
  }

  void patch_bytes(std::size_t offset, const void *data, std::size_t size) {
    if (offset + size > bytes_.size())
      throw std::runtime_error("blob byte patch outside buffer");

    const auto *source = static_cast<const unsigned char *>(data);

    std::copy(source, source + size,
              bytes_.begin() + static_cast<std::ptrdiff_t>(offset));
  }

private:
  std::vector<unsigned char> bytes_;
};

std::uint8_t encode_day_kind(day_kind kind) {
  switch (kind) {
  case day_kind::exact:
    return 0;

  case day_kind::last_weekday:
    return 1;

  case day_kind::weekday_on_or_after:
    return 2;

  case day_kind::weekday_on_or_before:
    return 3;
  }

  throw std::runtime_error("invalid day kind");
}

std::uint8_t encode_time_basis(time_basis basis) {
  switch (basis) {
  case time_basis::wall:
    return 0;

  case time_basis::standard:
    return 1;

  case time_basis::universal:
    return 2;
  }

  throw std::runtime_error("invalid time basis");
}

std::uint8_t encode_rules_kind(rules_kind kind) {
  switch (kind) {
  case rules_kind::none:
    return 0;

  case rules_kind::fixed:
    return 1;

  case rules_kind::named:
    return 2;
  }

  throw std::runtime_error("invalid rules kind");
}

void write_day(blob_writer &writer, const day_spec &day) {
  writer.write_u8(encode_day_kind(day.kind));

  writer.write_i32(day.weekday);
  writer.write_i32(day.day);
}

void require_record_size(const blob_writer &writer, std::size_t begin,
                         std::size_t expected, const char *name) {
  const auto actual = writer.size() - begin;

  if (actual != expected) {
    throw std::runtime_error(std::string("internal ") + name +
                             " record size mismatch");
  }
}

std::uint32_t checked_u32(std::size_t value, const char *what) {
  if (value > std::numeric_limits<std::uint32_t>::max()) {
    throw std::runtime_error(std::string(what) + " exceeds blob format limit");
  }

  return static_cast<std::uint32_t>(value);
}

void write_text_if_changed(const std::filesystem::path &path,
                           const std::string &contents) {
  {
    std::ifstream input(path, std::ios::binary);

    if (input) {
      const std::string old{
          std::istreambuf_iterator<char>{input},
          std::istreambuf_iterator<char>{},
      };

      if (old == contents)
        return;
    }
  }

  std::filesystem::create_directories(path.parent_path());

  std::ofstream output(path, std::ios::binary | std::ios::trunc);

  if (!output) {
    throw std::runtime_error("unable to write " + path.string());
  }

  output.write(contents.data(), static_cast<std::streamsize>(contents.size()));
}

void write_binary_if_changed(const std::filesystem::path &path,
                             const std::vector<unsigned char> &contents) {
  {
    std::ifstream input(path, std::ios::binary);

    if (input) {
      const std::vector<unsigned char> old{
          std::istreambuf_iterator<char>{input},
          std::istreambuf_iterator<char>{},
      };

      if (old == contents)
        return;
    }
  }

  std::filesystem::create_directories(path.parent_path());

  std::ofstream output(path, std::ios::binary | std::ios::trunc);

  if (!output) {
    throw std::runtime_error("unable to write " + path.string());
  }

  output.write(reinterpret_cast<const char *>(contents.data()),
               static_cast<std::streamsize>(contents.size()));
}

void write_blob_source(const std::filesystem::path &path,
                       const std::vector<unsigned char> &blob) {
  static constexpr char digits[] = "0123456789abcdef";

  std::ostringstream out;

  out <<
      R"(// Generated by tools/buildtzdb. Do not edit.

#include <ftl/detail/tzdb_blob.hpp>

namespace {

alignas(8)
const unsigned char tzdb_blob[] = {
)";

  constexpr std::size_t per_line = 12;

  for (std::size_t index = 0; index < blob.size(); ++index) {
    if ((index % per_line) == 0)
      out << "    ";

    const unsigned value = blob[index];

    out << "0x" << digits[(value >> 4) & 0xf] << digits[value & 0xf] << ',';

    if ((index % per_line) == per_line - 1 || index + 1 == blob.size()) {
      out << '\n';
    } else {
      out << ' ';
    }
  }

  out <<
      R"(};

} // namespace

namespace ftl_tzdb_blob {

const unsigned char *data() noexcept {
  return tzdb_blob;
}

size_type size() noexcept {
  return sizeof(tzdb_blob);
}

} // namespace ftl_tzdb_blob
)";

  write_text_if_changed(path, out.str());
}

} // namespace

void emit_database(const database &input, const compiled_database &compiled,
                   const std::vector<windows_mapping> &windows_mappings,
                   const std::filesystem::path &blob_output,
                   const std::filesystem::path &source_output) {
  database db = input;

  for (const auto &leap : db.leaps) {
    if (leap.rolling) {
      throw std::runtime_error("rolling leap seconds cannot "
                               "be represented by the C++ "
                               "tzdb leap_second model");
    }
  }

  std::sort(db.zones.begin(), db.zones.end(),
            [](const zone &left, const zone &right) {
              return left.name < right.name;
            });

  for (std::size_t index = 1; index < db.zones.size(); ++index) {
    if (db.zones[index - 1].name == db.zones[index].name) {
      throw std::runtime_error("duplicate Zone: " + db.zones[index].name);
    }
  }

  if (compiled.zones.size() != db.zones.size()) {
    throw std::runtime_error("compiled zone count does not "
                             "match parsed zone count");
  }

  for (std::size_t index = 0; index < db.zones.size(); ++index) {
    const auto &raw = db.zones[index];

    const auto &compiled_zone = compiled.zones[index];

    if (raw.name != compiled_zone.name) {
      throw std::runtime_error("compiled zone order disagrees "
                               "with parsed zone order at '" +
                               raw.name + "'");
    }

    if (compiled_zone.precomputed_until != compiled.precomputed_until) {
      throw std::runtime_error("zone '" + compiled_zone.name +
                               "' has inconsistent transition "
                               "horizon");
    }

    bool have_previous = false;
    std::int64_t previous = 0;

    for (const auto &transition : compiled_zone.transitions) {
      if (transition.at >= compiled.precomputed_until) {
        throw std::runtime_error("zone '" + compiled_zone.name +
                                 "' contains transition at or "
                                 "beyond horizon");
      }

      if (have_previous && transition.at <= previous) {
        throw std::runtime_error("zone '" + compiled_zone.name +
                                 "' has non-increasing "
                                 "compiled transitions");
      }

      previous = transition.at;
      have_previous = true;
    }
  }

  std::unordered_map<std::string, unsigned> zone_indices;

  for (unsigned index = 0; index < static_cast<unsigned>(db.zones.size());
       ++index) {
    zone_indices.emplace(db.zones[index].name, index);
  }

  std::map<std::string, std::vector<rule>> rule_sets;

  for (const auto &rule : db.rules)
    rule_sets[rule.name].push_back(rule);

  for (auto &[name, rules] : rule_sets) {
    (void)name;

    std::sort(rules.begin(), rules.end(),
              [](const rule &left, const rule &right) {
                if (left.from_year != right.from_year)
                  return left.from_year < right.from_year;

                if (left.month != right.month)
                  return left.month < right.month;

                if (left.on.kind != right.on.kind)
                  return left.on.kind < right.on.kind;

                if (left.on.weekday != right.on.weekday)
                  return left.on.weekday < right.on.weekday;

                if (left.on.day != right.on.day)
                  return left.on.day < right.on.day;

                return left.at.seconds < right.at.seconds;
              });
  }

  std::unordered_map<std::string, unsigned> rule_set_indices;

  {
    unsigned index = 0;

    for (const auto &[name, rules] : rule_sets) {
      (void)rules;

      rule_set_indices.emplace(name, index++);
    }
  }

  for (const auto &zone : db.zones) {
    for (const auto &era : zone.eras) {
      if (era.rules != rules_kind::named)
        continue;

      if (!rule_set_indices.contains(era.rule_name)) {
        throw std::runtime_error("Zone '" + zone.name +
                                 "' references unknown rule "
                                 "set '" +
                                 era.rule_name + "'");
      }
    }
  }

  std::unordered_map<std::string, std::string> raw_links;

  for (const auto &link : db.links) {
    if (!raw_links.emplace(link.name, link.target).second) {
      throw std::runtime_error("duplicate Link: " + link.name);
    }

    if (zone_indices.contains(link.name)) {
      throw std::runtime_error("Link collides with Zone: " + link.name);
    }
  }

  struct resolved_link {
    std::string name;
    unsigned target_zone = 0;
  };

  std::vector<resolved_link> links;
  links.reserve(db.links.size());

  for (const auto &link : db.links) {
    std::string target = link.target;

    std::unordered_set<std::string> seen;

    for (;;) {
      const auto zone_it = zone_indices.find(target);

      if (zone_it != zone_indices.end()) {
        links.push_back({
            link.name,
            zone_it->second,
        });

        break;
      }

      if (!seen.insert(target).second) {
        throw std::runtime_error("Link cycle involving " + target);
      }

      const auto link_it = raw_links.find(target);

      if (link_it == raw_links.end()) {
        throw std::runtime_error("Link '" + link.name +
                                 "' resolves to missing "
                                 "target '" +
                                 target + "'");
      }

      target = link_it->second;
    }
  }

  std::sort(links.begin(), links.end(),
            [](const resolved_link &left, const resolved_link &right) {
              return left.name < right.name;
            });

  std::sort(db.leaps.begin(), db.leaps.end(),
            [](const leap &left, const leap &right) {
              return left.date_seconds < right.date_seconds;
            });

  string_table strings;

  strings.add(db.version);

  for (const auto &zone : db.zones) {
    strings.add(zone.name);

    for (const auto &era : zone.eras)
      strings.add(era.format);
  }

  for (const auto &[name, rules] : rule_sets) {
    strings.add(name);

    for (const auto &rule : rules)
      strings.add(rule.letters);
  }

  for (const auto &link : links)
    strings.add(link.name);

  for (const auto &zone : compiled.zones) {
    strings.add(zone.initial.abbreviation);

    for (const auto &transition : zone.transitions) {
      strings.add(transition.state.abbreviation);
    }
  }

  for (const auto &mapping : windows_mappings) {
    strings.add(mapping.windows_name);
    strings.add(mapping.iana_name);
  }

  strings.build();

  struct flattened_rule_set {
    std::string name;
    unsigned begin = 0;
    unsigned count = 0;
  };

  std::vector<flattened_rule_set> flat_rule_sets;

  std::vector<rule> flat_rules;

  for (const auto &[name, rules] : rule_sets) {
    flattened_rule_set set;

    set.name = name;
    set.begin = static_cast<unsigned>(flat_rules.size());

    set.count = static_cast<unsigned>(rules.size());

    flat_rules.insert(flat_rules.end(), rules.begin(), rules.end());

    flat_rule_sets.push_back(static_cast<flattened_rule_set &&>(set));
  }

  struct flattened_zone {
    std::string name;

    unsigned era_begin = 0;
    unsigned era_count = 0;

    unsigned transition_begin = 0;
    unsigned transition_count = 0;

    compiled_state initial;
  };

  std::vector<flattened_zone> flat_zones;

  std::vector<zone_era> flat_eras;

  unsigned transition_begin = 0;

  for (std::size_t index = 0; index < db.zones.size(); ++index) {
    const auto &zone = db.zones[index];

    const auto &compiled_zone = compiled.zones[index];

    flattened_zone result;

    result.name = zone.name;

    result.era_begin = static_cast<unsigned>(flat_eras.size());

    result.era_count = static_cast<unsigned>(zone.eras.size());

    result.transition_begin = transition_begin;

    result.transition_count =
        static_cast<unsigned>(compiled_zone.transitions.size());

    result.initial = compiled_zone.initial;

    transition_begin += result.transition_count;

    flat_eras.insert(flat_eras.end(), zone.eras.begin(), zone.eras.end());

    flat_zones.push_back(static_cast<flattened_zone &&>(result));
  }

  using blob_format::table_id;

  std::array<table_span, static_cast<std::size_t>(table_id::count)> spans{};

  blob_writer writer(blob_format::header::size);

  auto begin_table = [&](table_id table, std::size_t count) {
    writer.align(8);

    auto &span = spans[static_cast<std::size_t>(table)];

    span.offset = checked_u32(writer.size(), "table offset");

    span.count = checked_u32(count, "table count");
  };

  begin_table(table_id::strings, strings.data().size());

  writer.write_bytes(strings.data().data(), strings.data().size());

  begin_table(table_id::rules, flat_rules.size());

  for (const auto &rule : flat_rules) {
    const auto begin = writer.size();

    writer.write_i32(rule.from_year);
    writer.write_i32(rule.to_year);

    writer.write_u8(rule.to_max ? 1 : 0);

    writer.write_i32(rule.month);

    write_day(writer, rule.on);

    writer.write_i32(rule.at.seconds);

    writer.write_u8(encode_time_basis(rule.at.basis));

    writer.write_i32(rule.save.seconds);

    writer.write_u8(rule.save.daylight ? 1 : 0);

    writer.write_u32(strings.offset(rule.letters));

    require_record_size(writer, begin, blob_format::rule_record::size, "rule");
  }

  begin_table(table_id::rule_sets, flat_rule_sets.size());

  for (const auto &set : flat_rule_sets) {
    const auto begin = writer.size();

    writer.write_u32(strings.offset(set.name));

    writer.write_u32(set.begin);
    writer.write_u32(set.count);

    require_record_size(writer, begin, blob_format::rule_set_record::size,
                        "rule-set");
  }

  begin_table(table_id::eras, flat_eras.size());

  for (const auto &era : flat_eras) {
    const auto begin = writer.size();

    writer.write_i32(era.standard_offset_seconds);

    writer.write_u8(encode_rules_kind(era.rules));

    if (era.rules == rules_kind::named) {
      writer.write_u32(rule_set_indices.at(era.rule_name));
    } else {
      writer.write_u32(blob_format::no_rule_set);
    }

    writer.write_i32(era.fixed_save.seconds);

    writer.write_u8(era.fixed_save.daylight ? 1 : 0);

    writer.write_u32(strings.offset(era.format));

    writer.write_u8(era.until ? 1 : 0);

    if (era.until) {
      writer.write_i32(era.until->year);

      writer.write_i32(era.until->month);

      write_day(writer, era.until->day);

      writer.write_i32(era.until->time.seconds);

      writer.write_u8(encode_time_basis(era.until->time.basis));
    } else {
      writer.write_i32(0);
      writer.write_i32(1);

      writer.write_u8(encode_day_kind(day_kind::exact));

      writer.write_i32(0);
      writer.write_i32(1);

      writer.write_i32(0);

      writer.write_u8(encode_time_basis(time_basis::wall));
    }

    require_record_size(writer, begin, blob_format::era_record::size, "era");
  }

  std::size_t transition_count = 0;

  for (const auto &zone : compiled.zones) {
    transition_count += zone.transitions.size();
  }

  begin_table(table_id::transitions, transition_count);

  for (const auto &zone : compiled.zones) {
    for (const auto &transition : zone.transitions) {
      const auto begin = writer.size();

      writer.write_i64(transition.at);

      writer.write_i32(transition.state.offset_seconds);

      writer.write_i32(transition.state.save_minutes);

      writer.write_u32(strings.offset(transition.state.abbreviation));

      require_record_size(writer, begin, blob_format::transition_record::size,
                          "transition");
    }
  }

  begin_table(table_id::zones, flat_zones.size());

  for (const auto &zone : flat_zones) {
    const auto begin = writer.size();

    writer.write_u32(strings.offset(zone.name));

    writer.write_u32(zone.era_begin);

    writer.write_u32(zone.era_count);

    writer.write_u32(zone.transition_begin);

    writer.write_u32(zone.transition_count);

    writer.write_i32(zone.initial.offset_seconds);

    writer.write_i32(zone.initial.save_minutes);

    writer.write_u32(strings.offset(zone.initial.abbreviation));

    require_record_size(writer, begin, blob_format::zone_record::size, "zone");
  }

  begin_table(table_id::links, links.size());

  for (const auto &link : links) {
    const auto begin = writer.size();

    writer.write_u32(strings.offset(link.name));

    writer.write_u32(link.target_zone);

    require_record_size(writer, begin, blob_format::link_record::size, "link");
  }

  begin_table(table_id::leaps, db.leaps.size());

  for (const auto &leap : db.leaps) {
    const auto begin = writer.size();

    writer.write_i64(leap.date_seconds);

    writer.write_i32(leap.correction);

    require_record_size(writer, begin, blob_format::leap_record::size, "leap");
  }

  begin_table(table_id::windows_zones, windows_mappings.size());

  for (const auto &mapping : windows_mappings) {
    const auto begin = writer.size();

    writer.write_u32(strings.offset(mapping.windows_name));

    writer.write_u32(strings.offset(mapping.iana_name));

    require_record_size(writer, begin, blob_format::windows_zone_record::size,
                        "Windows-zone");
  }

  if (writer.size() > std::numeric_limits<std::uint32_t>::max()) {
    throw std::runtime_error("tzdb blob exceeds 4 GiB");
  }

  writer.patch_bytes(blob_format::header::magic, blob_format::magic,
                     sizeof(blob_format::magic));

  writer.patch_u32(blob_format::header::format_version,
                   blob_format::format_version);

  writer.patch_u32(blob_format::header::total_size,
                   static_cast<std::uint32_t>(writer.size()));

  writer.patch_u32(blob_format::header::version_string,
                   strings.offset(db.version));

  writer.patch_u32(blob_format::header::reserved, 0);

  writer.patch_i64(blob_format::header::precomputed_until,
                   compiled.precomputed_until);

  writer.patch_i64(blob_format::header::leap_expiration,
                   db.leap_expiration ? *db.leap_expiration : 0);

  for (std::size_t index = 0; index < static_cast<std::size_t>(table_id::count);
       ++index) {
    const auto table = static_cast<table_id>(index);

    const auto offset = blob_format::table_span_offset(table);

    writer.patch_u32(offset, spans[index].offset);

    writer.patch_u32(offset + 4, spans[index].count);
  }

  write_binary_if_changed(blob_output, writer.bytes());

  write_blob_source(source_output, writer.bytes());
}

} // namespace ftl_tzdb_tool
