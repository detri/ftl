#ifdef FTL_REPLACE_STL
#include <concepts>
namespace tested = std;
#else
#include <ftl/concepts>
namespace tested = ftl;
#endif

#if __cpp_lib_concepts != 202002L
#error invalid <concepts> feature-test macro
#endif

struct base {};
struct derived : base {};
static_assert(tested::same_as<tested::remove_cvref_t<const int&>, int>);
static_assert(tested::integral<int>);
static_assert(tested::signed_integral<int>);
static_assert(tested::unsigned_integral<unsigned>);
static_assert(tested::floating_point<double>);
static_assert(tested::derived_from<derived, base>);
static_assert(tested::convertible_to<int, long>);
static_assert(tested::common_reference_with<int&, const int&>);
static_assert(tested::common_with<int, long>);
static_assert(tested::default_initializable<int>);
static_assert(tested::copy_constructible<int>);
static_assert(tested::assignable_from<int&, int>);
static_assert(tested::equality_comparable<int>);
static_assert(tested::totally_ordered<int>);
static_assert(tested::swappable<int>);
static_assert(tested::movable<int>);
static_assert(tested::copyable<int>);
static_assert(tested::semiregular<int>);
static_assert(tested::regular<int>);
static_assert(tested::invocable<int(*)(int), int>);
static_assert(tested::predicate<bool(*)(int), int>);
static_assert(tested::relation<bool(*)(int, int), int, int>);
static_assert(tested::equivalence_relation<bool(*)(int, int), int, int>);
static_assert(tested::strict_weak_order<bool(*)(int, int), int, int>);

bool ftl_test()
{
    return true;
}
