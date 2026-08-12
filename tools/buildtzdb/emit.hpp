#pragma once

#include "transitions.hpp"
#include "windows_zones.hpp"

#include <filesystem>
#include <vector>

namespace ftl_tzdb_tool {

void emit_database(const database &db, const compiled_database &compiled,
                   const std::vector<windows_mapping> &windows_mappings,
                   const std::filesystem::path &blob_output,
                   const std::filesystem::path &source_output);

} // namespace ftl_tzdb_tool
