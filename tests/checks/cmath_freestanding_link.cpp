#include <cmath>
#include <complex>

extern "C" int ftl_entry() {
  volatile double x = 0.75;
  volatile double y = 1.25;
  volatile long double z = 0.5L;
  int exponent;
  int quotient;

  double result = std::sin(x) + std::cos(x) + std::exp(x) + std::log(y);
  result += std::acos(x) + std::asin(x) + std::atan(x) + std::acosh(y);
  result += std::asinh(x) + std::atanh(x) + std::cbrt(x);
  result += std::ceil(x) + std::floor(x) + std::trunc(x) + std::round(x);
  result += std::cosh(x) + std::sinh(x) + std::tanh(x) + std::tan(x);
  result += std::erf(x) + std::erfc(x) + std::exp2(x) + std::expm1(x);
  result += std::fabs(x) + std::lgamma(y) + std::tgamma(y);
  result += std::log10(y) + std::log1p(x) + std::log2(y) + std::logb(y);
  result += std::sqrt(y) + std::fma(x, y, x) + std::nearbyint(y);
  result += std::rint(y) + std::atan2(y, x) + std::copysign(x, y);
  result += std::fdim(y, x) + std::fmax(x, y) + std::fmin(x, y);
  result += std::fmod(y, x) + std::hypot(x, y) + std::nextafter(x, y);
  result += std::pow(x, y) + std::remainder(y, x);
  result += std::frexp(x, &exponent) + std::remquo(y, x, &quotient);
  result += std::ldexp(x, exponent) + std::scalbn(x, exponent);
  result += std::scalbln(x, static_cast<long>(exponent));
  result += static_cast<double>(std::ilogb(x) + std::lrint(x) +
                                std::llrint(x) + std::lround(x) +
                                std::llround(x));
  result += std::nexttoward(x, z);
  long double integral;
  result += static_cast<double>(std::modf(z, &integral));
  result += std::assoc_laguerre(2, 1, x) + std::assoc_legendre(2, 1, x);
  result += std::beta(x, y) + std::comp_ellint_1(x) +
            std::comp_ellint_2(x) + std::comp_ellint_3(x, x);
  result += std::cyl_bessel_i(x, y) + std::cyl_bessel_j(x, y) +
            std::cyl_bessel_k(x, y) + std::cyl_neumann(x, y);
  result += std::ellint_1(x, y) + std::ellint_2(x, y) +
            std::ellint_3(x, x, y) + std::expint(x);
  result += std::hermite(2, x) + std::laguerre(2, x) +
            std::legendre(2, x) + std::riemann_zeta(y);
  result += std::sph_bessel(2, y) + std::sph_legendre(2, 1, x) +
            std::sph_neumann(2, y);
  result += static_cast<double>(
      std::acos(z) + std::asin(z) + std::atan(z) + std::atan2(z, z) +
      std::acosh(z + 1) + std::asinh(z) + std::atanh(z) + std::cbrt(z) +
      std::cos(z) + std::sin(z) + std::tan(z) + std::cosh(z) +
      std::sinh(z) + std::tanh(z) + std::exp(z) + std::exp2(z) +
      std::expm1(z) + std::log(z) + std::log10(z) + std::log1p(z) +
      std::log2(z) + std::sqrt(z) + std::nearbyint(z) + std::fma(z, z, z));

  const std::complex<double> value{static_cast<double>(x),
                                   static_cast<double>(y)};
  const auto transformed = std::sqrt(value) + std::log(value) +
                           std::log10(value) + std::sin(value) +
                           std::cos(value) + std::tan(value) +
                           std::sinh(value) + std::cosh(value) +
                           std::tanh(value) + std::exp(value) +
                           std::pow(value, value) + std::asin(value) +
                           std::acos(value) + std::atan(value) +
                           std::asinh(value) + std::acosh(value) +
                           std::atanh(value) + std::proj(value) +
                           std::conj(value);
  const std::complex<float> float_value{static_cast<float>(x),
                                        static_cast<float>(y)};
  const auto float_transformed =
      std::acos(float_value) + std::asin(float_value) +
      std::atan(float_value) + std::acosh(float_value) +
      std::asinh(float_value) + std::atanh(float_value) +
      std::cos(float_value) + std::sin(float_value) + std::tan(float_value) +
      std::cosh(float_value) + std::sinh(float_value) +
      std::tanh(float_value) + std::exp(float_value) + std::log(float_value) +
      std::sqrt(float_value) + std::pow(float_value, float_value);
  const std::complex<long double> long_value{static_cast<long double>(z),
                                             static_cast<long double>(z + 1)};
  const auto long_transformed =
      std::acos(long_value) + std::asin(long_value) + std::atan(long_value) +
      std::acosh(long_value) + std::asinh(long_value) +
      std::atanh(long_value) + std::cos(long_value) + std::sin(long_value) +
      std::tan(long_value) + std::cosh(long_value) + std::sinh(long_value) +
      std::tanh(long_value) + std::exp(long_value) + std::log(long_value) +
      std::sqrt(long_value) + std::pow(long_value, long_value);
  return result == 0.0 || transformed.real() == 0.0 ||
         float_transformed.real() == 0.0f || long_transformed.real() == 0.0L;
}
