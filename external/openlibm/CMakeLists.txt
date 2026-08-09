file(GLOB ftl_openlibm_real_sources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/bsdsrc/*.c")

# C++ <cmath> does not expose the POSIX Bessel family. Complex kernels are
# adapted against FTL's own complex type in a separate translation unit.
list(FILTER ftl_openlibm_real_sources EXCLUDE REGEX
        "[/\\\\](common|e_j0f?|e_j1f?|e_jnf?|k_expf?|s_nan|s_signgam|w_cabsf?)\\.c$")
list(FILTER ftl_openlibm_real_sources EXCLUDE REGEX
        "[/\\\\]s_c(abs|acos|acosh|arg|asin|asinh|atan|atanh|cos|cosh|exp|imag|log|onj|pow|proj|real|sin|sinh|sqrt|tan|tanh)[fl]?\\.c$")

# Long-double implementations are selected below. This first list is the
# shared binary32/binary64 core.
list(FILTER ftl_openlibm_real_sources EXCLUDE REGEX "l\\.c$")
list(FILTER ftl_openlibm_real_sources EXCLUDE REGEX "s_nexttoward\\.c$")
# These binary64 names happen to end in "l.c" but are not long-double files.
list(APPEND ftl_openlibm_real_sources
        src/s_ceil.c
        src/s_isnormal.c)

add_library(ftl_math_runtime STATIC
        ${ftl_openlibm_real_sources}
        src/ftl_constants.cpp
        src/ftl_adapters.cpp
        src/ftl_complex_adapters.cpp)
set_source_files_properties(${ftl_openlibm_real_sources} PROPERTIES
        LANGUAGE CXX)

target_compile_features(ftl_math_runtime PRIVATE cxx_std_23)
target_compile_definitions(ftl_math_runtime PRIVATE
        FTL_OPENLIBM_INTERNAL
        OPENLIBM_ONLY_THREAD_SAFE
        _ANSI_SOURCE
        __BSD_VISIBLE=1)
target_include_directories(ftl_math_runtime PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/bsdsrc"
        "${CMAKE_CURRENT_SOURCE_DIR}/ld80")

if (MSVC)
    target_compile_options(ftl_math_runtime PRIVATE /fp:strict /GS-)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(ftl_math_runtime PRIVATE
                /clang:-fno-builtin
                /clang:-fno-builtin-memset)
    endif ()
else ()
    target_compile_options(ftl_math_runtime PRIVATE
            -ffp-contract=off
            -fno-fast-math
            -fno-builtin
            -fno-strict-aliasing
            -fno-stack-protector
            -fno-exceptions
            -fno-unwind-tables
            -fno-asynchronous-unwind-tables)
endif ()

set(ftl_openlibm_complex_sources
        src/k_exp.c src/k_expf.c
        src/s_carg.c src/s_cargf.c src/s_csqrt.c src/s_csqrtf.c
        src/s_cpow.c src/s_cpowf.c src/w_cabs.c src/w_cabsf.c
        src/s_ccosh.c src/s_ccoshf.c src/s_cexp.c src/s_cexpf.c
        src/s_cimag.c src/s_cimagf.c src/s_conj.c src/s_conjf.c
        src/s_cproj.c src/s_cprojf.c src/s_creal.c src/s_crealf.c
        src/s_csinh.c src/s_csinhf.c src/s_ctanh.c src/s_ctanhf.c
        src/s_cacos.c src/s_cacosf.c src/s_cacosh.c src/s_cacoshf.c
        src/s_casin.c src/s_casinf.c src/s_casinh.c src/s_casinhf.c
        src/s_catan.c src/s_catanf.c src/s_catanh.c src/s_catanhf.c
        src/s_clog.c src/s_clogf.c)
target_sources(ftl_math_runtime PRIVATE ${ftl_openlibm_complex_sources})
set_source_files_properties(${ftl_openlibm_complex_sources}
        PROPERTIES LANGUAGE CXX)

# The supported x86-64 GCC/Clang ABIs use 80-bit extended precision.
# MSVC and Apple AArch64 use binary64 for long double and route those public
# overloads to the double kernels without compiling duplicate sources.
if (NOT MSVC AND NOT (APPLE AND
        CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$"))
    set(ftl_openlibm_long_double_sources
            src/s_copysignl.c src/s_fabsl.c src/s_llrintl.c src/s_lrintl.c
            src/s_modfl.c src/e_acosl.c src/e_asinl.c src/e_atan2l.c
            src/e_fmodl.c src/s_fmaxl.c src/s_fminl.c src/s_ilogbl.c
            src/e_hypotl.c src/e_lgammal.c src/e_remainderl.c src/e_sqrtl.c
            src/s_atanl.c src/s_ceill.c src/s_floorl.c src/s_fmal.c
            src/s_frexpl.c src/s_logbl.c src/s_nexttoward.c src/s_remquol.c
            src/s_roundl.c src/s_lroundl.c src/s_llroundl.c src/s_sinl.c
            src/s_cosl.c
            src/s_sincosl.c src/s_tanl.c src/s_truncl.c
            src/s_nextafterl.c src/s_rintl.c src/s_scalbnl.c src/polevll.c
            src/s_cbrtl.c
            ld80/invtrig.c ld80/e_acoshl.c ld80/e_powl.c ld80/k_tanl.c
            ld80/s_exp2l.c ld80/e_atanhl.c ld80/e_lgammal_r.c
            ld80/e_sinhl.c ld80/s_asinhl.c ld80/s_expm1l.c ld80/e_coshl.c
            ld80/e_log10l.c ld80/e_tgammal.c ld80/e_expl.c ld80/e_log2l.c
            ld80/k_cosl.c ld80/s_log1pl.c ld80/s_tanhl.c ld80/e_logl.c
            ld80/k_sinl.c ld80/s_erfl.c
            src/s_cargl.c src/s_csqrtl.c src/s_cpowl.c src/w_cabsl.c
            src/s_cprojl.c src/s_cimagl.c src/s_conjl.c src/s_creall.c
            src/s_casinl.c src/s_ctanl.c src/s_cacoshl.c src/s_catanhl.c
            src/s_casinhl.c src/s_catanl.c src/s_csinl.c src/s_cacosl.c
            src/s_cexpl.c src/s_csinhl.c src/s_ccoshl.c src/s_clogl.c
            src/s_ctanhl.c src/s_ccosl.c)
    target_sources(ftl_math_runtime PRIVATE
            ${ftl_openlibm_long_double_sources})
    set_source_files_properties(${ftl_openlibm_long_double_sources}
            PROPERTIES LANGUAGE CXX)
endif ()
