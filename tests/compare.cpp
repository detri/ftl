#ifdef FTL_REPLACE_STL
#include <compare>
namespace tested = std;
#else
#include <ftl/compare>
namespace tested = ftl;
#endif

#if __cpp_lib_three_way_comparison != 201907L
#error invalid <compare> feature-test macro
#endif

#if __cpp_lib_concepts != 202207L
#error invalid concepts feature-test macro from <compare>
#endif

static_assert(tested::compare_three_way{}(2, 1) > 0);
static_assert(tested::strong_ordering::equal == 0);
static_assert(tested::is_eq(tested::strong_ordering::equal));
static_assert(tested::is_neq(tested::partial_ordering::unordered));
static_assert(tested::is_lt(tested::weak_ordering::less));

static_assert(
    tested::is_same_v<tested::common_comparison_category_t<
                          tested::strong_ordering, tested::weak_ordering>,
                      tested::weak_ordering>);

struct comparable {
  int value;

  friend constexpr tested::strong_ordering
  operator<=>(comparable left, comparable right) noexcept {
    if (left.value < right.value)
      return tested::strong_ordering::less;

    if (left.value > right.value)
      return tested::strong_ordering::greater;

    return tested::strong_ordering::equal;
  }

  friend constexpr bool operator==(comparable, comparable) = default;
};

static_assert(
    tested::three_way_comparable<comparable, tested::strong_ordering>);

static_assert(tested::is_same_v<tested::compare_three_way_result_t<comparable>,
                                tested::strong_ordering>);

static_assert(tested::strong_order(comparable{1}, comparable{2}) < 0);

static_assert(tested::strong_order(1, 2) < 0);
static_assert(tested::strong_order(-0.0, 0.0) < 0);
static_assert(tested::strong_order(-0.0L, 0.0L) < 0);

constexpr float quiet_nan_one = __builtin_bit_cast(float, 0x7fc00001u);

constexpr float quiet_nan_two = __builtin_bit_cast(float, 0x7fc00002u);

static_assert(tested::strong_order(quiet_nan_one, quiet_nan_two) < 0);

static_assert(noexcept(tested::strong_order(1, 2)));
static_assert(noexcept(tested::weak_order(1, 2)));
static_assert(noexcept(tested::partial_order(1, 2)));

struct legacy_ordered {
  int value;

  friend constexpr bool operator==(legacy_ordered, legacy_ordered) = default;

  friend constexpr bool operator<(legacy_ordered left, legacy_ordered right) {
    return left.value < right.value;
  }

  friend constexpr bool operator>(legacy_ordered left, legacy_ordered right) {
    return right < left;
  }

  friend constexpr bool operator<=(legacy_ordered left, legacy_ordered right) {
    return !(right < left);
  }

  friend constexpr bool operator>=(legacy_ordered left, legacy_ordered right) {
    return !(left < right);
  }
};

static_assert(tested::compare_strong_order_fallback(legacy_ordered{1},
                                                    legacy_ordered{2}) < 0);

static_assert(noexcept(tested::compare_strong_order_fallback(1, 2)));

template <class T, class U>
concept can_compare_three_way = requires(T &&left, U &&right) {
  tested::compare_three_way{}(static_cast<T &&>(left),
                              static_cast<U &&>(right));
};

static_assert(!can_compare_three_way<legacy_ordered, legacy_ordered>);

struct partial_custom {
  int value;

  friend constexpr tested::partial_ordering
  partial_order(partial_custom left, partial_custom right) noexcept {
    if (left.value < right.value)
      return tested::partial_ordering::less;

    if (left.value > right.value)
      return tested::partial_ordering::greater;

    return tested::partial_ordering::equivalent;
  }
};

static_assert(tested::partial_order(partial_custom{2}, partial_custom{1}) > 0);

struct comparison_common {
  int value;

  friend constexpr tested::strong_ordering
  operator<=>(const comparison_common &left,
              const comparison_common &right) noexcept {
    if (left.value < right.value)
      return tested::strong_ordering::less;

    if (left.value > right.value)
      return tested::strong_ordering::greater;

    return tested::strong_ordering::equal;
  }

  friend constexpr bool operator==(const comparison_common &,
                                   const comparison_common &) = default;
};

struct comparison_left {
  int value;

  constexpr explicit comparison_left(int input) : value(input) {}

  comparison_left(const comparison_left &) = delete;
  comparison_left(comparison_left &&) = default;

  comparison_left &operator=(const comparison_left &) = delete;

  comparison_left &operator=(comparison_left &&) = default;

  constexpr operator comparison_common() && noexcept {
    return comparison_common{value};
  }

  friend constexpr tested::strong_ordering
  operator<=>(const comparison_left &left,
              const comparison_left &right) noexcept {
    if (left.value < right.value)
      return tested::strong_ordering::less;

    if (left.value > right.value)
      return tested::strong_ordering::greater;

    return tested::strong_ordering::equal;
  }

  friend constexpr bool operator==(const comparison_left &,
                                   const comparison_left &) = default;
};

struct comparison_right {
  int value;

  constexpr explicit comparison_right(int input) : value(input) {}

  comparison_right(const comparison_right &) = delete;
  comparison_right(comparison_right &&) = default;

  comparison_right &operator=(const comparison_right &) = delete;

  comparison_right &operator=(comparison_right &&) = default;

  constexpr operator comparison_common() && noexcept {
    return comparison_common{value};
  }

  friend constexpr tested::strong_ordering
  operator<=>(const comparison_right &left,
              const comparison_right &right) noexcept {
    if (left.value < right.value)
      return tested::strong_ordering::less;

    if (left.value > right.value)
      return tested::strong_ordering::greater;

    return tested::strong_ordering::equal;
  }

  friend constexpr bool operator==(const comparison_right &,
                                   const comparison_right &) = default;
};

constexpr tested::strong_ordering
operator<=>(const comparison_left &left,
            const comparison_right &right) noexcept {
  if (left.value < right.value)
    return tested::strong_ordering::less;

  if (left.value > right.value)
    return tested::strong_ordering::greater;

  return tested::strong_ordering::equal;
}

constexpr tested::strong_ordering
operator<=>(const comparison_right &left,
            const comparison_left &right) noexcept {
  if (left.value < right.value)
    return tested::strong_ordering::less;

  if (left.value > right.value)
    return tested::strong_ordering::greater;

  return tested::strong_ordering::equal;
}

constexpr bool operator==(const comparison_left &left,
                          const comparison_right &right) noexcept {
  return left.value == right.value;
}

constexpr bool operator==(const comparison_right &left,
                          const comparison_left &right) noexcept {
  return left.value == right.value;
}

#ifdef FTL_REPLACE_STL
namespace std {
#else
namespace ftl {
#endif

template <template <class> class TQual, template <class> class UQual>
struct basic_common_reference<::comparison_left, ::comparison_right, TQual,
                              UQual> {
  using type = ::comparison_common;
};

template <template <class> class TQual, template <class> class UQual>
struct basic_common_reference<::comparison_right, ::comparison_left, TQual,
                              UQual> {
  using type = ::comparison_common;
};

} // namespace std / ftl

static_assert(!tested::common_reference_with<const comparison_left &,
                                             const comparison_right &>);

static_assert(tested::three_way_comparable_with<
              comparison_left, comparison_right, tested::strong_ordering>);

constexpr float negative_quiet_nan_one = __builtin_bit_cast(float, 0xffc00001u);

static_assert(tested::weak_order(-0.0f, +0.0f) ==
              tested::weak_ordering::equivalent);

static_assert(tested::weak_order(quiet_nan_one, quiet_nan_two) ==
              tested::weak_ordering::equivalent);

static_assert(tested::weak_order(negative_quiet_nan_one, quiet_nan_one) < 0);

static_assert(tested::partial_order(quiet_nan_one, 0.0f) ==
              tested::partial_ordering::unordered);

struct weak_precedence {
  int value;

  friend constexpr bool operator==(weak_precedence, weak_precedence) = default;

  friend constexpr tested::weak_ordering
  operator<=>(weak_precedence left, weak_precedence right) noexcept {
    if (left.value < right.value) {
      return tested::weak_ordering::less;
    }

    if (left.value > right.value) {
      return tested::weak_ordering::greater;
    }

    return tested::weak_ordering::equivalent;
  }

  friend constexpr tested::strong_ordering
  strong_order(weak_precedence, weak_precedence) noexcept {
    // Deliberately disagrees with operator<=>.
    return tested::strong_ordering::greater;
  }
};

static_assert(tested::weak_order(weak_precedence{1}, weak_precedence{2}) < 0);

struct partial_precedence {
  int value;

  friend constexpr bool operator==(partial_precedence,
                                   partial_precedence) = default;

  friend constexpr tested::partial_ordering
  operator<=>(partial_precedence left, partial_precedence right) noexcept {
    if (left.value < right.value) {
      return tested::partial_ordering::less;
    }

    if (left.value > right.value) {
      return tested::partial_ordering::greater;
    }

    return tested::partial_ordering::equivalent;
  }

  friend constexpr tested::weak_ordering
  weak_order(partial_precedence, partial_precedence) noexcept {
    // Deliberately disagrees with operator<=>.
    return tested::weak_ordering::greater;
  }
};

static_assert(tested::partial_order(partial_precedence{1},
                                    partial_precedence{2}) < 0);

struct minimal_fallback_order {
  int value;

  friend constexpr bool operator==(minimal_fallback_order left,
                                   minimal_fallback_order right) noexcept {
    return left.value == right.value;
  }

  friend constexpr bool operator<(minimal_fallback_order left,
                                  minimal_fallback_order right) noexcept {
    return left.value < right.value;
  }
};

static_assert(!tested::totally_ordered<minimal_fallback_order>);

static_assert(tested::compare_strong_order_fallback(minimal_fallback_order{1},
                                                    minimal_fallback_order{2}) <
              0);

static_assert(tested::compare_weak_order_fallback(minimal_fallback_order{1},
                                                  minimal_fallback_order{2}) <
              0);

static_assert(tested::compare_partial_order_fallback(
                  minimal_fallback_order{2}, minimal_fallback_order{1}) > 0);

bool ftl_test() {
  int left = 0;
  int right = 0;

  const auto left_right = tested::compare_three_way{}(&left, &right);

  const auto right_left = tested::compare_three_way{}(&right, &left);

  if (left_right == 0 || right_left == 0) {
    return false;
  }

  if ((left_right < 0) != (right_left > 0)) {
    return false;
  }

  if ((left_right > 0) != (right_left < 0)) {
    return false;
  }

  return tested::compare_three_way{}(&left, &left) == 0;
}
