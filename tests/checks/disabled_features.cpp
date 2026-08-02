#include <ftl/array>
#include <ftl/exception>
#include <ftl/typeinfo>
#include <ftl/typeindex>
#include <any>
#include <expected>
#include <optional>
#include <variant>

#ifndef FTL_NO_THREADS
#error thread support must be disabled for this check
#endif

int main() {
    constexpr std::array<int, 1> value{42};
    static_assert(value[0] == 42);
    std::any erased = 3;
    std::expected<int, int> result = 4;
    std::optional<int> maybe = 5;
    std::variant<int, long> choice = 6;
    return std::any_cast<int>(erased) + *result + *maybe +
           std::get<int>(choice) == 18 ? 0 : 1;
}
