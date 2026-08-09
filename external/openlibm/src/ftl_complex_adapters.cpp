/* FTL adaptation support for OpenLibm v0.8.7. */
#include <float.h>
#include <openlibm_complex.h>

#if LDBL_MANT_DIG == 53

static ftl_olm_complex<double>
ftl_olm_to_double(ftl_olm_complex<long double> value) {
  return {static_cast<double>(value.real), static_cast<double>(value.imag)};
}
static ftl_olm_complex<long double>
ftl_olm_to_long_double(ftl_olm_complex<double> value) {
  return {static_cast<long double>(value.real),
          static_cast<long double>(value.imag)};
}

#define FTL_LONG_COMPLEX_UNARY(name)                                         \
  ftl_olm_complex<long double> ftl_olm_##name##l(                            \
      ftl_olm_complex<long double> value) {                                  \
    return ftl_olm_to_long_double(ftl_olm_##name(ftl_olm_to_double(value)));  \
  }

FTL_LONG_COMPLEX_UNARY(cacos)
FTL_LONG_COMPLEX_UNARY(casin)
FTL_LONG_COMPLEX_UNARY(catan)
FTL_LONG_COMPLEX_UNARY(ccos)
FTL_LONG_COMPLEX_UNARY(csin)
FTL_LONG_COMPLEX_UNARY(ctan)
FTL_LONG_COMPLEX_UNARY(cacosh)
FTL_LONG_COMPLEX_UNARY(casinh)
FTL_LONG_COMPLEX_UNARY(catanh)
FTL_LONG_COMPLEX_UNARY(ccosh)
FTL_LONG_COMPLEX_UNARY(csinh)
FTL_LONG_COMPLEX_UNARY(ctanh)
FTL_LONG_COMPLEX_UNARY(cexp)
FTL_LONG_COMPLEX_UNARY(clog)
FTL_LONG_COMPLEX_UNARY(csqrt)
FTL_LONG_COMPLEX_UNARY(conj)
FTL_LONG_COMPLEX_UNARY(cproj)

#undef FTL_LONG_COMPLEX_UNARY

ftl_olm_complex<long double> ftl_olm_cpowl(
    ftl_olm_complex<long double> left,
    ftl_olm_complex<long double> right) {
  return ftl_olm_to_long_double(
      ftl_olm_cpow(ftl_olm_to_double(left), ftl_olm_to_double(right)));
}
long double ftl_olm_cabsl(
    ftl_olm_complex<long double> value) {
  return static_cast<long double>(ftl_olm_cabs(ftl_olm_to_double(value)));
}
long double ftl_olm_cargl(
    ftl_olm_complex<long double> value) {
  return static_cast<long double>(ftl_olm_carg(ftl_olm_to_double(value)));
}
long double ftl_olm_cimagl(
    ftl_olm_complex<long double> value) {
  return value.imag;
}
long double ftl_olm_creall(
    ftl_olm_complex<long double> value) {
  return value.real;
}

#endif
