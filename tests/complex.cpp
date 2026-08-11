#ifdef FTL_REPLACE_STL
#include <complex>
#include <istream>
#include <ostream>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/complex>
#include <ftl/istream>
#include <ftl/ostream>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

#if __cpp_lib_complex_udls != 201309L || __cpp_lib_constexpr_complex != 201711L
#error <complex> feature-test macros are incomplete
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

class complex_input_buffer : public tested::streambuf {
public:
  complex_input_buffer(char *first, char *last) { setg(first, first, last); }
};

class complex_output_buffer : public tested::streambuf {
public:
  tested::string text;
protected:
  int_type overflow(int_type value) override {
    if (traits_type::eq_int_type(value, traits_type::eof()))
      return traits_type::not_eof(value);
    text.push_back(traits_type::to_char_type(value));
    return value;
  }
  tested::streamsize xsputn(const char *source,
                            tested::streamsize count) override {
    text.append(source, static_cast<tested::size_t>(count));
    return count;
  }
};

bool stream_round_trip_works() {
  complex_output_buffer output;
  tested::ostream destination(&output);
  destination << complex<double>{3.5, -2.25};
  if (output.text != "(3.5,-2.25)")
    return false;
  complex_input_buffer input(output.text.data(),
                             output.text.data() + output.text.size());
  tested::istream source(&input);
  complex<double> value;
  source >> value;
  if (value != complex<double>{3.5, -2.25})
    return false;
  char scalar_text[] = {'4', '.', '5'};
  complex_input_buffer scalar_buffer(scalar_text, scalar_text + 3);
  tested::istream scalar_stream(&scalar_buffer);
  scalar_stream >> value;
  return value == complex<double>{4.5, 0.0};
}

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
  const auto divided_by_zero = value / complex<double>{0.0, 0.0};
  const auto divided_by_negative_zero = value / complex<double>{-0.0, 0.0};
  const auto divided_by_infinity =
      value / complex<double>{HUGE_VAL, HUGE_VAL};
  const auto infinite_numerator =
      complex<double>{HUGE_VAL, 1.0} / complex<double>{2.0, 3.0};
  const auto nan = tested::numeric_limits<double>::quiet_NaN();
  const auto nan_quotient = value / complex<double>{nan, 1.0};
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
         tested::signbit(projected.imag()) &&
         tested::isinf(divided_by_zero.real()) &&
         tested::isinf(divided_by_zero.imag()) &&
         !tested::signbit(divided_by_zero.real()) &&
         !tested::signbit(divided_by_zero.imag()) &&
         tested::isinf(divided_by_negative_zero.real()) &&
         tested::isinf(divided_by_negative_zero.imag()) &&
         tested::signbit(divided_by_negative_zero.real()) &&
         tested::signbit(divided_by_negative_zero.imag()) &&
         divided_by_infinity.real() == 0.0 &&
         divided_by_infinity.imag() == 0.0 &&
         tested::isinf(infinite_numerator.real()) &&
         tested::isinf(infinite_numerator.imag()) &&
         !tested::signbit(infinite_numerator.real()) &&
         tested::signbit(infinite_numerator.imag()) &&
         tested::isnan(nan_quotient.real()) &&
         tested::isnan(nan_quotient.imag()) && stream_round_trip_works();
}
