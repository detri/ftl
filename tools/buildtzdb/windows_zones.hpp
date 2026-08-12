#pragma once

#include "model.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace ftl_tzdb_tool {

struct windows_mapping {
  std::string windows_name;
  std::string iana_name;
};

std::vector<windows_mapping>
parse_windows_zones(const database &db, const std::filesystem::path &path);

} // namespace ftl_tzdb_tool
