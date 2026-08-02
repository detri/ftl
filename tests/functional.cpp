#ifdef FTL_REPLACE_STL
#include <functional>
namespace tested = std;
#else
#include <ftl/functional>
namespace tested = ftl;
#endif

struct object {
    int value;
    constexpr int add(int input) const noexcept { return value + input; }
};

struct move_only_callable {
    int value;
    move_only_callable(int input) : value(input) {}
    move_only_callable(move_only_callable&&) = default;
    move_only_callable(const move_only_callable&) = delete;
    int operator()(int input) { return value + input; }
};

struct copyable_callable {
    int value;

    int operator()(int input) const {
        return value + input;
    }
};

template<class T>
concept can_ref_temporary = requires { tested::ref(T{}); };

constexpr bool functional_works() {
    object item{3};
    auto reference = tested::ref(item);
    return tested::invoke(&object::add, reference, 2) == 5 &&
           tested::invoke(&object::value, &item) == 3 &&
           reference.get().value == 3;
}

static_assert(functional_works());
static_assert(tested::is_invocable_v<decltype(&object::add), object&, int>);
static_assert(!can_ref_temporary<int>);
static_assert(tested::is_same_v<tested::common_reference_t<
    tested::reference_wrapper<int>, int&>, int&>);
static_assert(tested::plus<>{}(2, 3) == 5);
static_assert(tested::minus<>{}(7, 2) == 5);
static_assert(tested::multiplies<>{}(3, 4) == 12);
static_assert(tested::divides<>{}(8, 2) == 4);
static_assert(tested::modulus<>{}(7, 4) == 3);
static_assert(tested::negate<>{}(3) == -3);
static_assert(tested::equal_to<>{}(2, 2));
static_assert(tested::not_equal_to<>{}(2, 3));
static_assert(tested::greater<>{}(3, 2));
static_assert(tested::less<>{}(2, 3));
static_assert(tested::greater_equal<>{}(3, 3));
static_assert(tested::less_equal<>{}(3, 3));
static_assert(tested::logical_and<>{}(true, true));
static_assert(tested::logical_or<>{}(false, true));
static_assert(tested::logical_not<>{}(false));
static_assert(tested::bit_and<>{}(7, 3) == 3);
static_assert(tested::bit_or<>{}(4, 3) == 7);
static_assert(tested::bit_xor<>{}(7, 3) == 4);
static_assert(tested::bit_not<unsigned>{}(0u) == ~0u);
static_assert(tested::identity{}(42) == 42);
static_assert(tested::not_fn(tested::equal_to<>{})(2, 3));
static_assert(tested::bind_front(tested::plus<>{}, 2)(3) == 5);
static_assert(tested::bind_back(tested::minus<>{}, 2)(7) == 5);
static_assert(tested::bind(tested::minus<>{},
    tested::placeholders::_2, tested::placeholders::_1)(2, 7) == 5);
static_assert(tested::mem_fn(&object::add)(object{3}, 2) == 5);
static_assert(tested::ranges::equal_to{}(2, 2));
static_assert(tested::ranges::less{}(2, 3));
static_assert(tested::is_constructible_v<
    tested::move_only_function<int(int) const & noexcept>,
    decltype([](int value) noexcept { return value; })>);
static_assert(tested::hash<unsigned>{}(42) ==
              ftl_rapidhash::rapidhash_t<unsigned>{}(42));
static_assert(tested::hash<tested::string_view>{}("rapidhash") ==
              ftl_rapidhash::rapidhash("rapidhash"));

bool ftl_test() {
    tested::function<int(int)> copyable = copyable_callable{3};
    tested::function deduced = [](int input) { return input + 1; };
    if (deduced(1) != 2) return false;
    auto* target = copyable.target<copyable_callable>();
    if (!target || target->value != 3)
        return false;

    if (copyable.target<int>() != nullptr)
        return false;

    const auto& const_copyable = copyable;
    const auto* const_target =
        const_copyable.target<copyable_callable>();

    if (!const_target || const_target->value != 3)
        return false;
    tested::function<int(int)> copy = copyable;
    if (copy(2) != 5)
        return false;

#if !defined(FTL_NO_RTTI) && \
    (defined(_CPPRTTI) || defined(__GXX_RTTI))
    if (copy.target_type() == typeid(void))
        return false;
#endif
    copy = nullptr;
    if (copy != nullptr) return false;

    tested::move_only_function<int(int)> movable = move_only_callable{3};
    if (movable(2) != 5) return false;

    constexpr int pattern[] = {2, 3};
    constexpr int values[] = {1, 2, 3, 4};
    auto result = tested::default_searcher(pattern, pattern + 2)(
        values, values + 4);
    if (result.first != values + 1 || result.second != values + 3)
        return false;
    auto bm = tested::boyer_moore_searcher(pattern, pattern + 2)(
        values, values + 4);
    auto bmh = tested::boyer_moore_horspool_searcher(pattern, pattern + 2)(
        values, values + 4);
    return functional_works() && bm.first == values + 1 &&
           bmh.first == values + 1;
}
