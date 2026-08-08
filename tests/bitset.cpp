#ifdef FTL_REPLACE_STL
#include <bitset>
#include <functional>
#include <iosfwd>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/bitset>
#include <ftl/functional>
#include <ftl/iosfwd>
#include <ftl/limits>
#include <ftl/string>
#include <ftl/string_view>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

static_assert(__cpp_lib_constexpr_bitset >= 202207L);
static_assert(noexcept(tested::bitset<65>{}));
static_assert(noexcept(tested::bitset<65>(0ull)));
static_assert(tested::is_same_v<decltype(tested::bitset<8>{}[0]),
                                tested::bitset<8>::reference>);
static_assert(tested::is_same_v<
              decltype(tested::declval<tested::istream &>() >>
                       tested::declval<tested::bitset<8> &>()),
              tested::istream &>);
static_assert(tested::is_same_v<
              decltype(tested::declval<tested::ostream &>() <<
                       tested::declval<const tested::bitset<8> &>()),
              tested::ostream &>);

constexpr bool construction() {
  tested::bitset<0> empty;
  if (empty.size() != 0 || !empty.all() || empty.any() || !empty.none() ||
      empty.count() != 0 || empty.to_ullong() != 0 ||
      !empty.to_string().empty())
    return false;

  tested::bitset<5> integer(0xffu);
  if (integer.to_ullong() != 31 || integer.to_string() != "11111")
    return false;

  tested::bitset<6> string_value(tested::string("xx101101yy"), 2, 6);
  tested::bitset<4> view_value(tested::string_view("001011"), 1, 4);
  tested::bitset<4> pointer_value("1100tail", 4);
  tested::bitset<4> custom("abba", 4, 'a', 'b');
  tested::bitset<4> same_digits("0000", 4, '0', '0');
  return string_value.to_ullong() == 45 && view_value.to_ullong() == 5 &&
         pointer_value.to_ullong() == 12 &&
         custom.to_string('a', 'b') == "abba" && same_digits.none();
}
static_assert(construction());

constexpr bool operations() {
  tested::bitset<70> value;
  value.set(0).set(63).set(64).set(69);
  if (value.count() != 4 || !value.test(69) || value.all() || value.none())
    return false;

  tested::bitset<70> shifted = value << 1;
  if (!shifted[1] || !shifted[64] || !shifted[65] || shifted[0] ||
      shifted[69])
    return false;
  if ((shifted >> 1) != value.reset(69))
    return false;
  if ((value << 70).any() || (value >> 100).any())
    return false;

  value.flip(1).reset(0);
  auto bit = value[1];
  bit = false;
  const auto const_bit = value[2];
  const_bit = true;
  bool other = false;
  swap(value[2], other);
  if (value[2] || !other || ~value[2] != true)
    return false;
  value.set(2);
  swap(value[1], value[2]);
  if (!value[1] || value[2])
    return false;

  tested::bitset<8> left(0x55), right(0x0f);
  return (left & right).to_ullong() == 0x05 &&
         (left | right).to_ullong() == 0x5f &&
         (left ^ right).to_ullong() == 0x5a &&
         (~left).to_ullong() == 0xaa;
}
static_assert(operations());

bool ftl_test() {
  tested::bitset<70> partial;
  partial.set().flip(69).flip(69);
  if (!partial.all() || partial.count() != 70)
    return false;

  if (tested::hash<tested::bitset<70>>{}(partial) !=
      tested::hash<tested::bitset<70>>{}(tested::bitset<70>(partial)))
    return false;

#if FTL_HAS_EXCEPTIONS
  bool position = false;
  bool invalid = false;
  bool invalid_beyond_stored_bits = false;
  bool overflow = false;
  try {
    static_cast<void>(tested::bitset<3>(tested::string_view("0x")));
  } catch (const tested::invalid_argument &) {
    invalid = true;
  }
  try {
    static_cast<void>(tested::bitset<1>(tested::string_view("10x")));
  } catch (const tested::invalid_argument &) {
    invalid_beyond_stored_bits = true;
  }
  try {
    static_cast<void>(tested::bitset<3>(tested::string_view("01"), 3));
  } catch (const tested::out_of_range &) {
    position = true;
  }
  try {
    tested::bitset<65> value;
    value.set(64);
    static_cast<void>(value.to_ullong());
  } catch (const tested::overflow_error &) {
    overflow = true;
  }
  if (!position || !invalid || !invalid_beyond_stored_bits || !overflow)
    return false;

  if constexpr (tested::numeric_limits<unsigned long>::digits < 64) {
    bool ulong_overflow = false;
    try {
      tested::bitset<64> value;
      value.set(tested::numeric_limits<unsigned long>::digits);
      static_cast<void>(value.to_ulong());
    } catch (const tested::overflow_error &) {
      ulong_overflow = true;
    }
    if (!ulong_overflow)
      return false;
  }
#endif
  return true;
}
