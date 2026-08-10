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
if (FTL_HAS_WNO_CHARACTER_CONVERSION)
    target_compile_options(gtest PRIVATE "-Wno-character-conversion")
endif ()

function(add_ftl_test header)
    foreach (mode IN ITEMS normal replace)
        set(object ftl_${mode}_${header}_object)
        add_library(${object} OBJECT ${header}.cpp)
        target_compile_features(${object} PRIVATE cxx_std_23)
        target_compile_options(${object} PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/EHsc>")

        if (mode STREQUAL replace)
            target_compile_definitions(${object} PRIVATE FTL_REPLACE_STL)
            target_include_directories(${object} PRIVATE ../include/ftl)
            if (MSVC)
                target_compile_options(${object} PRIVATE /X)
            else ()
                target_compile_options(${object} PRIVATE -nostdinc++)
            endif ()
        else ()
            target_link_libraries(${object} PRIVATE ftl)
        endif ()

        set(target ftl_${mode}_${header})
        if (mode STREQUAL replace AND
                header MATCHES "^(string|vector|forward_list|list|map|set|unordered_map|unordered_set|flat_map|flat_set)$")
            add_executable(${target} standalone_driver.cpp $<TARGET_OBJECTS:${object}>)
        else ()
            add_executable(${target} gtest_driver.cpp $<TARGET_OBJECTS:${object}>)
            target_link_libraries(${target} PRIVATE GTest::gtest_main)
        endif ()
        target_link_libraries(${target} PRIVATE ftl)
        target_compile_options(${target} PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/EHsc>")
        add_test(NAME "${mode}.${header}" COMMAND ${target})
    endforeach ()
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
    if (compiled)
        message(FATAL_ERROR "${name} unexpectedly compiled:\n${output}")
    endif ()
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
    target_link_libraries(ftl_check_${name} PRIVATE ftl_compiler_runtime)

    if (MSVC)
        target_compile_options(
                ftl_check_${name}
                PRIVATE
                /X
                /GS-
        )

        target_link_options(
                ftl_check_${name}
                PRIVATE
                /NODEFAULTLIB
                /ENTRY:ftl_entry
                /SUBSYSTEM:CONSOLE
        )
    else ()
        target_compile_options(
                ftl_check_${name}
                PRIVATE
                -nostdinc++
                -fno-stack-protector
        )

        if (APPLE)
            target_link_options(
                    ftl_check_${name}
                    PRIVATE
                    "LINKER:-e,_ftl_entry"
            )
        else ()
            target_link_options(
                    ftl_check_${name}
                    PRIVATE
                    -nostdlib
                    "LINKER:-e,ftl_entry"
            )
        endif ()
    endif ()
endfunction()

function(add_ftl_disabled_features_check name source)
    add_ftl_standalone_check(${name} ${source})
    target_compile_definitions(ftl_check_${name} PRIVATE
            FTL_NO_EXCEPTIONS FTL_NO_RTTI FTL_NO_THREADS FTL_REPLACE_STL)
    target_include_directories(ftl_check_${name} PRIVATE ../include/ftl)
    if (MSVC)
        target_compile_options(ftl_check_${name} PRIVATE /EHs-c- /GR- /X)
    else ()
        target_compile_options(ftl_check_${name} PRIVATE
                -fno-exceptions -fno-rtti -nostdinc++)
    endif ()
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
add_ftl_compile_fail_check(memory_rejects_invalid_uses_allocator
        checks/memory_uses_allocator_compile_fail.cpp)
add_ftl_compile_fail_check(any_rejects_move_only
        checks/any_rejects_move_only_compile_fail.cpp)
add_ftl_compile_fail_check(expected_rejects_reference
        checks/expected_rejects_reference_compile_fail.cpp)
add_ftl_compile_fail_check(optional_rejects_rvalue_reference
        checks/optional_rejects_rvalue_reference_compile_fail.cpp)
add_ftl_compile_fail_check(variant_rejects_duplicate_get
        checks/variant_rejects_duplicate_get_compile_fail.cpp)
add_ftl_compile_fail_check(span_rejects_const_removal checks/span_rejects_const_removal_compile_fail.cpp)
add_ftl_compile_fail_check(span_rejects_derived_to_base_array_conversion checks/span_rejects_derived_to_base_array_conversion_compile_fail.cpp)
add_ftl_compile_fail_check(span_rejects_mismatched_static_extent checks/span_rejects_mismatched_static_extent_compile_fail.cpp)
add_ftl_compile_fail_check(mdspan_rejects_character_index checks/mdspan_character_index_compile_fail.cpp)
add_ftl_compile_fail_check(mdspan_rejects_void_element checks/mdspan_void_element_compile_fail.cpp)
add_ftl_compile_fail_check(numeric_reduce checks/numeric_reduce_compile_fail.cpp)
add_ftl_compile_fail_check(
        string_view_rejects_nullptr
        checks/string_view_nullptr_compile_fail.cpp
)
add_ftl_compile_fail_check(
        string_rejects_nullptr
        checks/string_nullptr_compile_fail.cpp
)
add_ftl_compile_fail_check(vector_rejects_mismatched_allocator
        checks/vector_mismatched_allocator_compile_fail.cpp)
add_ftl_compile_fail_check(deque_rejects_mismatched_allocator
        checks/deque_mismatched_allocator_compile_fail.cpp)
add_ftl_compile_fail_check(forward_list_rejects_mismatched_allocator
        checks/forward_list_mismatched_allocator_compile_fail.cpp)
add_ftl_compile_fail_check(list_rejects_mismatched_allocator
        checks/list_mismatched_allocator_compile_fail.cpp)
add_ftl_compile_fail_check(map_rejects_mismatched_allocator
        checks/map_mismatched_allocator_compile_fail.cpp)
add_ftl_compile_fail_check(set_rejects_mismatched_allocator
        checks/set_mismatched_allocator_compile_fail.cpp)
add_ftl_compile_fail_check(unordered_map_rejects_mismatched_allocator
        checks/unordered_map_mismatched_allocator_compile_fail.cpp)
add_ftl_compile_fail_check(unordered_set_rejects_mismatched_allocator
        checks/unordered_set_mismatched_allocator_compile_fail.cpp)
add_ftl_compile_fail_check(flat_map_rejects_mismatched_container
        checks/flat_map_mismatched_container_compile_fail.cpp)
add_ftl_compile_fail_check(flat_set_rejects_mismatched_container
        checks/flat_set_mismatched_container_compile_fail.cpp)
add_ftl_compile_fail_check(semaphore_rejects_negative_least_max checks/semaphore_negative_least_max_compile_fail.cpp)
add_ftl_compile_fail_check(barrier_rejects_throwing_completion checks/barrier_throwing_completion_compile_fail.cpp)
add_ftl_standalone_check(feature_macros checks/feature_macros.cpp)
add_ftl_standalone_check(no_allocation checks/no_allocation.cpp)
add_ftl_freestanding_link_check(freestanding_link checks/freestanding_link.cpp)
add_ftl_freestanding_link_check(
        fenv_freestanding_link
        checks/fenv_freestanding_link.cpp
)
add_ftl_freestanding_link_check(
        cmath_freestanding_link
        checks/cmath_freestanding_link.cpp
)
target_link_libraries(ftl_check_cmath_freestanding_link PRIVATE ftl)
if (MSVC)
    target_compile_options(ftl_check_fenv_freestanding_link PRIVATE /O2 /GS-)
    target_compile_options(ftl_check_cmath_freestanding_link PRIVATE /O2 /GS-)
else ()
    target_compile_options(ftl_check_fenv_freestanding_link
            PRIVATE -O2 -fno-stack-protector)
    target_compile_options(ftl_check_cmath_freestanding_link
            PRIVATE -O2 -fno-stack-protector -fno-exceptions
            -fno-unwind-tables -fno-asynchronous-unwind-tables)
endif ()
add_ftl_freestanding_link_check(
        cstring_freestanding_link
        checks/cstring_freestanding_link.cpp
)
add_ftl_freestanding_link_check(
        string_freestanding_link
        checks/string_freestanding_link.cpp
)
add_ftl_freestanding_link_check(vector_freestanding_link
        checks/vector_freestanding_link.cpp)
target_compile_definitions(ftl_check_vector_freestanding_link PRIVATE FTL_NO_EXCEPTIONS)
if (MSVC)
    target_compile_options(
            ftl_check_vector_freestanding_link
            PRIVATE
            /O2
            /EHs-c-
    )
else ()
    target_compile_options(
            ftl_check_vector_freestanding_link
            PRIVATE
            -O2
            -fno-exceptions
    )
endif ()
add_ftl_freestanding_link_check(random_freestanding_link
        checks/random_freestanding_link.cpp)
add_ftl_freestanding_link_check(valarray_freestanding_link
        checks/valarray_freestanding_link.cpp)
if (MSVC)
    target_compile_options(ftl_check_random_freestanding_link PRIVATE /O2 /GS-)
    target_compile_options(ftl_check_valarray_freestanding_link PRIVATE /O2 /GS-)
else ()
    target_compile_options(ftl_check_random_freestanding_link PRIVATE -fno-stack-protector)
    target_compile_options(ftl_check_valarray_freestanding_link PRIVATE -fno-stack-protector)
endif ()
add_ftl_freestanding_link_check(deque_freestanding_link
        checks/deque_freestanding_link.cpp)
target_compile_definitions(ftl_check_deque_freestanding_link PRIVATE FTL_NO_EXCEPTIONS)
if (MSVC)
    target_compile_options(
            ftl_check_deque_freestanding_link
            PRIVATE
            /O2
            /EHs-c-
    )
else ()
    target_compile_options(
            ftl_check_deque_freestanding_link
            PRIVATE
            -O2
            -fno-exceptions
    )
endif ()
add_ftl_freestanding_link_check(forward_list_freestanding_link
        checks/forward_list_freestanding_link.cpp)
target_compile_definitions(ftl_check_forward_list_freestanding_link PRIVATE FTL_NO_EXCEPTIONS)
add_ftl_freestanding_link_check(list_freestanding_link
        checks/list_freestanding_link.cpp)
target_compile_definitions(ftl_check_list_freestanding_link PRIVATE FTL_NO_EXCEPTIONS)
if (MSVC)
    target_compile_options(ftl_check_forward_list_freestanding_link PRIVATE /O2 /GS- /EHs-c-)
    target_compile_options(ftl_check_list_freestanding_link PRIVATE /O2 /GS- /EHs-c-)
else ()
    target_compile_options(ftl_check_forward_list_freestanding_link PRIVATE -fno-exceptions -fno-stack-protector)
    target_compile_options(ftl_check_list_freestanding_link PRIVATE -fno-exceptions -fno-stack-protector)
endif ()
add_ftl_freestanding_link_check(ordered_associative_freestanding_link
        checks/ordered_associative_freestanding_link.cpp)
add_ftl_freestanding_link_check(unordered_associative_freestanding_link
        checks/unordered_associative_freestanding_link.cpp)
add_ftl_freestanding_link_check(flat_associative_freestanding_link
        checks/flat_associative_freestanding_link.cpp)
foreach (target IN ITEMS ordered_associative unordered_associative flat_associative)
    target_compile_definitions(ftl_check_${target}_freestanding_link PRIVATE FTL_NO_EXCEPTIONS)
    if (MSVC)
        target_compile_options(ftl_check_${target}_freestanding_link PRIVATE /O2 /GS- /EHs-c-)
    else ()
        target_compile_options(ftl_check_${target}_freestanding_link PRIVATE -fno-exceptions -fno-stack-protector)
    endif ()
endforeach ()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(ftl_check_unordered_associative_freestanding_link
            PRIVATE -fno-builtin-memset)
endif ()
add_ftl_freestanding_link_check(
        bitset_freestanding_link
        checks/bitset_freestanding_link.cpp
)

if (MSVC)
    target_compile_options(
            ftl_check_cstring_freestanding_link
            PRIVATE /O2 /GS-
    )
else ()
    target_compile_options(
            ftl_check_cstring_freestanding_link
            PRIVATE -O3 -fno-stack-protector
    )
endif ()

add_ftl_freestanding_link_check(
        cwchar_freestanding_link
        checks/cwchar_freestanding_link.cpp
)

if (MSVC)
    target_compile_options(
            ftl_check_cwchar_freestanding_link
            PRIVATE
            /O2
            /GS-
    )
else ()
    target_compile_options(
            ftl_check_cwchar_freestanding_link
            PRIVATE
            -O3
            -fno-stack-protector
    )
endif ()
add_ftl_disabled_features_check(disabled_features checks/disabled_features.cpp)

foreach (header IN ITEMS
        cstddef
        type_traits
        utility
        concepts
        new
        memory
        scoped_allocator
        memory_resource
        atomic
        exception
        typeinfo
        typeindex
        any
        expected
        variant
        optional
        initializer_list
        functional
        compare
        array
        cstdint
        cstring
        bit
        bitset
        limits
        ratio
        numbers
        ctime
        chrono
        version
        source_location
        stdfloat
        cassert
        cerrno
        cfloat
        cfenv
        cmath
        cmath_accuracy
        complex
        climits
        cstdarg
        csetjmp
        csignal
        cstdlib
        cinttypes
        c_headers
        string_view
        string
        vector
        deque
        forward_list
        list
        map
        set
        unordered_map
        unordered_set
        flat_map
        flat_set
        queue
        stack
        iterator
        tuple
        coroutine
        span
        ranges
        views
        transform_views
        common_views
        elements_views
        join_views
        split_views
        lazy_split_views
        counted_views
        as_const_views
        repeat_views
        stride_views
        zip_views
        enumerate_views
        zip_transform_views
        adjacent_views
        slide_views
        chunk_by_views
        chunk_views
        cartesian_product_views
        ranges_audit
        basic_const_iterator
        ranges_adaptor_constraints
        ranges_adaptor_callables
        ranges_iterator_interop
        ranges_borrowed_range_propagation
        ranges_sized_range_propagation
        ranges_common_range_propagation
        ranges_iterator_category_propagation
        ranges_conditional_noexcept
        ranges_constexpr
        ranges_adaptor_composition
        ranges_constraints
        generator
        mdspan
        span_mdspan_integration
        generator_ranges_integration
        span_mdspan_generator_ranges_integration
        adjacent_transform_views
        join_with_view
        stdexcept
        system_error
        stacktrace
        iosfwd
        streambuf
        ios
        istream
        istream_view
        execution
        algorithm_ranges_result
        algorithm_all_any_none
        algorithm_count
        algorithm_for_each
        algorithm_find
        algorithm_range_compare
        algorithm_search
        algorithm_copy_move
        algorithm_transform_generate
        algorithm_fill
        algorithm_replace
        algorithm_remove_unique
        algorithm_reverse_rotate
        algorithm_swap_shift
        algorithm_partition
        algorithm_sort
        algorithm_binary_search
        algorithm_merge
        algorithm_set
        algorithm_heap
        algorithm_partial_sort
        algorithm_min_max
        algorithm_fold
        algorithm_permutation
        algorithm_lexicographical
        algorithm_random
        random
        valarray
        numeric
        cstring_synopsis
        cwchar
        cwchar_synopsis
        cwctype
        cwctype_synopsis
        cuchar
        cuchar_synopsis
        charconv
        big_uint
        floating_bits
        floating_conversion
        stop_token
        thread
        stdatomic_h
        mutex
        shared_mutex
        condition_variable
        semaphore
        latch
        barrier
)
    add_ftl_test(${header})
endforeach ()
