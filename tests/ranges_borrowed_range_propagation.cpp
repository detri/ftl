#ifdef FTL_REPLACE_STL
#include <concepts>
#include <cstddef>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/concepts>
#include <ftl/cstddef>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct pointer_sentinel {
  int *end = nullptr;

  friend constexpr bool operator==(int *iterator,
                                   pointer_sentinel sentinel) noexcept {
    return iterator == sentinel.end;
  }

  friend constexpr bool operator==(pointer_sentinel sentinel,
                                   int *iterator) noexcept {
    return iterator == sentinel;
  }
};

struct non_common_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept { return first; }

  constexpr pointer_sentinel end() noexcept { return {last}; }
};

struct input_sentinel {
  int *end = nullptr;
};

class input_iterator {
  int *current_ = nullptr;

public:
  using iterator_concept = tested::input_iterator_tag;
  using iterator_category = tested::input_iterator_tag;
  using value_type = int;
  using difference_type = tested::ptrdiff_t;
  using pointer = int *;
  using reference = int &;

  constexpr input_iterator() = default;

  constexpr explicit input_iterator(int *current) noexcept
      : current_(current) {}

  constexpr int *base() const noexcept { return current_; }

  constexpr int &operator*() const noexcept { return *current_; }

  constexpr input_iterator &operator++() noexcept {
    ++current_;
    return *this;
  }

  constexpr void operator++(int) noexcept { ++*this; }
};

constexpr bool operator==(const input_iterator &iterator,
                          input_sentinel sentinel) noexcept {
  return iterator.base() == sentinel.end;
}

constexpr bool operator==(input_sentinel sentinel,
                          const input_iterator &iterator) noexcept {
  return iterator == sentinel;
}

struct unary_predicate {
  constexpr bool operator()(int value) const noexcept { return value != 0; }
};

struct binary_predicate {
  constexpr bool operator()(int left, int right) const noexcept {
    return left == right;
  }
};

struct transform_function {
  constexpr int operator()(int value) const noexcept { return value; }
};

using borrowed_base = tested::ranges::ref_view<int[4]>;

using non_borrowed_base = tested::ranges::single_view<int>;

using borrowed_non_common_base =
    tested::ranges::subrange<int *, pointer_sentinel>;

using non_borrowed_non_common_base =
    tested::ranges::owning_view<non_common_range>;

using borrowed_input_base =
    tested::ranges::subrange<input_iterator, input_sentinel>;

using borrowed_pair_base = tested::ranges::ref_view<tested::pair<int, int>[2]>;

using non_borrowed_pair_base =
    tested::ranges::single_view<tested::pair<int, int>>;

template <class R>
concept correctly_borrowed =
    tested::ranges::range<R> && tested::ranges::borrowed_range<R> &&
    tested::same_as<tested::ranges::borrowed_iterator_t<R>,
                    tested::ranges::iterator_t<R>>;

template <class R>
concept correctly_non_borrowed =
    tested::ranges::range<R> && !tested::ranges::borrowed_range<R> &&
    tested::same_as<tested::ranges::borrowed_iterator_t<R>,
                    tested::ranges::dangling>;

/*
 * Intrinsically borrowed ranges.
 */

static_assert(correctly_borrowed<tested::ranges::subrange<int *>>);

static_assert(correctly_borrowed<tested::ranges::ref_view<int[4]>>);

static_assert(correctly_borrowed<tested::ranges::empty_view<int>>);

static_assert(correctly_borrowed<tested::ranges::iota_view<int>>);

/*
 * owning_view propagates the underlying setting.
 */

using borrowed_owning = tested::ranges::owning_view<borrowed_non_common_base>;

static_assert(correctly_borrowed<borrowed_owning>);
static_assert(correctly_non_borrowed<non_borrowed_non_common_base>);

/*
 * Unary views that propagate borrowed-range status.
 */

using borrowed_take = tested::ranges::take_view<borrowed_base>;

using non_borrowed_take = tested::ranges::take_view<non_borrowed_base>;

static_assert(correctly_borrowed<borrowed_take>);
static_assert(correctly_non_borrowed<non_borrowed_take>);

using borrowed_drop = tested::ranges::drop_view<borrowed_base>;

using non_borrowed_drop = tested::ranges::drop_view<non_borrowed_base>;

static_assert(correctly_borrowed<borrowed_drop>);
static_assert(correctly_non_borrowed<non_borrowed_drop>);

using borrowed_drop_while =
    tested::ranges::drop_while_view<borrowed_base, unary_predicate>;

using non_borrowed_drop_while =
    tested::ranges::drop_while_view<non_borrowed_base, unary_predicate>;

static_assert(correctly_borrowed<borrowed_drop_while>);
static_assert(correctly_non_borrowed<non_borrowed_drop_while>);

using borrowed_reverse = tested::ranges::reverse_view<borrowed_base>;

using non_borrowed_reverse = tested::ranges::reverse_view<non_borrowed_base>;

static_assert(correctly_borrowed<borrowed_reverse>);
static_assert(correctly_non_borrowed<non_borrowed_reverse>);

using borrowed_common = tested::ranges::common_view<borrowed_non_common_base>;

using non_borrowed_common =
    tested::ranges::common_view<non_borrowed_non_common_base>;

static_assert(correctly_borrowed<borrowed_common>);
static_assert(correctly_non_borrowed<non_borrowed_common>);

using borrowed_elements = tested::ranges::elements_view<borrowed_pair_base, 0>;

using non_borrowed_elements =
    tested::ranges::elements_view<non_borrowed_pair_base, 0>;

static_assert(correctly_borrowed<borrowed_elements>);
static_assert(correctly_non_borrowed<non_borrowed_elements>);

using borrowed_as_const = tested::ranges::as_const_view<borrowed_base>;

using non_borrowed_as_const = tested::ranges::as_const_view<non_borrowed_base>;

static_assert(correctly_borrowed<borrowed_as_const>);
static_assert(correctly_non_borrowed<non_borrowed_as_const>);

using borrowed_stride = tested::ranges::stride_view<borrowed_base>;

using non_borrowed_stride = tested::ranges::stride_view<non_borrowed_base>;

static_assert(correctly_borrowed<borrowed_stride>);
static_assert(correctly_non_borrowed<non_borrowed_stride>);

using borrowed_enumerate = tested::ranges::enumerate_view<borrowed_base>;

using non_borrowed_enumerate =
    tested::ranges::enumerate_view<non_borrowed_base>;

static_assert(correctly_borrowed<borrowed_enumerate>);
static_assert(correctly_non_borrowed<non_borrowed_enumerate>);

using borrowed_adjacent = tested::ranges::adjacent_view<borrowed_base, 2>;

using non_borrowed_adjacent =
    tested::ranges::adjacent_view<non_borrowed_base, 2>;

static_assert(correctly_borrowed<borrowed_adjacent>);
static_assert(correctly_non_borrowed<non_borrowed_adjacent>);

using borrowed_slide = tested::ranges::slide_view<borrowed_base>;

using non_borrowed_slide = tested::ranges::slide_view<non_borrowed_base>;

static_assert(correctly_borrowed<borrowed_slide>);
static_assert(correctly_non_borrowed<non_borrowed_slide>);

/*
 * Forward chunk_view propagates, input chunk_view never does.
 */

using borrowed_chunk = tested::ranges::chunk_view<borrowed_base>;

using non_borrowed_chunk = tested::ranges::chunk_view<non_borrowed_base>;

using borrowed_input_chunk = tested::ranges::chunk_view<borrowed_input_base>;

static_assert(tested::ranges::input_range<borrowed_input_base>);
static_assert(!tested::ranges::forward_range<borrowed_input_base>);

static_assert(correctly_borrowed<borrowed_chunk>);
static_assert(correctly_non_borrowed<non_borrowed_chunk>);
static_assert(correctly_non_borrowed<borrowed_input_chunk>);

/*
 * Multi-range views propagate only when every underlying view does.
 */

using borrowed_zip = tested::ranges::zip_view<borrowed_base, borrowed_base>;

using non_borrowed_zip =
    tested::ranges::zip_view<borrowed_base, non_borrowed_base>;

static_assert(correctly_borrowed<borrowed_zip>);
static_assert(correctly_non_borrowed<non_borrowed_zip>);

/*
 * These iterators depend on state owned by the view and must not be
 * borrowed, even when their underlying ranges are borrowed.
 */

using transform_type =
    tested::ranges::transform_view<borrowed_base, transform_function>;

using filter_type = tested::ranges::filter_view<borrowed_base, unary_predicate>;

using take_while_type =
    tested::ranges::take_while_view<borrowed_base, unary_predicate>;

using split_type =
    tested::ranges::split_view<borrowed_base, tested::ranges::single_view<int>>;

using lazy_split_type =
    tested::ranges::lazy_split_view<borrowed_base,
                                    tested::ranges::single_view<int>>;

using join_outer = tested::ranges::subrange<borrowed_base *>;

using join_type = tested::ranges::join_view<join_outer>;

using chunk_by_type =
    tested::ranges::chunk_by_view<borrowed_base, binary_predicate>;

using cartesian_type =
    tested::ranges::cartesian_product_view<borrowed_base, borrowed_base>;

static_assert(correctly_non_borrowed<tested::ranges::single_view<int>>);

static_assert(correctly_non_borrowed<tested::ranges::repeat_view<int>>);

static_assert(correctly_non_borrowed<transform_type>);
static_assert(correctly_non_borrowed<filter_type>);
static_assert(correctly_non_borrowed<take_while_type>);
static_assert(correctly_non_borrowed<split_type>);
static_assert(correctly_non_borrowed<lazy_split_type>);
static_assert(correctly_non_borrowed<join_type>);
static_assert(correctly_non_borrowed<chunk_by_type>);
static_assert(correctly_non_borrowed<cartesian_type>);

/*
 * Every range lvalue is borrowed, regardless of its prvalue setting.
 */

static_assert(correctly_borrowed<non_borrowed_base &>);
static_assert(correctly_borrowed<transform_type &>);

bool ftl_test() { return true; }