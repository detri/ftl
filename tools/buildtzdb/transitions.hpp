#pragma once

#include "model.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace ftl_tzdb_tool {

struct compiled_state {
  int offset_seconds = 0;
  int save_minutes = 0;
  std::string abbreviation;

  friend bool operator==(
      const compiled_state&,
      const compiled_state&) = default;
};

struct compiled_transition {
  std::int64_t at = 0;
  compiled_state state;
};

struct compiled_zone {
  std::string name;

  compiled_state initial;
  std::vector<compiled_transition> transitions;

  std::int64_t precomputed_until = 0;
};

struct compiled_database {
  std::vector<compiled_zone> zones;
  std::int64_t precomputed_until = 0;
};

compiled_database compile_transitions(
    const database& db,
    int through_year = 2500);

} // namespace ftl_tzdb_tool
