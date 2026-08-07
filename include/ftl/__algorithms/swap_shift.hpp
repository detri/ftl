// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_SWAP_SHIFT_HEADER
#define FTL_SWAP_SHIFT_HEADER

#ifdef FTL_REPLACE_STL
#include <__algorithms/result.hpp>
#include <__algorithms/reverse_rotate.hpp>
#include <__execution/policy_access.hpp>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#else
#include <ftl/__algorithms/result.hpp>
#include <ftl/__algorithms/reverse_rotate.hpp>
#include <ftl/__execution/policy_access.hpp>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_SWAP_SHIFT_NAMESPACE std
#else
#define FTL_SWAP_SHIFT_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace detail {

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2>
constexpr void swap_ranges_loop(Iterator1 &first1, const Sentinel1 &last1,
                                Iterator2 &first2, const Sentinel2 &last2) {
  while (first1 != last1 && first2 != last2) {
    ranges::iter_swap(first1, first2);
    ++first1;
    ++first2;
  }
}

template <class Iterator, class Sentinel, class Difference>
constexpr ranges::subrange<Iterator>
shift_left_loop(Iterator first, Sentinel last, Difference count) {
  Iterator original_first = first;

  if (count <= 0) {
    Iterator end = first;
    ranges::advance(end, last);

    return {
        FTL_SWAP_SHIFT_NAMESPACE::move(original_first),
        FTL_SWAP_SHIFT_NAMESPACE::move(end),
    };
  }

  Iterator source = first;
  Difference advanced = 0;

  while (advanced < count && source != last) {
    ++source;
    ++advanced;
  }

  if (source == last) {
    return {
        FTL_SWAP_SHIFT_NAMESPACE::move(original_first),
        FTL_SWAP_SHIFT_NAMESPACE::move(first),
    };
  }

  while (source != last) {
    *first = ranges::iter_move(source);
    ++first;
    ++source;
  }

  return {
      FTL_SWAP_SHIFT_NAMESPACE::move(original_first),
      FTL_SWAP_SHIFT_NAMESPACE::move(first),
  };
}

template <class Iterator, class Sentinel, class Difference>
constexpr ranges::subrange<Iterator>
shift_right_loop(Iterator first, Sentinel last, Difference count) {
  Iterator original_first = first;

  if (count <= 0) {
    Iterator end = first;
    ranges::advance(end, last);

    return {
        FTL_SWAP_SHIFT_NAMESPACE::move(original_first),
        FTL_SWAP_SHIFT_NAMESPACE::move(end),
    };
  }

  /*
   * lead is first + count. If that reaches the sentinel,
   * count is at least the range length.
   */
  Iterator lead = first;
  Difference advanced = 0;

  while (advanced < count && lead != last) {
    ++lead;
    ++advanced;
  }

  if (lead == last) {
    return {
        FTL_SWAP_SHIFT_NAMESPACE::move(lead),
        FTL_SWAP_SHIFT_NAMESPACE::move(lead),
    };
  }

  Iterator new_first = lead;

  /*
   * Advance lead to the concrete end while lag follows it.
   * At completion, lag is end - count.
   */
  Iterator lag = first;

  while (lead != last) {
    ++lead;
    ++lag;
  }

  Iterator end = lead;

  if constexpr (bidirectional_iterator<Iterator>) {
    Iterator source = lag;
    Iterator destination = end;

    while (source != first) {
      --source;
      --destination;
      *destination = ranges::iter_move(source);
    }
  } else {
    /*
     * For forward iterators, rotating [first, end-count)
     * behind [end-count, end) places the preserved elements
     * into [first+count, end). The leading elements are
     * permitted to have unspecified values.
     */
    detail::rotate_loop(FTL_SWAP_SHIFT_NAMESPACE::move(first),
                        FTL_SWAP_SHIFT_NAMESPACE::move(lag), end);
  }

  return {
      FTL_SWAP_SHIFT_NAMESPACE::move(new_first),
      FTL_SWAP_SHIFT_NAMESPACE::move(end),
  };
}

} // namespace detail

template <class ForwardIterator1, class ForwardIterator2>
constexpr ForwardIterator2 swap_ranges(ForwardIterator1 first1,
                                       ForwardIterator1 last1,
                                       ForwardIterator2 first2) {
  for (; first1 != last1; ++first1, ++first2) {
    ranges::iter_swap(first1, first2);
  }

  return first2;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator2 swap_ranges(ExecutionPolicy &&, ForwardIterator1 first1,
                             ForwardIterator1 last1, ForwardIterator2 first2) {
  return [&]() noexcept {
    for (; first1 != last1; ++first1, ++first2) {
      ranges::iter_swap(first1, first2);
    }

    return first2;
  }();
}

template <class ForwardIterator>
constexpr ForwardIterator
shift_left(ForwardIterator first, ForwardIterator last,
           typename iterator_traits<ForwardIterator>::difference_type count) {
  return detail::shift_left_loop(FTL_SWAP_SHIFT_NAMESPACE::move(first),
                                 FTL_SWAP_SHIFT_NAMESPACE::move(last), count)
      .end();
}

template <class ExecutionPolicy, class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator
shift_left(ExecutionPolicy &&, ForwardIterator first, ForwardIterator last,
           typename iterator_traits<ForwardIterator>::difference_type count) {
  return [&]() noexcept {
    return detail::shift_left_loop(FTL_SWAP_SHIFT_NAMESPACE::move(first),
                                   FTL_SWAP_SHIFT_NAMESPACE::move(last), count)
        .end();
  }();
}

template <class ForwardIterator>
constexpr ForwardIterator
shift_right(ForwardIterator first, ForwardIterator last,
            typename iterator_traits<ForwardIterator>::difference_type count) {
  return detail::shift_right_loop(FTL_SWAP_SHIFT_NAMESPACE::move(first),
                                  FTL_SWAP_SHIFT_NAMESPACE::move(last), count)
      .begin();
}

template <class ExecutionPolicy, class ForwardIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator
shift_right(ExecutionPolicy &&, ForwardIterator first, ForwardIterator last,
            typename iterator_traits<ForwardIterator>::difference_type count) {
  return [&]() noexcept {
    return detail::shift_right_loop(FTL_SWAP_SHIFT_NAMESPACE::move(first),
                                    FTL_SWAP_SHIFT_NAMESPACE::move(last), count)
        .begin();
  }();
}

namespace ranges {

template <class Iterator1, class Iterator2>
using swap_ranges_result = in_in_result<Iterator1, Iterator2>;

struct swap_ranges_fn {
  template <input_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
            input_iterator Iterator2, sentinel_for<Iterator2> Sentinel2>
    requires indirectly_swappable<Iterator1, Iterator2>
  constexpr swap_ranges_result<Iterator1, Iterator2>
  operator()(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
             Sentinel2 last2) const {
    FTL_SWAP_SHIFT_NAMESPACE::detail::swap_ranges_loop(first1, last1, first2,
                                                       last2);

    return {
        FTL_SWAP_SHIFT_NAMESPACE::move(first1),
        FTL_SWAP_SHIFT_NAMESPACE::move(first2),
    };
  }

  template <input_range Range1, input_range Range2>
    requires indirectly_swappable<iterator_t<Range1>, iterator_t<Range2>>
  constexpr swap_ranges_result<borrowed_iterator_t<Range1>,
                               borrowed_iterator_t<Range2>>
  operator()(Range1 &&range1, Range2 &&range2) const {
    auto result = (*this)(ranges::begin(range1), ranges::end(range1),
                          ranges::begin(range2), ranges::end(range2));

    return {
        FTL_SWAP_SHIFT_NAMESPACE::move(result.in1),
        FTL_SWAP_SHIFT_NAMESPACE::move(result.in2),
    };
  }
};

struct shift_left_fn {
  template <permutable Iterator, sentinel_for<Iterator> Sentinel>
  constexpr subrange<Iterator>
  operator()(Iterator first, Sentinel last,
             iter_difference_t<Iterator> count) const {
    return FTL_SWAP_SHIFT_NAMESPACE::detail::shift_left_loop(
        FTL_SWAP_SHIFT_NAMESPACE::move(first),
        FTL_SWAP_SHIFT_NAMESPACE::move(last), count);
  }

  template <forward_range Range>
    requires permutable<iterator_t<Range>>
  constexpr borrowed_subrange_t<Range>
  operator()(Range &&range, range_difference_t<Range> count) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range), count);

    return FTL_SWAP_SHIFT_NAMESPACE::move(result);
  }
};

struct shift_right_fn {
  template <permutable Iterator, sentinel_for<Iterator> Sentinel>
  constexpr subrange<Iterator>
  operator()(Iterator first, Sentinel last,
             iter_difference_t<Iterator> count) const {
    return FTL_SWAP_SHIFT_NAMESPACE::detail::shift_right_loop(
        FTL_SWAP_SHIFT_NAMESPACE::move(first),
        FTL_SWAP_SHIFT_NAMESPACE::move(last), count);
  }

  template <forward_range Range>
    requires permutable<iterator_t<Range>>
  constexpr borrowed_subrange_t<Range>
  operator()(Range &&range, range_difference_t<Range> count) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range), count);

    return FTL_SWAP_SHIFT_NAMESPACE::move(result);
  }
};

inline constexpr swap_ranges_fn swap_ranges{};
inline constexpr shift_left_fn shift_left{};
inline constexpr shift_right_fn shift_right{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_SWAP_SHIFT_NAMESPACE

#endif // FTL_SWAP_SHIFT_HEADER
