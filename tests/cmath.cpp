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

#if __cpp_lib_constexpr_cmath != 202202L || __cpp_lib_hypot != 201603L ||    \
    __cpp_lib_interpolate != 201902L ||                                      \
    __cpp_lib_math_special_functions != 201603L
#error <cmath> feature-test macros are incomplete
#endif

static_assert(tested::is_same_v<decltype(tested::sqrt(4.0f)), float>);
static_assert(tested::is_same_v<decltype(tested::sqrt(4)), double>);
static_assert(tested::is_same_v<decltype(tested::pow(2.0L, 3)), long double>);
static_assert(tested::is_same_v<decltype(tested::fma(1.0f, 2.0, 3)), double>);
static_assert(tested::is_same_v<decltype(tested::comp_ellint_1(0.5f)), float>);
static_assert(tested::is_same_v<decltype(tested::ellint_3(0.5L, 0.25L, 0.5L)), long double>);
static_assert(tested::is_same_v<decltype(tested::cyl_bessel_j(0.5, 1.0)), double>);
static_assert(tested::is_same_v<decltype(tested::sph_neumann(2, 1.0f)), float>);
static_assert(tested::is_same_v<decltype(tested::expintf(1.0f)), float>);
static_assert(tested::is_same_v<decltype(tested::riemann_zetal(2.0L)), long double>);
static_assert(tested::is_same_v<decltype(tested::beta(1.0f, 2)), double>);
static_assert(tested::is_same_v<decltype(tested::ellint_3(0.5f, 0.25, 1.0L)), long double>);
static_assert(tested::is_same_v<decltype(tested::expint(1)), double>);
static_assert(tested::is_same_v<decltype(tested::sph_bessel(2, 1)), double>);
static_assert(tested::is_same_v<decltype(tested::abs(1L)), long>);
static_assert(tested::is_same_v<decltype(tested::frexp(4, static_cast<int *>(nullptr))), double>);
static_assert(tested::is_same_v<decltype(tested::hypot(1.0f, 2.0, 3)), double>);
static_assert(tested::is_same_v<decltype(tested::lerp(1.0f, 2.0L, 3)), long double>);
static_assert(FP_INFINITE != FP_NAN && FP_ZERO != FP_NORMAL);
static_assert(MATH_ERREXCEPT != 0);

#if FLT_EVAL_METHOD == 0
static_assert(tested::is_same_v<tested::float_t, float>);
static_assert(tested::is_same_v<tested::double_t, double>);
#elif FLT_EVAL_METHOD == 1
static_assert(tested::is_same_v<tested::float_t, double>);
static_assert(tested::is_same_v<tested::double_t, double>);
#elif FLT_EVAL_METHOD == 2
static_assert(tested::is_same_v<tested::float_t, long double>);
static_assert(tested::is_same_v<tested::double_t, long double>);
#endif

#if defined(__STDCPP_FLOAT128_T__)
using extended_float = decltype(0.0f128);
static_assert(tested::is_same_v<decltype(tested::sqrt(extended_float{})),
                                extended_float>);
static_assert(tested::is_same_v<decltype(tested::beta(extended_float{}, 2)),
                                extended_float>);
#endif

constexpr bool constexpr_cmath() {
  int exponent{}, quotient{};
  double integer{};
  return tested::abs(-2.0) == 2.0 && tested::fabs(-0.0) == 0.0 &&
         tested::ceil(1.25) == 2.0 && tested::floor(1.75) == 1.0 &&
         tested::trunc(-1.75) == -1.0 && tested::round(-1.5) == -2.0 &&
         tested::frexp(8.0, &exponent) == 0.5 && exponent == 4 &&
         tested::ilogb(8.0) == 3 && tested::logb(8.0) == 3.0 &&
         tested::ldexp(0.5, 4) == 8.0 && tested::scalbn(1.0, 3) == 8.0 &&
         tested::modf(2.25, &integer) == 0.25 && integer == 2.0 &&
         tested::fmod(7.0, 2.0) == 1.0 &&
         tested::remainder(7.0, 2.0) == -1.0 &&
         tested::remquo(7.0, 2.0, &quotient) == -1.0 && quotient == 4 &&
         tested::copysign(1.0, -0.0) == -1.0 &&
         tested::nextafter(1.0, 2.0) > 1.0 &&
         tested::fdim(3.0, 2.0) == 1.0 && tested::fmax(1.0, 2.0) == 2.0 &&
         tested::fmin(1.0, 2.0) == 1.0 && tested::fma(2.0, 3.0, 1.0) == 7.0 &&
         tested::fpclassify(0.0) == FP_ZERO && tested::isfinite(1.0) &&
         !tested::isinf(1.0) && !tested::isnan(1.0) && tested::isnormal(1.0) &&
         tested::signbit(-0.0) && tested::isless(1.0, 2.0) &&
         tested::isunordered(tested::numeric_limits<double>::quiet_NaN(), 1.0);
}
static_assert(constexpr_cmath());

constexpr bool constexpr_reduction_avoids_ratio_overflow() {
  constexpr double maximum = tested::numeric_limits<double>::max();
  int quotient = 0;
  return tested::fmod(maximum, 0.5) == 0.0 &&
         tested::remainder(maximum, 0.5) == 0.0 &&
         tested::remquo(maximum, 0.5, &quotient) == 0.0;
}
static_assert(constexpr_reduction_avoids_ratio_overflow());

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
  tested::feclearexcept(FE_ALL_EXCEPT);
  const double special_domain = tested::beta(-1.0, 1.0);
  const bool special_reports_domain = tested::isnan(special_domain) &&
      (tested::fetestexcept(FE_INVALID) & FE_INVALID) != 0;
  tested::feclearexcept(FE_ALL_EXCEPT);
  const bool special_propagates_nan =
      tested::isnan(tested::comp_ellint_1(quiet_nan)) &&
      (tested::fetestexcept(FE_INVALID) & FE_INVALID) == 0;
  const double maximum = tested::numeric_limits<double>::max();
  const bool beta_large_arguments_underflow =
      tested::beta(maximum, maximum) == 0.0;
  volatile double signaling = tested::numeric_limits<double>::signaling_NaN();
  tested::feclearexcept(FE_ALL_EXCEPT);
  const bool quiet_comparisons_do_not_raise =
      !tested::isgreater(signaling, 1.0) &&
      (tested::fetestexcept(FE_INVALID) & FE_INVALID) == 0;

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
         tested::scalbn(1.0, 4) == 16.0 && tested::round(1.5) == 2.0 &&
         special_reports_domain && special_propagates_nan &&
         beta_large_arguments_underflow &&
         quiet_comparisons_do_not_raise &&
         tested::isinf(tested::cyl_bessel_k(0.0, 0.0)) &&
         tested::isinf(tested::cyl_neumann(0.0, 0.0));
}
