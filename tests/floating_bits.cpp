#ifdef FTL_REPLACE_STL
#include <cstdint>
#include <detail/floating_bits>
#include <limits>
namespace tested = std;
#else
#include <ftl/cstdint>
#include <ftl/detail/floating_bits>
#include <ftl/limits>
namespace tested = ftl;
#endif

using ftl_float_conversion::binary_parts;
using ftl_float_conversion::compose;
using ftl_float_conversion::decompose;
using ftl_float_conversion::floating_format_traits;
using ftl_float_conversion::floating_from_bits;
using ftl_float_conversion::floating_kind;
using ftl_float_conversion::floating_to_bits;
using ftl_float_conversion::long_double_format;

static_assert(floating_format_traits<float>::fraction_bits == 23);

static_assert(floating_format_traits<float>::exponent_bits == 8);

static_assert(floating_format_traits<float>::significand_bits == 24);

static_assert(floating_format_traits<float>::exponent_bias == 127);

static_assert(floating_format_traits<float>::minimum_significand_exponent ==
              -149);

static_assert(floating_format_traits<float>::maximum_significand_exponent ==
              104);

static_assert(floating_format_traits<double>::fraction_bits == 52);

static_assert(floating_format_traits<double>::exponent_bits == 11);

static_assert(floating_format_traits<double>::significand_bits == 53);

static_assert(floating_format_traits<double>::exponent_bias == 1023);

static_assert(floating_format_traits<double>::minimum_significand_exponent ==
              -1074);

static_assert(floating_format_traits<double>::maximum_significand_exponent ==
              971);

static_assert(floating_format_traits<long double>::significand_bits ==
              tested::numeric_limits<long double>::digits);

static_assert(
    floating_format_traits<long double>::minimum_significand_exponent ==
    tested::numeric_limits<long double>::min_exponent -
        tested::numeric_limits<long double>::digits);

static_assert(
    floating_format_traits<long double>::maximum_significand_exponent ==
    tested::numeric_limits<long double>::max_exponent -
        tested::numeric_limits<long double>::digits);

constexpr bool float_decomposition_tests() {
  {
    const auto parts = decompose(0.0f);

    if (parts.kind != floating_kind::zero || parts.negative ||
        !parts.significand.is_zero())
      return false;
  }

  {
    constexpr auto bits = tested::uint32_t{0x80000000u};

    const auto parts = decompose(floating_from_bits<float>(bits));

    if (parts.kind != floating_kind::zero || !parts.negative)
      return false;
  }

  {
    const auto parts = decompose(1.5f);

    if (parts.kind != floating_kind::finite)
      return false;

    if (parts.negative)
      return false;

    if (parts.significand.high != 0 || parts.significand.low != 0x00c00000ull)
      return false;

    if (parts.exponent != -23)
      return false;
  }

  /*
   * Smallest positive binary32 subnormal:
   *
   *   1 * 2^-149
   */
  {
    const auto value = floating_from_bits<float>(tested::uint32_t{0x00000001u});

    const auto parts = decompose(value);

    if (parts.kind != floating_kind::finite || parts.significand.low != 1 ||
        parts.exponent != -149)
      return false;
  }

  /*
   * Smallest positive binary32 normal:
   *
   *   0x800000 * 2^-149
   */
  {
    const auto value = floating_from_bits<float>(tested::uint32_t{0x00800000u});

    const auto parts = decompose(value);

    if (parts.kind != floating_kind::finite ||
        parts.significand.low != 0x00800000ull || parts.exponent != -149)
      return false;
  }

  /*
   * Largest finite binary32:
   *
   *   0xffffff * 2^104
   */
  {
    const auto value = floating_from_bits<float>(tested::uint32_t{0x7f7fffffu});

    const auto parts = decompose(value);

    if (parts.kind != floating_kind::finite ||
        parts.significand.low != 0x00ffffffull || parts.exponent != 104)
      return false;
  }

  {
    const auto value = floating_from_bits<float>(tested::uint32_t{0x7f800000u});

    const auto parts = decompose(value);

    if (parts.kind != floating_kind::infinity || parts.negative)
      return false;
  }

  {
    const auto value = floating_from_bits<float>(tested::uint32_t{0xff800000u});

    const auto parts = decompose(value);

    if (parts.kind != floating_kind::infinity || !parts.negative)
      return false;
  }

  {
    constexpr tested::uint32_t bits = 0x7fc12345u;

    const auto parts = decompose(floating_from_bits<float>(bits));

    if (parts.kind != floating_kind::nan || parts.negative)
      return false;

    if (parts.significand.low != (bits & 0x007fffffu))
      return false;
  }

  return true;
}

static_assert(float_decomposition_tests());

constexpr bool double_decomposition_tests() {
  {
    const auto parts = decompose(1.5);

    if (parts.kind != floating_kind::finite)
      return false;

    if (parts.significand.high != 0 ||
        parts.significand.low != 0x0018000000000000ull)
      return false;

    if (parts.exponent != -52)
      return false;
  }

  /*
   * Smallest positive binary64 subnormal:
   *
   *   1 * 2^-1074
   */
  {
    const auto value = floating_from_bits<double>(tested::uint64_t{1});

    const auto parts = decompose(value);

    if (parts.kind != floating_kind::finite || parts.significand.low != 1 ||
        parts.exponent != -1074)
      return false;
  }

  /*
   * Smallest positive binary64 normal.
   */
  {
    const auto value = floating_from_bits<double>(0x0010000000000000ull);

    const auto parts = decompose(value);

    if (parts.kind != floating_kind::finite ||
        parts.significand.low != 0x0010000000000000ull ||
        parts.exponent != -1074)
      return false;
  }

  /*
   * Largest finite binary64.
   */
  {
    const auto value = floating_from_bits<double>(0x7fefffffffffffffull);

    const auto parts = decompose(value);

    if (parts.kind != floating_kind::finite ||
        parts.significand.low != 0x001fffffffffffffull || parts.exponent != 971)
      return false;
  }

  {
    const auto parts =
        decompose(floating_from_bits<double>(0x7ff0000000000000ull));

    if (parts.kind != floating_kind::infinity)
      return false;
  }

  {
    constexpr tested::uint64_t bits = 0xfff8123456789abcull;

    const auto parts = decompose(floating_from_bits<double>(bits));

    if (parts.kind != floating_kind::nan || !parts.negative)
      return false;

    if (parts.significand.low != (bits & 0x000fffffffffffffull))
      return false;
  }

  return true;
}

static_assert(double_decomposition_tests());

template <class Float>
constexpr bool
round_trip(typename floating_format_traits<Float>::storage_type bits) {
  const Float original = floating_from_bits<Float>(bits);

  const auto parts = decompose(original);

  const Float reconstructed = compose(parts);

  return floating_to_bits(reconstructed) == bits;
}

constexpr bool float_round_trip_tests() {
  return round_trip<float>(0x00000000u) && round_trip<float>(0x80000000u) &&
         round_trip<float>(0x00000001u) && round_trip<float>(0x007fffffu) &&
         round_trip<float>(0x00800000u) && round_trip<float>(0x3f000000u) &&
         round_trip<float>(0x3f800000u) && round_trip<float>(0x3fc00000u) &&
         round_trip<float>(0xbf800000u) && round_trip<float>(0x7f7fffffu) &&
         round_trip<float>(0x7f800000u) && round_trip<float>(0xff800000u) &&
         round_trip<float>(0x7fc12345u) && round_trip<float>(0xffc12345u);
}

static_assert(float_round_trip_tests());

constexpr bool double_round_trip_tests() {
  return round_trip<double>(0x0000000000000000ull) &&
         round_trip<double>(0x8000000000000000ull) &&
         round_trip<double>(0x0000000000000001ull) &&
         round_trip<double>(0x000fffffffffffffull) &&
         round_trip<double>(0x0010000000000000ull) &&
         round_trip<double>(0x3fe0000000000000ull) &&
         round_trip<double>(0x3ff0000000000000ull) &&
         round_trip<double>(0x3ff8000000000000ull) &&
         round_trip<double>(0xbff0000000000000ull) &&
         round_trip<double>(0x7fefffffffffffffull) &&
         round_trip<double>(0x7ff0000000000000ull) &&
         round_trip<double>(0xfff0000000000000ull) &&
         round_trip<double>(0x7ff8123456789abcull) &&
         round_trip<double>(0xfff8123456789abcull);
}

static_assert(double_round_trip_tests());

constexpr bool manual_composition_tests() {
  {
    binary_parts<float> parts{
        floating_kind::finite, false, {0x00c00000ull, 0}, -23};

    if (floating_to_bits(compose(parts)) != 0x3fc00000u)
      return false;
  }

  {
    binary_parts<float> parts{floating_kind::finite, false, {1, 0}, -149};

    if (floating_to_bits(compose(parts)) != 0x00000001u)
      return false;
  }

  {
    binary_parts<double> parts{
        floating_kind::finite, true, {0x0018000000000000ull, 0}, -52};

    if (floating_to_bits(compose(parts)) != 0xbff8000000000000ull)
      return false;
  }

  return true;
}

static_assert(manual_composition_tests());

bool long_double_round_trip(long double value) {
  const auto original = decompose(value);
  const auto reconstructed = decompose(compose(original));

  return reconstructed.kind == original.kind &&
         reconstructed.negative == original.negative &&
         reconstructed.significand == original.significand &&
         reconstructed.exponent == original.exponent;
}

bool long_double_tests() {
  using format = floating_format_traits<long double>;

  {
    const auto parts = decompose(0.0L);

    if (parts.kind != floating_kind::zero || parts.negative)
      return false;
  }

  {
    const auto parts = decompose(-0.0L);

    if (parts.kind != floating_kind::zero || !parts.negative)
      return false;
  }

  {
    const auto parts = decompose(1.5L);

    if (parts.kind != floating_kind::finite || parts.negative)
      return false;

    if constexpr (format::format == long_double_format::binary64) {
      if (parts.significand.low != 0x0018000000000000ull ||
          parts.significand.high != 0 || parts.exponent != -52)
        return false;
    } else if constexpr (format::format == long_double_format::x87_extended80) {
      if (parts.significand.low != 0xc000000000000000ull ||
          parts.significand.high != 0 || parts.exponent != -63)
        return false;
    } else {
      if (parts.significand.low != 0 ||
          parts.significand.high != 0x0001800000000000ull ||
          parts.exponent != -112)
        return false;
    }
  }

  /*
   * Smallest positive subnormal always has:
   *
   *   significand = 1
   *   exponent    = minimum_significand_exponent
   */
  {
    const auto parts =
        decompose(tested::numeric_limits<long double>::denorm_min());

    if (parts.kind != floating_kind::finite || parts.significand.low != 1 ||
        parts.significand.high != 0 ||
        parts.exponent != format::minimum_significand_exponent)
      return false;
  }

  /*
   * Smallest normal value has exactly its
   * canonical leading bit set.
   */
  {
    const auto parts = decompose(tested::numeric_limits<long double>::min());

    if (parts.kind != floating_kind::finite)
      return false;

    if constexpr (format::format == long_double_format::binary64) {
      if (parts.significand.low != 0x0010000000000000ull ||
          parts.significand.high != 0)
        return false;
    } else if constexpr (format::format == long_double_format::x87_extended80) {
      if (parts.significand.low != 0x8000000000000000ull ||
          parts.significand.high != 0)
        return false;
    } else {
      if (parts.significand.low != 0 ||
          parts.significand.high != 0x0001000000000000ull)
        return false;
    }

    if (parts.exponent != format::minimum_significand_exponent)
      return false;
  }

  /*
   * Largest finite value has every canonical
   * significand bit set.
   */
  {
    const auto parts = decompose(tested::numeric_limits<long double>::max());

    if (parts.kind != floating_kind::finite)
      return false;

    if constexpr (format::format == long_double_format::binary64) {
      if (parts.significand.low != 0x001fffffffffffffull ||
          parts.significand.high != 0)
        return false;
    } else if constexpr (format::format == long_double_format::x87_extended80) {
      if (parts.significand.low != static_cast<tested::uint64_t>(-1) ||
          parts.significand.high != 0)
        return false;
    } else {
      if (parts.significand.low != static_cast<tested::uint64_t>(-1) ||
          parts.significand.high != 0x0001ffffffffffffull)
        return false;
    }

    if (parts.exponent != format::maximum_significand_exponent)
      return false;
  }

  {
    const auto parts =
        decompose(tested::numeric_limits<long double>::infinity());

    if (parts.kind != floating_kind::infinity)
      return false;
  }

  {
    const auto parts =
        decompose(tested::numeric_limits<long double>::quiet_NaN());

    if (parts.kind != floating_kind::nan)
      return false;
  }

  if (!long_double_round_trip(0.0L))
    return false;

  if (!long_double_round_trip(-0.0L))
    return false;

  if (!long_double_round_trip(1.5L))
    return false;

  if (!long_double_round_trip(-1.5L))
    return false;

  if (!long_double_round_trip(
          tested::numeric_limits<long double>::denorm_min()))
    return false;

  if (!long_double_round_trip(tested::numeric_limits<long double>::min()))
    return false;

  if (!long_double_round_trip(tested::numeric_limits<long double>::max()))
    return false;

  if (!long_double_round_trip(tested::numeric_limits<long double>::infinity()))
    return false;

  if (!long_double_round_trip(tested::numeric_limits<long double>::quiet_NaN()))
    return false;

  return true;
}

bool ftl_test() { return long_double_tests(); }
