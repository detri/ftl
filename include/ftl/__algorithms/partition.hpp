// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_PARTITION_HEADER
#define FTL_PARTITION_HEADER

#ifdef FTL_REPLACE_STL
#include <__algorithms/result.hpp>
#include <__algorithms/reverse_rotate.hpp>
#include <__execution/policy_access.hpp>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#else
#include <ftl/__algorithms/result.hpp>
#include <ftl/__algorithms/reverse_rotate.hpp>
#include <ftl/__execution/policy_access.hpp>
#include <ftl/functional>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_PARTITION_NAMESPACE std
#else
#define FTL_PARTITION_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace detail {

template <class Iterator, class Sentinel, class Predicate, class Projection>
constexpr bool is_partitioned_loop(Iterator first, const Sentinel &last,
                                   Predicate &predicate,
                                   Projection &projection) {
  for (; first != last; ++first) {
    if (!FTL_PARTITION_NAMESPACE::invoke(
            predicate, FTL_PARTITION_NAMESPACE::invoke(projection, *first))) {
      break;
    }
  }

  for (; first != last; ++first) {
    if (FTL_PARTITION_NAMESPACE::invoke(
            predicate, FTL_PARTITION_NAMESPACE::invoke(projection, *first))) {
      return false;
    }
  }

  return true;
}

template <class Iterator, class Sentinel, class Predicate, class Projection>
constexpr ranges::subrange<Iterator>
partition_loop(Iterator first, Sentinel last, Predicate &predicate,
               Projection &projection) {
  while (first != last &&
         FTL_PARTITION_NAMESPACE::invoke(
             predicate, FTL_PARTITION_NAMESPACE::invoke(projection, *first))) {
    ++first;
  }

  if (first == last) {
    return {
        FTL_PARTITION_NAMESPACE::move(first),
        FTL_PARTITION_NAMESPACE::move(first),
    };
  }

  Iterator partition_point = first;
  Iterator current = first;
  ++current;

  for (; current != last; ++current) {
    if (FTL_PARTITION_NAMESPACE::invoke(
            predicate, FTL_PARTITION_NAMESPACE::invoke(projection, *current))) {
      ranges::iter_swap(partition_point, current);
      ++partition_point;
    }
  }

  return {
      FTL_PARTITION_NAMESPACE::move(partition_point),
      FTL_PARTITION_NAMESPACE::move(current),
  };
}

template <class Iterator, class Predicate, class Projection>
constexpr Iterator
stable_partition_recursive(Iterator first, Iterator last,
                           iter_difference_t<Iterator> length,
                           Predicate &predicate, Projection &projection) {
  if (length == 0) {
    return first;
  }

  if (length == 1) {
    if (FTL_PARTITION_NAMESPACE::invoke(
            predicate, FTL_PARTITION_NAMESPACE::invoke(projection, *first))) {
      ++first;
    }

    return first;
  }

  const auto left_length = static_cast<iter_difference_t<Iterator>>(length / 2);

  Iterator middle = first;
  ranges::advance(middle, left_length);

  Iterator left_partition = stable_partition_recursive(
      first, middle, left_length, predicate, projection);

  Iterator right_partition = stable_partition_recursive(
      middle, last,
      static_cast<iter_difference_t<Iterator>>(length - left_length), predicate,
      projection);

  return detail::rotate_loop(FTL_PARTITION_NAMESPACE::move(left_partition),
                             FTL_PARTITION_NAMESPACE::move(middle),
                             FTL_PARTITION_NAMESPACE::move(right_partition));
}

template <class Iterator, class Sentinel, class Predicate, class Projection>
constexpr ranges::subrange<Iterator>
stable_partition_loop(Iterator first, Sentinel last, Predicate &predicate,
                      Projection &projection) {
  Iterator end = first;
  ranges::advance(end, last);

  const auto length = ranges::distance(first, end);

  Iterator partition_point =
      stable_partition_recursive(first, end, length, predicate, projection);

  return {
      FTL_PARTITION_NAMESPACE::move(partition_point),
      FTL_PARTITION_NAMESPACE::move(end),
  };
}

template <class Iterator, class Sentinel, class Output1, class Output2,
          class Predicate, class Projection>
constexpr void partition_copy_loop(Iterator &first, const Sentinel &last,
                                   Output1 &out_true, Output2 &out_false,
                                   Predicate &predicate,
                                   Projection &projection) {
  for (; first != last; ++first) {
    if (FTL_PARTITION_NAMESPACE::invoke(
            predicate, FTL_PARTITION_NAMESPACE::invoke(projection, *first))) {
      *out_true = *first;
      ++out_true;
    } else {
      *out_false = *first;
      ++out_false;
    }
  }
}

template <class Iterator, class Sentinel, class Predicate, class Projection>
constexpr Iterator partition_point_loop(Iterator first, Sentinel last,
                                        Predicate &predicate,
                                        Projection &projection) {
  auto length = ranges::distance(first, last);

  while (length != 0) {
    const auto half = static_cast<iter_difference_t<Iterator>>(length / 2);

    Iterator middle = first;
    ranges::advance(middle, half);

    if (FTL_PARTITION_NAMESPACE::invoke(
            predicate, FTL_PARTITION_NAMESPACE::invoke(projection, *middle))) {
      first = middle;
      ++first;

      length = static_cast<iter_difference_t<Iterator>>(length - half - 1);
    } else {
      length = half;
    }
  }

  return first;
}

} // namespace detail

template <class InputIterator, class Predicate>
constexpr bool is_partitioned(InputIterator first, InputIterator last,
                              Predicate predicate) {
  identity projection{};

  return detail::is_partitioned_loop(FTL_PARTITION_NAMESPACE::move(first),
                                     FTL_PARTITION_NAMESPACE::move(last),
                                     predicate, projection);
}

template <class ExecutionPolicy, class ForwardIterator, class Predicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool is_partitioned(ExecutionPolicy &&, ForwardIterator first,
                    ForwardIterator last, Predicate predicate) {
  return [&]() noexcept {
    identity projection{};

    return detail::is_partitioned_loop(FTL_PARTITION_NAMESPACE::move(first),
                                       FTL_PARTITION_NAMESPACE::move(last),
                                       predicate, projection);
  }();
}

template <class ForwardIterator, class Predicate>
constexpr ForwardIterator partition(ForwardIterator first, ForwardIterator last,
                                    Predicate predicate) {
  identity projection{};

  return detail::partition_loop(FTL_PARTITION_NAMESPACE::move(first),
                                FTL_PARTITION_NAMESPACE::move(last), predicate,
                                projection)
      .begin();
}

template <class ExecutionPolicy, class ForwardIterator, class Predicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator partition(ExecutionPolicy &&, ForwardIterator first,
                          ForwardIterator last, Predicate predicate) {
  return [&]() noexcept {
    identity projection{};

    return detail::partition_loop(FTL_PARTITION_NAMESPACE::move(first),
                                  FTL_PARTITION_NAMESPACE::move(last),
                                  predicate, projection)
        .begin();
  }();
}

template <class BidirectionalIterator, class Predicate>
BidirectionalIterator stable_partition(BidirectionalIterator first,
                                       BidirectionalIterator last,
                                       Predicate predicate) {
  identity projection{};

  return detail::stable_partition_loop(FTL_PARTITION_NAMESPACE::move(first),
                                       FTL_PARTITION_NAMESPACE::move(last),
                                       predicate, projection)
      .begin();
}

template <class ExecutionPolicy, class BidirectionalIterator, class Predicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
BidirectionalIterator
stable_partition(ExecutionPolicy &&, BidirectionalIterator first,
                 BidirectionalIterator last, Predicate predicate) {
  return [&]() noexcept {
    identity projection{};

    return detail::stable_partition_loop(FTL_PARTITION_NAMESPACE::move(first),
                                         FTL_PARTITION_NAMESPACE::move(last),
                                         predicate, projection)
        .begin();
  }();
}

template <class InputIterator, class OutputIterator1, class OutputIterator2,
          class Predicate>
constexpr pair<OutputIterator1, OutputIterator2>
partition_copy(InputIterator first, InputIterator last,
               OutputIterator1 out_true, OutputIterator2 out_false,
               Predicate predicate) {
  identity projection{};

  detail::partition_copy_loop(first, last, out_true, out_false, predicate,
                              projection);

  return {
      FTL_PARTITION_NAMESPACE::move(out_true),
      FTL_PARTITION_NAMESPACE::move(out_false),
  };
}

template <class ExecutionPolicy, class ForwardIterator, class ForwardIterator1,
          class ForwardIterator2, class Predicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
pair<ForwardIterator1, ForwardIterator2>
partition_copy(ExecutionPolicy &&, ForwardIterator first, ForwardIterator last,
               ForwardIterator1 out_true, ForwardIterator2 out_false,
               Predicate predicate) {
  return [&]() noexcept {
    identity projection{};

    detail::partition_copy_loop(first, last, out_true, out_false, predicate,
                                projection);

    return pair<ForwardIterator1, ForwardIterator2>{
        FTL_PARTITION_NAMESPACE::move(out_true),
        FTL_PARTITION_NAMESPACE::move(out_false),
    };
  }();
}

template <class ForwardIterator, class Predicate>
constexpr ForwardIterator partition_point(ForwardIterator first,
                                          ForwardIterator last,
                                          Predicate predicate) {
  identity projection{};

  return detail::partition_point_loop(FTL_PARTITION_NAMESPACE::move(first),
                                      FTL_PARTITION_NAMESPACE::move(last),
                                      predicate, projection);
}

namespace ranges {

template <class Iterator, class Output1, class Output2>
using partition_copy_result = in_out_out_result<Iterator, Output1, Output2>;

struct is_partitioned_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
  constexpr bool operator()(Iterator first, Sentinel last, Predicate predicate,
                            Projection projection = {}) const {
    return FTL_PARTITION_NAMESPACE::detail::is_partitioned_loop(
        FTL_PARTITION_NAMESPACE::move(first),
        FTL_PARTITION_NAMESPACE::move(last), predicate, projection);
  }

  template <input_range Range, class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
  constexpr bool operator()(Range &&range, Predicate predicate,
                            Projection projection = {}) const {
    return (*this)(ranges::begin(range), ranges::end(range),
                   FTL_PARTITION_NAMESPACE::move(predicate),
                   FTL_PARTITION_NAMESPACE::move(projection));
  }
};

struct partition_fn {
  template <permutable Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
  constexpr subrange<Iterator> operator()(Iterator first, Sentinel last,
                                          Predicate predicate,
                                          Projection projection = {}) const {
    return FTL_PARTITION_NAMESPACE::detail::partition_loop(
        FTL_PARTITION_NAMESPACE::move(first),
        FTL_PARTITION_NAMESPACE::move(last), predicate, projection);
  }

  template <forward_range Range, class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
    requires permutable<iterator_t<Range>>
  constexpr borrowed_subrange_t<Range>
  operator()(Range &&range, Predicate predicate,
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_PARTITION_NAMESPACE::move(predicate),
                          FTL_PARTITION_NAMESPACE::move(projection));

    return FTL_PARTITION_NAMESPACE::move(result);
  }
};

struct stable_partition_fn {
  template <bidirectional_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
    requires permutable<Iterator>
  subrange<Iterator> operator()(Iterator first, Sentinel last,
                                Predicate predicate,
                                Projection projection = {}) const {
    return FTL_PARTITION_NAMESPACE::detail::stable_partition_loop(
        FTL_PARTITION_NAMESPACE::move(first),
        FTL_PARTITION_NAMESPACE::move(last), predicate, projection);
  }

  template <bidirectional_range Range, class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
    requires permutable<iterator_t<Range>>
  borrowed_subrange_t<Range> operator()(Range &&range, Predicate predicate,
                                        Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_PARTITION_NAMESPACE::move(predicate),
                          FTL_PARTITION_NAMESPACE::move(projection));

    return FTL_PARTITION_NAMESPACE::move(result);
  }
};

struct partition_copy_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            weakly_incrementable Output1, weakly_incrementable Output2,
            class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
    requires(indirectly_copyable<Iterator, Output1> &&
             indirectly_copyable<Iterator, Output2>)
  constexpr partition_copy_result<Iterator, Output1, Output2>
  operator()(Iterator first, Sentinel last, Output1 out_true, Output2 out_false,
             Predicate predicate, Projection projection = {}) const {
    FTL_PARTITION_NAMESPACE::detail::partition_copy_loop(
        first, last, out_true, out_false, predicate, projection);

    return {
        FTL_PARTITION_NAMESPACE::move(first),
        FTL_PARTITION_NAMESPACE::move(out_true),
        FTL_PARTITION_NAMESPACE::move(out_false),
    };
  }

  template <input_range Range, weakly_incrementable Output1,
            weakly_incrementable Output2, class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
    requires(indirectly_copyable<iterator_t<Range>, Output1> &&
             indirectly_copyable<iterator_t<Range>, Output2>)
  constexpr partition_copy_result<borrowed_iterator_t<Range>, Output1, Output2>
  operator()(Range &&range, Output1 out_true, Output2 out_false,
             Predicate predicate, Projection projection = {}) const {
    auto converted = (*this)(ranges::begin(range), ranges::end(range),
                             FTL_PARTITION_NAMESPACE::move(out_true),
                             FTL_PARTITION_NAMESPACE::move(out_false),
                             FTL_PARTITION_NAMESPACE::move(predicate),
                             FTL_PARTITION_NAMESPACE::move(projection));

    return {
        FTL_PARTITION_NAMESPACE::move(converted.in),
        FTL_PARTITION_NAMESPACE::move(converted.out1),
        FTL_PARTITION_NAMESPACE::move(converted.out2),
    };
  }
};

struct partition_point_fn {
  template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                Predicate predicate,
                                Projection projection = {}) const {
    return FTL_PARTITION_NAMESPACE::detail::partition_point_loop(
        FTL_PARTITION_NAMESPACE::move(first),
        FTL_PARTITION_NAMESPACE::move(last), predicate, projection);
  }

  template <forward_range Range, class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, Predicate predicate,
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_PARTITION_NAMESPACE::move(predicate),
                          FTL_PARTITION_NAMESPACE::move(projection));

    return FTL_PARTITION_NAMESPACE::move(result);
  }
};

inline constexpr is_partitioned_fn is_partitioned{};
inline constexpr partition_fn partition{};
inline constexpr stable_partition_fn stable_partition{};
inline constexpr partition_copy_fn partition_copy{};
inline constexpr partition_point_fn partition_point{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_PARTITION_NAMESPACE

#endif // FTL_PARTITION_HEADER
