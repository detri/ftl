#ifdef FTL_REPLACE_STL
#include <cfenv>
#include <cmath>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/cfenv>
#include <ftl/cmath>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(tested::is_same_v<decltype(tested::sqrt(4.0f)), float>);
static_assert(tested::is_same_v<decltype(tested::sqrt(4)), double>);
static_assert(tested::is_same_v<decltype(tested::pow(2.0L, 3)), long double>);
static_assert(tested::is_same_v<decltype(tested::fma(1.0f, 2.0, 3)), double>);
static_assert(FP_INFINITE != FP_NAN && FP_ZERO != FP_NORMAL);
static_assert(MATH_ERREXCEPT != 0);

bool ftl_test() {
  tested::fenv_t environment{};
  if (tested::fegetenv(&environment) != 0 ||
      tested::feclearexcept(FE_ALL_EXCEPT) != 0)
    return false;
  volatile double rounding_input = 1.25;
  const double nearby = tested::nearbyint(rounding_input);
  const bool nearby_is_quiet =
      nearby == 1.0 && tested::fetestexcept(FE_INEXACT) == 0;
  const double rounded = tested::rint(rounding_input);
  const bool rint_is_inexact =
      rounded == 1.0 && (tested::fetestexcept(FE_INEXACT) & FE_INEXACT) != 0;
  tested::fesetenv(&environment);

  int exponent = 0;
  int quotient = 0;
  double integer = 0;
  const double fraction = tested::modf(2.25, &integer);
  const double significand = tested::frexp(8.0, &exponent);
  const double remainder = tested::remquo(7.0, 2.0, &quotient);
  const auto quiet_nan = tested::nan("");

  return nearby_is_quiet && rint_is_inexact &&
         tested::sqrt(4.0) == 2.0 && tested::cbrt(8.0) == 2.0 &&
         tested::exp(0.0) == 1.0 && tested::log(1.0) == 0.0 &&
         tested::sin(0.0) == 0.0 && tested::cos(0.0) == 1.0 &&
         tested::pow(2.0, 3.0) == 8.0 && tested::hypot(3.0, 4.0) == 5.0 &&
         integer == 2.0 && fraction == 0.25 && significand == 0.5 &&
         exponent == 4 && remainder == -1.0 && quotient != 0 &&
         tested::isnan(quiet_nan) && tested::isinf(HUGE_VAL) &&
         tested::fpclassify(0.0) == FP_ZERO && tested::isfinite(1) &&
         !tested::isnormal(0) && tested::signbit(-0.0) &&
         tested::nextafter(1.0, 2.0) > 1.0 &&
         tested::scalbn(1.0, 4) == 16.0 && tested::round(1.5) == 2.0;
}
