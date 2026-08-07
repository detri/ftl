// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_PARTIAL_SORT_HEADER
#define FTL_PARTIAL_SORT_HEADER

#ifdef FTL_REPLACE_STL
#include <__algorithms/heap.hpp>
#include <__algorithms/result.hpp>
#include <__algorithms/sort.hpp>
#include <__execution/policy_access.hpp>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#else
#include <ftl/__algorithms/heap.hpp>
#include <ftl/__algorithms/result.hpp>
#include <ftl/__algorithms/sort.hpp>
#include <ftl/__execution/policy_access.hpp>
#include <ftl/functional>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_PARTIAL_SORT_NAMESPACE std
#else
#define FTL_PARTIAL_SORT_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace detail {

template <class Iterator, class Comparator, class Projection>
constexpr void partial_sort_loop(Iterator first, Iterator middle, Iterator last,
                                 Comparator &comparator,
                                 Projection &projection) {
  if (first == middle) {
    return;
  }

  detail::make_heap_loop(first, middle, comparator, projection);

  const auto heap_length = middle - first;

  for (Iterator current = middle; current != last; ++current) {
    if (detail::sort_before(comparator, projection, *current, *first)) {
      ranges::iter_swap(first, current);

      detail::heap_sift_down(first, 0, heap_length, comparator, projection);
    }
  }

  detail::sort_heap_loop(first, middle, comparator, projection);
}

template <class InputIterator, class InputSentinel, class OutputIterator,
          class OutputSentinel, class Comparator, class InputProjection,
          class OutputProjection>
constexpr ranges::in_out_result<InputIterator, OutputIterator>
partial_sort_copy_loop(InputIterator first, InputSentinel last,
                       OutputIterator result_first, OutputSentinel result_last,
                       Comparator &comparator,
                       InputProjection &input_projection,
                       OutputProjection &output_projection) {
  OutputIterator result_end = result_first;

  while (first != last && result_end != result_last) {
    *result_end = *first;
    ++first;
    ++result_end;
  }

  detail::make_heap_loop(result_first, result_end, comparator,
                         output_projection);

  if (result_first != result_end) {
    const auto heap_length = result_end - result_first;

    for (; first != last; ++first) {
      if (FTL_PARTIAL_SORT_NAMESPACE::invoke(
              comparator,
              FTL_PARTIAL_SORT_NAMESPACE::invoke(input_projection, *first),
              FTL_PARTIAL_SORT_NAMESPACE::invoke(output_projection,
                                                 *result_first))) {
        *result_first = *first;

        detail::heap_sift_down(result_first, 0, heap_length, comparator,
                               output_projection);
      }
    }
  } else {
    while (first != last) {
      ++first;
    }
  }

  detail::sort_heap_loop(result_first, result_end, comparator,
                         output_projection);

  return {
      FTL_PARTIAL_SORT_NAMESPACE::move(first),
      FTL_PARTIAL_SORT_NAMESPACE::move(result_end),
  };
}

/*
 * Partitions [first, last) into three regions:
 *
 *   [first, result.begin())        < pivot
 *   [result.begin(), result.end()) equivalent to pivot
 *   [result.end(), last)           > pivot
 *
 * The pivot is stored outside the range while partitioning,
 * allowing move-only values and avoiding a quadratic all-equal
 * case in nth_element.
 */
template <class Iterator, class Comparator, class Projection>
constexpr ranges::subrange<Iterator>
nth_partition_loop(Iterator first, Iterator last, Comparator &comparator,
                   Projection &projection) {
  Iterator last_element = last;
  --last_element;

  Iterator pivot_position = first + (last - first) / 2;

  detail::order_three(first, pivot_position, last_element, comparator,
                      projection);

  iter_value_t<Iterator> pivot = ranges::iter_move(pivot_position);

  if (pivot_position != last_element) {
    *pivot_position = ranges::iter_move(last_element);
  }

  Iterator less_end = first;
  Iterator current = first;
  Iterator greater_begin = last_element;

  while (current != greater_begin) {
    if (detail::sort_before(comparator, projection, *current, pivot)) {
      ranges::iter_swap(less_end, current);

      ++less_end;
      ++current;
    } else if (detail::sort_before(comparator, projection, pivot, *current)) {
      --greater_begin;

      ranges::iter_swap(current, greater_begin);
    } else {
      ++current;
    }
  }

  if (greater_begin != last_element) {
    *last_element = ranges::iter_move(greater_begin);
  }

  *greater_begin = FTL_PARTIAL_SORT_NAMESPACE::move(pivot);

  Iterator equal_end = greater_begin;
  ++equal_end;

  return {
      FTL_PARTIAL_SORT_NAMESPACE::move(less_end),
      FTL_PARTIAL_SORT_NAMESPACE::move(equal_end),
  };
}

template <class Iterator, class Comparator, class Projection>
constexpr void nth_element_loop(Iterator first, Iterator nth, Iterator last,
                                Comparator &comparator,
                                Projection &projection) {
  if (nth == last) {
    return;
  }

  constexpr iter_difference_t<Iterator> insertion_threshold = 16;

  while (last - first > insertion_threshold) {
    auto equal_range = nth_partition_loop(first, last, comparator, projection);

    if (nth < equal_range.begin()) {
      last = equal_range.begin();
    } else if (nth >= equal_range.end()) {
      first = equal_range.end();
    } else {
      return;
    }
  }

  detail::insertion_sort_loop(first, last, comparator, projection);
}

} // namespace detail

template <class RandomAccessIterator>
constexpr void partial_sort(RandomAccessIterator first,
                            RandomAccessIterator middle,
                            RandomAccessIterator last) {
  detail::algorithm_less comparator{};
  identity projection{};

  detail::partial_sort_loop(FTL_PARTIAL_SORT_NAMESPACE::move(first),
                            FTL_PARTIAL_SORT_NAMESPACE::move(middle),
                            FTL_PARTIAL_SORT_NAMESPACE::move(last), comparator,
                            projection);
}

template <class RandomAccessIterator, class Comparator>
constexpr void partial_sort(RandomAccessIterator first,
                            RandomAccessIterator middle,
                            RandomAccessIterator last, Comparator comparator) {
  identity projection{};

  detail::partial_sort_loop(FTL_PARTIAL_SORT_NAMESPACE::move(first),
                            FTL_PARTIAL_SORT_NAMESPACE::move(middle),
                            FTL_PARTIAL_SORT_NAMESPACE::move(last), comparator,
                            projection);
}

template <class ExecutionPolicy, class RandomAccessIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void partial_sort(ExecutionPolicy &&, RandomAccessIterator first,
                  RandomAccessIterator middle, RandomAccessIterator last) {
  [&]() noexcept {
    detail::algorithm_less comparator{};
    identity projection{};

    detail::partial_sort_loop(FTL_PARTIAL_SORT_NAMESPACE::move(first),
                              FTL_PARTIAL_SORT_NAMESPACE::move(middle),
                              FTL_PARTIAL_SORT_NAMESPACE::move(last),
                              comparator, projection);
  }();
}

template <class ExecutionPolicy, class RandomAccessIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void partial_sort(ExecutionPolicy &&, RandomAccessIterator first,
                  RandomAccessIterator middle, RandomAccessIterator last,
                  Comparator comparator) {
  [&]() noexcept {
    identity projection{};

    detail::partial_sort_loop(FTL_PARTIAL_SORT_NAMESPACE::move(first),
                              FTL_PARTIAL_SORT_NAMESPACE::move(middle),
                              FTL_PARTIAL_SORT_NAMESPACE::move(last),
                              comparator, projection);
  }();
}

template <class InputIterator, class RandomAccessIterator>
constexpr RandomAccessIterator
partial_sort_copy(InputIterator first, InputIterator last,
                  RandomAccessIterator result_first,
                  RandomAccessIterator result_last) {
  detail::algorithm_less comparator{};
  identity input_projection{};
  identity output_projection{};

  return detail::partial_sort_copy_loop(
             FTL_PARTIAL_SORT_NAMESPACE::move(first),
             FTL_PARTIAL_SORT_NAMESPACE::move(last),
             FTL_PARTIAL_SORT_NAMESPACE::move(result_first),
             FTL_PARTIAL_SORT_NAMESPACE::move(result_last), comparator,
             input_projection, output_projection)
      .out;
}

template <class InputIterator, class RandomAccessIterator, class Comparator>
constexpr RandomAccessIterator
partial_sort_copy(InputIterator first, InputIterator last,
                  RandomAccessIterator result_first,
                  RandomAccessIterator result_last, Comparator comparator) {
  identity input_projection{};
  identity output_projection{};

  return detail::partial_sort_copy_loop(
             FTL_PARTIAL_SORT_NAMESPACE::move(first),
             FTL_PARTIAL_SORT_NAMESPACE::move(last),
             FTL_PARTIAL_SORT_NAMESPACE::move(result_first),
             FTL_PARTIAL_SORT_NAMESPACE::move(result_last), comparator,
             input_projection, output_projection)
      .out;
}

template <class ExecutionPolicy, class ForwardIterator,
          class RandomAccessIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
RandomAccessIterator
partial_sort_copy(ExecutionPolicy &&, ForwardIterator first,
                  ForwardIterator last, RandomAccessIterator result_first,
                  RandomAccessIterator result_last) {
  return [&]() noexcept {
    detail::algorithm_less comparator{};
    identity input_projection{};
    identity output_projection{};

    return detail::partial_sort_copy_loop(
               FTL_PARTIAL_SORT_NAMESPACE::move(first),
               FTL_PARTIAL_SORT_NAMESPACE::move(last),
               FTL_PARTIAL_SORT_NAMESPACE::move(result_first),
               FTL_PARTIAL_SORT_NAMESPACE::move(result_last), comparator,
               input_projection, output_projection)
        .out;
  }();
}

template <class ExecutionPolicy, class ForwardIterator,
          class RandomAccessIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
RandomAccessIterator
partial_sort_copy(ExecutionPolicy &&, ForwardIterator first,
                  ForwardIterator last, RandomAccessIterator result_first,
                  RandomAccessIterator result_last, Comparator comparator) {
  return [&]() noexcept {
    identity input_projection{};
    identity output_projection{};

    return detail::partial_sort_copy_loop(
               FTL_PARTIAL_SORT_NAMESPACE::move(first),
               FTL_PARTIAL_SORT_NAMESPACE::move(last),
               FTL_PARTIAL_SORT_NAMESPACE::move(result_first),
               FTL_PARTIAL_SORT_NAMESPACE::move(result_last), comparator,
               input_projection, output_projection)
        .out;
  }();
}

template <class RandomAccessIterator>
constexpr void nth_element(RandomAccessIterator first, RandomAccessIterator nth,
                           RandomAccessIterator last) {
  detail::algorithm_less comparator{};
  identity projection{};

  detail::nth_element_loop(FTL_PARTIAL_SORT_NAMESPACE::move(first),
                           FTL_PARTIAL_SORT_NAMESPACE::move(nth),
                           FTL_PARTIAL_SORT_NAMESPACE::move(last), comparator,
                           projection);
}

template <class RandomAccessIterator, class Comparator>
constexpr void nth_element(RandomAccessIterator first, RandomAccessIterator nth,
                           RandomAccessIterator last, Comparator comparator) {
  identity projection{};

  detail::nth_element_loop(FTL_PARTIAL_SORT_NAMESPACE::move(first),
                           FTL_PARTIAL_SORT_NAMESPACE::move(nth),
                           FTL_PARTIAL_SORT_NAMESPACE::move(last), comparator,
                           projection);
}

template <class ExecutionPolicy, class RandomAccessIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void nth_element(ExecutionPolicy &&, RandomAccessIterator first,
                 RandomAccessIterator nth, RandomAccessIterator last) {
  [&]() noexcept {
    detail::algorithm_less comparator{};
    identity projection{};

    detail::nth_element_loop(FTL_PARTIAL_SORT_NAMESPACE::move(first),
                             FTL_PARTIAL_SORT_NAMESPACE::move(nth),
                             FTL_PARTIAL_SORT_NAMESPACE::move(last), comparator,
                             projection);
  }();
}

template <class ExecutionPolicy, class RandomAccessIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void nth_element(ExecutionPolicy &&, RandomAccessIterator first,
                 RandomAccessIterator nth, RandomAccessIterator last,
                 Comparator comparator) {
  [&]() noexcept {
    identity projection{};

    detail::nth_element_loop(FTL_PARTIAL_SORT_NAMESPACE::move(first),
                             FTL_PARTIAL_SORT_NAMESPACE::move(nth),
                             FTL_PARTIAL_SORT_NAMESPACE::move(last), comparator,
                             projection);
  }();
}

namespace ranges {

template <class InputIterator, class OutputIterator>
using partial_sort_copy_result = in_out_result<InputIterator, OutputIterator>;

struct partial_sort_fn {
  template <random_access_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Comparator = ranges::less, class Projection = identity>
    requires sortable<Iterator, Comparator, Projection>
  constexpr Iterator operator()(Iterator first, Iterator middle, Sentinel last,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    Iterator end = first;
    ranges::advance(end, last);

    FTL_PARTIAL_SORT_NAMESPACE::detail::partial_sort_loop(
        first, middle, end, comparator, projection);

    return end;
  }

  template <random_access_range Range, class Comparator = ranges::less,
            class Projection = identity>
    requires sortable<iterator_t<Range>, Comparator, Projection>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, iterator_t<Range> middle,
             Comparator comparator = {}, Projection projection = {}) const {
    auto result = (*this)(
        ranges::begin(range), FTL_PARTIAL_SORT_NAMESPACE::move(middle),
        ranges::end(range), FTL_PARTIAL_SORT_NAMESPACE::move(comparator),
        FTL_PARTIAL_SORT_NAMESPACE::move(projection));

    return FTL_PARTIAL_SORT_NAMESPACE::move(result);
  }
};

struct partial_sort_copy_fn {
  template <input_iterator InputIterator,
            sentinel_for<InputIterator> InputSentinel,
            random_access_iterator OutputIterator,
            sentinel_for<OutputIterator> OutputSentinel,
            class Comparator = ranges::less, class InputProjection = identity,
            class OutputProjection = identity>
    requires(indirectly_copyable<InputIterator, OutputIterator> &&
             sortable<OutputIterator, Comparator, OutputProjection> &&
             indirect_strict_weak_order<
                 Comparator, projected<InputIterator, InputProjection>,
                 projected<OutputIterator, OutputProjection>>)
  constexpr partial_sort_copy_result<InputIterator, OutputIterator>
  operator()(InputIterator first, InputSentinel last,
             OutputIterator result_first, OutputSentinel result_last,
             Comparator comparator = {}, InputProjection input_projection = {},
             OutputProjection output_projection = {}) const {
    return FTL_PARTIAL_SORT_NAMESPACE::detail::partial_sort_copy_loop(
        FTL_PARTIAL_SORT_NAMESPACE::move(first),
        FTL_PARTIAL_SORT_NAMESPACE::move(last),
        FTL_PARTIAL_SORT_NAMESPACE::move(result_first),
        FTL_PARTIAL_SORT_NAMESPACE::move(result_last), comparator,
        input_projection, output_projection);
  }

  template <input_range InputRange, random_access_range OutputRange,
            class Comparator = ranges::less, class InputProjection = identity,
            class OutputProjection = identity>
    requires(
        indirectly_copyable<iterator_t<InputRange>, iterator_t<OutputRange>> &&
        sortable<iterator_t<OutputRange>, Comparator, OutputProjection> &&
        indirect_strict_weak_order<
            Comparator, projected<iterator_t<InputRange>, InputProjection>,
            projected<iterator_t<OutputRange>, OutputProjection>>)
  constexpr partial_sort_copy_result<borrowed_iterator_t<InputRange>,
                                     borrowed_iterator_t<OutputRange>>
  operator()(InputRange &&input_range, OutputRange &&output_range,
             Comparator comparator = {}, InputProjection input_projection = {},
             OutputProjection output_projection = {}) const {
    auto converted =
        (*this)(ranges::begin(input_range), ranges::end(input_range),
                ranges::begin(output_range), ranges::end(output_range),
                FTL_PARTIAL_SORT_NAMESPACE::move(comparator),
                FTL_PARTIAL_SORT_NAMESPACE::move(input_projection),
                FTL_PARTIAL_SORT_NAMESPACE::move(output_projection));

    return {
        FTL_PARTIAL_SORT_NAMESPACE::move(converted.in),
        FTL_PARTIAL_SORT_NAMESPACE::move(converted.out),
    };
  }
};

struct nth_element_fn {
  template <random_access_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Comparator = ranges::less, class Projection = identity>
    requires sortable<Iterator, Comparator, Projection>
  constexpr Iterator operator()(Iterator first, Iterator nth, Sentinel last,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    Iterator end = first;
    ranges::advance(end, last);

    FTL_PARTIAL_SORT_NAMESPACE::detail::nth_element_loop(
        first, nth, end, comparator, projection);

    return end;
  }

  template <random_access_range Range, class Comparator = ranges::less,
            class Projection = identity>
    requires sortable<iterator_t<Range>, Comparator, Projection>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, iterator_t<Range> nth, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(
        ranges::begin(range), FTL_PARTIAL_SORT_NAMESPACE::move(nth),
        ranges::end(range), FTL_PARTIAL_SORT_NAMESPACE::move(comparator),
        FTL_PARTIAL_SORT_NAMESPACE::move(projection));

    return FTL_PARTIAL_SORT_NAMESPACE::move(result);
  }
};

inline constexpr partial_sort_fn partial_sort{};
inline constexpr partial_sort_copy_fn partial_sort_copy{};
inline constexpr nth_element_fn nth_element{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_PARTIAL_SORT_NAMESPACE

#endif // FTL_PARTIAL_SORT_HEADER
