#include "parser.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <charconv>
#include <chrono>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace ftl_tzdb_tool {
namespace {

[[noreturn]] void fail(const std::filesystem::path &file, std::size_t line,
                       const std::string &message) {

  throw std::runtime_error(file.string() + ":" + std::to_string(line) + ": " +
                           message);
}

std::string lower(std::string_view value) {
  std::string result;
  result.reserve(value.size());

  for (unsigned char c : value)
    result.push_back(static_cast<char>(std::tolower(c)));

  return result;
}

bool prefix_equal(std::string_view value, std::string_view full) {
  if (value.size() > full.size())
    return false;

  for (std::size_t i = 0; i < value.size(); ++i) {
    if (std::tolower(static_cast<unsigned char>(value[i])) !=
        std::tolower(static_cast<unsigned char>(full[i])))
      return false;
  }

  return true;
}

bool keyword(std::string_view value, std::string_view full) {
  return !value.empty() && prefix_equal(value, full);
}

int parse_integer(std::string_view value) {
  int result = 0;

  const auto first = value.data();
  const auto last = value.data() + value.size();

  const auto [ptr, ec] = std::from_chars(first, last, result);

  if (ec != std::errc{} || ptr != last)
    throw std::runtime_error("invalid integer: " + std::string(value));

  return result;
}

std::vector<std::string> tokenize(std::string_view line) {
  std::vector<std::string> result;
  std::string current;

  bool quoted = false;

  auto flush = [&] {
    if (!current.empty()) {
      result.push_back(std::move(current));
      current.clear();
    }
  };

  for (std::size_t i = 0; i < line.size(); ++i) {
    const char c = line[i];

    if (c == '"') {
      quoted = !quoted;
      continue;
    }

    if (!quoted && c == '#')
      break;

    if (!quoted && std::isspace(static_cast<unsigned char>(c))) {
      flush();
      continue;
    }

    current.push_back(c);
  }

  if (quoted)
    throw std::runtime_error("unterminated quoted field");

  flush();
  return result;
}

int resolve_named_prefix(std::string_view value,
                         const std::array<std::string_view, 12> &names) {

  int found = -1;

  for (std::size_t i = 0; i < names.size(); ++i) {
    if (!prefix_equal(value, names[i]))
      continue;

    if (found != -1)
      throw std::runtime_error("ambiguous name: " + std::string(value));

    found = static_cast<int>(i);
  }

  if (found == -1)
    throw std::runtime_error("unknown name: " + std::string(value));

  return found;
}

int parse_month(std::string_view value) {
  static constexpr std::array<std::string_view, 12> months = {
      "January", "February", "March",     "April",   "May",      "June",
      "July",    "August",   "September", "October", "November", "December",
  };

  return resolve_named_prefix(value, months) + 1;
}

int parse_weekday(std::string_view value) {
  static constexpr std::array<std::string_view, 7> weekdays = {
      "Sunday",   "Monday", "Tuesday",  "Wednesday",
      "Thursday", "Friday", "Saturday",
  };

  int found = -1;

  for (std::size_t i = 0; i < weekdays.size(); ++i) {
    if (!prefix_equal(value, weekdays[i]))
      continue;

    if (found != -1)
      throw std::runtime_error("ambiguous weekday: " + std::string(value));

    found = static_cast<int>(i);
  }

  if (found == -1)
    throw std::runtime_error("unknown weekday: " + std::string(value));

  return found;
}

day_spec parse_day(std::string_view value) {
  if (!value.empty() &&
      (std::isdigit(static_cast<unsigned char>(value.front())) ||
       value.front() == '-' || value.front() == '+')) {

    return {
        day_kind::exact,
        0,
        parse_integer(value),
    };
  }

  const auto lowered = lower(value);

  if (lowered.starts_with("last")) {
    const auto weekday_text = value.substr(4);

    if (weekday_text.empty())
      throw std::runtime_error("invalid last-weekday expression");

    return {
        day_kind::last_weekday,
        parse_weekday(weekday_text),
        0,
    };
  }

  if (const auto pos = value.find(">="); pos != std::string_view::npos) {
    return {
        day_kind::weekday_on_or_after,
        parse_weekday(value.substr(0, pos)),
        parse_integer(value.substr(pos + 2)),
    };
  }

  if (const auto pos = value.find("<="); pos != std::string_view::npos) {
    return {
        day_kind::weekday_on_or_before,
        parse_weekday(value.substr(0, pos)),
        parse_integer(value.substr(pos + 2)),
    };
  }

  throw std::runtime_error("invalid day expression: " + std::string(value));
}

struct parsed_duration {
  int seconds = 0;
  char suffix = '\0';
};

parsed_duration parse_duration(std::string_view original) {
  if (original == "-")
    return {};

  std::string_view value = original;

  char suffix = '\0';

  if (!value.empty() &&
      std::isalpha(static_cast<unsigned char>(value.back()))) {

    suffix = static_cast<char>(
        std::tolower(static_cast<unsigned char>(value.back())));

    value.remove_suffix(1);
  }

  bool negative = false;

  if (!value.empty() && (value.front() == '+' || value.front() == '-')) {
    negative = value.front() == '-';
    value.remove_prefix(1);
  }

  if (value.empty())
    throw std::runtime_error("invalid time: " + std::string(original));

  std::array<std::string_view, 3> parts{};
  std::size_t part_count = 0;

  std::size_t begin = 0;

  while (true) {
    if (part_count == parts.size())
      throw std::runtime_error("too many ':' fields in time: " +
                               std::string(original));

    const auto pos = value.find(':', begin);

    parts[part_count++] = value.substr(begin, pos == std::string_view::npos
                                                  ? std::string_view::npos
                                                  : pos - begin);

    if (pos == std::string_view::npos)
      break;

    begin = pos + 1;
  }

  int hours = 0;
  int minutes = 0;
  int seconds = 0;

  std::string_view seconds_field;

  if (part_count == 1) {
    hours = parse_integer(parts[0]);
  } else {
    hours = parse_integer(parts[0]);
    minutes = parse_integer(parts[1]);

    if (minutes < 0 || minutes >= 60)
      throw std::runtime_error("minute field out of range: " +
                               std::string(original));

    if (part_count == 3)
      seconds_field = parts[2];
  }

  std::uint64_t fractional_numerator = 0;
  std::uint64_t fractional_scale = 1;

  if (!seconds_field.empty()) {
    const auto dot = seconds_field.find('.');

    const auto whole = dot == std::string_view::npos
                           ? seconds_field
                           : seconds_field.substr(0, dot);

    seconds = parse_integer(whole);

    if (seconds < 0 || seconds >= 60)
      throw std::runtime_error("second field out of range: " +
                               std::string(original));

    if (dot != std::string_view::npos) {
      const auto fraction = seconds_field.substr(dot + 1);

      if (fraction.empty())
        throw std::runtime_error("empty fractional seconds: " +
                                 std::string(original));

      for (char c : fraction) {
        if (!std::isdigit(static_cast<unsigned char>(c)))
          throw std::runtime_error("invalid fractional seconds: " +
                                   std::string(original));

        if (fractional_scale <=
            std::numeric_limits<std::uint64_t>::max() / 10) {

          fractional_numerator =
              fractional_numerator * 10 + static_cast<unsigned>(c - '0');

          fractional_scale *= 10;
        }
      }
    }
  }

  std::int64_t total = static_cast<std::int64_t>(hours) * 3600 +
                       static_cast<std::int64_t>(minutes) * 60 + seconds;

  if (fractional_scale != 1) {
    const auto doubled = fractional_numerator * 2;

    if (doubled > fractional_scale ||
        (doubled == fractional_scale && (total & 1) != 0))
      ++total;
  }

  if (negative)
    total = -total;

  if (total < std::numeric_limits<int>::min() ||
      total > std::numeric_limits<int>::max())
    throw std::runtime_error("time outside supported range: " +
                             std::string(original));

  return {
      static_cast<int>(total),
      suffix,
  };
}

clock_time parse_at(std::string_view value) {
  const auto parsed = parse_duration(value);

  time_basis basis = time_basis::wall;

  switch (parsed.suffix) {
  case '\0':
  case 'w':
    basis = time_basis::wall;
    break;

  case 's':
    basis = time_basis::standard;
    break;

  case 'u':
  case 'g':
  case 'z':
    basis = time_basis::universal;
    break;

  default:
    throw std::runtime_error("invalid AT/UNTIL suffix: " + std::string(value));
  }

  return {
      parsed.seconds,
      basis,
  };
}

int parse_standard_offset(std::string_view value) {
  const auto parsed = parse_duration(value);

  if (parsed.suffix != '\0')
    throw std::runtime_error("standard offset may not have a suffix: " +
                             std::string(value));

  return parsed.seconds;
}

save_spec parse_save(std::string_view value) {
  const auto parsed = parse_duration(value);

  bool daylight = parsed.seconds != 0;

  switch (parsed.suffix) {
  case '\0':
    break;

  case 's':
    daylight = false;
    break;

  case 'd':
    daylight = true;
    break;

  default:
    throw std::runtime_error("invalid SAVE suffix: " + std::string(value));
  }

  return {
      parsed.seconds,
      daylight,
  };
}

until_spec parse_until(const std::vector<std::string> &fields,
                       std::size_t begin) {

  if (begin >= fields.size())
    throw std::runtime_error("missing UNTIL year");

  until_spec result;

  result.year = parse_integer(fields[begin]);

  if (begin + 1 < fields.size())
    result.month = parse_month(fields[begin + 1]);

  if (begin + 2 < fields.size())
    result.day = parse_day(fields[begin + 2]);

  if (begin + 3 < fields.size())
    result.time = parse_at(fields[begin + 3]);

  if (begin + 4 < fields.size())
    throw std::runtime_error("too many UNTIL fields");

  return result;
}

zone_era parse_zone_era(const std::vector<std::string> &fields,
                        std::size_t begin) {

  if (fields.size() < begin + 3)
    throw std::runtime_error("incomplete Zone era");

  zone_era era;

  era.standard_offset_seconds = parse_standard_offset(fields[begin]);

  const auto &rules = fields[begin + 1];

  if (rules == "-") {
    era.rules = rules_kind::none;
  } else if (!rules.empty() &&
             (std::isdigit(static_cast<unsigned char>(rules.front())) ||
              rules.front() == '+' || rules.front() == '-')) {

    era.rules = rules_kind::fixed;
    era.fixed_save = parse_save(rules);
  } else {
    era.rules = rules_kind::named;
    era.rule_name = rules;
  }

  era.format = fields[begin + 2];

  if (fields.size() > begin + 3)
    era.until = parse_until(fields, begin + 3);

  return era;
}

rule parse_rule(const std::vector<std::string> &fields) {
  if (fields.size() != 10)
    throw std::runtime_error("Rule line must contain exactly 10 fields");

  rule result;

  result.name = fields[1];
  result.from_year = parse_integer(fields[2]);

  if (keyword(fields[3], "maximum")) {
    result.to_max = true;
    result.to_year = result.from_year;
  } else if (keyword(fields[3], "only")) {
    result.to_year = result.from_year;
  } else {
    result.to_year = parse_integer(fields[3]);
  }

  if (fields[4] != "-")
    throw std::runtime_error("Rule TYPE/reserved field must be '-'");

  result.month = parse_month(fields[5]);
  result.on = parse_day(fields[6]);
  result.at = parse_at(fields[7]);
  result.save = parse_save(fields[8]);

  result.letters = fields[9] == "-" ? std::string{} : fields[9];

  return result;
}

void parse_source_file(database &db, const std::filesystem::path &file) {

  std::ifstream input(file);

  if (!input)
    throw std::runtime_error("unable to open " + file.string());

  std::optional<std::size_t> current_zone;

  std::string line;
  std::size_t line_number = 0;

  while (std::getline(input, line)) {
    ++line_number;

    std::vector<std::string> fields;

    try {
      fields = tokenize(line);
    } catch (const std::exception &e) {
      fail(file, line_number, e.what());
    }

    if (fields.empty())
      continue;

    try {
      if (keyword(fields[0], "Rule")) {
        current_zone.reset();
        db.rules.push_back(parse_rule(fields));
        continue;
      }

      if (keyword(fields[0], "Zone")) {
        if (fields.size() < 5)
          throw std::runtime_error("incomplete Zone line");

        zone z;
        z.name = fields[1];

        z.eras.push_back(parse_zone_era(fields, 2));

        db.zones.push_back(std::move(z));
        current_zone = db.zones.size() - 1;
        continue;
      }

      if (keyword(fields[0], "Link")) {
        current_zone.reset();

        if (fields.size() != 3)
          throw std::runtime_error("Link line must contain exactly 3 fields");

        db.links.push_back({
            fields[1],
            fields[2],
        });

        continue;
      }

      if (!current_zone)
        throw std::runtime_error("orphan Zone continuation line");

      auto &z = db.zones[*current_zone];

      if (z.eras.empty() || !z.eras.back().until)
        throw std::runtime_error(
            "Zone continuation follows an era without UNTIL");

      z.eras.push_back(parse_zone_era(fields, 0));

    } catch (const std::exception &e) {
      fail(file, line_number, e.what());
    }
  }
}

std::int64_t leap_timestamp(int year_value, int month_value, int day_value,
                            int hour, int minute, int second) {

  using namespace std::chrono;

  const year_month_day ymd{
      year{year_value},
      month{static_cast<unsigned>(month_value)},
      day{static_cast<unsigned>(day_value)},
  };

  if (!ymd.ok())
    throw std::runtime_error("invalid leap-second date");

  const auto tp = sys_seconds{sys_days{ymd}} + hours{hour} + minutes{minute} +
                  seconds{second};

  return tp.time_since_epoch().count();
}

void parse_leapseconds(database &db, const std::filesystem::path &file) {

  std::ifstream input(file);

  if (!input)
    throw std::runtime_error("unable to open " + file.string());

  std::string line;
  std::size_t line_number = 0;

  while (std::getline(input, line)) {
    ++line_number;

    auto fields = tokenize(line);

    if (fields.empty())
      continue;

    try {
      if (keyword(fields[0], "Leap")) {
        if (fields.size() != 7)
          throw std::runtime_error("Leap line must contain exactly 7 fields");

        const int year_value = parse_integer(fields[1]);
        const int month_value = parse_month(fields[2]);
        const int day_value = parse_integer(fields[3]);

        const auto time = fields[4];

        const auto first_colon = time.find(':');
        const auto second_colon = first_colon == std::string::npos
                                      ? std::string::npos
                                      : time.find(':', first_colon + 1);

        if (first_colon == std::string::npos ||
            second_colon == std::string::npos)
          throw std::runtime_error("invalid Leap time");

        const int hour =
            parse_integer(std::string_view{time}.substr(0, first_colon));

        const int minute = parse_integer(std::string_view{time}.substr(
            first_colon + 1, second_colon - first_colon - 1));

        const int second =
            parse_integer(std::string_view{time}.substr(second_colon + 1));

        int correction = 0;

        if (fields[5] == "+")
          correction = 1;
        else if (fields[5] == "-")
          correction = -1;
        else
          throw std::runtime_error("invalid Leap correction");

        bool rolling = false;

        if (keyword(fields[6], "Stationary"))
          rolling = false;
        else if (keyword(fields[6], "Rolling"))
          rolling = true;
        else
          throw std::runtime_error("invalid Leap R/S field");

        db.leaps.push_back({
            leap_timestamp(year_value, month_value, day_value, hour, minute,
                           second),
            correction,
            rolling,
        });

        continue;
      }

      if (keyword(fields[0], "Expires")) {
        if (fields.size() != 5)
          throw std::runtime_error(
              "Expires line must contain exactly 5 fields");

        const int year_value = parse_integer(fields[1]);
        const int month_value = parse_month(fields[2]);
        const int day_value = parse_integer(fields[3]);

        const auto time = fields[4];

        const auto first_colon = time.find(':');
        const auto second_colon = first_colon == std::string::npos
                                      ? std::string::npos
                                      : time.find(':', first_colon + 1);

        if (first_colon == std::string::npos ||
            second_colon == std::string::npos)
          throw std::runtime_error("invalid Expires time");

        db.leap_expiration = leap_timestamp(
            year_value, month_value, day_value,
            parse_integer(std::string_view{time}.substr(0, first_colon)),
            parse_integer(std::string_view{time}.substr(
                first_colon + 1, second_colon - first_colon - 1)),
            parse_integer(std::string_view{time}.substr(second_colon + 1)));

        continue;
      }

      throw std::runtime_error("unknown leapseconds directive");

    } catch (const std::exception &e) {
      fail(file, line_number, e.what());
    }
  }
}

void parse_leap_seconds_list(database &db, const std::filesystem::path &file) {

  constexpr std::int64_t ntp_unix_epoch_delta = 2'208'988'800LL;

  std::ifstream input(file);

  if (!input)
    throw std::runtime_error("unable to open " + file.string());

  std::optional<std::int64_t> expiration;

  std::string line;
  std::size_t line_number = 0;

  while (std::getline(input, line)) {
    ++line_number;

    std::string_view text = line;

    while (!text.empty() &&
           std::isspace(static_cast<unsigned char>(text.front())))
      text.remove_prefix(1);

    if (!text.starts_with("#@"))
      continue;

    text.remove_prefix(2);

    while (!text.empty() &&
           std::isspace(static_cast<unsigned char>(text.front())))
      text.remove_prefix(1);

    if (text.empty())
      fail(file, line_number, "missing NTP expiration timestamp");

    const auto end =
        std::find_if(text.begin(), text.end(),
                     [](unsigned char c) { return std::isspace(c); });

    const std::string_view timestamp{
        text.data(),
        static_cast<std::size_t>(end - text.begin()),
    };

    std::uint64_t ntp_timestamp = 0;

    const auto [ptr, ec] = std::from_chars(
        timestamp.data(), timestamp.data() + timestamp.size(), ntp_timestamp);

    if (ec != std::errc{} || ptr != timestamp.data() + timestamp.size())
      fail(file, line_number, "invalid NTP expiration timestamp");

    if (ntp_timestamp >
        static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
      fail(file, line_number, "NTP expiration timestamp is too large");

    const auto unix_timestamp =
        static_cast<std::int64_t>(ntp_timestamp) - ntp_unix_epoch_delta;

    if (expiration)
      fail(file, line_number, "duplicate #@ expiration directive");

    expiration = unix_timestamp;
  }

  if (!expiration)
    throw std::runtime_error(file.string() +
                             ": missing #@ expiration directive");

  if (db.leap_expiration && *db.leap_expiration != *expiration) {

    throw std::runtime_error(
        file.string() + ": leap-second expiration disagrees with leapseconds");
  }

  db.leap_expiration = *expiration;
}

std::string read_version(const std::filesystem::path &file) {

  std::ifstream input(file);

  if (!input)
    throw std::runtime_error("unable to open " + file.string());

  std::string version;

  if (!std::getline(input, version))
    throw std::runtime_error("empty version file");

  while (!version.empty() &&
         std::isspace(static_cast<unsigned char>(version.back())))
    version.pop_back();

  return version;
}

} // namespace

database parse_database(const std::filesystem::path &directory) {

  static constexpr std::array<std::string_view, 10> sources = {
      "africa",       "antarctica",   "asia",     "australasia", "europe",
      "northamerica", "southamerica", "etcetera", "factory",     "backward",
  };

  database db;

  for (const auto source : sources)
    parse_source_file(db, directory / source);

  parse_leapseconds(db, directory / "leapseconds");
  parse_leap_seconds_list(db, directory / "leap-seconds.list");

  db.version = read_version(directory / "version");

  return db;
}

} // namespace ftl_tzdb_tool
