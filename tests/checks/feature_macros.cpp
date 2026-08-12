#include <ftl/any>
#include <ftl/atomic>
#include <ftl/barrier>
#include <ftl/bit>
#include <ftl/bitset>
#include <ftl/cstddef>
#include <ftl/expected>
#include <ftl/filesystem>
#include <ftl/ios>
#include <ftl/latch>
#include <ftl/numbers>
#include <ftl/optional>
#include <ftl/ranges>
#include <ftl/semaphore>
#include <ftl/source_location>
#include <ftl/stacktrace>
#include <ftl/stdfloat>
#include <ftl/stop_token>
#include <ftl/thread>
#include <ftl/utility>
#include <ftl/variant>
#include <ftl/version>

#if __cpp_lib_byte < 201603L
#error <cstddef> must advertise std::byte
#endif

#if __cpp_lib_is_invocable < 201703L
#error <type_traits> must advertise invocation traits
#endif
#if __cpp_lib_is_scoped_enum < 202011L
#error <type_traits> must advertise scoped-enum detection
#endif
#if __cpp_lib_integral_constant_callable < 201304L ||                          \
    __cpp_lib_is_null_pointer < 201309L || __cpp_lib_is_final < 201402L ||     \
    __cpp_lib_type_identity < 201806L ||                                       \
    __cpp_lib_move_iterator_concept < 202207L ||                               \
    __cpp_lib_common_reference < 202302L
#error <type_traits> must advertise completed facilities
#endif
#if __cpp_lib_concepts < 202207L
#error <concepts> must advertise standard concepts
#endif
#if __cpp_lib_three_way_comparison < 201907L
#error <compare> must advertise three-way comparison
#endif
#if __cpp_lib_forward_like < 202207L
#error <utility> must advertise forward_like
#endif
#if __cpp_lib_invoke_r < 202106L || __cpp_lib_bind_back < 202202L ||           \
    __cpp_lib_move_only_function < 202110L
#error <functional> must advertise its completed C++23 facilities
#endif
#if __cpp_lib_unreachable < 202202L
#error <utility> must advertise unreachable
#endif
#if __cpp_lib_integer_sequence < 201304L ||                                    \
    __cpp_lib_tuples_by_type < 201304L ||                                      \
    __cpp_lib_constexpr_utility < 201811L
#error <utility> must advertise completed facilities
#endif
#if __cpp_lib_bit_cast < 201806L || __cpp_lib_bitops < 201907L ||              \
    __cpp_lib_byteswap < 202110L
#error <bit> must advertise its facilities
#endif
#if __cpp_lib_constexpr_bitset < 202207L
#error <bitset> must advertise constexpr bitset
#endif
#if __cpp_lib_source_location < 201907L
#error <source_location> must advertise source locations
#endif
#if __cpp_lib_math_constants < 201907L
#error <numbers> must advertise mathematical constants
#endif
#if __cpp_lib_constexpr_cmath < 202202L || __cpp_lib_hypot < 201603L ||        \
    __cpp_lib_interpolate < 201902L ||                                         \
    __cpp_lib_math_special_functions < 201603L
#error <cmath> must advertise its completed facilities
#endif
#if __cpp_lib_complex_udls < 201309L || __cpp_lib_constexpr_complex < 201711L
#error <complex> must advertise its completed facilities
#endif
#if __cpp_lib_hardware_interference_size < 201703L ||                          \
    __cpp_lib_launder < 201606L || __cpp_lib_uncaught_exceptions < 201411L
#error Stage 1.4 language-support feature macros are incomplete
#endif
#ifdef __cpp_lib_stdfloat
#error N4950 does not define __cpp_lib_stdfloat
#endif
#if __cpp_lib_constexpr_memory < 202202L ||                                    \
    __cpp_lib_allocate_at_least < 202302L || __cpp_lib_out_ptr < 202106L ||    \
    __cpp_lib_start_lifetime_as < 202207L ||                                   \
    __cpp_lib_memory_resource < 201603L
#error Stage 2.4 memory-management feature macros are incomplete
#endif
#if __cpp_lib_any < 201606L || __cpp_lib_expected < 202211L ||                 \
    __cpp_lib_optional < 202110L || __cpp_lib_variant < 202106L
#error Stage 2.5 vocabulary feature macros are incomplete
#endif

#if __cpp_lib_ranges < 202302L || __cpp_lib_ranges_zip < 202110L ||            \
    __cpp_lib_ranges_chunk < 202202L || __cpp_lib_ranges_slide < 202202L ||    \
    __cpp_lib_ranges_chunk_by < 202202L ||                                     \
    __cpp_lib_ranges_stride < 202207L ||                                       \
    __cpp_lib_ranges_cartesian_product < 202207L ||                            \
    __cpp_lib_ranges_as_const < 202207L ||                                     \
    __cpp_lib_ranges_enumerate < 202302L ||                                    \
    __cpp_lib_ranges_repeat < 202207L ||                                       \
    __cpp_lib_ranges_as_rvalue < 202207L ||                                    \
    __cpp_lib_ranges_join_with < 202202L ||                                    \
    __cpp_lib_ranges_to_container < 202202L
#error <ranges> must advertise its completed C++23 facilities
#endif

#if __cpp_lib_coroutine < 201902L || __cpp_lib_span < 202002L ||               \
    __cpp_lib_generator < 202207L || __cpp_lib_mdspan < 202207L
#error Stage 2.6 feature macros are incomplete
#endif

#if __cpp_lib_mdspan < 202207L
#error <mdspan> must advertise the C++23 mdspan facility
#endif

#if __cpp_lib_string_view < 201803L ||                                         \
    __cpp_lib_constexpr_string_view < 201811L ||                               \
    __cpp_lib_starts_ends_with < 201711L ||                                    \
    __cpp_lib_string_contains < 202011L
#error Stage 3.3 string_view feature macros are incomplete
#endif

#if __cpp_lib_to_chars < 201611L || __cpp_lib_constexpr_charconv < 202207L
#error Stage 3.3 charconv feature macros are incomplete
#endif

#if __cpp_lib_constexpr_string < 201907L ||                                    \
    __cpp_lib_string_resize_and_overwrite < 202110L ||                         \
    __cpp_lib_containers_ranges < 202202L
#error Stage 3.4 string feature macros are incomplete
#endif

#if __cpp_lib_atomic_float < 201711L ||                                        \
    __cpp_lib_atomic_is_always_lock_free < 201603L ||                          \
    __cpp_lib_atomic_lock_free_type_aliases < 201907L ||                       \
    __cpp_lib_atomic_ref < 201806L || __cpp_lib_atomic_flag_test < 201907L ||  \
    __cpp_lib_atomic_value_initialization < 201911L ||                         \
    __cpp_lib_atomic_wait < 201907L
#error Stage 6.1 atomic feature macros are incomplete
#endif

#if __cpp_lib_jthread < 201911L
#error Stage 6.2 stop_token/thread feature macros are incomplete
#endif

#if __cpp_lib_ios_noreplace < 202207L || __cpp_lib_stacktrace < 202011L ||     \
    __cpp_lib_formatters < 202302L
#error Stage 7.3 stream-owned feature macros are incomplete
#endif

#if __cpp_lib_spanstream < 202106L || __cpp_lib_syncbuf < 201803L
#error Stage 7.5 stream feature macros are incomplete
#endif

#if __cpp_lib_filesystem < 201703L
#error Stage 7.6 filesystem feature macro is incomplete
#endif

#if __cpp_lib_semaphore < 201907L || __cpp_lib_latch < 201907L ||              \
    __cpp_lib_barrier < 202302L
#error Stage 6.4 coordination feature macros are incomplete
#endif

int main() {}
