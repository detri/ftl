#ifdef FTL_REPLACE_STL
#include <new>
namespace tested = std;
#else
#include <ftl/new>
namespace tested = ftl;
#endif

struct alignas(64) over_aligned {};

static_assert(noexcept(tested::launder(static_cast<int*>(nullptr))));
template<class T>
concept can_launder = requires(T* value) { tested::launder(value); };
static_assert(!can_launder<void>);
static_assert(tested::hardware_constructive_interference_size > 0);
static_assert(tested::hardware_destructive_interference_size >=
              tested::hardware_constructive_interference_size);
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
    ::operator delete(pointer, size, tested::align_val_t{64});
    ::operator delete[](pointer, size, tested::align_val_t{64});
    ::operator delete(pointer, tested::nothrow);
    ::operator delete[](pointer, tested::nothrow);
    ::operator delete(pointer, tested::align_val_t{64}, tested::nothrow);
    ::operator delete[](pointer, tested::align_val_t{64}, tested::nothrow);
    tested::destroying_delete;
});

bool ftl_test() {
    const auto old_handler = tested::set_new_handler(nullptr);
    const bool handler_works = tested::get_new_handler() == nullptr;
    tested::set_new_handler(old_handler);
    alignas(over_aligned) unsigned char storage[sizeof(over_aligned)];
    auto* placed = ::new (static_cast<void*>(storage)) over_aligned{};
    auto* value = tested::launder(placed);
    value->~over_aligned();

    return handler_works && value == reinterpret_cast<over_aligned*>(storage) &&
           tested::bad_alloc{}.what()[0] == 'b' &&
           tested::bad_array_new_length{}.what()[4] == 'a';
}
