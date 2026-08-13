#ifdef FTL_REPLACE_STL
#include <ratio>
namespace tested = std;
#else
#include <ftl/ratio>
namespace tested = ftl;
#endif

static_assert(tested::ratio<2, -4>::num == -1);
static_assert(tested::ratio<2, -4>::den == 2);
static_assert(tested::ratio_equal_v<tested::ratio<1, 2>, tested::ratio<2, 4>>);
static_assert(tested::ratio_less_v<tested::ratio<-2, 3>, tested::ratio<-1, 2>>);
static_assert(
    tested::ratio_add<tested::ratio<1, 6>, tested::ratio<1, 3>>::num == 1);
static_assert(
    tested::ratio_add<tested::ratio<1, 6>, tested::ratio<1, 3>>::den == 2);
static_assert(
    tested::ratio_multiply<tested::ratio<2, 3>, tested::ratio<9, 4>>::num ==
    3);
static_assert(
    tested::ratio_divide<tested::ratio<2, 3>, tested::ratio<4, 9>>::num == 3);

using half = tested::ratio<1, 2>;
using two_fourths = tested::ratio<2, 4>;
static_assert(tested::ratio_equal<half, two_fourths>::value);
static_assert(!tested::ratio_not_equal<half, two_fourths>::value);
static_assert(tested::ratio_less<half, tested::ratio<2, 3>>::value);
static_assert(tested::ratio_less_equal<half, half>::value);
static_assert(tested::ratio_greater<tested::ratio<2, 3>, half>::value);
static_assert(tested::ratio_greater_equal<half, half>::value);

static_assert(tested::nano::den == 1000000000);
static_assert(tested::kilo::num == 1000);

bool ftl_test() { return true; }
