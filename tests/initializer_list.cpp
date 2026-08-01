#ifdef FTL_REPLACE_STL
#include <initializer_list>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/initializer_list>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

#ifndef FTL_REPLACE_STL
#include <initializer_list>
#endif

constexpr int sum(tested::initializer_list<int> values) {
    int result = 0;
    for (int value : values)
        result += value;
    return result;
}

static_assert(sum({1, 2, 3}) == 6);
static_assert(sum({}) == 0);

using list = tested::initializer_list<int>;
static_assert(tested::is_same_v<list::value_type, int>);
static_assert(tested::is_same_v<list::reference, const int&>);
static_assert(tested::is_same_v<list::const_reference, const int&>);
static_assert(tested::is_same_v<list::iterator, const int*>);
static_assert(tested::is_same_v<list::const_iterator, const int*>);
static_assert(tested::is_same_v<list::size_type, decltype(sizeof(0))>);
static_assert(tested::is_trivially_copyable_v<list>);
static_assert(sizeof(list) == 2 * sizeof(void*));

constexpr list empty;
static_assert(empty.size() == 0);
static_assert(empty.begin() == empty.end());
static_assert(tested::begin(empty) == tested::end(empty));

bool ftl_test() { return true; }
