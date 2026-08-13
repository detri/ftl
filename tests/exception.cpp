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

struct private_nested : private tested::nested_exception {
    virtual ~private_nested() = default;
};
struct nested_left : tested::nested_exception {};
struct nested_right : tested::nested_exception {};
struct ambiguous_nested : nested_left, nested_right {};

static_assert(requires(const private_nested& value) {
    tested::rethrow_if_nested(value);
});
static_assert(requires(const ambiguous_nested& value) {
    tested::rethrow_if_nested(value);
});

bool exception_ptr_works() {
    tested::exception_ptr pointer;
    try {
        throw error{};
    } catch (...) {
        pointer = tested::current_exception();
    }

    tested::exception_ptr copy = pointer;
    tested::exception_ptr empty;
    tested::swap(copy, empty);
    copy = nullptr;
    if (copy || !empty || empty != pointer)
        return false;

    try {
        tested::rethrow_exception(empty);
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

bool make_exception_ptr_works() {
    try {
        tested::rethrow_exception(tested::make_exception_ptr(error{}));
    } catch (const error&) {
        return true;
    }
    return false;
}

static_assert(tested::is_base_of_v<tested::exception, tested::bad_alloc>);
static_assert(tested::is_base_of_v<tested::exception, tested::bad_weak_ptr>);

bool ftl_test() {
    const auto handler = tested::get_terminate();
    const bool terminate_handler_works =
        tested::set_terminate(handler) == handler;
    return terminate_handler_works && exception_ptr_works() &&
           nested_exception_works() &&
           uncaught_count_works() && make_exception_ptr_works();
}
