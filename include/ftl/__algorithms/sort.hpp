// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_SORT_HEADER
#define FTL_SORT_HEADER

#ifdef FTL_REPLACE_STL
#include <__algorithms/reverse_rotate.hpp>
#include <__execution/policy_access.hpp>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#else
#include <ftl/__algorithms/reverse_rotate.hpp>
#include <ftl/__execution/policy_access.hpp>
#include <ftl/functional>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_SORT_NAMESPACE std
#else
#define FTL_SORT_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace detail {

struct algorithm_less {
  template <class T, class U>
  constexpr bool operator()(T &&left, U &&right) const {
    return static_cast<T &&>(left) < static_cast<U &&>(right);
  }
};

template <class Comparator, class Projection, class T, class U>
constexpr bool sort_before(Comparator &comparator, Projection &projection,
                           T &&left, U &&right) {
  return FTL_SORT_NAMESPACE::invoke(
      comparator,
      FTL_SORT_NAMESPACE::invoke(projection, static_cast<T &&>(left)),
      FTL_SORT_NAMESPACE::invoke(projection, static_cast<U &&>(right)));
}

template <class Iterator, class Comparator, class Projection>
constexpr Iterator is_sorted_until_loop(Iterator first, Iterator last,
                                        Comparator &comparator,
                                        Projection &projection) {
  if (first == last) {
    return first;
  }

  Iterator next = first;
  ++next;

  for (; next != last; ++first, ++next) {
    if (sort_before(comparator, projection, *next, *first)) {
      return next;
    }
  }

  return next;
}

template <class Iterator, class Sentinel, class Comparator, class Projection>
constexpr Iterator ranges_is_sorted_until_loop(Iterator first, Sentinel last,
                                               Comparator &comparator,
                                               Projection &projection) {
  if (first == last) {
    return first;
  }

  Iterator previous = first;
  Iterator current = first;
  ++current;

  for (; current != last; ++previous, ++current) {
    if (sort_before(comparator, projection, *current, *previous)) {
      return current;
    }
  }

  return current;
}

template <class Iterator, class Comparator, class Projection>
constexpr void insertion_sort_loop(Iterator first, Iterator last,
                                   Comparator &comparator,
                                   Projection &projection) {
  if (first == last) {
    return;
  }

  Iterator current = first;
  ++current;

  for (; current != last; ++current) {
    iter_value_t<Iterator> value = ranges::iter_move(current);

    Iterator hole = current;

    while (hole != first) {
      Iterator previous = hole;
      --previous;

      if (!sort_before(comparator, projection, value, *previous)) {
        break;
      }

      *hole = ranges::iter_move(previous);
      hole = previous;
    }

    *hole = FTL_SORT_NAMESPACE::move(value);
  }
}

template <class Iterator, class Comparator, class Projection>
constexpr void sift_down_loop(Iterator first, iter_difference_t<Iterator> root,
                              iter_difference_t<Iterator> count,
                              Comparator &comparator, Projection &projection) {
  while (root < count / 2) {
    auto child = static_cast<iter_difference_t<Iterator>>(root * 2 + 1);

    if (child + 1 < count &&
        sort_before(comparator, projection, *(first + child),
                    *(first + child + 1))) {
      ++child;
    }

    if (!sort_before(comparator, projection, *(first + root),
                     *(first + child))) {
      return;
    }

    ranges::iter_swap(first + root, first + child);

    root = child;
  }
}

template <class Iterator, class Comparator, class Projection>
constexpr void heap_sort_loop(Iterator first, Iterator last,
                              Comparator &comparator, Projection &projection) {
  const auto count = last - first;

  if (count < 2) {
    return;
  }

  auto parent = static_cast<iter_difference_t<Iterator>>(count / 2);

  while (parent > 0) {
    --parent;

    sift_down_loop(first, parent, count, comparator, projection);
  }

  auto remaining = count;

  while (remaining > 1) {
    --remaining;

    ranges::iter_swap(first, first + remaining);

    sift_down_loop(first, 0, remaining, comparator, projection);
  }
}

template <class Iterator, class Comparator, class Projection>
constexpr void order_three(Iterator first, Iterator middle, Iterator last,
                           Comparator &comparator, Projection &projection) {
  if (sort_before(comparator, projection, *middle, *first)) {
    ranges::iter_swap(first, middle);
  }

  if (sort_before(comparator, projection, *last, *middle)) {
    ranges::iter_swap(middle, last);
  }

  if (sort_before(comparator, projection, *middle, *first)) {
    ranges::iter_swap(first, middle);
  }
}

template <class Iterator, class Comparator, class Projection>
constexpr Iterator partition_sort_loop(Iterator first, Iterator last,
                                       Comparator &comparator,
                                       Projection &projection) {
  Iterator last_element = last;
  --last_element;

  Iterator middle = first + (last - first) / 2;

  order_three(first, middle, last_element, comparator, projection);

  /*
   * order_three leaves the median at middle.
   * Keep it at the final slot during partitioning.
   */
  ranges::iter_swap(middle, last_element);

  Iterator result = first;

  for (Iterator current = first; current != last_element; ++current) {
    if (sort_before(comparator, projection, *current, *last_element)) {
      ranges::iter_swap(result, current);
      ++result;
    }
  }

  ranges::iter_swap(result, last_element);
  return result;
}

template <class Difference> constexpr int sort_depth_limit(Difference count) {
  int depth = 0;

  while (count > 1) {
    count /= 2;
    ++depth;
  }

  return depth * 2;
}

template <class Iterator, class Comparator, class Projection>
constexpr void introsort_loop(Iterator first, Iterator last, int depth,
                              Comparator &comparator, Projection &projection) {
  constexpr iter_difference_t<Iterator> insertion_threshold = 16;

  while (last - first > insertion_threshold) {
    if (depth == 0) {
      heap_sort_loop(first, last, comparator, projection);

      return;
    }

    --depth;

    Iterator pivot = partition_sort_loop(first, last, comparator, projection);

    Iterator after_pivot = pivot;
    ++after_pivot;

    /*
     * Recurse into the smaller side and iterate over the
     * larger side to keep stack use logarithmic.
     */
    if (pivot - first < last - after_pivot) {
      introsort_loop(first, pivot, depth, comparator, projection);

      first = after_pivot;
    } else {
      introsort_loop(after_pivot, last, depth, comparator, projection);

      last = pivot;
    }
  }

  insertion_sort_loop(first, last, comparator, projection);
}

template <class Iterator, class Comparator, class Projection>
constexpr void sort_loop(Iterator first, Iterator last, Comparator &comparator,
                         Projection &projection) {
  introsort_loop(first, last, sort_depth_limit(last - first), comparator,
                 projection);
}

template <class Iterator, class Comparator, class Projection>
constexpr Iterator stable_lower_bound(Iterator first, Iterator last,
                                      Iterator value, Comparator &comparator,
                                      Projection &projection) {
  auto length = last - first;

  while (length > 0) {
    const auto half = static_cast<iter_difference_t<Iterator>>(length / 2);

    Iterator middle = first + half;

    if (sort_before(comparator, projection, *middle, *value)) {
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
constexpr Iterator stable_upper_bound(Iterator first, Iterator last,
                                      Iterator value, Comparator &comparator,
                                      Projection &projection) {
  auto length = last - first;

  while (length > 0) {
    const auto half = static_cast<iter_difference_t<Iterator>>(length / 2);

    Iterator middle = first + half;

    if (!sort_before(comparator, projection, *value, *middle)) {
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
constexpr void stable_merge_loop(Iterator first, Iterator middle, Iterator last,
                                 Comparator &comparator,
                                 Projection &projection) {
  if (first == middle || middle == last) {
    return;
  }

  Iterator before_middle = middle;
  --before_middle;

  if (!sort_before(comparator, projection, *middle, *before_middle)) {
    return;
  }

  const auto left_length = middle - first;
  const auto right_length = last - middle;

  if (left_length + right_length == 2) {
    ranges::iter_swap(first, middle);
    return;
  }

  Iterator first_cut;
  Iterator second_cut;

  if (left_length > right_length) {
    first_cut = first + left_length / 2;

    second_cut =
        stable_lower_bound(middle, last, first_cut, comparator, projection);
  } else {
    second_cut = middle + right_length / 2;

    first_cut =
        stable_upper_bound(first, middle, second_cut, comparator, projection);
  }

  Iterator new_middle = detail::rotate_loop(first_cut, middle, second_cut);

  stable_merge_loop(first, first_cut, new_middle, comparator, projection);

  stable_merge_loop(new_middle, second_cut, last, comparator, projection);
}

template <class Iterator, class Comparator, class Projection>
constexpr void stable_sort_recursive(Iterator first, Iterator last,
                                     Comparator &comparator,
                                     Projection &projection) {
  const auto length = last - first;

  if (length < 2) {
    return;
  }

  constexpr iter_difference_t<Iterator> insertion_threshold = 16;

  if (length <= insertion_threshold) {
    insertion_sort_loop(first, last, comparator, projection);

    return;
  }

  Iterator middle = first + length / 2;

  stable_sort_recursive(first, middle, comparator, projection);

  stable_sort_recursive(middle, last, comparator, projection);

  stable_merge_loop(first, middle, last, comparator, projection);
}

template <class Iterator, class Comparator, class Projection>
constexpr void stable_sort_loop(Iterator first, Iterator last,
                                Comparator &comparator,
                                Projection &projection) {
  stable_sort_recursive(first, last, comparator, projection);
}

} // namespace detail

template <class ForwardIterator>
constexpr ForwardIterator is_sorted_until(ForwardIterator first,
                                          ForwardIterator last) {
  detail::algorithm_less comparator{};
  identity projection{};

  return detail::is_sorted_until_loop(FTL_SORT_NAMESPACE::move(first),
                                      FTL_SORT_NAMESPACE::move(last),
                                      comparator, projection);
}

template <class ExecutionPolicy, class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator is_sorted_until(ExecutionPolicy &&, ForwardIterator first,
                                ForwardIterator last) {
  return [&]() noexcept {
    detail::algorithm_less comparator{};
    identity projection{};

    return detail::is_sorted_until_loop(FTL_SORT_NAMESPACE::move(first),
                                        FTL_SORT_NAMESPACE::move(last),
                                        comparator, projection);
  }();
}

template <class ForwardIterator, class Comparator>
constexpr ForwardIterator is_sorted_until(ForwardIterator first,
                                          ForwardIterator last,
                                          Comparator comparator) {
  identity projection{};

  return detail::is_sorted_until_loop(FTL_SORT_NAMESPACE::move(first),
                                      FTL_SORT_NAMESPACE::move(last),
                                      comparator, projection);
}

template <class ExecutionPolicy, class ForwardIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator is_sorted_until(ExecutionPolicy &&, ForwardIterator first,
                                ForwardIterator last, Comparator comparator) {
  return [&]() noexcept {
    identity projection{};

    return detail::is_sorted_until_loop(FTL_SORT_NAMESPACE::move(first),
                                        FTL_SORT_NAMESPACE::move(last),
                                        comparator, projection);
  }();
}

template <class ForwardIterator>
constexpr bool is_sorted(ForwardIterator first, ForwardIterator last) {
  return FTL_SORT_NAMESPACE::is_sorted_until(first, last) == last;
}

template <class ExecutionPolicy, class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool is_sorted(ExecutionPolicy &&policy, ForwardIterator first,
               ForwardIterator last) {
  return FTL_SORT_NAMESPACE::is_sorted_until(
             FTL_SORT_NAMESPACE::forward<ExecutionPolicy>(policy), first,
             last) == last;
}

template <class ForwardIterator, class Comparator>
constexpr bool is_sorted(ForwardIterator first, ForwardIterator last,
                         Comparator comparator) {
  return FTL_SORT_NAMESPACE::is_sorted_until(
             first, last, FTL_SORT_NAMESPACE::move(comparator)) == last;
}

template <class ExecutionPolicy, class ForwardIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool is_sorted(ExecutionPolicy &&policy, ForwardIterator first,
               ForwardIterator last, Comparator comparator) {
  return FTL_SORT_NAMESPACE::is_sorted_until(
             FTL_SORT_NAMESPACE::forward<ExecutionPolicy>(policy), first, last,
             FTL_SORT_NAMESPACE::move(comparator)) == last;
}

template <class RandomAccessIterator>
constexpr void sort(RandomAccessIterator first, RandomAccessIterator last) {
  detail::algorithm_less comparator{};
  identity projection{};

  detail::sort_loop(FTL_SORT_NAMESPACE::move(first),
                    FTL_SORT_NAMESPACE::move(last), comparator, projection);
}

template <class ExecutionPolicy, class RandomAccessIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void sort(ExecutionPolicy &&, RandomAccessIterator first,
          RandomAccessIterator last) {
  [&]() noexcept {
    detail::algorithm_less comparator{};
    identity projection{};

    detail::sort_loop(FTL_SORT_NAMESPACE::move(first),
                      FTL_SORT_NAMESPACE::move(last), comparator, projection);
  }();
}

template <class RandomAccessIterator, class Comparator>
constexpr void sort(RandomAccessIterator first, RandomAccessIterator last,
                    Comparator comparator) {
  identity projection{};

  detail::sort_loop(FTL_SORT_NAMESPACE::move(first),
                    FTL_SORT_NAMESPACE::move(last), comparator, projection);
}

template <class ExecutionPolicy, class RandomAccessIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void sort(ExecutionPolicy &&, RandomAccessIterator first,
          RandomAccessIterator last, Comparator comparator) {
  [&]() noexcept {
    identity projection{};

    detail::sort_loop(FTL_SORT_NAMESPACE::move(first),
                      FTL_SORT_NAMESPACE::move(last), comparator, projection);
  }();
}

template <class RandomAccessIterator>
void stable_sort(RandomAccessIterator first, RandomAccessIterator last) {
  detail::algorithm_less comparator{};
  identity projection{};

  detail::stable_sort_loop(FTL_SORT_NAMESPACE::move(first),
                           FTL_SORT_NAMESPACE::move(last), comparator,
                           projection);
}

template <class ExecutionPolicy, class RandomAccessIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void stable_sort(ExecutionPolicy &&, RandomAccessIterator first,
                 RandomAccessIterator last) {
  [&]() noexcept {
    detail::algorithm_less comparator{};
    identity projection{};

    detail::stable_sort_loop(FTL_SORT_NAMESPACE::move(first),
                             FTL_SORT_NAMESPACE::move(last), comparator,
                             projection);
  }();
}

template <class RandomAccessIterator, class Comparator>
void stable_sort(RandomAccessIterator first, RandomAccessIterator last,
                 Comparator comparator) {
  identity projection{};

  detail::stable_sort_loop(FTL_SORT_NAMESPACE::move(first),
                           FTL_SORT_NAMESPACE::move(last), comparator,
                           projection);
}

template <class ExecutionPolicy, class RandomAccessIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void stable_sort(ExecutionPolicy &&, RandomAccessIterator first,
                 RandomAccessIterator last, Comparator comparator) {
  [&]() noexcept {
    identity projection{};

    detail::stable_sort_loop(FTL_SORT_NAMESPACE::move(first),
                             FTL_SORT_NAMESPACE::move(last), comparator,
                             projection);
  }();
}

namespace ranges {

struct is_sorted_until_fn {
  template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_strict_weak_order<projected<Iterator, Projection>>
                Comparator = ranges::less>
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    return FTL_SORT_NAMESPACE::detail::ranges_is_sorted_until_loop(
        FTL_SORT_NAMESPACE::move(first), FTL_SORT_NAMESPACE::move(last),
        comparator, projection);
  }

  template <forward_range Range, class Projection = identity,
            indirect_strict_weak_order<projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_SORT_NAMESPACE::move(comparator),
                          FTL_SORT_NAMESPACE::move(projection));

    return FTL_SORT_NAMESPACE::move(result);
  }
};

inline constexpr is_sorted_until_fn is_sorted_until{};

struct is_sorted_fn {
  template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_strict_weak_order<projected<Iterator, Projection>>
                Comparator = ranges::less>
  constexpr bool operator()(Iterator first, Sentinel last,
                            Comparator comparator = {},
                            Projection projection = {}) const {
    return ranges::is_sorted_until(FTL_SORT_NAMESPACE::move(first), last,
                                   FTL_SORT_NAMESPACE::move(comparator),
                                   FTL_SORT_NAMESPACE::move(projection)) ==
           last;
  }

  template <forward_range Range, class Projection = identity,
            indirect_strict_weak_order<projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr bool operator()(Range &&range, Comparator comparator = {},
                            Projection projection = {}) const {
    return (*this)(ranges::begin(range), ranges::end(range),
                   FTL_SORT_NAMESPACE::move(comparator),
                   FTL_SORT_NAMESPACE::move(projection));
  }
};

inline constexpr is_sorted_fn is_sorted{};

struct sort_fn {
  template <random_access_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Comparator = ranges::less, class Projection = identity>
    requires sortable<Iterator, Comparator, Projection>
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    Iterator end = first;
    ranges::advance(end, last);

    FTL_SORT_NAMESPACE::detail::sort_loop(first, end, comparator, projection);

    return end;
  }

  template <random_access_range Range, class Comparator = ranges::less,
            class Projection = identity>
    requires sortable<iterator_t<Range>, Comparator, Projection>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_SORT_NAMESPACE::move(comparator),
                          FTL_SORT_NAMESPACE::move(projection));

    return FTL_SORT_NAMESPACE::move(result);
  }
};

struct stable_sort_fn {
  template <random_access_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Comparator = ranges::less, class Projection = identity>
    requires sortable<Iterator, Comparator, Projection>
  Iterator operator()(Iterator first, Sentinel last, Comparator comparator = {},
                      Projection projection = {}) const {
    Iterator end = first;
    ranges::advance(end, last);

    FTL_SORT_NAMESPACE::detail::stable_sort_loop(first, end, comparator,
                                                 projection);

    return end;
  }

  template <random_access_range Range, class Comparator = ranges::less,
            class Projection = identity>
    requires sortable<iterator_t<Range>, Comparator, Projection>
  borrowed_iterator_t<Range> operator()(Range &&range,
                                        Comparator comparator = {},
                                        Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_SORT_NAMESPACE::move(comparator),
                          FTL_SORT_NAMESPACE::move(projection));

    return FTL_SORT_NAMESPACE::move(result);
  }
};

inline constexpr sort_fn sort{};
inline constexpr stable_sort_fn stable_sort{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_SORT_NAMESPACE

#endif // FTL_SORT_HEADER