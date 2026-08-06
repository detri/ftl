#include <ftl/cstddef>
#include <ftl/utility>
#include <ftl/bit>
#include <ftl/source_location>
#include <ftl/stdfloat>
#include <ftl/version>
#include <ftl/any>
#include <ftl/expected>
#include <ftl/optional>
#include <ftl/variant>
#include <ftl/ranges>

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
    __cpp_lib_const_iterator != 202207L || \
    __cpp_lib_move_iterator_concept != 202207L || \
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
#if __cpp_lib_invoke_r != 202106L || __cpp_lib_bind_back != 202202L || \
    __cpp_lib_move_only_function != 202110L
#error <functional> must advertise its completed C++23 facilities
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
#if __cpp_lib_hardware_interference_size != 201703L || \
    __cpp_lib_launder != 201606L || \
    __cpp_lib_uncaught_exceptions != 201411L
#error Stage 1.4 language-support feature macros are incomplete
#endif
#ifdef __cpp_lib_stdfloat
#error N4950 does not define __cpp_lib_stdfloat
#endif
#if __cpp_lib_constexpr_memory != 202202L || \
    __cpp_lib_allocate_at_least != 202302L || \
    __cpp_lib_out_ptr != 202106L || \
    __cpp_lib_start_lifetime_as != 202207L || \
    __cpp_lib_memory_resource != 201603L
#error Stage 2.4 memory-management feature macros are incomplete
#endif
#if __cpp_lib_any != 201606L || __cpp_lib_expected != 202211L || \
    __cpp_lib_optional != 202110L || __cpp_lib_variant != 202106L
#error Stage 2.5 vocabulary feature macros are incomplete
#endif

#if __cpp_lib_ranges != 202110L || __cpp_lib_ranges_zip != 202110L || \
    __cpp_lib_ranges_chunk != 202202L || __cpp_lib_ranges_slide != 202202L || \
    __cpp_lib_ranges_chunk_by != 202202L || __cpp_lib_ranges_stride != 202207L || \
    __cpp_lib_ranges_cartesian_product != 202207L || \
    __cpp_lib_ranges_as_const != 202207L || __cpp_lib_ranges_enumerate != 202302L || \
    __cpp_lib_ranges_repeat != 202207L
#error <ranges> must advertise its completed C++23 facilities
#endif

#if __cpp_lib_mdspan != 202207L
#error <mdspan> must advertise the C++23 mdspan facility
#endif

int main() {}
