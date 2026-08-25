if (NOT EXISTS "${FTL_LINK_MAP}")
    message(FATAL_ERROR "linker map was not produced: ${FTL_LINK_MAP}")
endif ()

file(READ "${FTL_LINK_MAP}" link_map)
string(TOLOWER "${link_map}" link_map)

if (NOT link_map MATCHES "ftl_compiler_runtime")
    message(FATAL_ERROR "FTL compiler runtime is absent from the linker map")
endif ()

if (link_map MATCHES "libstdc\\+\\+|libc\\+\\+[.]|libcpmt|msvcprt")
    message(FATAL_ERROR "vendor C++ standard library found in the linker map")
endif ()
