#ifdef FTL_REPLACE_STL
#include <any>
namespace tested = std;
#else
#include <ftl/any>
namespace tested = ftl;
#endif

struct aggregate {
    int first;
    int second;
};

struct noncopyable {
    noncopyable() = default;
    noncopyable(const noncopyable&) = delete;
};

struct throwing_value {
    explicit throwing_value(int) { throw 1; }
};

static_assert(!tested::is_constructible_v<
              tested::any, decltype(tested::in_place_type<noncopyable>)>);

bool ftl_test() {
    tested::any value = 3;
    if (!value.has_value() || tested::any_cast<int>(value) != 3)
        return false;
    tested::any copy = value;
    tested::any_cast<int&>(copy) = 4;
    if (tested::any_cast<int>(value) != 3 ||
        tested::any_cast<int>(copy) != 4 ||
        tested::any_cast<double>(&copy))
        return false;
    value.emplace<aggregate>(aggregate{5, 6});
    if (tested::any_cast<aggregate&>(value).second != 6)
        return false;
    auto made = tested::make_any<int>(7);
    value.swap(made);
    if (tested::any_cast<int>(value) != 7)
        return false;
#if FTL_HAS_EXCEPTIONS
    try {
        value.emplace<throwing_value>(1);
        return false;
    } catch (...) {
        if (value.has_value())
            return false;
    }
#endif
    value.reset();
    try {
        (void)tested::any_cast<int>(value);
    } catch (const tested::bad_any_cast&) {
        return !value.has_value() && value.type() == typeid(void);
    }
    return false;
}
