#ifdef FTL_REPLACE_STL
#include <concepts>
namespace tested = std;
#else
#include <ftl/concepts>
namespace tested = ftl;
#endif

#if __cpp_lib_concepts != 202207L
#error invalid <concepts> feature-test macro
#endif

struct base {};
struct derived : base {};

static_assert(tested::same_as<tested::remove_cvref_t<const int &>, int>);

static_assert(tested::integral<int>);
static_assert(tested::signed_integral<int>);
static_assert(tested::unsigned_integral<unsigned>);
static_assert(tested::floating_point<double>);

static_assert(tested::derived_from<derived, base>);
static_assert(tested::convertible_to<int, long>);

static_assert(tested::common_reference_with<int &, const int &>);

static_assert(tested::common_with<int, long>);

static_assert(tested::default_initializable<int>);
static_assert(tested::copy_constructible<int>);
static_assert(tested::assignable_from<int &, int>);

static_assert(tested::equality_comparable<int>);
static_assert(tested::equality_comparable_with<int, long>);

static_assert(tested::totally_ordered<int>);
static_assert(tested::totally_ordered_with<int, long>);

static_assert(tested::swappable<int>);
static_assert(tested::swappable_with<int &, int &>);

static_assert(tested::movable<int>);
static_assert(tested::copyable<int>);
static_assert(tested::semiregular<int>);
static_assert(tested::regular<int>);

static_assert(tested::invocable<int (*)(int), int>);
static_assert(tested::predicate<bool (*)(int), int>);

static_assert(tested::relation<bool (*)(int, int), int, int>);

static_assert(tested::equivalence_relation<bool (*)(int, int), int, int>);

static_assert(tested::strict_weak_order<bool (*)(int, int), int, int>);

namespace adl_swap_test {

struct value {
  int payload;

  constexpr explicit value(int input) : payload(input) {}

  value(const value &) = delete;
  value(value &&) = delete;

  value &operator=(const value &) = delete;
  value &operator=(value &&) = delete;
};

constexpr void swap(value &left, value &right) noexcept {
  int temporary = left.payload;
  left.payload = right.payload;
  right.payload = temporary;
}

} // namespace adl_swap_test

static_assert(tested::swappable<adl_swap_test::value>);

namespace unconstrained_swap_test {

struct value {
  value() = default;

  value(const value &) = delete;
  value(value &&) = delete;

  value &operator=(const value &) = delete;
  value &operator=(value &&) = delete;
};

template <class T> constexpr void swap(T &, T &) noexcept {}

} // namespace unconstrained_swap_test

static_assert(!tested::swappable<unconstrained_swap_test::value>);

struct comparison_common {
  int value;

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

  friend constexpr bool operator==(const comparison_right &,
                                   const comparison_right &) = default;
};

constexpr bool operator==(const comparison_left &left,
                          const comparison_right &right) noexcept {
  return left.value == right.value;
}

constexpr bool operator==(const comparison_right &left,
                          const comparison_left &right) noexcept {
  return left.value == right.value;
}

constexpr bool operator!=(const comparison_left &left,
                          const comparison_right &right) noexcept {
  return !(left == right);
}

constexpr bool operator!=(const comparison_right &left,
                          const comparison_left &right) noexcept {
  return !(left == right);
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

// This is the P2404 regression case.
//
// The C++20 rule required the const lvalue references themselves
// to convert to their common reference type. These types can only
// perform that conversion as values/rvalues.
//
// C++23 intentionally accepts them.
static_assert(!tested::common_reference_with<const comparison_left &,
                                             const comparison_right &>);

static_assert(
    tested::equality_comparable_with<comparison_left, comparison_right>);

bool ftl_test() {
  {
    int left = 1;
    int right = 2;

    tested::ranges::swap(left, right);

    if (left != 2 || right != 1)
      return false;
  }

  {
    int left[3] = {1, 2, 3};
    int right[3] = {4, 5, 6};

    tested::ranges::swap(left, right);

    if (left[0] != 4 || left[1] != 5 || left[2] != 6 || right[0] != 1 ||
        right[1] != 2 || right[2] != 3) {
      return false;
    }
  }

  {
    adl_swap_test::value left{7};
    adl_swap_test::value right{9};

    tested::ranges::swap(left, right);

    if (left.payload != 9 || right.payload != 7) {
      return false;
    }
  }

  return true;
}
