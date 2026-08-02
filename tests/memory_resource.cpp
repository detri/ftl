#ifdef FTL_REPLACE_STL
#include <memory_resource>
namespace tested = std;
#else
#include <ftl/memory_resource>
namespace tested = ftl;
#endif

struct value { int number; explicit value(int n) : number(n) {} };

bool ftl_test() {
    alignas(value) unsigned char storage[64];
    tested::pmr::monotonic_buffer_resource arena(storage, sizeof storage);
    tested::pmr::polymorphic_allocator<value> alloc(&arena);
    value* p = alloc.new_object<value>(42);
    const bool result = p->number == 42 &&
        alloc.resource() == &arena &&
        tested::pmr::new_delete_resource() ==
            tested::pmr::new_delete_resource();
    alloc.delete_object(p);
    tested::pmr::unsynchronized_pool_resource pool;
    tested::pmr::polymorphic_allocator<int> pool_alloc(&pool);
    int* values = pool_alloc.allocate(2);
    values[0] = 7;
    const bool pool_result = values[0] == 7 &&
        pool.upstream_resource() == tested::pmr::get_default_resource();
    pool_alloc.deallocate(values, 2);
    pool.release();
    auto* old = tested::pmr::set_default_resource(&arena);
    const bool default_result = tested::pmr::get_default_resource() == &arena;
    tested::pmr::set_default_resource(old);
    return result && pool_result && default_result;
}
