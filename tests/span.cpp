#ifdef FTL_REPLACE_STL
#include <array>
#include <cstddef>
#include <limits>
#include <span>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/cstddef>
#include <ftl/limits>
#include <ftl/span>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct base {
  int value;
};

struct derived : base {};

template <class T> struct contiguous_sized_test_range {
  T *first = nullptr;
  tested::size_t count = 0;

  [[nodiscard]]
  constexpr T *begin() const noexcept {
    return first;
  }

  [[nodiscard]]
  constexpr T *end() const noexcept {
    return first + count;
  }

  [[nodiscard]]
  constexpr T *data() const noexcept {
    return first;
  }

  [[nodiscard]]
  constexpr tested::size_t size() const noexcept {
    return count;
  }
};

using int_test_range = contiguous_sized_test_range<int>;

using derived_test_range = contiguous_sized_test_range<derived>;

template <class T>
concept default_span_constructible = requires { T{}; };

template <class T>
concept readable_bytes_available =
    requires(T value) { tested::as_bytes(value); };

template <class T>
concept writable_bytes_available =
    requires(T value) { tested::as_writable_bytes(value); };

static_assert(tested::dynamic_extent ==
              tested::numeric_limits<tested::size_t>::max());

static_assert(tested::span<int>::extent == tested::dynamic_extent);

static_assert(tested::span<int, 4>::extent == 4);

static_assert(default_span_constructible<tested::span<int>>);

static_assert(default_span_constructible<tested::span<int, 0>>);

static_assert(!default_span_constructible<tested::span<int, 1>>);

static_assert(tested::is_trivially_copyable_v<tested::span<int>>);

static_assert(tested::is_trivially_copyable_v<tested::span<int, 4>>);

static_assert(sizeof(tested::span<int, 4>) == sizeof(int *));

static_assert(sizeof(tested::span<int>) >=
              sizeof(int *) + sizeof(tested::size_t));

static_assert(tested::is_same_v<typename tested::span<const int>::element_type,
                                const int>);

static_assert(
    tested::is_same_v<typename tested::span<const int>::value_type, int>);

static_assert(tested::is_same_v<typename tested::span<int>::reference, int &>);

static_assert(tested::is_same_v<typename tested::span<const int>::reference,
                                const int &>);

static_assert(
    tested::is_constructible_v<tested::span<const int>, tested::span<int>>);

static_assert(
    !tested::is_constructible_v<tested::span<int>, tested::span<const int>>);

static_assert(
    tested::is_convertible_v<tested::span<int, 4>, tested::span<const int, 4>>);

static_assert(
    tested::is_convertible_v<tested::span<int, 4>, tested::span<int>>);

static_assert(
    !tested::is_convertible_v<tested::span<int>, tested::span<int, 4>>);

static_assert(
    tested::is_constructible_v<tested::span<int, 4>, tested::span<int>>);

static_assert(
    !tested::is_constructible_v<tested::span<int, 3>, tested::span<int, 4>>);

static_assert(
    !tested::is_constructible_v<tested::span<base>, tested::span<derived>>);

static_assert(tested::ranges::contiguous_range<int_test_range>);

static_assert(tested::ranges::sized_range<int_test_range>);

static_assert(tested::ranges::view<tested::span<int>>);

static_assert(tested::ranges::borrowed_range<tested::span<int>>);

static_assert(tested::is_constructible_v<tested::span<int>, int_test_range &>);

static_assert(tested::is_convertible_v<int_test_range &, tested::span<int>>);

static_assert(
    tested::is_constructible_v<tested::span<int, 4>, int_test_range &>);

static_assert(
    !tested::is_convertible_v<int_test_range &, tested::span<int, 4>>);

// A mutable span cannot bind to a temporary
// range unless that range is borrowed.
static_assert(!tested::is_constructible_v<tested::span<int>, int_test_range>);

// A const-element span may bind to a
// temporary contiguous sized range.
static_assert(
    tested::is_constructible_v<tested::span<const int>, int_test_range>);

// Array convertibility must reject derived-to-base
// element slicing through the range constructor.
static_assert(
    !tested::is_constructible_v<tested::span<base>, derived_test_range &>);

static_assert(readable_bytes_available<tested::span<int>>);

static_assert(!readable_bytes_available<tested::span<volatile int>>);

static_assert(writable_bytes_available<tested::span<int>>);

static_assert(!writable_bytes_available<tested::span<const int>>);

static_assert(!writable_bytes_available<tested::span<volatile int>>);

constexpr bool span_constexpr_works() {
  int values[] = {1, 2, 3, 4};

  tested::span fixed{values};

  contiguous_sized_test_range<int> range{values, 4};

  tested::span from_range{range};

  static_assert(tested::is_same_v<decltype(from_range), tested::span<int>>);

  if (from_range.data() != values || from_range.size() != 4 ||
      from_range[2] != 3) {
    return false;
  }

  tested::span<int, 4> fixed_from_range(range);

  if (fixed_from_range.data() != values || fixed_from_range.size() != 4) {
    return false;
  }

  tested::span<const int> const_from_temporary(
      contiguous_sized_test_range<int>{values, 4});

  if (const_from_temporary.data() != values ||
      const_from_temporary.size() != 4 || const_from_temporary.back() != 4) {
    return false;
  }

  static_assert(tested::is_same_v<decltype(fixed), tested::span<int, 4>>);

  if (fixed.size() != 4)
    return false;

  if (fixed.size_bytes() != 4 * sizeof(int)) {
    return false;
  }

  if (fixed.empty())
    return false;

  if (fixed.data() != values)
    return false;

  if (fixed.front() != 1)
    return false;

  if (fixed.back() != 4)
    return false;

  if (fixed[2] != 3)
    return false;

  fixed[1] = 20;

  if (values[1] != 20)
    return false;

  auto first_two = fixed.first<2>();

  static_assert(tested::is_same_v<decltype(first_two), tested::span<int, 2>>);

  if (first_two.size() != 2 || first_two[0] != 1 || first_two[1] != 20) {
    return false;
  }

  auto last_two = fixed.last<2>();

  static_assert(tested::is_same_v<decltype(last_two), tested::span<int, 2>>);

  if (last_two.size() != 2 || last_two[0] != 3 || last_two[1] != 4) {
    return false;
  }

  auto middle = fixed.subspan<1, 2>();

  static_assert(tested::is_same_v<decltype(middle), tested::span<int, 2>>);

  if (middle.size() != 2 || middle[0] != 20 || middle[1] != 3) {
    return false;
  }

  auto tail = fixed.subspan<2>();

  static_assert(tested::is_same_v<decltype(tail), tested::span<int, 2>>);

  if (tail.size() != 2 || tail[0] != 3 || tail[1] != 4) {
    return false;
  }

  auto dynamic_first = fixed.first(3);

  static_assert(tested::is_same_v<decltype(dynamic_first), tested::span<int>>);

  if (dynamic_first.size() != 3 || dynamic_first.front() != 1 ||
      dynamic_first.back() != 3) {
    return false;
  }

  auto dynamic_last = fixed.last(3);

  if (dynamic_last.size() != 3 || dynamic_last.front() != 20 ||
      dynamic_last.back() != 4) {
    return false;
  }

  auto dynamic_middle = fixed.subspan(1, 2);

  if (dynamic_middle.size() != 2 || dynamic_middle[0] != 20 ||
      dynamic_middle[1] != 3) {
    return false;
  }

  auto dynamic_tail = fixed.subspan(2);

  if (dynamic_tail.size() != 2 || dynamic_tail.front() != 3 ||
      dynamic_tail.back() != 4) {
    return false;
  }

  if (fixed.begin() != values)
    return false;

  if (fixed.end() != values + 4)
    return false;

  if (*fixed.begin() != 1)
    return false;

  if (*(fixed.end() - 1) != 4)
    return false;

  tested::span<int> dynamic(values, 4);

  if (dynamic.data() != values || dynamic.size() != 4) {
    return false;
  }

  tested::span<int> from_pair(values, values + 4);

  if (from_pair.data() != values || from_pair.size() != 4) {
    return false;
  }

  tested::span<const int> constant = fixed;

  if (constant.data() != values || constant[1] != 20) {
    return false;
  }

  tested::span<int, 0> empty(values, 0);

  if (!empty.empty() || empty.size() != 0 || empty.begin() != empty.end()) {
    return false;
  }

  return true;
}

static_assert(span_constexpr_works());

bool ftl_test() {
  int values[] = {0x01020304, 0x11121314, 0x21222324, 0x31323334};

  tested::span fixed{values};

  if (fixed.size() != 4)
    return false;

  if (*fixed.rbegin() != values[3])
    return false;

  if (*(fixed.rend() - 1) != values[0])
    return false;

  if (*fixed.cbegin() != values[0])
    return false;

  if (*(fixed.cend() - 1) != values[3])
    return false;

  if (*fixed.crbegin() != values[3])
    return false;

  if (*(fixed.crend() - 1) != values[0])
    return false;

  tested::array<int, 4> array_values{1, 2, 3, 4};

  tested::span from_array{array_values};

  static_assert(tested::is_same_v<decltype(from_array), tested::span<int, 4>>);

  if (from_array.data() != array_values.data()) {
    return false;
  }

  const tested::array<int, 4> const_array_values{5, 6, 7, 8};

  tested::span from_const_array{const_array_values};

  static_assert(tested::is_same_v<decltype(from_const_array),
                                  tested::span<const int, 4>>);

  if (from_const_array.front() != 5 || from_const_array.back() != 8) {
    return false;
  }

  tested::span<int> dynamic(values, 4);

  tested::span<int, 4> fixed_again(dynamic);

  if (fixed_again.data() != values || fixed_again.size() != 4) {
    return false;
  }

  auto bytes = tested::as_bytes(fixed);

  static_assert(
      tested::is_same_v<decltype(bytes),
                        tested::span<const tested::byte, sizeof(int) * 4>>);

  if (bytes.size() != sizeof(values))
    return false;

  if (bytes.data() != reinterpret_cast<const tested::byte *>(values)) {
    return false;
  }

  auto writable = tested::as_writable_bytes(fixed);

  static_assert(tested::is_same_v<decltype(writable),
                                  tested::span<tested::byte, sizeof(int) * 4>>);

  if (writable.size() != sizeof(values))
    return false;

  if (writable.data() != reinterpret_cast<tested::byte *>(values)) {
    return false;
  }

  auto dynamic_bytes = tested::as_bytes(dynamic);

  static_assert(tested::is_same_v<decltype(dynamic_bytes),
                                  tested::span<const tested::byte>>);

  if (dynamic_bytes.size() != sizeof(values)) {
    return false;
  }

  const auto &const_fixed = fixed;

  if (const_fixed[0] != values[0])
    return false;

  // Constness of the span object does not
  // propagate to mutable elements.
  const_fixed[0] = 42;

  if (values[0] != 42)
    return false;

  tested::span<const int> const_elements = fixed;

  if (const_elements.front() != 42)
    return false;

  return true;
}