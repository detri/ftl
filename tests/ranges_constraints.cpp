#ifdef FTL_REPLACE_STL
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/concepts>
#include <ftl/cstddef>
#include <ftl/initializer_list>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/tuple>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

/*
 * Generic expression probes.
 */

template <class T, class... Args>
concept can_construct = requires { T(tested::declval<Args>()...); };

template <class T>
concept can_default_construct = requires { T(); };

template <class T>
concept can_ranges_begin =
    requires { tested::ranges::begin(tested::declval<T>()); };

template <class T>
concept can_ranges_end =
    requires { tested::ranges::end(tested::declval<T>()); };

template <class T>
concept can_ranges_cbegin =
    requires { tested::ranges::cbegin(tested::declval<T>()); };

template <class T>
concept can_ranges_cend =
    requires { tested::ranges::cend(tested::declval<T>()); };

template <class T>
concept can_ranges_size =
    requires { tested::ranges::size(tested::declval<T>()); };

template <class T>
concept can_ranges_ssize =
    requires { tested::ranges::ssize(tested::declval<T>()); };

template <class T>
concept can_ranges_empty =
    requires { tested::ranges::empty(tested::declval<T>()); };

template <class T>
concept can_ranges_data =
    requires { tested::ranges::data(tested::declval<T>()); };

template <class T>
concept can_ranges_cdata =
    requires { tested::ranges::cdata(tested::declval<T>()); };

template <class T>
concept can_ranges_rbegin =
    requires { tested::ranges::rbegin(tested::declval<T>()); };

template <class T>
concept can_ranges_rend =
    requires { tested::ranges::rend(tested::declval<T>()); };

/*
 * Range alias probes.
 */

template <class R>
concept has_iterator_type =
    requires { typename tested::ranges::iterator_t<R>; };

template <class R>
concept has_sentinel_type =
    requires { typename tested::ranges::sentinel_t<R>; };

template <class R>
concept has_const_iterator_type =
    requires { typename tested::ranges::const_iterator_t<R>; };

template <class R>
concept has_const_sentinel_type =
    requires { typename tested::ranges::const_sentinel_t<R>; };

template <class R>
concept has_range_value_type =
    requires { typename tested::ranges::range_value_t<R>; };

template <class R>
concept has_range_reference_type =
    requires { typename tested::ranges::range_reference_t<R>; };

template <class R>
concept has_range_rvalue_reference_type =
    requires { typename tested::ranges::range_rvalue_reference_t<R>; };

template <class R>
concept has_range_difference_type =
    requires { typename tested::ranges::range_difference_t<R>; };

template <class R>
concept has_range_size_type =
    requires { typename tested::ranges::range_size_t<R>; };

template <class R>
concept has_borrowed_iterator_type =
    requires { typename tested::ranges::borrowed_iterator_t<R>; };

template <class R>
concept has_borrowed_subrange_type =
    requires { typename tested::ranges::borrowed_subrange_t<R>; };

template <class R>
concept has_all_type = requires { typename tested::ranges::views::all_t<R>; };

/*
 * view_interface member probes.
 */

template <class T>
concept has_member_empty =
    requires(T &&value) { static_cast<T &&>(value).empty(); };

template <class T>
concept has_member_cbegin =
    requires(T &&value) { static_cast<T &&>(value).cbegin(); };

template <class T>
concept has_member_cend =
    requires(T &&value) { static_cast<T &&>(value).cend(); };

template <class T>
concept has_member_boolean_test =
    requires(T &&value) { static_cast<bool>(static_cast<T &&>(value)); };

template <class T>
concept has_member_data =
    requires(T &&value) { static_cast<T &&>(value).data(); };

template <class T>
concept has_member_size =
    requires(T &&value) { static_cast<T &&>(value).size(); };

template <class T>
concept has_member_front =
    requires(T &&value) { static_cast<T &&>(value).front(); };

template <class T>
concept has_member_back =
    requires(T &&value) { static_cast<T &&>(value).back(); };

template <class T>
concept has_member_subscript =
    requires(T &&value) { static_cast<T &&>(value)[0]; };

template <class T>
concept has_const_lvalue_base = requires(const T &value) { value.base(); };

template <class T>
concept has_rvalue_base =
    requires(T &&value) { static_cast<T &&>(value).base(); };

/*
 * Basic iterator and sentinel fixtures.
 */

struct input_iterator {
  using value_type = int;

  using difference_type = tested::ptrdiff_t;

  using iterator_concept = tested::input_iterator_tag;

  int *current = nullptr;

  constexpr int &operator*() const noexcept { return *current; }

  constexpr input_iterator &operator++() noexcept {
    ++current;
    return *this;
  }

  constexpr void operator++(int) noexcept { ++current; }

  friend constexpr bool operator==(input_iterator, input_iterator) = default;
};

struct input_sentinel {
  int *last = nullptr;

  friend constexpr bool operator==(input_iterator current,
                                   input_sentinel bound) noexcept {
    return current.current == bound.last;
  }

  friend constexpr bool operator==(input_sentinel bound,
                                   input_iterator current) noexcept {
    return current == bound;
  }
};

struct forward_iterator {
  using value_type = int;

  using difference_type = tested::ptrdiff_t;

  using iterator_concept = tested::forward_iterator_tag;

  using iterator_category = tested::forward_iterator_tag;

  int *current = nullptr;

  constexpr int &operator*() const noexcept { return *current; }

  constexpr forward_iterator &operator++() noexcept {
    ++current;
    return *this;
  }

  constexpr forward_iterator operator++(int) noexcept {
    auto previous = *this;
    ++*this;
    return previous;
  }

  friend constexpr bool operator==(forward_iterator,
                                   forward_iterator) = default;
};

struct forward_sentinel {
  int *last = nullptr;

  friend constexpr bool operator==(forward_iterator current,
                                   forward_sentinel bound) noexcept {
    return current.current == bound.last;
  }

  friend constexpr bool operator==(forward_sentinel bound,
                                   forward_iterator current) noexcept {
    return current == bound;
  }
};

struct bidirectional_iterator {
  using value_type = int;

  using difference_type = tested::ptrdiff_t;

  using iterator_concept = tested::bidirectional_iterator_tag;

  using iterator_category = tested::bidirectional_iterator_tag;

  int *current = nullptr;

  constexpr int &operator*() const noexcept { return *current; }

  constexpr bidirectional_iterator &operator++() noexcept {
    ++current;
    return *this;
  }

  constexpr bidirectional_iterator operator++(int) noexcept {
    auto previous = *this;
    ++*this;
    return previous;
  }

  constexpr bidirectional_iterator &operator--() noexcept {
    --current;
    return *this;
  }

  constexpr bidirectional_iterator operator--(int) noexcept {
    auto previous = *this;
    --*this;
    return previous;
  }

  friend constexpr bool operator==(bidirectional_iterator,
                                   bidirectional_iterator) = default;
};

struct bidirectional_sentinel {
  int *last = nullptr;

  friend constexpr bool operator==(bidirectional_iterator current,
                                   bidirectional_sentinel bound) noexcept {
    return current.current == bound.last;
  }

  friend constexpr bool operator==(bidirectional_sentinel bound,
                                   bidirectional_iterator current) noexcept {
    return current == bound;
  }
};

struct output_iterator {
  using difference_type = tested::ptrdiff_t;

  int *current = nullptr;

  constexpr int &operator*() const noexcept { return *current; }

  constexpr output_iterator &operator++() noexcept {
    ++current;
    return *this;
  }

  constexpr void operator++(int) noexcept { ++current; }
};

struct output_sentinel {
  int *last = nullptr;

  friend constexpr bool operator==(output_iterator current,
                                   output_sentinel bound) noexcept {
    return current.current == bound.last;
  }

  friend constexpr bool operator==(output_sentinel bound,
                                   output_iterator current) noexcept {
    return current == bound;
  }
};

struct unsized_pointer_sentinel {
  int *last = nullptr;

  friend constexpr bool operator==(int *current,
                                   unsized_pointer_sentinel bound) noexcept {
    return current == bound.last;
  }

  friend constexpr bool operator==(unsized_pointer_sentinel bound,
                                   int *current) noexcept {
    return current == bound;
  }
};

struct bad_sentinel {};

static_assert(tested::input_iterator<input_iterator>);

static_assert(tested::forward_iterator<forward_iterator>);

static_assert(tested::bidirectional_iterator<bidirectional_iterator>);

static_assert(tested::input_or_output_iterator<output_iterator>);

static_assert(tested::sentinel_for<input_sentinel, input_iterator>);

static_assert(tested::sentinel_for<forward_sentinel, forward_iterator>);

static_assert(
    tested::sentinel_for<bidirectional_sentinel, bidirectional_iterator>);

static_assert(tested::sentinel_for<output_sentinel, output_iterator>);

static_assert(tested::sentinel_for<unsized_pointer_sentinel, int *>);

static_assert(!tested::sized_sentinel_for<unsized_pointer_sentinel, int *>);

/*
 * Range fixtures.
 */

template <class T> struct pointer_view : tested::ranges::view_base {
  T *first = nullptr;
  T *last = nullptr;

  constexpr pointer_view() = default;

  constexpr pointer_view(T *first, T *last) noexcept
      : first(first), last(last) {}

  constexpr T *begin() noexcept { return first; }

  constexpr const T *begin() const noexcept { return first; }

  constexpr T *end() noexcept { return last; }

  constexpr const T *end() const noexcept { return last; }

  constexpr tested::size_t size() const noexcept {
    return static_cast<tested::size_t>(last - first);
  }
};

struct mutable_pointer_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr mutable_pointer_view() = default;

  constexpr mutable_pointer_view(int *first, int *last) noexcept
      : first(first), last(last) {}

  constexpr int *begin() noexcept { return first; }

  constexpr int *end() noexcept { return last; }
};

struct input_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr input_view() = default;

  constexpr input_view(int *first, int *last) noexcept
      : first(first), last(last) {}

  constexpr input_iterator begin() noexcept { return {first}; }

  constexpr input_sentinel end() noexcept { return {last}; }
};

struct forward_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr forward_view() = default;

  constexpr forward_view(int *first, int *last) noexcept
      : first(first), last(last) {}

  constexpr forward_iterator begin() const noexcept { return {first}; }

  constexpr forward_sentinel end() const noexcept { return {last}; }
};

struct bidirectional_noncommon_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr bidirectional_noncommon_view() = default;

  constexpr bidirectional_noncommon_view(int *first, int *last) noexcept
      : first(first), last(last) {}

  constexpr bidirectional_iterator begin() const noexcept { return {first}; }

  constexpr bidirectional_sentinel end() const noexcept { return {last}; }
};

struct output_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr output_view() = default;

  constexpr output_view(int *first, int *last) noexcept
      : first(first), last(last) {}

  constexpr output_iterator begin() noexcept { return {first}; }

  constexpr output_sentinel end() noexcept { return {last}; }
};

struct move_only_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr move_only_range() = default;

  constexpr move_only_range(int *first, int *last) noexcept
      : first(first), last(last) {}

  move_only_range(const move_only_range &) = delete;

  move_only_range &operator=(const move_only_range &) = delete;

  constexpr move_only_range(move_only_range &&) noexcept = default;

  constexpr move_only_range &operator=(move_only_range &&) noexcept = default;

  constexpr int *begin() noexcept { return first; }

  constexpr const int *begin() const noexcept { return first; }

  constexpr int *end() noexcept { return last; }

  constexpr const int *end() const noexcept { return last; }

  constexpr tested::size_t size() const noexcept {
    return static_cast<tested::size_t>(last - first);
  }
};

struct immobile_range {
  int *first = nullptr;
  int *last = nullptr;

  immobile_range() = default;

  immobile_range(const immobile_range &) = delete;

  immobile_range(immobile_range &&) = delete;

  immobile_range &operator=(const immobile_range &) = delete;

  immobile_range &operator=(immobile_range &&) = delete;

  constexpr int *begin() noexcept { return first; }

  constexpr int *end() noexcept { return last; }
};

struct move_only_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr move_only_view() = default;

  constexpr move_only_view(int *first, int *last) noexcept
      : first(first), last(last) {}

  move_only_view(const move_only_view &) = delete;

  move_only_view &operator=(const move_only_view &) = delete;

  constexpr move_only_view(move_only_view &&) noexcept = default;

  constexpr move_only_view &operator=(move_only_view &&) noexcept = default;

  constexpr int *begin() noexcept { return first; }

  constexpr int *end() noexcept { return last; }
};

struct borrowed_pointer_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept { return first; }

  constexpr const int *begin() const noexcept { return first; }

  constexpr int *end() noexcept { return last; }

  constexpr const int *end() const noexcept { return last; }
};

#ifdef FTL_REPLACE_STL

namespace std::ranges {
template <>
inline constexpr bool enable_borrowed_range<::borrowed_pointer_range> = true;
}

#else

namespace ftl::ranges {
template <>
inline constexpr bool enable_borrowed_range<::borrowed_pointer_range> = true;
}

#endif

static_assert(tested::ranges::view<pointer_view<int>>);

static_assert(tested::ranges::input_range<input_view>);

static_assert(!tested::ranges::forward_range<input_view>);

static_assert(tested::ranges::forward_range<forward_view>);

static_assert(!tested::ranges::common_range<forward_view>);

static_assert(
    tested::ranges::bidirectional_range<bidirectional_noncommon_view>);

static_assert(!tested::ranges::common_range<bidirectional_noncommon_view>);

static_assert(tested::ranges::range<output_view>);

static_assert(!tested::ranges::input_range<output_view>);

/*
 * Miscellaneous argument and callable fixtures.
 */

struct unrelated {};

struct non_incrementable {};

struct non_equality_bound {};

struct move_only_value {
  int value = 0;

  constexpr explicit move_only_value(int value) noexcept : value(value) {}

  move_only_value(const move_only_value &) = delete;

  move_only_value &operator=(const move_only_value &) = delete;

  constexpr move_only_value(move_only_value &&) noexcept = default;

  constexpr move_only_value &operator=(move_only_value &&) noexcept = default;
};

struct immobile_value {
  immobile_value() = default;

  immobile_value(const immobile_value &) = delete;

  immobile_value(immobile_value &&) = delete;

  immobile_value &operator=(const immobile_value &) = delete;

  immobile_value &operator=(immobile_value &&) = delete;
};

struct unary_predicate {
  constexpr bool operator()(int value) const noexcept { return value != 0; }
};

struct wrong_unary_predicate {
  constexpr bool operator()(unrelated) const noexcept { return true; }
};

struct void_unary_predicate {
  constexpr void operator()(int) const noexcept {}
};

struct binary_predicate {
  constexpr bool operator()(int left, int right) const noexcept {
    return left == right;
  }
};

struct wrong_binary_predicate {
  constexpr bool operator()(unrelated, unrelated) const noexcept {
    return true;
  }
};

struct unary_transform {
  constexpr int operator()(int value) const noexcept { return value; }
};

struct binary_transform {
  constexpr int operator()(int left, int right) const noexcept {
    return left + right;
  }
};

struct void_unary_transform {
  constexpr void operator()(int) const noexcept {}
};

struct wrong_unary_transform {
  constexpr int operator()(unrelated) const noexcept { return 0; }
};

struct move_only_transform {
  constexpr move_only_transform() = default;

  move_only_transform(const move_only_transform &) = delete;

  move_only_transform &operator=(const move_only_transform &) = delete;

  constexpr move_only_transform(move_only_transform &&) noexcept = default;

  constexpr move_only_transform &
  operator=(move_only_transform &&) noexcept = default;

  constexpr int operator()(int value) const noexcept { return value; }
};

/*
 * Core range concept and alias participation.
 */

struct non_range {};

static_assert(tested::ranges::range<pointer_view<int>>);

static_assert(!tested::ranges::range<non_range>);

static_assert(tested::ranges::borrowed_range<pointer_view<int> &>);

static_assert(!tested::ranges::borrowed_range<pointer_view<int>>);

static_assert(tested::ranges::borrowed_range<borrowed_pointer_range>);

static_assert(tested::ranges::viewable_range<pointer_view<int> &>);

static_assert(tested::ranges::viewable_range<pointer_view<int>>);

static_assert(tested::ranges::viewable_range<move_only_range>);

static_assert(!tested::ranges::viewable_range<immobile_range>);

static_assert(!tested::ranges::viewable_range<tested::initializer_list<int>>);

static_assert(!tested::ranges::viewable_range<non_range>);

static_assert(has_iterator_type<pointer_view<int>>);

static_assert(has_sentinel_type<pointer_view<int>>);

static_assert(has_const_iterator_type<pointer_view<int>>);

static_assert(has_const_sentinel_type<pointer_view<int>>);

static_assert(has_range_value_type<pointer_view<int>>);

static_assert(has_range_reference_type<pointer_view<int>>);

static_assert(has_range_rvalue_reference_type<pointer_view<int>>);

static_assert(has_range_difference_type<pointer_view<int>>);

static_assert(has_range_size_type<pointer_view<int>>);

static_assert(has_borrowed_iterator_type<pointer_view<int>>);

static_assert(has_borrowed_subrange_type<pointer_view<int>>);

static_assert(has_iterator_type<input_view>);

static_assert(has_range_value_type<input_view>);

static_assert(has_range_reference_type<input_view>);

static_assert(has_range_difference_type<input_view>);

static_assert(!has_range_size_type<input_view>);

static_assert(!has_range_value_type<output_view>);

static_assert(!has_iterator_type<non_range>);

static_assert(!has_sentinel_type<non_range>);

static_assert(!has_const_iterator_type<non_range>);

static_assert(!has_const_sentinel_type<non_range>);

static_assert(!has_range_value_type<non_range>);

static_assert(!has_range_reference_type<non_range>);

static_assert(!has_range_rvalue_reference_type<non_range>);

static_assert(!has_range_difference_type<non_range>);

static_assert(!has_range_size_type<non_range>);

static_assert(!has_borrowed_iterator_type<non_range>);

static_assert(!has_borrowed_subrange_type<non_range>);

static_assert(has_all_type<pointer_view<int> &>);

static_assert(has_all_type<pointer_view<int>>);

static_assert(has_all_type<move_only_range>);

static_assert(!has_all_type<immobile_range>);

static_assert(!has_all_type<tested::initializer_list<int>>);

static_assert(!has_all_type<non_range>);

/*
 * Range CPO constraints.
 */

static_assert(can_ranges_begin<pointer_view<int> &>);

static_assert(can_ranges_end<pointer_view<int> &>);

static_assert(!can_ranges_begin<pointer_view<int> &&>);

static_assert(!can_ranges_end<pointer_view<int> &&>);

static_assert(can_ranges_begin<borrowed_pointer_range &&>);

static_assert(can_ranges_end<borrowed_pointer_range &&>);

static_assert(can_ranges_cbegin<pointer_view<int> &>);

static_assert(can_ranges_cend<pointer_view<int> &>);

static_assert(can_ranges_size<pointer_view<int> &>);

static_assert(can_ranges_size<pointer_view<int> &&>);

static_assert(can_ranges_ssize<pointer_view<int> &>);

static_assert(!can_ranges_size<forward_view &>);

static_assert(can_ranges_empty<pointer_view<int> &>);

static_assert(!can_ranges_empty<input_view &>);

static_assert(can_ranges_empty<forward_view &>);

static_assert(can_ranges_data<pointer_view<int> &>);

static_assert(can_ranges_cdata<pointer_view<int> &>);

static_assert(!can_ranges_data<forward_view &>);

static_assert(can_ranges_rbegin<pointer_view<int> &>);

static_assert(can_ranges_rend<pointer_view<int> &>);

static_assert(!can_ranges_rbegin<forward_view &>);

static_assert(!can_ranges_rbegin<bidirectional_noncommon_view &>);

static_assert(!can_ranges_begin<non_range &>);

static_assert(!can_ranges_size<non_range &>);

static_assert(!can_ranges_empty<non_range &>);

/*
 * view_interface type and conditional-member constraints.
 */

struct dummy_class {};

template <class T>
concept can_form_view_interface =
    requires { typename tested::ranges::view_interface<T>; };

static_assert(can_form_view_interface<dummy_class>);

static_assert(!can_form_view_interface<const dummy_class>);

static_assert(!can_form_view_interface<int>);

struct output_interface_view
    : tested::ranges::view_interface<output_interface_view> {
  int *first = nullptr;
  int *last = nullptr;

  constexpr output_iterator begin() noexcept { return {first}; }

  constexpr output_sentinel end() noexcept { return {last}; }
};

struct input_interface_view
    : tested::ranges::view_interface<input_interface_view> {
  int *first = nullptr;
  int *last = nullptr;

  constexpr input_iterator begin() noexcept { return {first}; }

  constexpr input_sentinel end() noexcept { return {last}; }
};

struct forward_interface_view
    : tested::ranges::view_interface<forward_interface_view> {
  int *first = nullptr;
  int *last = nullptr;

  constexpr forward_iterator begin() const noexcept { return {first}; }

  constexpr forward_sentinel end() const noexcept { return {last}; }
};

struct bidirectional_interface_view
    : tested::ranges::view_interface<bidirectional_interface_view> {
  int *first = nullptr;
  int *last = nullptr;

  constexpr bidirectional_iterator begin() const noexcept { return {first}; }

  constexpr bidirectional_sentinel end() const noexcept { return {last}; }
};

struct random_access_interface_view
    : tested::ranges::view_interface<random_access_interface_view> {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept { return first; }

  constexpr const int *begin() const noexcept { return first; }

  constexpr int *end() noexcept { return last; }

  constexpr const int *end() const noexcept { return last; }
};

struct mutable_random_access_interface_view
    : tested::ranges::view_interface<mutable_random_access_interface_view> {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept { return first; }

  constexpr int *end() noexcept { return last; }
};

static_assert(!has_member_empty<output_interface_view &>);

static_assert(!has_member_cbegin<output_interface_view &>);

static_assert(!has_member_cend<output_interface_view &>);

static_assert(!has_member_boolean_test<output_interface_view &>);

static_assert(!has_member_data<output_interface_view &>);

static_assert(!has_member_size<output_interface_view &>);

static_assert(!has_member_front<output_interface_view &>);

static_assert(!has_member_back<output_interface_view &>);

static_assert(!has_member_subscript<output_interface_view &>);

static_assert(!has_member_empty<input_interface_view &>);

static_assert(has_member_cbegin<input_interface_view &>);

static_assert(has_member_cend<input_interface_view &>);

static_assert(!has_member_boolean_test<input_interface_view &>);

static_assert(!has_member_data<input_interface_view &>);

static_assert(!has_member_size<input_interface_view &>);

static_assert(!has_member_front<input_interface_view &>);

static_assert(!has_member_back<input_interface_view &>);

static_assert(!has_member_subscript<input_interface_view &>);

static_assert(has_member_empty<forward_interface_view &>);

static_assert(has_member_boolean_test<forward_interface_view &>);

static_assert(has_member_cbegin<forward_interface_view &>);

static_assert(has_member_cend<forward_interface_view &>);

static_assert(has_member_front<forward_interface_view &>);

static_assert(!has_member_back<forward_interface_view &>);

static_assert(!has_member_data<forward_interface_view &>);

static_assert(!has_member_size<forward_interface_view &>);

static_assert(!has_member_subscript<forward_interface_view &>);

static_assert(has_member_empty<bidirectional_interface_view &>);

static_assert(has_member_front<bidirectional_interface_view &>);

static_assert(!has_member_back<bidirectional_interface_view &>);

static_assert(!has_member_size<bidirectional_interface_view &>);

static_assert(has_member_empty<random_access_interface_view &>);

static_assert(has_member_empty<const random_access_interface_view &>);

static_assert(has_member_boolean_test<random_access_interface_view &>);

static_assert(has_member_data<random_access_interface_view &>);

static_assert(has_member_data<const random_access_interface_view &>);

static_assert(has_member_size<random_access_interface_view &>);

static_assert(has_member_size<const random_access_interface_view &>);

static_assert(has_member_front<random_access_interface_view &>);

static_assert(has_member_back<random_access_interface_view &>);

static_assert(has_member_subscript<random_access_interface_view &>);

static_assert(has_member_cbegin<mutable_random_access_interface_view &>);

static_assert(has_member_cend<mutable_random_access_interface_view &>);

static_assert(has_member_empty<mutable_random_access_interface_view &>);

static_assert(has_member_front<mutable_random_access_interface_view &>);

static_assert(has_member_back<mutable_random_access_interface_view &>);

static_assert(has_member_data<mutable_random_access_interface_view &>);

static_assert(has_member_size<mutable_random_access_interface_view &>);

static_assert(has_member_subscript<mutable_random_access_interface_view &>);

static_assert(!has_member_empty<const mutable_random_access_interface_view &>);

static_assert(!has_member_cbegin<const mutable_random_access_interface_view &>);

static_assert(!has_member_data<const mutable_random_access_interface_view &>);

static_assert(!has_member_front<const mutable_random_access_interface_view &>);

/*
 * ref_view, owning_view, empty_view, and single_view.
 */

template <class R>
concept can_form_ref_view = requires { typename tested::ranges::ref_view<R>; };

template <class R>
concept can_form_owning_view =
    requires { typename tested::ranges::owning_view<R>; };

template <class T>
concept can_form_empty_view =
    requires { typename tested::ranges::empty_view<T>; };

template <class T>
concept can_form_single_view =
    requires { typename tested::ranges::single_view<T>; };

static_assert(can_form_ref_view<pointer_view<int>>);

static_assert(can_form_ref_view<const pointer_view<int>>);

static_assert(!can_form_ref_view<pointer_view<int> &>);

static_assert(!can_form_ref_view<int>);

using mutable_ref_view = tested::ranges::ref_view<pointer_view<int>>;

using const_ref_view = tested::ranges::ref_view<const pointer_view<int>>;

static_assert(can_construct<mutable_ref_view, pointer_view<int> &>);

static_assert(!can_construct<mutable_ref_view, pointer_view<int> &&>);

static_assert(!can_construct<mutable_ref_view, const pointer_view<int> &>);

static_assert(can_construct<const_ref_view, const pointer_view<int> &>);

static_assert(tested::copy_constructible<mutable_ref_view>);

static_assert(can_form_owning_view<move_only_range>);

static_assert(can_form_owning_view<pointer_view<int>>);

static_assert(!can_form_owning_view<immobile_range>);

static_assert(!can_form_owning_view<int[3]>);

static_assert(!can_form_owning_view<tested::initializer_list<int>>);

using move_owning_view = tested::ranges::owning_view<move_only_range>;

static_assert(can_construct<move_owning_view, move_only_range &&>);

static_assert(!can_construct<move_owning_view, move_only_range &>);

static_assert(tested::move_constructible<move_owning_view>);

static_assert(!tested::copy_constructible<move_owning_view>);

static_assert(can_form_empty_view<int>);

static_assert(can_form_empty_view<const int>);

static_assert(!can_form_empty_view<void>);

static_assert(!can_form_empty_view<int &>);

static_assert(can_form_single_view<int>);

static_assert(can_form_single_view<move_only_value>);

static_assert(!can_form_single_view<immobile_value>);

static_assert(!can_form_single_view<void>);

static_assert(!can_form_single_view<int &>);

static_assert(can_form_single_view<const int>);

using move_single_view = tested::ranges::single_view<move_only_value>;

static_assert(can_construct<move_single_view, move_only_value &&>);

static_assert(!can_construct<move_single_view, const move_only_value &>);

static_assert(can_construct<move_single_view, tested::in_place_t, int>);

static_assert(!can_construct<move_single_view, tested::in_place_t, unrelated>);

static_assert(tested::move_constructible<move_single_view>);

static_assert(!tested::copy_constructible<move_single_view>);

/*
 * subrange formation and constructor constraints.
 */

template <class Iterator, class Sentinel, tested::ranges::subrange_kind Kind>
concept can_form_subrange =
    requires { typename tested::ranges::subrange<Iterator, Sentinel, Kind>; };

template <class R>
concept can_deduce_subrange =
    requires { tested::ranges::subrange(tested::declval<R>()); };

static_assert(
    can_form_subrange<int *, int *, tested::ranges::subrange_kind::sized>);

static_assert(
    !can_form_subrange<int *, int *, tested::ranges::subrange_kind::unsized>);

static_assert(can_form_subrange<int *, unsized_pointer_sentinel,
                                tested::ranges::subrange_kind::unsized>);

static_assert(can_form_subrange<int *, unsized_pointer_sentinel,
                                tested::ranges::subrange_kind::sized>);

static_assert(
    !can_form_subrange<int, int, tested::ranges::subrange_kind::unsized>);

static_assert(!can_form_subrange<int *, bad_sentinel,
                                 tested::ranges::subrange_kind::unsized>);

using pointer_subrange = tested::ranges::subrange<int *>;

using stored_size_subrange =
    tested::ranges::subrange<int *, unsized_pointer_sentinel,
                             tested::ranges::subrange_kind::sized>;

using unsigned_difference = tested::make_unsigned_t<tested::ptrdiff_t>;

static_assert(can_construct<pointer_subrange, int *, int *>);

static_assert(
    can_construct<pointer_subrange, int *, int *, unsigned_difference>);

static_assert(
    !can_construct<stored_size_subrange, int *, unsized_pointer_sentinel>);

static_assert(can_construct<stored_size_subrange, int *,
                            unsized_pointer_sentinel, unsigned_difference>);

static_assert(can_construct<pointer_subrange, pointer_view<int> &>);

static_assert(!can_construct<pointer_subrange, pointer_view<int> &&>);

static_assert(can_construct<pointer_subrange, borrowed_pointer_range &&>);

static_assert(can_deduce_subrange<pointer_view<int> &>);

static_assert(!can_deduce_subrange<pointer_view<int> &&>);

static_assert(can_deduce_subrange<borrowed_pointer_range &&>);

/*
 * iota_view and repeat_view template constraints.
 */

template <class W, class Bound>
concept can_form_iota_view =
    requires { typename tested::ranges::iota_view<W, Bound>; };

template <class W, class Bound>
concept can_form_repeat_view =
    requires { typename tested::ranges::repeat_view<W, Bound>; };

static_assert(can_form_iota_view<int, int>);

static_assert(can_form_iota_view<int, tested::unreachable_sentinel_t>);

static_assert(!can_form_iota_view<non_incrementable, non_incrementable>);

static_assert(!can_form_iota_view<int, non_equality_bound>);

static_assert(!can_form_iota_view<const int, int>);

static_assert(can_form_repeat_view<int, int>);

static_assert(can_form_repeat_view<int, unsigned short>);

static_assert(can_form_repeat_view<move_only_value, int>);

static_assert(!can_form_repeat_view<immobile_value, int>);

static_assert(!can_form_repeat_view<const int, int>);

static_assert(!can_form_repeat_view<int, bool>);

static_assert(!can_form_repeat_view<int, double>);

using move_repeat_view = tested::ranges::repeat_view<move_only_value, int>;

static_assert(can_construct<move_repeat_view, move_only_value &&, int>);

static_assert(!can_construct<move_repeat_view, const move_only_value &, int>);

static_assert(tested::move_constructible<move_repeat_view>);

static_assert(!tested::copy_constructible<move_repeat_view>);

/*
 * Direct constrained view-template formation.
 */

template <class V>
concept can_form_take_view =
    requires { typename tested::ranges::take_view<V>; };

template <class V>
concept can_form_drop_view =
    requires { typename tested::ranges::drop_view<V>; };

template <class V, class Pred>
concept can_form_take_while_view =
    requires { typename tested::ranges::take_while_view<V, Pred>; };

template <class V, class Pred>
concept can_form_drop_while_view =
    requires { typename tested::ranges::drop_while_view<V, Pred>; };

template <class V, class Pred>
concept can_form_filter_view =
    requires { typename tested::ranges::filter_view<V, Pred>; };

template <class V, class Function>
concept can_form_transform_view =
    requires { typename tested::ranges::transform_view<V, Function>; };

template <class V>
concept can_form_reverse_view =
    requires { typename tested::ranges::reverse_view<V>; };

template <class V>
concept can_form_common_view =
    requires { typename tested::ranges::common_view<V>; };

template <class V, tested::size_t Index>
concept can_form_elements_view =
    requires { typename tested::ranges::elements_view<V, Index>; };

template <class V>
concept can_form_join_view =
    requires { typename tested::ranges::join_view<V>; };

template <class V, class Pattern>
concept can_form_split_view =
    requires { typename tested::ranges::split_view<V, Pattern>; };

template <class V, class Pattern>
concept can_form_lazy_split_view =
    requires { typename tested::ranges::lazy_split_view<V, Pattern>; };

template <class V>
concept can_form_as_const_view =
    requires { typename tested::ranges::as_const_view<V>; };

template <class V>
concept can_form_stride_view =
    requires { typename tested::ranges::stride_view<V>; };

template <class... Views>
concept can_form_zip_view =
    requires { typename tested::ranges::zip_view<Views...>; };

template <class Function, class... Views>
concept can_form_zip_transform_view = requires {
  typename tested::ranges::zip_transform_view<Function, Views...>;
};

template <class V, tested::size_t Count>
concept can_form_adjacent_view =
    requires { typename tested::ranges::adjacent_view<V, Count>; };

template <class Function, class V, tested::size_t Count>
concept can_form_adjacent_transform_view = requires {
  typename tested::ranges::adjacent_transform_view<V, Function, Count>;
};

template <class V>
concept can_form_slide_view =
    requires { typename tested::ranges::slide_view<V>; };

template <class V, class Pred>
concept can_form_chunk_by_view =
    requires { typename tested::ranges::chunk_by_view<V, Pred>; };

template <class V>
concept can_form_chunk_view =
    requires { typename tested::ranges::chunk_view<V>; };

template <class First, class... Rest>
concept can_form_cartesian_product_view = requires {
  typename tested::ranges::cartesian_product_view<First, Rest...>;
};

using integer_view = pointer_view<int>;

using pair_type = tested::pair<int, long>;

using pair_view = pointer_view<pair_type>;

using nested_view = pointer_view<pointer_view<int>>;

static_assert(can_form_take_view<integer_view>);

static_assert(can_form_take_view<input_view>);

static_assert(!can_form_take_view<move_only_range>);

static_assert(can_form_drop_view<integer_view>);

static_assert(can_form_drop_view<input_view>);

static_assert(!can_form_drop_view<move_only_range>);

static_assert(can_form_take_while_view<input_view, unary_predicate>);

static_assert(!can_form_take_while_view<output_view, unary_predicate>);

static_assert(!can_form_take_while_view<input_view, wrong_unary_predicate>);

static_assert(can_form_drop_while_view<input_view, unary_predicate>);

static_assert(!can_form_drop_while_view<output_view, unary_predicate>);

static_assert(!can_form_drop_while_view<input_view, void_unary_predicate>);

static_assert(can_form_filter_view<input_view, unary_predicate>);

static_assert(!can_form_filter_view<output_view, unary_predicate>);

static_assert(!can_form_filter_view<input_view, wrong_unary_predicate>);

static_assert(can_form_transform_view<input_view, unary_transform>);

static_assert(can_form_transform_view<integer_view, move_only_transform>);

static_assert(!can_form_transform_view<output_view, unary_transform>);

static_assert(!can_form_transform_view<input_view, void_unary_transform>);

static_assert(!can_form_transform_view<input_view, wrong_unary_transform>);

static_assert(can_form_reverse_view<integer_view>);

static_assert(can_form_reverse_view<bidirectional_noncommon_view>);

static_assert(!can_form_reverse_view<forward_view>);

static_assert(!can_form_reverse_view<input_view>);

static_assert(can_form_common_view<forward_view>);

static_assert(can_form_common_view<input_view>);

static_assert(!can_form_common_view<integer_view>);

static_assert(can_form_elements_view<pair_view, 0>);

static_assert(can_form_elements_view<pair_view, 1>);

static_assert(!can_form_elements_view<pair_view, 2>);

static_assert(!can_form_elements_view<integer_view, 0>);

static_assert(can_form_join_view<nested_view>);

static_assert(!can_form_join_view<integer_view>);

static_assert(!can_form_join_view<output_view>);

static_assert(
    can_form_split_view<integer_view, tested::ranges::single_view<int>>);

static_assert(can_form_split_view<integer_view, integer_view>);

static_assert(
    !can_form_split_view<input_view, tested::ranges::single_view<int>>);

static_assert(!can_form_split_view<integer_view, input_view>);

static_assert(
    can_form_lazy_split_view<input_view, tested::ranges::single_view<int>>);

static_assert(
    can_form_lazy_split_view<input_view, tested::ranges::empty_view<int>>);

static_assert(!can_form_lazy_split_view<input_view, integer_view>);

static_assert(can_form_lazy_split_view<integer_view, integer_view>);

static_assert(can_form_as_const_view<input_view>);

static_assert(can_form_as_const_view<integer_view>);

static_assert(!can_form_as_const_view<output_view>);

static_assert(can_form_stride_view<input_view>);

static_assert(can_form_stride_view<integer_view>);

static_assert(!can_form_stride_view<output_view>);

static_assert(can_form_zip_view<input_view>);

static_assert(can_form_zip_view<input_view, integer_view>);

static_assert(!can_form_zip_view<>);

static_assert(!can_form_zip_view<output_view>);

static_assert(can_form_zip_transform_view<unary_transform, integer_view>);

static_assert(
    can_form_zip_transform_view<binary_transform, integer_view, integer_view>);

static_assert(!can_form_zip_transform_view<void_unary_transform, integer_view>);

static_assert(!can_form_zip_transform_view<binary_transform, integer_view>);

static_assert(!can_form_zip_transform_view<unary_transform, output_view>);

static_assert(can_form_adjacent_view<integer_view, 2>);

static_assert(!can_form_adjacent_view<integer_view, 0>);

static_assert(!can_form_adjacent_view<input_view, 2>);

static_assert(
    can_form_adjacent_transform_view<binary_transform, integer_view, 2>);

static_assert(
    !can_form_adjacent_transform_view<unary_transform, integer_view, 2>);

static_assert(
    !can_form_adjacent_transform_view<binary_transform, integer_view, 0>);

static_assert(
    !can_form_adjacent_transform_view<binary_transform, input_view, 2>);

static_assert(can_form_slide_view<integer_view>);

static_assert(can_form_slide_view<forward_view>);

static_assert(!can_form_slide_view<input_view>);

static_assert(can_form_chunk_by_view<integer_view, binary_predicate>);

static_assert(!can_form_chunk_by_view<input_view, binary_predicate>);

static_assert(!can_form_chunk_by_view<integer_view, wrong_binary_predicate>);

static_assert(can_form_chunk_view<integer_view>);

static_assert(can_form_chunk_view<input_view>);

static_assert(!can_form_chunk_view<output_view>);

static_assert(can_form_cartesian_product_view<input_view>);

static_assert(can_form_cartesian_product_view<input_view, integer_view>);

static_assert(can_form_cartesian_product_view<integer_view, integer_view>);

static_assert(!can_form_cartesian_product_view<integer_view, input_view>);

static_assert(!can_form_cartesian_product_view<output_view>);

/*
 * Constructor and copy/move participation for stateful views.
 */

using movable_transform_view =
    tested::ranges::transform_view<integer_view, move_only_transform>;

static_assert(can_construct<movable_transform_view, integer_view,
                            move_only_transform &&>);

static_assert(!can_construct<movable_transform_view, integer_view,
                             move_only_transform &>);

static_assert(tested::move_constructible<movable_transform_view>);

static_assert(!tested::copy_constructible<movable_transform_view>);

using movable_take_view = tested::ranges::take_view<move_only_view>;

static_assert(tested::move_constructible<movable_take_view>);

static_assert(!tested::copy_constructible<movable_take_view>);

static_assert(!has_const_lvalue_base<movable_take_view>);

static_assert(has_rvalue_base<movable_take_view>);

using movable_stride_view = tested::ranges::stride_view<move_only_view>;

static_assert(tested::move_constructible<movable_stride_view>);

static_assert(!tested::copy_constructible<movable_stride_view>);

static_assert(!has_const_lvalue_base<movable_stride_view>);

static_assert(has_rvalue_base<movable_stride_view>);

bool ftl_test() { return true; }