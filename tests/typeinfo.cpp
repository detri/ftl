#ifdef FTL_REPLACE_STL
#include <typeinfo>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/typeinfo>
#include <ftl/exception>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

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

#ifdef FTL_REPLACE_STL
    return tested::bad_cast{}.what()[0] != '\0' &&
           tested::bad_typeid{}.what()[0] != '\0';

#elif !defined(_MSC_VER)
    try {
        polymorphic plain;
        (void)dynamic_cast<derived&>(plain);
    } catch (const tested::bad_cast& error) {
        polymorphic* null = nullptr;

        try {
            (void)typeid(*null);
        } catch (const tested::bad_typeid& typeid_error) {
            return error.what()[0] != '\0' &&
                   typeid_error.what()[0] != '\0';
        }
    }

    return false;

#else
    return tested::bad_cast{}.what()[0] != '\0' &&
           tested::bad_typeid{}.what()[0] != '\0';
#endif
}
