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

template <class T> struct unsized_sentinel {
  T *last = nullptr;

  friend constexpr bool operator==(T *iterator,
                                   unsized_sentinel sentinel) noexcept {
    return iterator == sentinel.last;
  }

  friend constexpr bool operator==(unsized_sentinel sentinel,
                                   T *iterator) noexcept {
    return iterator == sentinel;
  }
};

template <class T> struct sized_sentinel {
  T *last = nullptr;

  friend constexpr bool operator==(T *iterator,
                                   sized_sentinel sentinel) noexcept {
    return iterator == sentinel.last;
  }

  friend constexpr bool operator==(sized_sentinel sentinel,
                                   T *iterator) noexcept {
    return iterator == sentinel;
  }

  friend constexpr tested::ptrdiff_t operator-(sized_sentinel sentinel,
                                               T *iterator) noexcept {
    return sentinel.last - iterator;
  }

  friend constexpr tested::ptrdiff_t
  operator-(T *iterator, sized_sentinel sentinel) noexcept {
    return iterator - sentinel.last;
  }
};

template <class T> struct unsized_range {
  T *first = nullptr;
  T *last = nullptr;

  constexpr unsized_range() = default;

  constexpr unsized_range(T *first, T *last) noexcept
      : first(first), last(last) {}

  constexpr T *begin() const noexcept { return first; }

  constexpr unsized_sentinel<T> end() const noexcept { return {last}; }
};

template <class T> struct sized_non_common_range {
  T *first = nullptr;
  T *last = nullptr;

  constexpr sized_non_common_range() = default;

  constexpr sized_non_common_range(T *first, T *last) noexcept
      : first(first), last(last) {}

  constexpr T *begin() const noexcept { return first; }

  constexpr sized_sentinel<T> end() const noexcept { return {last}; }
};

template <class T>
struct unsized_view : unsized_range<T>, tested::ranges::view_base {
  constexpr unsized_view() = default;

  constexpr unsized_view(T *first, T *last) noexcept
      : unsized_range<T>(first, last) {}
};

template <class T>
struct sized_non_common_view : sized_non_common_range<T>,
                               tested::ranges::view_base {
  constexpr sized_non_common_view() = default;

  constexpr sized_non_common_view(T *first, T *last) noexcept
      : sized_non_common_range<T>(first, last) {}
};

/*
 * This range would normally be sized through end() - begin(), but
 * explicitly opts out through disable_sized_range.
 */
struct disabled_sized_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() const noexcept { return first; }

  constexpr unsized_sentinel<int> end() const noexcept { return {last}; }

  constexpr tested::size_t size() const noexcept {
    return static_cast<tested::size_t>(last - first);
  }
};

#ifdef FTL_REPLACE_STL
namespace std::ranges {
template <>
inline constexpr bool disable_sized_range<::disabled_sized_range> = true;
}
#else
namespace ftl::ranges {
template <>
inline constexpr bool disable_sized_range<::disabled_sized_range> = true;
}
#endif

class input_iterator {
  int *current_ = nullptr;

public:
  using iterator_concept = tested::input_iterator_tag;

  using iterator_category = tested::input_iterator_tag;

  using value_type = int;

  using difference_type = tested::ptrdiff_t;

  constexpr input_iterator() = default;

  constexpr explicit input_iterator(int *current) noexcept
      : current_(current) {}

  constexpr int &operator*() const noexcept { return *current_; }

  constexpr input_iterator &operator++() noexcept {
    ++current_;
    return *this;
  }

  constexpr void operator++(int) noexcept { ++*this; }

  friend constexpr bool operator==(const input_iterator &,
                                   const input_iterator &) = default;
};

struct input_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr input_view() = default;

  constexpr input_view(int *first, int *last) noexcept
      : first(first), last(last) {}

  constexpr input_iterator begin() noexcept { return input_iterator(first); }

  constexpr input_iterator end() noexcept { return input_iterator(last); }
};

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
concept sized_mutable_and_const =
    tested::ranges::range<R> && tested::ranges::range<const R> &&
    tested::ranges::sized_range<R> && tested::ranges::sized_range<const R>;

template <class R>
concept correctly_unsized =
    tested::ranges::range<R> && !tested::ranges::sized_range<R>;

/*
 * Fundamental ranges.
 */

using sized_ref_view = tested::ranges::ref_view<int[5]>;

using sized_owning_view =
    tested::ranges::owning_view<sized_non_common_range<int>>;

using unsized_owning_view = tested::ranges::owning_view<unsized_range<int>>;

using unsized_subrange = tested::ranges::subrange<int *, unsized_sentinel<int>>;

static_assert(sized_mutable_and_const<sized_ref_view>);

static_assert(sized_mutable_and_const<sized_owning_view>);

static_assert(sized_mutable_and_const<tested::ranges::empty_view<int>>);

static_assert(sized_mutable_and_const<tested::ranges::single_view<int>>);

static_assert(sized_mutable_and_const<tested::ranges::iota_view<int, int>>);

static_assert(sized_mutable_and_const<tested::ranges::repeat_view<int, int>>);

static_assert(sized_mutable_and_const<tested::ranges::subrange<int *>>);

static_assert(correctly_unsized<unsized_range<int>>);

static_assert(correctly_unsized<unsized_owning_view>);

static_assert(correctly_unsized<unsized_subrange>);

static_assert(correctly_unsized<input_view>);

static_assert(correctly_unsized<tested::ranges::iota_view<int>>);

static_assert(correctly_unsized<tested::ranges::repeat_view<int>>);

static_assert(correctly_unsized<disabled_sized_range>);

static_assert(!tested::ranges::sized_range<const disabled_sized_range>);

/*
 * Adaptor propagation.
 */

constexpr bool sized_range_concepts_work() {
  int values[] = {1, 2, 3, 4, 5};

  int short_values[] = {10, 20, 30};

  tested::pair<int, int> pairs[] = {{1, 10}, {2, 20}, {3, 30}};

  sized_non_common_view<int> sized_non_common{values, values + 5};

  unsized_view<int> unsized{values, values + 5};

  unsized_view<tested::pair<int, int>> unsized_pairs{pairs, pairs + 3};

  input_view input{values, values + 5};

  /*
   * Views that preserve or calculate a size from sized inputs.
   */

  auto taken = values | tested::ranges::views::take(3);

  auto dropped = values | tested::ranges::views::drop(2);

  auto reversed = values | tested::ranges::views::reverse;

  auto transformed =
      values | tested::ranges::views::transform(identity_function{});

  auto common = tested::ranges::views::common(sized_non_common);

  auto elements = pairs | tested::ranges::views::elements<0>;

  auto constant = tested::ranges::views::as_const(values);

  auto strided = values | tested::ranges::views::stride(2);

  auto enumerated = tested::ranges::views::enumerate(values);

  auto adjacent = values | tested::ranges::views::adjacent<2>;

  auto adjacent_transformed =
      values | tested::ranges::views::adjacent_transform<2>(add_values{});

  auto slid = values | tested::ranges::views::slide(2);

  auto chunked = values | tested::ranges::views::chunk(2);

  auto zipped = tested::ranges::views::zip(values, short_values);

  auto zip_transformed =
      tested::ranges::views::zip_transform(add_values{}, values, short_values);

  auto product = tested::ranges::views::cartesian_product(values, short_values);

  static_assert(sized_mutable_and_const<decltype(taken)>);

  static_assert(sized_mutable_and_const<decltype(dropped)>);

  static_assert(sized_mutable_and_const<decltype(reversed)>);

  static_assert(sized_mutable_and_const<decltype(transformed)>);

  static_assert(sized_mutable_and_const<decltype(common)>);

  static_assert(sized_mutable_and_const<decltype(elements)>);

  static_assert(sized_mutable_and_const<decltype(constant)>);

  static_assert(sized_mutable_and_const<decltype(strided)>);

  static_assert(sized_mutable_and_const<decltype(enumerated)>);

  static_assert(sized_mutable_and_const<decltype(adjacent)>);

  static_assert(sized_mutable_and_const<decltype(adjacent_transformed)>);

  static_assert(sized_mutable_and_const<decltype(slid)>);

  static_assert(sized_mutable_and_const<decltype(chunked)>);

  static_assert(sized_mutable_and_const<decltype(zipped)>);

  static_assert(sized_mutable_and_const<decltype(zip_transformed)>);

  static_assert(sized_mutable_and_const<decltype(product)>);

  /*
   * drop_while has no size() member, but a common random-access
   * base permits ranges::size to use end() - begin().
   */

  auto synthesized_drop_while =
      values | tested::ranges::views::drop_while(less_than_three{});

  static_assert(tested::ranges::sized_range<decltype(synthesized_drop_while)>);

  /*
   * Truly unsized inputs must not acquire a size through adaptors
   * whose iterator/sentinel pairs cannot determine a distance.
   */

  auto unsized_take = unsized | tested::ranges::views::take(2);

  auto unsized_drop = unsized | tested::ranges::views::drop(2);

  auto unsized_transform =
      unsized | tested::ranges::views::transform(identity_function{});

  auto unsized_common = tested::ranges::views::common(unsized);

  auto unsized_elements = unsized_pairs | tested::ranges::views::elements<0>;

  auto unsized_as_const = tested::ranges::views::as_const(unsized);

  auto unsized_stride = unsized | tested::ranges::views::stride(2);

  auto unsized_enumerate = tested::ranges::views::enumerate(unsized);

  auto unsized_adjacent = unsized | tested::ranges::views::adjacent<2>;

  auto unsized_adjacent_transform =
      unsized | tested::ranges::views::adjacent_transform<2>(add_values{});

  auto unsized_chunk = unsized | tested::ranges::views::chunk(2);

  auto unsized_filter =
      unsized | tested::ranges::views::filter(less_than_three{});

  auto unsized_take_while =
      unsized | tested::ranges::views::take_while(less_than_three{});

  auto unsized_drop_while =
      unsized | tested::ranges::views::drop_while(less_than_three{});

  auto unsized_split = unsized | tested::ranges::views::split(3);

  auto unsized_lazy_split = unsized | tested::ranges::views::lazy_split(3);

  auto unsized_chunk_by =
      unsized | tested::ranges::views::chunk_by(equal_adjacent{});

  auto unsized_zip = tested::ranges::views::zip(values, unsized);

  auto unsized_zip_transform =
      tested::ranges::views::zip_transform(add_values{}, values, unsized);

  auto unsized_product =
      tested::ranges::views::cartesian_product(values, unsized);

  unsized_view<int> inner_ranges[] = {{values, values + 2},
                                      {values + 2, values + 5}};

  auto unsized_join = inner_ranges | tested::ranges::views::join;

  auto input_chunk = input | tested::ranges::views::chunk(2);

  static_assert(correctly_unsized<decltype(unsized_take)>);

  static_assert(correctly_unsized<decltype(unsized_drop)>);

  static_assert(correctly_unsized<decltype(unsized_transform)>);

  static_assert(correctly_unsized<decltype(unsized_common)>);

  static_assert(correctly_unsized<decltype(unsized_elements)>);

  static_assert(correctly_unsized<decltype(unsized_as_const)>);

  static_assert(correctly_unsized<decltype(unsized_stride)>);

  static_assert(correctly_unsized<decltype(unsized_enumerate)>);

  static_assert(correctly_unsized<decltype(unsized_adjacent)>);

  static_assert(correctly_unsized<decltype(unsized_adjacent_transform)>);

  static_assert(correctly_unsized<decltype(unsized_chunk)>);

  static_assert(correctly_unsized<decltype(unsized_filter)>);

  static_assert(correctly_unsized<decltype(unsized_take_while)>);

  static_assert(correctly_unsized<decltype(unsized_drop_while)>);

  static_assert(correctly_unsized<decltype(unsized_split)>);

  static_assert(correctly_unsized<decltype(unsized_lazy_split)>);

  static_assert(correctly_unsized<decltype(unsized_chunk_by)>);

  static_assert(correctly_unsized<decltype(unsized_zip)>);

  static_assert(correctly_unsized<decltype(unsized_zip_transform)>);

  static_assert(correctly_unsized<decltype(unsized_product)>);

  static_assert(correctly_unsized<decltype(unsized_join)>);

  static_assert(correctly_unsized<decltype(input_chunk)>);

  return true;
}

template <class R> constexpr bool size_is(R &range, tested::size_t expected) {
  if constexpr (!tested::ranges::sized_range<R> ||
                !tested::ranges::sized_range<const R>) {
    return false;
  } else {
    const auto &constant = range;

    return static_cast<tested::size_t>(tested::ranges::size(range)) ==
               expected &&
           static_cast<tested::size_t>(tested::ranges::size(constant)) ==
               expected;
  }
}

/*
 * Verify the actual size calculations, not only concept membership.
 */

constexpr bool sized_range_values_work() {
  int values[] = {1, 2, 3, 4, 5};

  int short_values[] = {10, 20, 30};

  tested::pair<int, int> pairs[] = {{1, 10}, {2, 20}, {3, 30}};

  tested::ranges::ref_view<int[5]> referenced(values);

  if (!size_is(referenced, 5))
    return false;

  tested::ranges::owning_view<sized_non_common_range<int>> owned(
      sized_non_common_range<int>{values, values + 5});

  if (!size_is(owned, 5))
    return false;

  tested::ranges::empty_view<int> empty;

  if (!size_is(empty, 0))
    return false;

  auto single = tested::ranges::views::single(7);

  if (!size_is(single, 1))
    return false;

  auto iota = tested::ranges::views::iota(2, 7);

  if (!size_is(iota, 5))
    return false;

  auto empty_iota = tested::ranges::views::iota(7, 7);

  if (!size_is(empty_iota, 0))
    return false;

  auto repeated = tested::ranges::views::repeat(9, 4);

  if (!size_is(repeated, 4))
    return false;

  auto empty_repeat = tested::ranges::views::repeat(9, 0);

  if (!size_is(empty_repeat, 0))
    return false;

  auto subrange = tested::ranges::subrange(values, values + 5);

  if (!size_is(subrange, 5))
    return false;

  auto counted = tested::ranges::views::counted(values + 0, 4);

  if (!size_is(counted, 4))
    return false;

  auto take_three = values | tested::ranges::views::take(3);

  auto take_too_many = values | tested::ranges::views::take(8);

  auto take_none = values | tested::ranges::views::take(0);

  if (!size_is(take_three, 3))
    return false;

  if (!size_is(take_too_many, 5))
    return false;

  if (!size_is(take_none, 0))
    return false;

  auto drop_two = values | tested::ranges::views::drop(2);

  auto drop_too_many = values | tested::ranges::views::drop(8);

  if (!size_is(drop_two, 3))
    return false;

  if (!size_is(drop_too_many, 0))
    return false;

  auto reversed = values | tested::ranges::views::reverse;

  if (!size_is(reversed, 5))
    return false;

  auto transformed =
      values | tested::ranges::views::transform(identity_function{});

  if (!size_is(transformed, 5))
    return false;

  sized_non_common_view<int> non_common{values, values + 5};

  auto common = tested::ranges::views::common(non_common);

  if (!size_is(common, 5))
    return false;

  auto elements = pairs | tested::ranges::views::elements<0>;

  if (!size_is(elements, 3))
    return false;

  auto constant = tested::ranges::views::as_const(values);

  if (!size_is(constant, 5))
    return false;

  auto stride_two = values | tested::ranges::views::stride(2);

  auto stride_six = values | tested::ranges::views::stride(6);

  if (!size_is(stride_two, 3))
    return false;

  if (!size_is(stride_six, 1))
    return false;

  auto enumerated = tested::ranges::views::enumerate(values);

  if (!size_is(enumerated, 5))
    return false;

  auto adjacent_two = values | tested::ranges::views::adjacent<2>;

  auto adjacent_six = values | tested::ranges::views::adjacent<6>;

  if (!size_is(adjacent_two, 4))
    return false;

  if (!size_is(adjacent_six, 0))
    return false;

  auto adjacent_transformed =
      values | tested::ranges::views::adjacent_transform<2>(add_values{});

  if (!size_is(adjacent_transformed, 4)) {
    return false;
  }

  auto slide_two = values | tested::ranges::views::slide(2);

  auto slide_six = values | tested::ranges::views::slide(6);

  if (!size_is(slide_two, 4))
    return false;

  if (!size_is(slide_six, 0))
    return false;

  auto chunk_two = values | tested::ranges::views::chunk(2);

  auto chunk_six = values | tested::ranges::views::chunk(6);

  if (!size_is(chunk_two, 3))
    return false;

  if (!size_is(chunk_six, 1))
    return false;

  auto zipped = tested::ranges::views::zip(values, short_values);

  if (!size_is(zipped, 3))
    return false;

  auto empty_zip = tested::ranges::views::zip(values, empty);

  if (!size_is(empty_zip, 0))
    return false;

  auto zip_transformed =
      tested::ranges::views::zip_transform(add_values{}, values, short_values);

  if (!size_is(zip_transformed, 3))
    return false;

  auto empty_zip_transform =
      tested::ranges::views::zip_transform(add_values{}, values, empty);

  if (!size_is(empty_zip_transform, 0)) {
    return false;
  }

  auto product = tested::ranges::views::cartesian_product(values, short_values);

  if (!size_is(product, 15))
    return false;

  auto zero_product = tested::ranges::views::cartesian_product(values, empty);

  if (!size_is(zero_product, 0))
    return false;

  auto empty_product = tested::ranges::views::cartesian_product();

  if (!size_is(empty_product, 1))
    return false;

  return true;
}

/*
 * This exercises the end() - begin() ranges::size fallback at runtime.
 * Keeping it out of the constexpr aggregate avoids the normal-mode
 * constexpr construct_at portability boundary in drop_while's cache.
 */

bool synthesized_size_values_work() {
  int values[] = {1, 2, 3, 4, 5};

  auto dropped = values | tested::ranges::views::drop_while(less_than_three{});

  return tested::ranges::size(dropped) == 3;
}

static_assert(sized_range_concepts_work());

static_assert(sized_range_values_work());

bool ftl_test() {
  return sized_range_concepts_work() && sized_range_values_work() &&
         synthesized_size_values_work();
}