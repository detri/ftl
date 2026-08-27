// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_SET_OPERATIONS_HEADER
#define FTL_SET_OPERATIONS_HEADER

#include <__algorithms/result.hpp>
#include <__execution/policy_access.hpp>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>


namespace std {

namespace detail {

using set_operations_less = std::less<>;

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Comparator, class Projection1, class Projection2>
constexpr bool includes_loop(Iterator1 first1, const Sentinel1 &last1,
                             Iterator2 first2, const Sentinel2 &last2,
                             Comparator &comparator, Projection1 &projection1,
                             Projection2 &projection2) {
  while (first2 != last2) {
    if (first1 == last1) {
      return false;
    }

    if (std::invoke(
            comparator,
            std::invoke(projection2, *first2),
            std::invoke(projection1, *first1))) {
      return false;
    }

    if (!std::invoke(
            comparator,
            std::invoke(projection1, *first1),
            std::invoke(projection2, *first2))) {
      ++first2;
    }

    ++first1;
  }

  return true;
}

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Output, class Comparator, class Projection1, class Projection2>
constexpr void
set_union_loop(Iterator1 &first1, const Sentinel1 &last1, Iterator2 &first2,
               const Sentinel2 &last2, Output &result, Comparator &comparator,
               Projection1 &projection1, Projection2 &projection2) {
  while (first1 != last1 && first2 != last2) {
    if (std::invoke(
            comparator,
            std::invoke(projection1, *first1),
            std::invoke(projection2, *first2))) {
      *result = *first1;
      ++first1;
    } else if (std::invoke(
                   comparator,
                   std::invoke(projection2, *first2),
                   std::invoke(projection1,
                                                        *first1))) {
      *result = *first2;
      ++first2;
    } else {
      /*
       * Equivalent elements are taken from the first range.
       * Advancing both ranges preserves multiset semantics.
       */
      *result = *first1;
      ++first1;
      ++first2;
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

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Output, class Comparator, class Projection1, class Projection2>
constexpr void set_intersection_loop(Iterator1 &first1, const Sentinel1 &last1,
                                     Iterator2 &first2, const Sentinel2 &last2,
                                     Output &result, Comparator &comparator,
                                     Projection1 &projection1,
                                     Projection2 &projection2) {
  while (first1 != last1 && first2 != last2) {
    if (std::invoke(
            comparator,
            std::invoke(projection1, *first1),
            std::invoke(projection2, *first2))) {
      ++first1;
    } else if (std::invoke(
                   comparator,
                   std::invoke(projection2, *first2),
                   std::invoke(projection1,
                                                        *first1))) {
      ++first2;
    } else {
      /*
       * Intersection elements come from the first range.
       */
      *result = *first1;
      ++first1;
      ++first2;
      ++result;
    }
  }

  /*
   * The ranges result reports both concrete input ends.
   */
  while (first1 != last1) {
    ++first1;
  }

  while (first2 != last2) {
    ++first2;
  }
}

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Output, class Comparator, class Projection1, class Projection2>
constexpr void set_difference_loop(Iterator1 &first1, const Sentinel1 &last1,
                                   Iterator2 &first2, const Sentinel2 &last2,
                                   Output &result, Comparator &comparator,
                                   Projection1 &projection1,
                                   Projection2 &projection2) {
  while (first1 != last1 && first2 != last2) {
    if (std::invoke(
            comparator,
            std::invoke(projection1, *first1),
            std::invoke(projection2, *first2))) {
      *result = *first1;
      ++first1;
      ++result;
    } else if (std::invoke(
                   comparator,
                   std::invoke(projection2, *first2),
                   std::invoke(projection1,
                                                        *first1))) {
      ++first2;
    } else {
      ++first1;
      ++first2;
    }
  }

  while (first1 != last1) {
    *result = *first1;
    ++first1;
    ++result;
  }
}

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Output, class Comparator, class Projection1, class Projection2>
constexpr void set_symmetric_difference_loop(
    Iterator1 &first1, const Sentinel1 &last1, Iterator2 &first2,
    const Sentinel2 &last2, Output &result, Comparator &comparator,
    Projection1 &projection1, Projection2 &projection2) {
  while (first1 != last1 && first2 != last2) {
    if (std::invoke(
            comparator,
            std::invoke(projection1, *first1),
            std::invoke(projection2, *first2))) {
      *result = *first1;
      ++first1;
      ++result;
    } else if (std::invoke(
                   comparator,
                   std::invoke(projection2, *first2),
                   std::invoke(projection1,
                                                        *first1))) {
      *result = *first2;
      ++first2;
      ++result;
    } else {
      ++first1;
      ++first2;
    }
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

} // namespace detail

template <class InputIterator1, class InputIterator2>
constexpr bool includes(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2) {
  detail::set_operations_less comparator{};
  identity projection1{};
  identity projection2{};

  return detail::includes_loop(std::move(first1),
                               std::move(last1),
                               std::move(first2),
                               std::move(last2),
                               comparator, projection1, projection2);
}

template <class InputIterator1, class InputIterator2, class Comparator>
constexpr bool includes(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        Comparator comparator) {
  identity projection1{};
  identity projection2{};

  return detail::includes_loop(std::move(first1),
                               std::move(last1),
                               std::move(first2),
                               std::move(last2),
                               comparator, projection1, projection2);
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool includes(ExecutionPolicy &&, ForwardIterator1 first1,
              ForwardIterator1 last1, ForwardIterator2 first2,
              ForwardIterator2 last2) {
  return [&]() noexcept {
    return std::includes(first1, last1, first2, last2);
  }();
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool includes(ExecutionPolicy &&, ForwardIterator1 first1,
              ForwardIterator1 last1, ForwardIterator2 first2,
              ForwardIterator2 last2, Comparator comparator) {
  return [&]() noexcept {
    return std::includes(first1, last1, first2, last2,
                                                  comparator);
  }();
}

template <class InputIterator1, class InputIterator2, class OutputIterator>
constexpr OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                                   InputIterator2 first2, InputIterator2 last2,
                                   OutputIterator result) {
  detail::set_operations_less comparator{};
  identity projection1{};
  identity projection2{};

  detail::set_union_loop(first1, last1, first2, last2, result, comparator,
                         projection1, projection2);

  return result;
}

template <class InputIterator1, class InputIterator2, class OutputIterator,
          class Comparator>
constexpr OutputIterator
set_union(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
          InputIterator2 last2, OutputIterator result, Comparator comparator) {
  identity projection1{};
  identity projection2{};

  detail::set_union_loop(first1, last1, first2, last2, result, comparator,
                         projection1, projection2);

  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator set_union(ExecutionPolicy &&, ForwardIterator1 first1,
                          ForwardIterator1 last1, ForwardIterator2 first2,
                          ForwardIterator2 last2, ForwardIterator result) {
  return [&]() noexcept {
    return std::set_union(first1, last1, first2, last2,
                                                   result);
  }();
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class ForwardIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator set_union(ExecutionPolicy &&, ForwardIterator1 first1,
                          ForwardIterator1 last1, ForwardIterator2 first2,
                          ForwardIterator2 last2, ForwardIterator result,
                          Comparator comparator) {
  return [&]() noexcept {
    return std::set_union(first1, last1, first2, last2,
                                                   result, comparator);
  }();
}

template <class InputIterator1, class InputIterator2, class OutputIterator>
constexpr OutputIterator
set_intersection(InputIterator1 first1, InputIterator1 last1,
                 InputIterator2 first2, InputIterator2 last2,
                 OutputIterator result) {
  detail::set_operations_less comparator{};
  identity projection1{};
  identity projection2{};

  detail::set_intersection_loop(first1, last1, first2, last2, result,
                                comparator, projection1, projection2);

  return result;
}

template <class InputIterator1, class InputIterator2, class OutputIterator,
          class Comparator>
constexpr OutputIterator
set_intersection(InputIterator1 first1, InputIterator1 last1,
                 InputIterator2 first2, InputIterator2 last2,
                 OutputIterator result, Comparator comparator) {
  identity projection1{};
  identity projection2{};

  detail::set_intersection_loop(first1, last1, first2, last2, result,
                                comparator, projection1, projection2);

  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator
set_intersection(ExecutionPolicy &&, ForwardIterator1 first1,
                 ForwardIterator1 last1, ForwardIterator2 first2,
                 ForwardIterator2 last2, ForwardIterator result) {
  return [&]() noexcept {
    return std::set_intersection(first1, last1, first2,
                                                          last2, result);
  }();
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class ForwardIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator set_intersection(ExecutionPolicy &&, ForwardIterator1 first1,
                                 ForwardIterator1 last1,
                                 ForwardIterator2 first2,
                                 ForwardIterator2 last2, ForwardIterator result,
                                 Comparator comparator) {
  return [&]() noexcept {
    return std::set_intersection(
        first1, last1, first2, last2, result, comparator);
  }();
}

template <class InputIterator1, class InputIterator2, class OutputIterator>
constexpr OutputIterator
set_difference(InputIterator1 first1, InputIterator1 last1,
               InputIterator2 first2, InputIterator2 last2,
               OutputIterator result) {
  detail::set_operations_less comparator{};
  identity projection1{};
  identity projection2{};

  detail::set_difference_loop(first1, last1, first2, last2, result, comparator,
                              projection1, projection2);

  return result;
}

template <class InputIterator1, class InputIterator2, class OutputIterator,
          class Comparator>
constexpr OutputIterator
set_difference(InputIterator1 first1, InputIterator1 last1,
               InputIterator2 first2, InputIterator2 last2,
               OutputIterator result, Comparator comparator) {
  identity projection1{};
  identity projection2{};

  detail::set_difference_loop(first1, last1, first2, last2, result, comparator,
                              projection1, projection2);

  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator set_difference(ExecutionPolicy &&, ForwardIterator1 first1,
                               ForwardIterator1 last1, ForwardIterator2 first2,
                               ForwardIterator2 last2, ForwardIterator result) {
  return [&]() noexcept {
    return std::set_difference(first1, last1, first2,
                                                        last2, result);
  }();
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class ForwardIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator set_difference(ExecutionPolicy &&, ForwardIterator1 first1,
                               ForwardIterator1 last1, ForwardIterator2 first2,
                               ForwardIterator2 last2, ForwardIterator result,
                               Comparator comparator) {
  return [&]() noexcept {
    return std::set_difference(
        first1, last1, first2, last2, result, comparator);
  }();
}

template <class InputIterator1, class InputIterator2, class OutputIterator>
constexpr OutputIterator
set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                         InputIterator2 first2, InputIterator2 last2,
                         OutputIterator result) {
  detail::set_operations_less comparator{};
  identity projection1{};
  identity projection2{};

  detail::set_symmetric_difference_loop(first1, last1, first2, last2, result,
                                        comparator, projection1, projection2);

  return result;
}

template <class InputIterator1, class InputIterator2, class OutputIterator,
          class Comparator>
constexpr OutputIterator
set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                         InputIterator2 first2, InputIterator2 last2,
                         OutputIterator result, Comparator comparator) {
  identity projection1{};
  identity projection2{};

  detail::set_symmetric_difference_loop(first1, last1, first2, last2, result,
                                        comparator, projection1, projection2);

  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator
set_symmetric_difference(ExecutionPolicy &&, ForwardIterator1 first1,
                         ForwardIterator1 last1, ForwardIterator2 first2,
                         ForwardIterator2 last2, ForwardIterator result) {
  return [&]() noexcept {
    return std::set_symmetric_difference(
        first1, last1, first2, last2, result);
  }();
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class ForwardIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator
set_symmetric_difference(ExecutionPolicy &&, ForwardIterator1 first1,
                         ForwardIterator1 last1, ForwardIterator2 first2,
                         ForwardIterator2 last2, ForwardIterator result,
                         Comparator comparator) {
  return [&]() noexcept {
    return std::set_symmetric_difference(
        first1, last1, first2, last2, result, comparator);
  }();
}

namespace ranges {

template <class Iterator1, class Iterator2, class Output>
using set_union_result = in_in_out_result<Iterator1, Iterator2, Output>;

template <class Iterator1, class Iterator2, class Output>
using set_intersection_result = in_in_out_result<Iterator1, Iterator2, Output>;

template <class Iterator, class Output>
using set_difference_result = in_out_result<Iterator, Output>;

template <class Iterator1, class Iterator2, class Output>
using set_symmetric_difference_result =
    in_in_out_result<Iterator1, Iterator2, Output>;

struct includes_fn {
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
    return std::detail::includes_loop(
        std::move(first1),
        std::move(last1),
        std::move(first2),
        std::move(last2), comparator, projection1,
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
                   std::move(comparator),
                   std::move(projection1),
                   std::move(projection2));
  }
};

struct set_union_fn {
  template <input_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
            input_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
            weakly_incrementable Output, class Comparator = ranges::less,
            class Projection1 = identity, class Projection2 = identity>
    requires mergeable<Iterator1, Iterator2, Output, Comparator, Projection1,
                       Projection2>
  constexpr set_union_result<Iterator1, Iterator2, Output>
  operator()(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
             Sentinel2 last2, Output result, Comparator comparator = {},
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    std::detail::set_union_loop(
        first1, last1, first2, last2, result, comparator, projection1,
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
  constexpr set_union_result<borrowed_iterator_t<Range1>,
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

struct set_intersection_fn {
  template <input_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
            input_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
            weakly_incrementable Output, class Comparator = ranges::less,
            class Projection1 = identity, class Projection2 = identity>
    requires mergeable<Iterator1, Iterator2, Output, Comparator, Projection1,
                       Projection2>
  constexpr set_intersection_result<Iterator1, Iterator2, Output>
  operator()(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
             Sentinel2 last2, Output result, Comparator comparator = {},
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    std::detail::set_intersection_loop(
        first1, last1, first2, last2, result, comparator, projection1,
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
  constexpr set_intersection_result<borrowed_iterator_t<Range1>,
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

struct set_difference_fn {
  template <input_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
            input_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
            weakly_incrementable Output, class Comparator = ranges::less,
            class Projection1 = identity, class Projection2 = identity>
    requires mergeable<Iterator1, Iterator2, Output, Comparator, Projection1,
                       Projection2>
  constexpr set_difference_result<Iterator1, Output>
  operator()(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
             Sentinel2 last2, Output result, Comparator comparator = {},
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    std::detail::set_difference_loop(
        first1, last1, first2, last2, result, comparator, projection1,
        projection2);

    return {
        std::move(first1),
        std::move(result),
    };
  }

  template <input_range Range1, input_range Range2, weakly_incrementable Output,
            class Comparator = ranges::less, class Projection1 = identity,
            class Projection2 = identity>
    requires mergeable<iterator_t<Range1>, iterator_t<Range2>, Output,
                       Comparator, Projection1, Projection2>
  constexpr set_difference_result<borrowed_iterator_t<Range1>, Output>
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
        std::move(converted.in),
        std::move(converted.out),
    };
  }
};

struct set_symmetric_difference_fn {
  template <input_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
            input_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
            weakly_incrementable Output, class Comparator = ranges::less,
            class Projection1 = identity, class Projection2 = identity>
    requires mergeable<Iterator1, Iterator2, Output, Comparator, Projection1,
                       Projection2>
  constexpr set_symmetric_difference_result<Iterator1, Iterator2, Output>
  operator()(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
             Sentinel2 last2, Output result, Comparator comparator = {},
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    std::detail::set_symmetric_difference_loop(
        first1, last1, first2, last2, result, comparator, projection1,
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
  constexpr set_symmetric_difference_result<borrowed_iterator_t<Range1>,
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

inline constexpr includes_fn includes{};
inline constexpr set_union_fn set_union{};
inline constexpr set_intersection_fn set_intersection{};
inline constexpr set_difference_fn set_difference{};
inline constexpr set_symmetric_difference_fn set_symmetric_difference{};

} // namespace ranges

} // namespace std


#endif // FTL_SET_OPERATIONS_HEADER
