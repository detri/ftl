#ifdef FTL_REPLACE_STL
#include <array>
namespace tested = std;
#else
#include <ftl/array>
namespace tested = ftl;
#endif

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
