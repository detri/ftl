#ifdef FTL_REPLACE_STL
#include <scoped_allocator>
namespace tested = std;
#else
#include <ftl/scoped_allocator>
namespace tested = ftl;
#endif

struct aware {
    using allocator_type = tested::allocator<int>;
    int value;
    aware(tested::allocator_arg_t, const allocator_type&, int v) : value(v) {}
};

static_assert(tested::uses_allocator_v<aware, tested::allocator<int>>);
using adaptor = tested::scoped_allocator_adaptor<
    tested::allocator<aware>, tested::allocator<int>>;
static_assert(tested::is_same_v<adaptor::value_type, aware>);

bool ftl_test() {
    adaptor alloc;
    auto* p = alloc.allocate(1);
    alloc.construct(p, 42);
    const bool result = p->value == 42 &&
        alloc.outer_allocator() == tested::allocator<aware>{};
    alloc.destroy(p);
    alloc.deallocate(p, 1);
    return result;
}
