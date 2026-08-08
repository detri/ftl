#ifdef FTL_REPLACE_STL
#include <cstdint>
#include <detail/big_uint>
namespace tested = std;
#else
#include <ftl/cstdint>
#include <ftl/detail/big_uint>
namespace tested = ftl;
#endif

using ftl_float_conversion::big_uint;

constexpr bool construction_tests() {
  {
    big_uint<4> value;

    if (!value.is_zero())
      return false;

    if (value.limb_count() != 0)
      return false;

    if (value.bit_width() != 0)
      return false;
  }

  {
    big_uint<4> value(42);

    if (value.is_zero())
      return false;

    if (value.limb_count() != 1)
      return false;

    if (value.limb(0) != 42)
      return false;

    if (value.bit_width() != 6)
      return false;
  }

  return true;
}

static_assert(construction_tests());

constexpr bool comparison_tests() {
  big_uint<4> a(10);
  big_uint<4> b(20);
  big_uint<4> c(10);

  if (a.compare(b) >= 0)
    return false;

  if (b.compare(a) <= 0)
    return false;

  if (a.compare(c) != 0)
    return false;

  if (!(a == c))
    return false;

  if (a == b)
    return false;

  if (!b.shift_left(64))
    return false;

  if (b.compare(a) <= 0)
    return false;

  return true;
}

static_assert(comparison_tests());

constexpr bool shift_tests() {
  {
    big_uint<4> value(1);

    if (!value.shift_left(64))
      return false;

    if (value.limb_count() != 2)
      return false;

    if (value.limb(0) != 0)
      return false;

    if (value.limb(1) != 1)
      return false;

    if (value.bit_width() != 65)
      return false;
  }

  {
    big_uint<4> value(static_cast<tested::uint64_t>(-1));

    if (!value.shift_left(1))
      return false;

    if (value.limb(0) != static_cast<tested::uint64_t>(-2))
      return false;

    if (value.limb(1) != 1)
      return false;

    if (value.bit_width() != 65)
      return false;
  }

  {
    big_uint<1> value(1);
    const auto original = value;

    if (value.shift_left(64))
      return false;

    if (!(value == original))
      return false;
  }

  return true;
}

static_assert(shift_tests());

constexpr bool addition_tests() {
  {
    big_uint<4> value(static_cast<tested::uint64_t>(-1));

    if (!value.add(1))
      return false;

    if (value.limb_count() != 2)
      return false;

    if (value.limb(0) != 0 || value.limb(1) != 1)
      return false;
  }

  {
    big_uint<4> left(static_cast<tested::uint64_t>(-1));

    big_uint<4> right(1);

    if (!left.add(right))
      return false;

    if (left.limb(0) != 0 || left.limb(1) != 1)
      return false;
  }

  {
    big_uint<1> value(static_cast<tested::uint64_t>(-1));

    const auto original = value;

    if (value.add(1))
      return false;

    if (!(value == original))
      return false;
  }

  return true;
}

static_assert(addition_tests());

constexpr bool subtraction_tests() {
  {
    big_uint<4> value(1);

    if (!value.shift_left(64))
      return false;

    big_uint<4> one(1);

    if (!value.subtract(one))
      return false;

    if (value.limb_count() != 1)
      return false;

    if (value.limb(0) != static_cast<tested::uint64_t>(-1))
      return false;
  }

  {
    big_uint<4> value(100);
    big_uint<4> amount(40);

    if (!value.subtract(amount))
      return false;

    if (value.limb(0) != 60)
      return false;
  }

  {
    big_uint<4> value(10);
    big_uint<4> amount(11);

    const auto original = value;

    if (value.subtract(amount))
      return false;

    if (!(value == original))
      return false;
  }

  {
    big_uint<4> value(1234);

    if (!value.subtract(value))
      return false;

    if (!value.is_zero())
      return false;
  }

  return true;
}

static_assert(subtraction_tests());

constexpr bool multiplication_tests() {
  {
    big_uint<4> value(static_cast<tested::uint64_t>(-1));

    if (!value.multiply(2))
      return false;

    if (value.limb_count() != 2)
      return false;

    if (value.limb(0) != static_cast<tested::uint64_t>(-2))
      return false;

    if (value.limb(1) != 1)
      return false;
  }

  {
    big_uint<4> value(123456789);

    if (!value.multiply(10))
      return false;

    if (value.limb(0) != 1234567890)
      return false;
  }

  {
    big_uint<4> value(123);

    if (!value.multiply(0))
      return false;

    if (!value.is_zero())
      return false;
  }

  {
    big_uint<1> value(static_cast<tested::uint64_t>(-1));

    const auto original = value;

    if (value.multiply(2))
      return false;

    if (!(value == original))
      return false;
  }

  {
    constexpr tested::uint64_t maximum = static_cast<tested::uint64_t>(-1);

    big_uint<4> value(maximum);

    if (!value.multiply(maximum))
      return false;

    if (value.limb(0) != 1)
      return false;

    if (value.limb(1) != maximum - 1)
      return false;
  }

  return true;
}

static_assert(multiplication_tests());

constexpr bool power_of_five_tests() {
  {
    big_uint<4> value(1);

    if (!value.multiply_pow5(0))
      return false;

    if (value.limb(0) != 1)
      return false;
  }

  {
    big_uint<4> value(1);

    if (!value.multiply_pow5(27))
      return false;

    if (value.limb_count() != 1)
      return false;

    if (value.limb(0) != 7450580596923828125ull)
      return false;
  }

  /*
   * 5^28 =
   *
   *   0x2'04fce5e3e2502611
   */
  {
    big_uint<4> value(1);

    if (!value.multiply_pow5(28))
      return false;

    if (value.limb_count() != 2)
      return false;

    if (value.limb(0) != 0x04fce5e3e2502611ull)
      return false;

    if (value.limb(1) != 0x2ull)
      return false;
  }

  /*
   * Exercises multiple 5^27 chunks:
   *
   * 5^54 =
   *
   * 0x29c30f1029939b14'
   *   6664242d97d9f649
   */
  {
    big_uint<4> value(1);

    if (!value.multiply_pow5(54))
      return false;

    if (value.limb_count() != 2)
      return false;

    if (value.limb(0) != 0x6664242d97d9f649ull)
      return false;

    if (value.limb(1) != 0x29c30f1029939b14ull)
      return false;
  }

  {
    big_uint<1> value(static_cast<tested::uint64_t>(-1));

    const auto original = value;

    if (value.multiply_pow5(1))
      return false;

    if (!(value == original))
      return false;
  }

  return true;
}

static_assert(power_of_five_tests());

constexpr bool division_tests() {
  {
    big_uint<4> value(12345);

    const auto result = value.divide(10);

    if (!result.success)
      return false;

    if (result.remainder != 5)
      return false;

    if (value.limb_count() != 1 || value.limb(0) != 1234)
      return false;
  }

  /*
   * 2^64 / 10 =
   *
   * quotient  = 1844674407370955161
   * remainder = 6
   */
  {
    big_uint<4> value(1);

    if (!value.shift_left(64))
      return false;

    const auto result = value.divide(10);

    if (!result.success || result.remainder != 6)
      return false;

    if (value.limb_count() != 1)
      return false;

    if (value.limb(0) != 1844674407370955161ull)
      return false;
  }

  /*
   * This specifically exercises the 65-bit intermediate
   * path in the portable division algorithm:
   *
   * 2^64 / (2^64 - 1) = 1 remainder 1.
   */
  {
    constexpr tested::uint64_t maximum = static_cast<tested::uint64_t>(-1);

    big_uint<4> value(1);

    if (!value.shift_left(64))
      return false;

    const auto result = value.divide(maximum);

    if (!result.success || result.remainder != 1)
      return false;

    if (value.limb_count() != 1 || value.limb(0) != 1)
      return false;
  }

  /*
   * Multi-limb round trip:
   *
   * original == quotient * divisor + remainder.
   */
  {
    big_uint<4> original(1);

    if (!original.shift_left(190))
      return false;

    if (!original.add(0xfedcba9876543210ull))
      return false;

    auto quotient = original;

    constexpr tested::uint64_t divisor = 10000000000000000000ull;

    const auto result = quotient.divide(divisor);

    if (!result.success)
      return false;

    if (!quotient.multiply(divisor))
      return false;

    if (!quotient.add(result.remainder))
      return false;

    if (!(quotient == original))
      return false;
  }

  {
    big_uint<4> value(123);
    const auto original = value;

    const auto result = value.divide(0);

    if (result.success)
      return false;

    if (!(value == original))
      return false;
  }

  {
    big_uint<4> value;

    const auto result = value.divide(17);

    if (!result.success || result.remainder != 0 || !value.is_zero())
      return false;
  }

  return true;
}

static_assert(division_tests());

constexpr bool clear_tests() {
  big_uint<4> value(123);

  value.clear();

  return value.is_zero() && value.limb_count() == 0 && value.bit_width() == 0 &&
         value.limb(0) == 0;
}

static_assert(clear_tests());

constexpr bool bit_access_tests() {
  big_uint<4> value(5);

  if (!value.shift_left(64))
    return false;

  if (!value.add(3))
    return false;

  /*
   * value == 2^66 + 2^64 + 3
   */
  if (!value.test_bit(0))
    return false;

  if (!value.test_bit(1))
    return false;

  if (value.test_bit(2))
    return false;

  if (!value.test_bit(64))
    return false;

  if (!value.test_bit(66))
    return false;

  if (value.test_bit(67))
    return false;

  if (value.any_low_bits(0))
    return false;

  if (!value.any_low_bits(1))
    return false;

  if (!value.any_low_bits(2))
    return false;

  if (!value.any_low_bits(64))
    return false;

  return true;
}

static_assert(bit_access_tests());

constexpr bool right_shift_tests() {
  /*
   * (2^64 + 3) >> 1
   *   == 2^63 + 1
   */
  {
    big_uint<4> value(1);

    if (!value.shift_left(64))
      return false;

    if (!value.add(3))
      return false;

    value.shift_right(1);

    if (value.limb_count() != 1)
      return false;

    if (value.limb(0) != 0x8000000000000001ull)
      return false;
  }

  {
    big_uint<4> value(1);

    if (!value.shift_left(130))
      return false;

    if (!value.add(7))
      return false;

    value.shift_right(64);

    if (value.limb_count() != 2)
      return false;

    if (value.limb(0) != 0)
      return false;

    if (value.limb(1) != 4)
      return false;
  }

  {
    big_uint<4> value(123);

    value.shift_right(1000);

    if (!value.is_zero())
      return false;
  }

  return true;
}

static_assert(right_shift_tests());

bool ftl_test() { return true; }
