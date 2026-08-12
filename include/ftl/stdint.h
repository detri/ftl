// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_STDINT_H_HEADER
#define FTL_STDINT_H_HEADER

#ifdef FTL_REPLACE_STL
#include <cstdint>
#define FTL_STDINT_NAMESPACE std
#else
#include <ftl/cstdint>
#define FTL_STDINT_NAMESPACE ftl
#endif

using FTL_STDINT_NAMESPACE::int8_t;
using FTL_STDINT_NAMESPACE::uint8_t;
using FTL_STDINT_NAMESPACE::int16_t;
using FTL_STDINT_NAMESPACE::uint16_t;
using FTL_STDINT_NAMESPACE::int32_t;
using FTL_STDINT_NAMESPACE::uint32_t;
using FTL_STDINT_NAMESPACE::int64_t;
using FTL_STDINT_NAMESPACE::uint64_t;

using FTL_STDINT_NAMESPACE::int_least8_t;
using FTL_STDINT_NAMESPACE::uint_least8_t;
using FTL_STDINT_NAMESPACE::int_least16_t;
using FTL_STDINT_NAMESPACE::uint_least16_t;
using FTL_STDINT_NAMESPACE::int_least32_t;
using FTL_STDINT_NAMESPACE::uint_least32_t;
using FTL_STDINT_NAMESPACE::int_least64_t;
using FTL_STDINT_NAMESPACE::uint_least64_t;

using FTL_STDINT_NAMESPACE::int_fast8_t;
using FTL_STDINT_NAMESPACE::uint_fast8_t;
using FTL_STDINT_NAMESPACE::int_fast16_t;
using FTL_STDINT_NAMESPACE::uint_fast16_t;
using FTL_STDINT_NAMESPACE::int_fast32_t;
using FTL_STDINT_NAMESPACE::uint_fast32_t;
using FTL_STDINT_NAMESPACE::int_fast64_t;
using FTL_STDINT_NAMESPACE::uint_fast64_t;

using FTL_STDINT_NAMESPACE::intptr_t;
using FTL_STDINT_NAMESPACE::uintptr_t;

using FTL_STDINT_NAMESPACE::intmax_t;
using FTL_STDINT_NAMESPACE::uintmax_t;

#undef FTL_STDINT_NAMESPACE

#endif // FTL_STDINT_H_HEADER
