include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
    GIT_SHALLOW TRUE
)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(
    "-Wno-character-conversion"
    FTL_HAS_WNO_CHARACTER_CONVERSION
)
if(FTL_HAS_WNO_CHARACTER_CONVERSION)
    target_compile_options(gtest PRIVATE "-Wno-character-conversion")
endif()

function(add_ftl_test header)
    foreach(mode IN ITEMS normal replace)
        set(object ftl_${mode}_${header}_object)
        add_library(${object} OBJECT ${header}.cpp)
        target_compile_features(${object} PRIVATE cxx_std_23)
        target_compile_options(${object} PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/EHsc>")

        if(mode STREQUAL replace)
            target_compile_definitions(${object} PRIVATE FTL_REPLACE_STL)
            target_include_directories(${object} PRIVATE ../include/ftl)
        else()
            target_link_libraries(${object} PRIVATE ftl)
        endif()

        set(target ftl_${mode}_${header})
        add_executable(${target} gtest_driver.cpp $<TARGET_OBJECTS:${object}>)
        target_link_libraries(${target} PRIVATE GTest::gtest_main)
        target_compile_options(${target} PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/EHsc>")
        add_test(NAME "${mode}.${header}" COMMAND ${target})
    endforeach()
endfunction()

foreach(header IN ITEMS
        cstddef
        type_traits
        utility
        concepts
        new
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
