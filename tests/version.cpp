#ifdef FTL_REPLACE_STL
#include <version>
#else
#include <ftl/version>
#endif

#if __cpp_lib_bit_cast != 201806L
#error wrong bit_cast feature-test macro
#endif
#if __cpp_lib_byteswap != 202110L
#error wrong byteswap feature-test macro
#endif
#if __cpp_lib_source_location != 201907L
#error wrong source_location feature-test macro
#endif
#if __cpp_lib_integral_constant_callable != 201304L || \
    __cpp_lib_integer_sequence != 201304L || \
    __cpp_lib_type_identity != 201806L || \
    __cpp_lib_common_reference != 202302L
#error missing completed-foundation feature-test macro
#endif
#ifdef __cpp_lib_stdfloat
#error N4950 does not define __cpp_lib_stdfloat
#endif
#ifndef __cpp_lib_coroutine
#error "__cpp_lib_coroutine must be defined"
#endif
#ifndef __cpp_lib_execution
#error "__cpp_lib_execution must be defined"
#endif

static_assert(__cpp_lib_execution >= 201902L);

static_assert(__cpp_lib_coroutine >= 201902L);

static_assert(__cpp_lib_algorithm_iterator_requirements >= 202207L);
static_assert(__cpp_lib_constexpr_numeric >= 201911L);
static_assert(__cpp_lib_gcd_lcm >= 201606L);
static_assert(__cpp_lib_interpolate >= 201902L);
static_assert(__cpp_lib_parallel_algorithm >= 201603L);
static_assert(__cpp_lib_ranges_iota >= 202202L);

bool ftl_test() { return true; }
