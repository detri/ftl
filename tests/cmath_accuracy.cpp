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
  const T toward = tested::numeric_limits<T>::infinity();
  T ulp = tested::nextafter(expected, toward) - expected;
  if (ulp < T{}) ulp = -ulp;
  T error = actual - expected;
  if (error < T{}) error = -error;
  return error <= T(limit) * ulp;
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

bool ftl_test() {
  return elementary_vectors<float>() && elementary_vectors<double>() &&
         elementary_vectors<long double>() && boost_vectors<float>() &&
         boost_vectors<double>() && boost_vectors<long double>();
}
