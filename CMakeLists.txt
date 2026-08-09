cmake_minimum_required(VERSION 3.28)
set(FTL_VERSION 0.3.3)
project(ftl VERSION ${FTL_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# FTL

add_library(ftl INTERFACE)
add_subdirectory(external/openlibm)
target_include_directories(ftl INTERFACE include)
target_compile_features(ftl INTERFACE cxx_std_23)
target_link_libraries(ftl INTERFACE ftl_math_runtime)

# Concurrency

if(WIN32)
    target_link_libraries(ftl INTERFACE synchronization)
endif()

if(UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(ftl INTERFACE Threads::Threads)
endif()

# Tests

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
