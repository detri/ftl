#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/iterator>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

using mutable_iterator = tested::basic_const_iterator<int *>;

using constant_iterator = tested::basic_const_iterator<const int *>;

static_assert(tested::totally_ordered_with<int*, const int*>);

template <class T>
concept has_iterator_category = requires { typename T::iterator_category; };

template <class T>
concept has_predecrement = requires(T value) { --value; };

template <class T>
concept has_plus_assign =
    requires(T value, typename T::difference_type offset) { value += offset; };

template <class T>
concept has_subscript = requires(
    const T value, typename T::difference_type offset) { value[offset]; };

template <class Left, class Right>
concept has_equality = requires(const Left &left, const Right &right) {
  { left == right } -> tested::same_as<bool>;
};

template <class Left, class Right>
concept has_less_than = requires(const Left &left, const Right &right) {
  { left < right } -> tested::same_as<bool>;
};

template <class Left, class Right>
concept has_spaceship =
    requires(const Left &left, const Right &right) { left <=> right; };

template <class Left, class Right>
concept has_difference =
    requires(const Left &left, const Right &right) { left - right; };

struct input_test_iterator {
  using iterator_concept = tested::input_iterator_tag;

  using iterator_category = tested::input_iterator_tag;

  using value_type = int;
  using difference_type = tested::ptrdiff_t;
  using pointer = int *;
  using reference = int &;

  int *current = nullptr;

  constexpr int &operator*() const noexcept { return *current; }

  constexpr input_test_iterator &operator++() noexcept {
    ++current;
    return *this;
  }

  constexpr void operator++(int) noexcept { ++current; }

  friend constexpr bool operator==(const input_test_iterator &,
                                   const input_test_iterator &) = default;
};

struct input_test_sentinel {
  int *end = nullptr;

  friend constexpr bool operator==(const input_test_iterator &iterator,
                                   input_test_sentinel sentinel) noexcept {
    return iterator.current == sentinel.end;
  }

  friend constexpr bool
  operator==(input_test_sentinel sentinel,
             const input_test_iterator &iterator) noexcept {
    return sentinel.end == iterator.current;
  }

  friend constexpr bool operator!=(const input_test_iterator &iterator,
                                   input_test_sentinel sentinel) noexcept {
    return !(iterator == sentinel);
  }

  friend constexpr bool
  operator!=(input_test_sentinel sentinel,
             const input_test_iterator &iterator) noexcept {
    return !(sentinel == iterator);
  }
};

static_assert(tested::input_iterator<input_test_iterator>);

static_assert(tested::sentinel_for<input_test_sentinel, input_test_iterator>);

static_assert(
    !tested::sized_sentinel_for<input_test_sentinel, input_test_iterator>);

using input_const_iterator = tested::basic_const_iterator<input_test_iterator>;

static_assert(tested::same_as<typename mutable_iterator::iterator_concept,
                              tested::contiguous_iterator_tag>);

static_assert(tested::same_as<typename mutable_iterator::iterator_category,
                              tested::random_access_iterator_tag>);

static_assert(tested::same_as<typename mutable_iterator::value_type, int>);

static_assert(tested::same_as<typename mutable_iterator::difference_type,
                              tested::ptrdiff_t>);

/*
 * The Iterator constructor is intentionally implicit.
 */
static_assert(tested::convertible_to<int *, mutable_iterator>);

static_assert(tested::convertible_to<mutable_iterator, constant_iterator>);

static_assert(
    tested::same_as<
        decltype(tested::declval<const mutable_iterator &>().operator->()),
        const int *>);

static_assert(tested::same_as<decltype(tested::ranges::iter_move(
                                  tested::declval<const mutable_iterator &>())),
                              const int &&>);

static_assert(tested::same_as<tested::common_reference_t<const int &&, int &&>,
                              const int &&>);

static_assert(tested::same_as<tested::common_reference_t<int &&, const int &&>,
                              const int &&>);

static_assert(
    tested::same_as<tested::common_reference_t<int &&, int &&>, int &&>);

static_assert(tested::same_as<tested::common_reference_t<const int &, int &>,
                              const int &>);

static_assert(tested::totally_ordered_with<
    const int *,
    mutable_iterator>);

static_assert(requires(mutable_iterator mutable_value,
                       constant_iterator constant_value, int *mutable_pointer,
                       const int *constant_pointer) {
  { mutable_value == mutable_pointer } -> tested::same_as<bool>;

  { mutable_pointer == mutable_value } -> tested::same_as<bool>;

  { mutable_value == constant_value } -> tested::same_as<bool>;

  { mutable_value < constant_value } -> tested::same_as<bool>;

  { constant_value > mutable_value } -> tested::same_as<bool>;

  mutable_value <=> constant_value;

  { mutable_value - constant_value } -> tested::same_as<tested::ptrdiff_t>;

  { constant_pointer - mutable_value } -> tested::same_as<tested::ptrdiff_t>;
});

/*
 * iterator_category is present only when the underlying
 * iterator models forward_iterator.
 */
static_assert(!has_iterator_category<input_const_iterator>);

/*
 * Input-only iterators do not acquire stronger operations
 * from basic_const_iterator.
 */
static_assert(!has_predecrement<input_const_iterator>);

static_assert(!has_plus_assign<input_const_iterator>);

static_assert(!has_subscript<input_const_iterator>);

static_assert(!has_less_than<input_const_iterator, input_const_iterator>);

static_assert(!has_spaceship<input_const_iterator, input_const_iterator>);

/*
 * An unsized sentinel supports equality but not subtraction.
 */
static_assert(has_equality<input_const_iterator, input_test_sentinel>);

static_assert(!has_difference<input_const_iterator, input_test_sentinel>);

static_assert(!has_difference<input_test_sentinel, input_const_iterator>);

/*
 * Arbitrary unrelated values must not become sentinels merely
 * because the comparison function has a permissive body.
 */
static_assert(!has_equality<input_const_iterator, int>);

static_assert(has_predecrement<mutable_iterator>);

static_assert(has_plus_assign<mutable_iterator>);

static_assert(has_subscript<mutable_iterator>);

static_assert(has_less_than<mutable_iterator, constant_iterator>);

static_assert(has_spaceship<mutable_iterator, constant_iterator>);

static_assert(has_difference<mutable_iterator, constant_iterator>);

struct throwing_move_iterator {
  using iterator_concept = tested::input_iterator_tag;

  using iterator_category = tested::input_iterator_tag;

  using value_type = int;
  using difference_type = tested::ptrdiff_t;
  using pointer = int *;
  using reference = int &;

  int *current = nullptr;

  constexpr int &operator*() const noexcept { return *current; }

  constexpr throwing_move_iterator &operator++() noexcept {
    ++current;
    return *this;
  }

  constexpr void operator++(int) noexcept { ++current; }

  friend constexpr bool operator==(const throwing_move_iterator &,
                                   const throwing_move_iterator &) = default;

  friend constexpr int &&
  iter_move(const throwing_move_iterator &iterator) noexcept(false) {
    return static_cast<int &&>(*iterator.current);
  }
};

static_assert(tested::input_iterator<throwing_move_iterator>);

using throwing_const_iterator =
    tested::basic_const_iterator<throwing_move_iterator>;

/*
 * base() const& is unconditionally noexcept.
 */
static_assert(noexcept(tested::declval<const mutable_iterator &>().base()));

static_assert(noexcept(tested::ranges::iter_move(tested::declval<int *&>())));

static_assert(tested::same_as<
              decltype(tested::ranges::iter_move(tested::declval<int *&>())),
              int &&>);

/*
 * Pointer iter_move is non-throwing, so the adaptor's
 * iter_move is non-throwing.
 */
static_assert(noexcept(
    tested::ranges::iter_move(tested::declval<const mutable_iterator &>())));

/*
 * The adaptor must preserve a potentially throwing
 * underlying iter_move.
 */
static_assert(!noexcept(tested::ranges::iter_move(
    tested::declval<const throwing_const_iterator &>())));

constexpr bool basic_const_iterator_works() {
  int values[] = {1, 2, 3};

  mutable_iterator first(values);
  mutable_iterator second(values + 1);
  constant_iterator finish(values + 3);

  if (*first != 1)
    return false;

  if (first.operator->() != values)
    return false;

  auto previous = first++;

  if (*previous != 1 || *first != 2)
    return false;

  if (!(first < finish))
    return false;

  if (!(finish > first))
    return false;

  if (finish - first != 2)
    return false;

  if ((first <=> finish) >= 0)
    return false;

  if (!(second == first))
    return false;

  return true;
}

static_assert(basic_const_iterator_works());

bool ftl_test() { return basic_const_iterator_works(); }
