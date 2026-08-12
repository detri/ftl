#ifdef FTL_REPLACE_STL
#include <version>
#else
#include <ftl/version>
#endif

#define FTL_CHECK_VERSION_MACRO(NAME, VALUE)                                   \
  static_assert(NAME >= VALUE, #NAME " has the wrong value")

FTL_CHECK_VERSION_MACRO(__cpp_lib_adaptor_iterator_pair_constructor, 202106L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_addressof_constexpr, 201603L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_allocator_traits_is_always_equal, 201411L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_associative_heterogeneous_erasure, 202110L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_atomic_flag_test, 201907L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_atomic_ref, 201806L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_atomic_value_initialization, 201911L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_atomic_wait, 201907L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_bit_cast, 201806L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_byteswap, 202110L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_char8_t, 201907L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_chrono, 201907L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_chrono_udls, 201304L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_common_reference, 202302L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_common_reference_wrapper, 202302L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_concepts, 202207L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_constexpr_bitset, 202207L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_constexpr_cmath, 202202L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_constexpr_iterator, 201811L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_constexpr_typeinfo, 202106L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_destroying_delete, 201806L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_erase_if, 202002L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_format, 202207L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_format_ranges, 202207L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_formatters, 202302L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_generic_associative_lookup, 201304L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_generic_unordered_lookup, 201811L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_ios_noreplace, 202207L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_list_remove_return_type, 201806L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_make_reverse_iterator, 201402L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_map_try_emplace, 201411L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_math_constants, 201907L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_node_extract, 201606L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_nonmember_container_access, 201411L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_null_iterators, 201304L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_polymorphic_allocator, 201902L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_quoted_string_io, 201304L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_ranges, 202302L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_result_of_sfinae, 201210L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_scoped_lock, 201703L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_source_location, 201907L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_ssize, 201902L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_stacktrace, 202011L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_string_udls, 201304L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_tuple_like, 202207L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_unordered_map_try_emplace, 201411L);

FTL_CHECK_VERSION_MACRO(__cpp_lib_algorithm_iterator_requirements, 202207L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_constexpr_numeric, 201911L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_coroutine, 201902L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_execution, 201902L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_gcd_lcm, 201606L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_interpolate, 201902L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_parallel_algorithm, 201603L);
FTL_CHECK_VERSION_MACRO(__cpp_lib_ranges_iota, 202202L);

#undef FTL_CHECK_VERSION_MACRO

#ifdef __cpp_lib_stdfloat
#error N4950 does not define __cpp_lib_stdfloat
#endif

#ifdef __cpp_lib_const_iterator
#error N4950 does not define __cpp_lib_const_iterator
#endif

bool ftl_test() { return true; }
