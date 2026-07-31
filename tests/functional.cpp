#ifdef FTL_REPLACE_STL
#include <functional>
namespace tested = std;
#else
#include <ftl/functional>
namespace tested = ftl;
#endif

struct object {
    int value;
    constexpr int add(int input) const noexcept { return value + input; }
};

constexpr bool functional_works() {
    object item{3};
    auto reference = tested::ref(item);
    return tested::invoke(&object::add, reference, 2) == 5 &&
           tested::invoke(&object::value, &item) == 3 &&
           reference.get().value == 3;
}

static_assert(functional_works());
static_assert(tested::is_invocable_v<decltype(&object::add), object&, int>);
static_assert(tested::hash<unsigned>{}(42) ==
              ftl_rapidhash::rapidhash_t<unsigned>{}(42));
static_assert(tested::hash<tested::string_view>{}("rapidhash") ==
              ftl_rapidhash::rapidhash("rapidhash"));

bool ftl_test() { return functional_works(); }
