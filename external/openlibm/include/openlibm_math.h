/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/*
 * from: @(#)fdlibm.h 5.1 93/09/24
 * $FreeBSD: src/lib/msun/src/openlibm.h,v 1.82 2011/11/12 19:55:48 theraven Exp $
 */

#ifdef OPENLIBM_USE_HOST_MATH_H
#include <math.h>
#else /* !OPENLIBM_USE_HOST_MATH_H */

#include <openlibm_defs.h>
#include <float.h>

#ifndef OPENLIBM_MATH_H
#define	OPENLIBM_MATH_H

#if (defined(_WIN32) || defined (_MSC_VER)) && !defined(__WIN32__)
    #define __WIN32__
#endif

#if !defined(__arm__) && !defined(__wasm__) && LDBL_MANT_DIG != 53
#define OLM_LONG_DOUBLE
#endif

#ifndef __pure2
#define __pure2
#endif

/*
 * ANSI/POSIX
 */
extern const union ftl_olm_infinity_storage {
	unsigned char	__uc[8];
	double		__ud;
} ftl_olm_infinity;

extern const union ftl_olm_nan_storage {
	unsigned char	__uc[sizeof(float)];
	float		__uf;
} ftl_olm_nan_value;

/* VBS
#if __GNUC_PREREQ__(3, 3) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 800)
#define	__MATH_BUILTIN_CONSTANTS
#endif

#if __GNUC_PREREQ__(3, 0) && !defined(__INTEL_COMPILER)
#define	__MATH_BUILTIN_RELOPS
#endif
*/

//VBS begin
#if defined(__GNUC__) || defined(__clang__)
#define __MATH_BUILTIN_CONSTANTS
#define	__MATH_BUILTIN_RELOPS
#endif
#ifndef __ISO_C_VISIBLE
#define __ISO_C_VISIBLE 1999
#endif
//VBS end

#ifdef __MATH_BUILTIN_CONSTANTS
#define	HUGE_VAL	__builtin_huge_val()
#else
#define	HUGE_VAL	(ftl_olm_infinity.__ud)
#endif

#if __ISO_C_VISIBLE >= 1999
#define	FP_ILOGB0	(-INT_MAX)
#define	FP_ILOGBNAN	INT_MAX

#ifdef __MATH_BUILTIN_CONSTANTS
#define	HUGE_VALF	__builtin_huge_valf()
#define	HUGE_VALL	__builtin_huge_vall()
#ifndef INFINITY
#define	INFINITY	__builtin_inff()
#endif
#ifndef NAN
#define	NAN		__builtin_nanf("")
#endif
#else
#define	HUGE_VALF	(float)HUGE_VAL
#define	HUGE_VALL	(long double)HUGE_VAL
#define	INFINITY	HUGE_VALF
#define	NAN		(ftl_olm_nan_value.__uf)
#endif /* __MATH_BUILTIN_CONSTANTS */

#define	MATH_ERRNO	1
#define	MATH_ERREXCEPT	2
#define	math_errhandling	MATH_ERREXCEPT

#define	FP_FAST_FMAF	1
#ifdef __ia64__
#define	FP_FAST_FMA	1
#define	FP_FAST_FMAL	1
#endif

/* Symbolic constants to classify floating point numbers. */
#define	FP_INFINITE	0x01
#define	FP_NAN		0x02
#define	FP_NORMAL	0x04
#define	FP_SUBNORMAL	0x08
#define	FP_ZERO		0x10
#define	fpclassify(x) \
    ((sizeof (x) == sizeof (float)) ? ftl_olm___fpclassifyf(x) \
    : (sizeof (x) == sizeof (double)) ? ftl_olm___fpclassifyd(x) \
    : ftl_olm___fpclassifyl(x))

#define	isfinite(x)					\
    ((sizeof (x) == sizeof (float)) ? ftl_olm___isfinitef(x)	\
    : (sizeof (x) == sizeof (double)) ? ftl_olm___isfinite(x)	\
    : ftl_olm___isfinitel(x))
#define	ftl_olm_isinf(x)					\
    ((sizeof (x) == sizeof (float)) ? ftl_olm___isinff(x)	\
    : (sizeof (x) == sizeof (double)) ? ftl_olm_isinf(x)	\
    : ftl_olm___isinfl(x))
#define	ftl_olm_isnan(x)					\
    ((sizeof (x) == sizeof (float)) ? ftl_olm___isnanf(x)	\
    : (sizeof (x) == sizeof (double)) ? ftl_olm_isnan(x)	\
    : ftl_olm___isnanl(x))
#define	isnormal(x)					\
    ((sizeof (x) == sizeof (float)) ? ftl_olm___isnormalf(x)	\
    : (sizeof (x) == sizeof (double)) ? ftl_olm___isnormal(x)	\
    : ftl_olm___isnormall(x))

#ifdef __MATH_BUILTIN_RELOPS
#define	isgreater(x, y)		__builtin_isgreater((x), (y))
#define	isgreaterequal(x, y)	__builtin_isgreaterequal((x), (y))
#define	isless(x, y)		__builtin_isless((x), (y))
#define	islessequal(x, y)	__builtin_islessequal((x), (y))
#define	islessgreater(x, y)	__builtin_islessgreater((x), (y))
#define	isunordered(x, y)	__builtin_isunordered((x), (y))
#else
#define	isgreater(x, y)		(!isunordered((x), (y)) && (x) > (y))
#define	isgreaterequal(x, y)	(!isunordered((x), (y)) && (x) >= (y))
#define	isless(x, y)		(!isunordered((x), (y)) && (x) < (y))
#define	islessequal(x, y)	(!isunordered((x), (y)) && (x) <= (y))
#define	islessgreater(x, y)	(!isunordered((x), (y)) && \
					((x) > (y) || (y) > (x)))
#define	isunordered(x, y)	(ftl_olm_isnan(x) || ftl_olm_isnan(y))
#endif /* __MATH_BUILTIN_RELOPS */

#define	signbit(x)					\
    ((sizeof (x) == sizeof (float)) ? ftl_olm___signbitf(x)	\
    : (sizeof (x) == sizeof (double)) ? ftl_olm___signbit(x)	\
    : ftl_olm___signbitl(x))

//VBS
//typedef	__double_t	double_t;
//typedef	__float_t	float_t;
#endif /* __ISO_C_VISIBLE >= 1999 */

/*
 * XOPEN/SVID
 */
#if __BSD_VISIBLE || __XSI_VISIBLE
#define	M_E		2.7182818284590452354	/* e */
#define	M_LOG2E		1.4426950408889634074	/* ftl_olm_log 2e */
#define	M_LOG10E	0.43429448190325182765	/* ftl_olm_log 10e */
#define	M_LN2		0.69314718055994530942	/* ftl_olm_log e2 */
#define	M_LN10		2.30258509299404568402	/* ftl_olm_log e10 */
#define	M_PI		3.14159265358979323846	/* pi */
#define	M_PI_2		1.57079632679489661923	/* pi/2 */
#define	M_PI_4		0.78539816339744830962	/* pi/4 */
#define	M_1_PI		0.31830988618379067154	/* 1/pi */
#define	M_2_PI		0.63661977236758134308	/* 2/pi */
#define	M_2_SQRTPI	1.12837916709551257390	/* 2/ftl_olm_sqrt(pi) */
#define	M_SQRT2		1.41421356237309504880	/* ftl_olm_sqrt(2) */
#define	M_SQRT1_2	0.70710678118654752440	/* 1/ftl_olm_sqrt(2) */

#define	MAXFLOAT	((float)3.40282346638528860e+38)

#ifndef OPENLIBM_ONLY_THREAD_SAFE
OLM_DLLEXPORT extern int signgam;
#endif
#endif /* __BSD_VISIBLE || __XSI_VISIBLE */

#if __BSD_VISIBLE
#if 0
/* Old value from 4.4BSD-Lite openlibm.h; this is probably better. */
#define	HUGE		HUGE_VAL
#else
#define	HUGE		MAXFLOAT
#endif
#endif /* __BSD_VISIBLE */

/*
 * Most of these functions depend on the rounding mode and have the side
 * effect of raising floating-point exceptions, so they are not declared
 * as __pure2.  In C99, FENV_ACCESS affects the purity of these functions.
 */

#if defined(__cplusplus)
extern "C" {
#endif
/* Symbol present when OpenLibm is used. */
int ftl_olm_isopenlibm(void);

/*
 * ANSI/POSIX
 */
OLM_DLLEXPORT int	ftl_olm___fpclassifyd(double) __pure2;
OLM_DLLEXPORT int	ftl_olm___fpclassifyf(float) __pure2;
OLM_DLLEXPORT int	ftl_olm___fpclassifyl(long double) __pure2;
OLM_DLLEXPORT int	ftl_olm___isfinitef(float) __pure2;
OLM_DLLEXPORT int	ftl_olm___isfinite(double) __pure2;
OLM_DLLEXPORT int	ftl_olm___isfinitel(long double) __pure2;
OLM_DLLEXPORT int	ftl_olm___isinff(float) __pure2;
OLM_DLLEXPORT int	ftl_olm___isinfl(long double) __pure2;
OLM_DLLEXPORT int	ftl_olm___isnanf(float) __pure2;
OLM_DLLEXPORT int	ftl_olm___isnanl(long double) __pure2;
OLM_DLLEXPORT int	ftl_olm___isnormalf(float) __pure2;
OLM_DLLEXPORT int	ftl_olm___isnormal(double) __pure2;
OLM_DLLEXPORT int	ftl_olm___isnormall(long double) __pure2;
OLM_DLLEXPORT int	ftl_olm___signbit(double) __pure2;
OLM_DLLEXPORT int	ftl_olm___signbitf(float) __pure2;
OLM_DLLEXPORT int	ftl_olm___signbitl(long double) __pure2;

OLM_DLLEXPORT double	ftl_olm_acos(double);
OLM_DLLEXPORT double	ftl_olm_asin(double);
OLM_DLLEXPORT double	ftl_olm_atan(double);
OLM_DLLEXPORT double	ftl_olm_atan2(double, double);
OLM_DLLEXPORT double	ftl_olm_cos(double);
OLM_DLLEXPORT double	ftl_olm_sin(double);
OLM_DLLEXPORT double	ftl_olm_tan(double);

OLM_DLLEXPORT double	ftl_olm_cosh(double);
OLM_DLLEXPORT double	ftl_olm_sinh(double);
OLM_DLLEXPORT double	ftl_olm_tanh(double);

OLM_DLLEXPORT double	ftl_olm_exp(double);
OLM_DLLEXPORT double	ftl_olm_frexp(double, int *);	/* fundamentally !__pure2 */
OLM_DLLEXPORT double	ftl_olm_ldexp(double, int);
OLM_DLLEXPORT double	ftl_olm_log(double);
OLM_DLLEXPORT double	ftl_olm_log10(double);
OLM_DLLEXPORT double	ftl_olm_modf(double, double *);	/* fundamentally !__pure2 */

OLM_DLLEXPORT double	ftl_olm_pow(double, double);
OLM_DLLEXPORT double	ftl_olm_sqrt(double);

OLM_DLLEXPORT double	ftl_olm_ceil(double);
OLM_DLLEXPORT double	ftl_olm_fabs(double) __pure2;
OLM_DLLEXPORT double	ftl_olm_floor(double);
OLM_DLLEXPORT double	ftl_olm_fmod(double, double);

/*
 * These functions are not in C90.
 */
#if __BSD_VISIBLE || __ISO_C_VISIBLE >= 1999 || __XSI_VISIBLE
OLM_DLLEXPORT double	ftl_olm_acosh(double);
OLM_DLLEXPORT double	ftl_olm_asinh(double);
OLM_DLLEXPORT double	ftl_olm_atanh(double);
OLM_DLLEXPORT double	ftl_olm_cbrt(double);
OLM_DLLEXPORT double	ftl_olm_erf(double);
OLM_DLLEXPORT double	ftl_olm_erfc(double);
OLM_DLLEXPORT double	ftl_olm_exp2(double);
OLM_DLLEXPORT double	ftl_olm_expm1(double);
OLM_DLLEXPORT double	ftl_olm_fma(double, double, double);
OLM_DLLEXPORT double	ftl_olm_hypot(double, double);
OLM_DLLEXPORT int	ftl_olm_ilogb(double) __pure2;
OLM_DLLEXPORT int	(ftl_olm_isinf)(double) __pure2;
OLM_DLLEXPORT int	(ftl_olm_isnan)(double) __pure2;
OLM_DLLEXPORT double	ftl_olm_lgamma(double);
OLM_DLLEXPORT long long ftl_olm_llrint(double);
OLM_DLLEXPORT long long ftl_olm_llround(double);
OLM_DLLEXPORT double	ftl_olm_log1p(double);
OLM_DLLEXPORT double	ftl_olm_log2(double);
OLM_DLLEXPORT double	ftl_olm_logb(double);
OLM_DLLEXPORT long	ftl_olm_lrint(double);
OLM_DLLEXPORT long	ftl_olm_lround(double);
OLM_DLLEXPORT double	ftl_olm_nan(const char *) __pure2;
OLM_DLLEXPORT double	ftl_olm_nextafter(double, double);
OLM_DLLEXPORT double	ftl_olm_remainder(double, double);
OLM_DLLEXPORT double	ftl_olm_remquo(double, double, int *);
OLM_DLLEXPORT double	ftl_olm_rint(double);
#endif /* __BSD_VISIBLE || __ISO_C_VISIBLE >= 1999 || __XSI_VISIBLE */

#if __BSD_VISIBLE || __XSI_VISIBLE
OLM_DLLEXPORT double	ftl_olm_j0(double);
OLM_DLLEXPORT double	ftl_olm_j1(double);
OLM_DLLEXPORT double	ftl_olm_jn(int, double);
OLM_DLLEXPORT double	ftl_olm_y0(double);
OLM_DLLEXPORT double	ftl_olm_y1(double);
OLM_DLLEXPORT double	ftl_olm_yn(int, double);
#endif /* __BSD_VISIBLE || __XSI_VISIBLE */

#if __BSD_VISIBLE || __ISO_C_VISIBLE >= 1999
OLM_DLLEXPORT double	ftl_olm_copysign(double, double) __pure2;
OLM_DLLEXPORT double	ftl_olm_fdim(double, double);
OLM_DLLEXPORT double	ftl_olm_fmax(double, double) __pure2;
OLM_DLLEXPORT double	ftl_olm_fmin(double, double) __pure2;
OLM_DLLEXPORT double	ftl_olm_nearbyint(double);
OLM_DLLEXPORT double	ftl_olm_round(double);
OLM_DLLEXPORT double	ftl_olm_scalbln(double, long);
OLM_DLLEXPORT double	ftl_olm_scalbn(double, int);
OLM_DLLEXPORT double	ftl_olm_tgamma(double);
OLM_DLLEXPORT double	ftl_olm_trunc(double);
#endif

/*
 * BSD math library entry points
 */
#if __BSD_VISIBLE
OLM_DLLEXPORT int	ftl_olm_isinff(float) __pure2;
OLM_DLLEXPORT int	ftl_olm_isnanf(float) __pure2;

/*
 * Reentrant version of ftl_olm_lgamma; passes signgam back by reference as the
 * second argument; user must allocate space for signgam.
 */
OLM_DLLEXPORT double	ftl_olm_lgamma_r(double, int *);

/*
 * Single sine/cosine function.
 */
OLM_DLLEXPORT void	ftl_olm_sincos(double, double *, double *);
#endif /* __BSD_VISIBLE */

/* float versions of ANSI/POSIX functions */
#if __ISO_C_VISIBLE >= 1999
OLM_DLLEXPORT float	ftl_olm_acosf(float);
OLM_DLLEXPORT float	ftl_olm_asinf(float);
OLM_DLLEXPORT float	ftl_olm_atanf(float);
OLM_DLLEXPORT float	ftl_olm_atan2f(float, float);
OLM_DLLEXPORT float	ftl_olm_cosf(float);
OLM_DLLEXPORT float	ftl_olm_sinf(float);
OLM_DLLEXPORT float	ftl_olm_tanf(float);

OLM_DLLEXPORT float	ftl_olm_coshf(float);
OLM_DLLEXPORT float	ftl_olm_sinhf(float);
OLM_DLLEXPORT float	ftl_olm_tanhf(float);

OLM_DLLEXPORT float	ftl_olm_exp2f(float);
OLM_DLLEXPORT float	ftl_olm_expf(float);
OLM_DLLEXPORT float	ftl_olm_expm1f(float);
OLM_DLLEXPORT float	ftl_olm_frexpf(float, int *);	/* fundamentally !__pure2 */
OLM_DLLEXPORT int	ftl_olm_ilogbf(float) __pure2;
OLM_DLLEXPORT float	ftl_olm_ldexpf(float, int);
OLM_DLLEXPORT float	ftl_olm_log10f(float);
OLM_DLLEXPORT float	ftl_olm_log1pf(float);
OLM_DLLEXPORT float	ftl_olm_log2f(float);
OLM_DLLEXPORT float	ftl_olm_logf(float);
OLM_DLLEXPORT float	ftl_olm_modff(float, float *);	/* fundamentally !__pure2 */

OLM_DLLEXPORT float	ftl_olm_powf(float, float);
OLM_DLLEXPORT float	ftl_olm_sqrtf(float);

OLM_DLLEXPORT float	ftl_olm_ceilf(float);
OLM_DLLEXPORT float	ftl_olm_fabsf(float) __pure2;
OLM_DLLEXPORT float	ftl_olm_floorf(float);
OLM_DLLEXPORT float	ftl_olm_fmodf(float, float);
OLM_DLLEXPORT float	ftl_olm_roundf(float);

OLM_DLLEXPORT float	ftl_olm_erff(float);
OLM_DLLEXPORT float	ftl_olm_erfcf(float);
OLM_DLLEXPORT float	ftl_olm_hypotf(float, float);
OLM_DLLEXPORT float	ftl_olm_lgammaf(float);
OLM_DLLEXPORT float	ftl_olm_tgammaf(float);

OLM_DLLEXPORT float	ftl_olm_acoshf(float);
OLM_DLLEXPORT float	ftl_olm_asinhf(float);
OLM_DLLEXPORT float	ftl_olm_atanhf(float);
OLM_DLLEXPORT float	ftl_olm_cbrtf(float);
OLM_DLLEXPORT float	ftl_olm_logbf(float);
OLM_DLLEXPORT float	ftl_olm_copysignf(float, float) __pure2;
OLM_DLLEXPORT long long ftl_olm_llrintf(float);
OLM_DLLEXPORT long long ftl_olm_llroundf(float);
OLM_DLLEXPORT long	ftl_olm_lrintf(float);
OLM_DLLEXPORT long	ftl_olm_lroundf(float);
OLM_DLLEXPORT float	ftl_olm_nanf(const char *) __pure2;
OLM_DLLEXPORT float	ftl_olm_nearbyintf(float);
OLM_DLLEXPORT float	ftl_olm_nextafterf(float, float);
OLM_DLLEXPORT float	ftl_olm_remainderf(float, float);
OLM_DLLEXPORT float	ftl_olm_remquof(float, float, int *);
OLM_DLLEXPORT float	ftl_olm_rintf(float);
OLM_DLLEXPORT float	ftl_olm_scalblnf(float, long);
OLM_DLLEXPORT float	ftl_olm_scalbnf(float, int);
OLM_DLLEXPORT float	ftl_olm_truncf(float);

OLM_DLLEXPORT float	ftl_olm_fdimf(float, float);
OLM_DLLEXPORT float	ftl_olm_fmaf(float, float, float);
OLM_DLLEXPORT float	ftl_olm_fmaxf(float, float) __pure2;
OLM_DLLEXPORT float	ftl_olm_fminf(float, float) __pure2;
#endif

/*
 * float versions of BSD math library entry points
 */
#if __BSD_VISIBLE
OLM_DLLEXPORT float	ftl_olm_dremf(float, float);
OLM_DLLEXPORT float	ftl_olm_j0f(float);
OLM_DLLEXPORT float	ftl_olm_j1f(float);
OLM_DLLEXPORT float	ftl_olm_jnf(int, float);
OLM_DLLEXPORT float	ftl_olm_y0f(float);
OLM_DLLEXPORT float	ftl_olm_y1f(float);
OLM_DLLEXPORT float	ftl_olm_ynf(int, float);

/*
 * Float versions of reentrant version of ftl_olm_lgamma; passes signgam back by
 * reference as the second argument; user must allocate space for signgam.
 */
OLM_DLLEXPORT float	ftl_olm_lgammaf_r(float, int *);

/*
 * Single sine/cosine function.
 */
OLM_DLLEXPORT void	ftl_olm_sincosf(float, float *, float *);
#endif	/* __BSD_VISIBLE */

/*
 * long double versions of ISO/POSIX math functions
 */
#if __ISO_C_VISIBLE >= 1999
OLM_DLLEXPORT long double	ftl_olm_acoshl(long double);
OLM_DLLEXPORT long double	ftl_olm_acosl(long double);
OLM_DLLEXPORT long double	ftl_olm_asinhl(long double);
OLM_DLLEXPORT long double	ftl_olm_asinl(long double);
OLM_DLLEXPORT long double	ftl_olm_atan2l(long double, long double);
OLM_DLLEXPORT long double	ftl_olm_atanhl(long double);
OLM_DLLEXPORT long double	ftl_olm_atanl(long double);
OLM_DLLEXPORT long double	ftl_olm_cbrtl(long double);
OLM_DLLEXPORT long double	ftl_olm_ceill(long double);
OLM_DLLEXPORT long double	ftl_olm_copysignl(long double, long double) __pure2;
OLM_DLLEXPORT long double	ftl_olm_coshl(long double);
OLM_DLLEXPORT long double	ftl_olm_cosl(long double);
OLM_DLLEXPORT long double	ftl_olm_erfcl(long double);
OLM_DLLEXPORT long double	ftl_olm_erfl(long double);
OLM_DLLEXPORT long double	ftl_olm_exp2l(long double);
OLM_DLLEXPORT long double	ftl_olm_expl(long double);
OLM_DLLEXPORT long double	ftl_olm_expm1l(long double);
OLM_DLLEXPORT long double	ftl_olm_fabsl(long double) __pure2;
OLM_DLLEXPORT long double	ftl_olm_fdiml(long double, long double);
OLM_DLLEXPORT long double	ftl_olm_floorl(long double);
OLM_DLLEXPORT long double	ftl_olm_fmal(long double, long double, long double);
OLM_DLLEXPORT long double	ftl_olm_fmaxl(long double, long double) __pure2;
OLM_DLLEXPORT long double	ftl_olm_fminl(long double, long double) __pure2;
OLM_DLLEXPORT long double	ftl_olm_fmodl(long double, long double);
OLM_DLLEXPORT long double	ftl_olm_frexpl(long double value, int *); /* fundamentally !__pure2 */
OLM_DLLEXPORT long double	ftl_olm_hypotl(long double, long double);
OLM_DLLEXPORT int		ftl_olm_ilogbl(long double) __pure2;
OLM_DLLEXPORT long double	ftl_olm_ldexpl(long double, int);
OLM_DLLEXPORT long double	ftl_olm_lgammal(long double);
OLM_DLLEXPORT long long	ftl_olm_llrintl(long double);
OLM_DLLEXPORT long long	ftl_olm_llroundl(long double);
OLM_DLLEXPORT long double	ftl_olm_log10l(long double);
OLM_DLLEXPORT long double	ftl_olm_log1pl(long double);
OLM_DLLEXPORT long double	ftl_olm_log2l(long double);
OLM_DLLEXPORT long double	ftl_olm_logbl(long double);
OLM_DLLEXPORT long double	ftl_olm_logl(long double);
OLM_DLLEXPORT long		ftl_olm_lrintl(long double);
OLM_DLLEXPORT long		ftl_olm_lroundl(long double);
OLM_DLLEXPORT long double	ftl_olm_modfl(long double, long double *); /* fundamentally !__pure2 */
OLM_DLLEXPORT long double	ftl_olm_nanl(const char *) __pure2;
OLM_DLLEXPORT long double	ftl_olm_nearbyintl(long double);
OLM_DLLEXPORT long double	ftl_olm_nextafterl(long double, long double);
OLM_DLLEXPORT double		ftl_olm_nexttoward(double, long double);
OLM_DLLEXPORT float		ftl_olm_nexttowardf(float, long double);
OLM_DLLEXPORT long double	ftl_olm_nexttowardl(long double, long double);
OLM_DLLEXPORT long double	ftl_olm_powl(long double, long double);
OLM_DLLEXPORT long double	ftl_olm_remainderl(long double, long double);
OLM_DLLEXPORT long double	ftl_olm_remquol(long double, long double, int *);
OLM_DLLEXPORT long double	ftl_olm_rintl(long double);
OLM_DLLEXPORT long double	ftl_olm_roundl(long double);
OLM_DLLEXPORT long double	ftl_olm_scalblnl(long double, long);
OLM_DLLEXPORT long double	ftl_olm_scalbnl(long double, int);
OLM_DLLEXPORT long double	ftl_olm_sinhl(long double);
OLM_DLLEXPORT long double	ftl_olm_sinl(long double);
OLM_DLLEXPORT long double	ftl_olm_sqrtl(long double);
OLM_DLLEXPORT long double	ftl_olm_tanhl(long double);
OLM_DLLEXPORT long double	ftl_olm_tanl(long double);
OLM_DLLEXPORT long double	ftl_olm_tgammal(long double);
OLM_DLLEXPORT long double	ftl_olm_truncl(long double);
#endif /* __ISO_C_VISIBLE >= 1999 */

/* Reentrant version of ftl_olm_lgammal. */
#if __BSD_VISIBLE
OLM_DLLEXPORT long double	ftl_olm_lgammal_r(long double, int *);

/*
 * Single sine/cosine function.
 */
OLM_DLLEXPORT void	ftl_olm_sincosl(long double, long double *, long double *);
#endif	/* __BSD_VISIBLE */

#if defined(__cplusplus)
}
#endif
#endif /* !OPENLIBM_MATH_H */

#endif /* OPENLIBM_USE_HOST_MATH_H */
