// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_COUNT_HEADER
#define FTL_COUNT_HEADER

#ifdef FTL_REPLACE_STL
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#include <__execution/policy_access.hpp>
#else
#include <ftl/functional>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#include <ftl/__execution/policy_access.hpp>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_COUNT_NAMESPACE std
#else
#define FTL_COUNT_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace detail {

template <class Difference, class Iterator, class Sentinel, class T>
constexpr Difference count_loop(Iterator &first, const Sentinel &last,
                                const T &value) {
  Difference result = 0;

  for (; first != last; ++first) {
    if (*first == value) {
      ++result;
    }
  }

  return result;
}

template <class Difference, class Iterator, class Sentinel, class Predicate>
constexpr Difference count_if_loop(Iterator &first, const Sentinel &last,
                                   Predicate &predicate) {
  Difference result = 0;

  for (; first != last; ++first) {
    if (predicate(*first)) {
      ++result;
    }
  }

  return result;
}

template <class Difference, class Iterator, class Sentinel, class T,
          class Projection>
constexpr Difference ranges_count_loop(Iterator &first, const Sentinel &last,
                                       const T &value,
                                       Projection &projection) {
  Difference result = 0;

  for (; first != last; ++first) {
    if (FTL_COUNT_NAMESPACE::invoke(projection, *first) == value) {
      ++result;
    }
  }

  return result;
}

template <class Difference, class Iterator, class Sentinel, class Predicate,
          class Projection>
constexpr Difference ranges_count_if_loop(Iterator &first,
                                          const Sentinel &last,
                                          Predicate &predicate,
                                          Projection &projection) {
  Difference result = 0;

  for (; first != last; ++first) {
    if (FTL_COUNT_NAMESPACE::invoke(
            predicate,
            FTL_COUNT_NAMESPACE::invoke(projection, *first))) {
      ++result;
    }
  }

  return result;
}

} // namespace detail

template <class InputIterator, class T>
constexpr typename iterator_traits<InputIterator>::difference_type
count(InputIterator first, InputIterator last, const T &value) {
  using difference_type =
      typename iterator_traits<InputIterator>::difference_type;

  return detail::count_loop<difference_type>(first, last, value);
}

template <class ExecutionPolicy, class ForwardIterator, class T>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
typename iterator_traits<ForwardIterator>::difference_type
count(ExecutionPolicy &&, ForwardIterator first, ForwardIterator last,
      const T &value) {
  using difference_type =
      typename iterator_traits<ForwardIterator>::difference_type;

  return [&]() noexcept -> difference_type {
    return detail::count_loop<difference_type>(first, last, value);
  }();
}

template <class InputIterator, class Predicate>
constexpr typename iterator_traits<InputIterator>::difference_type
count_if(InputIterator first, InputIterator last, Predicate predicate) {
  using difference_type =
      typename iterator_traits<InputIterator>::difference_type;

  return detail::count_if_loop<difference_type>(first, last, predicate);
}

template <class ExecutionPolicy, class ForwardIterator, class Predicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
typename iterator_traits<ForwardIterator>::difference_type
count_if(ExecutionPolicy &&, ForwardIterator first, ForwardIterator last,
         Predicate predicate) {
  using difference_type =
      typename iterator_traits<ForwardIterator>::difference_type;

  return [&]() noexcept -> difference_type {
    return detail::count_if_loop<difference_type>(first, last, predicate);
  }();
}

namespace ranges {

struct count_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel, class T,
            class Projection = identity>
    requires indirect_binary_predicate<
        ranges::equal_to, projected<Iterator, Projection>, const T *>
  constexpr iter_difference_t<Iterator>
  operator()(Iterator first, Sentinel last, const T &value,
             Projection projection = {}) const {
    using difference_type = iter_difference_t<Iterator>;

    return FTL_COUNT_NAMESPACE::detail::ranges_count_loop<difference_type>(
        first, last, value, projection);
  }

  template <input_range Range, class T, class Projection = identity>
    requires indirect_binary_predicate<
        ranges::equal_to, projected<iterator_t<Range>, Projection>, const T *>
  constexpr range_difference_t<Range>
  operator()(Range &&range, const T &value,
             Projection projection = {}) const {
    return (*this)(ranges::begin(range), ranges::end(range), value,
                   FTL_COUNT_NAMESPACE::move(projection));
  }
};

struct count_if_fn {
  template <
      input_iterator Iterator, sentinel_for<Iterator> Sentinel,
      class Projection = identity,
      indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
  constexpr iter_difference_t<Iterator>
  operator()(Iterator first, Sentinel last, Predicate predicate,
             Projection projection = {}) const {
    using difference_type = iter_difference_t<Iterator>;

    return FTL_COUNT_NAMESPACE::detail::ranges_count_if_loop<difference_type>(
        first, last, predicate, projection);
  }

  template <
      input_range Range, class Projection = identity,
      indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
          Predicate>
  constexpr range_difference_t<Range>
  operator()(Range &&range, Predicate predicate,
             Projection projection = {}) const {
    return (*this)(ranges::begin(range), ranges::end(range),
                   FTL_COUNT_NAMESPACE::move(predicate),
                   FTL_COUNT_NAMESPACE::move(projection));
  }
};

inline constexpr count_fn count{};
inline constexpr count_if_fn count_if{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_COUNT_NAMESPACE

#endif // FTL_COUNT_HEADER