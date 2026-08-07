// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_FOLD_HEADER
#define FTL_FOLD_HEADER

#ifdef FTL_REPLACE_STL
#include <__algorithms/result.hpp>
#include <functional>
#include <iterator>
#include <optional>
#include <ranges>
#include <type_traits>
#include <utility>
#else
#include <ftl/__algorithms/result.hpp>
#include <ftl/functional>
#include <ftl/iterator>
#include <ftl/optional>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_FOLD_NAMESPACE std
#else
#define FTL_FOLD_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace ranges {
namespace detail {

template <class Function, class T, class Iterator>
using fold_left_value_t =
    decay_t<invoke_result_t<Function &, T, iter_reference_t<Iterator>>>;

template <class Function, class T, class Iterator>
using fold_right_value_t =
    decay_t<invoke_result_t<Function &, iter_reference_t<Iterator>, T>>;

template <class Function, class T, class Iterator, class U>
concept fold_left_impl =
    movable<T> && movable<U> && convertible_to<T, U> &&
    invocable<Function &, U, iter_reference_t<Iterator>> &&
    assignable_from<U &,
                    invoke_result_t<Function &, U, iter_reference_t<Iterator>>>;

template <class Function, class T, class Iterator>
concept indirectly_binary_left_foldable =
    copy_constructible<Function> && indirectly_readable<Iterator> &&
    invocable<Function &, T, iter_reference_t<Iterator>> &&
    convertible_to<invoke_result_t<Function &, T, iter_reference_t<Iterator>>,
                   fold_left_value_t<Function, T, Iterator>> &&
    fold_left_impl<Function, T, Iterator,
                   fold_left_value_t<Function, T, Iterator>>;

template <class Function, class T, class Iterator, class U>
concept fold_right_impl =
    movable<T> && movable<U> && convertible_to<T, U> &&
    invocable<Function &, iter_reference_t<Iterator>, U> &&
    assignable_from<U &,
                    invoke_result_t<Function &, iter_reference_t<Iterator>, U>>;

template <class Function, class T, class Iterator>
concept indirectly_binary_right_foldable =
    copy_constructible<Function> && indirectly_readable<Iterator> &&
    invocable<Function &, iter_reference_t<Iterator>, T> &&
    convertible_to<invoke_result_t<Function &, iter_reference_t<Iterator>, T>,
                   fold_right_value_t<Function, T, Iterator>> &&
    fold_right_impl<Function, T, Iterator,
                    fold_right_value_t<Function, T, Iterator>>;

template <class Iterator, class Sentinel, class T, class Function>
constexpr auto fold_left_with_iter_loop(Iterator first, Sentinel last, T init,
                                        Function &function)
    -> in_value_result<Iterator, fold_left_value_t<Function, T, Iterator>> {
  using U = fold_left_value_t<Function, T, Iterator>;

  if (first == last) {
    return {
        FTL_FOLD_NAMESPACE::move(first),
        U(FTL_FOLD_NAMESPACE::move(init)),
    };
  }

  U accumulator = FTL_FOLD_NAMESPACE::invoke(
      function, FTL_FOLD_NAMESPACE::move(init), *first);

  ++first;

  for (; first != last; ++first) {
    accumulator = FTL_FOLD_NAMESPACE::invoke(
        function, FTL_FOLD_NAMESPACE::move(accumulator), *first);
  }

  return {
      FTL_FOLD_NAMESPACE::move(first),
      FTL_FOLD_NAMESPACE::move(accumulator),
  };
}

template <class Iterator, class Sentinel, class Function>
constexpr auto fold_left_first_with_iter_loop(Iterator first, Sentinel last,
                                              Function &function)
    -> in_value_result<
        Iterator, optional<fold_left_value_t<Function, iter_value_t<Iterator>,
                                             Iterator>>> {
  using U = fold_left_value_t<Function, iter_value_t<Iterator>, Iterator>;

  if (first == last) {
    return {
        FTL_FOLD_NAMESPACE::move(first),
        optional<U>{},
    };
  }

  optional<U> accumulator{
      in_place,
      *first,
  };

  ++first;

  for (; first != last; ++first) {
    *accumulator = FTL_FOLD_NAMESPACE::invoke(
        function, FTL_FOLD_NAMESPACE::move(*accumulator), *first);
  }

  return {
      FTL_FOLD_NAMESPACE::move(first),
      FTL_FOLD_NAMESPACE::move(accumulator),
  };
}

template <class Iterator, class Sentinel, class T, class Function>
constexpr auto fold_right_loop(Iterator first, Sentinel last, T init,
                               Function &function)
    -> fold_right_value_t<Function, T, Iterator> {
  using U = fold_right_value_t<Function, T, Iterator>;

  if (first == last) {
    return U(FTL_FOLD_NAMESPACE::move(init));
  }

  Iterator tail = first;

  while (tail != last) {
    ++tail;
  }

  --tail;

  U accumulator = FTL_FOLD_NAMESPACE::invoke(function, *tail,
                                             FTL_FOLD_NAMESPACE::move(init));

  while (first != tail) {
    --tail;

    accumulator = FTL_FOLD_NAMESPACE::invoke(
        function, *tail, FTL_FOLD_NAMESPACE::move(accumulator));
  }

  return accumulator;
}

template <class Iterator, class Sentinel, class Function>
constexpr auto fold_right_last_loop(Iterator first, Sentinel last,
                                    Function &function)
    -> optional<
        fold_right_value_t<Function, iter_value_t<Iterator>, Iterator>> {
  using U = fold_right_value_t<Function, iter_value_t<Iterator>, Iterator>;

  if (first == last) {
    return optional<U>{};
  }

  Iterator tail = first;

  while (tail != last) {
    ++tail;
  }

  --tail;

  iter_value_t<Iterator> init(*tail);

  return optional<U>{
      in_place,
      fold_right_loop(first, tail, FTL_FOLD_NAMESPACE::move(init), function),
  };
}

} // namespace detail

template <class Iterator, class T>
using fold_left_with_iter_result = in_value_result<Iterator, T>;

template <class Iterator, class T>
using fold_left_first_with_iter_result = in_value_result<Iterator, T>;

struct fold_left_with_iter_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel, class T,
            class Function>
    requires detail::indirectly_binary_left_foldable<Function, T, Iterator>
  constexpr auto operator()(Iterator first, Sentinel last, T init,
                            Function function) const
      -> fold_left_with_iter_result<
          Iterator, detail::fold_left_value_t<Function, T, Iterator>> {
    return detail::fold_left_with_iter_loop(
        FTL_FOLD_NAMESPACE::move(first), FTL_FOLD_NAMESPACE::move(last),
        FTL_FOLD_NAMESPACE::move(init), function);
  }

  template <input_range Range, class T, class Function>
    requires detail::indirectly_binary_left_foldable<Function, T,
                                                     iterator_t<Range>>
  constexpr auto operator()(Range &&range, T init, Function function) const
      -> fold_left_with_iter_result<
          borrowed_iterator_t<Range>,
          detail::fold_left_value_t<Function, T, iterator_t<Range>>> {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_FOLD_NAMESPACE::move(init),
                          FTL_FOLD_NAMESPACE::move(function));

    return FTL_FOLD_NAMESPACE::move(result);
  }
};

struct fold_left_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel, class T,
            class Function>
    requires detail::indirectly_binary_left_foldable<Function, T, Iterator>
  constexpr auto operator()(Iterator first, Sentinel last, T init,
                            Function function) const
      -> detail::fold_left_value_t<Function, T, Iterator> {
    return detail::fold_left_with_iter_loop(
               FTL_FOLD_NAMESPACE::move(first), FTL_FOLD_NAMESPACE::move(last),
               FTL_FOLD_NAMESPACE::move(init), function)
        .value;
  }

  template <input_range Range, class T, class Function>
    requires detail::indirectly_binary_left_foldable<Function, T,
                                                     iterator_t<Range>>
  constexpr auto operator()(Range &&range, T init, Function function) const
      -> detail::fold_left_value_t<Function, T, iterator_t<Range>> {
    return (*this)(ranges::begin(range), ranges::end(range),
                   FTL_FOLD_NAMESPACE::move(init),
                   FTL_FOLD_NAMESPACE::move(function));
  }
};

struct fold_left_first_with_iter_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Function>
    requires(
        detail::indirectly_binary_left_foldable<
            Function, iter_value_t<Iterator>, Iterator> &&
        constructible_from<iter_value_t<Iterator>, iter_reference_t<Iterator>>)
  constexpr auto operator()(Iterator first, Sentinel last,
                            Function function) const
      -> fold_left_first_with_iter_result<
          Iterator, optional<detail::fold_left_value_t<
                        Function, iter_value_t<Iterator>, Iterator>>> {
    return detail::fold_left_first_with_iter_loop(
        FTL_FOLD_NAMESPACE::move(first), FTL_FOLD_NAMESPACE::move(last),
        function);
  }

  template <input_range Range, class Function>
    requires(detail::indirectly_binary_left_foldable<
                 Function, range_value_t<Range>, iterator_t<Range>> &&
             constructible_from<range_value_t<Range>, range_reference_t<Range>>)
  constexpr auto operator()(Range &&range, Function function) const
      -> fold_left_first_with_iter_result<
          borrowed_iterator_t<Range>,
          optional<detail::fold_left_value_t<Function, range_value_t<Range>,
                                             iterator_t<Range>>>> {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_FOLD_NAMESPACE::move(function));

    return FTL_FOLD_NAMESPACE::move(result);
  }
};

struct fold_left_first_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Function>
    requires(
        detail::indirectly_binary_left_foldable<
            Function, iter_value_t<Iterator>, Iterator> &&
        constructible_from<iter_value_t<Iterator>, iter_reference_t<Iterator>>)
  constexpr auto operator()(Iterator first, Sentinel last,
                            Function function) const
      -> optional<detail::fold_left_value_t<Function, iter_value_t<Iterator>,
                                            Iterator>> {
    return detail::fold_left_first_with_iter_loop(
               FTL_FOLD_NAMESPACE::move(first), FTL_FOLD_NAMESPACE::move(last),
               function)
        .value;
  }

  template <input_range Range, class Function>
    requires(detail::indirectly_binary_left_foldable<
                 Function, range_value_t<Range>, iterator_t<Range>> &&
             constructible_from<range_value_t<Range>, range_reference_t<Range>>)
  constexpr auto operator()(Range &&range, Function function) const
      -> optional<detail::fold_left_value_t<Function, range_value_t<Range>,
                                            iterator_t<Range>>> {
    return (*this)(ranges::begin(range), ranges::end(range),
                   FTL_FOLD_NAMESPACE::move(function));
  }
};

struct fold_right_fn {
  template <bidirectional_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class T, class Function>
    requires detail::indirectly_binary_right_foldable<Function, T, Iterator>
  constexpr auto operator()(Iterator first, Sentinel last, T init,
                            Function function) const
      -> detail::fold_right_value_t<Function, T, Iterator> {
    return detail::fold_right_loop(FTL_FOLD_NAMESPACE::move(first),
                                   FTL_FOLD_NAMESPACE::move(last),
                                   FTL_FOLD_NAMESPACE::move(init), function);
  }

  template <bidirectional_range Range, class T, class Function>
    requires detail::indirectly_binary_right_foldable<Function, T,
                                                      iterator_t<Range>>
  constexpr auto operator()(Range &&range, T init, Function function) const
      -> detail::fold_right_value_t<Function, T, iterator_t<Range>> {
    return (*this)(ranges::begin(range), ranges::end(range),
                   FTL_FOLD_NAMESPACE::move(init),
                   FTL_FOLD_NAMESPACE::move(function));
  }
};

struct fold_right_last_fn {
  template <bidirectional_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Function>
    requires(
        detail::indirectly_binary_right_foldable<
            Function, iter_value_t<Iterator>, Iterator> &&
        constructible_from<iter_value_t<Iterator>, iter_reference_t<Iterator>>)
  constexpr auto operator()(Iterator first, Sentinel last,
                            Function function) const
      -> optional<detail::fold_right_value_t<Function, iter_value_t<Iterator>,
                                             Iterator>> {
    return detail::fold_right_last_loop(FTL_FOLD_NAMESPACE::move(first),
                                        FTL_FOLD_NAMESPACE::move(last),
                                        function);
  }

  template <bidirectional_range Range, class Function>
    requires(detail::indirectly_binary_right_foldable<
                 Function, range_value_t<Range>, iterator_t<Range>> &&
             constructible_from<range_value_t<Range>, range_reference_t<Range>>)
  constexpr auto operator()(Range &&range, Function function) const
      -> optional<detail::fold_right_value_t<Function, range_value_t<Range>,
                                             iterator_t<Range>>> {
    return (*this)(ranges::begin(range), ranges::end(range),
                   FTL_FOLD_NAMESPACE::move(function));
  }
};

inline constexpr fold_left_fn fold_left{};
inline constexpr fold_left_first_fn fold_left_first{};
inline constexpr fold_right_fn fold_right{};
inline constexpr fold_right_last_fn fold_right_last{};
inline constexpr fold_left_with_iter_fn fold_left_with_iter{};
inline constexpr fold_left_first_with_iter_fn fold_left_first_with_iter{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_FOLD_NAMESPACE

#endif // FTL_FOLD_HEADER
