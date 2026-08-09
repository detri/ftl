/*	$OpenBSD: complex.h,v 1.5 2014/03/16 18:38:30 guenther Exp $	*/
/*
 * Copyright (c) 2008 Martynas Venckus <martynas@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef OPENLIBM_USE_HOST_COMPLEX_H
#include <complex.h>
#else /* !OPENLIBM_USE_HOST_COMPLEX_H */

#ifndef OPENLIBM_COMPLEX_H
#define	OPENLIBM_COMPLEX_H

#include <ftl/detail/complex_abi>

template <class T>
constexpr ftl_olm_complex<T> operator+(ftl_olm_complex<T> value) {
	return value;
}
template <class T>
constexpr ftl_olm_complex<T> operator-(ftl_olm_complex<T> value) {
	return {-value.real, -value.imag};
}
template <class T>
constexpr ftl_olm_complex<T> operator+(ftl_olm_complex<T> left,
	                                    ftl_olm_complex<T> right) {
	return {left.real + right.real, left.imag + right.imag};
}
template <class T>
constexpr ftl_olm_complex<T> operator-(ftl_olm_complex<T> left,
	                                    ftl_olm_complex<T> right) {
	return {left.real - right.real, left.imag - right.imag};
}
template <class T>
constexpr ftl_olm_complex<T> operator*(ftl_olm_complex<T> left,
	                                    ftl_olm_complex<T> right) {
	return {left.real * right.real - left.imag * right.imag,
	        left.real * right.imag + left.imag * right.real};
}
template <class T>
inline ftl_olm_complex<T> operator/(ftl_olm_complex<T> left,
	                                ftl_olm_complex<T> right) {
	const T abs_real = right.real < T{} ? -right.real : right.real;
	const T abs_imag = right.imag < T{} ? -right.imag : right.imag;
	if (abs_real < abs_imag) {
		const T ratio = right.real / right.imag;
		const T denominator = right.real * ratio + right.imag;
		return {(left.real * ratio + left.imag) / denominator,
		        (left.imag * ratio - left.real) / denominator};
	}
	const T ratio = right.imag / right.real;
	const T denominator = right.imag * ratio + right.real;
	return {(left.imag * ratio + left.real) / denominator,
	        (left.imag - left.real * ratio) / denominator};
}
template <class T>
constexpr bool operator==(ftl_olm_complex<T> left,
                          ftl_olm_complex<T> right) {
	return left.real == right.real && left.imag == right.imag;
}
template <class T, class U>
constexpr bool operator==(ftl_olm_complex<T> left, U right) {
	return left == ftl_olm_complex<T>{static_cast<T>(right), T{}};
}
template <class T, class U>
constexpr bool operator==(U left, ftl_olm_complex<T> right) {
	return right == left;
}

#define FTL_OLM_COMPLEX_SCALAR_OPERATOR(symbol)                               \
	template <class T, class U>                                                 \
	constexpr ftl_olm_complex<T> operator symbol(ftl_olm_complex<T> left, U right) { \
		return left symbol ftl_olm_complex<T>(static_cast<T>(right));              \
	}                                                                           \
	template <class T, class U>                                                 \
	constexpr ftl_olm_complex<T> operator symbol(U left, ftl_olm_complex<T> right) { \
		return ftl_olm_complex<T>(static_cast<T>(left)) symbol right;              \
	}

FTL_OLM_COMPLEX_SCALAR_OPERATOR(+)
FTL_OLM_COMPLEX_SCALAR_OPERATOR(-)
FTL_OLM_COMPLEX_SCALAR_OPERATOR(*)
FTL_OLM_COMPLEX_SCALAR_OPERATOR(/)
#undef FTL_OLM_COMPLEX_SCALAR_OPERATOR

template <class T>
constexpr ftl_olm_complex<T> &operator+=(ftl_olm_complex<T> &left,
	                                      ftl_olm_complex<T> right) {
	return left = left + right;
}
template <class T>
constexpr ftl_olm_complex<T> &operator-=(ftl_olm_complex<T> &left,
	                                      ftl_olm_complex<T> right) {
	return left = left - right;
}
template <class T>
constexpr ftl_olm_complex<T> &operator*=(ftl_olm_complex<T> &left,
	                                      ftl_olm_complex<T> right) {
	return left = left * right;
}
template <class T>
inline ftl_olm_complex<T> &operator/=(ftl_olm_complex<T> &left,
	                                  ftl_olm_complex<T> right) {
	return left = left / right;
}
template <class T, class U>
constexpr ftl_olm_complex<T> &operator+=(ftl_olm_complex<T> &left, U right) {
	return left = left + right;
}
template <class T, class U>
constexpr ftl_olm_complex<T> &operator-=(ftl_olm_complex<T> &left, U right) {
	return left = left - right;
}
template <class T, class U>
constexpr ftl_olm_complex<T> &operator*=(ftl_olm_complex<T> &left, U right) {
	return left = left * right;
}
template <class T, class U>
inline ftl_olm_complex<T> &operator/=(ftl_olm_complex<T> &left, U right) {
	return left = left / right;
}

struct ftl_olm_imaginary_unit { int sign = 1; };
constexpr ftl_olm_imaginary_unit operator-(ftl_olm_imaginary_unit value) {
	return {-value.sign};
}
template <class T>
constexpr ftl_olm_complex<T> operator*(T value, ftl_olm_imaginary_unit unit) {
	return {T{}, unit.sign < 0 ? -value : value};
}
template <class T>
constexpr ftl_olm_complex<T> operator*(ftl_olm_imaginary_unit unit, T value) {
	return {T{}, unit.sign < 0 ? -value : value};
}
template <class T>
constexpr ftl_olm_complex<T> operator*(ftl_olm_complex<T> value,
	                                    ftl_olm_imaginary_unit unit) {
	return unit.sign < 0 ? ftl_olm_complex<T>{value.imag, -value.real}
	                     : ftl_olm_complex<T>{-value.imag, value.real};
}
template <class T>
constexpr ftl_olm_complex<T> operator*(ftl_olm_imaginary_unit unit,
	                                    ftl_olm_complex<T> value) {
	return value * unit;
}

#define I ftl_olm_imaginary_unit{}
#define CMPLXF(x, y) ftl_olm_complex<float>((x), (y))
#define CMPLX(x, y) ftl_olm_complex<double>((x), (y))
#define CMPLXL(x, y) ftl_olm_complex<long double>((x), (y))

/*
 * Double versions of C99 functions
 */
ftl_olm_complex<double> ftl_olm_cacos(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_casin(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_catan(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_ccos(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_csin(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_ctan(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_cacosh(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_casinh(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_catanh(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_ccosh(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_csinh(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_ctanh(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_cexp(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_clog(ftl_olm_complex<double>);
double ftl_olm_cabs(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_cpow(ftl_olm_complex<double>, ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_csqrt(ftl_olm_complex<double>);
double ftl_olm_carg(ftl_olm_complex<double>);
double ftl_olm_cimag(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_conj(ftl_olm_complex<double>);
ftl_olm_complex<double> ftl_olm_cproj(ftl_olm_complex<double>);
double ftl_olm_creal(ftl_olm_complex<double>);

/*
 * Float versions of C99 functions
 */
ftl_olm_complex<float> ftl_olm_cacosf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_casinf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_catanf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_ccosf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_csinf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_ctanf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_cacoshf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_casinhf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_catanhf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_ccoshf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_csinhf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_ctanhf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_cexpf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_clogf(ftl_olm_complex<float>);
float ftl_olm_cabsf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_cpowf(ftl_olm_complex<float>, ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_csqrtf(ftl_olm_complex<float>);
float ftl_olm_cargf(ftl_olm_complex<float>);
float ftl_olm_cimagf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_conjf(ftl_olm_complex<float>);
ftl_olm_complex<float> ftl_olm_cprojf(ftl_olm_complex<float>);
float ftl_olm_crealf(ftl_olm_complex<float>);

/*
 * Long double versions of C99 functions
 */
ftl_olm_complex<long double> ftl_olm_cacosl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_casinl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_catanl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_ccosl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_csinl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_ctanl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_cacoshl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_casinhl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_catanhl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_ccoshl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_csinhl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_ctanhl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_cexpl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_clogl(ftl_olm_complex<long double>);
long double ftl_olm_cabsl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_cpowl(ftl_olm_complex<long double>,
	ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_csqrtl(ftl_olm_complex<long double>);
long double ftl_olm_cargl(ftl_olm_complex<long double>);
long double ftl_olm_cimagl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_conjl(ftl_olm_complex<long double>);
ftl_olm_complex<long double> ftl_olm_cprojl(ftl_olm_complex<long double>);
long double ftl_olm_creall(ftl_olm_complex<long double>);

#endif /* !OPENLIBM_COMPLEX_H */

#endif /* OPENLIBM_USE_HOST_COMPLEX_H */
