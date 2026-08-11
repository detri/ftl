#pragma once

#include "model.hpp"

#include <filesystem>

namespace ftl_tzdb_tool {

void emit_database(const database &db, const std::filesystem::path &output);

} // namespace ftl_tzdb_tool
