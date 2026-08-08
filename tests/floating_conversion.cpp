#ifdef FTL_REPLACE_STL
#include <cstdint>
#include <detail/big_uint>
#include <detail/floating_bits>
#include <detail/floating_conversion>
#include <limits>
namespace tested = std;
#else
#include <ftl/cstdint>
#include <ftl/detail/big_uint>
#include <ftl/detail/floating_bits>
#include <ftl/detail/floating_conversion>
#include <ftl/limits>
namespace tested = ftl;
#endif

using ftl_float_conversion::big_uint;
using ftl_float_conversion::conversion_status;
using ftl_float_conversion::decimal_exponent_mode;
using ftl_float_conversion::decimal_number;
using ftl_float_conversion::decimal_scan_status;
using ftl_float_conversion::decimal_to_binary;
using ftl_float_conversion::floating_format_status;
using ftl_float_conversion::floating_special_scan_status;
using ftl_float_conversion::floating_to_bits;
using ftl_float_conversion::format_fixed_precision;
using ftl_float_conversion::format_general_precision;
using ftl_float_conversion::format_hexadecimal_precision;
using ftl_float_conversion::format_scientific_precision;
using ftl_float_conversion::hexadecimal_number;
using ftl_float_conversion::hexadecimal_scan_status;
using ftl_float_conversion::hexadecimal_to_binary;
using ftl_float_conversion::scan_decimal;
using ftl_float_conversion::scan_floating_special;
using ftl_float_conversion::scan_hexadecimal;

template <class Float, decltype(sizeof(0)) Limbs>
constexpr bool converts_to(
    const decimal_number<Limbs> &number,
    typename ftl_float_conversion::floating_format_traits<Float>::storage_type
        expected) {
  const auto result = decimal_to_binary<Float>(number);

  return result.status == conversion_status::success &&
         floating_to_bits(result.value) == expected;
}

constexpr bool hexadecimal_significand_is(const hexadecimal_number &number,
                                          tested::uint64_t low,
                                          tested::uint64_t high = 0) {
  return number.significand.low == low && number.significand.high == high;
}

constexpr bool text_equals(const char *first, const char *last,
                           const char *expected) {
  while (first != last && *expected != '\0') {
    if (*first++ != *expected++)
      return false;
  }

  return first == last && *expected == '\0';
}

constexpr bool basic_float_tests() {
  if (!converts_to<float>(decimal_number<32>{1, 0}, 0x3f800000u))
    return false;

  if (!converts_to<float>(decimal_number<32>{15, -1}, 0x3fc00000u))
    return false;

  if (!converts_to<float>(decimal_number<32>{25, -1}, 0x40200000u))
    return false;

  if (!converts_to<float>(decimal_number<32>{15, -1, true}, 0xbfc00000u))
    return false;

  /*
   * 0.1 is not exactly representable.
   */
  if (!converts_to<float>(decimal_number<32>{1, -1}, 0x3dcccccdu))
    return false;

  return true;
}

static_assert(basic_float_tests());

constexpr bool basic_double_tests() {
  if (!converts_to<double>(decimal_number<32>{1, 0}, 0x3ff0000000000000ull))
    return false;

  if (!converts_to<double>(decimal_number<32>{15, -1}, 0x3ff8000000000000ull))
    return false;

  if (!converts_to<double>(decimal_number<32>{1, -1}, 0x3fb999999999999aull))
    return false;

  if (!converts_to<double>(decimal_number<32>{1, -1, true},
                           0xbfb999999999999aull))
    return false;

  return true;
}

static_assert(basic_double_tests());

constexpr bool signed_zero_tests() {
  {
    decimal_number<32> number{0, 0, false};

    const auto result = decimal_to_binary<float>(number);

    if (result.status != conversion_status::success)
      return false;

    if (floating_to_bits(result.value) != 0x00000000u)
      return false;
  }

  {
    decimal_number<32> number{0, 0, true};

    const auto result = decimal_to_binary<float>(number);

    if (result.status != conversion_status::success)
      return false;

    if (floating_to_bits(result.value) != 0x80000000u)
      return false;
  }

  return true;
}

static_assert(signed_zero_tests());

constexpr bool float_boundary_tests() {
  /*
   * Smallest normal float:
   *
   *   2^-126
   *
   * exactly:
   *
   *   5^126 * 10^-126
   */
  {
    big_uint<32> significand(1);

    if (!significand.multiply_pow5(126))
      return false;

    decimal_number<32> number{significand, -126};

    if (!converts_to<float>(number, 0x00800000u))
      return false;
  }

  /*
   * Exactly half of the minimum subnormal:
   *
   *   2^-150
   *
   * This is the midpoint between +0 and the
   * minimum subnormal. Ties-to-even chooses zero.
   */
  {
    big_uint<32> significand(1);

    if (!significand.multiply_pow5(150))
      return false;

    decimal_number<32> number{significand, -150};

    const auto result = decimal_to_binary<float>(number);

    if (result.status != conversion_status::underflow)
      return false;
  }

  /*
   * 8e-46 is above the zero/min-subnormal
   * midpoint and therefore rounds to the minimum
   * positive subnormal.
   */
  {
    decimal_number<32> number{8, -46};

    if (!converts_to<float>(number, 0x00000001u))
      return false;
  }

  /*
   * 7e-46 is below that midpoint.
   */
  {
    decimal_number<32> number{7, -46};

    const auto result = decimal_to_binary<float>(number);

    if (result.status != conversion_status::underflow)
      return false;
  }

  /*
   * Largest finite float is exactly the integer:
   *
   *   (2^24 - 1) * 2^104
   */
  {
    big_uint<32> significand(0x00ffffffu);

    if (!significand.shift_left(104))
      return false;

    decimal_number<32> number{significand, 0};

    if (!converts_to<float>(number, 0x7f7fffffu))
      return false;
  }

  return true;
}

static_assert(float_boundary_tests());

constexpr bool float_halfway_tests() {
  /*
   * Midpoint between:
   *
   *   0x3f800000  == 1.0
   *   0x3f800001
   *
   * is:
   *
   *   (2^24 + 1) * 2^-24
   *
   * or exactly:
   *
   *   (2^24 + 1) * 5^24 * 10^-24
   *
   * The lower significand is even, so the tie
   * rounds to 1.0.
   */
  {
    big_uint<32> significand((tested::uint64_t{1} << 24) + 1);

    if (!significand.multiply_pow5(24))
      return false;

    decimal_number<32> number{significand, -24};

    if (!converts_to<float>(number, 0x3f800000u))
      return false;
  }

  /*
   * Midpoint between:
   *
   *   0x3f800001
   *   0x3f800002
   *
   * The lower significand is odd, so
   * ties-to-even chooses the upper value.
   */
  {
    big_uint<32> significand((tested::uint64_t{1} << 24) + 3);

    if (!significand.multiply_pow5(24))
      return false;

    decimal_number<32> number{significand, -24};

    if (!converts_to<float>(number, 0x3f800002u))
      return false;
  }

  return true;
}

static_assert(float_halfway_tests());

constexpr bool double_halfway_tests() {
  /*
   * Midpoint between 1.0 and its next binary64
   * neighbor.
   */
  {
    big_uint<32> significand((tested::uint64_t{1} << 53) + 1);

    if (!significand.multiply_pow5(53))
      return false;

    decimal_number<32> number{significand, -53};

    if (!converts_to<double>(number, 0x3ff0000000000000ull))
      return false;
  }

  /*
   * The corresponding midpoint where the lower
   * significand is odd.
   */
  {
    big_uint<32> significand((tested::uint64_t{1} << 53) + 3);

    if (!significand.multiply_pow5(53))
      return false;

    decimal_number<32> number{significand, -53};

    if (!converts_to<double>(number, 0x3ff0000000000002ull))
      return false;
  }

  return true;
}

static_assert(double_halfway_tests());

constexpr bool double_boundary_tests() {
  /*
   * Largest finite binary64:
   *
   *   (2^53 - 1) * 2^971
   */
  {
    big_uint<32> significand((tested::uint64_t{1} << 53) - 1);

    if (!significand.shift_left(971))
      return false;

    decimal_number<32> number{significand, 0};

    if (!converts_to<double>(number, 0x7fefffffffffffffull))
      return false;
  }

  /*
   * 1e308 is finite.
   */
  {
    decimal_number<32> number{1, 308};

    if (!converts_to<double>(number, 0x7fe1ccf385ebc8a0ull))
      return false;
  }

  /*
   * 1e309 is unambiguously outside binary64.
   */
  {
    decimal_number<32> number{1, 309};

    const auto result = decimal_to_binary<double>(number);

    if (result.status != conversion_status::overflow)
      return false;
  }

  /*
   * 1e-400 is unambiguously below binary64's
   * representable nonzero range.
   */
  {
    decimal_number<32> number{1, -400};

    const auto result = decimal_to_binary<double>(number);

    if (result.status != conversion_status::underflow)
      return false;
  }

  return true;
}

static_assert(double_boundary_tests());

constexpr bool decimal_scanner_basic_tests() {
  {
    constexpr char text[] = "1.5xyz";

    const auto result = scan_decimal<float>(text, text + 6);

    if (result.status != decimal_scan_status::success)
      return false;

    if (result.ptr != text + 3)
      return false;

    if (result.number.negative)
      return false;

    if (result.number.significand.limb(0) != 15)
      return false;

    if (result.number.exponent != -1)
      return false;

    if (result.number.discarded_nonzero)
      return false;
  }

  {
    constexpr char text[] = "-0.125e+2x";

    const auto result = scan_decimal<double>(text, text + 10);

    if (result.status != decimal_scan_status::success)
      return false;

    if (result.ptr != text + 9)
      return false;

    if (!result.number.negative)
      return false;

    if (result.number.significand.limb(0) != 125)
      return false;

    if (result.number.exponent != -1)
      return false;
  }

  {
    constexpr char text[] = ".5";

    const auto result = scan_decimal<float>(text, text + 2);

    if (result.status != decimal_scan_status::success ||
        result.ptr != text + 2 || result.number.significand.limb(0) != 5 ||
        result.number.exponent != -1)
      return false;
  }

  {
    constexpr char text[] = "1.";

    const auto result = scan_decimal<float>(text, text + 2);

    if (result.status != decimal_scan_status::success ||
        result.ptr != text + 2 || result.number.significand.limb(0) != 1 ||
        result.number.exponent != 0)
      return false;
  }

  return true;
}

static_assert(decimal_scanner_basic_tests());

constexpr bool decimal_scanner_rejection_tests() {
  {
    constexpr char text[] = ".";

    const auto result = scan_decimal<float>(text, text + 1);

    if (result.status != decimal_scan_status::no_match || result.ptr != text)
      return false;
  }

  {
    constexpr char text[] = "+1";

    const auto result = scan_decimal<float>(text, text + 2);

    if (result.status != decimal_scan_status::no_match || result.ptr != text)
      return false;
  }

  {
    constexpr char text[] = "-x";

    const auto result = scan_decimal<float>(text, text + 2);

    if (result.status != decimal_scan_status::no_match || result.ptr != text)
      return false;
  }

  {
    constexpr char text[] = " x";

    const auto result = scan_decimal<float>(text, text + 2);

    if (result.status != decimal_scan_status::no_match || result.ptr != text)
      return false;
  }

  return true;
}

static_assert(decimal_scanner_rejection_tests());

constexpr bool decimal_scanner_exponent_tests() {
  /*
   * Optional malformed exponent:
   * only the significand matches.
   */
  {
    constexpr char text[] = "1e+";

    const auto result =
        scan_decimal<double>(text, text + 3, decimal_exponent_mode::optional);

    if (result.status != decimal_scan_status::success ||
        result.ptr != text + 1 || result.number.significand.limb(0) != 1 ||
        result.number.exponent != 0)
      return false;
  }

  /*
   * The same input fails scientific-required
   * syntax entirely.
   */
  {
    constexpr char text[] = "1e+";

    const auto result =
        scan_decimal<double>(text, text + 3, decimal_exponent_mode::required);

    if (result.status != decimal_scan_status::no_match || result.ptr != text)
      return false;
  }

  {
    constexpr char text[] = "1";

    const auto result =
        scan_decimal<double>(text, text + 1, decimal_exponent_mode::required);

    if (result.status != decimal_scan_status::no_match || result.ptr != text)
      return false;
  }

  {
    constexpr char text[] = "1e2x";

    const auto result =
        scan_decimal<double>(text, text + 4, decimal_exponent_mode::required);

    if (result.status != decimal_scan_status::success ||
        result.ptr != text + 3 || result.number.exponent != 2)
      return false;
  }

  /*
   * Fixed-only syntax leaves exponent text
   * untouched.
   */
  {
    constexpr char text[] = "1e2";

    const auto result =
        scan_decimal<double>(text, text + 3, decimal_exponent_mode::forbidden);

    if (result.status != decimal_scan_status::success ||
        result.ptr != text + 1 || result.number.exponent != 0)
      return false;
  }

  return true;
}

static_assert(decimal_scanner_exponent_tests());

constexpr bool decimal_scanner_normalization_tests() {
  {
    constexpr char text[] = "000123.4500";

    const auto result = scan_decimal<double>(text, text + 11);

    if (result.status != decimal_scan_status::success)
      return false;

    if (result.number.significand.limb(0) != 1234500)
      return false;

    if (result.number.exponent != -4)
      return false;
  }

  {
    constexpr char text[] = "0.00123";

    const auto result = scan_decimal<double>(text, text + 7);

    if (result.number.significand.limb(0) != 123)
      return false;

    if (result.number.exponent != -5)
      return false;
  }

  {
    constexpr char text[] = "12.5e-3";

    const auto result = scan_decimal<double>(text, text + 7);

    if (result.number.significand.limb(0) != 125)
      return false;

    if (result.number.exponent != -4)
      return false;
  }

  return true;
}

static_assert(decimal_scanner_normalization_tests());

constexpr bool decimal_scanner_retention_tests() {
  constexpr auto retained =
      ftl_float_conversion::floating_conversion_detail::decimal_scan_traits<
          double>::max_significant_digits;

  /*
   * Two integer digits beyond the retention
   * boundary increase the normalized decimal
   * exponent by two.
   */
  {
    char text[retained + 2]{};

    for (decltype(sizeof(0)) index = 0; index < retained + 2; ++index) {
      text[index] = '0';
    }

    text[0] = '1';
    text[retained + 1] = '1';

    const auto result = scan_decimal<double>(text, text + retained + 2);

    if (result.status != decimal_scan_status::success)
      return false;

    if (result.ptr != text + retained + 2)
      return false;

    if (result.number.exponent != 2)
      return false;

    if (!result.number.discarded_nonzero)
      return false;
  }

  /*
   * Discarded zeroes do not set the sticky bit.
   */
  {
    char text[retained + 2]{};

    for (decltype(sizeof(0)) index = 0; index < retained + 2; ++index) {
      text[index] = '0';
    }

    text[0] = '1';

    const auto result = scan_decimal<double>(text, text + retained + 2);

    if (result.number.exponent != 2)
      return false;

    if (result.number.discarded_nonzero)
      return false;
  }

  return true;
}

static_assert(decimal_scanner_retention_tests());

constexpr bool sticky_rounding_tests() {
  /*
   * Start exactly at the midpoint between 1.0f
   * and its next representable neighbor.
   */
  big_uint<32> midpoint((tested::uint64_t{1} << 24) + 1);

  if (!midpoint.multiply_pow5(24))
    return false;

  /*
   * Exact midpoint -> ties-to-even -> 1.0f.
   */
  {
    decimal_number<32> number{midpoint, -24};

    if (!converts_to<float>(number, 0x3f800000u))
      return false;
  }

  /*
   * Same retained midpoint plus some later
   * nonzero decimal tail -> strictly above the
   * midpoint -> next float.
   */
  {
    decimal_number<32> number{midpoint, -24, false, true};

    if (!converts_to<float>(number, 0x3f800001u))
      return false;
  }

  /*
   * Rounding magnitude first and applying the
   * sign afterward gives the symmetric negative
   * result.
   */
  {
    decimal_number<32> number{midpoint, -24, true, true};

    if (!converts_to<float>(number, 0xbf800001u))
      return false;
  }

  return true;
}

static_assert(sticky_rounding_tests());

constexpr bool scanner_conversion_integration_tests() {
  {
    constexpr char text[] = "0.1";

    const auto scanned = scan_decimal<float>(text, text + 3);

    if (scanned.status != decimal_scan_status::success)
      return false;

    const auto converted = decimal_to_binary<float>(scanned.number);

    if (converted.status != conversion_status::success)
      return false;

    if (floating_to_bits(converted.value) != 0x3dcccccdu)
      return false;
  }

  {
    constexpr char text[] = "-1.5e2";

    const auto scanned = scan_decimal<double>(text, text + 6);

    if (scanned.status != decimal_scan_status::success)
      return false;

    const auto converted = decimal_to_binary<double>(scanned.number);

    if (converted.status != conversion_status::success)
      return false;

    if (floating_to_bits(converted.value) != 0xc062c00000000000ull)
      return false;
  }

  return true;
}

constexpr bool hexadecimal_scanner_tests() {
  {
    constexpr char text[] = "1.8p+1x";

    const auto result = scan_hexadecimal<double>(text, text + 7);

    if (result.status != hexadecimal_scan_status::success)
      return false;

    if (result.ptr != text + 6)
      return false;

    if (!hexadecimal_significand_is(result.number, 0x18))
      return false;

    if (result.number.exponent != -3)
      return false;

    if (result.number.discarded_nonzero)
      return false;
  }

  {
    constexpr char text[] = ".8p+1";

    const auto result = scan_hexadecimal<float>(text, text + 5);

    if (result.status != hexadecimal_scan_status::success ||
        result.ptr != text + 5 ||
        !hexadecimal_significand_is(result.number, 8) ||
        result.number.exponent != -3)
      return false;
  }

  /*
   * No explicit 0x prefix is consumed.
   */
  {
    constexpr char text[] = "0x123";

    const auto result = scan_hexadecimal<double>(text, text + 5);

    if (result.status != hexadecimal_scan_status::success ||
        result.ptr != text + 1 || !result.number.significand.is_zero())
      return false;
  }

  /*
   * Optional malformed p exponent is excluded
   * from the subject sequence.
   */
  {
    constexpr char text[] = "1p+";

    const auto result = scan_hexadecimal<double>(text, text + 3);

    if (result.status != hexadecimal_scan_status::success ||
        result.ptr != text + 1 ||
        !hexadecimal_significand_is(result.number, 1) ||
        result.number.exponent != 0)
      return false;
  }

  {
    constexpr char text[] = "0.001p0";

    const auto result = scan_hexadecimal<double>(text, text + 7);

    if (!hexadecimal_significand_is(result.number, 1) ||
        result.number.exponent != -12)
      return false;
  }

  {
    constexpr char text[] = "+1";

    const auto result = scan_hexadecimal<double>(text, text + 2);

    if (result.status != hexadecimal_scan_status::no_match ||
        result.ptr != text)
      return false;
  }

  {
    constexpr char text[] = ".";

    const auto result = scan_hexadecimal<double>(text, text + 1);

    if (result.status != hexadecimal_scan_status::no_match ||
        result.ptr != text)
      return false;
  }

  return true;
}

static_assert(hexadecimal_scanner_tests());

constexpr bool hexadecimal_retention_tests() {
  /*
   * 17 significant hex digits now fit exactly
   * across the two uint128 limbs.
   *
   *   0x1'0000000000000001
   */
  {
    constexpr char text[] = "10000000000000001";

    const auto result = scan_hexadecimal<double>(text, text + 17);

    if (result.status != hexadecimal_scan_status::success)
      return false;

    if (!hexadecimal_significand_is(result.number, 0x0000000000000001ull,
                                    0x0000000000000001ull))
      return false;

    if (result.number.exponent != 0)
      return false;

    if (result.number.discarded_nonzero)
      return false;
  }

  /*
   * 33 significant digits exceed the 128-bit
   * retention window by one nibble.
   *
   * Retained:
   *
   *   0x1000000000000000'
   *     0000000000000000
   *
   * and the final discarded 1 makes the sticky
   * bit true. The normalized exponent gains
   * four bits.
   */
  {
    constexpr char text[] = "100000000000000000000000000000001";

    const auto result = scan_hexadecimal<long double>(text, text + 33);

    if (result.status != hexadecimal_scan_status::success)
      return false;

    if (!hexadecimal_significand_is(result.number, 0, 0x1000000000000000ull))
      return false;

    if (result.number.exponent != 4)
      return false;

    if (!result.number.discarded_nonzero)
      return false;
  }

  /*
   * A discarded zero does not set sticky.
   */
  {
    constexpr char text[] = "100000000000000000000000000000000";

    const auto result = scan_hexadecimal<long double>(text, text + 33);

    if (result.number.exponent != 4)
      return false;

    if (result.number.discarded_nonzero)
      return false;
  }

  return true;
}

static_assert(hexadecimal_retention_tests());

static_assert(hexadecimal_retention_tests());

constexpr bool hexadecimal_conversion_tests() {
  {
    constexpr char text[] = "1.8p+1";

    const auto scanned = scan_hexadecimal<double>(text, text + 6);

    const auto converted = hexadecimal_to_binary<double>(scanned.number);

    if (converted.status != conversion_status::success)
      return false;

    if (floating_to_bits(converted.value) != 0x4008000000000000ull)
      return false;
  }

  /*
   * Minimum positive binary32 subnormal.
   */
  {
    constexpr char text[] = "1p-149";

    const auto scanned = scan_hexadecimal<float>(text, text + 6);

    const auto converted = hexadecimal_to_binary<float>(scanned.number);

    if (converted.status != conversion_status::success ||
        floating_to_bits(converted.value) != 0x00000001u)
      return false;
  }

  /*
   * Exactly halfway between zero and the
   * minimum binary32 subnormal.
   */
  {
    constexpr char text[] = "1p-150";

    const auto scanned = scan_hexadecimal<float>(text, text + 6);

    const auto converted = hexadecimal_to_binary<float>(scanned.number);

    if (converted.status != conversion_status::underflow)
      return false;
  }

  /*
   * 1 + 2^-24 is exactly halfway between 1.0f
   * and its next neighbor. Lower result is even.
   */
  {
    constexpr char text[] = "1.000001p0";

    const auto scanned = scan_hexadecimal<float>(text, text + 11);

    const auto converted = hexadecimal_to_binary<float>(scanned.number);

    if (converted.status != conversion_status::success ||
        floating_to_bits(converted.value) != 0x3f800000u)
      return false;
  }

  /*
   * Midpoint with an odd lower significand:
   * ties-to-even chooses the upper value.
   */
  {
    constexpr char text[] = "1.000003p0";

    const auto scanned = scan_hexadecimal<float>(text, text + 11);

    const auto converted = hexadecimal_to_binary<float>(scanned.number);

    if (converted.status != conversion_status::success ||
        floating_to_bits(converted.value) != 0x3f800002u)
      return false;
  }

  /*
   * Retain an exact binary64 midpoint in the
   * first 32 significant hex digits, then put a
   * nonzero 33rd digit beyond the retention
   * boundary.
   *
   * The retained value is exactly:
   *
   *   1 + 2^-53
   *
   * and the discarded tail places the complete
   * value strictly above the midpoint.
   */
  {
    constexpr char text[] = "1.00000000000008000000000000000001p0";

    const auto scanned = scan_hexadecimal<double>(text, text + 36);

    if (!scanned.number.discarded_nonzero)
      return false;

    const auto converted = hexadecimal_to_binary<double>(scanned.number);

    if (converted.status != conversion_status::success ||
        floating_to_bits(converted.value) != 0x3ff0000000000001ull)
      return false;
  }

  return true;
}

static_assert(hexadecimal_conversion_tests());

static_assert(scanner_conversion_integration_tests());

constexpr bool floating_special_scanner_tests() {
  {
    constexpr char text[] = "INFtail";

    const auto result = scan_floating_special<float>(text, text + 7);

    if (result.status != floating_special_scan_status::success ||
        result.ptr != text + 3)
      return false;

    const auto parts = ftl_float_conversion::decompose(result.value);

    if (parts.kind != ftl_float_conversion::floating_kind::infinity ||
        parts.negative)
      return false;
  }

  {
    constexpr char text[] = "-InFiNiTy!";

    const auto result = scan_floating_special<double>(text, text + 10);

    if (result.status != floating_special_scan_status::success ||
        result.ptr != text + 9)
      return false;

    const auto parts = ftl_float_conversion::decompose(result.value);

    if (parts.kind != ftl_float_conversion::floating_kind::infinity ||
        !parts.negative)
      return false;
  }

  /*
   * "infinite" does not contain the complete
   * "infinity" spelling, but "inf" is itself a
   * valid subject sequence.
   */
  {
    constexpr char text[] = "infinite";

    const auto result = scan_floating_special<double>(text, text + 8);

    if (result.status != floating_special_scan_status::success ||
        result.ptr != text + 3)
      return false;
  }

  {
    constexpr char text[] = "NaNtail";

    const auto result = scan_floating_special<float>(text, text + 7);

    if (result.status != floating_special_scan_status::success ||
        result.ptr != text + 3)
      return false;

    const auto parts = ftl_float_conversion::decompose(result.value);

    if (parts.kind != ftl_float_conversion::floating_kind::nan)
      return false;
  }

  {
    constexpr char text[] = "-NAN(payload_123)x";

    const auto result = scan_floating_special<double>(text, text + 18);

    if (result.status != floating_special_scan_status::success ||
        result.ptr != text + 17)
      return false;

    const auto parts = ftl_float_conversion::decompose(result.value);

    if (parts.kind != ftl_float_conversion::floating_kind::nan ||
        !parts.negative)
      return false;
  }

  /*
   * Empty n-char-sequence is permitted.
   */
  {
    constexpr char text[] = "nan()x";

    const auto result = scan_floating_special<double>(text, text + 6);

    if (result.status != floating_special_scan_status::success ||
        result.ptr != text + 5)
      return false;
  }

  /*
   * Incomplete payload falls back to the shorter
   * valid "nan" spelling.
   */
  {
    constexpr char text[] = "nan(payload";

    const auto result = scan_floating_special<double>(text, text + 11);

    if (result.status != floating_special_scan_status::success ||
        result.ptr != text + 3)
      return false;
  }

  /*
   * Invalid payload characters likewise prevent
   * the optional suffix from participating.
   */
  {
    constexpr char text[] = "nan(foo-bar)";

    const auto result = scan_floating_special<double>(text, text + 12);

    if (result.status != floating_special_scan_status::success ||
        result.ptr != text + 3)
      return false;
  }

  {
    constexpr char text[] = "+inf";

    const auto result = scan_floating_special<double>(text, text + 4);

    if (result.status != floating_special_scan_status::no_match ||
        result.ptr != text)
      return false;
  }

  {
    constexpr char text[] = "nope";

    const auto result = scan_floating_special<double>(text, text + 4);

    if (result.status != floating_special_scan_status::no_match ||
        result.ptr != text)
      return false;
  }

  return true;
}

static_assert(floating_special_scanner_tests());

bool long_double_decimal_tests() {
  using format = ftl_float_conversion::floating_format_traits<long double>;

  /*
   * This simple case already forces the quotient
   * machinery past 53 bits on x87 and binary128:
   *
   * x87:
   *   1.5 -> 0xc000000000000000 * 2^-63
   *
   * binary128:
   *   1.5 ->
   *   0x0001800000000000'0000000000000000
   *   * 2^-112
   */
  {
    decimal_number<640> number{15, -1};

    const auto converted = decimal_to_binary<long double>(number);

    if (converted.status != conversion_status::success)
      return false;

    if (converted.value != 1.5L)
      return false;

    const auto parts = ftl_float_conversion::decompose(converted.value);

    if (parts.exponent != -format::fraction_bits)
      return false;

    if constexpr (format::format ==
                  ftl_float_conversion::long_double_format::binary64) {
      if (parts.significand.low != 0x0018000000000000ull ||
          parts.significand.high != 0)
        return false;
    } else if constexpr (format::format ==
                         ftl_float_conversion::long_double_format::
                             x87_extended80) {
      if (parts.significand.low != 0xc000000000000000ull ||
          parts.significand.high != 0)
        return false;
    } else {
      if (parts.significand.low != 0 ||
          parts.significand.high != 0x0001800000000000ull)
        return false;
    }
  }

  /*
   * Exercise the long-double scanner and a
   * non-exact decimal conversion.
   */
  {
    constexpr char text[] = "0.1";

    const auto scanned = scan_decimal<long double>(text, text + 3);

    if (scanned.status != decimal_scan_status::success)
      return false;

    const auto converted = decimal_to_binary<long double>(scanned.number);

    if (converted.status != conversion_status::success)
      return false;

    if (converted.value != 0.1L)
      return false;
  }

  /*
   * Exact smallest normal long double:
   *
   *   2^(min_exponent - 1)
   *
   * represented exactly as:
   *
   *   5^n * 10^-n
   *
   * where:
   *
   *   n = 1 - min_exponent
   */
  {
    constexpr int power = 1 - tested::numeric_limits<long double>::min_exponent;

    big_uint<640> significand(1);

    if (!significand.multiply_pow5(static_cast<unsigned>(power)))
      return false;

    decimal_number<640> number{significand, -power};

    const auto converted = decimal_to_binary<long double>(number);

    if (converted.status != conversion_status::success)
      return false;

    if (converted.value != tested::numeric_limits<long double>::min())
      return false;
  }

  /*
   * Exact smallest positive subnormal.
   *
   * This is the nastiest exponent-side case:
   * binary128 reaches 2^-16494.
   */
  {
    constexpr int power = -format::minimum_significand_exponent;

    big_uint<640> significand(1);

    if (!significand.multiply_pow5(static_cast<unsigned>(power)))
      return false;

    decimal_number<640> number{significand, -power};

    const auto converted = decimal_to_binary<long double>(number);

    if (converted.status != conversion_status::success)
      return false;

    if (converted.value != tested::numeric_limits<long double>::denorm_min())
      return false;
  }

  return true;
}

bool long_double_hexadecimal_tests() {
  using format = ftl_float_conversion::floating_format_traits<long double>;

  /*
   * Scanner + conversion.
   */
  {
    constexpr char text[] = "1.8p+1";

    const auto scanned = scan_hexadecimal<long double>(text, text + 6);

    if (scanned.status != hexadecimal_scan_status::success)
      return false;

    const auto converted = hexadecimal_to_binary<long double>(scanned.number);

    if (converted.status != conversion_status::success ||
        converted.value != 3.0L)
      return false;
  }

  /*
   * Smallest positive long-double subnormal is
   * exactly:
   *
   *   1 * 2^minimum_significand_exponent
   */
  {
    hexadecimal_number number{
        false, {1, 0}, format::minimum_significand_exponent, false};

    const auto converted = hexadecimal_to_binary<long double>(number);

    if (converted.status != conversion_status::success)
      return false;

    if (converted.value != tested::numeric_limits<long double>::denorm_min())
      return false;
  }

  /*
   * Smallest normal value uses exactly the target
   * format's canonical leading significand bit.
   */
  {
    hexadecimal_number number{
        false,
        ftl_float_conversion::floating_conversion_detail::uint128_bit(
            static_cast<unsigned>(format::significand_bits - 1)),
        format::minimum_significand_exponent, false};

    const auto converted = hexadecimal_to_binary<long double>(number);

    if (converted.status != conversion_status::success)
      return false;

    if (converted.value != tested::numeric_limits<long double>::min())
      return false;
  }

  /*
   * Verify the 128-bit scanner really keeps data
   * beyond the old 64-bit boundary.
   */
  {
    constexpr char text[] = "10000000000000001";

    const auto scanned = scan_hexadecimal<long double>(text, text + 17);

    if (!hexadecimal_significand_is(scanned.number, 1, 1))
      return false;
  }

  return true;
}

bool fixed_precision_format_tests() {
  char buffer[256]{};

  {
    const auto result = format_fixed_precision(buffer, buffer + 256, 1.5, 3);

    if (result.status != floating_format_status::success ||
        !text_equals(buffer, result.ptr, "1.500"))
      return false;
  }

  {
    const auto result = format_fixed_precision(buffer, buffer + 256, -1.5f, 2);

    if (result.status != floating_format_status::success ||
        !text_equals(buffer, result.ptr, "-1.50"))
      return false;
  }

  /*
   * Exact halfway case:
   *
   *   2.5 -> 2
   *
   * nearest-even.
   */
  {
    const auto result = format_fixed_precision(buffer, buffer + 256, 2.5, 0);

    if (result.status != floating_format_status::success ||
        !text_equals(buffer, result.ptr, "2"))
      return false;
  }

  /*
   * Odd lower integer means the opposite halfway
   * direction:
   *
   *   3.5 -> 4
   */
  {
    const auto result = format_fixed_precision(buffer, buffer + 256, 3.5, 0);

    if (result.status != floating_format_status::success ||
        !text_equals(buffer, result.ptr, "4"))
      return false;
  }

  /*
   * Fractional halfway cases.
   */
  {
    const auto result = format_fixed_precision(buffer, buffer + 256, 0.125, 2);

    if (!text_equals(buffer, result.ptr, "0.12"))
      return false;
  }

  {
    const auto result = format_fixed_precision(buffer, buffer + 256, 0.375, 2);

    if (!text_equals(buffer, result.ptr, "0.38"))
      return false;
  }

  {
    const auto result = format_fixed_precision(buffer, buffer + 256, -0.125, 2);

    if (!text_equals(buffer, result.ptr, "-0.12"))
      return false;
  }

  /*
   * Negative zero retains its sign.
   */
  {
    const auto result = format_fixed_precision(buffer, buffer + 256, -0.0, 4);

    if (!text_equals(buffer, result.ptr, "-0.0000"))
      return false;
  }

  /*
   * Requested precision beyond the exact binary
   * denominator is emitted as trailing zeroes
   * rather than expanded into the bigint.
   */
  {
    const auto result = format_fixed_precision(buffer, buffer + 256, 1.25, 20);

    if (!text_equals(buffer, result.ptr, "1.25000000000000000000"))
      return false;
  }

  /*
   * Carry across the radix point after rounding.
   */
  {
    const auto result = format_fixed_precision(buffer, buffer + 256, 9.875, 2);

    if (!text_equals(buffer, result.ptr, "9.88"))
      return false;
  }

  /*
   * Long double exercises 64- or 113-bit source
   * significands depending on the target.
   */
  {
    const auto result = format_fixed_precision(buffer, buffer + 256, 1.5L, 8);

    if (!text_equals(buffer, result.ptr, "1.50000000"))
      return false;
  }

  /*
   * Special values use canonical spellings.
   */
  {
    const auto result = format_fixed_precision(
        buffer, buffer + 256, tested::numeric_limits<double>::infinity(), 100);

    if (!text_equals(buffer, result.ptr, "inf"))
      return false;
  }

  {
    const auto result = format_fixed_precision(
        buffer, buffer + 256, -tested::numeric_limits<double>::infinity(), 100);

    if (!text_equals(buffer, result.ptr, "-inf"))
      return false;
  }

  {
    const auto result = format_fixed_precision(
        buffer, buffer + 256, tested::numeric_limits<double>::quiet_NaN(), 5);

    if (result.status != floating_format_status::success)
      return false;

    if (!text_equals(buffer, result.ptr, "nan"))
      return false;
  }

  /*
   * Buffer failure reports last and performs no
   * partial output.
   */
  {
    char tiny[3] = {'x', 'x', 'x'};

    const auto result = format_fixed_precision(tiny, tiny + 3, 1.25, 2);

    if (result.status != floating_format_status::value_too_large ||
        result.ptr != tiny + 3)
      return false;

    if (tiny[0] != 'x' || tiny[1] != 'x' || tiny[2] != 'x')
      return false;
  }

  return true;
}

bool scientific_precision_format_tests() {
  char buffer[512]{};

  {
    const auto result =
        format_scientific_precision(buffer, buffer + 512, 1.5, 3);

    if (result.status != floating_format_status::success ||
        !text_equals(buffer, result.ptr, "1.500e+00"))
      return false;
  }

  {
    const auto result =
        format_scientific_precision(buffer, buffer + 512, -1.5f, 2);

    if (!text_equals(buffer, result.ptr, "-1.50e+00"))
      return false;
  }

  {
    const auto result =
        format_scientific_precision(buffer, buffer + 512, 12345.0, 2);

    if (!text_equals(buffer, result.ptr, "1.23e+04"))
      return false;
  }

  /*
   * Decimal nearest-even rounding.
   *
   * 1.25 with one fractional scientific digit is
   * exactly halfway between 1.2 and 1.3. 1.2 has
   * the even retained digit.
   */
  {
    const auto result =
        format_scientific_precision(buffer, buffer + 512, 1.25, 1);

    if (!text_equals(buffer, result.ptr, "1.2e+00"))
      return false;
  }

  {
    const auto result =
        format_scientific_precision(buffer, buffer + 512, 1.75, 1);

    if (!text_equals(buffer, result.ptr, "1.8e+00"))
      return false;
  }

  /*
   * Carry through every retained decimal digit
   * also increments the scientific exponent.
   *
   * 999.5 -> 1.00e+03 at precision 2.
   */
  {
    const auto result =
        format_scientific_precision(buffer, buffer + 512, 999.5, 2);

    if (!text_equals(buffer, result.ptr, "1.00e+03"))
      return false;
  }

  /*
   * The corresponding even-lower tie stays down.
   */
  {
    const auto result =
        format_scientific_precision(buffer, buffer + 512, 998.5, 2);

    if (!text_equals(buffer, result.ptr, "9.98e+02"))
      return false;
  }

  {
    const auto result =
        format_scientific_precision(buffer, buffer + 512, 0.0015, 3);

    if (!text_equals(buffer, result.ptr, "1.500e-03"))
      return false;
  }

  /*
   * Negative zero keeps its sign and always uses
   * exponent zero.
   */
  {
    const auto result =
        format_scientific_precision(buffer, buffer + 512, -0.0, 2);

    if (!text_equals(buffer, result.ptr, "-0.00e+00"))
      return false;
  }

  /*
   * Precision beyond the exact decimal expansion
   * is padded rather than materialized through
   * giant powers of five.
   */
  {
    const auto result =
        format_scientific_precision(buffer, buffer + 512, 1.25, 20);

    if (!text_equals(buffer, result.ptr, "1.25000000000000000000e+00"))
      return false;
  }

  /*
   * Long double exercises the widened exact
   * representation on x87/binary128 platforms.
   */
  {
    const auto result =
        format_scientific_precision(buffer, buffer + 512, 1.5L, 8);

    if (!text_equals(buffer, result.ptr, "1.50000000e+00"))
      return false;
  }

  /*
   * Largest finite binary64 rounds across the
   * leading decimal digit at precision zero.
   */
  {
    const auto result = format_scientific_precision(
        buffer, buffer + 512, tested::numeric_limits<double>::max(), 0);

    if (!text_equals(buffer, result.ptr, "2e+308"))
      return false;
  }

  {
    const auto result = format_scientific_precision(
        buffer, buffer + 512, tested::numeric_limits<double>::infinity(), 100);

    if (!text_equals(buffer, result.ptr, "inf"))
      return false;
  }

  {
    const auto result = format_scientific_precision(
        buffer, buffer + 512, -tested::numeric_limits<double>::infinity(), 100);

    if (!text_equals(buffer, result.ptr, "-inf"))
      return false;
  }

  {
    const auto result = format_scientific_precision(
        buffer, buffer + 512, tested::numeric_limits<double>::quiet_NaN(), 5);

    if (result.status != floating_format_status::success ||
        !text_equals(buffer, result.ptr, "nan"))
      return false;
  }

  /*
   * No partial output on insufficient capacity.
   */
  {
    char tiny[6] = {'x', 'x', 'x', 'x', 'x', 'x'};

    const auto result = format_scientific_precision(tiny, tiny + 6, 1.5, 2);

    if (result.status != floating_format_status::value_too_large ||
        result.ptr != tiny + 6)
      return false;

    for (char value : tiny) {
      if (value != 'x')
        return false;
    }
  }

  return true;
}

bool general_precision_format_tests() {
  char buffer[512]{};

  /*
   * Ordinary fixed selection.
   */
  {
    const auto result =
        format_general_precision(buffer, buffer + 512, 123.45, 5);

    if (result.status != floating_format_status::success ||
        !text_equals(buffer, result.ptr, "123.45"))
      return false;
  }

  /*
   * Trailing fractional zeroes are removed.
   */
  {
    const auto result = format_general_precision(buffer, buffer + 512, 1.25, 6);

    if (!text_equals(buffer, result.ptr, "1.25"))
      return false;
  }

  {
    const auto result =
        format_general_precision(buffer, buffer + 512, 100.0, 5);

    if (!text_equals(buffer, result.ptr, "100"))
      return false;
  }

  /*
   * exponent == -4 still uses fixed notation.
   */
  {
    const auto result =
        format_general_precision(buffer, buffer + 512, 0.00012345, 3);

    if (!text_equals(buffer, result.ptr, "0.000123"))
      return false;
  }

  /*
   * exponent == -5 crosses into scientific notation.
   */
  {
    const auto result =
        format_general_precision(buffer, buffer + 512, 0.000012345, 3);

    if (!text_equals(buffer, result.ptr, "1.23e-05"))
      return false;
  }

  /*
   * exponent >= precision uses scientific notation.
   */
  {
    const auto result =
        format_general_precision(buffer, buffer + 512, 12345.0, 3);

    if (!text_equals(buffer, result.ptr, "1.23e+04"))
      return false;
  }

  /*
   * exponent == precision - 1 remains fixed.
   */
  {
    const auto result =
        format_general_precision(buffer, buffer + 512, 999.0, 3);

    if (!text_equals(buffer, result.ptr, "999"))
      return false;
  }

  /*
   * Crucial selection-after-rounding case:
   *
   *   999.5 -> 1.00e+03 at three significant digits.
   *
   * The rounded exponent is 3, equal to P, so %g
   * selects scientific notation and strips zeroes.
   */
  {
    const auto result =
        format_general_precision(buffer, buffer + 512, 999.5, 3);

    if (!text_equals(buffer, result.ptr, "1e+03"))
      return false;
  }

  /*
   * Rounding can also move a value across the -4
   * notation boundary.
   */
  {
    const auto result =
        format_general_precision(buffer, buffer + 512, 0.00009995, 3);

    if (!text_equals(buffer, result.ptr, "0.0001"))
      return false;
  }

  /*
   * Nearest-even significant-digit rounding.
   */
  {
    const auto result = format_general_precision(buffer, buffer + 512, 1.25, 2);

    if (!text_equals(buffer, result.ptr, "1.2"))
      return false;
  }

  {
    const auto result = format_general_precision(buffer, buffer + 512, 1.75, 2);

    if (!text_equals(buffer, result.ptr, "1.8"))
      return false;
  }

  /*
   * Precision zero is treated as one significant
   * digit by %g.
   */
  {
    const auto result = format_general_precision(buffer, buffer + 512, 12.5, 0);

    if (!text_equals(buffer, result.ptr, "1e+01"))
      return false;
  }

  /*
   * Zero uses fixed notation and strips the
   * otherwise-requested fractional zeroes.
   */
  {
    const auto result = format_general_precision(buffer, buffer + 512, 0.0, 6);

    if (!text_equals(buffer, result.ptr, "0"))
      return false;
  }

  {
    const auto result = format_general_precision(buffer, buffer + 512, -0.0, 6);

    if (!text_equals(buffer, result.ptr, "-0"))
      return false;
  }

  /*
   * Long double goes through the same widened exact
   * decimal path.
   */
  {
    const auto result =
        format_general_precision(buffer, buffer + 512, 1.5L, 20);

    if (!text_equals(buffer, result.ptr, "1.5"))
      return false;
  }

  {
    const auto result = format_general_precision(
        buffer, buffer + 512, tested::numeric_limits<double>::max(), 6);

    if (!text_equals(buffer, result.ptr, "1.79769e+308"))
      return false;
  }

  {
    const auto result = format_general_precision(
        buffer, buffer + 512, tested::numeric_limits<double>::infinity(), 100);

    if (!text_equals(buffer, result.ptr, "inf"))
      return false;
  }

  {
    const auto result = format_general_precision(
        buffer, buffer + 512, tested::numeric_limits<double>::quiet_NaN(), 100);

    if (result.status != floating_format_status::success ||
        !text_equals(buffer, result.ptr, "nan"))
      return false;
  }

  /*
   * Insufficient capacity remains all-or-nothing.
   */
  {
    char tiny[4] = {'x', 'x', 'x', 'x'};

    const auto result = format_general_precision(tiny, tiny + 4, 12345.0, 3);

    if (result.status != floating_format_status::value_too_large ||
        result.ptr != tiny + 4)
      return false;

    for (char value : tiny) {
      if (value != 'x')
        return false;
    }
  }

  return true;
}

bool hexadecimal_precision_format_tests() {
  char buffer[512]{};

  {
    const auto result =
        format_hexadecimal_precision(buffer, buffer + 512, 1.5, 3);

    if (result.status != floating_format_status::success ||
        !text_equals(buffer, result.ptr, "1.800p+0"))
      return false;
  }

  {
    const auto result =
        format_hexadecimal_precision(buffer, buffer + 512, -1.5f, 2);

    if (!text_equals(buffer, result.ptr, "-1.80p+0"))
      return false;
  }

  {
    const auto result =
        format_hexadecimal_precision(buffer, buffer + 512, 1.25, 1);

    if (!text_equals(buffer, result.ptr, "1.4p+0"))
      return false;
  }

  /*
   * Precision zero performs hexadecimal
   * nearest-even rounding:
   *
   * 1.5 is halfway between 1p+0 and 2p+0.
   * The retained integer 2 is even.
   */
  {
    const auto result =
        format_hexadecimal_precision(buffer, buffer + 512, 1.5, 0);

    if (!text_equals(buffer, result.ptr, "2p+0"))
      return false;
  }

  /*
   * 1.03125 == 0x1.08p+0.
   *
   * At one fractional hex digit this is halfway
   * between 1.0 and 1.1; lower nibble 0 is even.
   */
  {
    const auto result =
        format_hexadecimal_precision(buffer, buffer + 512, 1.03125, 1);

    if (!text_equals(buffer, result.ptr, "1.0p+0"))
      return false;
  }

  /*
   * 1.09375 == 0x1.18p+0.
   *
   * Lower retained nibble 1 is odd, so the tie
   * rounds upward to 2.
   */
  {
    const auto result =
        format_hexadecimal_precision(buffer, buffer + 512, 1.09375, 1);

    if (!text_equals(buffer, result.ptr, "1.2p+0"))
      return false;
  }

  /*
   * Carry through the hexadecimal point. We do not
   * need to renormalize the exponent: 2.0p+0 is a
   * valid %a-style representation.
   */
  {
    const auto result =
        format_hexadecimal_precision(buffer, buffer + 512, 1.96875, 1);

    if (!text_equals(buffer, result.ptr, "2.0p+0"))
      return false;
  }

  /*
   * A familiar non-exact binary64 value.
   *
   * Exact representation starts:
   *
   *   1.999999999999ap-4
   *
   * and rounds to three hex fractional digits as
   * 1.99ap-4.
   */
  {
    const auto result =
        format_hexadecimal_precision(buffer, buffer + 512, 0.1, 3);

    if (!text_equals(buffer, result.ptr, "1.99ap-4"))
      return false;
  }

  /*
   * Exact minimum normal binary64.
   */
  {
    const auto result = format_hexadecimal_precision(
        buffer, buffer + 512, tested::numeric_limits<double>::min(), 13);

    if (!text_equals(buffer, result.ptr, "1.0000000000000p-1022"))
      return false;
  }

  /*
   * Subnormals remain anchored at the minimum normal
   * exponent, matching %a semantics.
   */
  {
    const auto result = format_hexadecimal_precision(
        buffer, buffer + 512, tested::numeric_limits<double>::denorm_min(), 13);

    if (!text_equals(buffer, result.ptr, "0.0000000000001p-1022"))
      return false;
  }

  {
    const auto result = format_hexadecimal_precision(
        buffer, buffer + 512, tested::numeric_limits<float>::denorm_min(), 6);

    if (!text_equals(buffer, result.ptr, "0.000002p-126"))
      return false;
  }

  /*
   * Rounding the largest finite double at precision
   * zero produces 2p+1023. The representation does
   * not need exponent renormalization.
   */
  {
    const auto result = format_hexadecimal_precision(
        buffer, buffer + 512, tested::numeric_limits<double>::max(), 0);

    if (!text_equals(buffer, result.ptr, "2p+1023"))
      return false;
  }

  {
    const auto result =
        format_hexadecimal_precision(buffer, buffer + 512, 0.0, 4);

    if (!text_equals(buffer, result.ptr, "0.0000p+0"))
      return false;
  }

  {
    const auto result =
        format_hexadecimal_precision(buffer, buffer + 512, -0.0, 2);

    if (!text_equals(buffer, result.ptr, "-0.00p+0"))
      return false;
  }

  /*
   * Same representation on binary64, x87 and
   * binary128 because we normalize around the
   * canonical leading binary bit.
   */
  {
    const auto result =
        format_hexadecimal_precision(buffer, buffer + 512, 1.5L, 4);

    if (!text_equals(buffer, result.ptr, "1.8000p+0"))
      return false;
  }

  /*
   * Precision beyond the exact source representation
   * is zero-padded rather than materialized.
   */
  {
    const auto result =
        format_hexadecimal_precision(buffer, buffer + 512, 1.5, 20);

    if (!text_equals(buffer, result.ptr, "1.80000000000000000000p+0"))
      return false;
  }

  {
    const auto result = format_hexadecimal_precision(
        buffer, buffer + 512, tested::numeric_limits<double>::infinity(), 100);

    if (!text_equals(buffer, result.ptr, "inf"))
      return false;
  }

  {
    const auto result = format_hexadecimal_precision(
        buffer, buffer + 512, tested::numeric_limits<double>::quiet_NaN(), 100);

    if (result.status != floating_format_status::success ||
        !text_equals(buffer, result.ptr, "nan"))
      return false;
  }

  /*
   * Buffer failure remains all-or-nothing.
   */
  {
    char tiny[6] = {'x', 'x', 'x', 'x', 'x', 'x'};

    const auto result =
        format_hexadecimal_precision(tiny, tiny + 6, 1.5, 2);

    if (result.status != floating_format_status::value_too_large ||
        result.ptr != tiny + 6)
      return false;

    for (char character : tiny) {
      if (character != 'x')
        return false;
    }
  }

  return true;
}

bool ftl_test() {
  return long_double_decimal_tests() && long_double_hexadecimal_tests() &&
         fixed_precision_format_tests() &&
         scientific_precision_format_tests() &&
         general_precision_format_tests() &&
         hexadecimal_precision_format_tests();
}
