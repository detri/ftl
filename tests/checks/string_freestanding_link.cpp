#include <string>

extern "C" int ftl_entry() {
  constexpr std::string value = "small string";
  static_assert(value == "small string");
  return value.size() == 12 ? 0 : 1;
}
