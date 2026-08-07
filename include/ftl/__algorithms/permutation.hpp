// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_PERMUTATION_HEADER
#define FTL_PERMUTATION_HEADER

#ifdef FTL_REPLACE_STL
#include <__algorithms/result.hpp>
#include <__execution/policy_access.hpp>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#else
#include <ftl/__algorithms/result.hpp>
#include <ftl/__execution/policy_access.hpp>
#include <ftl/functional>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_PERMUTATION_NAMESPACE std
#else
#define FTL_PERMUTATION_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace detail {

template <class Predicate, class Projection1, class Projection2, class T,
          class U>
constexpr bool permutation_equal(Predicate &predicate, Projection1 &projection1,
                                 Projection2 &projection2, T &&left,
                                 U &&right) {
  return static_cast<bool>(FTL_PERMUTATION_NAMESPACE::invoke(
      predicate,
      FTL_PERMUTATION_NAMESPACE::invoke(projection1, static_cast<T &&>(left)),
      FTL_PERMUTATION_NAMESPACE::invoke(projection2,
                                        static_cast<U &&>(right))));
}

template <class Iterator, class Sentinel>
constexpr iter_difference_t<Iterator> permutation_distance(Iterator first,
                                                           Sentinel last) {
  iter_difference_t<Iterator> result = 0;

  for (; first != last; ++first) {
    ++result;
  }

  return result;
}

template <class Iterator, class Sentinel>
constexpr Iterator permutation_end(Iterator first, Sentinel last) {
  while (first != last) {
    ++first;
  }

  return first;
}

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Predicate, class Projection1, class Projection2>
constexpr bool
is_permutation_loop(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
                    Sentinel2 last2, Predicate &predicate,
                    Projection1 &projection1, Projection2 &projection2) {
  if (permutation_distance(first1, last1) !=
      permutation_distance(first2, last2)) {
    return false;
  }

  while (first1 != last1 && permutation_equal(predicate, projection1,
                                              projection2, *first1, *first2)) {
    ++first1;
    ++first2;
  }

  if (first1 == last1) {
    return true;
  }

  Iterator1 range1_begin = first1;
  Iterator2 range2_begin = first2;

  for (Iterator1 current = range1_begin; current != last1; ++current) {
    bool already_counted = false;

    for (Iterator1 previous = range1_begin; previous != current; ++previous) {
      if (permutation_equal(predicate, projection1, projection1, *previous,
                            *current)) {
        already_counted = true;
        break;
      }
    }

    if (already_counted) {
      continue;
    }

    iter_difference_t<Iterator1> count1 = 0;

    for (Iterator1 scan = current; scan != last1; ++scan) {
      if (permutation_equal(predicate, projection1, projection1, *current,
                            *scan)) {
        ++count1;
      }
    }

    iter_difference_t<Iterator2> count2 = 0;

    for (Iterator2 scan = range2_begin; scan != last2; ++scan) {
      if (permutation_equal(predicate, projection1, projection2, *current,
                            *scan)) {
        ++count2;
      }
    }

    if (count1 != count2) {
      return false;
    }
  }

  return true;
}

template <class Comparator, class Projection, class T, class U>
constexpr bool permutation_before(Comparator &comparator,
                                  Projection &projection, T &&left, U &&right) {
  return static_cast<bool>(FTL_PERMUTATION_NAMESPACE::invoke(
      comparator,
      FTL_PERMUTATION_NAMESPACE::invoke(projection, static_cast<T &&>(left)),
      FTL_PERMUTATION_NAMESPACE::invoke(projection, static_cast<U &&>(right))));
}

template <class Iterator>
constexpr void permutation_reverse(Iterator first, Iterator last) {
  while (first != last) {
    Iterator previous = last;
    --previous;

    if (first == previous) {
      return;
    }

    ranges::iter_swap(first, previous);

    ++first;
    last = previous;
  }
}

template <class Iterator, class Comparator, class Projection>
constexpr bool next_permutation_loop(Iterator first, Iterator last,
                                     Comparator &comparator,
                                     Projection &projection) {
  if (first == last) {
    return false;
  }

  Iterator current = last;
  --current;

  if (first == current) {
    return false;
  }

  for (;;) {
    Iterator next = current;
    --current;

    if (permutation_before(comparator, projection, *current, *next)) {
      Iterator successor = last;

      do {
        --successor;
      } while (
          !permutation_before(comparator, projection, *current, *successor));

      ranges::iter_swap(current, successor);

      permutation_reverse(next, last);

      return true;
    }

    if (current == first) {
      permutation_reverse(first, last);

      return false;
    }
  }
}

template <class Iterator, class Comparator, class Projection>
constexpr bool prev_permutation_loop(Iterator first, Iterator last,
                                     Comparator &comparator,
                                     Projection &projection) {
  if (first == last) {
    return false;
  }

  Iterator current = last;
  --current;

  if (first == current) {
    return false;
  }

  for (;;) {
    Iterator next = current;
    --current;

    if (permutation_before(comparator, projection, *next, *current)) {
      Iterator predecessor = last;

      do {
        --predecessor;
      } while (
          !permutation_before(comparator, projection, *predecessor, *current));

      ranges::iter_swap(current, predecessor);

      permutation_reverse(next, last);

      return true;
    }

    if (current == first) {
      permutation_reverse(first, last);

      return false;
    }
  }
}

} // namespace detail

template <class ForwardIterator1, class ForwardIterator2>
constexpr bool is_permutation(ForwardIterator1 first1, ForwardIterator1 last1,
                              ForwardIterator2 first2) {
  equal_to<> predicate{};
  identity projection1{};
  identity projection2{};

  ForwardIterator2 last2 = first2;

  for (ForwardIterator1 scan = first1; scan != last1; ++scan) {
    ++last2;
  }

  return detail::is_permutation_loop(FTL_PERMUTATION_NAMESPACE::move(first1),
                                     FTL_PERMUTATION_NAMESPACE::move(last1),
                                     FTL_PERMUTATION_NAMESPACE::move(first2),
                                     FTL_PERMUTATION_NAMESPACE::move(last2),
                                     predicate, projection1, projection2);
}

template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
constexpr bool is_permutation(ForwardIterator1 first1, ForwardIterator1 last1,
                              ForwardIterator2 first2,
                              BinaryPredicate predicate) {
  identity projection1{};
  identity projection2{};

  ForwardIterator2 last2 = first2;

  for (ForwardIterator1 scan = first1; scan != last1; ++scan) {
    ++last2;
  }

  return detail::is_permutation_loop(FTL_PERMUTATION_NAMESPACE::move(first1),
                                     FTL_PERMUTATION_NAMESPACE::move(last1),
                                     FTL_PERMUTATION_NAMESPACE::move(first2),
                                     FTL_PERMUTATION_NAMESPACE::move(last2),
                                     predicate, projection1, projection2);
}

template <class ForwardIterator1, class ForwardIterator2>
constexpr bool is_permutation(ForwardIterator1 first1, ForwardIterator1 last1,
                              ForwardIterator2 first2, ForwardIterator2 last2) {
  equal_to<> predicate{};
  identity projection1{};
  identity projection2{};

  return detail::is_permutation_loop(FTL_PERMUTATION_NAMESPACE::move(first1),
                                     FTL_PERMUTATION_NAMESPACE::move(last1),
                                     FTL_PERMUTATION_NAMESPACE::move(first2),
                                     FTL_PERMUTATION_NAMESPACE::move(last2),
                                     predicate, projection1, projection2);
}

template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
constexpr bool is_permutation(ForwardIterator1 first1, ForwardIterator1 last1,
                              ForwardIterator2 first2, ForwardIterator2 last2,
                              BinaryPredicate predicate) {
  identity projection1{};
  identity projection2{};

  return detail::is_permutation_loop(FTL_PERMUTATION_NAMESPACE::move(first1),
                                     FTL_PERMUTATION_NAMESPACE::move(last1),
                                     FTL_PERMUTATION_NAMESPACE::move(first2),
                                     FTL_PERMUTATION_NAMESPACE::move(last2),
                                     predicate, projection1, projection2);
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool is_permutation(ExecutionPolicy &&, ForwardIterator1 first1,
                    ForwardIterator1 last1, ForwardIterator2 first2) {
  return [&]() noexcept {
    return FTL_PERMUTATION_NAMESPACE::is_permutation(
        FTL_PERMUTATION_NAMESPACE::move(first1),
        FTL_PERMUTATION_NAMESPACE::move(last1),
        FTL_PERMUTATION_NAMESPACE::move(first2));
  }();
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class BinaryPredicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool is_permutation(ExecutionPolicy &&, ForwardIterator1 first1,
                    ForwardIterator1 last1, ForwardIterator2 first2,
                    BinaryPredicate predicate) {
  return [&]() noexcept {
    return FTL_PERMUTATION_NAMESPACE::is_permutation(
        FTL_PERMUTATION_NAMESPACE::move(first1),
        FTL_PERMUTATION_NAMESPACE::move(last1),
        FTL_PERMUTATION_NAMESPACE::move(first2),
        FTL_PERMUTATION_NAMESPACE::move(predicate));
  }();
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool is_permutation(ExecutionPolicy &&, ForwardIterator1 first1,
                    ForwardIterator1 last1, ForwardIterator2 first2,
                    ForwardIterator2 last2) {
  return [&]() noexcept {
    return FTL_PERMUTATION_NAMESPACE::is_permutation(
        FTL_PERMUTATION_NAMESPACE::move(first1),
        FTL_PERMUTATION_NAMESPACE::move(last1),
        FTL_PERMUTATION_NAMESPACE::move(first2),
        FTL_PERMUTATION_NAMESPACE::move(last2));
  }();
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class BinaryPredicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool is_permutation(ExecutionPolicy &&, ForwardIterator1 first1,
                    ForwardIterator1 last1, ForwardIterator2 first2,
                    ForwardIterator2 last2, BinaryPredicate predicate) {
  return [&]() noexcept {
    return FTL_PERMUTATION_NAMESPACE::is_permutation(
        FTL_PERMUTATION_NAMESPACE::move(first1),
        FTL_PERMUTATION_NAMESPACE::move(last1),
        FTL_PERMUTATION_NAMESPACE::move(first2),
        FTL_PERMUTATION_NAMESPACE::move(last2),
        FTL_PERMUTATION_NAMESPACE::move(predicate));
  }();
}

template <class BidirectionalIterator>
constexpr bool next_permutation(BidirectionalIterator first,
                                BidirectionalIterator last) {
  less<> comparator{};
  identity projection{};

  return detail::next_permutation_loop(FTL_PERMUTATION_NAMESPACE::move(first),
                                       FTL_PERMUTATION_NAMESPACE::move(last),
                                       comparator, projection);
}

template <class BidirectionalIterator, class Comparator>
constexpr bool next_permutation(BidirectionalIterator first,
                                BidirectionalIterator last,
                                Comparator comparator) {
  identity projection{};

  return detail::next_permutation_loop(FTL_PERMUTATION_NAMESPACE::move(first),
                                       FTL_PERMUTATION_NAMESPACE::move(last),
                                       comparator, projection);
}

template <class ExecutionPolicy, class BidirectionalIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool next_permutation(ExecutionPolicy &&, BidirectionalIterator first,
                      BidirectionalIterator last) {
  return [&]() noexcept {
    return FTL_PERMUTATION_NAMESPACE::next_permutation(
        FTL_PERMUTATION_NAMESPACE::move(first),
        FTL_PERMUTATION_NAMESPACE::move(last));
  }();
}

template <class ExecutionPolicy, class BidirectionalIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool next_permutation(ExecutionPolicy &&, BidirectionalIterator first,
                      BidirectionalIterator last, Comparator comparator) {
  return [&]() noexcept {
    return FTL_PERMUTATION_NAMESPACE::next_permutation(
        FTL_PERMUTATION_NAMESPACE::move(first),
        FTL_PERMUTATION_NAMESPACE::move(last),
        FTL_PERMUTATION_NAMESPACE::move(comparator));
  }();
}

template <class BidirectionalIterator>
constexpr bool prev_permutation(BidirectionalIterator first,
                                BidirectionalIterator last) {
  less<> comparator{};
  identity projection{};

  return detail::prev_permutation_loop(FTL_PERMUTATION_NAMESPACE::move(first),
                                       FTL_PERMUTATION_NAMESPACE::move(last),
                                       comparator, projection);
}

template <class BidirectionalIterator, class Comparator>
constexpr bool prev_permutation(BidirectionalIterator first,
                                BidirectionalIterator last,
                                Comparator comparator) {
  identity projection{};

  return detail::prev_permutation_loop(FTL_PERMUTATION_NAMESPACE::move(first),
                                       FTL_PERMUTATION_NAMESPACE::move(last),
                                       comparator, projection);
}

template <class ExecutionPolicy, class BidirectionalIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool prev_permutation(ExecutionPolicy &&, BidirectionalIterator first,
                      BidirectionalIterator last) {
  return [&]() noexcept {
    return FTL_PERMUTATION_NAMESPACE::prev_permutation(
        FTL_PERMUTATION_NAMESPACE::move(first),
        FTL_PERMUTATION_NAMESPACE::move(last));
  }();
}

template <class ExecutionPolicy, class BidirectionalIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool prev_permutation(ExecutionPolicy &&, BidirectionalIterator first,
                      BidirectionalIterator last, Comparator comparator) {
  return [&]() noexcept {
    return FTL_PERMUTATION_NAMESPACE::prev_permutation(
        FTL_PERMUTATION_NAMESPACE::move(first),
        FTL_PERMUTATION_NAMESPACE::move(last),
        FTL_PERMUTATION_NAMESPACE::move(comparator));
  }();
}

namespace ranges {

struct is_permutation_fn {
  template <forward_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
            forward_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
            class Projection1 = identity, class Projection2 = identity,
            indirect_equivalence_relation<projected<Iterator1, Projection1>,
                                          projected<Iterator2, Projection2>>
                Predicate = ranges::equal_to>
  constexpr bool operator()(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
                            Sentinel2 last2, Predicate predicate = {},
                            Projection1 projection1 = {},
                            Projection2 projection2 = {}) const {
    return FTL_PERMUTATION_NAMESPACE::detail::is_permutation_loop(
        FTL_PERMUTATION_NAMESPACE::move(first1),
        FTL_PERMUTATION_NAMESPACE::move(last1),
        FTL_PERMUTATION_NAMESPACE::move(first2),
        FTL_PERMUTATION_NAMESPACE::move(last2), predicate, projection1,
        projection2);
  }

  template <
      forward_range Range1, forward_range Range2, class Projection1 = identity,
      class Projection2 = identity,
      indirect_equivalence_relation<projected<iterator_t<Range1>, Projection1>,
                                    projected<iterator_t<Range2>, Projection2>>
          Predicate = ranges::equal_to>
  constexpr bool
  operator()(Range1 &&range1, Range2 &&range2, Predicate predicate = {},
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    return (*this)(ranges::begin(range1), ranges::end(range1),
                   ranges::begin(range2), ranges::end(range2),
                   FTL_PERMUTATION_NAMESPACE::move(predicate),
                   FTL_PERMUTATION_NAMESPACE::move(projection1),
                   FTL_PERMUTATION_NAMESPACE::move(projection2));
  }
};

template <class Iterator>
using next_permutation_result = in_found_result<Iterator>;

struct next_permutation_fn {
  template <bidirectional_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Comparator = ranges::less, class Projection = identity>
    requires sortable<Iterator, Comparator, Projection>
  constexpr next_permutation_result<Iterator>
  operator()(Iterator first, Sentinel last, Comparator comparator = {},
             Projection projection = {}) const {
    Iterator end =
        FTL_PERMUTATION_NAMESPACE::detail::permutation_end(first, last);

    bool found = FTL_PERMUTATION_NAMESPACE::detail::next_permutation_loop(
        first, end, comparator, projection);

    return {
        FTL_PERMUTATION_NAMESPACE::move(end),
        found,
    };
  }

  template <bidirectional_range Range, class Comparator = ranges::less,
            class Projection = identity>
    requires sortable<iterator_t<Range>, Comparator, Projection>
  constexpr next_permutation_result<borrowed_iterator_t<Range>>
  operator()(Range &&range, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_PERMUTATION_NAMESPACE::move(comparator),
                          FTL_PERMUTATION_NAMESPACE::move(projection));

    return FTL_PERMUTATION_NAMESPACE::move(result);
  }
};

template <class Iterator>
using prev_permutation_result = in_found_result<Iterator>;

struct prev_permutation_fn {
  template <bidirectional_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Comparator = ranges::less, class Projection = identity>
    requires sortable<Iterator, Comparator, Projection>
  constexpr prev_permutation_result<Iterator>
  operator()(Iterator first, Sentinel last, Comparator comparator = {},
             Projection projection = {}) const {
    Iterator end =
        FTL_PERMUTATION_NAMESPACE::detail::permutation_end(first, last);

    bool found = FTL_PERMUTATION_NAMESPACE::detail::prev_permutation_loop(
        first, end, comparator, projection);

    return {
        FTL_PERMUTATION_NAMESPACE::move(end),
        found,
    };
  }

  template <bidirectional_range Range, class Comparator = ranges::less,
            class Projection = identity>
    requires sortable<iterator_t<Range>, Comparator, Projection>
  constexpr prev_permutation_result<borrowed_iterator_t<Range>>
  operator()(Range &&range, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_PERMUTATION_NAMESPACE::move(comparator),
                          FTL_PERMUTATION_NAMESPACE::move(projection));

    return FTL_PERMUTATION_NAMESPACE::move(result);
  }
};

inline constexpr is_permutation_fn is_permutation{};
inline constexpr next_permutation_fn next_permutation{};
inline constexpr prev_permutation_fn prev_permutation{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_PERMUTATION_NAMESPACE

#endif // FTL_PERMUTATION_HEADER
