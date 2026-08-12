#ifdef FTL_REPLACE_STL

#include <assert.h>
#include <complex.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <fenv.h>
#include <inttypes.h>
#include <iso646.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <time.h>
#include <uchar.h>
#include <wchar.h>
#include <wctype.h>

#else

#include <ftl/assert.h>
#include <ftl/complex.h>
#include <ftl/ctype.h>
#include <ftl/errno.h>
#include <ftl/float.h>
#include <ftl/fenv.h>
#include <ftl/inttypes.h>
#include <ftl/iso646.h>
#include <ftl/limits.h>
#include <ftl/locale.h>
#include <ftl/math.h>
#include <ftl/setjmp.h>
#include <ftl/signal.h>
#include <ftl/stdalign.h>
#include <ftl/stdarg.h>
#include <ftl/stdbool.h>
#include <ftl/stddef.h>
#include <ftl/stdint.h>
#include <ftl/stdio.h>
#include <ftl/stdlib.h>
#include <ftl/string.h>
#include <ftl/tgmath.h>
#include <ftl/time.h>
#include <ftl/uchar.h>
#include <ftl/wchar.h>
#include <ftl/wctype.h>

#endif

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

static_assert(__alignas_is_defined == 1);
static_assert(__bool_true_false_are_defined == 1);

static_assert(sizeof(ptrdiff_t) >= 1);
static_assert(sizeof(max_align_t) >= 1);
static_assert(sizeof(nullptr_t) == sizeof(void *));

static_assert(sizeof(fpos_t) >= 1);

[[maybe_unused]]
tm *tm_pointer = nullptr;

static_assert(WCHAR_MIN <= L'A');
static_assert(WCHAR_MAX >= L'A');

static_assert(sizeof(lconv) >= sizeof(char *) * 10);

[[maybe_unused]]
lconv *lconv_pointer = nullptr;

[[maybe_unused]]
FILE* file_pointer = nullptr;

[[maybe_unused]]
fpos_t* fpos_pointer = nullptr;

bool ftl_test() {
  assert(true);

  return isdigit('7') != 0 && isalpha('A') != 0 && tolower('A') == 'a' &&
         toupper('a') == 'A';
}
