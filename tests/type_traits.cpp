#ifdef FTL_REPLACE_STL
#include <type_traits>
namespace tested = std;
#else
#include <ftl/type_traits>
namespace tested = ftl;
#endif

#if __cpp_lib_is_invocable != 201703L || \
    __cpp_lib_is_scoped_enum != 202011L
#error invalid <type_traits> feature-test macros
#endif

static_assert(tested::is_same_v<tested::remove_cvref_t<const int&>, int>);
static_assert(tested::integral_constant<int, 3>::value == 3);
static_assert(tested::is_void_v<const void>);
static_assert(tested::is_null_pointer_v<decltype(nullptr)>);
static_assert(tested::is_integral_v<const unsigned long long>);
static_assert(tested::is_floating_point_v<volatile long double>);
static_assert(tested::is_array_v<int[]> && tested::is_pointer_v<int*>);
static_assert(tested::is_lvalue_reference_v<int&> &&
              tested::is_rvalue_reference_v<int&&>);
static_assert(tested::is_function_v<int(double)>);
static_assert(tested::is_reference_v<int&> && tested::is_object_v<int>);
static_assert(tested::is_arithmetic_v<double> &&
              tested::is_fundamental_v<void> &&
              tested::is_scalar_v<int*> && tested::is_compound_v<int*>);
static_assert(tested::alignment_of_v<int> == alignof(int));
static_assert(tested::is_same_v<typename tested::rank<int[2]>::value_type,
                                tested::size_t>);
static_assert(tested::extent_v<int[2][3], 1> == 3);
using abominable_function = int() const;
static_assert(tested::is_same_v<tested::add_pointer_t<abominable_function>,
                                abominable_function>);
static_assert(tested::is_same_v<tested::make_signed_t<unsigned>, int>);
static_assert(tested::is_same_v<tested::make_unsigned_t<int>, unsigned>);
static_assert(tested::is_trivially_default_constructible_v<int>);
static_assert(tested::is_trivially_copy_constructible_v<int>);
static_assert(tested::is_trivially_move_assignable_v<int>);
static_assert(tested::is_const_v<const int> &&
              tested::is_volatile_v<volatile int>);
static_assert(tested::is_trivial_v<int> &&
              tested::is_trivially_copyable_v<int> &&
              tested::is_standard_layout_v<int>);
struct empty_type {};
struct polymorphic_type { virtual ~polymorphic_type() = default; };
struct abstract_type { virtual void call() = 0; };
struct final_type final {};
struct aggregate_type { int value; };
union union_type { int value; double other; };
struct base_type { int member; void function() {} };
struct derived_type : base_type {};
struct pointer_base {};
struct pointer_derived : pointer_base { int value; };
enum enum_type { enum_value };
static_assert(tested::is_empty_v<empty_type>);
static_assert(tested::is_polymorphic_v<polymorphic_type>);
static_assert(tested::is_abstract_v<abstract_type>);
static_assert(tested::is_final_v<final_type>);
static_assert(tested::is_aggregate_v<aggregate_type>);
static_assert(tested::is_union_v<union_type> && tested::is_class_v<base_type> &&
              tested::is_enum_v<enum_type>);
static_assert(tested::is_member_object_pointer_v<decltype(&base_type::member)> &&
              tested::is_member_function_pointer_v<
                  decltype(&base_type::function)> &&
              tested::is_member_pointer_v<decltype(&base_type::member)>);
static_assert(tested::is_base_of_v<base_type, derived_type> &&
              tested::is_convertible_v<derived_type*, base_type*> &&
              !tested::is_convertible_v<base_type*, derived_type*>);
static_assert(tested::is_pointer_interconvertible_base_of_v<
              pointer_base, pointer_derived>);
static_assert(tested::has_virtual_destructor_v<polymorphic_type>);
static_assert(tested::has_unique_object_representations_v<unsigned char>);
static_assert(tested::is_bounded_array_v<int[1]> &&
              tested::is_unbounded_array_v<int[]>);
static_assert(tested::is_constructible_v<int, short> &&
              tested::is_default_constructible_v<int> &&
              tested::is_copy_constructible_v<int> &&
              tested::is_move_constructible_v<int>);
static_assert(tested::is_assignable_v<int&, int> &&
              tested::is_copy_assignable_v<int> &&
              tested::is_move_assignable_v<int>);
static_assert(tested::is_destructible_v<int> &&
              tested::is_trivially_destructible_v<int> &&
              tested::is_nothrow_destructible_v<int>);
static_assert(tested::is_nothrow_default_constructible_v<int> &&
              tested::is_nothrow_copy_constructible_v<int> &&
              tested::is_nothrow_move_constructible_v<int> &&
              tested::is_nothrow_copy_assignable_v<int> &&
              tested::is_nothrow_move_assignable_v<int>);
#ifdef __cpp_lib_is_implicit_lifetime
static_assert(tested::is_implicit_lifetime_v<int>);
#endif
static_assert(tested::reference_constructs_from_temporary_v<const int&, int>);
static_assert(tested::reference_converts_from_temporary_v<const int&, int>);
static_assert(tested::is_nothrow_convertible_v<int, long>);
static_assert(tested::is_nothrow_convertible_v<void, const void>);
static_assert(!tested::is_nothrow_convertible_v<void, int>);
static_assert(tested::is_swappable_v<int>);
static_assert(tested::is_nothrow_swappable_v<int[2]>);
struct immobile {
    immobile(immobile&&) = delete;
    immobile& operator=(immobile&&) = delete;
};
static_assert(!tested::is_swappable_v<immobile>);
static_assert(tested::is_same_v<tested::common_type_t<int, const long>, long>);
static_assert(tested::is_same_v<tested::common_reference_t<int&, const int&>,
                                const int&>);
static_assert(tested::is_same_v<tested::common_reference_t<int&&, int&>,
                                const int&>);
static_assert(tested::is_same_v<tested::unwrap_ref_decay_t<int&>, int>);
static_assert(tested::is_same_v<
              tested::unwrap_reference_t<tested::reference_wrapper<int>>, int&>);
static_assert(tested::is_same_v<tested::remove_const_t<const int>, int> &&
              tested::is_same_v<tested::remove_volatile_t<volatile int>, int> &&
              tested::is_same_v<tested::remove_cv_t<const volatile int>, int>);
static_assert(tested::is_same_v<tested::add_cv_t<int>, const volatile int> &&
              tested::is_same_v<tested::add_lvalue_reference_t<void>, void> &&
              tested::is_same_v<tested::add_rvalue_reference_t<int>, int&&>);
static_assert(tested::is_same_v<tested::remove_extent_t<int[2]>, int> &&
              tested::is_same_v<tested::remove_all_extents_t<int[2][3]>, int> &&
              tested::is_same_v<tested::remove_pointer_t<int* const>, int> &&
              tested::is_same_v<tested::add_pointer_t<int&>, int*>);
static_assert(tested::is_same_v<tested::decay_t<const int&>, int> &&
              tested::is_same_v<tested::enable_if_t<true, int>, int> &&
              tested::is_same_v<tested::conditional_t<false, void, int>, int> &&
              tested::is_same_v<tested::type_identity_t<int>, int> &&
              tested::is_same_v<tested::void_t<int, void>, void>);
static_assert(tested::conjunction_v<tested::true_type, tested::true_type> &&
              tested::disjunction_v<tested::false_type, tested::true_type> &&
              tested::negation_v<tested::false_type>);

template<class... T>
concept has_common_type = requires { typename tested::common_type_t<T...>; };
template<class... T>
concept has_common_reference = requires {
    typename tested::common_reference_t<T...>;
};
static_assert(has_common_type<int, short, long>);
static_assert(!has_common_type<int, void, long>);
static_assert(has_common_reference<int&, const int&, volatile int&>);

enum plain_enum { plain_value };
enum class scoped_enum { scoped_value };
static_assert(!tested::is_scoped_enum_v<plain_enum>);
static_assert(tested::is_scoped_enum_v<scoped_enum>);
static_assert(tested::is_same_v<tested::make_unsigned_t<scoped_enum>,
                                unsigned>);
static_assert(tested::is_same_v<tested::make_signed_t<const scoped_enum>,
                                const int>);

struct invocable_type {
    int value;
    constexpr int add(int amount) const noexcept { return value + amount; }
};
static_assert(tested::is_invocable_v<decltype(&invocable_type::add),
                                     invocable_type, int>);
static_assert(tested::is_nothrow_invocable_r_v<
              long, decltype(&invocable_type::add), invocable_type, int>);
static_assert(tested::is_same_v<tested::invoke_result_t<
              decltype(&invocable_type::value), invocable_type>, int&&>);

struct explicit_result { explicit constexpr operator int() const { return 0; } };
struct throwing_result { constexpr operator int() const noexcept(false) { return 0; } };
constexpr explicit_result returns_explicit() { return {}; }
constexpr throwing_result returns_throwing() { return {}; }
static_assert(!tested::is_invocable_r_v<int, decltype(returns_explicit)>);
static_assert(tested::is_invocable_r_v<int, decltype(returns_throwing)>);
static_assert(!tested::is_nothrow_invocable_r_v<int,
                                                decltype(returns_throwing)>);

struct layout_one { int first; int second; };
struct layout_two { int first; int second; };
static_assert(tested::is_layout_compatible_v<layout_one, layout_two>);
static_assert(tested::is_pointer_interconvertible_with_class(
              &layout_one::first));
static_assert(!tested::is_pointer_interconvertible_with_class(
              &layout_one::second));
static_assert(tested::is_corresponding_member(&layout_one::first,
                                              &layout_two::first));

bool ftl_test()
{
    return true;
}
