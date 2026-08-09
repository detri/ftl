#ifdef FTL_REPLACE_STL
#include <stdatomic.h>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/stdatomic.h>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

#if __cpp_lib_stdatomic_h < 202011L
#error <stdatomic.h> must advertise __cpp_lib_stdatomic_h
#endif

#ifndef _Atomic
#error <stdatomic.h> must define _Atomic(T)
#endif

#ifndef ATOMIC_BOOL_LOCK_FREE
#error <stdatomic.h> must expose atomic lock-free macros
#endif

#ifndef ATOMIC_FLAG_INIT
#error <stdatomic.h> must expose ATOMIC_FLAG_INIT
#endif

static_assert(tested::is_same_v<_Atomic(int), tested::atomic<int>>);

static_assert(tested::is_same_v<atomic_int, tested::atomic_int>);

static_assert(tested::is_same_v<atomic_uint64_t, tested::atomic_uint64_t>);

static_assert(tested::is_same_v<atomic_ptrdiff_t, tested::atomic_ptrdiff_t>);

static_assert(tested::is_same_v<memory_order, tested::memory_order>);

bool global_atomic_operations_work() {
  atomic_int value{0};

  atomic_store_explicit(&value, 4, memory_order_release);

  if (atomic_load_explicit(&value, memory_order_acquire) != 4) {
    return false;
  }

  if (atomic_exchange(&value, 7) != 4)
    return false;

  int expected = 7;

  if (!atomic_compare_exchange_strong(&value, &expected, 11)) {
    return false;
  }

  if (atomic_fetch_add(&value, 3) != 11)
    return false;

  if (atomic_fetch_sub(&value, 2) != 14)
    return false;

  if (atomic_fetch_or(&value, 1) != 12)
    return false;

  if (atomic_fetch_and(&value, 12) != 13)
    return false;

  if (atomic_fetch_xor(&value, 4) != 12)
    return false;

  return atomic_load(&value) == 8;
}

bool atomic_flag_operations_work() {
  atomic_flag flag = ATOMIC_FLAG_INIT;

  if (atomic_flag_test_and_set_explicit(&flag, memory_order_acquire)) {
    return false;
  }

  if (!atomic_flag_test_and_set(&flag))
    return false;

  atomic_flag_clear_explicit(&flag, memory_order_release);

  return !atomic_flag_test_and_set(&flag);
}

bool ftl_test() {
  atomic_thread_fence(memory_order_seq_cst);

  atomic_signal_fence(memory_order_seq_cst);

  return global_atomic_operations_work() && atomic_flag_operations_work();
}
