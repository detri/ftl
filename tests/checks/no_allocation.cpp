#include <ftl/array>
#include <ftl/functional>

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

int increment(int value) { return value + 1; }

struct callable {
    int operator()(int value) const { return value + 2; }
};

int main() {
    constexpr ftl::array<int, 3> values{1, 2, 3};
    static_assert(values.size() == 3);
    ftl::move_only_function<int(int)> pointer = &increment;
    callable object;
    ftl::move_only_function<int(int)> reference = ftl::ref(object);
    if (pointer(1) != 2 || reference(1) != 3)
        return 1;
    return allocations;
}
