#pragma once

#include "transitions.hpp"

#include <filesystem>

namespace ftl_tzdb_tool {

void emit_database(const database &db, const compiled_database &compiled,
                   const std::filesystem::path &output);

} // namespace ftl_tzdb_tool