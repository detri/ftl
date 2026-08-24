function(ftl_replace_stl target)
    if (NOT TARGET "${target}")
        message(FATAL_ERROR "ftl_replace_stl: '${target}' is not a target")
    endif ()

    get_target_property(aliased_target "${target}" ALIASED_TARGET)
    if (aliased_target)
        set(target "${aliased_target}")
    endif ()

    get_target_property(imported "${target}" IMPORTED)
    if (imported)
        string(MAKE_C_IDENTIFIER "${target}_ftl" replacement_target)
        if (NOT TARGET "${replacement_target}")
            add_library("${replacement_target}" INTERFACE)
            target_link_libraries("${replacement_target}" INTERFACE "${target}")
        endif ()
        set(target "${replacement_target}")
        set(scope INTERFACE)
        message(STATUS
                "ftl_replace_stl: use '${target}' in place of the imported target")
    else ()
        get_target_property(type "${target}" TYPE)
        if (type STREQUAL "INTERFACE_LIBRARY")
            set(scope INTERFACE)
        elseif (type STREQUAL "EXECUTABLE")
            set(scope PRIVATE)
        else ()
            set(scope PUBLIC)
        endif ()
    endif ()

    target_compile_definitions("${target}" ${scope} FTL_REPLACE_STL)
    target_include_directories(
            "${target}"
            BEFORE ${scope}
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../include/ftl"
    )
    target_link_libraries("${target}" ${scope} ftl ftl_compiler_runtime)

    if (MSVC)
        target_compile_options("${target}" ${scope} /X)
        target_link_options(
                "${target}"
                ${scope}
                /NODEFAULTLIB:libcpmt
                /NODEFAULTLIB:libcpmtd
                /NODEFAULTLIB:msvcprt
                /NODEFAULTLIB:msvcprtd
        )
    else ()
        target_compile_options("${target}" ${scope} -nostdinc++)
        target_link_options("${target}" ${scope} -nostdlib++)

        if (APPLE)
            target_link_libraries("${target}" ${scope} c++abi)
        else ()
            target_link_libraries("${target}" ${scope} supc++)
        endif ()
    endif ()
endfunction()
