#ifdef FTL_REPLACE_STL
#include <variant>
namespace tested = std;
#else
#include <ftl/variant>
namespace tested = ftl;
#endif

struct value {
    int number;
    constexpr explicit value(int input) : number(input) {}
    friend constexpr bool operator==(const value&, const value&) = default;
};

constexpr bool variant_works() {
    tested::variant<int, value> item;
    if (item.index() != 0 || tested::get<int>(item) != 0) return false;
    item.emplace<value>(3);
    if (!tested::holds_alternative<value>(item) || tested::get<1>(item).number != 3) return false;
    auto result = tested::visit([](const auto& current) {
        if constexpr (tested::is_same_v<tested::remove_cvref_t<decltype(current)>, int>)
            return current;
        else
            return current.number;
    }, item);
    tested::variant<int, value> copy = item;
    return result == 3 && copy == item && tested::get_if<value>(&copy)->number == 3;
}

static_assert(variant_works());
static_assert(tested::variant_size_v<tested::variant<int, float>> == 2);
static_assert(tested::is_same_v<tested::variant_alternative_t<1, tested::variant<int, float>>, float>);
static_assert(tested::is_trivially_copyable_v<tested::variant<int, float>>);
static_assert(tested::is_copy_constructible_v<tested::variant<int, int>>);
static_assert(tested::variant<float, long>{1}.index() == 1);

bool ftl_test() {
    if (!variant_works()) return false;
    tested::variant<int, int> duplicate(tested::in_place_index<1>, 6);
    auto duplicate_copy = duplicate;
    if (tested::get<1>(duplicate_copy) != 6 || duplicate_copy != duplicate)
        return false;
    tested::variant<int, value> first(4);
    tested::variant<int, value> second(value{2});
    bool visited = false;
    tested::visit([&](const auto&) { visited = true; }, first);
    first.swap(second);
    if (!visited || tested::get<value>(first).number != 2 ||
        tested::get<int>(second) != 4)
        return false;
    if (tested::visit([](const auto& left, const auto& right) {
            auto number = [](const auto& item) {
                if constexpr (tested::is_same_v<tested::remove_cvref_t<decltype(item)>, int>) return item;
                else return item.number;
            };
            return number(left) + number(right);
        }, first, second) != 6)
        return false;
    tested::variant<int, value> item(4);
    try {
        (void)tested::get<value>(item);
    } catch (const tested::bad_variant_access&) {
        return true;
    }
    return false;
}
