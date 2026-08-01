#include <cstddef>

extern "C" int ftl_entry() {
    constexpr auto value = std::to_integer<unsigned>(std::byte{7});
    return value == 7 ? 0 : 1;
}
