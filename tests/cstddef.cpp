#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <type_traits>
namespace tested = std;
#else
#include <cstddef>
#include <ftl/cstddef>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

#ifndef FTL_REPLACE_STL
static_assert(ftl::is_same_v<ftl::size_t, std::size_t>);
#endif

struct empty {};
struct compressed {
    FTL_NO_UNIQUE_ADDRESS empty value;
    int payload;
};

struct layout {
    char prefix;
    int value;
};

enum class integer_like {};
constexpr int operator&(integer_like, int) { return 0; }

template<class T>
concept byte_shift = requires(T value) {
    tested::byte{} << value;
    tested::byte{} >> value;
};

template<class T>
concept byte_conversion = requires {
    tested::to_integer<T>(tested::byte{});
};

static_assert(sizeof(compressed) == sizeof(int));
static_assert(tested::is_same_v<decltype(sizeof(0)), tested::size_t>);
static_assert(tested::is_same_v<decltype(nullptr), tested::nullptr_t>);
static_assert(NULL == nullptr);
static_assert(tested::to_integer<unsigned>(tested::byte{3} << 1) == 6);
static_assert(byte_shift<int> && byte_conversion<int>);
static_assert(!byte_shift<float> && !byte_conversion<float>);
static_assert(!byte_shift<integer_like> && !byte_conversion<integer_like>);
static_assert(offsetof(layout, value) == 4);
static_assert(alignof(tested::max_align_t) >= alignof(long double));
static_assert(alignof(tested::max_align_t) >= alignof(void*));

bool ftl_test()
{
    return true;
}
