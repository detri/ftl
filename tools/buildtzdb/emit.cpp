#include "emit.hpp"

#include <algorithm>
#include <fstream>
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

    if (it == offsets_.end())
      throw std::runtime_error("string was not interned: " + value);

    return it->second;
  }

  const std::string &data() const noexcept { return data_; }

private:
  std::set<std::string> values_;
  std::unordered_map<std::string, unsigned> offsets_;
  std::string data_;
};

std::string escape_string(std::string_view value) {
  std::ostringstream out;

  for (unsigned char c : value) {
    if (c >= 0x20 && c <= 0x7e && c != '"' && c != '\\') {

      out << static_cast<char>(c);
      continue;
    }

    if (c == '"') {
      out << "\\\"";
      continue;
    }

    if (c == '\\') {
      out << "\\\\";
      continue;
    }

    const char digits[] = "01234567";

    out << '\\' << digits[(c >> 6) & 7] << digits[(c >> 3) & 7]
        << digits[c & 7];
  }

  return out.str();
}

const char *day_kind_name(day_kind kind) {
  switch (kind) {
  case day_kind::exact:
    return "day_kind::exact";
  case day_kind::last_weekday:
    return "day_kind::last_weekday";
  case day_kind::weekday_on_or_after:
    return "day_kind::weekday_on_or_after";
  case day_kind::weekday_on_or_before:
    return "day_kind::weekday_on_or_before";
  }

  throw std::runtime_error("invalid day kind");
}

const char *time_basis_name(time_basis basis) {
  switch (basis) {
  case time_basis::wall:
    return "time_basis::wall";
  case time_basis::standard:
    return "time_basis::standard";
  case time_basis::universal:
    return "time_basis::universal";
  }

  throw std::runtime_error("invalid time basis");
}

const char *rules_kind_name(rules_kind kind) {
  switch (kind) {
  case rules_kind::none:
    return "rules_kind::none";
  case rules_kind::fixed:
    return "rules_kind::fixed";
  case rules_kind::named:
    return "rules_kind::named";
  }

  throw std::runtime_error("invalid rules kind");
}

void emit_day(std::ostream &out, const day_spec &day) {

  out << "{ " << day_kind_name(day.kind) << ", " << day.weekday << ", "
      << day.day << " }";
}

void write_if_changed(const std::filesystem::path &path,
                      const std::string &contents) {

  {
    std::ifstream input(path, std::ios::binary);

    if (input) {
      std::string old((std::istreambuf_iterator<char>(input)),
                      std::istreambuf_iterator<char>());

      if (old == contents)
        return;
    }
  }

  std::filesystem::create_directories(path.parent_path());

  std::ofstream output(path, std::ios::binary | std::ios::trunc);

  if (!output)
    throw std::runtime_error("unable to write " + path.string());

  output.write(contents.data(), static_cast<std::streamsize>(contents.size()));
}

} // namespace

void emit_database(const database &input,
                   const std::filesystem::path &output_path) {

  database db = input;

  for (const auto &leap : db.leaps) {
    if (leap.rolling)
      throw std::runtime_error("rolling leap seconds cannot be represented by "
                               "the C++ tzdb leap_second model");
  }

  std::sort(db.zones.begin(), db.zones.end(),
            [](const zone &a, const zone &b) { return a.name < b.name; });

  for (std::size_t i = 1; i < db.zones.size(); ++i) {
    if (db.zones[i - 1].name == db.zones[i].name)
      throw std::runtime_error("duplicate Zone: " + db.zones[i].name);
  }

  std::unordered_map<std::string, unsigned> zone_indices;

  for (unsigned i = 0; i < static_cast<unsigned>(db.zones.size()); ++i) {

    zone_indices.emplace(db.zones[i].name, i);
  }

  std::map<std::string, std::vector<rule>> rule_sets;

  for (const auto &r : db.rules)
    rule_sets[r.name].push_back(r);

  for (auto &[name, rules] : rule_sets) {
    std::sort(rules.begin(), rules.end(), [](const rule &a, const rule &b) {
      if (a.from_year != b.from_year)
        return a.from_year < b.from_year;
      if (a.month != b.month)
        return a.month < b.month;
      if (a.on.kind != b.on.kind)
        return a.on.kind < b.on.kind;
      if (a.on.weekday != b.on.weekday)
        return a.on.weekday < b.on.weekday;
      if (a.on.day != b.on.day)
        return a.on.day < b.on.day;
      return a.at.seconds < b.at.seconds;
    });
  }

  std::unordered_map<std::string, unsigned> rule_set_indices;

  {
    unsigned index = 0;

    for (const auto &[name, rules] : rule_sets)
      rule_set_indices.emplace(name, index++);
  }

  for (const auto &z : db.zones) {
    for (const auto &era : z.eras) {
      if (era.rules != rules_kind::named)
        continue;

      if (!rule_set_indices.contains(era.rule_name))
        throw std::runtime_error("Zone '" + z.name +
                                 "' references unknown rule set '" +
                                 era.rule_name + "'");
    }
  }

  std::unordered_map<std::string, std::string> raw_links;

  for (const auto &l : db.links) {
    if (!raw_links.emplace(l.name, l.target).second)
      throw std::runtime_error("duplicate Link: " + l.name);

    if (zone_indices.contains(l.name))
      throw std::runtime_error("Link collides with Zone: " + l.name);
  }

  struct resolved_link {
    std::string name;
    unsigned target_zone = 0;
  };

  std::vector<resolved_link> links;
  links.reserve(db.links.size());

  for (const auto &l : db.links) {
    std::string target = l.target;
    std::unordered_set<std::string> seen;

    while (true) {
      if (const auto zone_it = zone_indices.find(target);
          zone_it != zone_indices.end()) {

        links.push_back({
            l.name,
            zone_it->second,
        });

        break;
      }

      if (!seen.insert(target).second)
        throw std::runtime_error("Link cycle involving " + target);

      const auto link_it = raw_links.find(target);

      if (link_it == raw_links.end())
        throw std::runtime_error("Link '" + l.name +
                                 "' resolves to missing target '" + target +
                                 "'");

      target = link_it->second;
    }
  }

  std::sort(links.begin(), links.end(),
            [](const resolved_link &a, const resolved_link &b) {
              return a.name < b.name;
            });

  std::sort(db.leaps.begin(), db.leaps.end(), [](const leap &a, const leap &b) {
    return a.date_seconds < b.date_seconds;
  });

  string_table strings;

  for (const auto &z : db.zones) {
    strings.add(z.name);

    for (const auto &era : z.eras)
      strings.add(era.format);
  }

  for (const auto &[name, rules] : rule_sets) {
    strings.add(name);

    for (const auto &r : rules)
      strings.add(r.letters);
  }

  for (const auto &l : links)
    strings.add(l.name);

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

    flat_rule_sets.push_back(std::move(set));
  }

  struct flattened_zone {
    std::string name;
    unsigned era_begin = 0;
    unsigned era_count = 0;
  };

  std::vector<flattened_zone> flat_zones;
  std::vector<zone_era> flat_eras;

  for (const auto &z : db.zones) {
    flattened_zone result;
    result.name = z.name;
    result.era_begin = static_cast<unsigned>(flat_eras.size());
    result.era_count = static_cast<unsigned>(z.eras.size());

    flat_eras.insert(flat_eras.end(), z.eras.begin(), z.eras.end());

    flat_zones.push_back(std::move(result));
  }

  std::ostringstream out;

  out <<
      R"(// Generated by tools/buildtzdb. Do not edit.
#pragma once

namespace ftl_tzdb_data {

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
    day_kind kind;
    int weekday;
    int day;
};

struct rule_record {
    int from_year;
    int to_year;
    bool to_max;

    int month;
    day_spec on;

    int at_seconds;
    time_basis at_basis;

    int save_seconds;
    bool save_is_daylight;

    unsigned letters;
};

struct rule_set_record {
    unsigned name;
    unsigned rule_begin;
    unsigned rule_count;
};

struct era_record {
    int standard_offset_seconds;

    rules_kind rules;
    unsigned rule_set;
    int fixed_save_seconds;
    bool fixed_save_is_daylight;

    unsigned format;

    bool has_until;
    int until_year;
    int until_month;
    day_spec until_day;
    int until_seconds;
    time_basis until_basis;
};

struct zone_record {
    unsigned name;
    unsigned era_begin;
    unsigned era_count;
};

struct link_record {
    unsigned name;
    unsigned target_zone;
};

struct leap_record {
    long long date_seconds;
    int value;
};

inline constexpr unsigned no_rule_set = ~0u;

)";

  out << "inline constexpr char version[] = \"" << escape_string(db.version)
      << "\";\n\n";

  out << "inline constexpr char strings[] = \"" << escape_string(strings.data())
      << "\";\n\n";

  out << "inline constexpr rule_record rules[] = {\n";

  for (const auto &r : flat_rules) {
    out << "    { " << r.from_year << ", " << r.to_year << ", "
        << (r.to_max ? "true" : "false") << ", " << r.month << ", ";

    emit_day(out, r.on);

    out << ", " << r.at.seconds << ", " << time_basis_name(r.at.basis) << ", "
        << r.save.seconds << ", " << (r.save.daylight ? "true" : "false")
        << ", " << strings.offset(r.letters) << " },\n";
  }

  out << "};\n\n";

  out << "inline constexpr rule_set_record rule_sets[] = {\n";

  for (const auto &set : flat_rule_sets) {
    out << "    { " << strings.offset(set.name) << ", " << set.begin << ", "
        << set.count << " },\n";
  }

  out << "};\n\n";

  out << "inline constexpr era_record eras[] = {\n";

  for (const auto &era : flat_eras) {
    const unsigned rule_set = era.rules == rules_kind::named
                                  ? rule_set_indices.at(era.rule_name)
                                  : ~0u;

    out << "    { " << era.standard_offset_seconds << ", "
        << rules_kind_name(era.rules) << ", ";

    if (rule_set == ~0u)
      out << "no_rule_set";
    else
      out << rule_set;

    out << ", " << era.fixed_save.seconds << ", "
        << (era.fixed_save.daylight ? "true" : "false") << ", "
        << strings.offset(era.format) << ", ";

    if (era.until) {
      out << "true, " << era.until->year << ", " << era.until->month << ", ";

      emit_day(out, era.until->day);

      out << ", " << era.until->time.seconds << ", "
          << time_basis_name(era.until->time.basis);
    } else {
      out << "false, 0, 1, "
          << "{ day_kind::exact, 0, 1 }, "
          << "0, time_basis::wall";
    }

    out << " },\n";
  }

  out << "};\n\n";

  out << "inline constexpr zone_record zones[] = {\n";

  for (const auto &z : flat_zones) {
    out << "    { " << strings.offset(z.name) << ", " << z.era_begin << ", "
        << z.era_count << " },\n";
  }

  out << "};\n\n";

  out << "inline constexpr link_record links[] = {\n";

  for (const auto &l : links) {
    out << "    { " << strings.offset(l.name) << ", " << l.target_zone
        << " },\n";
  }

  out << "};\n\n";

  out << "inline constexpr leap_record leaps[] = {\n";

  for (const auto &l : db.leaps) {
    out << "    { " << l.date_seconds << "LL, " << l.correction << " },\n";
  }

  out << "};\n\n";

  if (db.leap_expiration) {
    out << "inline constexpr long long leap_expiration = "
        << *db.leap_expiration << "LL;\n";
  } else {
    out << "inline constexpr long long leap_expiration = 0LL;\n";
  }

  out <<
      R"(
constexpr const char* string_at(unsigned offset) noexcept {
    return strings + offset;
}

} // namespace ftl_tzdb_data
)";

  write_if_changed(output_path, out.str());
}

} // namespace ftl_tzdb_tool
