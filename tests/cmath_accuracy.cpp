// Reference values adapted from Boost.Math 1.91.0 test data (BSL-1.0).
#ifdef FTL_REPLACE_STL
#include <cmath>
#include <limits>
namespace tested = std;
#else
#include <ftl/cmath>
#include <ftl/limits>
namespace tested = ftl;
#endif

template <class T>
bool within_ulps(T actual, T expected, unsigned limit) {
  if (actual == expected) return true;
  if (!tested::isfinite(actual) || !tested::isfinite(expected)) return false;
  for (unsigned distance = 0; distance < limit; ++distance) {
    expected = tested::nextafter(expected, actual);
    if (expected == actual) return true;
  }
  return false;
}

template <class T>
bool elementary_vectors() {
  const T x = T(0.3L);
  return within_ulps(tested::sin(x),
                     T(0.29552020666133957510532074568502737368L), 4) &&
         within_ulps(tested::cos(x),
                     T(0.95533648912560601964231022756804989824L), 4) &&
         within_ulps(tested::tan(x),
                     T(0.30933624960962323303530367969829466726L), 5) &&
         within_ulps(tested::exp(x),
                     T(1.34985880757600310398374431332800733038L), 4) &&
         within_ulps(tested::sqrt(T(1.3L)),
                     T(1.14017542509913797913604902556675447908L), 4) &&
         within_ulps(tested::pow(T(1.3L), T(0.7L)),
                     T(1.20160118129295246963827877910250388273L), 8);
}

template <class T>
bool boost_vectors() {
  const T log_input = T(-0.2047410048544406890869140625e-1L);
  const T acosh_input = T(1.2495574951171875L);
  return within_ulps(
             tested::log1p(log_input),
             T(-0.2068660038044094868521052319477265955827e-1L), 5) &&
         within_ulps(
             tested::expm1(log_input),
             T(-0.2026592921724753704129022027337835687888e-1L), 5) &&
         within_ulps(
             tested::acosh(acosh_input),
             T(0.6925568837084910405419269283192900693752L), 6) &&
         within_ulps(
             tested::erf(T(0.47747135162353515625L)),
             T(0.5004808072450297309682843640858704431896L), 5) &&
         within_ulps(
             tested::erfc(T(0.47747135162353515625L)),
             T(0.4995191927549702690317156359141295568104L), 5);
}

template <class T>
bool special_vectors() {
  const T pi = T(3.141592653589793238462643383279502884L);
  const auto close = [](T actual, T expected) {
    // Binary80 donors carry binary64 reference uncertainty. Binary32/64 use
    // a much tighter adaptation-regression bound.
    constexpr unsigned limit = tested::numeric_limits<T>::digits > 53 ? 8192 : 64;
    return within_ulps(actual, expected, limit);
  };
  return close(tested::comp_ellint_1(T{}), pi / T{2}) &&
         close(tested::assoc_legendre(1, 1, T(0.5L)),
               T(0.866025403784438646763723170752936183L)) &&
         close(tested::hermite(20, T(0.5L)), T(-759627879679.0L)) &&
         close(tested::laguerre(20, T{1}),
               T(-0.164258811827792959953337607485686470L)) &&
         close(tested::assoc_laguerre(20, 10, T(0.5L)),
               T(11389309.2731485325302939533920319270L)) &&
         close(tested::legendre(20, T{}), T(0.176197052001953125L)) &&
         close(tested::beta(T{10}, T{12}),
               T(2.83514215402760294401161274226289706e-7L)) &&
         tested::sph_legendre(1, 1, T(0.5L)) < T{} &&
         close(tested::comp_ellint_2(T{}), pi / T{2}) &&
         close(tested::comp_ellint_3(T{}, T{}), pi / T{2}) &&
         close(tested::ellint_1(T{}, T(0.7L)), T(0.7L)) &&
         close(tested::ellint_2(T{}, T(0.7L)), T(0.7L)) &&
         close(tested::ellint_3(T{}, T{}, T(0.7L)), T(0.7L)) &&
         close(tested::cyl_bessel_i(T{}, T{1}),
               T(1.266065877752008335598244625214717537L)) &&
         close(tested::cyl_bessel_j(T{}, T{1}),
               T(0.765197686557966551449717526102663221L)) &&
         close(tested::cyl_bessel_k(T{}, T{1}),
               T(0.421024438240708333335627379212609037L)) &&
         close(tested::cyl_neumann(T{}, T{1}),
               T(0.0882569642156769579829267660235151628L)) &&
         close(tested::cyl_bessel_i(T(5.5L), T{10}),
               T(597.577653628482380554L)) &&
         close(tested::cyl_bessel_j(T(20.25L), T{20}),
               T(0.150604808827436348340L)) &&
         close(tested::cyl_bessel_k(T(5.5L), T{10}),
               T(0.0000733045300798502140211L)) &&
         close(tested::cyl_neumann(T(20.25L), T{20}),
               T(-0.309187670622299848322L)) &&
         close(tested::cyl_bessel_j(T(-0.5L), T{1}),
               tested::cos(T(1)) * tested::sqrt(T{2} / pi)) &&
         close(tested::cyl_neumann(T(-0.5L), T{1}),
               tested::sin(T(1)) * tested::sqrt(T{2} / pi)) &&
         close(tested::ellint_1(T(0.9L), T(0.7L)),
               T(0.750265780142851390622L)) &&
         close(tested::ellint_2(T(0.9L), T(0.7L)),
               T(0.655523111540768588235L)) &&
         close(tested::ellint_3(T(0.9L), T(0.5L), T(0.7L)),
               T(0.817830442339514740446L)) &&
         close(tested::expint(T{1}),
               T(1.89511781635593675546652093433163427L)) &&
         close(tested::expint(T{-20}),
               T(-9.83552529064988153984e-11L)) &&
         close(tested::riemann_zeta(T{2}), pi * pi / T{6}) &&
         close(tested::riemann_zeta(T(-3.5L)),
               T(0.00444101133547943408009L)) &&
         close(tested::sph_bessel(0, T{1}), tested::sin(T{1})) &&
         close(tested::sph_neumann(0, T{1}), -tested::cos(T{1}));
}

bool ftl_test() {
  return elementary_vectors<float>() && elementary_vectors<double>() &&
         elementary_vectors<long double>() && boost_vectors<float>() &&
         boost_vectors<double>() && boost_vectors<long double>() &&
         special_vectors<float>() && special_vectors<double>() &&
         special_vectors<long double>();
}
