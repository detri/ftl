#ifdef FTL_REPLACE_STL
#include <cstdint>
#include <type_traits>
namespace tested = std;
#else
#include <cstdint>
#include <ftl/cstdint>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

#ifndef FTL_REPLACE_STL
static_assert(ftl::is_same_v<ftl::int64_t, std::int64_t>);
#endif

static_assert(sizeof(tested::uint8_t) == 1);
static_assert(sizeof(tested::int16_t) == 2);
static_assert(sizeof(tested::uint32_t) == 4);
static_assert(sizeof(tested::uint64_t) == 8);
static_assert(sizeof(tested::int_least8_t) >= 1);
static_assert(sizeof(tested::int_least16_t) >= 2);
static_assert(sizeof(tested::int_least32_t) >= 4);
static_assert(sizeof(tested::int_least64_t) >= 8);
static_assert(sizeof(tested::int_fast8_t) >= 1);
static_assert(sizeof(tested::int_fast16_t) >= 2);
static_assert(sizeof(tested::int_fast32_t) >= 4);
static_assert(sizeof(tested::int_fast64_t) >= 8);
static_assert(sizeof(tested::uintptr_t) == sizeof(void*));
static_assert(sizeof(tested::intptr_t) == sizeof(void*));
static_assert(sizeof(tested::uintmax_t) >= sizeof(tested::uint64_t));
static_assert(tested::is_same_v<tested::make_unsigned_t<int>, unsigned int>);
static_assert(tested::is_constant_evaluated());

static_assert(INT8_MIN == -128 && INT8_MAX == 127 && UINT8_MAX == 255);
static_assert(INT16_MIN == -32768 && INT16_MAX == 32767 && UINT16_MAX == 65535);
static_assert(INT32_MIN == (-2147483647 - 1) && INT32_MAX == 2147483647);
static_assert(UINT32_MAX == 4294967295U);
static_assert(INT64_MIN == (-9223372036854775807LL - 1));
static_assert(INT64_MAX == 9223372036854775807LL);
static_assert(UINT64_MAX == 18446744073709551615ULL);
static_assert(INT_LEAST8_MIN == INT8_MIN && INT_LEAST64_MAX == INT64_MAX);
static_assert(UINT_LEAST32_MAX == UINT32_MAX);
static_assert(INT_FAST8_MIN < 0 && INT_FAST64_MAX >= INT64_MAX);
static_assert(UINT_FAST16_MAX >= UINT16_MAX);
static_assert(INTMAX_MIN == INT64_MIN && INTMAX_MAX == INT64_MAX);
static_assert(UINTMAX_MAX == UINT64_MAX);
static_assert(INTPTR_MIN == INT64_MIN && INTPTR_MAX == INT64_MAX);
static_assert(UINTPTR_MAX == UINT64_MAX);
static_assert(PTRDIFF_MIN < 0 && PTRDIFF_MAX > 0 && SIZE_MAX >= UINT32_MAX);
static_assert(SIG_ATOMIC_MIN <= 0 && SIG_ATOMIC_MAX > 0);
static_assert(WCHAR_MIN <= 0 && WCHAR_MAX > 0);
static_assert(WINT_MIN <= 0 && WINT_MAX > 0);
static_assert(INT8_C(1) == 1 && UINT8_C(1) == 1U);
static_assert(INT16_C(1) == 1 && UINT16_C(1) == 1U);
static_assert(INT32_C(1) == 1 && UINT32_C(1) == 1U);
static_assert(INT64_C(1) == tested::int64_t{1});
static_assert(UINT64_C(1) == tested::uint64_t{1});
static_assert(INTMAX_C(1) == tested::intmax_t{1});
static_assert(UINTMAX_C(1) == tested::uintmax_t{1});

bool ftl_test() { return true; }
