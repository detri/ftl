#ifdef FTL_REPLACE_STL
#include <ranges>
namespace tested = std;
#else
#include <ftl/ranges>
namespace tested = ftl;
#endif

struct close { constexpr bool operator()(int left, int right) const { return right - left <= 1; } };

template<class R>
constexpr bool equal(R&& range, const int* values, tested::size_t size) {
    auto iterator = range.begin();
    for (tested::size_t i = 0; i < size; ++i, ++iterator)
        if (iterator == range.end() || *iterator != values[i]) return false;
    return iterator == range.end();
}

constexpr bool chunk_by_works() {
    int values[] = {1, 2, 3, 7, 8, 20};
    auto view = values | tested::ranges::views::chunk_by(close{});
    auto iterator = view.begin();
    const int first[] = {1, 2, 3};
    const int second[] = {7, 8};
    const int third[] = {20};
    if (!equal(*iterator++, first, 3) || !equal(*iterator++, second, 2) || !equal(*iterator++, third, 1)) return false;
    if (iterator != view.end()) return false;
    return equal(*--iterator, third, 1) && equal(*--iterator, second, 2);
}

static_assert(chunk_by_works());
bool ftl_test() { return chunk_by_works(); }
