#ifdef FTL_REPLACE_STL
#include <complex>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/complex>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

using tested::complex;
using namespace tested::literals::complex_literals;

static_assert(complex<double>{1.0, 2.0}.real() == 1.0);
static_assert(complex<double>{1.0, 2.0}.imag() == 2.0);
static_assert(complex<double>{1.0, 2.0} + complex<double>{3.0, 4.0} ==
              complex<double>{4.0, 6.0});
static_assert(complex<double>{1.0, 2.0} * complex<double>{3.0, 4.0} ==
              complex<double>{-5.0, 10.0});
static_assert(2.0i == complex<double>{0.0, 2.0});
static_assert(tested::is_same_v<decltype(2.0if), complex<float>>);
static_assert(tested::is_same_v<decltype(2.0il), complex<long double>>);
static_assert(tested::is_trivially_copyable_v<complex<float>>);
static_assert(tested::is_trivially_copyable_v<complex<double>>);
static_assert(tested::is_trivially_copyable_v<complex<long double>>);
static_assert(tested::is_convertible_v<complex<float>, complex<double>>);
static_assert(tested::is_convertible_v<complex<double>, complex<long double>>);
static_assert(!tested::is_convertible_v<complex<double>, complex<float>>);
static_assert(!tested::is_convertible_v<complex<long double>, complex<double>>);
static_assert(tested::is_same_v<decltype(tested::real(1)), double>);
static_assert(tested::is_same_v<decltype(tested::imag(1.0f)), float>);
static_assert(tested::is_same_v<decltype(tested::norm(1)), double>);
static_assert(tested::is_same_v<decltype(tested::conj(1.0f)), complex<float>>);
static_assert(tested::is_same_v<decltype(tested::proj(1)), complex<double>>);
static_assert(tested::is_same_v<decltype(tested::pow(complex<float>{}, 2.0)),
                                complex<double>>);
static_assert(tested::is_same_v<decltype(tested::pow(complex<float>{},
                                                     complex<long double>{})),
                                complex<long double>>);

bool ftl_test() {
  const complex<double> value{3.0, 4.0};
  const auto square_root = tested::sqrt(complex<double>{-1.0, 0.0});
  const auto exponential = tested::exp(complex<double>{0.0, 0.0});
  const auto logarithm = tested::log(complex<double>{1.0, 0.0});
  const auto power = tested::pow(complex<double>{2.0, 0.0},
                                 complex<double>{3.0, 0.0});
  const auto lower_sqrt = tested::sqrt(complex<double>{-1.0, -0.0});
  const auto upper_log = tested::log(complex<double>{-1.0, 0.0});
  const auto lower_log = tested::log(complex<double>{-1.0, -0.0});
  const auto projected =
      tested::proj(complex<double>{HUGE_VAL, -2.0});
  const double *layout = reinterpret_cast<const double *>(&value);
  return tested::abs(value) == 5.0 && square_root.real() == 0.0 &&
         square_root.imag() == 1.0 && exponential == complex<double>{1.0} &&
         logarithm == complex<double>{0.0} && power == complex<double>{8.0} &&
         tested::conj(value) == complex<double>{3.0, -4.0} &&
         tested::norm(value) == 25.0 && layout[0] == 3.0 && layout[1] == 4.0 &&
         lower_sqrt.real() == 0.0 && lower_sqrt.imag() == -1.0 &&
         upper_log.imag() > 3.14 && upper_log.imag() < 3.15 &&
         lower_log.imag() < -3.14 && lower_log.imag() > -3.15 &&
         tested::isinf(projected.real()) && projected.imag() == 0.0 &&
         tested::signbit(projected.imag());
}
