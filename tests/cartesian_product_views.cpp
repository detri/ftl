#ifdef FTL_REPLACE_STL
#include <ranges>
#include <tuple>
namespace tested = std;
#else
#include <ftl/ranges>
#include <ftl/tuple>
namespace tested = ftl;
#endif

constexpr bool cartesian_product_works() {
    int numbers[] = {1, 2};
    char letters[] = {'a', 'b', 'c'};
    auto view = tested::ranges::views::cartesian_product(numbers, letters);
    if (view.size() != 6) return false;
    auto iterator = view.begin();
    const int expected_numbers[] = {1, 1, 1, 2, 2, 2};
    const char expected_letters[] = {'a', 'b', 'c', 'a', 'b', 'c'};
    for (int i = 0; i < 6; ++i, ++iterator) {
        auto value = *iterator;
        if (tested::get<0>(value) != expected_numbers[i] || tested::get<1>(value) != expected_letters[i]) return false;
    }
    if (iterator != view.end()) return false;
    auto empty = tested::ranges::views::cartesian_product();
    return empty.size() == 1;
}

static_assert(cartesian_product_works());
bool ftl_test() { return cartesian_product_works(); }
