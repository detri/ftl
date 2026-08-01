#ifdef FTL_REPLACE_STL
#include <typeindex>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/typeindex>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(tested::is_copy_constructible_v<tested::type_index>);
static_assert(tested::is_copy_assignable_v<tested::type_index>);
static_assert(noexcept(tested::type_index{typeid(int)}));

bool ftl_test() {
    const tested::type_index integer{typeid(int)};
    const tested::type_index other_integer{typeid(int)};
    const tested::type_index floating{typeid(float)};
    return integer == other_integer && integer != floating &&
           tested::hash<tested::type_index>{}(integer) == integer.hash_code() &&
           integer.name()[0] != '\0' && (integer < floating || integer > floating);
}
