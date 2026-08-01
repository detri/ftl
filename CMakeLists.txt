cmake_minimum_required(VERSION 3.28)
project(ftl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_library(ftl INTERFACE)
target_include_directories(ftl INTERFACE include)
target_compile_features(ftl INTERFACE cxx_std_23)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
    GIT_SHALLOW TRUE
)

if(BUILD_TESTING)
    enable_testing()

    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    include(CheckCXXCompilerFlag)

    check_cxx_compiler_flag(
            "-Wno-character-conversion"
            FTL_HAS_WNO_CHARACTER_CONVERSION
    )

    if(FTL_HAS_WNO_CHARACTER_CONVERSION)
        target_compile_options(
                gtest
                PRIVATE
                "-Wno-character-conversion"
        )
    endif()
    include(GoogleTest)

    function(add_ftl_test header)
        foreach(mode IN ITEMS normal replace)
            set(object ftl_${mode}_${header}_object)
            add_library(${object} OBJECT tests/${header}.cpp)
            target_compile_features(${object} PRIVATE cxx_std_23)

            if(mode STREQUAL replace)
                target_compile_definitions(${object} PRIVATE FTL_REPLACE_STL)
                target_include_directories(${object} PRIVATE include/ftl)
            else()
                target_link_libraries(${object} PRIVATE ftl)
            endif()

            set(target ftl_${mode}_${header})
            add_executable(
                ${target}
                tests/gtest_driver.cpp
                $<TARGET_OBJECTS:${object}>
            )
            target_link_libraries(${target} PRIVATE GTest::gtest_main)
            gtest_add_tests(
                TARGET ${target}
                SOURCES tests/gtest_driver.cpp
                TEST_PREFIX "${mode}.${header}."
            )
        endforeach()
    endfunction()

    foreach(header IN ITEMS
            cstddef
            type_traits
            utility
            concepts
            memory
            atomic
            exception
            optional
            initializer_list
            functional
            compare
            array
            cstdint
            cstring
            bit
            string_view)
        add_ftl_test(${header})
    endforeach()
endif()
