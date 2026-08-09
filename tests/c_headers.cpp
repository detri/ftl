#include <assert.h>
#include <errno.h>
#include <float.h>
#include <fenv.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>
#include <wctype.h>

static_assert(sizeof(va_list) >= 1);
static_assert(sizeof(jmp_buf) >= 1);
static_assert(sizeof(sig_atomic_t) >= 1);
static_assert(sizeof(fenv_t) >= sizeof(fexcept_t));
static_assert(FP_NORMAL != FP_ZERO && MATH_ERREXCEPT != 0);
static_assert(sizeof(div_t) >= sizeof(int) * 2);
static_assert(sizeof(imaxdiv_t) >= sizeof(intmax_t) * 2);
static_assert(CHAR_BIT >= 8 && FLT_RADIX >= 2);
static_assert(EDOM > 0 && PRIdMAX[0] != '\0');
static_assert(sizeof(wctrans_t) >= 1);
static_assert(sizeof(wctype_t) >= 1);

static_assert(sizeof(size_t) >= 1);
static_assert(sizeof(mbstate_t) >= 1);
static_assert(sizeof(wint_t) >= 1);
static_assert(sizeof(wctrans_t) >= 1);
static_assert(sizeof(wctype_t) >= 1);

[[maybe_unused]]
tm* tm_pointer = nullptr;

static_assert(WCHAR_MIN <= L'A');
static_assert(WCHAR_MAX >= L'A');

bool ftl_test() {
    assert(true);
    return true;
}
