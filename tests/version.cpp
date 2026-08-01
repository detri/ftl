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
#if __cpp_lib_stdfloat != 202306L
#error wrong stdfloat feature-test macro
#endif

bool ftl_test() { return true; }
