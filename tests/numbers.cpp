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

template<class T>
constexpr bool mathematical_constant_type_works() {
  static_assert(tested::is_floating_point_v<T>);

  static_assert(
      tested::is_same_v<
          decltype(tested::numbers::pi_v<T>),
          const T>);

  return tested::numbers::pi_v<T> > static_cast<T>(3) &&
         tested::numbers::pi_v<T> < static_cast<T>(4);
}

static_assert(mathematical_constant_type_works<float>());
static_assert(mathematical_constant_type_works<double>());
static_assert(mathematical_constant_type_works<long double>());

#ifdef __STDCPP_FLOAT16_T__
static_assert(
    mathematical_constant_type_works<decltype(0.0f16)>());
#endif

#ifdef __STDCPP_FLOAT32_T__
static_assert(
    mathematical_constant_type_works<decltype(0.0f32)>());
#endif

#ifdef __STDCPP_FLOAT64_T__
static_assert(
    mathematical_constant_type_works<decltype(0.0f64)>());
#endif

#ifdef __STDCPP_FLOAT128_T__

using standard_float128 = decltype(0.0f128);

static_assert(
    mathematical_constant_type_works<standard_float128>());

static_assert(
    tested::numbers::pi_v<standard_float128> ==
    3.141592653589793238462643383279502884f128);

static_assert(
    tested::numbers::e_v<standard_float128> ==
    2.718281828459045235360287471352662498f128);

#endif

#ifdef __STDCPP_BFLOAT16_T__
static_assert(
    mathematical_constant_type_works<decltype(0.0bf16)>());
#endif

#if defined(__SIZEOF_FLOAT128__) && !defined(__STRICT_ANSI__)

static_assert(tested::is_floating_point_v<__float128>);

static_assert(
    mathematical_constant_type_works<__float128>());

static_assert(
    tested::numbers::pi_v<__float128> ==
    3.141592653589793238462643383279502884Q);

static_assert(
    tested::numbers::e_v<__float128> ==
    2.718281828459045235360287471352662498Q);

#endif

bool ftl_test() { return true; }
