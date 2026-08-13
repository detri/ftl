#include <any>
#include <bitset>
#include <expected>
#include <ftl/array>
#include <ftl/exception>
#include <ftl/typeindex>
#include <ftl/typeinfo>
#include <functional>
#include <optional>
#include <stdexcept>
#include <variant>

#ifndef FTL_NO_THREADS
#error thread support must be disabled for this check
#endif

struct disabled_callable {
  int value;

  int operator()(int input) const { return value + input; }
};

int main() {
  [[maybe_unused]] auto any_type_observer = &std::any::type;
  constexpr std::array<int, 1> value{42};
  static_assert(value[0] == 42);
  static_assert(std::bitset<8>(0xa5).count() == 4);

  std::any erased = 3;
  std::expected<int, int> result = 4;
  std::optional<int> maybe = 5;
  std::variant<int, long> choice = 6;

  std::function<int(int)> callable = disabled_callable{7};

  if (callable(2) != 9)
    return 1;

  auto *stored = callable.target<disabled_callable>();
  if (!stored || stored->value != 7)
    return 1;

  if (callable.target<int>() != nullptr)
    return 1;

  return std::any_cast<int>(erased) + *result + *maybe +
                     std::get<int>(choice) ==
                 18
             ? 0
             : 1;
}
