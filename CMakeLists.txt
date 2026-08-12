cmake_minimum_required(VERSION 3.28)
set(FTL_VERSION 0.3.3)
project(ftl VERSION ${FTL_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# tzdb tool
if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(tools/buildtzdb)
endif()

# FTL

add_library(ftl INTERFACE)
add_subdirectory(external/openlibm)
target_include_directories(ftl INTERFACE include)
target_compile_features(ftl INTERFACE cxx_std_23)
target_link_libraries(ftl INTERFACE ftl_math_runtime)

# Chrono

add_library(
        ftl_chrono_runtime
        STATIC
        runtime/current_zone.cpp
)

target_include_directories(
        ftl_chrono_runtime
        PRIVATE
        include
)

target_compile_features(
        ftl_chrono_runtime
        PRIVATE
        cxx_std_23
)

if (MSVC)
    target_compile_options(
            ftl_chrono_runtime
            PRIVATE
            /EHs-c-
    )
else ()
    target_compile_options(
            ftl_chrono_runtime
            PRIVATE
            -fno-exceptions
            -fno-rtti
    )
endif ()

if (APPLE)
    find_library(
            FTL_COREFOUNDATION_FRAMEWORK
            CoreFoundation
            REQUIRED
    )

    target_link_libraries(
            ftl_chrono_runtime
            PUBLIC
            "${FTL_COREFOUNDATION_FRAMEWORK}"
    )
endif ()

# I/O

add_library(ftl_io_runtime STATIC runtime/native_io.cpp runtime/native_filesystem.cpp runtime/stdio.cpp
                                  runtime/stdio_format.cpp)
target_include_directories(ftl_io_runtime PRIVATE include)
target_compile_features(ftl_io_runtime PRIVATE cxx_std_23)
target_link_libraries(ftl_io_runtime PRIVATE ftl_math_runtime)
if (MSVC)
    target_compile_options(ftl_io_runtime PRIVATE /EHs-c-)
else ()
    target_compile_options(ftl_io_runtime PRIVATE -fno-exceptions -fno-rtti)
endif ()

# Dependencies

target_link_libraries(ftl INTERFACE ftl_io_runtime)
target_link_libraries(ftl INTERFACE ftl_chrono_runtime)

# Compiler

add_library(ftl_compiler_runtime STATIC runtime/memory.cpp)

target_compile_features(ftl_compiler_runtime PRIVATE cxx_std_23)

if(MSVC)
    target_compile_options(
            ftl_compiler_runtime
            PRIVATE
            /GS-
            /Oi-
    )
else()
    target_compile_options(
            ftl_compiler_runtime
            PRIVATE
            -fno-stack-protector
            -fno-exceptions
            -fno-rtti
    )
endif()

# Concurrency

if (WIN32)
    target_link_libraries(ftl INTERFACE synchronization)
endif ()

if (UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(ftl INTERFACE Threads::Threads)
endif ()

# Tests

if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()
