// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_MERGE_HEADER
#define FTL_MERGE_HEADER

#include <__algorithms/result.hpp>
#include <__algorithms/reverse_rotate.hpp>
#include <__execution/policy_access.hpp>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>


namespace std {

namespace detail {

using merge_less = std::less<>;

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Output, class Comparator, class Projection1, class Projection2>
constexpr void merge_loop(Iterator1 &first1, const Sentinel1 &last1,
                          Iterator2 &first2, const Sentinel2 &last2,
                          Output &result, Comparator &comparator,
                          Projection1 &projection1, Projection2 &projection2) {
  while (first1 != last1 && first2 != last2) {
    if (std::invoke(
            comparator, std::invoke(projection2, *first2),
            std::invoke(projection1, *first1))) {
      *result = *first2;
      ++first2;
    } else {
      /*
       * Choose the first range when the elements are
       * equivalent, preserving merge stability.
       */
      *result = *first1;
      ++first1;
    }

    ++result;
  }

  while (first1 != last1) {
    *result = *first1;
    ++first1;
    ++result;
  }

  while (first2 != last2) {
    *result = *first2;
    ++first2;
    ++result;
  }
}

template <class Iterator, class Comparator, class Projection>
constexpr Iterator merge_lower_bound(Iterator first,
                                     iter_difference_t<Iterator> length,
                                     Iterator value, Comparator &comparator,
                                     Projection &projection) {
  while (length > 0) {
    const auto half = static_cast<iter_difference_t<Iterator>>(length / 2);

    Iterator middle = first;
    ranges::advance(middle, half);

    if (std::invoke(
            comparator, std::invoke(projection, *middle),
            std::invoke(projection, *value))) {
      first = middle;
      ++first;

      length = static_cast<iter_difference_t<Iterator>>(length - half - 1);
    } else {
      length = half;
    }
  }

  return first;
}

template <class Iterator, class Comparator, class Projection>
constexpr Iterator merge_upper_bound(Iterator first,
                                     iter_difference_t<Iterator> length,
                                     Iterator value, Comparator &comparator,
                                     Projection &projection) {
  while (length > 0) {
    const auto half = static_cast<iter_difference_t<Iterator>>(length / 2);

    Iterator middle = first;
    ranges::advance(middle, half);

    if (!std::invoke(
            comparator, std::invoke(projection, *value),
            std::invoke(projection, *middle))) {
      first = middle;
      ++first;

      length = static_cast<iter_difference_t<Iterator>>(length - half - 1);
    } else {
      length = half;
    }
  }

  return first;
}

/*
 * Allocation-free stable in-place merge.
 *
 * Each recursive step cuts the larger half, locates the
 * matching cut in the other sorted half, rotates the middle
 * blocks, and recursively merges the two resulting pairs.
 */
template <class Iterator, class Comparator, class Projection>
constexpr void
inplace_merge_recursive(Iterator first, Iterator middle, Iterator last,
                        iter_difference_t<Iterator> left_length,
                        iter_difference_t<Iterator> right_length,
                        Comparator &comparator, Projection &projection) {
  if (left_length == 0 || right_length == 0) {
    return;
  }

  if (left_length + right_length == 2) {
    if (std::invoke(
            comparator, std::invoke(projection, *middle),
            std::invoke(projection, *first))) {
      ranges::iter_swap(first, middle);
    }

    return;
  }

  Iterator first_cut;
  Iterator second_cut;

  iter_difference_t<Iterator> left_cut_length;
  iter_difference_t<Iterator> right_cut_length;

  if (left_length > right_length) {
    left_cut_length = static_cast<iter_difference_t<Iterator>>(left_length / 2);

    first_cut = first;
    ranges::advance(first_cut, left_cut_length);

    second_cut = merge_lower_bound(middle, right_length, first_cut, comparator,
                                   projection);

    right_cut_length = ranges::distance(middle, second_cut);
  } else {
    right_cut_length =
        static_cast<iter_difference_t<Iterator>>(right_length / 2);

    second_cut = middle;
    ranges::advance(second_cut, right_cut_length);

    first_cut = merge_upper_bound(first, left_length, second_cut, comparator,
                                  projection);

    left_cut_length = ranges::distance(first, first_cut);
  }

  Iterator new_middle = detail::rotate_loop(first_cut, middle, second_cut);

  inplace_merge_recursive(first, first_cut, new_middle, left_cut_length,
                          right_cut_length, comparator, projection);

  inplace_merge_recursive(
      new_middle, second_cut, last,
      static_cast<iter_difference_t<Iterator>>(left_length - left_cut_length),
      static_cast<iter_difference_t<Iterator>>(right_length - right_cut_length),
      comparator, projection);
}

template <class Iterator, class Sentinel, class Comparator, class Projection>
constexpr Iterator inplace_merge_loop(Iterator first, Iterator middle,
                                      Sentinel last, Comparator &comparator,
                                      Projection &projection) {
  Iterator end = middle;
  ranges::advance(end, last);

  const auto left_length = ranges::distance(first, middle);

  const auto right_length = ranges::distance(middle, end);

  inplace_merge_recursive(first, middle, end, left_length, right_length,
                          comparator, projection);

  return end;
}

} // namespace detail

template <class InputIterator1, class InputIterator2, class OutputIterator>
constexpr OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
                               InputIterator2 first2, InputIterator2 last2,
                               OutputIterator result) {
  detail::merge_less comparator{};
  identity projection1{};
  identity projection2{};

  detail::merge_loop(first1, last1, first2, last2, result, comparator,
                     projection1, projection2);

  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator merge(ExecutionPolicy &&, ForwardIterator1 first1,
                      ForwardIterator1 last1, ForwardIterator2 first2,
                      ForwardIterator2 last2, ForwardIterator result) {
  return [&]() noexcept {
    detail::merge_less comparator{};
    identity projection1{};
    identity projection2{};

    detail::merge_loop(first1, last1, first2, last2, result, comparator,
                       projection1, projection2);

    return result;
  }();
}

template <class InputIterator1, class InputIterator2, class OutputIterator,
          class Comparator>
constexpr OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
                               InputIterator2 first2, InputIterator2 last2,
                               OutputIterator result, Comparator comparator) {
  identity projection1{};
  identity projection2{};

  detail::merge_loop(first1, last1, first2, last2, result, comparator,
                     projection1, projection2);

  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class ForwardIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator merge(ExecutionPolicy &&, ForwardIterator1 first1,
                      ForwardIterator1 last1, ForwardIterator2 first2,
                      ForwardIterator2 last2, ForwardIterator result,
                      Comparator comparator) {
  return [&]() noexcept {
    identity projection1{};
    identity projection2{};

    detail::merge_loop(first1, last1, first2, last2, result, comparator,
                       projection1, projection2);

    return result;
  }();
}

template <class BidirectionalIterator>
void inplace_merge(BidirectionalIterator first, BidirectionalIterator middle,
                   BidirectionalIterator last) {
  detail::merge_less comparator{};
  identity projection{};

  detail::inplace_merge_loop(
      std::move(first), std::move(middle),
      std::move(last), comparator, projection);
}

template <class ExecutionPolicy, class BidirectionalIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void inplace_merge(ExecutionPolicy &&, BidirectionalIterator first,
                   BidirectionalIterator middle, BidirectionalIterator last) {
  [&]() noexcept {
    detail::merge_less comparator{};
    identity projection{};

    detail::inplace_merge_loop(
        std::move(first), std::move(middle),
        std::move(last), comparator, projection);
  }();
}

template <class BidirectionalIterator, class Comparator>
void inplace_merge(BidirectionalIterator first, BidirectionalIterator middle,
                   BidirectionalIterator last, Comparator comparator) {
  identity projection{};

  detail::inplace_merge_loop(
      std::move(first), std::move(middle),
      std::move(last), comparator, projection);
}

template <class ExecutionPolicy, class BidirectionalIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void inplace_merge(ExecutionPolicy &&, BidirectionalIterator first,
                   BidirectionalIterator middle, BidirectionalIterator last,
                   Comparator comparator) {
  [&]() noexcept {
    identity projection{};

    detail::inplace_merge_loop(
        std::move(first), std::move(middle),
        std::move(last), comparator, projection);
  }();
}

namespace ranges {

template <class Iterator1, class Iterator2, class Output>
using merge_result = in_in_out_result<Iterator1, Iterator2, Output>;

struct merge_fn {
  template <input_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
            input_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
            weakly_incrementable Output, class Comparator = ranges::less,
            class Projection1 = identity, class Projection2 = identity>
    requires mergeable<Iterator1, Iterator2, Output, Comparator, Projection1,
                       Projection2>
  constexpr merge_result<Iterator1, Iterator2, Output>
  operator()(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
             Sentinel2 last2, Output result, Comparator comparator = {},
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    std::detail::merge_loop(first1, last1, first2, last2,
                                            result, comparator, projection1,
                                            projection2);

    return {
        std::move(first1),
        std::move(first2),
        std::move(result),
    };
  }

  template <input_range Range1, input_range Range2, weakly_incrementable Output,
            class Comparator = ranges::less, class Projection1 = identity,
            class Projection2 = identity>
    requires mergeable<iterator_t<Range1>, iterator_t<Range2>, Output,
                       Comparator, Projection1, Projection2>
  constexpr merge_result<borrowed_iterator_t<Range1>,
                         borrowed_iterator_t<Range2>, Output>
  operator()(Range1 &&range1, Range2 &&range2, Output result,
             Comparator comparator = {}, Projection1 projection1 = {},
             Projection2 projection2 = {}) const {
    auto converted = (*this)(ranges::begin(range1), ranges::end(range1),
                             ranges::begin(range2), ranges::end(range2),
                             std::move(result),
                             std::move(comparator),
                             std::move(projection1),
                             std::move(projection2));

    return {
        std::move(converted.in1),
        std::move(converted.in2),
        std::move(converted.out),
    };
  }
};

struct inplace_merge_fn {
  template <bidirectional_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Comparator = ranges::less, class Projection = identity>
    requires sortable<Iterator, Comparator, Projection>
  Iterator operator()(Iterator first, Iterator middle, Sentinel last,
                      Comparator comparator = {},
                      Projection projection = {}) const {
    return std::detail::inplace_merge_loop(
        std::move(first), std::move(middle),
        std::move(last), comparator, projection);
  }

  template <bidirectional_range Range, class Comparator = ranges::less,
            class Projection = identity>
    requires sortable<iterator_t<Range>, Comparator, Projection>
  borrowed_iterator_t<Range> operator()(Range &&range, iterator_t<Range> middle,
                                        Comparator comparator = {},
                                        Projection projection = {}) const {
    auto result =
        (*this)(ranges::begin(range), std::move(middle),
                ranges::end(range), std::move(comparator),
                std::move(projection));

    return std::move(result);
  }
};

inline constexpr merge_fn merge{};
inline constexpr inplace_merge_fn inplace_merge{};

} // namespace ranges

} // namespace std


#endif // FTL_MERGE_HEADER
