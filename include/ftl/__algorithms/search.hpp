// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_SEARCH_HEADER
#define FTL_SEARCH_HEADER

#include <__execution/policy_access.hpp>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>


namespace std {

namespace detail {

struct search_equal_to {
  template <class T, class U>
  constexpr bool operator()(T &&left, U &&right) const {
    return static_cast<T &&>(left) == static_cast<U &&>(right);
  }
};

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Predicate, class Projection1, class Projection2>
constexpr ranges::subrange<Iterator1>
search_subrange_loop(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
                     Sentinel2 last2, Predicate &predicate,
                     Projection1 &projection1, Projection2 &projection2) {
  using difference_type =
      common_type_t<iter_difference_t<Iterator1>, iter_difference_t<Iterator2>>;

  const auto length1 =
      static_cast<difference_type>(ranges::distance(first1, last1));

  const auto length2 =
      static_cast<difference_type>(ranges::distance(first2, last2));

  Iterator1 end1 = first1;
  ranges::advance(end1, static_cast<iter_difference_t<Iterator1>>(length1));

  if (length2 == 0) {
    return {first1, first1};
  }

  if (length1 < length2) {
    return {end1, end1};
  }

  auto candidates = length1 - length2 + 1;

  for (; candidates > 0; --candidates, ++first1) {
    Iterator1 current1 = first1;
    Iterator2 current2 = first2;

    while (current2 != last2 &&
           std::invoke(
               predicate, std::invoke(projection1, *current1),
               std::invoke(projection2, *current2))) {
      ++current1;
      ++current2;
    }

    if (current2 == last2) {
      return {
          std::move(first1),
          std::move(current1),
      };
    }
  }

  return {
      std::move(end1),
      std::move(end1),
  };
}

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Predicate, class Projection1, class Projection2>
constexpr ranges::subrange<Iterator1>
find_end_subrange_loop(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
                       Sentinel2 last2, Predicate &predicate,
                       Projection1 &projection1, Projection2 &projection2) {
  using difference_type =
      common_type_t<iter_difference_t<Iterator1>, iter_difference_t<Iterator2>>;

  const auto length1 =
      static_cast<difference_type>(ranges::distance(first1, last1));

  const auto length2 =
      static_cast<difference_type>(ranges::distance(first2, last2));

  Iterator1 end1 = first1;
  ranges::advance(end1, static_cast<iter_difference_t<Iterator1>>(length1));

  if (length2 == 0 || length1 < length2) {
    return {end1, end1};
  }

  Iterator1 result_first = end1;
  Iterator1 result_last = end1;

  auto candidates = length1 - length2 + 1;

  for (; candidates > 0; --candidates, ++first1) {
    Iterator1 current1 = first1;
    Iterator2 current2 = first2;

    while (current2 != last2 &&
           std::invoke(
               predicate, std::invoke(projection1, *current1),
               std::invoke(projection2, *current2))) {
      ++current1;
      ++current2;
    }

    if (current2 == last2) {
      result_first = first1;
      result_last = current1;
    }
  }

  return {
      std::move(result_first),
      std::move(result_last),
  };
}

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Predicate, class Projection1, class Projection2>
constexpr Iterator1
find_first_of_loop(Iterator1 first1, const Sentinel1 &last1, Iterator2 first2,
                   const Sentinel2 &last2, Predicate &predicate,
                   Projection1 &projection1, Projection2 &projection2) {
  for (; first1 != last1; ++first1) {
    for (Iterator2 current2 = first2; current2 != last2; ++current2) {
      if (std::invoke(
              predicate, std::invoke(projection1, *first1),
              std::invoke(projection2, *current2))) {
        return first1;
      }
    }
  }

  return first1;
}

template <class Iterator, class Sentinel, class Predicate, class Projection>
constexpr Iterator adjacent_find_loop(Iterator first, const Sentinel &last,
                                      Predicate &predicate,
                                      Projection &projection) {
  if (first == last) {
    return first;
  }

  Iterator next = first;
  ++next;

  for (; next != last; ++first, ++next) {
    if (std::invoke(
            predicate, std::invoke(projection, *first),
            std::invoke(projection, *next))) {
      return first;
    }
  }

  return next;
}

template <class Iterator, class Sentinel, class Count, class T, class Predicate,
          class Projection>
constexpr ranges::subrange<Iterator>
search_n_subrange_loop(Iterator first, const Sentinel &last, Count count,
                       const T &value, Predicate &predicate,
                       Projection &projection) {
  if (count <= 0) {
    return {first, first};
  }

  while (first != last) {
    if (!std::invoke(
            predicate, std::invoke(projection, *first),
            value)) {
      ++first;
      continue;
    }

    Iterator candidate = first;
    Iterator current = first;
    Count matched = 1;
    ++current;

    if (matched == count) {
      return {
          std::move(candidate),
          std::move(current),
      };
    }

    while (current != last &&
           std::invoke(
               predicate, std::invoke(projection, *current),
               value)) {
      ++matched;
      ++current;

      if (matched == count) {
        return {
            std::move(candidate),
            std::move(current),
        };
      }
    }

    if (current == last) {
      return {
          std::move(current),
          std::move(current),
      };
    }

    /*
     * current has already been tested and does not match, so skip it
     * rather than invoking the predicate on that element again.
     */
    first = current;
    ++first;
  }

  return {
      std::move(first),
      std::move(first),
  };
}

} // namespace detail

template <class ForwardIterator1, class ForwardIterator2>
constexpr ForwardIterator1
find_end(ForwardIterator1 first1, ForwardIterator1 last1,
         ForwardIterator2 first2, ForwardIterator2 last2) {
  detail::search_equal_to predicate{};
  identity projection1{};
  identity projection2{};

  return detail::find_end_subrange_loop(first1, last1, first2, last2, predicate,
                                        projection1, projection2)
      .begin();
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator1 find_end(ExecutionPolicy &&, ForwardIterator1 first1,
                          ForwardIterator1 last1, ForwardIterator2 first2,
                          ForwardIterator2 last2) {
  return [&]() noexcept {
    detail::search_equal_to predicate{};
    identity projection1{};
    identity projection2{};

    return detail::find_end_subrange_loop(first1, last1, first2, last2,
                                          predicate, projection1, projection2)
        .begin();
  }();
}

template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
constexpr ForwardIterator1
find_end(ForwardIterator1 first1, ForwardIterator1 last1,
         ForwardIterator2 first2, ForwardIterator2 last2,
         BinaryPredicate predicate) {
  identity projection1{};
  identity projection2{};

  return detail::find_end_subrange_loop(first1, last1, first2, last2, predicate,
                                        projection1, projection2)
      .begin();
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class BinaryPredicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator1 find_end(ExecutionPolicy &&, ForwardIterator1 first1,
                          ForwardIterator1 last1, ForwardIterator2 first2,
                          ForwardIterator2 last2, BinaryPredicate predicate) {
  return [&]() noexcept {
    identity projection1{};
    identity projection2{};

    return detail::find_end_subrange_loop(first1, last1, first2, last2,
                                          predicate, projection1, projection2)
        .begin();
  }();
}

template <class InputIterator, class ForwardIterator>
constexpr InputIterator find_first_of(InputIterator first1, InputIterator last1,
                                      ForwardIterator first2,
                                      ForwardIterator last2) {
  detail::search_equal_to predicate{};
  identity projection1{};
  identity projection2{};

  return detail::find_first_of_loop(first1, last1, first2, last2, predicate,
                                    projection1, projection2);
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator1 find_first_of(ExecutionPolicy &&, ForwardIterator1 first1,
                               ForwardIterator1 last1, ForwardIterator2 first2,
                               ForwardIterator2 last2) {
  return [&]() noexcept {
    detail::search_equal_to predicate{};
    identity projection1{};
    identity projection2{};

    return detail::find_first_of_loop(first1, last1, first2, last2, predicate,
                                      projection1, projection2);
  }();
}

template <class InputIterator, class ForwardIterator, class BinaryPredicate>
constexpr InputIterator
find_first_of(InputIterator first1, InputIterator last1, ForwardIterator first2,
              ForwardIterator last2, BinaryPredicate predicate) {
  identity projection1{};
  identity projection2{};

  return detail::find_first_of_loop(first1, last1, first2, last2, predicate,
                                    projection1, projection2);
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class BinaryPredicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator1 find_first_of(ExecutionPolicy &&, ForwardIterator1 first1,
                               ForwardIterator1 last1, ForwardIterator2 first2,
                               ForwardIterator2 last2,
                               BinaryPredicate predicate) {
  return [&]() noexcept {
    identity projection1{};
    identity projection2{};

    return detail::find_first_of_loop(first1, last1, first2, last2, predicate,
                                      projection1, projection2);
  }();
}

template <class ForwardIterator>
constexpr ForwardIterator adjacent_find(ForwardIterator first,
                                        ForwardIterator last) {
  detail::search_equal_to predicate{};
  identity projection{};

  return detail::adjacent_find_loop(first, last, predicate, projection);
}

template <class ExecutionPolicy, class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator adjacent_find(ExecutionPolicy &&, ForwardIterator first,
                              ForwardIterator last) {
  return [&]() noexcept {
    detail::search_equal_to predicate{};
    identity projection{};

    return detail::adjacent_find_loop(first, last, predicate, projection);
  }();
}

template <class ForwardIterator, class BinaryPredicate>
constexpr ForwardIterator adjacent_find(ForwardIterator first,
                                        ForwardIterator last,
                                        BinaryPredicate predicate) {
  identity projection{};

  return detail::adjacent_find_loop(first, last, predicate, projection);
}

template <class ExecutionPolicy, class ForwardIterator, class BinaryPredicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator adjacent_find(ExecutionPolicy &&, ForwardIterator first,
                              ForwardIterator last, BinaryPredicate predicate) {
  return [&]() noexcept {
    identity projection{};

    return detail::adjacent_find_loop(first, last, predicate, projection);
  }();
}

template <class ForwardIterator1, class ForwardIterator2>
constexpr ForwardIterator1
search(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2,
       ForwardIterator2 last2) {
  detail::search_equal_to predicate{};
  identity projection1{};
  identity projection2{};

  return detail::search_subrange_loop(first1, last1, first2, last2, predicate,
                                      projection1, projection2)
      .begin();
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator1 search(ExecutionPolicy &&, ForwardIterator1 first1,
                        ForwardIterator1 last1, ForwardIterator2 first2,
                        ForwardIterator2 last2) {
  return [&]() noexcept {
    detail::search_equal_to predicate{};
    identity projection1{};
    identity projection2{};

    return detail::search_subrange_loop(first1, last1, first2, last2, predicate,
                                        projection1, projection2)
        .begin();
  }();
}

template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
constexpr ForwardIterator1
search(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2,
       ForwardIterator2 last2, BinaryPredicate predicate) {
  identity projection1{};
  identity projection2{};

  return detail::search_subrange_loop(first1, last1, first2, last2, predicate,
                                      projection1, projection2)
      .begin();
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class BinaryPredicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator1 search(ExecutionPolicy &&, ForwardIterator1 first1,
                        ForwardIterator1 last1, ForwardIterator2 first2,
                        ForwardIterator2 last2, BinaryPredicate predicate) {
  return [&]() noexcept {
    identity projection1{};
    identity projection2{};

    return detail::search_subrange_loop(first1, last1, first2, last2, predicate,
                                        projection1, projection2)
        .begin();
  }();
}

template <class ForwardIterator, class Searcher>
constexpr ForwardIterator search(ForwardIterator first, ForwardIterator last,
                                 const Searcher &searcher) {
  return searcher(first, last).first;
}

template <class ForwardIterator, class Size, class T>
constexpr ForwardIterator search_n(ForwardIterator first, ForwardIterator last,
                                   Size count, const T &value) {
  detail::search_equal_to predicate{};
  identity projection{};

  return detail::search_n_subrange_loop(first, last, +count, value, predicate,
                                        projection)
      .begin();
}

template <class ExecutionPolicy, class ForwardIterator, class Size, class T>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator search_n(ExecutionPolicy &&, ForwardIterator first,
                         ForwardIterator last, Size count, const T &value) {
  return [&]() noexcept {
    detail::search_equal_to predicate{};
    identity projection{};

    return detail::search_n_subrange_loop(first, last, +count, value, predicate,
                                          projection)
        .begin();
  }();
}

template <class ForwardIterator, class Size, class T, class BinaryPredicate>
constexpr ForwardIterator search_n(ForwardIterator first, ForwardIterator last,
                                   Size count, const T &value,
                                   BinaryPredicate predicate) {
  identity projection{};

  return detail::search_n_subrange_loop(first, last, +count, value, predicate,
                                        projection)
      .begin();
}

template <class ExecutionPolicy, class ForwardIterator, class Size, class T,
          class BinaryPredicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator search_n(ExecutionPolicy &&, ForwardIterator first,
                         ForwardIterator last, Size count, const T &value,
                         BinaryPredicate predicate) {
  return [&]() noexcept {
    identity projection{};

    return detail::search_n_subrange_loop(first, last, +count, value, predicate,
                                          projection)
        .begin();
  }();
}

namespace ranges {

struct find_end_fn {
  template <forward_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
            forward_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
            class Predicate = ranges::equal_to, class Projection1 = identity,
            class Projection2 = identity>
    requires indirectly_comparable<Iterator1, Iterator2, Predicate, Projection1,
                                   Projection2>
  constexpr subrange<Iterator1>
  operator()(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
             Sentinel2 last2, Predicate predicate = {},
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    return std::detail::find_end_subrange_loop(
        std::move(first1), std::move(last1),
        std::move(first2), std::move(last2),
        predicate, projection1, projection2);
  }

  template <forward_range Range1, forward_range Range2,
            class Predicate = ranges::equal_to, class Projection1 = identity,
            class Projection2 = identity>
    requires indirectly_comparable<iterator_t<Range1>, iterator_t<Range2>,
                                   Predicate, Projection1, Projection2>
  constexpr borrowed_subrange_t<Range1>
  operator()(Range1 &&range1, Range2 &&range2, Predicate predicate = {},
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    auto result = (*this)(ranges::begin(range1), ranges::end(range1),
                          ranges::begin(range2), ranges::end(range2),
                          std::move(predicate),
                          std::move(projection1),
                          std::move(projection2));

    return std::move(result);
  }
};

struct find_first_of_fn {
  template <input_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
            forward_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
            class Predicate = ranges::equal_to, class Projection1 = identity,
            class Projection2 = identity>
    requires indirectly_comparable<Iterator1, Iterator2, Predicate, Projection1,
                                   Projection2>
  constexpr Iterator1
  operator()(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
             Sentinel2 last2, Predicate predicate = {},
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    return std::detail::find_first_of_loop(
        std::move(first1), last1,
        std::move(first2), last2, predicate, projection1,
        projection2);
  }

  template <input_range Range1, forward_range Range2,
            class Predicate = ranges::equal_to, class Projection1 = identity,
            class Projection2 = identity>
    requires indirectly_comparable<iterator_t<Range1>, iterator_t<Range2>,
                                   Predicate, Projection1, Projection2>
  constexpr borrowed_iterator_t<Range1>
  operator()(Range1 &&range1, Range2 &&range2, Predicate predicate = {},
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    auto result = (*this)(ranges::begin(range1), ranges::end(range1),
                          ranges::begin(range2), ranges::end(range2),
                          std::move(predicate),
                          std::move(projection1),
                          std::move(projection2));

    return std::move(result);
  }
};

struct adjacent_find_fn {
  template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_binary_predicate<projected<Iterator, Projection>,
                                      projected<Iterator, Projection>>
                Predicate = ranges::equal_to>
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                Predicate predicate = {},
                                Projection projection = {}) const {
    return std::detail::adjacent_find_loop(
        std::move(first), last, predicate, projection);
  }

  template <forward_range Range, class Projection = identity,
            indirect_binary_predicate<projected<iterator_t<Range>, Projection>,
                                      projected<iterator_t<Range>, Projection>>
                Predicate = ranges::equal_to>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, Predicate predicate = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          std::move(predicate),
                          std::move(projection));

    return std::move(result);
  }
};

struct search_fn {
  template <forward_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
            forward_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
            class Predicate = ranges::equal_to, class Projection1 = identity,
            class Projection2 = identity>
    requires indirectly_comparable<Iterator1, Iterator2, Predicate, Projection1,
                                   Projection2>
  constexpr subrange<Iterator1>
  operator()(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
             Sentinel2 last2, Predicate predicate = {},
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    return std::detail::search_subrange_loop(
        std::move(first1), std::move(last1),
        std::move(first2), std::move(last2),
        predicate, projection1, projection2);
  }

  template <forward_range Range1, forward_range Range2,
            class Predicate = ranges::equal_to, class Projection1 = identity,
            class Projection2 = identity>
    requires indirectly_comparable<iterator_t<Range1>, iterator_t<Range2>,
                                   Predicate, Projection1, Projection2>
  constexpr borrowed_subrange_t<Range1>
  operator()(Range1 &&range1, Range2 &&range2, Predicate predicate = {},
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    auto result = (*this)(ranges::begin(range1), ranges::end(range1),
                          ranges::begin(range2), ranges::end(range2),
                          std::move(predicate),
                          std::move(projection1),
                          std::move(projection2));

    return std::move(result);
  }
};

struct search_n_fn {
  template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel, class T,
            class Predicate = ranges::equal_to, class Projection = identity>
    requires indirectly_comparable<Iterator, const T *, Predicate, Projection>
  constexpr subrange<Iterator>
  operator()(Iterator first, Sentinel last, iter_difference_t<Iterator> count,
             const T &value, Predicate predicate = {},
             Projection projection = {}) const {
    return std::detail::search_n_subrange_loop(
        std::move(first), last, count, value, predicate,
        projection);
  }

  template <forward_range Range, class T, class Predicate = ranges::equal_to,
            class Projection = identity>
    requires indirectly_comparable<iterator_t<Range>, const T *, Predicate,
                                   Projection>
  constexpr borrowed_subrange_t<Range>
  operator()(Range &&range, range_difference_t<Range> count, const T &value,
             Predicate predicate = {}, Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range), count,
                          value, std::move(predicate),
                          std::move(projection));

    return std::move(result);
  }
};

inline constexpr find_end_fn find_end{};
inline constexpr find_first_of_fn find_first_of{};
inline constexpr adjacent_find_fn adjacent_find{};
inline constexpr search_fn search{};
inline constexpr search_n_fn search_n{};

struct contains_subrange_fn {
  template <forward_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
            forward_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
            class Predicate = ranges::equal_to, class Projection1 = identity,
            class Projection2 = identity>
    requires indirectly_comparable<Iterator1, Iterator2, Predicate, Projection1,
                                   Projection2>
  constexpr bool operator()(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
                            Sentinel2 last2, Predicate predicate = {},
                            Projection1 projection1 = {},
                            Projection2 projection2 = {}) const {
    if (first2 == last2) {
      return true;
    }

    return !ranges::search(std::move(first1), last1,
                           std::move(first2), last2,
                           std::move(predicate),
                           std::move(projection1),
                           std::move(projection2))
                .empty();
  }

  template <forward_range Range1, forward_range Range2,
            class Predicate = ranges::equal_to, class Projection1 = identity,
            class Projection2 = identity>
    requires indirectly_comparable<iterator_t<Range1>, iterator_t<Range2>,
                                   Predicate, Projection1, Projection2>
  constexpr bool
  operator()(Range1 &&range1, Range2 &&range2, Predicate predicate = {},
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    return (*this)(ranges::begin(range1), ranges::end(range1),
                   ranges::begin(range2), ranges::end(range2),
                   std::move(predicate),
                   std::move(projection1),
                   std::move(projection2));
  }
};

inline constexpr contains_subrange_fn contains_subrange{};

} // namespace ranges

} // namespace std


#endif // FTL_SEARCH_HEADER