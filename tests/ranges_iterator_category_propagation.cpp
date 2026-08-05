#ifdef FTL_REPLACE_STL
#include <compare>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/compare>
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

template <class T> class random_access_test_iterator {
  T *current_ = nullptr;

public:
  using iterator_concept = tested::random_access_iterator_tag;

  using iterator_category = tested::random_access_iterator_tag;

  using value_type = tested::remove_cv_t<T>;

  using difference_type = tested::ptrdiff_t;

  using pointer = T *;
  using reference = T &;

  constexpr random_access_test_iterator() = default;

  constexpr explicit random_access_test_iterator(T *current) noexcept
      : current_(current) {}

  constexpr T &operator*() const noexcept { return *current_; }

  constexpr T &operator[](difference_type offset) const noexcept {
    return current_[offset];
  }

  constexpr random_access_test_iterator &operator++() noexcept {
    ++current_;
    return *this;
  }

  constexpr random_access_test_iterator operator++(int) noexcept {
    auto previous = *this;
    ++*this;
    return previous;
  }

  constexpr random_access_test_iterator &operator--() noexcept {
    --current_;
    return *this;
  }

  constexpr random_access_test_iterator operator--(int) noexcept {
    auto previous = *this;
    --*this;
    return previous;
  }

  constexpr random_access_test_iterator &
  operator+=(difference_type offset) noexcept {
    current_ += offset;
    return *this;
  }

  constexpr random_access_test_iterator &
  operator-=(difference_type offset) noexcept {
    current_ -= offset;
    return *this;
  }

  friend constexpr random_access_test_iterator
  operator+(random_access_test_iterator iterator,
            difference_type offset) noexcept {
    iterator += offset;
    return iterator;
  }

  friend constexpr random_access_test_iterator
  operator+(difference_type offset,
            random_access_test_iterator iterator) noexcept {
    iterator += offset;
    return iterator;
  }

  friend constexpr random_access_test_iterator
  operator-(random_access_test_iterator iterator,
            difference_type offset) noexcept {
    iterator -= offset;
    return iterator;
  }

  friend constexpr difference_type
  operator-(random_access_test_iterator left,
            random_access_test_iterator right) noexcept {
    return left.current_ - right.current_;
  }

  friend constexpr bool
  operator==(const random_access_test_iterator &,
             const random_access_test_iterator &) = default;

  friend constexpr bool
  operator<(const random_access_test_iterator &left,
            const random_access_test_iterator &right) noexcept {
    return left.current_ < right.current_;
  }

  friend constexpr bool
  operator>(const random_access_test_iterator &left,
            const random_access_test_iterator &right) noexcept {
    return right < left;
  }

  friend constexpr bool
  operator<=(const random_access_test_iterator &left,
             const random_access_test_iterator &right) noexcept {
    return !(right < left);
  }

  friend constexpr bool
  operator>=(const random_access_test_iterator &left,
             const random_access_test_iterator &right) noexcept {
    return !(left < right);
  }
};

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

template <class Iterator, class Sentinel = Iterator>
struct basic_test_view : tested::ranges::view_base {
  Iterator first{};
  Sentinel last{};

  constexpr Iterator begin() noexcept { return first; }

  constexpr Iterator begin() const noexcept { return first; }

  constexpr Sentinel end() noexcept { return last; }

  constexpr Sentinel end() const noexcept { return last; }
};

template <class T> using input_view = basic_test_view<input_test_iterator<T>>;

template <class T>
using forward_view = basic_test_view<forward_test_iterator<T>>;

template <class T>
using bidirectional_view = basic_test_view<bidirectional_test_iterator<T>>;

template <class T>
using random_access_view = basic_test_view<random_access_test_iterator<T>>;

template <class T> using contiguous_view = basic_test_view<T *>;

template <class T>
using non_common_input_view =
    basic_test_view<input_test_iterator<T>,
                    unsized_sentinel<input_test_iterator<T>>>;

template <class T>
using non_common_forward_view =
    basic_test_view<forward_test_iterator<T>,
                    unsized_sentinel<forward_test_iterator<T>>>;

template <class T>
using non_common_random_access_view =
    basic_test_view<random_access_test_iterator<T>,
                    unsized_sentinel<random_access_test_iterator<T>>>;

template <class R>
concept exactly_input_range =
    tested::ranges::input_range<R> && !tested::ranges::forward_range<R>;

template <class R>
concept exactly_forward_range =
    tested::ranges::forward_range<R> && !tested::ranges::bidirectional_range<R>;

template <class R>
concept exactly_bidirectional_range = tested::ranges::bidirectional_range<R> &&
                                      !tested::ranges::random_access_range<R>;

template <class R>
concept exactly_random_access_range = tested::ranges::random_access_range<R> &&
                                      !tested::ranges::contiguous_range<R>;

template <class R>
concept exactly_contiguous_range = tested::ranges::contiguous_range<R>;

template <class Iterator>
concept has_member_iterator_category =
    requires { typename Iterator::iterator_category; };

template <class Iterator, class Category> consteval bool member_category_is() {
  if constexpr (has_member_iterator_category<Iterator>) {
    return tested::same_as<typename Iterator::iterator_category, Category>;
  } else {
    return false;
  }
}

template <class R, class Category> consteval bool range_member_category_is() {
  return member_category_is<tested::ranges::iterator_t<R>, Category>();
}

template <class R, class Category>
concept legacy_category_is =
    tested::same_as<typename tested::iterator_traits<
                        tested::ranges::iterator_t<R>>::iterator_category,
                    Category>;

struct identity_value {
  constexpr int operator()(int value) const noexcept { return value; }
};

struct identity_reference {
  constexpr int &operator()(int &value) const noexcept { return value; }
};

struct is_nonzero {
  constexpr bool operator()(int value) const noexcept { return value != 0; }
};

struct equal_values {
  constexpr bool operator()(int left, int right) const noexcept {
    return left == right;
  }
};

struct sum_values {
  constexpr int operator()(int left, int right) const noexcept {
    return left + right;
  }
};

struct first_reference {
  constexpr int &operator()(int &left, int &) const noexcept { return left; }
};

struct make_pair_value {
  constexpr tested::pair<int, int> operator()(int value) const noexcept {
    return {value, value};
  }
};

using int_input = input_view<int>;

using int_forward = forward_view<int>;

using int_bidirectional = bidirectional_view<int>;

using int_random_access = random_access_view<int>;

using int_contiguous = contiguous_view<int>;

struct make_forward_view {
  constexpr int_forward operator()(int &) const noexcept { return {}; }
};

/*
 * Verify the test ranges themselves.
 */

static_assert(exactly_input_range<int_input>);

static_assert(exactly_forward_range<int_forward>);

static_assert(exactly_bidirectional_range<int_bidirectional>);

static_assert(exactly_random_access_range<int_random_access>);

static_assert(exactly_contiguous_range<int_contiguous>);

/*
 * transform_view propagates iterator_concept.
 *
 * A value-producing transformation advertises the legacy input
 * category. A reference-producing transformation preserves the base
 * legacy category, except that contiguous is reduced to random access.
 */

using transform_input =
    tested::ranges::transform_view<int_input, identity_value>;

using transform_forward_value =
    tested::ranges::transform_view<int_forward, identity_value>;

using transform_bidirectional_reference =
    tested::ranges::transform_view<int_bidirectional, identity_reference>;

using transform_random_value =
    tested::ranges::transform_view<int_random_access, identity_value>;

using transform_contiguous_reference =
    tested::ranges::transform_view<int_contiguous, identity_reference>;

static_assert(exactly_input_range<transform_input>);

static_assert(exactly_forward_range<transform_forward_value>);

static_assert(exactly_bidirectional_range<transform_bidirectional_reference>);

static_assert(exactly_random_access_range<transform_random_value>);

static_assert(exactly_random_access_range<transform_contiguous_reference>);

static_assert(
    !has_member_iterator_category<tested::ranges::iterator_t<transform_input>>);

static_assert(range_member_category_is<transform_forward_value,
                                       tested::input_iterator_tag>());

static_assert(range_member_category_is<transform_bidirectional_reference,
                                       tested::bidirectional_iterator_tag>());

static_assert(range_member_category_is<transform_random_value,
                                       tested::input_iterator_tag>());

static_assert(range_member_category_is<transform_contiguous_reference,
                                       tested::random_access_iterator_tag>());

/*
 * filter_view can never exceed bidirectional.
 */

using filter_input = tested::ranges::filter_view<int_input, is_nonzero>;

using filter_forward = tested::ranges::filter_view<int_forward, is_nonzero>;

using filter_random =
    tested::ranges::filter_view<int_random_access, is_nonzero>;

static_assert(exactly_input_range<filter_input>);

static_assert(exactly_forward_range<filter_forward>);

static_assert(exactly_bidirectional_range<filter_random>);

static_assert(
    !has_member_iterator_category<tested::ranges::iterator_t<filter_input>>);

static_assert(
    range_member_category_is<filter_forward, tested::forward_iterator_tag>());

static_assert(range_member_category_is<filter_random,
                                       tested::bidirectional_iterator_tag>());

/*
 * take, take_while, drop, and drop_while preserve the underlying
 * iterator concept. They may retain contiguous iteration because they
 * expose the underlying iterator directly.
 */

using take_input = tested::ranges::take_view<int_input>;

using take_forward = tested::ranges::take_view<int_forward>;

using take_bidirectional = tested::ranges::take_view<int_bidirectional>;

using take_random = tested::ranges::take_view<int_random_access>;

using take_contiguous = tested::ranges::take_view<int_contiguous>;

static_assert(exactly_input_range<take_input>);

static_assert(exactly_forward_range<take_forward>);

static_assert(exactly_bidirectional_range<take_bidirectional>);

static_assert(exactly_random_access_range<take_random>);

static_assert(exactly_contiguous_range<take_contiguous>);

using take_while_input = tested::ranges::take_while_view<int_input, is_nonzero>;

using take_while_forward =
    tested::ranges::take_while_view<int_forward, is_nonzero>;

using take_while_bidirectional =
    tested::ranges::take_while_view<int_bidirectional, is_nonzero>;

using take_while_random =
    tested::ranges::take_while_view<int_random_access, is_nonzero>;

using take_while_contiguous =
    tested::ranges::take_while_view<int_contiguous, is_nonzero>;

static_assert(exactly_input_range<take_while_input>);

static_assert(exactly_forward_range<take_while_forward>);

static_assert(exactly_bidirectional_range<take_while_bidirectional>);

static_assert(exactly_random_access_range<take_while_random>);

static_assert(exactly_contiguous_range<take_while_contiguous>);

using drop_input = tested::ranges::drop_view<int_input>;

using drop_forward = tested::ranges::drop_view<int_forward>;

using drop_bidirectional = tested::ranges::drop_view<int_bidirectional>;

using drop_random = tested::ranges::drop_view<int_random_access>;

using drop_contiguous = tested::ranges::drop_view<int_contiguous>;

static_assert(exactly_input_range<drop_input>);

static_assert(exactly_forward_range<drop_forward>);

static_assert(exactly_bidirectional_range<drop_bidirectional>);

static_assert(exactly_random_access_range<drop_random>);

static_assert(exactly_contiguous_range<drop_contiguous>);

using drop_while_input = tested::ranges::drop_while_view<int_input, is_nonzero>;

using drop_while_forward =
    tested::ranges::drop_while_view<int_forward, is_nonzero>;

using drop_while_bidirectional =
    tested::ranges::drop_while_view<int_bidirectional, is_nonzero>;

using drop_while_random =
    tested::ranges::drop_while_view<int_random_access, is_nonzero>;

using drop_while_contiguous =
    tested::ranges::drop_while_view<int_contiguous, is_nonzero>;

static_assert(exactly_input_range<drop_while_input>);

static_assert(exactly_forward_range<drop_while_forward>);

static_assert(exactly_bidirectional_range<drop_while_bidirectional>);

static_assert(exactly_random_access_range<drop_while_random>);

static_assert(exactly_contiguous_range<drop_while_contiguous>);

/*
 * reverse_view preserves bidirectional or random-access traversal but
 * cannot remain contiguous.
 */

using reverse_bidirectional = tested::ranges::reverse_view<int_bidirectional>;

using reverse_random = tested::ranges::reverse_view<int_random_access>;

using reverse_contiguous = tested::ranges::reverse_view<int_contiguous>;

static_assert(exactly_bidirectional_range<reverse_bidirectional>);

static_assert(exactly_random_access_range<reverse_random>);

static_assert(exactly_random_access_range<reverse_contiguous>);

static_assert(legacy_category_is<reverse_bidirectional,
                                 tested::bidirectional_iterator_tag>);

static_assert(
    legacy_category_is<reverse_random, tested::random_access_iterator_tag>);

/*
 * common_view uses common_iterator for distinct iterator/sentinel
 * types. common_iterator can preserve input or forward traversal, but
 * it does not preserve bidirectional or random-access traversal.
 */

using common_input = tested::ranges::common_view<non_common_input_view<int>>;

using common_forward =
    tested::ranges::common_view<non_common_forward_view<int>>;

using common_random =
    tested::ranges::common_view<non_common_random_access_view<int>>;

static_assert(exactly_input_range<common_input>);

static_assert(exactly_forward_range<common_forward>);

static_assert(exactly_forward_range<common_random>);

static_assert(legacy_category_is<common_forward, tested::forward_iterator_tag>);

static_assert(legacy_category_is<common_random, tested::forward_iterator_tag>);

/*
 * elements_view preserves the iterator concept but cannot be
 * contiguous. An rvalue tuple element forces the legacy category down
 * to input.
 */

using pair_type = tested::pair<int, int>;

using pair_forward = forward_view<pair_type>;

using pair_random = random_access_view<pair_type>;

using pair_contiguous = contiguous_view<pair_type>;

using elements_forward = tested::ranges::elements_view<pair_forward, 0>;

using elements_random = tested::ranges::elements_view<pair_random, 0>;

using elements_contiguous = tested::ranges::elements_view<pair_contiguous, 0>;

using pair_value_transform =
    tested::ranges::transform_view<int_contiguous, make_pair_value>;

using elements_prvalue = tested::ranges::elements_view<pair_value_transform, 0>;

static_assert(exactly_forward_range<elements_forward>);

static_assert(exactly_random_access_range<elements_random>);

static_assert(exactly_random_access_range<elements_contiguous>);

static_assert(exactly_random_access_range<elements_prvalue>);

static_assert(
    range_member_category_is<elements_forward, tested::forward_iterator_tag>());

static_assert(range_member_category_is<elements_random,
                                       tested::random_access_iterator_tag>());

static_assert(range_member_category_is<elements_contiguous,
                                       tested::random_access_iterator_tag>());

static_assert(
    range_member_category_is<elements_prvalue, tested::input_iterator_tag>());

/*
 * as_const_view preserves traversal, including contiguous traversal.
 */

using as_const_input = tested::ranges::as_const_view<int_input>;

using as_const_forward = tested::ranges::as_const_view<int_forward>;

using as_const_contiguous = tested::ranges::as_const_view<int_contiguous>;

static_assert(exactly_input_range<as_const_input>);

static_assert(exactly_forward_range<as_const_forward>);

static_assert(exactly_contiguous_range<as_const_contiguous>);

static_assert(
    legacy_category_is<as_const_forward, tested::forward_iterator_tag>);

static_assert(legacy_category_is<as_const_contiguous,
                                 tested::random_access_iterator_tag>);

/*
 * stride_view propagates the iterator concept but cannot be
 * contiguous. Its legacy category is absent for input ranges.
 */

using stride_input = tested::ranges::stride_view<int_input>;

using stride_forward = tested::ranges::stride_view<int_forward>;

using stride_bidirectional = tested::ranges::stride_view<int_bidirectional>;

using stride_random = tested::ranges::stride_view<int_random_access>;

using stride_contiguous = tested::ranges::stride_view<int_contiguous>;

static_assert(exactly_input_range<stride_input>);

static_assert(exactly_forward_range<stride_forward>);

static_assert(exactly_bidirectional_range<stride_bidirectional>);

static_assert(exactly_random_access_range<stride_random>);

static_assert(exactly_random_access_range<stride_contiguous>);

static_assert(
    !has_member_iterator_category<tested::ranges::iterator_t<stride_input>>);

static_assert(
    range_member_category_is<stride_forward, tested::forward_iterator_tag>());

static_assert(range_member_category_is<stride_bidirectional,
                                       tested::bidirectional_iterator_tag>());

static_assert(range_member_category_is<stride_random,
                                       tested::random_access_iterator_tag>());

static_assert(range_member_category_is<stride_contiguous,
                                       tested::random_access_iterator_tag>());

/*
 * enumerate_view preserves the iterator concept but always advertises
 * the legacy input category.
 */

using enumerate_input = tested::ranges::enumerate_view<int_input>;

using enumerate_forward = tested::ranges::enumerate_view<int_forward>;

using enumerate_bidirectional =
    tested::ranges::enumerate_view<int_bidirectional>;

using enumerate_random = tested::ranges::enumerate_view<int_random_access>;

using enumerate_contiguous = tested::ranges::enumerate_view<int_contiguous>;

static_assert(exactly_input_range<enumerate_input>);

static_assert(exactly_forward_range<enumerate_forward>);

static_assert(exactly_bidirectional_range<enumerate_bidirectional>);

static_assert(exactly_random_access_range<enumerate_random>);

static_assert(exactly_random_access_range<enumerate_contiguous>);

static_assert(
    range_member_category_is<enumerate_input, tested::input_iterator_tag>());

static_assert(
    range_member_category_is<enumerate_forward, tested::input_iterator_tag>());

static_assert(range_member_category_is<enumerate_bidirectional,
                                       tested::input_iterator_tag>());

static_assert(
    range_member_category_is<enumerate_random, tested::input_iterator_tag>());

/*
 * zip_view uses the weakest iterator concept of its inputs. Its
 * legacy category is present only when every input is forward, and is
 * always input_iterator_tag.
 */

using zip_input = tested::ranges::zip_view<int_input, int_forward>;

using zip_forward = tested::ranges::zip_view<int_forward, int_bidirectional>;

using zip_bidirectional =
    tested::ranges::zip_view<int_bidirectional, int_bidirectional>;

using zip_random = tested::ranges::zip_view<int_random_access, int_contiguous>;

static_assert(exactly_input_range<zip_input>);

static_assert(exactly_forward_range<zip_forward>);

static_assert(exactly_bidirectional_range<zip_bidirectional>);

static_assert(exactly_random_access_range<zip_random>);

static_assert(
    !has_member_iterator_category<tested::ranges::iterator_t<zip_input>>);

static_assert(
    range_member_category_is<zip_forward, tested::input_iterator_tag>());

static_assert(
    range_member_category_is<zip_bidirectional, tested::input_iterator_tag>());

static_assert(
    range_member_category_is<zip_random, tested::input_iterator_tag>());

/*
 * zip_transform_view keeps zip_view's iterator concept. Its legacy
 * category depends on whether invocation returns a reference.
 */

using zip_transform_input = decltype(tested::ranges::views::zip_transform(
    sum_values{}, tested::declval<int_input &>(),
    tested::declval<int_forward &>()));

using zip_transform_forward_value =
    decltype(tested::ranges::views::zip_transform(
        sum_values{}, tested::declval<int_forward &>(),
        tested::declval<int_forward &>()));

using zip_transform_random_value =
    decltype(tested::ranges::views::zip_transform(
        sum_values{}, tested::declval<int_random_access &>(),
        tested::declval<int_contiguous &>()));

using zip_transform_random_reference =
    decltype(tested::ranges::views::zip_transform(
        first_reference{}, tested::declval<int_random_access &>(),
        tested::declval<int_contiguous &>()));

static_assert(exactly_input_range<zip_transform_input>);

static_assert(exactly_forward_range<zip_transform_forward_value>);

static_assert(exactly_random_access_range<zip_transform_random_value>);

static_assert(exactly_random_access_range<zip_transform_random_reference>);

static_assert(!has_member_iterator_category<
              tested::ranges::iterator_t<zip_transform_input>>);

static_assert(range_member_category_is<zip_transform_forward_value,
                                       tested::input_iterator_tag>());

static_assert(range_member_category_is<zip_transform_random_value,
                                       tested::input_iterator_tag>());

static_assert(range_member_category_is<zip_transform_random_reference,
                                       tested::random_access_iterator_tag>());

/*
 * adjacent_view preserves traversal but always advertises the legacy
 * input category.
 */

using adjacent_forward = tested::ranges::adjacent_view<int_forward, 2>;

using adjacent_bidirectional =
    tested::ranges::adjacent_view<int_bidirectional, 2>;

using adjacent_random = tested::ranges::adjacent_view<int_random_access, 2>;

using adjacent_contiguous = tested::ranges::adjacent_view<int_contiguous, 2>;

static_assert(exactly_forward_range<adjacent_forward>);

static_assert(exactly_bidirectional_range<adjacent_bidirectional>);

static_assert(exactly_random_access_range<adjacent_random>);

static_assert(exactly_random_access_range<adjacent_contiguous>);

static_assert(
    range_member_category_is<adjacent_forward, tested::input_iterator_tag>());

static_assert(range_member_category_is<adjacent_bidirectional,
                                       tested::input_iterator_tag>());

static_assert(
    range_member_category_is<adjacent_random, tested::input_iterator_tag>());

/*
 * adjacent_transform_view retains a stronger legacy category only
 * when its function returns a reference.
 */

using adjacent_transform_forward_value =
    decltype(tested::ranges::views::adjacent_transform<2>(
        tested::declval<int_forward &>(), sum_values{}));

using adjacent_transform_random_value =
    decltype(tested::ranges::views::adjacent_transform<2>(
        tested::declval<int_random_access &>(), sum_values{}));

using adjacent_transform_random_reference =
    decltype(tested::ranges::views::adjacent_transform<2>(
        tested::declval<int_random_access &>(), first_reference{}));

static_assert(exactly_forward_range<adjacent_transform_forward_value>);

static_assert(exactly_random_access_range<adjacent_transform_random_value>);

static_assert(exactly_random_access_range<adjacent_transform_random_reference>);

static_assert(range_member_category_is<adjacent_transform_forward_value,
                                       tested::input_iterator_tag>());

static_assert(range_member_category_is<adjacent_transform_random_value,
                                       tested::input_iterator_tag>());

static_assert(range_member_category_is<adjacent_transform_random_reference,
                                       tested::input_iterator_tag>());

/*
 * slide_view preserves forward, bidirectional, or random-access
 * traversal, but always advertises input_iterator_tag.
 */

using slide_forward = tested::ranges::slide_view<int_forward>;

using slide_bidirectional = tested::ranges::slide_view<int_bidirectional>;

using slide_random = tested::ranges::slide_view<int_random_access>;

using slide_contiguous = tested::ranges::slide_view<int_contiguous>;

static_assert(exactly_forward_range<slide_forward>);

static_assert(exactly_bidirectional_range<slide_bidirectional>);

static_assert(exactly_random_access_range<slide_random>);

static_assert(exactly_random_access_range<slide_contiguous>);

static_assert(
    range_member_category_is<slide_forward, tested::input_iterator_tag>());

static_assert(range_member_category_is<slide_bidirectional,
                                       tested::input_iterator_tag>());

static_assert(
    range_member_category_is<slide_random, tested::input_iterator_tag>());

/*
 * chunk_view has a separate input specialization. The forward
 * specialization preserves stronger iterator concepts, but every
 * outer iterator advertises input_iterator_tag.
 */

using chunk_input = tested::ranges::chunk_view<int_input>;

using chunk_forward = tested::ranges::chunk_view<int_forward>;

using chunk_bidirectional = tested::ranges::chunk_view<int_bidirectional>;

using chunk_random = tested::ranges::chunk_view<int_random_access>;

using chunk_contiguous = tested::ranges::chunk_view<int_contiguous>;

static_assert(exactly_input_range<chunk_input>);

static_assert(exactly_forward_range<chunk_forward>);

static_assert(exactly_bidirectional_range<chunk_bidirectional>);

static_assert(exactly_random_access_range<chunk_random>);

static_assert(exactly_random_access_range<chunk_contiguous>);

static_assert(
    range_member_category_is<chunk_input, tested::input_iterator_tag>());

static_assert(
    range_member_category_is<chunk_forward, tested::input_iterator_tag>());

static_assert(range_member_category_is<chunk_bidirectional,
                                       tested::input_iterator_tag>());

static_assert(
    range_member_category_is<chunk_random, tested::input_iterator_tag>());

/*
 * chunk_by_view is forward or bidirectional. Random-access input is
 * intentionally reduced to bidirectional. Its legacy category is
 * always input_iterator_tag.
 */

using chunk_by_forward =
    tested::ranges::chunk_by_view<int_forward, equal_values>;

using chunk_by_bidirectional =
    tested::ranges::chunk_by_view<int_bidirectional, equal_values>;

using chunk_by_random =
    tested::ranges::chunk_by_view<int_random_access, equal_values>;

static_assert(exactly_forward_range<chunk_by_forward>);

static_assert(exactly_bidirectional_range<chunk_by_bidirectional>);

static_assert(exactly_bidirectional_range<chunk_by_random>);

static_assert(
    range_member_category_is<chunk_by_forward, tested::input_iterator_tag>());

static_assert(range_member_category_is<chunk_by_bidirectional,
                                       tested::input_iterator_tag>());

static_assert(
    range_member_category_is<chunk_by_random, tested::input_iterator_tag>());

/*
 * split_view is always forward and always advertises the legacy input
 * category, regardless of the base's stronger traversal.
 */

using single_pattern = tested::ranges::single_view<int>;

using split_forward = tested::ranges::split_view<int_forward, single_pattern>;

using split_random =
    tested::ranges::split_view<int_random_access, single_pattern>;

static_assert(exactly_forward_range<split_forward>);

static_assert(exactly_forward_range<split_random>);

static_assert(
    range_member_category_is<split_forward, tested::input_iterator_tag>());

static_assert(
    range_member_category_is<split_random, tested::input_iterator_tag>());

/*
 * lazy_split_view is input for an input base and forward for a
 * forward base. The input specialization has no iterator_category.
 */

using lazy_split_input =
    tested::ranges::lazy_split_view<int_input, single_pattern>;

using lazy_split_forward =
    tested::ranges::lazy_split_view<int_forward, single_pattern>;

static_assert(exactly_input_range<lazy_split_input>);

static_assert(exactly_forward_range<lazy_split_forward>);

static_assert(!has_member_iterator_category<
              tested::ranges::iterator_t<lazy_split_input>>);

static_assert(
    range_member_category_is<lazy_split_forward, tested::input_iterator_tag>());

/*
 * join_view depends on both outer and inner traversal. It can be
 * bidirectional, but never random access. A prvalue inner range forces
 * input traversal and removes iterator_category.
 */

using join_input_outer = input_view<int_forward>;

using join_forward_outer = forward_view<int_forward>;

using join_bidirectional_outer = bidirectional_view<int_bidirectional>;

using join_random_outer = random_access_view<int_random_access>;

using join_input = tested::ranges::join_view<join_input_outer>;

using join_forward = tested::ranges::join_view<join_forward_outer>;

using join_bidirectional = tested::ranges::join_view<join_bidirectional_outer>;

using join_random = tested::ranges::join_view<join_random_outer>;

using prvalue_inner_outer =
    tested::ranges::transform_view<int_contiguous, make_forward_view>;

using join_prvalue_inner = tested::ranges::join_view<prvalue_inner_outer>;

static_assert(exactly_input_range<join_input>);

static_assert(exactly_forward_range<join_forward>);

static_assert(exactly_bidirectional_range<join_bidirectional>);

static_assert(exactly_bidirectional_range<join_random>);

static_assert(exactly_input_range<join_prvalue_inner>);

static_assert(
    !has_member_iterator_category<tested::ranges::iterator_t<join_input>>);

static_assert(
    range_member_category_is<join_forward, tested::forward_iterator_tag>());

static_assert(range_member_category_is<join_bidirectional,
                                       tested::bidirectional_iterator_tag>());

static_assert(range_member_category_is<join_random,
                                       tested::bidirectional_iterator_tag>());

static_assert(!has_member_iterator_category<
              tested::ranges::iterator_t<join_prvalue_inner>>);

/*
 * cartesian_product_view derives its iterator concept from all
 * component ranges. It never models contiguous_range and its iterator
 * has no member iterator_category.
 */

using cartesian_input =
    tested::ranges::cartesian_product_view<int_input, int_forward>;

using cartesian_forward =
    tested::ranges::cartesian_product_view<int_forward, int_forward>;

using cartesian_bidirectional =
    tested::ranges::cartesian_product_view<int_bidirectional,
                                           int_bidirectional>;

using cartesian_random =
    tested::ranges::cartesian_product_view<int_random_access,
                                           int_random_access>;

using cartesian_contiguous =
    tested::ranges::cartesian_product_view<int_contiguous, int_contiguous>;

static_assert(exactly_input_range<cartesian_input>);

static_assert(exactly_forward_range<cartesian_forward>);

static_assert(exactly_bidirectional_range<cartesian_bidirectional>);

static_assert(exactly_random_access_range<cartesian_random>);

static_assert(exactly_random_access_range<cartesian_contiguous>);

static_assert(
    range_member_category_is<cartesian_input, tested::input_iterator_tag>());

static_assert(
    range_member_category_is<cartesian_forward, tested::input_iterator_tag>());

static_assert(
    range_member_category_is<cartesian_random, tested::input_iterator_tag>());

bool ftl_test() { return true; }