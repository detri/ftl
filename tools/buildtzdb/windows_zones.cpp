#include "windows_zones.hpp"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace ftl_tzdb_tool {
namespace {

struct windows_mapping {
  std::string windows_name;
  std::string iana_name;
};

std::optional<std::string> xml_attribute(std::string_view element,
                                         std::string_view attribute) {
  std::string needle;
  needle.reserve(attribute.size() + 3);

  needle.append(attribute);
  needle.append("=\"");

  const auto begin = element.find(needle);

  if (begin == std::string_view::npos)
    return std::nullopt;

  const auto value_begin = begin + needle.size();
  const auto value_end = element.find('"', value_begin);

  if (value_end == std::string_view::npos)
    throw std::runtime_error("unterminated XML attribute '" +
                             std::string(attribute) + "'");

  return std::string{
      element.substr(value_begin, value_end - value_begin),
  };
}

std::string escape_cpp(std::string_view value) {
  std::string result;

  for (char character : value) {
    if (character == '\\' || character == '"')
      result.push_back('\\');

    result.push_back(character);
  }

  return result;
}

bool known_iana_name(const database &db, std::string_view name) {
  for (const auto &zone : db.zones) {
    if (zone.name == name)
      return true;
  }

  for (const auto &link : db.links) {
    if (link.name == name)
      return true;
  }

  return false;
}

std::vector<windows_mapping>
parse_windows_zones(const database &db, const std::filesystem::path &path) {
  std::ifstream input(path, std::ios::binary);

  if (!input)
    throw std::runtime_error("unable to open " + path.string());

  const std::string xml{
      std::istreambuf_iterator<char>{input},
      std::istreambuf_iterator<char>{},
  };

  std::vector<windows_mapping> result;

  std::size_t position = 0;

  for (;;) {
    const auto begin = xml.find("<mapZone", position);

    if (begin == std::string::npos)
      break;

    const auto end = xml.find("/>", begin);

    if (end == std::string::npos)
      throw std::runtime_error("unterminated <mapZone> element");

    const std::string_view element{
        xml.data() + begin,
        end + 2 - begin,
    };

    position = end + 2;

    const auto territory = xml_attribute(element, "territory");

    if (!territory || *territory != "001") {
      continue;
    }

    const auto windows_name = xml_attribute(element, "other");

    const auto iana_name = xml_attribute(element, "type");

    if (!windows_name || !iana_name)
      throw std::runtime_error("territory 001 mapZone is missing "
                               "'other' or 'type'");

    /*
     * CLDR territory 001 is the global/default mapping.
     * It must identify one preferred IANA zone.
     */
    if (iana_name->find(' ') != std::string::npos) {
      throw std::runtime_error("territory 001 Windows mapping has "
                               "multiple IANA targets: " +
                               *windows_name);
    }

    if (!known_iana_name(db, *iana_name)) {
      throw std::runtime_error("Windows mapping target is absent "
                               "from vendored tzdb: " +
                               *iana_name);
    }

    result.push_back({
        *windows_name,
        *iana_name,
    });
  }

  std::sort(result.begin(), result.end(),
            [](const windows_mapping &left, const windows_mapping &right) {
              return left.windows_name < right.windows_name;
            });

  for (std::size_t index = 1; index < result.size(); ++index) {
    if (result[index - 1].windows_name == result[index].windows_name) {
      throw std::runtime_error("duplicate Windows time zone mapping: " +
                               result[index].windows_name);
    }
  }

  if (result.empty())
    throw std::runtime_error("windowsZones.xml produced no "
                             "territory 001 mappings");

  return result;
}

void write_if_changed(const std::filesystem::path &path,
                      const std::string &contents) {
  {
    std::ifstream input(path, std::ios::binary);

    if (input) {
      const std::string existing{
          std::istreambuf_iterator<char>{input},
          std::istreambuf_iterator<char>{},
      };

      if (existing == contents)
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

void emit_windows_zones(const database &db,
                        const std::filesystem::path &input_path,
                        const std::filesystem::path &output_path) {
  const auto mappings = parse_windows_zones(db, input_path);

  std::ostringstream out;

  out <<
      R"(// Generated by tools/buildtzdb. Do not edit.
#pragma once

namespace ftl_windows_tz_data {

struct mapping_record {
  const char *windows_name;
  const char *iana_name;
};

inline constexpr mapping_record mappings[] = {
)";

  for (const auto &mapping : mappings) {
    out << "    { \"" << escape_cpp(mapping.windows_name) << "\", \""
        << escape_cpp(mapping.iana_name) << "\" },\n";
  }

  out <<
      R"(};

} // namespace ftl_windows_tz_data
)";

  write_if_changed(output_path, out.str());
}

} // namespace ftl_tzdb_tool
