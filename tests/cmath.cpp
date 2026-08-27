#include <cfenv>
#include <cmath>
#include <type_traits>
namespace tested = std;

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

template <class T>
constexpr bool extended_cmath_signatures() {
#define FTL_CHECK_UNARY(name)                                                \
  static_assert(tested::is_same_v<decltype(tested::name(T{})), T>)
#define FTL_CHECK_BINARY(name)                                               \
  static_assert(tested::is_same_v<decltype(tested::name(T{}, T{})), T>)
  FTL_CHECK_UNARY(acos); FTL_CHECK_UNARY(acosh); FTL_CHECK_UNARY(asin);
  FTL_CHECK_UNARY(asinh); FTL_CHECK_UNARY(atan); FTL_CHECK_UNARY(atanh);
  FTL_CHECK_UNARY(cbrt); FTL_CHECK_UNARY(ceil); FTL_CHECK_UNARY(cos);
  FTL_CHECK_UNARY(cosh); FTL_CHECK_UNARY(erf); FTL_CHECK_UNARY(erfc);
  FTL_CHECK_UNARY(exp); FTL_CHECK_UNARY(exp2); FTL_CHECK_UNARY(expm1);
  FTL_CHECK_UNARY(fabs); FTL_CHECK_UNARY(floor); FTL_CHECK_UNARY(lgamma);
  FTL_CHECK_UNARY(log); FTL_CHECK_UNARY(log10); FTL_CHECK_UNARY(log1p);
  FTL_CHECK_UNARY(log2); FTL_CHECK_UNARY(logb); FTL_CHECK_UNARY(nearbyint);
  FTL_CHECK_UNARY(rint); FTL_CHECK_UNARY(round); FTL_CHECK_UNARY(sin);
  FTL_CHECK_UNARY(sinh); FTL_CHECK_UNARY(sqrt); FTL_CHECK_UNARY(tan);
  FTL_CHECK_UNARY(tanh); FTL_CHECK_UNARY(tgamma); FTL_CHECK_UNARY(trunc);
  FTL_CHECK_BINARY(atan2); FTL_CHECK_BINARY(copysign); FTL_CHECK_BINARY(fdim);
  FTL_CHECK_BINARY(fmax); FTL_CHECK_BINARY(fmin); FTL_CHECK_BINARY(fmod);
  FTL_CHECK_BINARY(hypot); FTL_CHECK_BINARY(nextafter); FTL_CHECK_BINARY(pow);
  FTL_CHECK_BINARY(remainder);
  static_assert(tested::is_same_v<decltype(tested::abs(T{})), T>);
  static_assert(tested::is_same_v<decltype(tested::frexp(T{}, (int*)nullptr)), T>);
  static_assert(tested::is_same_v<decltype(tested::ilogb(T{})), int>);
  static_assert(tested::is_same_v<decltype(tested::ldexp(T{}, 1)), T>);
  static_assert(tested::is_same_v<decltype(tested::modf(T{}, (T*)nullptr)), T>);
  static_assert(tested::is_same_v<decltype(tested::scalbn(T{}, 1)), T>);
  static_assert(tested::is_same_v<decltype(tested::scalbln(T{}, 1L)), T>);
  static_assert(tested::is_same_v<decltype(tested::lrint(T{})), long>);
  static_assert(tested::is_same_v<decltype(tested::llrint(T{})), long long>);
  static_assert(tested::is_same_v<decltype(tested::lround(T{})), long>);
  static_assert(tested::is_same_v<decltype(tested::llround(T{})), long long>);
  static_assert(tested::is_same_v<decltype(tested::remquo(T{}, T{}, (int*)nullptr)), T>);
  static_assert(tested::is_same_v<decltype(tested::fma(T{}, T{}, T{})), T>);
  static_assert(tested::is_same_v<decltype(tested::lerp(T{}, T{}, T{})), T>);
  static_assert(tested::is_same_v<decltype(tested::hypot(T{}, T{}, T{})), T>);
#undef FTL_CHECK_BINARY
#undef FTL_CHECK_UNARY
  return true;
}

#ifdef __STDCPP_FLOAT16_T__
static_assert(extended_cmath_signatures<decltype(0.0f16)>());
#endif
#ifdef __STDCPP_FLOAT32_T__
static_assert(extended_cmath_signatures<decltype(0.0f32)>());
#endif
#ifdef __STDCPP_FLOAT64_T__
static_assert(extended_cmath_signatures<decltype(0.0f64)>());
#endif
#ifdef __STDCPP_FLOAT128_T__
static_assert(extended_cmath_signatures<decltype(0.0f128)>());
#endif
#ifdef __STDCPP_BFLOAT16_T__
static_assert(extended_cmath_signatures<decltype(0.0bf16)>());
#endif

template <class T>
bool extended_cmath_runtime() {
  volatile T input = T{1} / T{2};
  T x = input;
  int exponent{}, quotient{};
  T integer{};
  const T unary = tested::acos(x) + tested::acosh(T{1} + x) +
      tested::asin(x) + tested::asinh(x) + tested::atan(x) +
      tested::atanh(x) + tested::cbrt(x) + tested::ceil(x) +
      tested::cos(x) + tested::cosh(x) + tested::erf(x) +
      tested::erfc(x) + tested::exp(x) + tested::exp2(x) +
      tested::expm1(x) + tested::fabs(x) + tested::floor(x) +
      tested::lgamma(T{1} + x) + tested::log(T{1} + x) +
      tested::log10(T{1} + x) + tested::log1p(x) + tested::log2(T{1} + x) +
      tested::logb(x) + tested::nearbyint(x) + tested::rint(x) +
      tested::round(x) + tested::sin(x) + tested::sinh(x) +
      tested::sqrt(x) + tested::tan(x) + tested::tanh(x) +
      tested::tgamma(T{1} + x) + tested::trunc(x);
  const T binary = tested::atan2(x, T{1}) + tested::copysign(x, T{-1}) +
      tested::fdim(T{1}, x) + tested::fmax(x, T{1}) + tested::fmin(x, T{1}) +
      tested::fmod(T{3}, T{2}) + tested::hypot(T{3}, T{4}) +
      tested::nextafter(x, T{1}) + tested::pow(T{2}, T{3}) +
      tested::remainder(T{3}, T{2});
  const T decomposed = tested::frexp(x, &exponent) + tested::modf(x, &integer) +
      tested::ldexp(x, 2) + tested::scalbn(x, 2) + tested::scalbln(x, 2L) +
      tested::remquo(T{3}, T{2}, &quotient) + tested::fma(x, T{2}, T{1});
  return tested::isfinite(unary + binary + decomposed) &&
         tested::fpclassify(x) == FP_NORMAL && !tested::isinf(x) &&
         !tested::isnan(x) && tested::isnormal(x) && !tested::signbit(x) &&
         tested::ilogb(x) == -1 && tested::lrint(x) == 0 &&
         tested::llrint(x) == 0 && tested::lround(x) == 1 &&
         tested::llround(x) == 1;
}

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
      !tested::isgreater(1.0, signaling) &&
      !tested::isgreaterequal(signaling, 1.0) &&
      !tested::isgreaterequal(1.0, signaling) &&
      !tested::isless(signaling, 1.0) &&
      !tested::isless(1.0, signaling) &&
      !tested::islessequal(signaling, 1.0) &&
      !tested::islessequal(1.0, signaling) &&
      !tested::islessgreater(signaling, 1.0) &&
      !tested::islessgreater(1.0, signaling) &&
      tested::isunordered(signaling, 1.0) &&
      tested::isunordered(1.0, signaling) &&
      (tested::fetestexcept(FE_INVALID) & FE_INVALID) == 0;

  bool extended_works = true;
#ifdef __STDCPP_FLOAT16_T__
  extended_works = extended_works && extended_cmath_runtime<decltype(0.0f16)>();
#endif
#ifdef __STDCPP_FLOAT32_T__
  extended_works = extended_works && extended_cmath_runtime<decltype(0.0f32)>();
#endif
#ifdef __STDCPP_FLOAT64_T__
  extended_works = extended_works && extended_cmath_runtime<decltype(0.0f64)>();
#endif
#ifdef __STDCPP_FLOAT128_T__
  extended_works = extended_works && extended_cmath_runtime<decltype(0.0f128)>();
#endif
#ifdef __STDCPP_BFLOAT16_T__
  extended_works = extended_works && extended_cmath_runtime<decltype(0.0bf16)>();
#endif

  return extended_works && nearby_is_quiet && rint_is_inexact &&
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
