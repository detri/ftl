// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_FILL_HEADER
#define FTL_FILL_HEADER

#include <__execution/policy_access.hpp>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>


namespace std {

namespace detail {

template <class Iterator, class Sentinel, class T>
constexpr void fill_loop(Iterator &first, const Sentinel &last,
                         const T &value) {
  for (; first != last; ++first) {
    *first = value;
  }
}

template <class Iterator, class Size, class T>
constexpr void fill_n_loop(Iterator &first, Size count, const T &value) {
  auto remaining = +count;

  for (; remaining > 0; --remaining, ++first) {
    *first = value;
  }
}

} // namespace detail

template <class ForwardIterator, class T>
constexpr void fill(ForwardIterator first, ForwardIterator last,
                    const T &value) {
  detail::fill_loop(first, last, value);
}

template <class ExecutionPolicy, class ForwardIterator, class T>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void fill(ExecutionPolicy &&, ForwardIterator first, ForwardIterator last,
          const T &value) {
  [&]() noexcept { detail::fill_loop(first, last, value); }();
}

template <class OutputIterator, class Size, class T>
constexpr OutputIterator fill_n(OutputIterator first, Size count,
                                const T &value) {
  detail::fill_n_loop(first, count, value);
  return first;
}

template <class ExecutionPolicy, class ForwardIterator, class Size, class T>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator fill_n(ExecutionPolicy &&, ForwardIterator first, Size count,
                       const T &value) {
  return [&]() noexcept {
    detail::fill_n_loop(first, count, value);
    return first;
  }();
}

namespace ranges {

struct fill_fn {
  template <class T, output_iterator<const T &> Output,
            sentinel_for<Output> Sentinel>
  constexpr Output operator()(Output first, Sentinel last,
                              const T &value) const {
    std::detail::fill_loop(first, last, value);

    return std::move(first);
  }

  template <class T, output_range<const T &> Range>
  constexpr borrowed_iterator_t<Range> operator()(Range &&range,
                                                  const T &value) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range), value);

    return std::move(result);
  }
};

struct fill_n_fn {
  template <class T, output_iterator<const T &> Output>
  constexpr Output operator()(Output first, iter_difference_t<Output> count,
                              const T &value) const {
    std::detail::fill_n_loop(first, count, value);

    return std::move(first);
  }
};

inline constexpr fill_fn fill{};
inline constexpr fill_n_fn fill_n{};

} // namespace ranges

} // namespace std


#endif // FTL_FILL_HEADER
