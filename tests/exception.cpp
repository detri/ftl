#ifdef FTL_REPLACE_STL
#include <exception>
#include <memory>
namespace tested = std;
#else
#include <ftl/exception>
#include <ftl/memory>
namespace tested = ftl;
#endif

struct error : tested::exception {
    const char* what() const noexcept override { return "error"; }
};

bool exception_ptr_works() {
    tested::exception_ptr pointer;
    try {
        throw error{};
    } catch (...) {
        pointer = tested::current_exception();
    }

    try {
        tested::rethrow_exception(pointer);
    } catch (const error& value) {
        return value.what()[0] == 'e';
    }
    return false;
}

bool nested_exception_works() {
    try {
        try {
            throw 42;
        } catch (...) {
            tested::throw_with_nested(error{});
        }
    } catch (const error& outer) {
        try {
            tested::rethrow_if_nested(outer);
        } catch (int value) {
            return value == 42;
        }
    }
    return false;
}

bool uncaught_count_works() {
    struct observer {
        bool* result;
        ~observer() {
            *result = tested::uncaught_exceptions() > 0;
        }
    };

    bool observed = false;
    try {
        observer value{&observed};
        throw 1;
    } catch (...) {}
    return observed;
}

static_assert(tested::is_base_of_v<tested::exception, tested::bad_alloc>);
static_assert(tested::is_base_of_v<tested::exception, tested::bad_weak_ptr>);

bool ftl_test() {
    return exception_ptr_works() &&
           nested_exception_works() &&
           uncaught_count_works();
}
