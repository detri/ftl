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
            if(MSVC)
                target_compile_options(${object} PRIVATE /X)
            else()
                target_compile_options(${object} PRIVATE -nostdinc++)
            endif()
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

function(add_ftl_compile_fail_check name source)
    try_compile(compiled
        "${CMAKE_CURRENT_BINARY_DIR}/compile-fail/${name}"
        "${CMAKE_CURRENT_SOURCE_DIR}/${source}"
        CMAKE_FLAGS
            "-DCMAKE_CXX_STANDARD=23"
            "-DINCLUDE_DIRECTORIES=${PROJECT_SOURCE_DIR}/include/ftl"
        COMPILE_DEFINITIONS FTL_REPLACE_STL
        OUTPUT_VARIABLE output
    )
    if(compiled)
        message(FATAL_ERROR "${name} unexpectedly compiled:\n${output}")
    endif()
endfunction()

function(add_ftl_standalone_check name source)
    add_executable(ftl_check_${name} ${source})
    target_compile_features(ftl_check_${name} PRIVATE cxx_std_23)
    target_link_libraries(ftl_check_${name} PRIVATE ftl)
    add_test(NAME check.${name} COMMAND ftl_check_${name})
endfunction()

function(add_ftl_freestanding_link_check name source)
    add_executable(ftl_check_${name} ${source})
    target_compile_features(ftl_check_${name} PRIVATE cxx_std_23)
    target_compile_definitions(ftl_check_${name} PRIVATE FTL_REPLACE_STL)
    target_include_directories(ftl_check_${name} PRIVATE ../include/ftl)
    if(MSVC)
        target_compile_options(ftl_check_${name} PRIVATE /X)
        target_link_options(ftl_check_${name} PRIVATE
            /NODEFAULTLIB /ENTRY:ftl_entry /SUBSYSTEM:CONSOLE)
    else()
        target_compile_options(ftl_check_${name} PRIVATE -nostdinc++)
        if(APPLE)
            target_link_options(ftl_check_${name} PRIVATE "LINKER:-e,_ftl_entry")
        else()
            target_link_options(ftl_check_${name} PRIVATE
                -nostdlib "LINKER:-e,ftl_entry")
        endif()
    endif()
endfunction()

function(add_ftl_disabled_features_check name source)
    add_ftl_standalone_check(${name} ${source})
    target_compile_definitions(ftl_check_${name} PRIVATE FTL_NO_THREADS)
    if(MSVC)
        target_compile_options(ftl_check_${name} PRIVATE /EHs-c- /GR-)
    else()
        target_compile_options(ftl_check_${name} PRIVATE -fno-exceptions -fno-rtti)
    endif()
endfunction()

add_ftl_compile_fail_check(byte_rejects_float checks/compile_fail.cpp)
add_ftl_compile_fail_check(byte_shift_rejects_float checks/byte_shift_compile_fail.cpp)
add_ftl_compile_fail_check(initializer_list_rejects_specialization
    checks/initializer_list_specialization_compile_fail.cpp)
add_ftl_compile_fail_check(type_traits_rejects_make_signed_bool
    checks/type_traits_make_signed_bool_compile_fail.cpp)
add_ftl_compile_fail_check(utility_rejects_bad_forward
    checks/utility_forward_compile_fail.cpp)
add_ftl_compile_fail_check(compare_rejects_nonzero
    checks/compare_nonzero_compile_fail.cpp)
add_ftl_compile_fail_check(pair_rejects_dangling_reference
    checks/pair_dangling_compile_fail.cpp)
add_ftl_standalone_check(feature_macros checks/feature_macros.cpp)
add_ftl_standalone_check(no_allocation checks/no_allocation.cpp)
add_ftl_freestanding_link_check(freestanding_link checks/freestanding_link.cpp)
add_ftl_disabled_features_check(disabled_features checks/disabled_features.cpp)

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
        limits
        version
        source_location
        stdfloat
        string_view)
    add_ftl_test(${header})
endforeach()
