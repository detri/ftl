#ifdef FTL_REPLACE_STL
#include <atomic>
namespace tested = std;
#else
#include <ftl/atomic>
namespace tested = ftl;
#endif

bool atomic_works() {
    tested::atomic_long value{1};
    const auto old = value.fetch_add(2, tested::memory_order_relaxed);
    long expected = 3;
    return old == 1 &&
           value.compare_exchange_strong(
               expected, 4, tested::memory_order_acq_rel,
               tested::memory_order_acquire) &&
           value.load(tested::memory_order_acquire) == 4;
}

static_assert(tested::atomic_long::is_always_lock_free);

bool ftl_test() { return atomic_works(); }
