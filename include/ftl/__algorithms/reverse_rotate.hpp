// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_REVERSE_ROTATE_HEADER
#define FTL_REVERSE_ROTATE_HEADER

#ifdef FTL_REPLACE_STL
#include <__algorithms/result.hpp>
#include <__execution/policy_access.hpp>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#else
#include <ftl/__algorithms/result.hpp>
#include <ftl/__execution/policy_access.hpp>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_REVERSE_ROTATE_NAMESPACE std
#else
#define FTL_REVERSE_ROTATE_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

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

  reverse_loop(FTL_REVERSE_ROTATE_NAMESPACE::move(first), end);

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
      rotate_loop(FTL_REVERSE_ROTATE_NAMESPACE::move(first),
                  FTL_REVERSE_ROTATE_NAMESPACE::move(middle), end);

  return {
      FTL_REVERSE_ROTATE_NAMESPACE::move(result),
      FTL_REVERSE_ROTATE_NAMESPACE::move(end),
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
  detail::reverse_loop(FTL_REVERSE_ROTATE_NAMESPACE::move(first),
                       FTL_REVERSE_ROTATE_NAMESPACE::move(last));
}

template <class ExecutionPolicy, class BidirectionalIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void reverse(ExecutionPolicy &&, BidirectionalIterator first,
             BidirectionalIterator last) {
  [&]() noexcept {
    detail::reverse_loop(FTL_REVERSE_ROTATE_NAMESPACE::move(first),
                         FTL_REVERSE_ROTATE_NAMESPACE::move(last));
  }();
}

template <class BidirectionalIterator, class OutputIterator>
constexpr OutputIterator reverse_copy(BidirectionalIterator first,
                                      BidirectionalIterator last,
                                      OutputIterator result) {
  detail::reverse_copy_loop(FTL_REVERSE_ROTATE_NAMESPACE::move(first),
                            FTL_REVERSE_ROTATE_NAMESPACE::move(last), result);

  return result;
}

template <class ExecutionPolicy, class BidirectionalIterator,
          class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator reverse_copy(ExecutionPolicy &&, BidirectionalIterator first,
                             BidirectionalIterator last,
                             ForwardIterator result) {
  return [&]() noexcept {
    detail::reverse_copy_loop(FTL_REVERSE_ROTATE_NAMESPACE::move(first),
                              FTL_REVERSE_ROTATE_NAMESPACE::move(last), result);

    return result;
  }();
}

template <class ForwardIterator>
constexpr ForwardIterator rotate(ForwardIterator first, ForwardIterator middle,
                                 ForwardIterator last) {
  return detail::rotate_loop(FTL_REVERSE_ROTATE_NAMESPACE::move(first),
                             FTL_REVERSE_ROTATE_NAMESPACE::move(middle),
                             FTL_REVERSE_ROTATE_NAMESPACE::move(last));
}

template <class ExecutionPolicy, class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator rotate(ExecutionPolicy &&, ForwardIterator first,
                       ForwardIterator middle, ForwardIterator last) {
  return [&]() noexcept {
    return detail::rotate_loop(FTL_REVERSE_ROTATE_NAMESPACE::move(first),
                               FTL_REVERSE_ROTATE_NAMESPACE::move(middle),
                               FTL_REVERSE_ROTATE_NAMESPACE::move(last));
  }();
}

template <class ForwardIterator, class OutputIterator>
constexpr OutputIterator
rotate_copy(ForwardIterator first, ForwardIterator middle, ForwardIterator last,
            OutputIterator result) {
  detail::rotate_copy_loop(FTL_REVERSE_ROTATE_NAMESPACE::move(first),
                           FTL_REVERSE_ROTATE_NAMESPACE::move(middle),
                           FTL_REVERSE_ROTATE_NAMESPACE::move(last), result);

  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator2 rotate_copy(ExecutionPolicy &&, ForwardIterator1 first,
                             ForwardIterator1 middle, ForwardIterator1 last,
                             ForwardIterator2 result) {
  return [&]() noexcept {
    detail::rotate_copy_loop(FTL_REVERSE_ROTATE_NAMESPACE::move(first),
                             FTL_REVERSE_ROTATE_NAMESPACE::move(middle),
                             FTL_REVERSE_ROTATE_NAMESPACE::move(last), result);

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
    return FTL_REVERSE_ROTATE_NAMESPACE::detail::ranges_reverse_loop(
        FTL_REVERSE_ROTATE_NAMESPACE::move(first),
        FTL_REVERSE_ROTATE_NAMESPACE::move(last));
  }

  template <bidirectional_range Range>
    requires permutable<iterator_t<Range>>
  constexpr borrowed_iterator_t<Range> operator()(Range &&range) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range));

    return FTL_REVERSE_ROTATE_NAMESPACE::move(result);
  }
};

struct reverse_copy_fn {
  template <bidirectional_iterator Iterator, sentinel_for<Iterator> Sentinel,
            weakly_incrementable Output>
    requires indirectly_copyable<Iterator, Output>
  constexpr reverse_copy_result<Iterator, Output>
  operator()(Iterator first, Sentinel last, Output result) const {
    Iterator input_end =
        FTL_REVERSE_ROTATE_NAMESPACE::detail::reverse_copy_loop(
            FTL_REVERSE_ROTATE_NAMESPACE::move(first),
            FTL_REVERSE_ROTATE_NAMESPACE::move(last), result);

    return {
        FTL_REVERSE_ROTATE_NAMESPACE::move(input_end),
        FTL_REVERSE_ROTATE_NAMESPACE::move(result),
    };
  }

  template <bidirectional_range Range, weakly_incrementable Output>
    requires indirectly_copyable<iterator_t<Range>, Output>
  constexpr reverse_copy_result<borrowed_iterator_t<Range>, Output>
  operator()(Range &&range, Output result) const {
    auto converted = (*this)(ranges::begin(range), ranges::end(range),
                             FTL_REVERSE_ROTATE_NAMESPACE::move(result));

    return {
        FTL_REVERSE_ROTATE_NAMESPACE::move(converted.in),
        FTL_REVERSE_ROTATE_NAMESPACE::move(converted.out),
    };
  }
};

struct rotate_fn {
  template <permutable Iterator, sentinel_for<Iterator> Sentinel>
  constexpr subrange<Iterator> operator()(Iterator first, Iterator middle,
                                          Sentinel last) const {
    return FTL_REVERSE_ROTATE_NAMESPACE::detail::ranges_rotate_loop(
        FTL_REVERSE_ROTATE_NAMESPACE::move(first),
        FTL_REVERSE_ROTATE_NAMESPACE::move(middle),
        FTL_REVERSE_ROTATE_NAMESPACE::move(last));
  }

  template <forward_range Range>
    requires permutable<iterator_t<Range>>
  constexpr borrowed_subrange_t<Range>
  operator()(Range &&range, iterator_t<Range> middle) const {
    auto result =
        (*this)(ranges::begin(range),
                FTL_REVERSE_ROTATE_NAMESPACE::move(middle), ranges::end(range));

    return FTL_REVERSE_ROTATE_NAMESPACE::move(result);
  }
};

struct rotate_copy_fn {
  template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel,
            weakly_incrementable Output>
    requires indirectly_copyable<Iterator, Output>
  constexpr rotate_copy_result<Iterator, Output>
  operator()(Iterator first, Iterator middle, Sentinel last,
             Output result) const {
    Iterator input_end = FTL_REVERSE_ROTATE_NAMESPACE::detail::rotate_copy_loop(
        FTL_REVERSE_ROTATE_NAMESPACE::move(first),
        FTL_REVERSE_ROTATE_NAMESPACE::move(middle),
        FTL_REVERSE_ROTATE_NAMESPACE::move(last), result);

    return {
        FTL_REVERSE_ROTATE_NAMESPACE::move(input_end),
        FTL_REVERSE_ROTATE_NAMESPACE::move(result),
    };
  }

  template <forward_range Range, weakly_incrementable Output>
    requires indirectly_copyable<iterator_t<Range>, Output>
  constexpr rotate_copy_result<borrowed_iterator_t<Range>, Output>
  operator()(Range &&range, iterator_t<Range> middle, Output result) const {
    auto converted = (*this)(
        ranges::begin(range), FTL_REVERSE_ROTATE_NAMESPACE::move(middle),
        ranges::end(range), FTL_REVERSE_ROTATE_NAMESPACE::move(result));

    return {
        FTL_REVERSE_ROTATE_NAMESPACE::move(converted.in),
        FTL_REVERSE_ROTATE_NAMESPACE::move(converted.out),
    };
  }
};

inline constexpr reverse_fn reverse{};
inline constexpr reverse_copy_fn reverse_copy{};
inline constexpr rotate_fn rotate{};
inline constexpr rotate_copy_fn rotate_copy{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_REVERSE_ROTATE_NAMESPACE

#endif // FTL_REVERSE_ROTATE_HEADER