/* FTL adaptation support for OpenLibm v0.8.7. */
#include <float.h>
#include <ftl/fenv.h>
#include <openlibm_math.h>

extern "C" double ftl_olm_ldexp(double value, int exponent) {
  return ftl_olm_scalbn(value, exponent);
}
extern "C" float ftl_olm_ldexpf(float value, int exponent) {
  return ftl_olm_scalbnf(value, exponent);
}
extern "C" long double ftl_olm_ldexpl(long double value, int exponent) {
  return ftl_olm_scalbnl(value, exponent);
}
#if LDBL_MANT_DIG == 53

extern "C" long double ftl_olm_nearbyintl(long double value) {
  /*
   * OpenLibm's binary80 implementation saves/restores the environment around
   * rintl.  The binary64 implementation does the same around rint.
   */
  fenv_t environment;
  fegetenv(&environment);
  const long double result =
      static_cast<long double>(ftl_olm_rint(static_cast<double>(value)));
  fesetenv(&environment);
  return result;
}
extern "C" long double ftl_olm_nexttowardl(long double from,
                                             long double to) {
  return ftl_olm_nextafter(from, to);
}

#define FTL_LONG_UNARY(name)                                                  \
  extern "C" long double ftl_olm_##name##l(long double value) {              \
    return static_cast<long double>(                                          \
        ftl_olm_##name(static_cast<double>(value)));                           \
  }

#define FTL_LONG_BINARY(name)                                                 \
  extern "C" long double ftl_olm_##name##l(long double left,                 \
                                             long double right) {              \
    return static_cast<long double>(ftl_olm_##name(                            \
        static_cast<double>(left), static_cast<double>(right)));               \
  }

FTL_LONG_UNARY(acos)
FTL_LONG_UNARY(acosh)
FTL_LONG_UNARY(asin)
FTL_LONG_UNARY(asinh)
FTL_LONG_UNARY(atan)
FTL_LONG_UNARY(atanh)
FTL_LONG_UNARY(cbrt)
FTL_LONG_UNARY(ceil)
FTL_LONG_UNARY(cos)
FTL_LONG_UNARY(cosh)
FTL_LONG_UNARY(erf)
FTL_LONG_UNARY(erfc)
FTL_LONG_UNARY(exp)
FTL_LONG_UNARY(exp2)
FTL_LONG_UNARY(expm1)
FTL_LONG_UNARY(fabs)
FTL_LONG_UNARY(floor)
FTL_LONG_UNARY(lgamma)
FTL_LONG_UNARY(log)
FTL_LONG_UNARY(log10)
FTL_LONG_UNARY(log1p)
FTL_LONG_UNARY(log2)
FTL_LONG_UNARY(logb)
FTL_LONG_UNARY(rint)
FTL_LONG_UNARY(round)
FTL_LONG_UNARY(sin)
FTL_LONG_UNARY(sinh)
FTL_LONG_UNARY(sqrt)
FTL_LONG_UNARY(tan)
FTL_LONG_UNARY(tanh)
FTL_LONG_UNARY(tgamma)
FTL_LONG_UNARY(trunc)

FTL_LONG_BINARY(atan2)
FTL_LONG_BINARY(copysign)
FTL_LONG_BINARY(fdim)
FTL_LONG_BINARY(fmax)
FTL_LONG_BINARY(fmin)
FTL_LONG_BINARY(fmod)
FTL_LONG_BINARY(hypot)
FTL_LONG_BINARY(nextafter)
FTL_LONG_BINARY(pow)
FTL_LONG_BINARY(remainder)

#undef FTL_LONG_UNARY
#undef FTL_LONG_BINARY

extern "C" int ftl_olm___fpclassifyl(long double value) {
  return ftl_olm___fpclassifyd(static_cast<double>(value));
}
extern "C" int ftl_olm___isfinitel(long double value) {
  return ftl_olm___isfinite(static_cast<double>(value));
}
extern "C" int ftl_olm___isinfl(long double value) {
  return ftl_olm_isinf(static_cast<double>(value));
}
extern "C" int ftl_olm___isnanl(long double value) {
  return ftl_olm_isnan(static_cast<double>(value));
}
extern "C" int ftl_olm___isnormall(long double value) {
  return ftl_olm___isnormal(static_cast<double>(value));
}
extern "C" int ftl_olm___signbitl(long double value) {
  return ftl_olm___signbit(static_cast<double>(value));
}
extern "C" long double ftl_olm_fmal(long double x, long double y,
                                     long double z) {
  return ftl_olm_fma(static_cast<double>(x), static_cast<double>(y),
                     static_cast<double>(z));
}
extern "C" long double ftl_olm_frexpl(long double value, int *exponent) {
  return ftl_olm_frexp(static_cast<double>(value), exponent);
}
extern "C" int ftl_olm_ilogbl(long double value) {
  return ftl_olm_ilogb(static_cast<double>(value));
}
extern "C" long double ftl_olm_modfl(long double value,
                                      long double *integer) {
  double double_integer;
  const double fraction = ftl_olm_modf(static_cast<double>(value),
                                       &double_integer);
  *integer = static_cast<long double>(double_integer);
  return static_cast<long double>(fraction);
}
extern "C" long ftl_olm_lrintl(long double value) {
  return ftl_olm_lrint(static_cast<double>(value));
}
extern "C" long long ftl_olm_llrintl(long double value) {
  return ftl_olm_llrint(static_cast<double>(value));
}
extern "C" long ftl_olm_lroundl(long double value) {
  return ftl_olm_lround(static_cast<double>(value));
}
extern "C" long long ftl_olm_llroundl(long double value) {
  return ftl_olm_llround(static_cast<double>(value));
}
extern "C" long double ftl_olm_scalbnl(long double value, int exponent) {
  return ftl_olm_scalbn(static_cast<double>(value), exponent);
}
extern "C" long double ftl_olm_scalblnl(long double value, long exponent) {
  return ftl_olm_scalbln(static_cast<double>(value), exponent);
}
extern "C" long double ftl_olm_remquol(long double x, long double y,
                                        int *quotient) {
  return ftl_olm_remquo(static_cast<double>(x), static_cast<double>(y),
                       quotient);
}
extern "C" double ftl_olm_nexttoward(double from, long double to) {
  return ftl_olm_nextafter(from, static_cast<double>(to));
}
extern "C" float ftl_olm_nexttowardf(float from, long double to) {
  if (ftl_olm_isnan(static_cast<double>(to)) || ftl_olm___isnanf(from))
    return from + static_cast<float>(to);
  if (static_cast<long double>(from) == to)
    return static_cast<float>(to);
  return ftl_olm_nextafterf(
      from, to > static_cast<long double>(from) ? HUGE_VALF : -HUGE_VALF);
}
#endif
