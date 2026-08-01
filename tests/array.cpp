#ifdef FTL_REPLACE_STL
#include <array>
namespace tested = std;
#else
#include <ftl/array>
namespace tested = ftl;
#endif

struct legacy_ordered {
    int value;
    friend constexpr bool operator==(legacy_ordered, legacy_ordered) = default;
    friend constexpr bool operator<(legacy_ordered left, legacy_ordered right) {
        return left.value < right.value;
    }
};

using two_ints = tested::array<int, 2>;
static_assert(tested::is_same_v<decltype(tested::array{1, 2}), two_ints>);
static_assert(tested::is_same_v<decltype(&two_ints::size),
                               two_ints::size_type (two_ints::*)() const noexcept>);
static_assert(!noexcept(tested::declval<two_ints&>().front()));
static_assert(tested::array<legacy_ordered, 1>{{1}} <
              tested::array<legacy_ordered, 1>{{2}});

constexpr bool array_works() {
    tested::array<int, 3> values{1, 2, 3};
    tested::array<int, 3> other{};
    other.fill(4);
    values.swap(other);
    return values.size() == 3 && values.front() == 4 &&
           *values.rbegin() == 4 && other.back() == 3 &&
           tested::get<1>(other) == 2 &&
           tested::to_array({5, 6})[1] == 6 &&
           tested::array<double, 1>{1.0} < tested::array<double, 1>{2.0} &&
           tested::array<int, 0>{}.empty();
}

static_assert(array_works());

bool ftl_test() { return array_works(); }
