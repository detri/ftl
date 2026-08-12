#include "emit.hpp"
#include "parser.hpp"
#include "transitions.hpp"
#include "windows_zones.hpp"

#include <exception>
#include <filesystem>
#include <iostream>

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "usage: buildtzdb "
                 "<tzdata-directory> "
                 "<windowsZones.xml> "
                 "<output-blob> "
                 "<output-source>\n";

    return 2;
  }

  try {
    const std::filesystem::path input = argv[1];

    const std::filesystem::path windows_input = argv[2];

    const std::filesystem::path blob_output = argv[3];

    const std::filesystem::path source_output = argv[4];

    const auto db = ftl_tzdb_tool::parse_database(input);

    const auto compiled = ftl_tzdb_tool::compile_transitions(db, 2500);

    const auto windows_mappings =
        ftl_tzdb_tool::parse_windows_zones(db, windows_input);

    std::size_t transition_count = 0;
    std::size_t largest_count = 0;
    std::string largest_zone;

    for (const auto &zone : compiled.zones) {
      transition_count += zone.transitions.size();

      if (zone.transitions.size() > largest_count) {
        largest_count = zone.transitions.size();

        largest_zone = zone.name;
      }
    }

    ftl_tzdb_tool::emit_database(db, compiled, windows_mappings, blob_output,
                                 source_output);

    std::cout << "tzdb " << db.version << ": " << db.zones.size() << " zones, "
              << db.links.size() << " links, " << db.rules.size() << " rules, "
              << db.leaps.size() << " leap seconds, " << windows_mappings.size()
              << " Windows mappings\n";

    std::cout << "precomputed through 2500: " << transition_count
              << " transitions; largest zone " << largest_zone << " has "
              << largest_count << '\n';

    return 0;

  } catch (const std::exception &error) {
    std::cerr << "buildtzdb: " << error.what() << '\n';

    return 1;
  }
}
