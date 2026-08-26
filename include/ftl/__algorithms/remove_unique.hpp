// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_REMOVE_UNIQUE_HEADER
#define FTL_REMOVE_UNIQUE_HEADER

#include <__algorithms/result.hpp>
#include <__execution/policy_access.hpp>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>


namespace std {

namespace detail {

struct remove_unique_equal_to {
  template <class T, class U>
  constexpr bool operator()(T &&left, U &&right) const {
    return static_cast<T &&>(left) == static_cast<U &&>(right);
  }
};

template <class Iterator, class T>
constexpr Iterator remove_loop(Iterator first, Iterator last, const T &value) {
  Iterator result = first;

  for (Iterator current = first; current != last; ++current) {
    if (!(*current == value)) {
      if (result != current) {
        *result = std::move(*current);
      }

      ++result;
    }
  }

  return result;
}

template <class Iterator, class Predicate>
constexpr Iterator remove_if_loop(Iterator first, Iterator last,
                                  Predicate &predicate) {
  Iterator result = first;

  for (Iterator current = first; current != last; ++current) {
    if (!predicate(*current)) {
      if (result != current) {
        *result = std::move(*current);
      }

      ++result;
    }
  }

  return result;
}

template <class Iterator, class Sentinel, class T, class Projection>
constexpr ranges::subrange<Iterator>
ranges_remove_loop(Iterator first, Sentinel last, const T &value,
                   Projection &projection) {
  ranges::equal_to equal{};
  Iterator result = first;
  Iterator current = first;

  for (; current != last; ++current) {
    if (!std::invoke(
            equal, std::invoke(projection, *current),
            value)) {
      if (result != current) {
        *result = ranges::iter_move(current);
      }

      ++result;
    }
  }

  return {
      std::move(result),
      std::move(current),
  };
}

template <class Iterator, class Sentinel, class Predicate, class Projection>
constexpr ranges::subrange<Iterator>
ranges_remove_if_loop(Iterator first, Sentinel last, Predicate &predicate,
                      Projection &projection) {
  Iterator result = first;
  Iterator current = first;

  for (; current != last; ++current) {
    if (!std::invoke(
            predicate,
            std::invoke(projection, *current))) {
      if (result != current) {
        *result = ranges::iter_move(current);
      }

      ++result;
    }
  }

  return {
      std::move(result),
      std::move(current),
  };
}

template <class Iterator, class Sentinel, class Output, class T>
constexpr void remove_copy_loop(Iterator &first, const Sentinel &last,
                                Output &result, const T &value) {
  for (; first != last; ++first) {
    if (!(*first == value)) {
      *result = *first;
      ++result;
    }
  }
}

template <class Iterator, class Sentinel, class Output, class Predicate>
constexpr void remove_copy_if_loop(Iterator &first, const Sentinel &last,
                                   Output &result, Predicate &predicate) {
  for (; first != last; ++first) {
    if (!predicate(*first)) {
      *result = *first;
      ++result;
    }
  }
}

template <class Iterator, class Sentinel, class Output, class T,
          class Projection>
constexpr void ranges_remove_copy_loop(Iterator &first, const Sentinel &last,
                                       Output &result, const T &value,
                                       Projection &projection) {
  ranges::equal_to equal{};

  for (; first != last; ++first) {
    if (!std::invoke(
            equal, std::invoke(projection, *first),
            value)) {
      *result = *first;
      ++result;
    }
  }
}

template <class Iterator, class Sentinel, class Output, class Predicate,
          class Projection>
constexpr void ranges_remove_copy_if_loop(Iterator &first, const Sentinel &last,
                                          Output &result, Predicate &predicate,
                                          Projection &projection) {
  for (; first != last; ++first) {
    if (!std::invoke(
            predicate,
            std::invoke(projection, *first))) {
      *result = *first;
      ++result;
    }
  }
}

template <class Iterator, class Predicate>
constexpr Iterator unique_loop(Iterator first, Iterator last,
                               Predicate &predicate) {
  if (first == last) {
    return last;
  }

  Iterator result = first;
  Iterator current = first;
  ++current;

  for (; current != last; ++current) {
    if (!predicate(*result, *current)) {
      ++result;

      if (result != current) {
        *result = std::move(*current);
      }
    }
  }

  ++result;
  return result;
}

template <class Iterator, class Sentinel, class Comparator, class Projection>
constexpr ranges::subrange<Iterator>
ranges_unique_loop(Iterator first, Sentinel last, Comparator &comparator,
                   Projection &projection) {
  if (first == last) {
    return {first, first};
  }

  Iterator result = first;
  Iterator current = first;
  ++current;

  for (; current != last; ++current) {
    if (!std::invoke(
            comparator,
            std::invoke(projection, *result),
            std::invoke(projection, *current))) {
      ++result;

      if (result != current) {
        *result = ranges::iter_move(current);
      }
    }
  }

  ++result;

  return {
      std::move(result),
      std::move(current),
  };
}

template <class Iterator, class Sentinel, class Output, class Comparator,
          class Projection>
constexpr ranges::in_out_result<Iterator, Output>
unique_copy_loop(Iterator first, Sentinel last, Output result,
                 Comparator &comparator, Projection &projection) {
  if (first == last) {
    return {
        std::move(first),
        std::move(result),
    };
  }

  if constexpr (forward_iterator<Iterator>) {
    Iterator previous = first;

    *result = *previous;
    ++result;
    ++first;

    for (; first != last; ++first) {
      if (!std::invoke(
              comparator,
              std::invoke(projection, *previous),
              std::invoke(projection, *first))) {
        previous = first;
        *result = *first;
        ++result;
      }
    }
  } else if constexpr (input_iterator<Output> &&
                       same_as<iter_value_t<Iterator>, iter_value_t<Output>>) {
    *result = *first;

    Output previous = result;
    ++result;
    ++first;

    for (; first != last; ++first) {
      if (!std::invoke(
              comparator,
              std::invoke(projection, *previous),
              std::invoke(projection, *first))) {
        *result = *first;
        previous = result;
        ++result;
      }
    }
  } else {
    iter_value_t<Iterator> previous = *first;

    *result = previous;
    ++result;
    ++first;

    for (; first != last; ++first) {
      if (!std::invoke(
              comparator,
              std::invoke(projection, previous),
              std::invoke(projection, *first))) {
        previous = *first;
        *result = previous;
        ++result;
      }
    }
  }

  return {
      std::move(first),
      std::move(result),
  };
}

} // namespace detail

template <class ForwardIterator, class T>
constexpr ForwardIterator remove(ForwardIterator first, ForwardIterator last,
                                 const T &value) {
  return detail::remove_loop(first, last, value);
}

template <class ExecutionPolicy, class ForwardIterator, class T>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator remove(ExecutionPolicy &&, ForwardIterator first,
                       ForwardIterator last, const T &value) {
  return [&]() noexcept { return detail::remove_loop(first, last, value); }();
}

template <class ForwardIterator, class Predicate>
constexpr ForwardIterator remove_if(ForwardIterator first, ForwardIterator last,
                                    Predicate predicate) {
  return detail::remove_if_loop(first, last, predicate);
}

template <class ExecutionPolicy, class ForwardIterator, class Predicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator remove_if(ExecutionPolicy &&, ForwardIterator first,
                          ForwardIterator last, Predicate predicate) {
  return [&]() noexcept {
    return detail::remove_if_loop(first, last, predicate);
  }();
}

template <class InputIterator, class OutputIterator, class T>
constexpr OutputIterator remove_copy(InputIterator first, InputIterator last,
                                     OutputIterator result, const T &value) {
  detail::remove_copy_loop(first, last, result, value);

  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class T>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator2 remove_copy(ExecutionPolicy &&, ForwardIterator1 first,
                             ForwardIterator1 last, ForwardIterator2 result,
                             const T &value) {
  return [&]() noexcept {
    detail::remove_copy_loop(first, last, result, value);

    return result;
  }();
}

template <class InputIterator, class OutputIterator, class Predicate>
constexpr OutputIterator remove_copy_if(InputIterator first, InputIterator last,
                                        OutputIterator result,
                                        Predicate predicate) {
  detail::remove_copy_if_loop(first, last, result, predicate);

  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class Predicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator2 remove_copy_if(ExecutionPolicy &&, ForwardIterator1 first,
                                ForwardIterator1 last, ForwardIterator2 result,
                                Predicate predicate) {
  return [&]() noexcept {
    detail::remove_copy_if_loop(first, last, result, predicate);

    return result;
  }();
}

template <class ForwardIterator>
constexpr ForwardIterator unique(ForwardIterator first, ForwardIterator last) {
  detail::remove_unique_equal_to predicate{};

  return detail::unique_loop(first, last, predicate);
}

template <class ExecutionPolicy, class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator unique(ExecutionPolicy &&, ForwardIterator first,
                       ForwardIterator last) {
  return [&]() noexcept {
    detail::remove_unique_equal_to predicate{};

    return detail::unique_loop(first, last, predicate);
  }();
}

template <class ForwardIterator, class BinaryPredicate>
constexpr ForwardIterator unique(ForwardIterator first, ForwardIterator last,
                                 BinaryPredicate predicate) {
  return detail::unique_loop(first, last, predicate);
}

template <class ExecutionPolicy, class ForwardIterator, class BinaryPredicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator unique(ExecutionPolicy &&, ForwardIterator first,
                       ForwardIterator last, BinaryPredicate predicate) {
  return
      [&]() noexcept { return detail::unique_loop(first, last, predicate); }();
}

template <class InputIterator, class OutputIterator>
constexpr OutputIterator unique_copy(InputIterator first, InputIterator last,
                                     OutputIterator result) {
  detail::remove_unique_equal_to comparator{};
  identity projection{};

  return detail::unique_copy_loop(first, last, result, comparator, projection)
      .out;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator2 unique_copy(ExecutionPolicy &&, ForwardIterator1 first,
                             ForwardIterator1 last, ForwardIterator2 result) {
  return [&]() noexcept {
    detail::remove_unique_equal_to comparator{};
    identity projection{};

    return detail::unique_copy_loop(first, last, result, comparator, projection)
        .out;
  }();
}

template <class InputIterator, class OutputIterator, class BinaryPredicate>
constexpr OutputIterator unique_copy(InputIterator first, InputIterator last,
                                     OutputIterator result,
                                     BinaryPredicate predicate) {
  identity projection{};

  return detail::unique_copy_loop(first, last, result, predicate, projection)
      .out;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class BinaryPredicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator2 unique_copy(ExecutionPolicy &&, ForwardIterator1 first,
                             ForwardIterator1 last, ForwardIterator2 result,
                             BinaryPredicate predicate) {
  return [&]() noexcept {
    identity projection{};

    return detail::unique_copy_loop(first, last, result, predicate, projection)
        .out;
  }();
}

namespace ranges {

template <class Iterator, class Output>
using remove_copy_result = in_out_result<Iterator, Output>;

template <class Iterator, class Output>
using remove_copy_if_result = in_out_result<Iterator, Output>;

template <class Iterator, class Output>
using unique_copy_result = in_out_result<Iterator, Output>;

struct remove_fn {
  template <permutable Iterator, sentinel_for<Iterator> Sentinel, class T,
            class Projection = identity>
    requires indirect_binary_predicate<
        ranges::equal_to, projected<Iterator, Projection>, const T *>
  constexpr subrange<Iterator> operator()(Iterator first, Sentinel last,
                                          const T &value,
                                          Projection projection = {}) const {
    return std::detail::ranges_remove_loop(
        std::move(first),
        std::move(last), value, projection);
  }

  template <forward_range Range, class T, class Projection = identity>
    requires(permutable<iterator_t<Range>> &&
             indirect_binary_predicate<ranges::equal_to,
                                       projected<iterator_t<Range>, Projection>,
                                       const T *>)
  constexpr borrowed_subrange_t<Range>
  operator()(Range &&range, const T &value, Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range), value,
                          std::move(projection));

    return std::move(result);
  }
};

struct remove_if_fn {
  template <permutable Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
  constexpr subrange<Iterator> operator()(Iterator first, Sentinel last,
                                          Predicate predicate,
                                          Projection projection = {}) const {
    return std::detail::ranges_remove_if_loop(
        std::move(first),
        std::move(last), predicate, projection);
  }

  template <forward_range Range, class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
    requires permutable<iterator_t<Range>>
  constexpr borrowed_subrange_t<Range>
  operator()(Range &&range, Predicate predicate,
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          std::move(predicate),
                          std::move(projection));

    return std::move(result);
  }
};

struct remove_copy_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            weakly_incrementable Output, class T, class Projection = identity>
    requires(indirectly_copyable<Iterator, Output> &&
             indirect_binary_predicate<
                 ranges::equal_to, projected<Iterator, Projection>, const T *>)
  constexpr remove_copy_result<Iterator, Output>
  operator()(Iterator first, Sentinel last, Output result, const T &value,
             Projection projection = {}) const {
    std::detail::ranges_remove_copy_loop(
        first, last, result, value, projection);

    return {
        std::move(first),
        std::move(result),
    };
  }

  template <input_range Range, weakly_incrementable Output, class T,
            class Projection = identity>
    requires(indirectly_copyable<iterator_t<Range>, Output> &&
             indirect_binary_predicate<ranges::equal_to,
                                       projected<iterator_t<Range>, Projection>,
                                       const T *>)
  constexpr remove_copy_result<borrowed_iterator_t<Range>, Output>
  operator()(Range &&range, Output result, const T &value,
             Projection projection = {}) const {
    auto converted = (*this)(ranges::begin(range), ranges::end(range),
                             std::move(result), value,
                             std::move(projection));

    return {
        std::move(converted.in),
        std::move(converted.out),
    };
  }
};

struct remove_copy_if_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            weakly_incrementable Output, class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
    requires indirectly_copyable<Iterator, Output>
  constexpr remove_copy_if_result<Iterator, Output>
  operator()(Iterator first, Sentinel last, Output result, Predicate predicate,
             Projection projection = {}) const {
    std::detail::ranges_remove_copy_if_loop(
        first, last, result, predicate, projection);

    return {
        std::move(first),
        std::move(result),
    };
  }

  template <input_range Range, weakly_incrementable Output,
            class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
    requires indirectly_copyable<iterator_t<Range>, Output>
  constexpr remove_copy_if_result<borrowed_iterator_t<Range>, Output>
  operator()(Range &&range, Output result, Predicate predicate,
             Projection projection = {}) const {
    auto converted = (*this)(ranges::begin(range), ranges::end(range),
                             std::move(result),
                             std::move(predicate),
                             std::move(projection));

    return {
        std::move(converted.in),
        std::move(converted.out),
    };
  }
};

struct unique_fn {
  template <permutable Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_equivalence_relation<projected<Iterator, Projection>>
                Comparator = ranges::equal_to>
  constexpr subrange<Iterator> operator()(Iterator first, Sentinel last,
                                          Comparator comparator = {},
                                          Projection projection = {}) const {
    return std::detail::ranges_unique_loop(
        std::move(first),
        std::move(last), comparator, projection);
  }

  template <
      forward_range Range, class Projection = identity,
      indirect_equivalence_relation<projected<iterator_t<Range>, Projection>>
          Comparator = ranges::equal_to>
    requires permutable<iterator_t<Range>>
  constexpr borrowed_subrange_t<Range>
  operator()(Range &&range, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          std::move(comparator),
                          std::move(projection));

    return std::move(result);
  }
};

struct unique_copy_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            weakly_incrementable Output, class Projection = identity,
            indirect_equivalence_relation<projected<Iterator, Projection>>
                Comparator = ranges::equal_to>
    requires(indirectly_copyable<Iterator, Output> &&
             (forward_iterator<Iterator> ||
              (input_iterator<Output> &&
               same_as<iter_value_t<Iterator>, iter_value_t<Output>>) ||
              indirectly_copyable_storable<Iterator, Output>))
  constexpr unique_copy_result<Iterator, Output>
  operator()(Iterator first, Sentinel last, Output result,
             Comparator comparator = {}, Projection projection = {}) const {
    return std::detail::unique_copy_loop(
        std::move(first),
        std::move(last),
        std::move(result), comparator, projection);
  }

  template <
      input_range Range, weakly_incrementable Output,
      class Projection = identity,
      indirect_equivalence_relation<projected<iterator_t<Range>, Projection>>
          Comparator = ranges::equal_to>
    requires(indirectly_copyable<iterator_t<Range>, Output> &&
             (forward_iterator<iterator_t<Range>> ||
              (input_iterator<Output> &&
               same_as<range_value_t<Range>, iter_value_t<Output>>) ||
              indirectly_copyable_storable<iterator_t<Range>, Output>))
  constexpr unique_copy_result<borrowed_iterator_t<Range>, Output>
  operator()(Range &&range, Output result, Comparator comparator = {},
             Projection projection = {}) const {
    auto converted = (*this)(ranges::begin(range), ranges::end(range),
                             std::move(result),
                             std::move(comparator),
                             std::move(projection));

    return {
        std::move(converted.in),
        std::move(converted.out),
    };
  }
};

inline constexpr remove_fn remove{};
inline constexpr remove_if_fn remove_if{};
inline constexpr remove_copy_fn remove_copy{};
inline constexpr remove_copy_if_fn remove_copy_if{};
inline constexpr unique_fn unique{};
inline constexpr unique_copy_fn unique_copy{};

} // namespace ranges

} // namespace std


#endif // FTL_REMOVE_UNIQUE_HEADER
