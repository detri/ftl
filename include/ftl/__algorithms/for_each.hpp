// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_FOR_EACH_HEADER
#define FTL_FOR_EACH_HEADER

#ifdef FTL_REPLACE_STL
#include <__algorithms/result.hpp>
#include <__execution/policy_access.hpp>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#else
#include <ftl/__algorithms/result.hpp>
#include <ftl/__execution/policy_access.hpp>
#include <ftl/functional>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_FOR_EACH_NAMESPACE std
#else
#define FTL_FOR_EACH_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace detail {

template <class Iterator, class Sentinel, class Function>
constexpr void for_each_loop(Iterator &first, const Sentinel &last,
                             Function &function) {
  for (; first != last; ++first) {
    function(*first);
  }
}

template <class Iterator, class Size, class Function>
constexpr void for_each_n_loop(Iterator &first, Size count,
                               Function &function) {
  auto remaining = +count;

  for (; remaining > 0; --remaining, ++first) {
    function(*first);
  }
}

template <class Iterator, class Sentinel, class Function, class Projection>
constexpr void ranges_for_each_loop(Iterator &first, const Sentinel &last,
                                    Function &function,
                                    Projection &projection) {
  for (; first != last; ++first) {
    FTL_FOR_EACH_NAMESPACE::invoke(
        function, FTL_FOR_EACH_NAMESPACE::invoke(projection, *first));
  }
}

template <class Iterator, class Function, class Projection>
constexpr void
ranges_for_each_n_loop(Iterator &first, iter_difference_t<Iterator> count,
                       Function &function, Projection &projection) {
  for (; count > 0; --count, ++first) {
    FTL_FOR_EACH_NAMESPACE::invoke(
        function, FTL_FOR_EACH_NAMESPACE::invoke(projection, *first));
  }
}

} // namespace detail

template <class InputIterator, class Function>
constexpr Function for_each(InputIterator first, InputIterator last,
                            Function function) {
  detail::for_each_loop(first, last, function);
  return FTL_FOR_EACH_NAMESPACE::move(function);
}

template <class ExecutionPolicy, class ForwardIterator, class Function>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void for_each(ExecutionPolicy &&, ForwardIterator first, ForwardIterator last,
              Function function) {
  [&]() noexcept { detail::for_each_loop(first, last, function); }();
}

template <class InputIterator, class Size, class Function>
constexpr InputIterator for_each_n(InputIterator first, Size count,
                                   Function function) {
  detail::for_each_n_loop(first, count, function);
  return first;
}

template <class ExecutionPolicy, class ForwardIterator, class Size,
          class Function>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator for_each_n(ExecutionPolicy &&, ForwardIterator first,
                           Size count, Function function) {
  return [&]() noexcept {
    detail::for_each_n_loop(first, count, function);
    return first;
  }();
}

namespace ranges {

template <class Iterator, class Function>
using for_each_result = in_fun_result<Iterator, Function>;

template <class Iterator, class Function>
using for_each_n_result = in_fun_result<Iterator, Function>;

struct for_each_fn {
  template <
      input_iterator Iterator, sentinel_for<Iterator> Sentinel,
      class Projection = identity,
      indirectly_unary_invocable<projected<Iterator, Projection>> Function>
  constexpr for_each_result<Iterator, Function>
  operator()(Iterator first, Sentinel last, Function function,
             Projection projection = {}) const {
    FTL_FOR_EACH_NAMESPACE::detail::ranges_for_each_loop(first, last, function,
                                                         projection);

    return {
        FTL_FOR_EACH_NAMESPACE::move(first),
        FTL_FOR_EACH_NAMESPACE::move(function),
    };
  }

  template <input_range Range, class Projection = identity,
            indirectly_unary_invocable<projected<iterator_t<Range>, Projection>>
                Function>
  constexpr for_each_result<borrowed_iterator_t<Range>, Function>
  operator()(Range &&range, Function function,
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_FOR_EACH_NAMESPACE::move(function),
                          FTL_FOR_EACH_NAMESPACE::move(projection));

    return {
        FTL_FOR_EACH_NAMESPACE::move(result.in),
        FTL_FOR_EACH_NAMESPACE::move(result.fun),
    };
  }
};

struct for_each_n_fn {
  template <
      input_iterator Iterator, class Projection = identity,
      indirectly_unary_invocable<projected<Iterator, Projection>> Function>
  constexpr for_each_n_result<Iterator, Function>
  operator()(Iterator first, iter_difference_t<Iterator> count,
             Function function, Projection projection = {}) const {
    FTL_FOR_EACH_NAMESPACE::detail::ranges_for_each_n_loop(
        first, count, function, projection);

    return {
        FTL_FOR_EACH_NAMESPACE::move(first),
        FTL_FOR_EACH_NAMESPACE::move(function),
    };
  }
};

inline constexpr for_each_fn for_each{};
inline constexpr for_each_n_fn for_each_n{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_FOR_EACH_NAMESPACE

#endif // FTL_FOR_EACH_HEADER