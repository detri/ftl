// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_STDATOMIC_H_HEADER
#define FTL_STDATOMIC_H_HEADER

#ifdef _Atomic
#undef _Atomic
#endif

#ifdef FTL_REPLACE_STL
#include <atomic>
#define FTL_STDATOMIC_NAMESPACE std
#define _Atomic(T) std::atomic<T>
#else
#include <ftl/atomic>
#define FTL_STDATOMIC_NAMESPACE ftl
#define _Atomic(T) ftl::atomic<T>
#endif

// Memory orders.
using FTL_STDATOMIC_NAMESPACE::memory_order;
using FTL_STDATOMIC_NAMESPACE::memory_order_relaxed;
using FTL_STDATOMIC_NAMESPACE::memory_order_consume;
using FTL_STDATOMIC_NAMESPACE::memory_order_acquire;
using FTL_STDATOMIC_NAMESPACE::memory_order_release;
using FTL_STDATOMIC_NAMESPACE::memory_order_acq_rel;
using FTL_STDATOMIC_NAMESPACE::memory_order_seq_cst;

// Atomic flag.
using FTL_STDATOMIC_NAMESPACE::atomic_flag;

// Atomic aliases.
using FTL_STDATOMIC_NAMESPACE::atomic_bool;
using FTL_STDATOMIC_NAMESPACE::atomic_char;
using FTL_STDATOMIC_NAMESPACE::atomic_schar;
using FTL_STDATOMIC_NAMESPACE::atomic_uchar;
using FTL_STDATOMIC_NAMESPACE::atomic_short;
using FTL_STDATOMIC_NAMESPACE::atomic_ushort;
using FTL_STDATOMIC_NAMESPACE::atomic_int;
using FTL_STDATOMIC_NAMESPACE::atomic_uint;
using FTL_STDATOMIC_NAMESPACE::atomic_long;
using FTL_STDATOMIC_NAMESPACE::atomic_ulong;
using FTL_STDATOMIC_NAMESPACE::atomic_llong;
using FTL_STDATOMIC_NAMESPACE::atomic_ullong;

using FTL_STDATOMIC_NAMESPACE::atomic_char8_t;
using FTL_STDATOMIC_NAMESPACE::atomic_char16_t;
using FTL_STDATOMIC_NAMESPACE::atomic_char32_t;
using FTL_STDATOMIC_NAMESPACE::atomic_wchar_t;

using FTL_STDATOMIC_NAMESPACE::atomic_int8_t;
using FTL_STDATOMIC_NAMESPACE::atomic_uint8_t;
using FTL_STDATOMIC_NAMESPACE::atomic_int16_t;
using FTL_STDATOMIC_NAMESPACE::atomic_uint16_t;
using FTL_STDATOMIC_NAMESPACE::atomic_int32_t;
using FTL_STDATOMIC_NAMESPACE::atomic_uint32_t;
using FTL_STDATOMIC_NAMESPACE::atomic_int64_t;
using FTL_STDATOMIC_NAMESPACE::atomic_uint64_t;

using FTL_STDATOMIC_NAMESPACE::atomic_int_least8_t;
using FTL_STDATOMIC_NAMESPACE::atomic_uint_least8_t;
using FTL_STDATOMIC_NAMESPACE::atomic_int_least16_t;
using FTL_STDATOMIC_NAMESPACE::atomic_uint_least16_t;
using FTL_STDATOMIC_NAMESPACE::atomic_int_least32_t;
using FTL_STDATOMIC_NAMESPACE::atomic_uint_least32_t;
using FTL_STDATOMIC_NAMESPACE::atomic_int_least64_t;
using FTL_STDATOMIC_NAMESPACE::atomic_uint_least64_t;

using FTL_STDATOMIC_NAMESPACE::atomic_int_fast8_t;
using FTL_STDATOMIC_NAMESPACE::atomic_uint_fast8_t;
using FTL_STDATOMIC_NAMESPACE::atomic_int_fast16_t;
using FTL_STDATOMIC_NAMESPACE::atomic_uint_fast16_t;
using FTL_STDATOMIC_NAMESPACE::atomic_int_fast32_t;
using FTL_STDATOMIC_NAMESPACE::atomic_uint_fast32_t;
using FTL_STDATOMIC_NAMESPACE::atomic_int_fast64_t;
using FTL_STDATOMIC_NAMESPACE::atomic_uint_fast64_t;

using FTL_STDATOMIC_NAMESPACE::atomic_intptr_t;
using FTL_STDATOMIC_NAMESPACE::atomic_uintptr_t;
using FTL_STDATOMIC_NAMESPACE::atomic_size_t;
using FTL_STDATOMIC_NAMESPACE::atomic_ptrdiff_t;
using FTL_STDATOMIC_NAMESPACE::atomic_intmax_t;
using FTL_STDATOMIC_NAMESPACE::atomic_uintmax_t;

// Generic atomic operations.
using FTL_STDATOMIC_NAMESPACE::atomic_is_lock_free;

using FTL_STDATOMIC_NAMESPACE::atomic_load;
using FTL_STDATOMIC_NAMESPACE::atomic_load_explicit;

using FTL_STDATOMIC_NAMESPACE::atomic_store;
using FTL_STDATOMIC_NAMESPACE::atomic_store_explicit;

using FTL_STDATOMIC_NAMESPACE::atomic_exchange;
using FTL_STDATOMIC_NAMESPACE::atomic_exchange_explicit;

using FTL_STDATOMIC_NAMESPACE::atomic_compare_exchange_strong;
using FTL_STDATOMIC_NAMESPACE::atomic_compare_exchange_strong_explicit;
using FTL_STDATOMIC_NAMESPACE::atomic_compare_exchange_weak;
using FTL_STDATOMIC_NAMESPACE::atomic_compare_exchange_weak_explicit;

using FTL_STDATOMIC_NAMESPACE::atomic_fetch_add;
using FTL_STDATOMIC_NAMESPACE::atomic_fetch_add_explicit;
using FTL_STDATOMIC_NAMESPACE::atomic_fetch_sub;
using FTL_STDATOMIC_NAMESPACE::atomic_fetch_sub_explicit;

using FTL_STDATOMIC_NAMESPACE::atomic_fetch_and;
using FTL_STDATOMIC_NAMESPACE::atomic_fetch_and_explicit;
using FTL_STDATOMIC_NAMESPACE::atomic_fetch_or;
using FTL_STDATOMIC_NAMESPACE::atomic_fetch_or_explicit;
using FTL_STDATOMIC_NAMESPACE::atomic_fetch_xor;
using FTL_STDATOMIC_NAMESPACE::atomic_fetch_xor_explicit;

// Atomic flag operations.
using FTL_STDATOMIC_NAMESPACE::atomic_flag_test_and_set;
using FTL_STDATOMIC_NAMESPACE::atomic_flag_test_and_set_explicit;
using FTL_STDATOMIC_NAMESPACE::atomic_flag_clear;
using FTL_STDATOMIC_NAMESPACE::atomic_flag_clear_explicit;

// Fences.
using FTL_STDATOMIC_NAMESPACE::atomic_thread_fence;
using FTL_STDATOMIC_NAMESPACE::atomic_signal_fence;

#if !defined(__cpp_lib_stdatomic_h) || \
    __cpp_lib_stdatomic_h < 202011L
#undef __cpp_lib_stdatomic_h
#define __cpp_lib_stdatomic_h 202011L
#endif

#undef FTL_STDATOMIC_NAMESPACE

#endif // FTL_STDATOMIC_H_HEADER
