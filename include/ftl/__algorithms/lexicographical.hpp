// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_LEXICOGRAPHICAL_HEADER
#define FTL_LEXICOGRAPHICAL_HEADER

#ifdef FTL_REPLACE_STL
#include <__execution/policy_access.hpp>
#include <compare>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#else
#include <ftl/__execution/policy_access.hpp>
#include <ftl/compare>
#include <ftl/functional>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_LEXICOGRAPHICAL_NAMESPACE std
#else
#define FTL_LEXICOGRAPHICAL_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace detail {

using lexicographical_less = FTL_LEXICOGRAPHICAL_NAMESPACE::less<>;

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Comparator>
constexpr bool lexicographical_compare_loop(Iterator1 first1, Sentinel1 last1,
                                            Iterator2 first2, Sentinel2 last2,
                                            Comparator &comparator) {
  for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
    if (comparator(*first1, *first2)) {
      return true;
    }

    if (comparator(*first2, *first1)) {
      return false;
    }
  }

  return first1 == last1 && first2 != last2;
}

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Comparator, class Projection1, class Projection2>
constexpr bool ranges_lexicographical_compare_loop(
    Iterator1 first1, Sentinel1 last1, Iterator2 first2, Sentinel2 last2,
    Comparator &comparator, Projection1 &projection1,
    Projection2 &projection2) {
  for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
    if (FTL_LEXICOGRAPHICAL_NAMESPACE::invoke(
            comparator,
            FTL_LEXICOGRAPHICAL_NAMESPACE::invoke(projection1, *first1),
            FTL_LEXICOGRAPHICAL_NAMESPACE::invoke(projection2, *first2))) {
      return true;
    }

    if (FTL_LEXICOGRAPHICAL_NAMESPACE::invoke(
            comparator,
            FTL_LEXICOGRAPHICAL_NAMESPACE::invoke(projection2, *first2),
            FTL_LEXICOGRAPHICAL_NAMESPACE::invoke(projection1, *first1))) {
      return false;
    }
  }

  return first1 == last1 && first2 != last2;
}

template <class T>
inline constexpr bool is_comparison_category_v =
    is_same_v<remove_cvref_t<T>, strong_ordering> ||
    is_same_v<remove_cvref_t<T>, weak_ordering> ||
    is_same_v<remove_cvref_t<T>, partial_ordering>;

template <class Iterator1, class Iterator2, class Comparator>
using lexicographical_three_way_result_t = decltype(declval<Comparator &>()(
    *declval<Iterator1 &>(), *declval<Iterator2 &>()));

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Comparator>
constexpr auto
lexicographical_compare_three_way_loop(Iterator1 first1, Sentinel1 last1,
                                       Iterator2 first2, Sentinel2 last2,
                                       Comparator &comparator)
    -> lexicographical_three_way_result_t<Iterator1, Iterator2, Comparator> {
  using result_type =
      lexicographical_three_way_result_t<Iterator1, Iterator2, Comparator>;

  using category_type = remove_cvref_t<result_type>;

  static_assert(is_comparison_category_v<result_type>);

  for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
    result_type result = comparator(*first1, *first2);

    if (result != 0) {
      return result;
    }
  }

  if (first1 == last1) {
    if (first2 == last2) {
      return category_type::equivalent;
    }

    return category_type::less;
  }

  return category_type::greater;
}

} // namespace detail

template <class InputIterator1, class InputIterator2>
constexpr bool
lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2) {
  detail::lexicographical_less comparator{};

  return detail::lexicographical_compare_loop(
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(first1),
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(last1),
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(first2),
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(last2), comparator);
}

template <class InputIterator1, class InputIterator2, class Comparator>
constexpr bool
lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        Comparator comparator) {
  return detail::lexicographical_compare_loop(
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(first1),
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(last1),
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(first2),
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(last2), comparator);
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool lexicographical_compare(ExecutionPolicy &&, ForwardIterator1 first1,
                             ForwardIterator1 last1, ForwardIterator2 first2,
                             ForwardIterator2 last2) {
  return [&]() noexcept {
    detail::lexicographical_less comparator{};

    return detail::lexicographical_compare_loop(
        FTL_LEXICOGRAPHICAL_NAMESPACE::move(first1),
        FTL_LEXICOGRAPHICAL_NAMESPACE::move(last1),
        FTL_LEXICOGRAPHICAL_NAMESPACE::move(first2),
        FTL_LEXICOGRAPHICAL_NAMESPACE::move(last2), comparator);
  }();
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool lexicographical_compare(ExecutionPolicy &&, ForwardIterator1 first1,
                             ForwardIterator1 last1, ForwardIterator2 first2,
                             ForwardIterator2 last2, Comparator comparator) {
  return [&]() noexcept {
    return detail::lexicographical_compare_loop(
        FTL_LEXICOGRAPHICAL_NAMESPACE::move(first1),
        FTL_LEXICOGRAPHICAL_NAMESPACE::move(last1),
        FTL_LEXICOGRAPHICAL_NAMESPACE::move(first2),
        FTL_LEXICOGRAPHICAL_NAMESPACE::move(last2), comparator);
  }();
}

template <class InputIterator1, class InputIterator2, class Comparator>
constexpr auto
lexicographical_compare_three_way(InputIterator1 first1, InputIterator1 last1,
                                  InputIterator2 first2, InputIterator2 last2,
                                  Comparator comparator)
    -> decltype(comparator(*first1, *first2)) {
  return detail::lexicographical_compare_three_way_loop(
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(first1),
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(last1),
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(first2),
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(last2), comparator);
}

template <class InputIterator1, class InputIterator2>
constexpr auto
lexicographical_compare_three_way(InputIterator1 first1, InputIterator1 last1,
                                  InputIterator2 first2, InputIterator2 last2) {
  compare_three_way comparator{};

  return detail::lexicographical_compare_three_way_loop(
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(first1),
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(last1),
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(first2),
      FTL_LEXICOGRAPHICAL_NAMESPACE::move(last2), comparator);
}

namespace ranges {

struct lexicographical_compare_fn {
  template <input_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
            input_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
            class Projection1 = identity, class Projection2 = identity,
            indirect_strict_weak_order<projected<Iterator1, Projection1>,
                                       projected<Iterator2, Projection2>>
                Comparator = ranges::less>
  constexpr bool operator()(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
                            Sentinel2 last2, Comparator comparator = {},
                            Projection1 projection1 = {},
                            Projection2 projection2 = {}) const {
    return FTL_LEXICOGRAPHICAL_NAMESPACE::detail::
        ranges_lexicographical_compare_loop(
            FTL_LEXICOGRAPHICAL_NAMESPACE::move(first1),
            FTL_LEXICOGRAPHICAL_NAMESPACE::move(last1),
            FTL_LEXICOGRAPHICAL_NAMESPACE::move(first2),
            FTL_LEXICOGRAPHICAL_NAMESPACE::move(last2), comparator, projection1,
            projection2);
  }

  template <
      input_range Range1, input_range Range2, class Projection1 = identity,
      class Projection2 = identity,
      indirect_strict_weak_order<projected<iterator_t<Range1>, Projection1>,
                                 projected<iterator_t<Range2>, Projection2>>
          Comparator = ranges::less>
  constexpr bool
  operator()(Range1 &&range1, Range2 &&range2, Comparator comparator = {},
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    return (*this)(ranges::begin(range1), ranges::end(range1),
                   ranges::begin(range2), ranges::end(range2),
                   FTL_LEXICOGRAPHICAL_NAMESPACE::move(comparator),
                   FTL_LEXICOGRAPHICAL_NAMESPACE::move(projection1),
                   FTL_LEXICOGRAPHICAL_NAMESPACE::move(projection2));
  }
};

inline constexpr lexicographical_compare_fn lexicographical_compare{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_LEXICOGRAPHICAL_NAMESPACE

#endif // FTL_LEXICOGRAPHICAL_HEADER
