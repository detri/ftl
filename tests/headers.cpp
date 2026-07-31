#ifdef FTL_REPLACE_STL
#include <concepts>
namespace tested = std;
#else
#include <ftl/concepts>
namespace tested = ftl;
#endif

enum class value : unsigned char { one = 1 };
struct base {};
struct derived : base {};
struct empty {};
struct compressed {
    FTL_NO_UNIQUE_ADDRESS empty value;
    int payload;
};

static_assert(tested::same_as<tested::remove_cvref_t<const int&>, int>);
static_assert(sizeof(compressed) == sizeof(int));
static_assert(tested::integral<int>);
static_assert(tested::signed_integral<int>);
static_assert(tested::unsigned_integral<unsigned>);
static_assert(tested::floating_point<double>);
static_assert(tested::derived_from<derived, base>);
static_assert(tested::convertible_to<int, long>);
static_assert(tested::default_initializable<int>);
static_assert(tested::copy_constructible<int>);
static_assert(tested::assignable_from<int&, int>);
static_assert(tested::equality_comparable<int>);
static_assert(tested::totally_ordered<int>);
static_assert(tested::swappable<int>);
static_assert(tested::to_underlying(value::one) == 1);
static_assert(tested::is_same_v<decltype(sizeof(0)), tested::size_t>);
static_assert(tested::to_integer<unsigned>(tested::byte{3} << 1) == 6);
static_assert(alignof(tested::max_align_t) >= alignof(long double));

constexpr bool utility_works() {
    int a = 1, b = 2;
    tested::swap(a, b);
    return a == 2 && b == 1 && tested::exchange(a, 3) == 2 && a == 3;
}

static_assert(utility_works());

bool ftl_test() { return utility_works(); }
