#include <typeinfo>
#include <type_traits>
namespace tested = std;

struct polymorphic { virtual ~polymorphic() = default; };
struct derived : polymorphic {};

static_assert(!tested::is_copy_constructible_v<tested::type_info>);
static_assert(!tested::is_copy_assignable_v<tested::type_info>);
static_assert(tested::is_base_of_v<tested::exception, tested::bad_cast>);
static_assert(tested::is_base_of_v<tested::exception, tested::bad_typeid>);

bool ftl_test() {
    derived value;
    polymorphic& base = value;

    const auto& dynamic = typeid(base);

    if (!(dynamic == typeid(derived)) ||
        dynamic.before(typeid(int)) == typeid(int).before(dynamic) ||
        !dynamic.name()[0]) {
        return false;
        }

    return tested::bad_cast{}.what()[0] != '\0' &&
           tested::bad_typeid{}.what()[0] != '\0';

}
