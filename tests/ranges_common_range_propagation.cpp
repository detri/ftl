#ifdef FTL_REPLACE_STL
#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/concepts>
#include <ftl/cstddef>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/tuple>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

template <class T> class input_test_iterator {
  T *current_ = nullptr;

public:
  using iterator_concept = tested::input_iterator_tag;

  using iterator_category = tested::input_iterator_tag;

  using value_type = tested::remove_cv_t<T>;

  using difference_type = tested::ptrdiff_t;

  using pointer = T *;
  using reference = T &;

  constexpr input_test_iterator() = default;

  constexpr explicit input_test_iterator(T *current) noexcept
      : current_(current) {}

  constexpr T *base() const noexcept { return current_; }

  constexpr T &operator*() const noexcept { return *current_; }

  constexpr input_test_iterator &operator++() noexcept {
    ++current_;
    return *this;
  }

  constexpr void operator++(int) noexcept { ++*this; }

  friend constexpr bool operator==(const input_test_iterator &,
                                   const input_test_iterator &) = default;
};

template <class T> class forward_test_iterator {
  T *current_ = nullptr;

public:
  using iterator_concept = tested::forward_iterator_tag;

  using iterator_category = tested::forward_iterator_tag;

  using value_type = tested::remove_cv_t<T>;

  using difference_type = tested::ptrdiff_t;

  using pointer = T *;
  using reference = T &;

  constexpr forward_test_iterator() = default;

  constexpr explicit forward_test_iterator(T *current) noexcept
      : current_(current) {}

  constexpr T *base() const noexcept { return current_; }

  constexpr T &operator*() const noexcept { return *current_; }

  constexpr forward_test_iterator &operator++() noexcept {
    ++current_;
    return *this;
  }

  constexpr forward_test_iterator operator++(int) noexcept {
    auto previous = *this;
    ++*this;
    return previous;
  }

  friend constexpr bool operator==(const forward_test_iterator &,
                                   const forward_test_iterator &) = default;
};

template <class T> class bidirectional_test_iterator {
  T *current_ = nullptr;

public:
  using iterator_concept = tested::bidirectional_iterator_tag;

  using iterator_category = tested::bidirectional_iterator_tag;

  using value_type = tested::remove_cv_t<T>;

  using difference_type = tested::ptrdiff_t;

  using pointer = T *;
  using reference = T &;

  constexpr bidirectional_test_iterator() = default;

  constexpr explicit bidirectional_test_iterator(T *current) noexcept
      : current_(current) {}

  constexpr T *base() const noexcept { return current_; }

  constexpr T &operator*() const noexcept { return *current_; }

  constexpr bidirectional_test_iterator &operator++() noexcept {
    ++current_;
    return *this;
  }

  constexpr bidirectional_test_iterator operator++(int) noexcept {
    auto previous = *this;
    ++*this;
    return previous;
  }

  constexpr bidirectional_test_iterator &operator--() noexcept {
    --current_;
    return *this;
  }

  constexpr bidirectional_test_iterator operator--(int) noexcept {
    auto previous = *this;
    --*this;
    return previous;
  }

  friend constexpr bool
  operator==(const bidirectional_test_iterator &,
             const bidirectional_test_iterator &) = default;
};

template <class T> constexpr T *iterator_address(T *iterator) noexcept {
  return iterator;
}

template <class T>
constexpr T *iterator_address(const input_test_iterator<T> &iterator) noexcept {
  return iterator.base();
}

template <class T>
constexpr T *
iterator_address(const forward_test_iterator<T> &iterator) noexcept {
  return iterator.base();
}

template <class T>
constexpr T *
iterator_address(const bidirectional_test_iterator<T> &iterator) noexcept {
  return iterator.base();
}

template <class Iterator> struct unsized_sentinel {
  Iterator last{};

  friend constexpr bool operator==(const Iterator &iterator,
                                   const unsized_sentinel &sentinel) noexcept {
    return iterator == sentinel.last;
  }

  friend constexpr bool operator==(const unsized_sentinel &sentinel,
                                   const Iterator &iterator) noexcept {
    return iterator == sentinel;
  }
};

template <class Iterator> struct sized_sentinel {
  Iterator last{};

  friend constexpr bool operator==(const Iterator &iterator,
                                   const sized_sentinel &sentinel) noexcept {
    return iterator == sentinel.last;
  }

  friend constexpr bool operator==(const sized_sentinel &sentinel,
                                   const Iterator &iterator) noexcept {
    return iterator == sentinel;
  }

  friend constexpr tested::ptrdiff_t
  operator-(const sized_sentinel &sentinel, const Iterator &iterator) noexcept {
    return iterator_address(sentinel.last) - iterator_address(iterator);
  }

  friend constexpr tested::ptrdiff_t
  operator-(const Iterator &iterator, const sized_sentinel &sentinel) noexcept {
    return iterator_address(iterator) - iterator_address(sentinel.last);
  }
};

template <class Iterator, class Sentinel = Iterator>
struct basic_test_view : tested::ranges::view_base {
  Iterator first{};
  Sentinel last{};

  constexpr basic_test_view() = default;

  constexpr basic_test_view(Iterator first, Sentinel last)
      : first(first), last(last) {}

  constexpr Iterator begin() noexcept { return first; }

  constexpr Iterator begin() const noexcept { return first; }

  constexpr Sentinel end() noexcept { return last; }

  constexpr Sentinel end() const noexcept { return last; }
};

using input_iterator = input_test_iterator<int>;

using forward_iterator = forward_test_iterator<int>;

using bidirectional_iterator = bidirectional_test_iterator<int>;

using common_input_view = basic_test_view<input_iterator>;

using non_common_input_view =
    basic_test_view<input_iterator, unsized_sentinel<input_iterator>>;

using common_forward_view = basic_test_view<forward_iterator>;

using non_common_forward_view =
    basic_test_view<forward_iterator, unsized_sentinel<forward_iterator>>;

using non_common_sized_forward_view =
    basic_test_view<forward_iterator, sized_sentinel<forward_iterator>>;

using common_bidirectional_view = basic_test_view<bidirectional_iterator>;

using non_common_bidirectional_view =
    basic_test_view<bidirectional_iterator,
                    unsized_sentinel<bidirectional_iterator>>;

using non_common_random_access_view =
    basic_test_view<int *, sized_sentinel<int *>>;

using pair_type = tested::pair<int, int>;

using pair_forward_iterator = forward_test_iterator<pair_type>;

using common_pair_view = basic_test_view<pair_forward_iterator>;

using non_common_pair_view =
    basic_test_view<pair_forward_iterator,
                    unsized_sentinel<pair_forward_iterator>>;

struct identity_function {
  constexpr int operator()(int value) const noexcept { return value; }
};

struct add_values {
  constexpr int operator()(int left, int right) const noexcept {
    return left + right;
  }
};

struct less_than_three {
  constexpr bool operator()(int value) const noexcept { return value < 3; }
};

struct equal_adjacent {
  constexpr bool operator()(int left, int right) const noexcept {
    return left == right;
  }
};

template <class R>
concept correctly_common =
    tested::ranges::range<R> && tested::ranges::common_range<R> &&
    tested::same_as<tested::ranges::iterator_t<R>,
                    tested::ranges::sentinel_t<R>>;

template <class R>
concept correctly_non_common =
    tested::ranges::range<R> && !tested::ranges::common_range<R> &&
    !tested::same_as<tested::ranges::iterator_t<R>,
                     tested::ranges::sentinel_t<R>>;

template <class R>
concept common_mutable_and_const =
    correctly_common<R> && correctly_common<const R>;

template <class R>
concept non_common_mutable_and_const =
    correctly_non_common<R> && correctly_non_common<const R>;

/*
 * Verify the test range categories first.
 */

static_assert(tested::ranges::input_range<common_input_view>);

static_assert(!tested::ranges::forward_range<common_input_view>);

static_assert(common_mutable_and_const<common_input_view>);

static_assert(non_common_mutable_and_const<non_common_input_view>);

static_assert(tested::ranges::forward_range<common_forward_view>);

static_assert(!tested::ranges::bidirectional_range<common_forward_view>);

static_assert(!tested::ranges::sized_range<common_forward_view>);

static_assert(common_mutable_and_const<common_forward_view>);

static_assert(non_common_mutable_and_const<non_common_forward_view>);

static_assert(tested::ranges::sized_range<non_common_sized_forward_view>);

static_assert(non_common_mutable_and_const<non_common_sized_forward_view>);

static_assert(tested::ranges::bidirectional_range<common_bidirectional_view>);

static_assert(!tested::ranges::random_access_range<common_bidirectional_view>);

static_assert(!tested::ranges::sized_range<common_bidirectional_view>);

static_assert(common_mutable_and_const<common_bidirectional_view>);

static_assert(non_common_mutable_and_const<non_common_bidirectional_view>);

static_assert(
    tested::ranges::random_access_range<non_common_random_access_view>);

static_assert(tested::ranges::sized_range<non_common_random_access_view>);

static_assert(non_common_mutable_and_const<non_common_random_access_view>);

/*
 * Fundamental ranges.
 */

using common_subrange = tested::ranges::subrange<forward_iterator>;

using non_common_subrange =
    tested::ranges::subrange<forward_iterator,
                             unsized_sentinel<forward_iterator>>;

using common_owned = tested::ranges::owning_view<common_forward_view>;

using non_common_owned = tested::ranges::owning_view<non_common_forward_view>;

static_assert(common_mutable_and_const<tested::ranges::empty_view<int>>);

static_assert(common_mutable_and_const<tested::ranges::single_view<int>>);

static_assert(common_mutable_and_const<tested::ranges::iota_view<int, int>>);

static_assert(correctly_non_common<tested::ranges::iota_view<int>>);

static_assert(common_mutable_and_const<tested::ranges::repeat_view<int, int>>);

static_assert(correctly_non_common<tested::ranges::repeat_view<int>>);

static_assert(common_mutable_and_const<common_subrange>);

static_assert(non_common_mutable_and_const<non_common_subrange>);

static_assert(common_mutable_and_const<common_owned>);

static_assert(non_common_mutable_and_const<non_common_owned>);

/*
 * Ordinary unary propagation.
 */

using common_transform = decltype(tested::ranges::views::transform(
    tested::declval<common_forward_view &>(), identity_function{}));

using non_common_transform = decltype(tested::ranges::views::transform(
    tested::declval<non_common_forward_view &>(), identity_function{}));

static_assert(correctly_common<common_transform>);

static_assert(correctly_non_common<non_common_transform>);

using common_filter = decltype(tested::ranges::views::filter(
    tested::declval<common_forward_view &>(), less_than_three{}));

using non_common_filter = decltype(tested::ranges::views::filter(
    tested::declval<non_common_forward_view &>(), less_than_three{}));

static_assert(correctly_common<common_filter>);

static_assert(correctly_non_common<non_common_filter>);

using common_drop = decltype(tested::ranges::views::drop(
    tested::declval<common_forward_view &>(), 2));

using non_common_drop = decltype(tested::ranges::views::drop(
    tested::declval<non_common_forward_view &>(), 2));

static_assert(correctly_common<common_drop>);

static_assert(correctly_non_common<non_common_drop>);

using common_drop_while = decltype(tested::ranges::views::drop_while(
    tested::declval<common_forward_view &>(), less_than_three{}));

using non_common_drop_while = decltype(tested::ranges::views::drop_while(
    tested::declval<non_common_forward_view &>(), less_than_three{}));

static_assert(correctly_common<common_drop_while>);

static_assert(correctly_non_common<non_common_drop_while>);

/*
 * take_view is common only for random-access sized bases.
 */

using common_take = decltype(tested::ranges::views::take(
    tested::declval<non_common_random_access_view &>(), 2));

using non_common_take = decltype(tested::ranges::views::take(
    tested::declval<common_forward_view &>(), 2));

static_assert(correctly_common<common_take>);

static_assert(correctly_non_common<non_common_take>);

/*
 * take_while_view always uses its predicate sentinel.
 */

using common_base_take_while = decltype(tested::ranges::views::take_while(
    tested::declval<common_forward_view &>(), less_than_three{}));

static_assert(correctly_non_common<common_base_take_while>);

/*
 * reverse_view and common_view manufacture common ends.
 */

using reversed_non_common = decltype(tested::ranges::views::reverse(
    tested::declval<non_common_bidirectional_view &>()));

using explicitly_common = decltype(tested::ranges::views::common(
    tested::declval<non_common_forward_view &>()));

static_assert(correctly_common<reversed_non_common>);

static_assert(correctly_common<explicitly_common>);

/*
 * elements_view and as_const_view propagate commonness.
 */

using common_elements = decltype(tested::ranges::views::elements<0>(
    tested::declval<common_pair_view &>()));

using non_common_elements = decltype(tested::ranges::views::elements<0>(
    tested::declval<non_common_pair_view &>()));

static_assert(correctly_common<common_elements>);

static_assert(correctly_non_common<non_common_elements>);

using common_as_const = decltype(tested::ranges::views::as_const(
    tested::declval<common_forward_view &>()));

using non_common_as_const = decltype(tested::ranges::views::as_const(
    tested::declval<non_common_forward_view &>()));

static_assert(correctly_common<common_as_const>);

static_assert(correctly_non_common<non_common_as_const>);

/*
 * enumerate_view requires forward, common, and sized simultaneously.
 */

using common_enumerate =
    decltype(tested::ranges::views::enumerate(tested::declval<int (&)[5]>()));

using unsized_common_enumerate = decltype(tested::ranges::views::enumerate(
    tested::declval<common_forward_view &>()));

using input_common_enumerate = decltype(tested::ranges::views::enumerate(
    tested::declval<common_input_view &>()));

using non_common_sized_enumerate = decltype(tested::ranges::views::enumerate(
    tested::declval<non_common_random_access_view &>()));

static_assert(correctly_common<common_enumerate>);

static_assert(correctly_non_common<unsized_common_enumerate>);

static_assert(correctly_non_common<input_common_enumerate>);

static_assert(correctly_non_common<non_common_sized_enumerate>);

/*
 * stride_view propagates commonness directly.
 */

using common_stride = decltype(tested::ranges::views::stride(
    tested::declval<common_forward_view &>(), 2));

using non_common_bidirectional_stride = decltype(tested::ranges::views::stride(
    tested::declval<common_bidirectional_view &>(), 2));

using non_common_stride = decltype(tested::ranges::views::stride(
    tested::declval<non_common_random_access_view &>(), 2));

static_assert(correctly_non_common<non_common_bidirectional_stride>);

static_assert(correctly_common<common_stride>);

static_assert(correctly_non_common<non_common_stride>);

/*
 * adjacent and adjacent_transform propagate commonness.
 */

using common_adjacent = decltype(tested::ranges::views::adjacent<2>(
    tested::declval<common_forward_view &>()));

using non_common_adjacent = decltype(tested::ranges::views::adjacent<2>(
    tested::declval<non_common_random_access_view &>()));

using common_adjacent_transform =
    decltype(tested::ranges::views::adjacent_transform<2>(
        tested::declval<common_forward_view &>(), add_values{}));

using non_common_adjacent_transform =
    decltype(tested::ranges::views::adjacent_transform<2>(
        tested::declval<non_common_random_access_view &>(), add_values{}));

static_assert(correctly_common<common_adjacent>);

static_assert(correctly_non_common<non_common_adjacent>);

static_assert(correctly_common<common_adjacent_transform>);

static_assert(correctly_non_common<non_common_adjacent_transform>);

/*
 * slide_view is common for common bases and for random-access sized
 * bases, even when the latter has a distinct sentinel.
 */

using common_forward_slide = decltype(tested::ranges::views::slide(
    tested::declval<common_forward_view &>(), 2));

using common_random_access_slide = decltype(tested::ranges::views::slide(
    tested::declval<non_common_random_access_view &>(), 2));

using non_common_slide = decltype(tested::ranges::views::slide(
    tested::declval<non_common_forward_view &>(), 2));

static_assert(correctly_common<common_forward_slide>);

static_assert(correctly_common<common_random_access_slide>);

static_assert(correctly_non_common<non_common_slide>);

/*
 * Forward chunk_view:
 *
 * - common and sized: common
 * - common and non-bidirectional: common
 * - common, bidirectional, and unsized: non-common
 * - input-only specialization: non-common
 */

using common_sized_chunk =
    decltype(tested::ranges::views::chunk(tested::declval<int (&)[5]>(), 2));

using common_forward_chunk = decltype(tested::ranges::views::chunk(
    tested::declval<common_forward_view &>(), 2));

using non_common_bidirectional_chunk = decltype(tested::ranges::views::chunk(
    tested::declval<common_bidirectional_view &>(), 2));

using input_chunk = decltype(tested::ranges::views::chunk(
    tested::declval<common_input_view &>(), 2));

using non_common_base_chunk = decltype(tested::ranges::views::chunk(
    tested::declval<non_common_sized_forward_view &>(), 2));

static_assert(correctly_common<common_sized_chunk>);

static_assert(correctly_common<common_forward_chunk>);

static_assert(correctly_non_common<non_common_bidirectional_chunk>);

static_assert(correctly_non_common<input_chunk>);

static_assert(correctly_non_common<non_common_base_chunk>);

/*
 * chunk_by propagates commonness.
 */

using common_chunk_by = decltype(tested::ranges::views::chunk_by(
    tested::declval<common_forward_view &>(), equal_adjacent{}));

using non_common_chunk_by = decltype(tested::ranges::views::chunk_by(
    tested::declval<non_common_forward_view &>(), equal_adjacent{}));

static_assert(correctly_common<common_chunk_by>);

static_assert(correctly_non_common<non_common_chunk_by>);

/*
 * split propagates commonness.
 *
 * lazy_split is common only when the base is both forward and common.
 */

using common_split = decltype(tested::ranges::views::split(
    tested::declval<common_forward_view &>(), 0));

using non_common_split = decltype(tested::ranges::views::split(
    tested::declval<non_common_forward_view &>(), 0));

using common_lazy_split = decltype(tested::ranges::views::lazy_split(
    tested::declval<common_forward_view &>(), 0));

using non_common_lazy_split = decltype(tested::ranges::views::lazy_split(
    tested::declval<non_common_forward_view &>(), 0));

using input_lazy_split = decltype(tested::ranges::views::lazy_split(
    tested::declval<common_input_view &>(), 0));

static_assert(correctly_common<common_split>);

static_assert(correctly_non_common<non_common_split>);

static_assert(correctly_common<common_lazy_split>);

static_assert(correctly_non_common<non_common_lazy_split>);

static_assert(correctly_non_common<input_lazy_split>);

/*
 * join requires common forward outer and inner ranges, with the inner
 * range obtained by reference.
 */

using common_inner_array = common_forward_view[2];

using non_common_inner_array = non_common_forward_view[2];

using common_join = decltype(tested::ranges::views::join(
    tested::declval<common_inner_array &>()));

using non_common_inner_join = decltype(tested::ranges::views::join(
    tested::declval<non_common_inner_array &>()));

using outer_iterator = forward_test_iterator<common_forward_view>;

using non_common_outer_view =
    basic_test_view<outer_iterator, unsized_sentinel<outer_iterator>>;

using non_common_outer_join = decltype(tested::ranges::views::join(
    tested::declval<non_common_outer_view &>()));

static_assert(correctly_common<common_join>);

static_assert(correctly_non_common<non_common_inner_join>);

static_assert(correctly_non_common<non_common_outer_join>);

/*
 * zip commonness has three independent paths.
 */

using single_common_zip = decltype(tested::ranges::views::zip(
    tested::declval<common_bidirectional_view &>()));

using non_bidirectional_common_zip = decltype(tested::ranges::views::zip(
    tested::declval<common_input_view &>(),
    tested::declval<common_bidirectional_view &>()));

using random_access_sized_zip = decltype(tested::ranges::views::zip(
    tested::declval<non_common_random_access_view &>(),
    tested::declval<non_common_random_access_view &>()));

using bidirectional_unsized_zip = decltype(tested::ranges::views::zip(
    tested::declval<common_bidirectional_view &>(),
    tested::declval<common_bidirectional_view &>()));

using mixed_non_common_zip = decltype(tested::ranges::views::zip(
    tested::declval<common_forward_view &>(),
    tested::declval<non_common_forward_view &>()));

static_assert(correctly_common<single_common_zip>);

static_assert(correctly_common<non_bidirectional_common_zip>);

static_assert(correctly_common<random_access_sized_zip>);

static_assert(correctly_non_common<bidirectional_unsized_zip>);

static_assert(correctly_non_common<mixed_non_common_zip>);

/*
 * zip_transform inherits zip_view's end shape.
 */

using common_zip_transform = decltype(tested::ranges::views::zip_transform(
    add_values{}, tested::declval<common_input_view &>(),
    tested::declval<common_forward_view &>()));

using random_access_zip_transform =
    decltype(tested::ranges::views::zip_transform(
        add_values{}, tested::declval<non_common_random_access_view &>(),
        tested::declval<non_common_random_access_view &>()));

using non_common_zip_transform = decltype(tested::ranges::views::zip_transform(
    add_values{}, tested::declval<common_bidirectional_view &>(),
    tested::declval<common_bidirectional_view &>()));

static_assert(correctly_common<common_zip_transform>);

static_assert(correctly_common<random_access_zip_transform>);

static_assert(correctly_non_common<non_common_zip_transform>);

/*
 * Cartesian product only uses the first range to determine commonness.
 */

using common_first_product = decltype(tested::ranges::views::cartesian_product(
    tested::declval<common_input_view &>(),
    tested::declval<non_common_forward_view &>()));

using random_access_first_product =
    decltype(tested::ranges::views::cartesian_product(
        tested::declval<non_common_random_access_view &>(),
        tested::declval<common_forward_view &>()));

using non_common_first_product =
    decltype(tested::ranges::views::cartesian_product(
        tested::declval<non_common_forward_view &>(),
        tested::declval<common_forward_view &>()));

using empty_product = decltype(tested::ranges::views::cartesian_product());

static_assert(correctly_common<common_first_product>);

static_assert(correctly_common<random_access_first_product>);

static_assert(correctly_non_common<non_common_first_product>);

static_assert(correctly_common<empty_product>);

/*
 * Verify that common iterator end values describe the logical end, not
 * merely that begin() and end() have the same type.
 */

template <class R> bool length_is(R &range, tested::size_t expected) {
  static_assert(tested::ranges::common_range<R>);

  auto iterator = tested::ranges::begin(range);

  const auto bound = tested::ranges::end(range);

  tested::size_t length = 0;

  while (iterator != bound && length <= expected) {
    ++iterator;
    ++length;
  }

  return iterator == bound && length == expected;
}

bool common_end_values_work() {
  int values[] = {1, 0, 2, 0, 3};

  int short_values[] = {10, 20, 30};

  non_common_random_access_view random_access{
      values, sized_sentinel<int *>{values + 5}};

  non_common_random_access_view short_random_access{
      short_values, sized_sentinel<int *>{short_values + 3}};

  common_input_view input{input_iterator(values), input_iterator(values + 5)};

  common_forward_view forward{forward_iterator(values),
                              forward_iterator(values + 5)};

  common_forward_view short_forward{forward_iterator(short_values),
                                    forward_iterator(short_values + 3)};

  common_bidirectional_view bidirectional{bidirectional_iterator(values),
                                          bidirectional_iterator(values + 5)};

  non_common_bidirectional_view non_common_bidirectional{
      bidirectional_iterator(values), unsized_sentinel<bidirectional_iterator>{
                                          bidirectional_iterator(values + 5)}};

  non_common_forward_view non_common_forward{
      forward_iterator(values),
      unsized_sentinel<forward_iterator>{forward_iterator(values + 5)}};

  auto taken = tested::ranges::views::take(random_access, 3);

  if (!length_is(taken, 3))
    return false;

  auto reversed = tested::ranges::views::reverse(non_common_bidirectional);

  if (!length_is(reversed, 5))
    return false;

  auto made_common = tested::ranges::views::common(non_common_forward);

  if (!length_is(made_common, 5))
    return false;

  auto enumerated = tested::ranges::views::enumerate(values);

  if (!length_is(enumerated, 5))
    return false;

  auto strided = tested::ranges::views::stride(forward, 2);

  if (!length_is(strided, 3))
    return false;

  auto adjacent = tested::ranges::views::adjacent<2>(forward);

  if (!length_is(adjacent, 4))
    return false;

  auto slid = tested::ranges::views::slide(random_access, 2);

  if (!length_is(slid, 4))
    return false;

  auto chunked = tested::ranges::views::chunk(forward, 2);

  if (!length_is(chunked, 3))
    return false;

  auto zipped_input = tested::ranges::views::zip(input, short_forward);

  if (!length_is(zipped_input, 3))
    return false;

  auto zipped_random_access =
      tested::ranges::views::zip(random_access, short_random_access);

  if (!length_is(zipped_random_access, 3)) {
    return false;
  }

  auto transformed_zip =
      tested::ranges::views::zip_transform(add_values{}, input, short_forward);

  if (!length_is(transformed_zip, 3)) {
    return false;
  }

  auto product =
      tested::ranges::views::cartesian_product(short_random_access, forward);

  if (!length_is(product, 15))
    return false;

  auto split = tested::ranges::views::split(forward, 0);

  if (!length_is(split, 3))
    return false;

  auto lazy_split = tested::ranges::views::lazy_split(forward, 0);

  if (!length_is(lazy_split, 3))
    return false;

  common_forward_view inner[] = {
      {forward_iterator(values), forward_iterator(values + 2)},
      {forward_iterator(values + 2), forward_iterator(values + 5)}};

  auto joined = tested::ranges::views::join(inner);

  if (!length_is(joined, 5))
    return false;

  auto grouped = tested::ranges::views::chunk_by(forward, equal_adjacent{});

  if (!length_is(grouped, 5))
    return false;

  return true;
}

bool ftl_test() { return common_end_values_work(); }