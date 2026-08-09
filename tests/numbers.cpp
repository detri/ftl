#ifdef FTL_REPLACE_STL
#include <numbers>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/numbers>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(__cpp_lib_math_constants >= 201907L);
static_assert(tested::is_same_v<decltype(tested::numbers::pi), const double>);
static_assert(tested::is_same_v<decltype(tested::numbers::pi_v<float>),
                                const float>);
static_assert(tested::numbers::pi > 3.14 && tested::numbers::pi < 3.15);
static_assert(tested::numbers::sqrt2_v<long double> *
                  tested::numbers::sqrt2_v<long double> >
              1.99L);
static_assert(tested::numbers::sqrt2_v<long double> *
                  tested::numbers::sqrt2_v<long double> <
              2.01L);
static_assert(tested::numbers::phi > 1.61 && tested::numbers::phi < 1.62);

bool ftl_test() { return true; }
