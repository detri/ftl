if (NOT DEFINED FTL_SOURCE_DIR)
    message(FATAL_ERROR "FTL_SOURCE_DIR must be provided")
endif ()

function(ftl_require_feature_macro header macro value)
    set(path "${FTL_SOURCE_DIR}/include/ftl/${header}")

    if (NOT EXISTS "${path}")
        message(FATAL_ERROR "missing FTL header: ${header}")
    endif ()

    file(READ "${path}" content)

    string(
            REGEX MATCH
            "#[ \t]*define[ \t]+${macro}[ \t]+${value}([ \t\r\n]|$)"
            match
            "${content}"
    )

    if (match STREQUAL "")
        message(
                FATAL_ERROR
                "<${header}> must directly define ${macro} as ${value}"
        )
    endif ()
endfunction()

function(ftl_require_feature_macros_after header marker)
    set(path "${FTL_SOURCE_DIR}/include/ftl/${header}")
    file(READ "${path}" content)

    string(FIND "${content}" "${marker}" marker_position REVERSE)
    string(FIND "${content}" "__cpp_lib_" macro_position)

    if (marker_position EQUAL -1)
        message(
                FATAL_ERROR
                "<${header}> feature-macro footer marker '${marker}' was not found"
        )
    endif ()

    if (macro_position EQUAL -1)
        message(
                FATAL_ERROR
                "<${header}> has no library feature-test macros"
        )
    endif ()

    if (macro_position LESS marker_position)
        message(
                FATAL_ERROR
                "<${header}> defines a library feature-test macro before its footer"
        )
    endif ()
endfunction()

# ---------------------------------------------------------------------------
# Cross-header / foundation exports added by the N4950 feature-macro closure.
# ---------------------------------------------------------------------------

ftl_require_feature_macro(
        algorithm
        __cpp_lib_ranges
        202302L
)

ftl_require_feature_macro(
        array
        __cpp_lib_nonmember_container_access
        201411L
)

ftl_require_feature_macro(
        compare
        __cpp_lib_concepts
        202207L
)

ftl_require_feature_macro(
        functional
        __cpp_lib_common_reference_wrapper
        202302L
)
ftl_require_feature_macro(
        functional
        __cpp_lib_ranges
        202302L
)
ftl_require_feature_macro(
        functional
        __cpp_lib_result_of_sfinae
        201210L
)

ftl_require_feature_macro(
        iterator
        __cpp_lib_array_constexpr
        201811L
)
ftl_require_feature_macro(
        iterator
        __cpp_lib_make_reverse_iterator
        201402L
)
ftl_require_feature_macro(
        iterator
        __cpp_lib_null_iterators
        201304L
)
ftl_require_feature_macro(
        iterator
        __cpp_lib_ranges
        202302L
)

ftl_require_feature_macro(
        memory
        __cpp_lib_addressof_constexpr
        201603L
)
ftl_require_feature_macro(
        memory
        __cpp_lib_allocator_traits_is_always_equal
        201411L
)
ftl_require_feature_macro(
        memory
        __cpp_lib_atomic_value_initialization
        201911L
)
ftl_require_feature_macro(
        memory
        __cpp_lib_ranges
        202302L
)
ftl_require_feature_macro(
        memory
        __cpp_lib_transparent_operators
        201510L
)

ftl_require_feature_macro(
        memory_resource
        __cpp_lib_polymorphic_allocator
        201902L
)

ftl_require_feature_macro(
        tuple
        __cpp_lib_ranges_zip
        202110L
)
ftl_require_feature_macro(
        tuple
        __cpp_lib_tuple_element_t
        201402L
)
ftl_require_feature_macro(
        tuple
        __cpp_lib_tuple_like
        202207L
)
ftl_require_feature_macro(
        tuple
        __cpp_lib_tuples_by_type
        201304L
)

ftl_require_feature_macro(
        type_traits
        __cpp_lib_result_of_sfinae
        201210L
)

ftl_require_feature_macro(
        utility
        __cpp_lib_constexpr_algorithms
        201806L
)
ftl_require_feature_macro(
        utility
        __cpp_lib_ranges_zip
        202110L
)
ftl_require_feature_macro(
        utility
        __cpp_lib_tuple_like
        202207L
)

# ---------------------------------------------------------------------------
# char8_t exports.
# ---------------------------------------------------------------------------

foreach (header IN ITEMS
        atomic
        filesystem
        istream
        limits
        locale
        ostream
        string
        string_view
)
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_char8_t
            201907L
    )
endforeach ()

# ---------------------------------------------------------------------------
# Strings / formatting / streams.
# ---------------------------------------------------------------------------

ftl_require_feature_macro(
        ostream
        __cpp_lib_print
        202207L
)

ftl_require_feature_macro(
        string
        __cpp_lib_allocator_traits_is_always_equal
        201411L
)
ftl_require_feature_macro(
        string
        __cpp_lib_erase_if
        202002L
)
ftl_require_feature_macro(
        string
        __cpp_lib_nonmember_container_access
        201411L
)
ftl_require_feature_macro(
        string
        __cpp_lib_starts_ends_with
        201711L
)
ftl_require_feature_macro(
        string
        __cpp_lib_string_udls
        201304L
)
ftl_require_feature_macro(
        string
        __cpp_lib_string_view
        201803L
)

ftl_require_feature_macro(
        format
        __cpp_lib_format
        202207L
)
ftl_require_feature_macro(
        format
        __cpp_lib_format_ranges
        202207L
)

ftl_require_feature_macro(
        iomanip
        __cpp_lib_quoted_string_io
        201304L
)

# ---------------------------------------------------------------------------
# Sequence containers.
# ---------------------------------------------------------------------------

ftl_require_feature_macro(
        scoped_allocator
        __cpp_lib_allocator_traits_is_always_equal
        201411L
)

foreach (header IN ITEMS
        deque
        forward_list
        list
        vector
)
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_allocator_traits_is_always_equal
            201411L
    )
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_erase_if
            202002L
    )
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_nonmember_container_access
            201411L
    )
endforeach ()

foreach (header IN ITEMS
        deque
        forward_list
        list
)
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_containers_ranges
            202202L
    )
endforeach ()

foreach (header IN ITEMS
        forward_list
        list
)
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_incomplete_container_elements
            201505L
    )
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_list_remove_return_type
            201806L
    )
endforeach ()

# ---------------------------------------------------------------------------
# Associative containers.
# ---------------------------------------------------------------------------

foreach (header IN ITEMS
        map
        set
        unordered_map
        unordered_set
)
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_allocator_traits_is_always_equal
            201411L
    )
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_associative_heterogeneous_erasure
            202110L
    )
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_containers_ranges
            202202L
    )
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_erase_if
            202002L
    )
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_node_extract
            201606L
    )
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_nonmember_container_access
            201411L
    )
endforeach ()

foreach (header IN ITEMS
        map
        set
)
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_generic_associative_lookup
            201304L
    )
endforeach ()

ftl_require_feature_macro(
        map
        __cpp_lib_map_try_emplace
        201411L
)

foreach (header IN ITEMS
        unordered_map
        unordered_set
)
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_generic_unordered_lookup
            201811L
    )
endforeach ()

foreach (header IN ITEMS
        map
        unordered_map
)
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_tuple_like
            202207L
    )
endforeach ()

ftl_require_feature_macro(
        unordered_map
        __cpp_lib_unordered_map_try_emplace
        201411L
)

# ---------------------------------------------------------------------------
# Container adaptors.
# ---------------------------------------------------------------------------

foreach (header IN ITEMS
        queue
        stack
)
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_adaptor_iterator_pair_constructor
            202106L
    )
    ftl_require_feature_macro(
            ${header}
            __cpp_lib_containers_ranges
            202202L
    )
endforeach ()

# ---------------------------------------------------------------------------
# Remaining exports.
# ---------------------------------------------------------------------------

ftl_require_feature_macro(
        mutex
        __cpp_lib_scoped_lock
        201703L
)

ftl_require_feature_macro(
        new
        __cpp_lib_destroying_delete
        201806L
)

ftl_require_feature_macro(
        regex
        __cpp_lib_nonmember_container_access
        201411L
)

# ---------------------------------------------------------------------------
# Existing macros that were relocated to footer position in this closure.
# ---------------------------------------------------------------------------

ftl_require_feature_macro(
        ios
        __cpp_lib_ios_noreplace
        202207L
)
ftl_require_feature_macros_after(
        ios
        "FTL_END_NAMESPACE"
)

ftl_require_feature_macro(
        span
        __cpp_lib_span
        202002L
)
ftl_require_feature_macros_after(
        span
        "FTL_END_NAMESPACE"
)

ftl_require_feature_macro(
        thread
        __cpp_lib_formatters
        202302L
)
ftl_require_feature_macros_after(
        thread
        "FTL_THREAD_END_NAMESPACE"
)

ftl_require_feature_macro(
        complex
        __cpp_lib_complex_udls
        201309L
)
ftl_require_feature_macro(
        complex
        __cpp_lib_constexpr_complex
        201711L
)
ftl_require_feature_macros_after(
        complex
        "FTL_COMPLEX_END"
)

ftl_require_feature_macro(
        cstdlib
        __cpp_lib_constexpr_cmath
        202202L
)
ftl_require_feature_macros_after(
        cstdlib
        "FTL_CSTDLIB_END"
)

ftl_require_feature_macro(
        cmath
        __cpp_lib_constexpr_cmath
        202202L
)
ftl_require_feature_macro(
        cmath
        __cpp_lib_hypot
        201603L
)
ftl_require_feature_macro(
        cmath
        __cpp_lib_interpolate
        201902L
)
ftl_require_feature_macro(
        cmath
        __cpp_lib_math_special_functions
        201603L
)
ftl_require_feature_macros_after(
        cmath
        "FTL_CMATH_END"
)

ftl_require_feature_macro(
        syncstream
        __cpp_lib_syncbuf
        201803L
)
ftl_require_feature_macros_after(
        syncstream
        "FTL_END_NAMESPACE"
)

ftl_require_feature_macro(
        stacktrace
        __cpp_lib_stacktrace
        202011L
)
ftl_require_feature_macro(
        stacktrace
        __cpp_lib_formatters
        202302L
)
ftl_require_feature_macros_after(
        stacktrace
        "FTL_END_NAMESPACE"
)

ftl_require_feature_macro(
        spanstream
        __cpp_lib_spanstream
        202106L
)
ftl_require_feature_macros_after(
        spanstream
        "FTL_END_NAMESPACE"
)

ftl_require_feature_macro(
        shared_mutex
        __cpp_lib_shared_timed_mutex
        201402L
)
ftl_require_feature_macro(
        shared_mutex
        __cpp_lib_shared_mutex
        201505L
)
ftl_require_feature_macros_after(
        shared_mutex
        "FTL_SHARED_MUTEX_END_NAMESPACE"
)

ftl_require_feature_macro(
        flat_set
        __cpp_lib_flat_set
        202207L
)
ftl_require_feature_macros_after(
        flat_set
        "FTL_END_NAMESPACE"
)

ftl_require_feature_macro(
        flat_map
        __cpp_lib_flat_map
        202207L
)
ftl_require_feature_macros_after(
        flat_map
        "FTL_END_NAMESPACE"
)

ftl_require_feature_macro(
        filesystem
        __cpp_lib_filesystem
        201703L
)
ftl_require_feature_macros_after(
        filesystem
        "FTL_END_NAMESPACE"
)

# ---------------------------------------------------------------------------
# Obsolete/non-N4950 advertisement must not survive anywhere in public headers.
# ---------------------------------------------------------------------------

file(
        GLOB_RECURSE
        ftl_public_files
        LIST_DIRECTORIES FALSE
        "${FTL_SOURCE_DIR}/include/ftl/*"
)

foreach (path IN LISTS ftl_public_files)
    file(READ "${path}" content)

    string(
            FIND
            "${content}"
            "__cpp_lib_const_iterator"
            obsolete_position
    )

    if (NOT obsolete_position EQUAL -1)
        message(
                FATAL_ERROR
                "obsolete __cpp_lib_const_iterator remains in ${path}"
        )
    endif ()
endforeach ()
