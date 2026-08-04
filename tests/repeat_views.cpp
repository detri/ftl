#ifdef FTL_REPLACE_STL
#include <ranges>
namespace tested = std;
#else
#include <ftl/ranges>
namespace tested = ftl;
#endif

struct move_only_value {
    int value;
    constexpr explicit move_only_value(int value) : value(value) {}
    move_only_value(const move_only_value&) = delete;
    constexpr move_only_value(move_only_value&&) = default;
    constexpr move_only_value& operator=(move_only_value&&) = default;
};

constexpr bool repeat_works() {
    auto bounded = tested::ranges::views::repeat(7, 3);
    if (bounded.size() != 3 || bounded[0] != 7 || bounded[2] != 7) return false;
    auto iterator = bounded.begin();
    iterator += 3;
    if (iterator != bounded.end()) return false;
    auto unbounded = tested::ranges::views::repeat(4);
    auto moved = tested::ranges::views::repeat(move_only_value{9}, 2);
    return *unbounded.begin() == 4 && *(unbounded.begin() + 100) == 4 &&
        (*moved.begin()).value == 9;
}

static_assert(repeat_works());
bool ftl_test() { return repeat_works(); }
