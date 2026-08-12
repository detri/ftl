#pragma once

#include "model.hpp"

#include <filesystem>

namespace ftl_tzdb_tool {

void emit_windows_zones(const database &db,
                        const std::filesystem::path &input_path,
                        const std::filesystem::path &output_path);

} // namespace ftl_tzdb_tool
