cmake_minimum_required(VERSION 3.28)
project(ftl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

add_library(ftl INTERFACE)
target_include_directories(ftl INTERFACE include)
target_compile_features(ftl INTERFACE cxx_std_23)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
