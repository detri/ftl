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

struct list_value {
    constexpr list_value(tested::initializer_list<int>) {}
};

template<class V>
concept can_emplace_out_of_bounds = requires(V& item) {
    item.template emplace<9>();
};

template<class V>
concept can_emplace_duplicate_type = requires(V& item) {
    item.template emplace<list_value>(tested::initializer_list<int>{});
};

struct derived_variant : tested::variant<int, value> {
    using base = tested::variant<int, value>;
    using tested::variant<int, value>::variant;
    constexpr derived_variant(int input) : base(input) {}
};

#if FTL_HAS_EXCEPTIONS
struct guarded_copy {
    int number{};
    static inline bool throw_on_copy = false;
    guarded_copy() = default;
    explicit guarded_copy(int input) : number(input) {}
    guarded_copy(const guarded_copy& other) : number(other.number) {
        if (throw_on_copy) throw 1;
    }
    guarded_copy(guarded_copy&&) noexcept = default;
    guarded_copy& operator=(const guarded_copy&) = default;
    guarded_copy& operator=(guarded_copy&&) = default;
};
#endif

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
static_assert(!can_emplace_out_of_bounds<tested::variant<int, float>>);
static_assert(!can_emplace_duplicate_type<tested::variant<list_value,
                                                          list_value>>);
static_assert(tested::visit([](const auto& item) {
    if constexpr (tested::is_same_v<tested::remove_cvref_t<decltype(item)>, int>)
        return item;
    else
        return item.number;
}, derived_variant{7}) == 7);

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
#if FTL_HAS_EXCEPTIONS
    tested::variant<int, guarded_copy> preserved(17);
    tested::variant<int, guarded_copy> source(tested::in_place_index<1>, 8);
    guarded_copy::throw_on_copy = true;
    try {
        preserved = source;
        return false;
    } catch (...) {
        guarded_copy::throw_on_copy = false;
        if (preserved.index() != 0 || tested::get<0>(preserved) != 17)
            return false;
    }
#endif
    try {
        (void)tested::get<value>(item);
    } catch (const tested::bad_variant_access&) {
        return true;
    }
    return false;
}
