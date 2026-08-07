// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_MIN_MAX_HEADER
#define FTL_MIN_MAX_HEADER

#ifdef FTL_REPLACE_STL
#include <__algorithms/result.hpp>
#include <__execution/policy_access.hpp>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#else
#include <ftl/__algorithms/result.hpp>
#include <ftl/__execution/policy_access.hpp>
#include <ftl/functional>
#include <ftl/initializer_list>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_MIN_MAX_NAMESPACE std
#else
#define FTL_MIN_MAX_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace detail {

struct min_max_less {
  template <class T, class U>
  constexpr bool operator()(T &&left, U &&right) const {
    return static_cast<T &&>(left) < static_cast<U &&>(right);
  }
};

template <class Comparator, class Projection, class T, class U>
constexpr bool min_max_before(Comparator &comparator, Projection &projection,
                              T &&left, U &&right) {
  return FTL_MIN_MAX_NAMESPACE::invoke(
      comparator,
      FTL_MIN_MAX_NAMESPACE::invoke(projection, static_cast<T &&>(left)),
      FTL_MIN_MAX_NAMESPACE::invoke(projection, static_cast<U &&>(right)));
}

template <class Iterator, class Sentinel, class Comparator, class Projection>
constexpr Iterator min_element_loop(Iterator first, Sentinel last,
                                    Comparator &comparator,
                                    Projection &projection) {
  if (first == last) {
    return first;
  }

  Iterator result = first;

  while (++first != last) {
    if (min_max_before(comparator, projection, *first, *result)) {
      result = first;
    }
  }

  return result;
}

template <class Iterator, class Sentinel, class Comparator, class Projection>
constexpr Iterator max_element_loop(Iterator first, Sentinel last,
                                    Comparator &comparator,
                                    Projection &projection) {
  if (first == last) {
    return first;
  }

  Iterator result = first;

  while (++first != last) {
    if (min_max_before(comparator, projection, *result, *first)) {
      result = first;
    }
  }

  return result;
}

/*
 * Pairwise implementation:
 *
 * - first minimum
 * - last maximum
 * - at most floor(3 * (N - 1) / 2) comparisons
 */
template <class Iterator, class Sentinel, class Comparator, class Projection>
constexpr pair<Iterator, Iterator>
minmax_element_loop(Iterator first, Sentinel last, Comparator &comparator,
                    Projection &projection) {
  if (first == last) {
    return {first, first};
  }

  Iterator minimum = first;
  Iterator maximum = first;

  Iterator current = first;
  ++current;

  if (current == last) {
    return {minimum, maximum};
  }

  if (min_max_before(comparator, projection, *current, *first)) {
    minimum = current;
    maximum = first;
  } else {
    minimum = first;
    maximum = current;
  }

  ++current;

  while (current != last) {
    Iterator first_in_pair = current;
    ++current;

    if (current == last) {
      if (min_max_before(comparator, projection, *first_in_pair, *minimum)) {
        minimum = first_in_pair;
      } else if (!min_max_before(comparator, projection, *first_in_pair,
                                 *maximum)) {
        maximum = first_in_pair;
      }

      break;
    }

    Iterator second_in_pair = current;
    ++current;

    Iterator smaller;
    Iterator larger;

    if (min_max_before(comparator, projection, *second_in_pair,
                       *first_in_pair)) {
      smaller = second_in_pair;
      larger = first_in_pair;
    } else {
      smaller = first_in_pair;
      larger = second_in_pair;
    }

    if (min_max_before(comparator, projection, *smaller, *minimum)) {
      minimum = smaller;
    }

    /*
     * Deliberately update on equivalence so maximum is the
     * last iterator referring to a maximal element.
     */
    if (!min_max_before(comparator, projection, *larger, *maximum)) {
      maximum = larger;
    }
  }

  return {
      FTL_MIN_MAX_NAMESPACE::move(minimum),
      FTL_MIN_MAX_NAMESPACE::move(maximum),
  };
}

template <class Iterator, class Sentinel, class Comparator, class Projection>
constexpr iter_value_t<Iterator> min_value_loop(Iterator first, Sentinel last,
                                                Comparator &comparator,
                                                Projection &projection) {
  iter_value_t<Iterator> result = *first;
  ++first;

  for (; first != last; ++first) {
    if (min_max_before(comparator, projection, *first, result)) {
      result = *first;
    }
  }

  return result;
}

template <class Iterator, class Sentinel, class Comparator, class Projection>
constexpr iter_value_t<Iterator> max_value_loop(Iterator first, Sentinel last,
                                                Comparator &comparator,
                                                Projection &projection) {
  iter_value_t<Iterator> result = *first;
  ++first;

  for (; first != last; ++first) {
    if (min_max_before(comparator, projection, result, *first)) {
      result = *first;
    }
  }

  return result;
}

template <class Iterator, class Sentinel, class Comparator, class Projection>
constexpr ranges::min_max_result<iter_value_t<Iterator>>
minmax_value_loop(Iterator first, Sentinel last, Comparator &comparator,
                  Projection &projection) {
  using value_type = iter_value_t<Iterator>;

  value_type first_value = *first;
  ++first;

  if (first == last) {
    return {
        first_value,
        first_value,
    };
  }

  value_type second_value = *first;
  ++first;

  value_type minimum = first_value;
  value_type maximum = second_value;

  if (min_max_before(comparator, projection, second_value, first_value)) {
    minimum = second_value;
    maximum = first_value;
  }

  while (first != last) {
    value_type first_in_pair = *first;
    ++first;

    if (first == last) {
      if (min_max_before(comparator, projection, first_in_pair, minimum)) {
        minimum = FTL_MIN_MAX_NAMESPACE::move(first_in_pair);
      } else if (!min_max_before(comparator, projection, first_in_pair,
                                 maximum)) {
        maximum = FTL_MIN_MAX_NAMESPACE::move(first_in_pair);
      }

      break;
    }

    value_type second_in_pair = *first;
    ++first;

    if (min_max_before(comparator, projection, second_in_pair, first_in_pair)) {
      if (min_max_before(comparator, projection, second_in_pair, minimum)) {
        minimum = FTL_MIN_MAX_NAMESPACE::move(second_in_pair);
      }

      if (!min_max_before(comparator, projection, first_in_pair, maximum)) {
        maximum = FTL_MIN_MAX_NAMESPACE::move(first_in_pair);
      }
    } else {
      if (min_max_before(comparator, projection, first_in_pair, minimum)) {
        minimum = FTL_MIN_MAX_NAMESPACE::move(first_in_pair);
      }

      if (!min_max_before(comparator, projection, second_in_pair, maximum)) {
        maximum = FTL_MIN_MAX_NAMESPACE::move(second_in_pair);
      }
    }
  }

  return {
      FTL_MIN_MAX_NAMESPACE::move(minimum),
      FTL_MIN_MAX_NAMESPACE::move(maximum),
  };
}

} // namespace detail

template <class T> constexpr const T &min(const T &left, const T &right) {
  return right < left ? right : left;
}

template <class T, class Comparator>
constexpr const T &min(const T &left, const T &right, Comparator comparator) {
  return comparator(right, left) ? right : left;
}

template <class T> constexpr T min(initializer_list<T> values) {
  detail::min_max_less comparator{};
  identity projection{};

  return detail::min_value_loop(values.begin(), values.end(), comparator,
                                projection);
}

template <class T, class Comparator>
constexpr T min(initializer_list<T> values, Comparator comparator) {
  identity projection{};

  return detail::min_value_loop(values.begin(), values.end(), comparator,
                                projection);
}

template <class T> constexpr const T &max(const T &left, const T &right) {
  return left < right ? right : left;
}

template <class T, class Comparator>
constexpr const T &max(const T &left, const T &right, Comparator comparator) {
  return comparator(left, right) ? right : left;
}

template <class T> constexpr T max(initializer_list<T> values) {
  detail::min_max_less comparator{};
  identity projection{};

  return detail::max_value_loop(values.begin(), values.end(), comparator,
                                projection);
}

template <class T, class Comparator>
constexpr T max(initializer_list<T> values, Comparator comparator) {
  identity projection{};

  return detail::max_value_loop(values.begin(), values.end(), comparator,
                                projection);
}

template <class T>
constexpr pair<const T &, const T &> minmax(const T &left, const T &right) {
  return right < left ? pair<const T &, const T &>{right, left}
                      : pair<const T &, const T &>{left, right};
}

template <class T, class Comparator>
constexpr pair<const T &, const T &> minmax(const T &left, const T &right,
                                            Comparator comparator) {
  return comparator(right, left) ? pair<const T &, const T &>{right, left}
                                 : pair<const T &, const T &>{left, right};
}

template <class T> constexpr pair<T, T> minmax(initializer_list<T> values) {
  detail::min_max_less comparator{};
  identity projection{};

  auto result = detail::minmax_value_loop(values.begin(), values.end(),
                                          comparator, projection);

  return {
      FTL_MIN_MAX_NAMESPACE::move(result.min),
      FTL_MIN_MAX_NAMESPACE::move(result.max),
  };
}

template <class T, class Comparator>
constexpr pair<T, T> minmax(initializer_list<T> values, Comparator comparator) {
  identity projection{};

  auto result = detail::minmax_value_loop(values.begin(), values.end(),
                                          comparator, projection);

  return {
      FTL_MIN_MAX_NAMESPACE::move(result.min),
      FTL_MIN_MAX_NAMESPACE::move(result.max),
  };
}

template <class ForwardIterator>
constexpr ForwardIterator min_element(ForwardIterator first,
                                      ForwardIterator last) {
  detail::min_max_less comparator{};
  identity projection{};

  return detail::min_element_loop(FTL_MIN_MAX_NAMESPACE::move(first),
                                  FTL_MIN_MAX_NAMESPACE::move(last), comparator,
                                  projection);
}

template <class ForwardIterator, class Comparator>
constexpr ForwardIterator min_element(ForwardIterator first,
                                      ForwardIterator last,
                                      Comparator comparator) {
  identity projection{};

  return detail::min_element_loop(FTL_MIN_MAX_NAMESPACE::move(first),
                                  FTL_MIN_MAX_NAMESPACE::move(last), comparator,
                                  projection);
}

template <class ExecutionPolicy, class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator min_element(ExecutionPolicy &&, ForwardIterator first,
                            ForwardIterator last) {
  return [&]() noexcept {
    detail::min_max_less comparator{};
    identity projection{};

    return detail::min_element_loop(FTL_MIN_MAX_NAMESPACE::move(first),
                                    FTL_MIN_MAX_NAMESPACE::move(last),
                                    comparator, projection);
  }();
}

template <class ExecutionPolicy, class ForwardIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator min_element(ExecutionPolicy &&, ForwardIterator first,
                            ForwardIterator last, Comparator comparator) {
  return [&]() noexcept {
    identity projection{};

    return detail::min_element_loop(FTL_MIN_MAX_NAMESPACE::move(first),
                                    FTL_MIN_MAX_NAMESPACE::move(last),
                                    comparator, projection);
  }();
}

template <class ForwardIterator>
constexpr ForwardIterator max_element(ForwardIterator first,
                                      ForwardIterator last) {
  detail::min_max_less comparator{};
  identity projection{};

  return detail::max_element_loop(FTL_MIN_MAX_NAMESPACE::move(first),
                                  FTL_MIN_MAX_NAMESPACE::move(last), comparator,
                                  projection);
}

template <class ForwardIterator, class Comparator>
constexpr ForwardIterator max_element(ForwardIterator first,
                                      ForwardIterator last,
                                      Comparator comparator) {
  identity projection{};

  return detail::max_element_loop(FTL_MIN_MAX_NAMESPACE::move(first),
                                  FTL_MIN_MAX_NAMESPACE::move(last), comparator,
                                  projection);
}

template <class ExecutionPolicy, class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator max_element(ExecutionPolicy &&, ForwardIterator first,
                            ForwardIterator last) {
  return [&]() noexcept {
    detail::min_max_less comparator{};
    identity projection{};

    return detail::max_element_loop(FTL_MIN_MAX_NAMESPACE::move(first),
                                    FTL_MIN_MAX_NAMESPACE::move(last),
                                    comparator, projection);
  }();
}

template <class ExecutionPolicy, class ForwardIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator max_element(ExecutionPolicy &&, ForwardIterator first,
                            ForwardIterator last, Comparator comparator) {
  return [&]() noexcept {
    identity projection{};

    return detail::max_element_loop(FTL_MIN_MAX_NAMESPACE::move(first),
                                    FTL_MIN_MAX_NAMESPACE::move(last),
                                    comparator, projection);
  }();
}

template <class ForwardIterator>
constexpr pair<ForwardIterator, ForwardIterator>
minmax_element(ForwardIterator first, ForwardIterator last) {
  detail::min_max_less comparator{};
  identity projection{};

  return detail::minmax_element_loop(FTL_MIN_MAX_NAMESPACE::move(first),
                                     FTL_MIN_MAX_NAMESPACE::move(last),
                                     comparator, projection);
}

template <class ForwardIterator, class Comparator>
constexpr pair<ForwardIterator, ForwardIterator>
minmax_element(ForwardIterator first, ForwardIterator last,
               Comparator comparator) {
  identity projection{};

  return detail::minmax_element_loop(FTL_MIN_MAX_NAMESPACE::move(first),
                                     FTL_MIN_MAX_NAMESPACE::move(last),
                                     comparator, projection);
}

template <class ExecutionPolicy, class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
pair<ForwardIterator, ForwardIterator> minmax_element(ExecutionPolicy &&,
                                                      ForwardIterator first,
                                                      ForwardIterator last) {
  return [&]() noexcept {
    detail::min_max_less comparator{};
    identity projection{};

    return detail::minmax_element_loop(FTL_MIN_MAX_NAMESPACE::move(first),
                                       FTL_MIN_MAX_NAMESPACE::move(last),
                                       comparator, projection);
  }();
}

template <class ExecutionPolicy, class ForwardIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
pair<ForwardIterator, ForwardIterator>
minmax_element(ExecutionPolicy &&, ForwardIterator first, ForwardIterator last,
               Comparator comparator) {
  return [&]() noexcept {
    identity projection{};

    return detail::minmax_element_loop(FTL_MIN_MAX_NAMESPACE::move(first),
                                       FTL_MIN_MAX_NAMESPACE::move(last),
                                       comparator, projection);
  }();
}

template <class T>
constexpr const T &clamp(const T &value, const T &low, const T &high) {
  return value < low ? low : high < value ? high : value;
}

template <class T, class Comparator>
constexpr const T &clamp(const T &value, const T &low, const T &high,
                         Comparator comparator) {
  return comparator(value, low) ? low : comparator(high, value) ? high : value;
}

namespace ranges {

struct min_fn {
  template <class T, class Projection = identity,
            indirect_strict_weak_order<projected<const T *, Projection>>
                Comparator = ranges::less>
  constexpr const T &operator()(const T &left, const T &right,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    return FTL_MIN_MAX_NAMESPACE::detail::min_max_before(comparator, projection,
                                                         right, left)
               ? right
               : left;
  }

  template <copyable T, class Projection = identity,
            indirect_strict_weak_order<projected<const T *, Projection>>
                Comparator = ranges::less>
  constexpr T operator()(initializer_list<T> values, Comparator comparator = {},
                         Projection projection = {}) const {
    return FTL_MIN_MAX_NAMESPACE::detail::min_value_loop(
        values.begin(), values.end(), comparator, projection);
  }

  template <input_range Range, class Projection = identity,
            indirect_strict_weak_order<projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
    requires indirectly_copyable_storable<iterator_t<Range>,
                                          range_value_t<Range> *>
  constexpr range_value_t<Range> operator()(Range &&range,
                                            Comparator comparator = {},
                                            Projection projection = {}) const {
    return FTL_MIN_MAX_NAMESPACE::detail::min_value_loop(
        ranges::begin(range), ranges::end(range), comparator, projection);
  }
};

struct max_fn {
  template <class T, class Projection = identity,
            indirect_strict_weak_order<projected<const T *, Projection>>
                Comparator = ranges::less>
  constexpr const T &operator()(const T &left, const T &right,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    return FTL_MIN_MAX_NAMESPACE::detail::min_max_before(comparator, projection,
                                                         left, right)
               ? right
               : left;
  }

  template <copyable T, class Projection = identity,
            indirect_strict_weak_order<projected<const T *, Projection>>
                Comparator = ranges::less>
  constexpr T operator()(initializer_list<T> values, Comparator comparator = {},
                         Projection projection = {}) const {
    return FTL_MIN_MAX_NAMESPACE::detail::max_value_loop(
        values.begin(), values.end(), comparator, projection);
  }

  template <input_range Range, class Projection = identity,
            indirect_strict_weak_order<projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
    requires indirectly_copyable_storable<iterator_t<Range>,
                                          range_value_t<Range> *>
  constexpr range_value_t<Range> operator()(Range &&range,
                                            Comparator comparator = {},
                                            Projection projection = {}) const {
    return FTL_MIN_MAX_NAMESPACE::detail::max_value_loop(
        ranges::begin(range), ranges::end(range), comparator, projection);
  }
};

struct minmax_fn {
  template <class T, class Projection = identity,
            indirect_strict_weak_order<projected<const T *, Projection>>
                Comparator = ranges::less>
  constexpr min_max_result<const T &>
  operator()(const T &left, const T &right, Comparator comparator = {},
             Projection projection = {}) const {
    if (FTL_MIN_MAX_NAMESPACE::detail::min_max_before(comparator, projection,
                                                      right, left)) {
      return {right, left};
    }

    return {left, right};
  }

  template <copyable T, class Projection = identity,
            indirect_strict_weak_order<projected<const T *, Projection>>
                Comparator = ranges::less>
  constexpr min_max_result<T> operator()(initializer_list<T> values,
                                         Comparator comparator = {},
                                         Projection projection = {}) const {
    return FTL_MIN_MAX_NAMESPACE::detail::minmax_value_loop(
        values.begin(), values.end(), comparator, projection);
  }

  template <input_range Range, class Projection = identity,
            indirect_strict_weak_order<projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
    requires indirectly_copyable_storable<iterator_t<Range>,
                                          range_value_t<Range> *>
  constexpr min_max_result<range_value_t<Range>>
  operator()(Range &&range, Comparator comparator = {},
             Projection projection = {}) const {
    return FTL_MIN_MAX_NAMESPACE::detail::minmax_value_loop(
        ranges::begin(range), ranges::end(range), comparator, projection);
  }
};

struct min_element_fn {
  template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_strict_weak_order<projected<Iterator, Projection>>
                Comparator = ranges::less>
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    return FTL_MIN_MAX_NAMESPACE::detail::min_element_loop(
        FTL_MIN_MAX_NAMESPACE::move(first), FTL_MIN_MAX_NAMESPACE::move(last),
        comparator, projection);
  }

  template <forward_range Range, class Projection = identity,
            indirect_strict_weak_order<projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_MIN_MAX_NAMESPACE::move(comparator),
                          FTL_MIN_MAX_NAMESPACE::move(projection));

    return FTL_MIN_MAX_NAMESPACE::move(result);
  }
};

struct max_element_fn {
  template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_strict_weak_order<projected<Iterator, Projection>>
                Comparator = ranges::less>
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    return FTL_MIN_MAX_NAMESPACE::detail::max_element_loop(
        FTL_MIN_MAX_NAMESPACE::move(first), FTL_MIN_MAX_NAMESPACE::move(last),
        comparator, projection);
  }

  template <forward_range Range, class Projection = identity,
            indirect_strict_weak_order<projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_MIN_MAX_NAMESPACE::move(comparator),
                          FTL_MIN_MAX_NAMESPACE::move(projection));

    return FTL_MIN_MAX_NAMESPACE::move(result);
  }
};

template <class Iterator>
using minmax_element_result = min_max_result<Iterator>;

struct minmax_element_fn {
  template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_strict_weak_order<projected<Iterator, Projection>>
                Comparator = ranges::less>
  constexpr minmax_element_result<Iterator>
  operator()(Iterator first, Sentinel last, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = FTL_MIN_MAX_NAMESPACE::detail::minmax_element_loop(
        FTL_MIN_MAX_NAMESPACE::move(first), FTL_MIN_MAX_NAMESPACE::move(last),
        comparator, projection);

    return {
        FTL_MIN_MAX_NAMESPACE::move(result.first),
        FTL_MIN_MAX_NAMESPACE::move(result.second),
    };
  }

  template <forward_range Range, class Projection = identity,
            indirect_strict_weak_order<projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr minmax_element_result<borrowed_iterator_t<Range>>
  operator()(Range &&range, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_MIN_MAX_NAMESPACE::move(comparator),
                          FTL_MIN_MAX_NAMESPACE::move(projection));

    return {
        FTL_MIN_MAX_NAMESPACE::move(result.min),
        FTL_MIN_MAX_NAMESPACE::move(result.max),
    };
  }
};

struct clamp_fn {
  template <class T, class Projection = identity,
            indirect_strict_weak_order<projected<const T *, Projection>>
                Comparator = ranges::less>
  constexpr const T &operator()(const T &value, const T &low, const T &high,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    if (FTL_MIN_MAX_NAMESPACE::detail::min_max_before(comparator, projection,
                                                      value, low)) {
      return low;
    }

    if (FTL_MIN_MAX_NAMESPACE::detail::min_max_before(comparator, projection,
                                                      high, value)) {
      return high;
    }

    return value;
  }
};

inline constexpr min_fn min{};
inline constexpr max_fn max{};
inline constexpr minmax_fn minmax{};
inline constexpr min_element_fn min_element{};
inline constexpr max_element_fn max_element{};
inline constexpr minmax_element_fn minmax_element{};
inline constexpr clamp_fn clamp{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_MIN_MAX_NAMESPACE

#endif // FTL_MIN_MAX_HEADER
