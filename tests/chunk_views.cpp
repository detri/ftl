#ifdef FTL_REPLACE_STL
#include <ranges>
namespace tested = std;
#else
#include <ftl/ranges>
namespace tested = ftl;
#endif

struct input_view : tested::ranges::view_base {
    int* first{}; int* last{};
    constexpr input_view() = default;
    constexpr input_view(int* first, int* last) : first(first), last(last) {}
    struct iterator {
        using value_type = int; using difference_type = tested::ptrdiff_t;
        using iterator_concept = tested::input_iterator_tag;
        int* current{};
        constexpr int& operator*() const { return *current; }
        constexpr iterator& operator++() { ++current; return *this; }
        constexpr void operator++(int) { ++current; }
        friend constexpr bool operator==(iterator, iterator) = default;
    };
    constexpr iterator begin() { return {first}; }
    constexpr iterator end() { return {last}; }
};

template<class R> constexpr int sum(R&& range) { int result = 0; for (int value : range) result += value; return result; }

constexpr bool chunk_works() {
    int values[] = {1, 2, 3, 4, 5};
    auto forward = values | tested::ranges::views::chunk(2);
    if (forward.size() != 3 || sum(forward[0]) != 3 || sum(forward[1]) != 7 || sum(forward[2]) != 5) return false;
    auto input = input_view{values, values + 5} | tested::ranges::views::chunk(2);
    auto iterator = input.begin();
    if (sum(*iterator) != 3) return false; ++iterator;
    if (sum(*iterator) != 7) return false; ++iterator;
    if (sum(*iterator) != 5) return false; ++iterator;
    return iterator == input.end();
}

static_assert(chunk_works());
bool ftl_test() { return chunk_works(); }
