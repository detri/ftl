add_executable(ftl-buildtzdb
        main.cpp
        parser.cpp
        transitions.cpp
        emit.cpp
)

target_compile_features(ftl-buildtzdb PRIVATE cxx_std_23)

set(FTL_TZDATA_DIR
        "${PROJECT_SOURCE_DIR}/external/tzdata"
)

set(FTL_TZDB_HEADER
        "${PROJECT_SOURCE_DIR}/include/ftl/detail/tzdb_data.hpp"
)

add_custom_target(ftl-tzdb-data
        COMMAND
        $<TARGET_FILE:ftl-buildtzdb>
        "${FTL_TZDATA_DIR}"
        "${FTL_TZDB_HEADER}"
        DEPENDS
        ftl-buildtzdb
        BYPRODUCTS
        "${FTL_TZDB_HEADER}"
        VERBATIM
)