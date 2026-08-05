#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <iterator>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/tuple>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

template <bool IsNoexcept> struct member_access_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept(IsNoexcept) { return first; }

  constexpr const int *begin() const noexcept(IsNoexcept) { return first; }

  constexpr int *end() noexcept(IsNoexcept) { return last; }

  constexpr const int *end() const noexcept(IsNoexcept) { return last; }

  constexpr tested::reverse_iterator<int *> rbegin() noexcept(IsNoexcept) {
    return tested::reverse_iterator<int *>(last);
  }

  constexpr tested::reverse_iterator<const int *> rbegin() const
      noexcept(IsNoexcept) {
    return tested::reverse_iterator<const int *>(last);
  }

  constexpr tested::reverse_iterator<int *> rend() noexcept(IsNoexcept) {
    return tested::reverse_iterator<int *>(first);
  }

  constexpr tested::reverse_iterator<const int *> rend() const
      noexcept(IsNoexcept) {
    return tested::reverse_iterator<const int *>(first);
  }

  constexpr tested::size_t size() const noexcept(IsNoexcept) {
    return static_cast<tested::size_t>(last - first);
  }

  constexpr bool empty() const noexcept(IsNoexcept) { return first == last; }

  constexpr int *data() noexcept(IsNoexcept) { return first; }

  constexpr const int *data() const noexcept(IsNoexcept) { return first; }
};

namespace adl_access {

template <bool IsNoexcept> struct range {
  int *first = nullptr;
  int *last = nullptr;
};

template <bool IsNoexcept>
constexpr int *begin(range<IsNoexcept> &value) noexcept(IsNoexcept) {
  return value.first;
}

template <bool IsNoexcept>
constexpr const int *
begin(const range<IsNoexcept> &value) noexcept(IsNoexcept) {
  return value.first;
}

template <bool IsNoexcept>
constexpr int *end(range<IsNoexcept> &value) noexcept(IsNoexcept) {
  return value.last;
}

template <bool IsNoexcept>
constexpr const int *end(const range<IsNoexcept> &value) noexcept(IsNoexcept) {
  return value.last;
}

template <bool IsNoexcept>
constexpr tested::reverse_iterator<int *>
rbegin(range<IsNoexcept> &value) noexcept(IsNoexcept) {
  return tested::reverse_iterator<int *>(value.last);
}

template <bool IsNoexcept>
constexpr tested::reverse_iterator<const int *>
rbegin(const range<IsNoexcept> &value) noexcept(IsNoexcept) {
  return tested::reverse_iterator<const int *>(value.last);
}

template <bool IsNoexcept>
constexpr tested::reverse_iterator<int *>
rend(range<IsNoexcept> &value) noexcept(IsNoexcept) {
  return tested::reverse_iterator<int *>(value.first);
}

template <bool IsNoexcept>
constexpr tested::reverse_iterator<const int *>
rend(const range<IsNoexcept> &value) noexcept(IsNoexcept) {
  return tested::reverse_iterator<const int *>(value.first);
}

template <bool IsNoexcept>
constexpr tested::size_t
size(const range<IsNoexcept> &value) noexcept(IsNoexcept) {
  return static_cast<tested::size_t>(value.last - value.first);
}

} // namespace adl_access

template <bool BeginNoexcept, bool EndNoexcept> struct fallback_access_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept(BeginNoexcept) { return first; }

  constexpr int *begin() const noexcept(BeginNoexcept) { return first; }

  constexpr int *end() noexcept(EndNoexcept) { return last; }

  constexpr int *end() const noexcept(EndNoexcept) { return last; }
};

template <class Iterator, bool CompareNoexcept> struct unsized_sentinel {
  Iterator last{};

  friend constexpr bool
  operator==(const Iterator &iterator,
             const unsized_sentinel &sentinel) noexcept(CompareNoexcept) {
    return iterator == sentinel.last;
  }

  friend constexpr bool
  operator==(const unsized_sentinel &sentinel,
             const Iterator &iterator) noexcept(CompareNoexcept) {
    return iterator == sentinel;
  }
};

template <bool BeginNoexcept, bool EndNoexcept, bool CompareNoexcept>
struct iterator_empty_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept(BeginNoexcept) { return first; }

  constexpr int *begin() const noexcept(BeginNoexcept) { return first; }

  constexpr auto end() noexcept(EndNoexcept) {
    return unsized_sentinel<int *, CompareNoexcept>{last};
  }

  constexpr auto end() const noexcept(EndNoexcept) {
    return unsized_sentinel<int *, CompareNoexcept>{last};
  }
};

template <bool SubtractionNoexcept> struct subtraction_sentinel {
  int *last = nullptr;

  friend constexpr bool operator==(int *iterator,
                                   subtraction_sentinel sentinel) noexcept {
    return iterator == sentinel.last;
  }

  friend constexpr bool operator==(subtraction_sentinel sentinel,
                                   int *iterator) noexcept {
    return iterator == sentinel;
  }

  friend constexpr tested::ptrdiff_t
  operator-(subtraction_sentinel sentinel,
            int *iterator) noexcept(SubtractionNoexcept) {
    return sentinel.last - iterator;
  }

  friend constexpr tested::ptrdiff_t
  operator-(int *iterator,
            subtraction_sentinel sentinel) noexcept(SubtractionNoexcept) {
    return iterator - sentinel.last;
  }
};

template <bool BeginNoexcept, bool EndNoexcept, bool SubtractionNoexcept>
struct subtraction_size_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept(BeginNoexcept) { return first; }

  constexpr int *begin() const noexcept(BeginNoexcept) { return first; }

  constexpr auto end() noexcept(EndNoexcept) {
    return subtraction_sentinel<SubtractionNoexcept>{last};
  }

  constexpr auto end() const noexcept(EndNoexcept) {
    return subtraction_sentinel<SubtractionNoexcept>{last};
  }
};

template <bool MoveNoexcept> struct movable_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr movable_view() = default;

  movable_view(const movable_view &) = delete;

  movable_view &operator=(const movable_view &) = delete;

  constexpr movable_view(movable_view &&other) noexcept(MoveNoexcept)
      : first(other.first), last(other.last) {
    other.first = nullptr;
    other.last = nullptr;
  }

  constexpr movable_view &
  operator=(movable_view &&other) noexcept(MoveNoexcept) {
    first = other.first;
    last = other.last;

    other.first = nullptr;
    other.last = nullptr;

    return *this;
  }

  constexpr int *begin() noexcept { return first; }

  constexpr int *end() noexcept { return last; }
};

template <bool MoveNoexcept> struct movable_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr movable_range() = default;

  movable_range(const movable_range &) = delete;

  movable_range &operator=(const movable_range &) = delete;

  constexpr movable_range(movable_range &&other) noexcept(MoveNoexcept)
      : first(other.first), last(other.last) {
    other.first = nullptr;
    other.last = nullptr;
  }

  constexpr movable_range &
  operator=(movable_range &&other) noexcept(MoveNoexcept) {
    first = other.first;
    last = other.last;

    other.first = nullptr;
    other.last = nullptr;

    return *this;
  }

  constexpr int *begin() noexcept { return first; }

  constexpr int *end() noexcept { return last; }
};

template <bool MoveNoexcept> struct movable_value {
  constexpr movable_value() = default;

  movable_value(const movable_value &) = delete;

  movable_value &operator=(const movable_value &) = delete;

  constexpr movable_value(movable_value &&) noexcept(MoveNoexcept) {}

  constexpr movable_value &operator=(movable_value &&) noexcept(MoveNoexcept) {
    return *this;
  }
};

struct pointer_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept { return first; }

  constexpr int *begin() const noexcept { return first; }

  constexpr int *end() noexcept { return last; }

  constexpr int *end() const noexcept { return last; }
};

struct nothrow_unary {
  constexpr int operator()(int &value) const noexcept { return value; }
};

struct throwing_unary {
  constexpr int operator()(int &value) const noexcept(false) { return value; }
};

struct nothrow_binary {
  constexpr int operator()(int &left, int &right) const noexcept {
    return left + right;
  }
};

struct throwing_binary {
  constexpr int operator()(int &left, int &right) const noexcept(false) {
    return left + right;
  }
};

template <bool CopyNoexcept> struct counter {
  using difference_type = tested::ptrdiff_t;

  int value = 0;

  constexpr counter() = default;

  constexpr explicit counter(int value) noexcept : value(value) {}

  constexpr counter(const counter &other) noexcept(CopyNoexcept)
      : value(other.value) {}

  constexpr counter(counter &&other) noexcept : value(other.value) {}

  constexpr counter &operator=(const counter &other) noexcept(CopyNoexcept) {
    value = other.value;
    return *this;
  }

  constexpr counter &operator=(counter &&other) noexcept {
    value = other.value;
    return *this;
  }

  constexpr counter &operator++() noexcept {
    ++value;
    return *this;
  }

  constexpr counter operator++(int) {
    auto previous = *this;
    ++*this;
    return previous;
  }

  friend constexpr bool operator==(const counter &, const counter &) = default;
};

template <bool MoveNoexcept, bool SwapNoexcept> class audit_iterator {
  int *current_ = nullptr;

public:
  using iterator_concept = tested::forward_iterator_tag;

  using iterator_category = tested::forward_iterator_tag;

  using value_type = int;

  using difference_type = tested::ptrdiff_t;

  using pointer = int *;
  using reference = int &;

  constexpr audit_iterator() = default;

  constexpr explicit audit_iterator(int *current) noexcept
      : current_(current) {}

  constexpr int &operator*() const noexcept { return *current_; }

  constexpr audit_iterator &operator++() noexcept {
    ++current_;
    return *this;
  }

  constexpr audit_iterator operator++(int) noexcept {
    auto previous = *this;
    ++*this;
    return previous;
  }

  friend constexpr bool operator==(const audit_iterator &,
                                   const audit_iterator &) = default;

  friend constexpr int &&
  iter_move(const audit_iterator &iterator) noexcept(MoveNoexcept) {
    return static_cast<int &&>(*iterator.current_);
  }

  friend constexpr void
  iter_swap(const audit_iterator &left,
            const audit_iterator &right) noexcept(SwapNoexcept) {
    const int temporary = *left.current_;

    *left.current_ = *right.current_;

    *right.current_ = temporary;
  }
};

template <bool MoveNoexcept, bool SwapNoexcept>
struct audit_view : tested::ranges::view_base {
  using iterator = audit_iterator<MoveNoexcept, SwapNoexcept>;

  int *first = nullptr;
  int *last = nullptr;

  constexpr iterator begin() noexcept { return iterator(first); }

  constexpr iterator begin() const noexcept { return iterator(first); }

  constexpr iterator end() noexcept { return iterator(last); }

  constexpr iterator end() const noexcept { return iterator(last); }
};

/*
 * Range access CPOs: arrays.
 */

using array_type = int[4];

static_assert(noexcept(tested::ranges::begin(tested::declval<array_type &>())));

static_assert(noexcept(tested::ranges::end(tested::declval<array_type &>())));

static_assert(
    noexcept(tested::ranges::cbegin(tested::declval<array_type &>())));

static_assert(noexcept(tested::ranges::cend(tested::declval<array_type &>())));

static_assert(noexcept(tested::ranges::size(tested::declval<array_type &>())));

static_assert(noexcept(tested::ranges::ssize(tested::declval<array_type &>())));

static_assert(noexcept(tested::ranges::empty(tested::declval<array_type &>())));

static_assert(noexcept(tested::ranges::data(tested::declval<array_type &>())));

static_assert(noexcept(tested::ranges::cdata(tested::declval<array_type &>())));

/*
 * Member customization paths.
 */

using noexcept_member_range = member_access_range<true>;

using throwing_member_range = member_access_range<false>;

static_assert(noexcept(
    tested::ranges::begin(tested::declval<noexcept_member_range &>())));

static_assert(!noexcept(
    tested::ranges::begin(tested::declval<throwing_member_range &>())));

static_assert(
    noexcept(tested::ranges::end(tested::declval<noexcept_member_range &>())));

static_assert(
    !noexcept(tested::ranges::end(tested::declval<throwing_member_range &>())));

static_assert(noexcept(
    tested::ranges::cbegin(tested::declval<noexcept_member_range &>())));

static_assert(!noexcept(
    tested::ranges::cbegin(tested::declval<throwing_member_range &>())));

static_assert(
    noexcept(tested::ranges::cend(tested::declval<noexcept_member_range &>())));

static_assert(!noexcept(
    tested::ranges::cend(tested::declval<throwing_member_range &>())));

static_assert(noexcept(
    tested::ranges::rbegin(tested::declval<noexcept_member_range &>())));

static_assert(!noexcept(
    tested::ranges::rbegin(tested::declval<throwing_member_range &>())));

static_assert(
    noexcept(tested::ranges::rend(tested::declval<noexcept_member_range &>())));

static_assert(!noexcept(
    tested::ranges::rend(tested::declval<throwing_member_range &>())));

static_assert(noexcept(
    tested::ranges::crbegin(tested::declval<noexcept_member_range &>())));

static_assert(!noexcept(
    tested::ranges::crbegin(tested::declval<throwing_member_range &>())));

static_assert(noexcept(
    tested::ranges::crend(tested::declval<noexcept_member_range &>())));

static_assert(!noexcept(
    tested::ranges::crend(tested::declval<throwing_member_range &>())));

static_assert(
    noexcept(tested::ranges::size(tested::declval<noexcept_member_range &>())));

static_assert(!noexcept(
    tested::ranges::size(tested::declval<throwing_member_range &>())));

static_assert(noexcept(
    tested::ranges::ssize(tested::declval<noexcept_member_range &>())));

static_assert(!noexcept(
    tested::ranges::ssize(tested::declval<throwing_member_range &>())));

static_assert(noexcept(
    tested::ranges::empty(tested::declval<noexcept_member_range &>())));

static_assert(!noexcept(
    tested::ranges::empty(tested::declval<throwing_member_range &>())));

static_assert(
    noexcept(tested::ranges::data(tested::declval<noexcept_member_range &>())));

static_assert(!noexcept(
    tested::ranges::data(tested::declval<throwing_member_range &>())));

static_assert(noexcept(
    tested::ranges::cdata(tested::declval<noexcept_member_range &>())));

static_assert(!noexcept(
    tested::ranges::cdata(tested::declval<throwing_member_range &>())));

/*
 * ADL customization paths.
 */

using noexcept_adl_range = adl_access::range<true>;

using throwing_adl_range = adl_access::range<false>;

static_assert(
    noexcept(tested::ranges::begin(tested::declval<noexcept_adl_range &>())));

static_assert(
    !noexcept(tested::ranges::begin(tested::declval<throwing_adl_range &>())));

static_assert(
    noexcept(tested::ranges::end(tested::declval<noexcept_adl_range &>())));

static_assert(
    !noexcept(tested::ranges::end(tested::declval<throwing_adl_range &>())));

static_assert(
    noexcept(tested::ranges::cbegin(tested::declval<noexcept_adl_range &>())));

static_assert(
    !noexcept(tested::ranges::cbegin(tested::declval<throwing_adl_range &>())));

static_assert(
    noexcept(tested::ranges::cend(tested::declval<noexcept_adl_range &>())));

static_assert(
    !noexcept(tested::ranges::cend(tested::declval<throwing_adl_range &>())));

static_assert(
    noexcept(tested::ranges::rbegin(tested::declval<noexcept_adl_range &>())));

static_assert(
    !noexcept(tested::ranges::rbegin(tested::declval<throwing_adl_range &>())));

static_assert(
    noexcept(tested::ranges::rend(tested::declval<noexcept_adl_range &>())));

static_assert(
    !noexcept(tested::ranges::rend(tested::declval<throwing_adl_range &>())));

static_assert(
    noexcept(tested::ranges::crbegin(tested::declval<noexcept_adl_range &>())));

static_assert(!noexcept(
    tested::ranges::crbegin(tested::declval<throwing_adl_range &>())));

static_assert(
    noexcept(tested::ranges::crend(tested::declval<noexcept_adl_range &>())));

static_assert(
    !noexcept(tested::ranges::crend(tested::declval<throwing_adl_range &>())));

static_assert(
    noexcept(tested::ranges::size(tested::declval<noexcept_adl_range &>())));

static_assert(
    !noexcept(tested::ranges::size(tested::declval<throwing_adl_range &>())));

static_assert(
    noexcept(tested::ranges::ssize(tested::declval<noexcept_adl_range &>())));

static_assert(
    !noexcept(tested::ranges::ssize(tested::declval<throwing_adl_range &>())));

static_assert(
    noexcept(tested::ranges::empty(tested::declval<noexcept_adl_range &>())));

static_assert(
    !noexcept(tested::ranges::empty(tested::declval<throwing_adl_range &>())));

static_assert(
    noexcept(tested::ranges::data(tested::declval<noexcept_adl_range &>())));

static_assert(
    !noexcept(tested::ranges::data(tested::declval<throwing_adl_range &>())));

static_assert(
    noexcept(tested::ranges::cdata(tested::declval<noexcept_adl_range &>())));

static_assert(
    !noexcept(tested::ranges::cdata(tested::declval<throwing_adl_range &>())));

/*
 * Fallback paths.
 */

using noexcept_fallback_range = fallback_access_range<true, true>;

using throwing_begin_fallback_range = fallback_access_range<false, true>;

using throwing_end_fallback_range = fallback_access_range<true, false>;

static_assert(noexcept(
    tested::ranges::size(tested::declval<noexcept_fallback_range &>())));

static_assert(!noexcept(
    tested::ranges::size(tested::declval<throwing_begin_fallback_range &>())));

static_assert(!noexcept(
    tested::ranges::size(tested::declval<throwing_end_fallback_range &>())));

static_assert(noexcept(
    tested::ranges::empty(tested::declval<noexcept_fallback_range &>())));

static_assert(!noexcept(
    tested::ranges::empty(tested::declval<throwing_begin_fallback_range &>())));

static_assert(!noexcept(
    tested::ranges::empty(tested::declval<throwing_end_fallback_range &>())));

static_assert(noexcept(
    tested::ranges::data(tested::declval<noexcept_fallback_range &>())));

static_assert(!noexcept(
    tested::ranges::data(tested::declval<throwing_begin_fallback_range &>())));

static_assert(noexcept(
    tested::ranges::cdata(tested::declval<noexcept_fallback_range &>())));

static_assert(!noexcept(
    tested::ranges::cdata(tested::declval<throwing_begin_fallback_range &>())));

static_assert(!noexcept(
    tested::ranges::rbegin(tested::declval<throwing_end_fallback_range &>())));

static_assert(!noexcept(
    tested::ranges::rend(tested::declval<throwing_begin_fallback_range &>())));

static_assert(!noexcept(
    tested::ranges::crbegin(tested::declval<throwing_end_fallback_range &>())));

static_assert(!noexcept(
    tested::ranges::crend(tested::declval<throwing_begin_fallback_range &>())));

/*
 * Sized-sentinel subtraction fallback.
 */

using noexcept_subtraction_range = subtraction_size_range<true, true, true>;

using throwing_subtraction_range = subtraction_size_range<true, true, false>;

using throwing_subtraction_begin_range =
    subtraction_size_range<false, true, true>;

using throwing_subtraction_end_range =
    subtraction_size_range<true, false, true>;

static_assert(noexcept(
    tested::ranges::size(tested::declval<noexcept_subtraction_range &>())));

static_assert(!noexcept(
    tested::ranges::size(tested::declval<throwing_subtraction_range &>())));

static_assert(!noexcept(tested::ranges::size(
    tested::declval<throwing_subtraction_begin_range &>())));

static_assert(!noexcept(
    tested::ranges::size(tested::declval<throwing_subtraction_end_range &>())));

static_assert(noexcept(
    tested::ranges::ssize(tested::declval<noexcept_subtraction_range &>())));

static_assert(!noexcept(
    tested::ranges::ssize(tested::declval<throwing_subtraction_range &>())));

static_assert(noexcept(
    tested::ranges::empty(tested::declval<noexcept_subtraction_range &>())));

static_assert(!noexcept(
    tested::ranges::empty(tested::declval<throwing_subtraction_range &>())));

/*
 * Iterator-comparison empty fallback.
 */

using noexcept_iterator_empty_range = iterator_empty_range<true, true, true>;

using throwing_iterator_compare_range = iterator_empty_range<true, true, false>;

using throwing_iterator_begin_range = iterator_empty_range<false, true, true>;

using throwing_iterator_end_range = iterator_empty_range<true, false, true>;

static_assert(!tested::ranges::sized_range<noexcept_iterator_empty_range>);

static_assert(noexcept(
    tested::ranges::empty(tested::declval<noexcept_iterator_empty_range &>())));

static_assert(!noexcept(tested::ranges::empty(
    tested::declval<throwing_iterator_compare_range &>())));

static_assert(!noexcept(
    tested::ranges::empty(tested::declval<throwing_iterator_begin_range &>())));

static_assert(!noexcept(
    tested::ranges::empty(tested::declval<throwing_iterator_end_range &>())));

/*
 * Range factories and expression-equivalent adaptors.
 */

using noexcept_movable_view = movable_view<true>;

using throwing_movable_view = movable_view<false>;

static_assert(noexcept(tested::ranges::views::all(
                  tested::declval<noexcept_movable_view &&>())) ==
              tested::is_nothrow_constructible_v<noexcept_movable_view,
                                                 noexcept_movable_view &&>);

static_assert(noexcept(tested::ranges::views::all(
                  tested::declval<throwing_movable_view &&>())) ==
              tested::is_nothrow_constructible_v<throwing_movable_view,
                                                 throwing_movable_view &&>);

using noexcept_movable_range = movable_range<true>;

using throwing_movable_range = movable_range<false>;

using noexcept_owning_view =
    tested::ranges::owning_view<noexcept_movable_range>;

using throwing_owning_view =
    tested::ranges::owning_view<throwing_movable_range>;

static_assert(noexcept(tested::ranges::views::all(
                  tested::declval<noexcept_movable_range &&>())) ==
              noexcept(noexcept_owning_view(
                  tested::declval<noexcept_movable_range &&>())));

static_assert(noexcept(tested::ranges::views::all(
                  tested::declval<throwing_movable_range &&>())) ==
              noexcept(throwing_owning_view(
                  tested::declval<throwing_movable_range &&>())));

using noexcept_ref_view = tested::ranges::ref_view<noexcept_movable_range>;

static_assert(
    noexcept(tested::ranges::views::all(
        tested::declval<noexcept_movable_range &>())) ==
    noexcept(noexcept_ref_view(tested::declval<noexcept_movable_range &>())));

using noexcept_single_value = movable_value<true>;

using throwing_single_value = movable_value<false>;

using noexcept_single_view = tested::ranges::single_view<noexcept_single_value>;

using throwing_single_view = tested::ranges::single_view<throwing_single_value>;

static_assert(noexcept(tested::ranges::views::single(
                  tested::declval<noexcept_single_value &&>())) ==
              noexcept(noexcept_single_view(
                  tested::declval<noexcept_single_value &&>())));

static_assert(noexcept(tested::ranges::views::single(
                  tested::declval<throwing_single_value &&>())) ==
              noexcept(throwing_single_view(
                  tested::declval<throwing_single_value &&>())));

/*
 * Unconditional noexcept view operations.
 */

using empty_view = tested::ranges::empty_view<int>;

static_assert(noexcept(empty_view::begin()));

static_assert(noexcept(empty_view::end()));

static_assert(noexcept(empty_view::data()));

static_assert(noexcept(empty_view::size()));

static_assert(noexcept(empty_view::empty()));

using integer_single_view = tested::ranges::single_view<int>;

static_assert(noexcept(tested::declval<integer_single_view &>().begin()));

static_assert(noexcept(tested::declval<const integer_single_view &>().begin()));

static_assert(noexcept(tested::declval<integer_single_view &>().end()));

static_assert(noexcept(tested::declval<const integer_single_view &>().end()));

static_assert(noexcept(tested::declval<integer_single_view &>().data()));

static_assert(noexcept(tested::declval<const integer_single_view &>().data()));

static_assert(noexcept(integer_single_view::size()));

static_assert(noexcept(integer_single_view::empty()));

static_assert(noexcept(tested::declval<throwing_owning_view &>().base()));

static_assert(noexcept(tested::declval<const throwing_owning_view &>().base()));

static_assert(noexcept(tested::declval<throwing_owning_view &&>().base()));

static_assert(
    noexcept(tested::declval<const throwing_owning_view &&>().base()));

/*
 * transform_view dereference.
 */

using noexcept_transform =
    tested::ranges::transform_view<pointer_view, nothrow_unary>;

using throwing_transform =
    tested::ranges::transform_view<pointer_view, throwing_unary>;

using noexcept_transform_iterator =
    tested::ranges::iterator_t<noexcept_transform>;

using throwing_transform_iterator =
    tested::ranges::iterator_t<throwing_transform>;

static_assert(
    noexcept(*tested::declval<const noexcept_transform_iterator &>()));

static_assert(
    !noexcept(*tested::declval<const throwing_transform_iterator &>()));

static_assert(
    noexcept(tested::declval<const noexcept_transform_iterator &>().base()));

/*
 * zip_transform_view dereference.
 */

using noexcept_zip_transform =
    tested::ranges::zip_transform_view<nothrow_binary, pointer_view,
                                       pointer_view>;

using throwing_zip_transform =
    tested::ranges::zip_transform_view<throwing_binary, pointer_view,
                                       pointer_view>;

using noexcept_zip_transform_iterator =
    tested::ranges::iterator_t<noexcept_zip_transform>;

using throwing_zip_transform_iterator =
    tested::ranges::iterator_t<throwing_zip_transform>;

static_assert(
    noexcept(*tested::declval<const noexcept_zip_transform_iterator &>()));

static_assert(
    !noexcept(*tested::declval<const throwing_zip_transform_iterator &>()));

/*
 * adjacent_transform_view dereference.
 */

using noexcept_adjacent_transform =
    decltype(tested::ranges::views::adjacent_transform<2>(
        tested::declval<pointer_view &>(), nothrow_binary{}));

using throwing_adjacent_transform =
    decltype(tested::ranges::views::adjacent_transform<2>(
        tested::declval<pointer_view &>(), throwing_binary{}));

using noexcept_adjacent_transform_iterator =
    tested::ranges::iterator_t<noexcept_adjacent_transform>;

using throwing_adjacent_transform_iterator =
    tested::ranges::iterator_t<throwing_adjacent_transform>;

static_assert(
    noexcept(*tested::declval<const noexcept_adjacent_transform_iterator &>()));

static_assert(!noexcept(
    *tested::declval<const throwing_adjacent_transform_iterator &>()));

/*
 * iota_view iterator dereference.
 */

using noexcept_iota = tested::ranges::iota_view<counter<true>>;

using throwing_iota = tested::ranges::iota_view<counter<false>>;

using noexcept_iota_iterator = tested::ranges::iterator_t<noexcept_iota>;

using throwing_iota_iterator = tested::ranges::iterator_t<throwing_iota>;

static_assert(noexcept(*tested::declval<const noexcept_iota_iterator &>()));

static_assert(!noexcept(*tested::declval<const throwing_iota_iterator &>()));

/*
 * repeat_view dereference.
 */

using repeat_view = tested::ranges::repeat_view<int, int>;

using repeat_iterator = tested::ranges::iterator_t<repeat_view>;

static_assert(noexcept(*tested::declval<const repeat_iterator &>()));

/*
 * Direct iter_move and iter_swap customization.
 */

using noexcept_audit_iterator = audit_iterator<true, true>;

using throwing_move_audit_iterator = audit_iterator<false, true>;

using throwing_swap_audit_iterator = audit_iterator<true, false>;

static_assert(noexcept(tested::ranges::iter_move(
    tested::declval<const noexcept_audit_iterator &>())));

static_assert(!noexcept(tested::ranges::iter_move(
    tested::declval<const throwing_move_audit_iterator &>())));

static_assert(noexcept(tested::ranges::iter_swap(
    tested::declval<const noexcept_audit_iterator &>(),
    tested::declval<const noexcept_audit_iterator &>())));

static_assert(!noexcept(tested::ranges::iter_swap(
    tested::declval<const throwing_swap_audit_iterator &>(),
    tested::declval<const throwing_swap_audit_iterator &>())));

/*
 * zip_view iter_move and iter_swap propagation.
 */

using noexcept_zip =
    tested::ranges::zip_view<audit_view<true, true>, audit_view<true, true>>;

using throwing_move_zip =
    tested::ranges::zip_view<audit_view<true, true>, audit_view<false, true>>;

using throwing_swap_zip =
    tested::ranges::zip_view<audit_view<true, true>, audit_view<true, false>>;

using noexcept_zip_iterator = tested::ranges::iterator_t<noexcept_zip>;

using throwing_move_zip_iterator =
    tested::ranges::iterator_t<throwing_move_zip>;

using throwing_swap_zip_iterator =
    tested::ranges::iterator_t<throwing_swap_zip>;

static_assert(noexcept(tested::ranges::iter_move(
    tested::declval<const noexcept_zip_iterator &>())));

static_assert(!noexcept(tested::ranges::iter_move(
    tested::declval<const throwing_move_zip_iterator &>())));

static_assert(noexcept(tested::ranges::iter_swap(
    tested::declval<const noexcept_zip_iterator &>(),
    tested::declval<const noexcept_zip_iterator &>())));

static_assert(!noexcept(tested::ranges::iter_swap(
    tested::declval<const throwing_swap_zip_iterator &>(),
    tested::declval<const throwing_swap_zip_iterator &>())));

/*
 * adjacent_view iter_move and iter_swap propagation.
 */

using noexcept_adjacent =
    tested::ranges::adjacent_view<audit_view<true, true>, 2>;

using throwing_move_adjacent =
    tested::ranges::adjacent_view<audit_view<false, true>, 2>;

using throwing_swap_adjacent =
    tested::ranges::adjacent_view<audit_view<true, false>, 2>;

using noexcept_adjacent_iterator =
    tested::ranges::iterator_t<noexcept_adjacent>;

using throwing_move_adjacent_iterator =
    tested::ranges::iterator_t<throwing_move_adjacent>;

using throwing_swap_adjacent_iterator =
    tested::ranges::iterator_t<throwing_swap_adjacent>;

static_assert(noexcept(tested::ranges::iter_move(
    tested::declval<const noexcept_adjacent_iterator &>())));

static_assert(!noexcept(tested::ranges::iter_move(
    tested::declval<const throwing_move_adjacent_iterator &>())));

static_assert(noexcept(tested::ranges::iter_swap(
    tested::declval<const noexcept_adjacent_iterator &>(),
    tested::declval<const noexcept_adjacent_iterator &>())));

static_assert(!noexcept(tested::ranges::iter_swap(
    tested::declval<const throwing_swap_adjacent_iterator &>(),
    tested::declval<const throwing_swap_adjacent_iterator &>())));

bool ftl_test() { return true; }