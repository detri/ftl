#include <ftl/array>
#include <ftl/exception>
#include <ftl/typeinfo>
#include <ftl/typeindex>

#ifndef FTL_NO_THREADS
#error thread support must be disabled for this check
#endif

int main() {
    constexpr std::array<int, 1> value{42};
    static_assert(value[0] == 42);
}
