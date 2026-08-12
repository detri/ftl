add_executable(
        ftl-buildtzdb
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

target_include_directories(
        ftl-buildtzdb
        PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
)

set(
        FTL_TZDATA_DIR
        "${PROJECT_SOURCE_DIR}/external/tzdata"
)

set(
        FTL_CLDR_WINDOWS_ZONES
        "${PROJECT_SOURCE_DIR}/external/cldr/windowsZones.xml"
)

set(
        FTL_TZDB_BLOB
        "${PROJECT_SOURCE_DIR}/generated/tzdb.bin"
)

set(
        FTL_TZDB_SOURCE
        "${PROJECT_SOURCE_DIR}/runtime/tzdb_blob.cpp"
)

file(
        GLOB
        FTL_TZDATA_FILES
        CONFIGURE_DEPENDS
        "${FTL_TZDATA_DIR}/*"
)

add_custom_command(
        OUTPUT
        "${FTL_TZDB_BLOB}"
        "${FTL_TZDB_SOURCE}"

        COMMAND
        $<TARGET_FILE:ftl-buildtzdb>
        "${FTL_TZDATA_DIR}"
        "${FTL_CLDR_WINDOWS_ZONES}"
        "${FTL_TZDB_BLOB}"
        "${FTL_TZDB_SOURCE}"

        DEPENDS
        ftl-buildtzdb
        ${FTL_TZDATA_FILES}
        "${FTL_CLDR_WINDOWS_ZONES}"

        VERBATIM
)

add_custom_target(
        ftl-tzdb-data
        DEPENDS
        "${FTL_TZDB_BLOB}"
        "${FTL_TZDB_SOURCE}"
)