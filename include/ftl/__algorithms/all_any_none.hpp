// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_ALL_ANY_NONE_HEADER
#define FTL_ALL_ANY_NONE_HEADER

#include <__execution/policy_access.hpp>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>


namespace std {

namespace detail {

template <class Iterator, class Sentinel, class Predicate>
constexpr bool all_of_loop(Iterator &first, const Sentinel &last,
                           Predicate &predicate) {
  for (; first != last; ++first) {
    if (!predicate(*first)) {
      return false;
    }
  }

  return true;
}

template <class Iterator, class Sentinel, class Predicate>
constexpr bool any_of_loop(Iterator &first, const Sentinel &last,
                           Predicate &predicate) {
  for (; first != last; ++first) {
    if (predicate(*first)) {
      return true;
    }
  }

  return false;
}

template <class Iterator, class Sentinel, class Predicate>
constexpr bool none_of_loop(Iterator &first, const Sentinel &last,
                            Predicate &predicate) {
  for (; first != last; ++first) {
    if (predicate(*first)) {
      return false;
    }
  }

  return true;
}

template <class Iterator, class Sentinel, class Predicate, class Projection>
constexpr bool ranges_all_of_loop(Iterator &first, const Sentinel &last,
                                  Predicate &predicate,
                                  Projection &projection) {
  for (; first != last; ++first) {
    if (!std::invoke(
            predicate,
            std::invoke(projection, *first))) {
      return false;
    }
  }

  return true;
}

template <class Iterator, class Sentinel, class Predicate, class Projection>
constexpr bool ranges_any_of_loop(Iterator &first, const Sentinel &last,
                                  Predicate &predicate,
                                  Projection &projection) {
  for (; first != last; ++first) {
    if (std::invoke(
            predicate,
            std::invoke(projection, *first))) {
      return true;
    }
  }

  return false;
}

template <class Iterator, class Sentinel, class Predicate, class Projection>
constexpr bool ranges_none_of_loop(Iterator &first, const Sentinel &last,
                                   Predicate &predicate,
                                   Projection &projection) {
  for (; first != last; ++first) {
    if (std::invoke(
            predicate,
            std::invoke(projection, *first))) {
      return false;
    }
  }

  return true;
}

} // namespace detail

template <class InputIterator, class Predicate>
constexpr bool all_of(InputIterator first, InputIterator last,
                      Predicate predicate) {
  return detail::all_of_loop(first, last, predicate);
}

template <class ExecutionPolicy, class ForwardIterator, class Predicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool all_of(ExecutionPolicy &&, ForwardIterator first, ForwardIterator last,
            Predicate predicate) {
  return
      [&]() noexcept { return detail::all_of_loop(first, last, predicate); }();
}

template <class InputIterator, class Predicate>
constexpr bool any_of(InputIterator first, InputIterator last,
                      Predicate predicate) {
  return detail::any_of_loop(first, last, predicate);
}

template <class ExecutionPolicy, class ForwardIterator, class Predicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool any_of(ExecutionPolicy &&, ForwardIterator first, ForwardIterator last,
            Predicate predicate) {
  return
      [&]() noexcept { return detail::any_of_loop(first, last, predicate); }();
}

template <class InputIterator, class Predicate>
constexpr bool none_of(InputIterator first, InputIterator last,
                       Predicate predicate) {
  return detail::none_of_loop(first, last, predicate);
}

template <class ExecutionPolicy, class ForwardIterator, class Predicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool none_of(ExecutionPolicy &&, ForwardIterator first, ForwardIterator last,
             Predicate predicate) {
  return
      [&]() noexcept { return detail::none_of_loop(first, last, predicate); }();
}

namespace ranges {

struct all_of_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
  constexpr bool operator()(Iterator first, Sentinel last, Predicate predicate,
                            Projection projection = {}) const {
    return std::detail::ranges_all_of_loop(
        first, last, predicate, projection);
  }

  template <input_range Range, class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
  constexpr bool operator()(Range &&range, Predicate predicate,
                            Projection projection = {}) const {
    return (*this)(ranges::begin(range), ranges::end(range),
                   std::move(predicate),
                   std::move(projection));
  }
};

struct any_of_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
  constexpr bool operator()(Iterator first, Sentinel last, Predicate predicate,
                            Projection projection = {}) const {
    return std::detail::ranges_any_of_loop(
        first, last, predicate, projection);
  }

  template <input_range Range, class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
  constexpr bool operator()(Range &&range, Predicate predicate,
                            Projection projection = {}) const {
    return (*this)(ranges::begin(range), ranges::end(range),
                   std::move(predicate),
                   std::move(projection));
  }
};

struct none_of_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
  constexpr bool operator()(Iterator first, Sentinel last, Predicate predicate,
                            Projection projection = {}) const {
    return std::detail::ranges_none_of_loop(
        first, last, predicate, projection);
  }

  template <input_range Range, class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
  constexpr bool operator()(Range &&range, Predicate predicate,
                            Projection projection = {}) const {
    return (*this)(ranges::begin(range), ranges::end(range),
                   std::move(predicate),
                   std::move(projection));
  }
};

inline constexpr all_of_fn all_of{};
inline constexpr any_of_fn any_of{};
inline constexpr none_of_fn none_of{};

} // namespace ranges

} // namespace std

#endif // FTL_ALL_ANY_NONE_HEADER
