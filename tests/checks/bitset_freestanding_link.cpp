#include <bitset>

extern "C" int ftl_entry() {
  constexpr std::bitset<70> value = std::bitset<70>(1) << 69;
  static_assert(value.count() == 1 && value.test(69));
  static_assert(value.to_string().size() == 70);
  static_assert(value.to_string().front() == '1');
  static_assert(value.to_string().back() == '0');
  return value.count() == 1 ? 0 : 1;
}
