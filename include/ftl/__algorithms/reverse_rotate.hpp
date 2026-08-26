// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_REVERSE_ROTATE_HEADER
#define FTL_REVERSE_ROTATE_HEADER

#include <__algorithms/result.hpp>
#include <__execution/policy_access.hpp>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>


namespace std {

namespace detail {

template <class Iterator>
constexpr Iterator reverse_loop(Iterator first, Iterator last) {
  Iterator end = last;

  while (first != last) {
    --last;

    if (first == last) {
      break;
    }

    ranges::iter_swap(first, last);
    ++first;
  }

  return end;
}

template <class Iterator, class Sentinel>
constexpr Iterator ranges_reverse_loop(Iterator first, Sentinel last) {
  Iterator end = first;
  ranges::advance(end, last);

  reverse_loop(std::move(first), end);

  return end;
}

template <class Iterator, class Sentinel, class Output>
constexpr Iterator reverse_copy_loop(Iterator first, Sentinel last,
                                     Output &result) {
  Iterator input_end = first;
  ranges::advance(input_end, last);

  Iterator current = input_end;

  while (first != current) {
    --current;
    *result = *current;
    ++result;
  }

  return input_end;
}

/*
 * Forward-iterator rotation using the cycle-exchange algorithm.
 *
 * This performs at most last - first swaps and does not require
 * bidirectional traversal or additional storage.
 */
template <class Iterator>
constexpr Iterator rotate_loop(Iterator first, Iterator middle, Iterator last) {
  if (first == middle) {
    return last;
  }

  if (middle == last) {
    return first;
  }

  Iterator second = middle;

  do {
    ranges::iter_swap(first, second);
    ++first;
    ++second;

    if (first == middle) {
      middle = second;
    }
  } while (second != last);

  Iterator result = first;
  second = middle;

  while (second != last) {
    ranges::iter_swap(first, second);
    ++first;
    ++second;

    if (first == middle) {
      middle = second;
    } else if (second == last) {
      second = middle;
    }
  }

  return result;
}

template <class Iterator, class Sentinel>
constexpr ranges::subrange<Iterator>
ranges_rotate_loop(Iterator first, Iterator middle, Sentinel last) {
  Iterator end = first;
  ranges::advance(end, last);

  Iterator result =
      rotate_loop(std::move(first),
                  std::move(middle), end);

  return {
      std::move(result),
      std::move(end),
  };
}

template <class Iterator, class Sentinel, class Output>
constexpr Iterator rotate_copy_loop(Iterator first, Iterator middle,
                                    Sentinel last, Output &result) {
  Iterator input_end = middle;

  for (; input_end != last; ++input_end, ++result) {
    *result = *input_end;
  }

  for (; first != middle; ++first, ++result) {
    *result = *first;
  }

  return input_end;
}

} // namespace detail

template <class BidirectionalIterator>
constexpr void reverse(BidirectionalIterator first,
                       BidirectionalIterator last) {
  detail::reverse_loop(std::move(first),
                       std::move(last));
}

template <class ExecutionPolicy, class BidirectionalIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void reverse(ExecutionPolicy &&, BidirectionalIterator first,
             BidirectionalIterator last) {
  [&]() noexcept {
    detail::reverse_loop(std::move(first),
                         std::move(last));
  }();
}

template <class BidirectionalIterator, class OutputIterator>
constexpr OutputIterator reverse_copy(BidirectionalIterator first,
                                      BidirectionalIterator last,
                                      OutputIterator result) {
  detail::reverse_copy_loop(std::move(first),
                            std::move(last), result);

  return result;
}

template <class ExecutionPolicy, class BidirectionalIterator,
          class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator reverse_copy(ExecutionPolicy &&, BidirectionalIterator first,
                             BidirectionalIterator last,
                             ForwardIterator result) {
  return [&]() noexcept {
    detail::reverse_copy_loop(std::move(first),
                              std::move(last), result);

    return result;
  }();
}

template <class ForwardIterator>
constexpr ForwardIterator rotate(ForwardIterator first, ForwardIterator middle,
                                 ForwardIterator last) {
  return detail::rotate_loop(std::move(first),
                             std::move(middle),
                             std::move(last));
}

template <class ExecutionPolicy, class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator rotate(ExecutionPolicy &&, ForwardIterator first,
                       ForwardIterator middle, ForwardIterator last) {
  return [&]() noexcept {
    return detail::rotate_loop(std::move(first),
                               std::move(middle),
                               std::move(last));
  }();
}

template <class ForwardIterator, class OutputIterator>
constexpr OutputIterator
rotate_copy(ForwardIterator first, ForwardIterator middle, ForwardIterator last,
            OutputIterator result) {
  detail::rotate_copy_loop(std::move(first),
                           std::move(middle),
                           std::move(last), result);

  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator2 rotate_copy(ExecutionPolicy &&, ForwardIterator1 first,
                             ForwardIterator1 middle, ForwardIterator1 last,
                             ForwardIterator2 result) {
  return [&]() noexcept {
    detail::rotate_copy_loop(std::move(first),
                             std::move(middle),
                             std::move(last), result);

    return result;
  }();
}

namespace ranges {

template <class Iterator, class Output>
using reverse_copy_result = in_out_result<Iterator, Output>;

template <class Iterator, class Output>
using rotate_copy_result = in_out_result<Iterator, Output>;

struct reverse_fn {
  template <bidirectional_iterator Iterator, sentinel_for<Iterator> Sentinel>
    requires permutable<Iterator>
  constexpr Iterator operator()(Iterator first, Sentinel last) const {
    return std::detail::ranges_reverse_loop(
        std::move(first),
        std::move(last));
  }

  template <bidirectional_range Range>
    requires permutable<iterator_t<Range>>
  constexpr borrowed_iterator_t<Range> operator()(Range &&range) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range));

    return std::move(result);
  }
};

struct reverse_copy_fn {
  template <bidirectional_iterator Iterator, sentinel_for<Iterator> Sentinel,
            weakly_incrementable Output>
    requires indirectly_copyable<Iterator, Output>
  constexpr reverse_copy_result<Iterator, Output>
  operator()(Iterator first, Sentinel last, Output result) const {
    Iterator input_end =
        std::detail::reverse_copy_loop(
            std::move(first),
            std::move(last), result);

    return {
        std::move(input_end),
        std::move(result),
    };
  }

  template <bidirectional_range Range, weakly_incrementable Output>
    requires indirectly_copyable<iterator_t<Range>, Output>
  constexpr reverse_copy_result<borrowed_iterator_t<Range>, Output>
  operator()(Range &&range, Output result) const {
    auto converted = (*this)(ranges::begin(range), ranges::end(range),
                             std::move(result));

    return {
        std::move(converted.in),
        std::move(converted.out),
    };
  }
};

struct rotate_fn {
  template <permutable Iterator, sentinel_for<Iterator> Sentinel>
  constexpr subrange<Iterator> operator()(Iterator first, Iterator middle,
                                          Sentinel last) const {
    return std::detail::ranges_rotate_loop(
        std::move(first),
        std::move(middle),
        std::move(last));
  }

  template <forward_range Range>
    requires permutable<iterator_t<Range>>
  constexpr borrowed_subrange_t<Range>
  operator()(Range &&range, iterator_t<Range> middle) const {
    auto result =
        (*this)(ranges::begin(range),
                std::move(middle), ranges::end(range));

    return std::move(result);
  }
};

struct rotate_copy_fn {
  template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel,
            weakly_incrementable Output>
    requires indirectly_copyable<Iterator, Output>
  constexpr rotate_copy_result<Iterator, Output>
  operator()(Iterator first, Iterator middle, Sentinel last,
             Output result) const {
    Iterator input_end = std::detail::rotate_copy_loop(
        std::move(first),
        std::move(middle),
        std::move(last), result);

    return {
        std::move(input_end),
        std::move(result),
    };
  }

  template <forward_range Range, weakly_incrementable Output>
    requires indirectly_copyable<iterator_t<Range>, Output>
  constexpr rotate_copy_result<borrowed_iterator_t<Range>, Output>
  operator()(Range &&range, iterator_t<Range> middle, Output result) const {
    auto converted = (*this)(
        ranges::begin(range), std::move(middle),
        ranges::end(range), std::move(result));

    return {
        std::move(converted.in),
        std::move(converted.out),
    };
  }
};

inline constexpr reverse_fn reverse{};
inline constexpr reverse_copy_fn reverse_copy{};
inline constexpr rotate_fn rotate{};
inline constexpr rotate_copy_fn rotate_copy{};

} // namespace ranges

} // namespace std


#endif // FTL_REVERSE_ROTATE_HEADER