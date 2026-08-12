add_executable(ftl-buildtzdb
        main.cpp
        parser.cpp
        transitions.cpp
        emit.cpp
        windows_zones.cpp
)

target_compile_features(
        ftl-buildtzdb
        PRIVATE
        cxx_std_23
)

set(FTL_TZDATA_DIR
        "${PROJECT_SOURCE_DIR}/external/tzdata"
)

set(FTL_CLDR_WINDOWS_ZONES
        "${PROJECT_SOURCE_DIR}/external/cldr/windowsZones.xml"
)

set(FTL_TZDB_HEADER
        "${PROJECT_SOURCE_DIR}/include/ftl/detail/tzdb_data.hpp"
)

set(FTL_WINDOWS_TZ_HEADER
        "${PROJECT_SOURCE_DIR}/include/ftl/detail/windows_tz_data.hpp"
)

add_custom_target(ftl-tzdb-data
        COMMAND
        $<TARGET_FILE:ftl-buildtzdb>
        "${FTL_TZDATA_DIR}"
        "${FTL_CLDR_WINDOWS_ZONES}"
        "${FTL_TZDB_HEADER}"
        "${FTL_WINDOWS_TZ_HEADER}"

        DEPENDS
        ftl-buildtzdb
        "${FTL_CLDR_WINDOWS_ZONES}"

        BYPRODUCTS
        "${FTL_TZDB_HEADER}"
        "${FTL_WINDOWS_TZ_HEADER}"

        VERBATIM
)
