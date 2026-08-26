// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_COPY_MOVE_HEADER
#define FTL_COPY_MOVE_HEADER

#include <__algorithms/result.hpp>
#include <__execution/policy_access.hpp>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>


namespace std {

namespace detail {

template <class Iterator, class Sentinel, class Output>
constexpr void copy_loop(Iterator &first, const Sentinel &last,
                         Output &result) {
  for (; first != last; ++first, ++result) {
    *result = *first;
  }
}

template <class Iterator, class Count, class Output>
constexpr void copy_n_loop(Iterator &first, Count count, Output &result) {
  auto remaining = +count;

  for (; remaining > 0; --remaining, ++first, ++result) {
    *result = *first;
  }
}

template <class Iterator, class Sentinel, class Output, class Predicate>
constexpr void copy_if_loop(Iterator &first, const Sentinel &last,
                            Output &result, Predicate &predicate) {
  for (; first != last; ++first) {
    if (predicate(*first)) {
      *result = *first;
      ++result;
    }
  }
}

template <class Iterator, class Sentinel, class Output, class Predicate,
          class Projection>
constexpr void ranges_copy_if_loop(Iterator &first, const Sentinel &last,
                                   Output &result, Predicate &predicate,
                                   Projection &projection) {
  for (; first != last; ++first) {
    if (std::invoke(
            predicate, std::invoke(projection, *first))) {
      *result = *first;
      ++result;
    }
  }
}

template <class Iterator, class Sentinel, class Output>
constexpr Iterator ranges_copy_backward_loop(Iterator first, Sentinel last,
                                             Output &result) {
  Iterator input_end = first;
  ranges::advance(input_end, last);

  Iterator current = input_end;

  while (first != current) {
    --current;
    --result;
    *result = *current;
  }

  return input_end;
}

template <class Iterator, class Sentinel, class Output>
constexpr void move_loop(Iterator &first, const Sentinel &last,
                         Output &result) {
  for (; first != last; ++first, ++result) {
    *result = std::move(*first);
  }
}

template <class Iterator, class Sentinel, class Output>
constexpr void ranges_move_loop(Iterator &first, const Sentinel &last,
                                Output &result) {
  for (; first != last; ++first, ++result) {
    *result = ranges::iter_move(first);
  }
}

template <class Iterator, class Sentinel, class Output>
constexpr Iterator ranges_move_backward_loop(Iterator first, Sentinel last,
                                             Output &result) {
  Iterator input_end = first;
  ranges::advance(input_end, last);

  Iterator current = input_end;

  while (first != current) {
    --current;
    --result;
    *result = ranges::iter_move(current);
  }

  return input_end;
}

} // namespace detail

template <class InputIterator, class OutputIterator>
constexpr OutputIterator copy(InputIterator first, InputIterator last,
                              OutputIterator result) {
  detail::copy_loop(first, last, result);
  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator2 copy(ExecutionPolicy &&, ForwardIterator1 first,
                      ForwardIterator1 last, ForwardIterator2 result) {
  return [&]() noexcept {
    detail::copy_loop(first, last, result);
    return result;
  }();
}

template <class InputIterator, class Size, class OutputIterator>
constexpr OutputIterator copy_n(InputIterator first, Size count,
                                OutputIterator result) {
  detail::copy_n_loop(first, count, result);
  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class Size,
          class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator2 copy_n(ExecutionPolicy &&, ForwardIterator1 first, Size count,
                        ForwardIterator2 result) {
  return [&]() noexcept {
    detail::copy_n_loop(first, count, result);
    return result;
  }();
}

template <class InputIterator, class OutputIterator, class Predicate>
constexpr OutputIterator copy_if(InputIterator first, InputIterator last,
                                 OutputIterator result, Predicate predicate) {
  detail::copy_if_loop(first, last, result, predicate);
  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class Predicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator2 copy_if(ExecutionPolicy &&, ForwardIterator1 first,
                         ForwardIterator1 last, ForwardIterator2 result,
                         Predicate predicate) {
  return [&]() noexcept {
    detail::copy_if_loop(first, last, result, predicate);
    return result;
  }();
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
constexpr BidirectionalIterator2 copy_backward(BidirectionalIterator1 first,
                                               BidirectionalIterator1 last,
                                               BidirectionalIterator2 result) {
  while (first != last) {
    --last;
    --result;
    *result = *last;
  }

  return result;
}

template <class InputIterator, class OutputIterator>
constexpr OutputIterator move(InputIterator first, InputIterator last,
                              OutputIterator result) {
  detail::move_loop(first, last, result);
  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator2 move(ExecutionPolicy &&, ForwardIterator1 first,
                      ForwardIterator1 last, ForwardIterator2 result) {
  return [&]() noexcept {
    detail::move_loop(first, last, result);
    return result;
  }();
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
constexpr BidirectionalIterator2 move_backward(BidirectionalIterator1 first,
                                               BidirectionalIterator1 last,
                                               BidirectionalIterator2 result) {
  while (first != last) {
    --last;
    --result;
    *result = std::move(*last);
  }

  return result;
}

namespace ranges {

template <class Iterator, class Output>
using copy_result = in_out_result<Iterator, Output>;

template <class Iterator, class Output>
using copy_n_result = in_out_result<Iterator, Output>;

template <class Iterator, class Output>
using copy_if_result = in_out_result<Iterator, Output>;

template <class Iterator, class Output>
using copy_backward_result = in_out_result<Iterator, Output>;

template <class Iterator, class Output>
using move_result = in_out_result<Iterator, Output>;

template <class Iterator, class Output>
using move_backward_result = in_out_result<Iterator, Output>;

struct copy_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            weakly_incrementable Output>
    requires indirectly_copyable<Iterator, Output>
  constexpr copy_result<Iterator, Output>
  operator()(Iterator first, Sentinel last, Output result) const {
    std::detail::copy_loop(first, last, result);

    return {
        std::move(first),
        std::move(result),
    };
  }

  template <input_range Range, weakly_incrementable Output>
    requires indirectly_copyable<iterator_t<Range>, Output>
  constexpr copy_result<borrowed_iterator_t<Range>, Output>
  operator()(Range &&range, Output result) const {
    auto converted = (*this)(ranges::begin(range), ranges::end(range),
                             std::move(result));

    return {
        std::move(converted.in),
        std::move(converted.out),
    };
  }
};

struct copy_n_fn {
  template <input_iterator Iterator, weakly_incrementable Output>
    requires indirectly_copyable<Iterator, Output>
  constexpr copy_n_result<Iterator, Output>
  operator()(Iterator first, iter_difference_t<Iterator> count,
             Output result) const {
    std::detail::copy_n_loop(first, count, result);

    return {
        std::move(first),
        std::move(result),
    };
  }
};

struct copy_if_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            weakly_incrementable Output, class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
    requires indirectly_copyable<Iterator, Output>
  constexpr copy_if_result<Iterator, Output>
  operator()(Iterator first, Sentinel last, Output result, Predicate predicate,
             Projection projection = {}) const {
    std::detail::ranges_copy_if_loop(first, last, result,
                                                         predicate, projection);

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
  constexpr copy_if_result<borrowed_iterator_t<Range>, Output>
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

struct copy_backward_fn {
  template <bidirectional_iterator Iterator, sentinel_for<Iterator> Sentinel,
            bidirectional_iterator Output>
    requires indirectly_copyable<Iterator, Output>
  constexpr copy_backward_result<Iterator, Output>
  operator()(Iterator first, Sentinel last, Output result) const {
    auto input_end = std::detail::ranges_copy_backward_loop(
        std::move(first),
        std::move(last), result);

    return {
        std::move(input_end),
        std::move(result),
    };
  }

  template <bidirectional_range Range, bidirectional_iterator Output>
    requires indirectly_copyable<iterator_t<Range>, Output>
  constexpr copy_backward_result<borrowed_iterator_t<Range>, Output>
  operator()(Range &&range, Output result) const {
    auto converted = (*this)(ranges::begin(range), ranges::end(range),
                             std::move(result));

    return {
        std::move(converted.in),
        std::move(converted.out),
    };
  }
};

struct move_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            weakly_incrementable Output>
    requires indirectly_movable<Iterator, Output>
  constexpr move_result<Iterator, Output>
  operator()(Iterator first, Sentinel last, Output result) const {
    std::detail::ranges_move_loop(first, last, result);

    return {
        std::move(first),
        std::move(result),
    };
  }

  template <input_range Range, weakly_incrementable Output>
    requires indirectly_movable<iterator_t<Range>, Output>
  constexpr move_result<borrowed_iterator_t<Range>, Output>
  operator()(Range &&range, Output result) const {
    auto converted = (*this)(ranges::begin(range), ranges::end(range),
                             std::move(result));

    return {
        std::move(converted.in),
        std::move(converted.out),
    };
  }
};

struct move_backward_fn {
  template <bidirectional_iterator Iterator, sentinel_for<Iterator> Sentinel,
            bidirectional_iterator Output>
    requires indirectly_movable<Iterator, Output>
  constexpr move_backward_result<Iterator, Output>
  operator()(Iterator first, Sentinel last, Output result) const {
    auto input_end = std::detail::ranges_move_backward_loop(
        std::move(first),
        std::move(last), result);

    return {
        std::move(input_end),
        std::move(result),
    };
  }

  template <bidirectional_range Range, bidirectional_iterator Output>
    requires indirectly_movable<iterator_t<Range>, Output>
  constexpr move_backward_result<borrowed_iterator_t<Range>, Output>
  operator()(Range &&range, Output result) const {
    auto converted = (*this)(ranges::begin(range), ranges::end(range),
                             std::move(result));

    return {
        std::move(converted.in),
        std::move(converted.out),
    };
  }
};

inline constexpr copy_fn copy{};
inline constexpr copy_n_fn copy_n{};
inline constexpr copy_if_fn copy_if{};
inline constexpr copy_backward_fn copy_backward{};
inline constexpr move_fn move{};
inline constexpr move_backward_fn move_backward{};

} // namespace ranges

} // namespace std


#endif // FTL_COPY_MOVE_HEADER