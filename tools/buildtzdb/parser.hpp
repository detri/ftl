#pragma once

#include "model.hpp"

#include <filesystem>

namespace ftl_tzdb_tool {

database parse_database(const std::filesystem::path &directory);

} // namespace ftl_tzdb_tool
