#include <ftl/array>

static int allocations;
alignas(ftl::max_align_t) static unsigned char storage[1024];

void* operator new(decltype(sizeof(0))) {
    ++allocations;
    return storage;
}

void* operator new[](decltype(sizeof(0))) {
    ++allocations;
    return storage;
}

void operator delete(void*) noexcept {}
void operator delete[](void*) noexcept {}

int main() {
    constexpr ftl::array<int, 3> values{1, 2, 3};
    static_assert(values.size() == 3);
    return allocations;
}
