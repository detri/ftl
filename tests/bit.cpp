#ifdef FTL_REPLACE_STL
#include <bit>
#include <cstdint>
namespace tested = std;
#else
#include <ftl/bit>
#include <ftl/cstdint>
namespace tested = ftl;
#endif

static_assert(tested::bit_cast<tested::uint32_t>(1.0f) == 0x3f800000u);
static_assert(tested::byteswap(tested::uint32_t{0x01020304}) == 0x04030201);
static_assert(tested::byteswap(-2) < 0);
static_assert(tested::byteswap('x') == 'x');
static_assert(tested::rotl(tested::uint8_t{1}, 1) == 2);
static_assert(tested::popcount(0xf0u) == 4);
static_assert(tested::bit_ceil(5u) == 8);
static_assert(tested::countl_zero(0u) == tested::numeric_limits<unsigned>::digits);

template<class T>
concept has_bit_width = requires(T value) { tested::bit_width(value); };
static_assert(has_bit_width<unsigned>);
static_assert(!has_bit_width<wchar_t>);

bool ftl_test() { return true; }
