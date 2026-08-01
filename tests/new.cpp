#ifdef FTL_REPLACE_STL
#include <new>
namespace tested = std;
#else
#include <ftl/new>
namespace tested = ftl;
#endif

struct alignas(64) over_aligned {};

static_assert(noexcept(tested::launder(static_cast<int*>(nullptr))));
static_assert(requires(tested::size_t size, void* pointer) {
    ::operator new(size);
    ::operator new[](size);
    ::operator new(size, tested::align_val_t{64});
    ::operator new[](size, tested::align_val_t{64});
    ::operator new(size, tested::nothrow);
    ::operator new[](size, tested::nothrow);
    ::operator new(size, tested::align_val_t{64}, tested::nothrow);
    ::operator new[](size, tested::align_val_t{64}, tested::nothrow);
    ::operator delete(pointer);
    ::operator delete[](pointer);
    ::operator delete(pointer, size);
    ::operator delete[](pointer, size);
    ::operator delete(pointer, tested::align_val_t{64});
    ::operator delete[](pointer, tested::align_val_t{64});
    ::operator delete(pointer, tested::nothrow);
    ::operator delete[](pointer, tested::nothrow);
    ::operator delete(pointer, tested::align_val_t{64}, tested::nothrow);
    ::operator delete[](pointer, tested::align_val_t{64}, tested::nothrow);
    tested::destroying_delete;
});

bool ftl_test() {
    alignas(over_aligned) unsigned char storage[sizeof(over_aligned)];
    auto* placed = ::new (static_cast<void*>(storage)) over_aligned{};
    auto* value = tested::launder(placed);
    value->~over_aligned();

    return value == reinterpret_cast<over_aligned*>(storage) &&
           tested::bad_alloc{}.what()[0] == 'b' &&
           tested::bad_array_new_length{}.what()[4] == 'a';
}
