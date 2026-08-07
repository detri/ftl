#include <assert.h>
#include <errno.h>
#include <float.h>
#include <inttypes.h>
#include <limits.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <wctype.h>

static_assert(sizeof(va_list) >= 1);
static_assert(sizeof(jmp_buf) >= 1);
static_assert(sizeof(sig_atomic_t) >= 1);
static_assert(sizeof(div_t) >= sizeof(int) * 2);
static_assert(sizeof(imaxdiv_t) >= sizeof(intmax_t) * 2);
static_assert(CHAR_BIT >= 8 && FLT_RADIX >= 2);
static_assert(EDOM > 0 && PRIdMAX[0] != '\0');
static_assert(sizeof(wctrans_t) >= 1);
static_assert(sizeof(wctype_t) >= 1);

bool ftl_test() {
    assert(true);
    return true;
}
