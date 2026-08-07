// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_RANGE_COMPARE_HEADER
#define FTL_RANGE_COMPARE_HEADER

#ifdef FTL_REPLACE_STL
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#include <__algorithms/result.hpp>
#include <__execution/policy_access.hpp>
#else
#include <ftl/functional>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#include <ftl/__algorithms/result.hpp>
#include <ftl/__execution/policy_access.hpp>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_RANGE_COMPARE_NAMESPACE std
#else
#define FTL_RANGE_COMPARE_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace detail {

struct algorithm_equal_to {
  template <class T, class U>
  constexpr bool operator()(T &&left, U &&right) const {
    return static_cast<T &&>(left) == static_cast<U &&>(right);
  }
};

template <class Iterator, class = void>
struct classic_random_access_iterator : false_type {};

template <class Iterator>
struct classic_random_access_iterator<
    Iterator,
    void_t<typename iterator_traits<Iterator>::iterator_category>>
    : bool_constant<
          is_base_of_v<
              random_access_iterator_tag,
              typename iterator_traits<Iterator>::iterator_category>> {};

template <class Iterator>
inline constexpr bool classic_random_access_iterator_v =
    classic_random_access_iterator<Iterator>::value;

template <class Iterator1, class Sentinel1, class Iterator2,
          class Predicate>
constexpr void mismatch_single_end_loop(
    Iterator1 &first1, const Sentinel1 &last1,
    Iterator2 &first2, Predicate &predicate) {
  while (first1 != last1 && predicate(*first1, *first2)) {
    ++first1;
    ++first2;
  }
}

template <class Iterator1, class Sentinel1, class Iterator2,
          class Sentinel2, class Predicate>
constexpr void mismatch_loop(
    Iterator1 &first1, const Sentinel1 &last1,
    Iterator2 &first2, const Sentinel2 &last2,
    Predicate &predicate) {
  while (first1 != last1 && first2 != last2 &&
         predicate(*first1, *first2)) {
    ++first1;
    ++first2;
  }
}

template <class Iterator1, class Sentinel1, class Iterator2,
          class Predicate>
constexpr bool equal_single_end_loop(
    Iterator1 first1, const Sentinel1 &last1,
    Iterator2 first2, Predicate &predicate) {
  for (; first1 != last1; ++first1, ++first2) {
    if (!predicate(*first1, *first2)) {
      return false;
    }
  }

  return true;
}

template <class Iterator1, class Sentinel1, class Iterator2,
          class Sentinel2, class Predicate>
constexpr bool equal_loop(
    Iterator1 first1, const Sentinel1 &last1,
    Iterator2 first2, const Sentinel2 &last2,
    Predicate &predicate) {
  if constexpr (
      same_as<Iterator1, Sentinel1> &&
      same_as<Iterator2, Sentinel2> &&
      classic_random_access_iterator_v<Iterator1> &&
      classic_random_access_iterator_v<Iterator2>) {
    if (last1 - first1 != last2 - first2) {
      return false;
    }
  }

  while (first1 != last1 && first2 != last2) {
    if (!predicate(*first1, *first2)) {
      return false;
    }

    ++first1;
    ++first2;
  }

  return first1 == last1 && first2 == last2;
}

template <class Iterator1, class Sentinel1, class Iterator2,
          class Sentinel2, class Predicate, class Projection1,
          class Projection2>
constexpr void ranges_mismatch_loop(
    Iterator1 &first1, const Sentinel1 &last1,
    Iterator2 &first2, const Sentinel2 &last2,
    Predicate &predicate, Projection1 &projection1,
    Projection2 &projection2) {
  while (first1 != last1 && first2 != last2) {
    if (!FTL_RANGE_COMPARE_NAMESPACE::invoke(
            predicate,
            FTL_RANGE_COMPARE_NAMESPACE::invoke(projection1, *first1),
            FTL_RANGE_COMPARE_NAMESPACE::invoke(projection2, *first2))) {
      return;
    }

    ++first1;
    ++first2;
  }
}

template <class Iterator1, class Sentinel1, class Iterator2,
          class Sentinel2, class Predicate, class Projection1,
          class Projection2>
constexpr bool ranges_equal_loop(
    Iterator1 first1, const Sentinel1 &last1,
    Iterator2 first2, const Sentinel2 &last2,
    Predicate &predicate, Projection1 &projection1,
    Projection2 &projection2) {
  if constexpr (
      sized_sentinel_for<Sentinel1, Iterator1> &&
      sized_sentinel_for<Sentinel2, Iterator2>) {
    if (last1 - first1 != last2 - first2) {
      return false;
    }
  }

  while (first1 != last1 && first2 != last2) {
    if (!FTL_RANGE_COMPARE_NAMESPACE::invoke(
            predicate,
            FTL_RANGE_COMPARE_NAMESPACE::invoke(projection1, *first1),
            FTL_RANGE_COMPARE_NAMESPACE::invoke(projection2, *first2))) {
      return false;
    }

    ++first1;
    ++first2;
  }

  return first1 == last1 && first2 == last2;
}

} // namespace detail

template <class InputIterator1, class InputIterator2>
constexpr pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1,
         InputIterator2 first2) {
  detail::algorithm_equal_to predicate{};

  detail::mismatch_single_end_loop(
      first1, last1, first2, predicate);

  return {
      FTL_RANGE_COMPARE_NAMESPACE::move(first1),
      FTL_RANGE_COMPARE_NAMESPACE::move(first2),
  };
}

template <class ExecutionPolicy, class ForwardIterator1,
          class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
pair<ForwardIterator1, ForwardIterator2>
mismatch(ExecutionPolicy &&, ForwardIterator1 first1,
         ForwardIterator1 last1, ForwardIterator2 first2) {
  return [&]() noexcept {
    detail::algorithm_equal_to predicate{};

    detail::mismatch_single_end_loop(
        first1, last1, first2, predicate);

    return pair<ForwardIterator1, ForwardIterator2>{
        FTL_RANGE_COMPARE_NAMESPACE::move(first1),
        FTL_RANGE_COMPARE_NAMESPACE::move(first2),
    };
  }();
}

template <class InputIterator1, class InputIterator2,
          class BinaryPredicate>
constexpr pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1,
         InputIterator2 first2, BinaryPredicate predicate) {
  detail::mismatch_single_end_loop(
      first1, last1, first2, predicate);

  return {
      FTL_RANGE_COMPARE_NAMESPACE::move(first1),
      FTL_RANGE_COMPARE_NAMESPACE::move(first2),
  };
}

template <class ExecutionPolicy, class ForwardIterator1,
          class ForwardIterator2, class BinaryPredicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
pair<ForwardIterator1, ForwardIterator2>
mismatch(ExecutionPolicy &&, ForwardIterator1 first1,
         ForwardIterator1 last1, ForwardIterator2 first2,
         BinaryPredicate predicate) {
  return [&]() noexcept {
    detail::mismatch_single_end_loop(
        first1, last1, first2, predicate);

    return pair<ForwardIterator1, ForwardIterator2>{
        FTL_RANGE_COMPARE_NAMESPACE::move(first1),
        FTL_RANGE_COMPARE_NAMESPACE::move(first2),
    };
  }();
}

template <class InputIterator1, class InputIterator2>
constexpr pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1,
         InputIterator2 first2, InputIterator2 last2) {
  detail::algorithm_equal_to predicate{};

  detail::mismatch_loop(
      first1, last1, first2, last2, predicate);

  return {
      FTL_RANGE_COMPARE_NAMESPACE::move(first1),
      FTL_RANGE_COMPARE_NAMESPACE::move(first2),
  };
}

template <class ExecutionPolicy, class ForwardIterator1,
          class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
pair<ForwardIterator1, ForwardIterator2>
mismatch(ExecutionPolicy &&, ForwardIterator1 first1,
         ForwardIterator1 last1, ForwardIterator2 first2,
         ForwardIterator2 last2) {
  return [&]() noexcept {
    detail::algorithm_equal_to predicate{};

    detail::mismatch_loop(
        first1, last1, first2, last2, predicate);

    return pair<ForwardIterator1, ForwardIterator2>{
        FTL_RANGE_COMPARE_NAMESPACE::move(first1),
        FTL_RANGE_COMPARE_NAMESPACE::move(first2),
    };
  }();
}

template <class InputIterator1, class InputIterator2,
          class BinaryPredicate>
constexpr pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1,
         InputIterator2 first2, InputIterator2 last2,
         BinaryPredicate predicate) {
  detail::mismatch_loop(
      first1, last1, first2, last2, predicate);

  return {
      FTL_RANGE_COMPARE_NAMESPACE::move(first1),
      FTL_RANGE_COMPARE_NAMESPACE::move(first2),
  };
}

template <class ExecutionPolicy, class ForwardIterator1,
          class ForwardIterator2, class BinaryPredicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
pair<ForwardIterator1, ForwardIterator2>
mismatch(ExecutionPolicy &&, ForwardIterator1 first1,
         ForwardIterator1 last1, ForwardIterator2 first2,
         ForwardIterator2 last2, BinaryPredicate predicate) {
  return [&]() noexcept {
    detail::mismatch_loop(
        first1, last1, first2, last2, predicate);

    return pair<ForwardIterator1, ForwardIterator2>{
        FTL_RANGE_COMPARE_NAMESPACE::move(first1),
        FTL_RANGE_COMPARE_NAMESPACE::move(first2),
    };
  }();
}

template <class InputIterator1, class InputIterator2>
constexpr bool equal(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2) {
  detail::algorithm_equal_to predicate{};

  return detail::equal_single_end_loop(
      first1, last1, first2, predicate);
}

template <class ExecutionPolicy, class ForwardIterator1,
          class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool equal(ExecutionPolicy &&, ForwardIterator1 first1,
           ForwardIterator1 last1, ForwardIterator2 first2) {
  return [&]() noexcept {
    detail::algorithm_equal_to predicate{};

    return detail::equal_single_end_loop(
        first1, last1, first2, predicate);
  }();
}

template <class InputIterator1, class InputIterator2,
          class BinaryPredicate>
constexpr bool equal(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2,
                     BinaryPredicate predicate) {
  return detail::equal_single_end_loop(
      first1, last1, first2, predicate);
}

template <class ExecutionPolicy, class ForwardIterator1,
          class ForwardIterator2, class BinaryPredicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool equal(ExecutionPolicy &&, ForwardIterator1 first1,
           ForwardIterator1 last1, ForwardIterator2 first2,
           BinaryPredicate predicate) {
  return [&]() noexcept {
    return detail::equal_single_end_loop(
        first1, last1, first2, predicate);
  }();
}

template <class InputIterator1, class InputIterator2>
constexpr bool equal(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2, InputIterator2 last2) {
  detail::algorithm_equal_to predicate{};

  return detail::equal_loop(
      first1, last1, first2, last2, predicate);
}

template <class ExecutionPolicy, class ForwardIterator1,
          class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool equal(ExecutionPolicy &&, ForwardIterator1 first1,
           ForwardIterator1 last1, ForwardIterator2 first2,
           ForwardIterator2 last2) {
  return [&]() noexcept {
    detail::algorithm_equal_to predicate{};

    return detail::equal_loop(
        first1, last1, first2, last2, predicate);
  }();
}

template <class InputIterator1, class InputIterator2,
          class BinaryPredicate>
constexpr bool equal(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2, InputIterator2 last2,
                     BinaryPredicate predicate) {
  return detail::equal_loop(
      first1, last1, first2, last2, predicate);
}

template <class ExecutionPolicy, class ForwardIterator1,
          class ForwardIterator2, class BinaryPredicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool equal(ExecutionPolicy &&, ForwardIterator1 first1,
           ForwardIterator1 last1, ForwardIterator2 first2,
           ForwardIterator2 last2, BinaryPredicate predicate) {
  return [&]() noexcept {
    return detail::equal_loop(
        first1, last1, first2, last2, predicate);
  }();
}

namespace ranges {

template <class Iterator1, class Iterator2>
using mismatch_result = in_in_result<Iterator1, Iterator2>;

struct mismatch_fn {
  template <
      input_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
      input_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
      class Predicate = ranges::equal_to, class Projection1 = identity,
      class Projection2 = identity>
    requires indirectly_comparable<
        Iterator1, Iterator2, Predicate, Projection1, Projection2>
  constexpr mismatch_result<Iterator1, Iterator2>
  operator()(Iterator1 first1, Sentinel1 last1,
             Iterator2 first2, Sentinel2 last2,
             Predicate predicate = {},
             Projection1 projection1 = {},
             Projection2 projection2 = {}) const {
    FTL_RANGE_COMPARE_NAMESPACE::detail::ranges_mismatch_loop(
        first1, last1, first2, last2,
        predicate, projection1, projection2);

    return {
        FTL_RANGE_COMPARE_NAMESPACE::move(first1),
        FTL_RANGE_COMPARE_NAMESPACE::move(first2),
    };
  }

  template <
      input_range Range1, input_range Range2,
      class Predicate = ranges::equal_to,
      class Projection1 = identity,
      class Projection2 = identity>
    requires indirectly_comparable<
        iterator_t<Range1>, iterator_t<Range2>,
        Predicate, Projection1, Projection2>
  constexpr mismatch_result<
      borrowed_iterator_t<Range1>,
      borrowed_iterator_t<Range2>>
  operator()(Range1 &&range1, Range2 &&range2,
             Predicate predicate = {},
             Projection1 projection1 = {},
             Projection2 projection2 = {}) const {
    auto result = (*this)(
        ranges::begin(range1), ranges::end(range1),
        ranges::begin(range2), ranges::end(range2),
        FTL_RANGE_COMPARE_NAMESPACE::move(predicate),
        FTL_RANGE_COMPARE_NAMESPACE::move(projection1),
        FTL_RANGE_COMPARE_NAMESPACE::move(projection2));

    return {
        FTL_RANGE_COMPARE_NAMESPACE::move(result.in1),
        FTL_RANGE_COMPARE_NAMESPACE::move(result.in2),
    };
  }
};

struct equal_fn {
  template <
      input_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
      input_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
      class Predicate = ranges::equal_to, class Projection1 = identity,
      class Projection2 = identity>
    requires indirectly_comparable<
        Iterator1, Iterator2, Predicate, Projection1, Projection2>
  constexpr bool
  operator()(Iterator1 first1, Sentinel1 last1,
             Iterator2 first2, Sentinel2 last2,
             Predicate predicate = {},
             Projection1 projection1 = {},
             Projection2 projection2 = {}) const {
    return FTL_RANGE_COMPARE_NAMESPACE::detail::ranges_equal_loop(
        FTL_RANGE_COMPARE_NAMESPACE::move(first1), last1,
        FTL_RANGE_COMPARE_NAMESPACE::move(first2), last2,
        predicate, projection1, projection2);
  }

  template <
      input_range Range1, input_range Range2,
      class Predicate = ranges::equal_to,
      class Projection1 = identity,
      class Projection2 = identity>
    requires indirectly_comparable<
        iterator_t<Range1>, iterator_t<Range2>,
        Predicate, Projection1, Projection2>
  constexpr bool
  operator()(Range1 &&range1, Range2 &&range2,
             Predicate predicate = {},
             Projection1 projection1 = {},
             Projection2 projection2 = {}) const {
    if constexpr (sized_range<Range1> && sized_range<Range2>) {
      if (ranges::size(range1) != ranges::size(range2)) {
        return false;
      }
    }

    return (*this)(
        ranges::begin(range1), ranges::end(range1),
        ranges::begin(range2), ranges::end(range2),
        FTL_RANGE_COMPARE_NAMESPACE::move(predicate),
        FTL_RANGE_COMPARE_NAMESPACE::move(projection1),
        FTL_RANGE_COMPARE_NAMESPACE::move(projection2));
  }
};

inline constexpr mismatch_fn mismatch{};
inline constexpr equal_fn equal{};

struct starts_with_fn {
  template <
      input_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
      input_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
      class Predicate = ranges::equal_to, class Projection1 = identity,
      class Projection2 = identity>
    requires indirectly_comparable<
        Iterator1, Iterator2, Predicate, Projection1, Projection2>
  constexpr bool
  operator()(Iterator1 first1, Sentinel1 last1,
             Iterator2 first2, Sentinel2 last2,
             Predicate predicate = {},
             Projection1 projection1 = {},
             Projection2 projection2 = {}) const {
    return ranges::mismatch(
               FTL_RANGE_COMPARE_NAMESPACE::move(first1), last1,
               FTL_RANGE_COMPARE_NAMESPACE::move(first2), last2,
               FTL_RANGE_COMPARE_NAMESPACE::move(predicate),
               FTL_RANGE_COMPARE_NAMESPACE::move(projection1),
               FTL_RANGE_COMPARE_NAMESPACE::move(projection2))
               .in2 == last2;
  }

  template <
      input_range Range1, input_range Range2,
      class Predicate = ranges::equal_to,
      class Projection1 = identity,
      class Projection2 = identity>
    requires indirectly_comparable<
        iterator_t<Range1>, iterator_t<Range2>,
        Predicate, Projection1, Projection2>
  constexpr bool
  operator()(Range1 &&range1, Range2 &&range2,
             Predicate predicate = {},
             Projection1 projection1 = {},
             Projection2 projection2 = {}) const {
    return (*this)(
        ranges::begin(range1), ranges::end(range1),
        ranges::begin(range2), ranges::end(range2),
        FTL_RANGE_COMPARE_NAMESPACE::move(predicate),
        FTL_RANGE_COMPARE_NAMESPACE::move(projection1),
        FTL_RANGE_COMPARE_NAMESPACE::move(projection2));
  }
};

struct ends_with_fn {
  template <
      input_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
      input_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
      class Predicate = ranges::equal_to, class Projection1 = identity,
      class Projection2 = identity>
    requires(
        (forward_iterator<Iterator1> ||
         sized_sentinel_for<Sentinel1, Iterator1>) &&
        (forward_iterator<Iterator2> ||
         sized_sentinel_for<Sentinel2, Iterator2>) &&
        indirectly_comparable<
            Iterator1, Iterator2, Predicate,
            Projection1, Projection2>)
  constexpr bool
  operator()(Iterator1 first1, Sentinel1 last1,
             Iterator2 first2, Sentinel2 last2,
             Predicate predicate = {},
             Projection1 projection1 = {},
             Projection2 projection2 = {}) const {
    using difference_type = common_type_t<
        iter_difference_t<Iterator1>,
        iter_difference_t<Iterator2>>;

    const auto length1 = static_cast<difference_type>(
        ranges::distance(first1, last1));

    const auto length2 = static_cast<difference_type>(
        ranges::distance(first2, last2));

    if (length1 < length2) {
      return false;
    }

    ranges::advance(
        first1,
        static_cast<iter_difference_t<Iterator1>>(
            length1 - length2));

    return ranges::equal(
        FTL_RANGE_COMPARE_NAMESPACE::move(first1), last1,
        FTL_RANGE_COMPARE_NAMESPACE::move(first2), last2,
        FTL_RANGE_COMPARE_NAMESPACE::move(predicate),
        FTL_RANGE_COMPARE_NAMESPACE::move(projection1),
        FTL_RANGE_COMPARE_NAMESPACE::move(projection2));
  }

  template <
      input_range Range1, input_range Range2,
      class Predicate = ranges::equal_to,
      class Projection1 = identity,
      class Projection2 = identity>
    requires(
        (forward_range<Range1> || sized_range<Range1>) &&
        (forward_range<Range2> || sized_range<Range2>) &&
        indirectly_comparable<
            iterator_t<Range1>, iterator_t<Range2>,
            Predicate, Projection1, Projection2>)
  constexpr bool
  operator()(Range1 &&range1, Range2 &&range2,
             Predicate predicate = {},
             Projection1 projection1 = {},
             Projection2 projection2 = {}) const {
    return (*this)(
        ranges::begin(range1), ranges::end(range1),
        ranges::begin(range2), ranges::end(range2),
        FTL_RANGE_COMPARE_NAMESPACE::move(predicate),
        FTL_RANGE_COMPARE_NAMESPACE::move(projection1),
        FTL_RANGE_COMPARE_NAMESPACE::move(projection2));
  }
};

inline constexpr starts_with_fn starts_with{};
inline constexpr ends_with_fn ends_with{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_RANGE_COMPARE_NAMESPACE

#endif // FTL_RANGE_COMPARE_HEADER
