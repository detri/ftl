#include <ftl/cstddef>
#include <ftl/utility>
#include <ftl/bit>
#include <ftl/source_location>
#include <ftl/stdfloat>

#if __cpp_lib_byte != 201603L
#error <cstddef> must advertise std::byte
#endif

#if __cpp_lib_is_invocable != 201703L
#error <type_traits> must advertise invocation traits
#endif
#if __cpp_lib_is_scoped_enum != 202011L
#error <type_traits> must advertise scoped-enum detection
#endif
#if __cpp_lib_integral_constant_callable != 201304L || \
    __cpp_lib_is_null_pointer != 201309L || \
    __cpp_lib_is_final != 201402L || \
    __cpp_lib_type_identity != 201806L || \
    __cpp_lib_common_reference != 202302L
#error <type_traits> must advertise completed facilities
#endif
#if __cpp_lib_concepts != 202002L
#error <concepts> must advertise standard concepts
#endif
#if __cpp_lib_three_way_comparison != 201907L
#error <compare> must advertise three-way comparison
#endif
#if __cpp_lib_forward_like != 202207L
#error <utility> must advertise forward_like
#endif
#if __cpp_lib_unreachable != 202202L
#error <utility> must advertise unreachable
#endif
#if __cpp_lib_integer_sequence != 201304L || \
    __cpp_lib_tuples_by_type != 201304L || \
    __cpp_lib_constexpr_utility != 201811L
#error <utility> must advertise completed facilities
#endif
#if __cpp_lib_bit_cast != 201806L || __cpp_lib_bitops != 201907L || \
    __cpp_lib_byteswap != 202110L
#error <bit> must advertise its facilities
#endif
#if __cpp_lib_source_location != 201907L
#error <source_location> must advertise source locations
#endif
#ifdef __cpp_lib_stdfloat
#error N4950 does not define __cpp_lib_stdfloat
#endif

int main() {}
