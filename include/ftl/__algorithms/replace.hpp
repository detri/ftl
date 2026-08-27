// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_REPLACE_HEADER
#define FTL_REPLACE_HEADER

#include <__algorithms/result.hpp>
#include <__execution/policy_access.hpp>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>


namespace std {

namespace detail {

template <class Iterator, class Sentinel, class T>
constexpr void replace_loop(Iterator &first, const Sentinel &last,
                            const T &old_value, const T &new_value) {
  for (; first != last; ++first) {
    if (*first == old_value) {
      *first = new_value;
    }
  }
}

template <class Iterator, class Sentinel, class Predicate, class T>
constexpr void replace_if_loop(Iterator &first, const Sentinel &last,
                               Predicate &predicate, const T &new_value) {
  for (; first != last; ++first) {
    if (predicate(*first)) {
      *first = new_value;
    }
  }
}

template <class Iterator, class Sentinel, class T1, class T2, class Projection>
constexpr void ranges_replace_loop(Iterator &first, const Sentinel &last,
                                   const T1 &old_value, const T2 &new_value,
                                   Projection &projection) {
  ranges::equal_to equal{};

  for (; first != last; ++first) {
    if (std::invoke(
            equal, std::invoke(projection, *first),
            old_value)) {
      *first = new_value;
    }
  }
}

template <class Iterator, class Sentinel, class Predicate, class T,
          class Projection>
constexpr void ranges_replace_if_loop(Iterator &first, const Sentinel &last,
                                      Predicate &predicate, const T &new_value,
                                      Projection &projection) {
  for (; first != last; ++first) {
    if (std::invoke(
            predicate, std::invoke(projection, *first))) {
      *first = new_value;
    }
  }
}

template <class Iterator, class Sentinel, class Output, class T>
constexpr void replace_copy_loop(Iterator &first, const Sentinel &last,
                                 Output &result, const T &old_value,
                                 const T &new_value) {
  for (; first != last; ++first, ++result) {
    if (*first == old_value) {
      *result = new_value;
    } else {
      *result = *first;
    }
  }
}

template <class Iterator, class Sentinel, class Output, class Predicate,
          class T>
constexpr void replace_copy_if_loop(Iterator &first, const Sentinel &last,
                                    Output &result, Predicate &predicate,
                                    const T &new_value) {
  for (; first != last; ++first, ++result) {
    if (predicate(*first)) {
      *result = new_value;
    } else {
      *result = *first;
    }
  }
}

template <class Iterator, class Sentinel, class Output, class T1, class T2,
          class Projection>
constexpr void ranges_replace_copy_loop(Iterator &first, const Sentinel &last,
                                        Output &result, const T1 &old_value,
                                        const T2 &new_value,
                                        Projection &projection) {
  ranges::equal_to equal{};

  for (; first != last; ++first, ++result) {
    if (std::invoke(
            equal, std::invoke(projection, *first),
            old_value)) {
      *result = new_value;
    } else {
      *result = *first;
    }
  }
}

template <class Iterator, class Sentinel, class Output, class Predicate,
          class T, class Projection>
constexpr void
ranges_replace_copy_if_loop(Iterator &first, const Sentinel &last,
                            Output &result, Predicate &predicate,
                            const T &new_value, Projection &projection) {
  for (; first != last; ++first, ++result) {
    if (std::invoke(
            predicate, std::invoke(projection, *first))) {
      *result = new_value;
    } else {
      *result = *first;
    }
  }
}

} // namespace detail

template <class ForwardIterator, class T>
constexpr void replace(ForwardIterator first, ForwardIterator last,
                       const T &old_value, const T &new_value) {
  detail::replace_loop(first, last, old_value, new_value);
}

template <class ExecutionPolicy, class ForwardIterator, class T>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void replace(ExecutionPolicy &&, ForwardIterator first, ForwardIterator last,
             const T &old_value, const T &new_value) {
  [&]() noexcept { detail::replace_loop(first, last, old_value, new_value); }();
}

template <class ForwardIterator, class Predicate, class T>
constexpr void replace_if(ForwardIterator first, ForwardIterator last,
                          Predicate predicate, const T &new_value) {
  detail::replace_if_loop(first, last, predicate, new_value);
}

template <class ExecutionPolicy, class ForwardIterator, class Predicate,
          class T>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void replace_if(ExecutionPolicy &&, ForwardIterator first, ForwardIterator last,
                Predicate predicate, const T &new_value) {
  [&]() noexcept {
    detail::replace_if_loop(first, last, predicate, new_value);
  }();
}

template <class InputIterator, class OutputIterator, class T>
constexpr OutputIterator replace_copy(InputIterator first, InputIterator last,
                                      OutputIterator result, const T &old_value,
                                      const T &new_value) {
  detail::replace_copy_loop(first, last, result, old_value, new_value);

  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class T>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator2 replace_copy(ExecutionPolicy &&, ForwardIterator1 first,
                              ForwardIterator1 last, ForwardIterator2 result,
                              const T &old_value, const T &new_value) {
  return [&]() noexcept {
    detail::replace_copy_loop(first, last, result, old_value, new_value);

    return result;
  }();
}

template <class InputIterator, class OutputIterator, class Predicate, class T>
constexpr OutputIterator
replace_copy_if(InputIterator first, InputIterator last, OutputIterator result,
                Predicate predicate, const T &new_value) {
  detail::replace_copy_if_loop(first, last, result, predicate, new_value);

  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class Predicate, class T>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator2 replace_copy_if(ExecutionPolicy &&, ForwardIterator1 first,
                                 ForwardIterator1 last, ForwardIterator2 result,
                                 Predicate predicate, const T &new_value) {
  return [&]() noexcept {
    detail::replace_copy_if_loop(first, last, result, predicate, new_value);

    return result;
  }();
}

namespace ranges {

template <class Iterator, class Output>
using replace_copy_result = in_out_result<Iterator, Output>;

template <class Iterator, class Output>
using replace_copy_if_result = in_out_result<Iterator, Output>;

struct replace_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel, class T1,
            class T2, class Projection = identity>
    requires(indirectly_writable<Iterator, const T2 &> &&
             indirect_binary_predicate<
                 ranges::equal_to, projected<Iterator, Projection>, const T1 *>)
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                const T1 &old_value, const T2 &new_value,
                                Projection projection = {}) const {
    std::detail::ranges_replace_loop(first, last, old_value,
                                                       new_value, projection);

    return std::move(first);
  }

  template <input_range Range, class T1, class T2, class Projection = identity>
    requires(indirectly_writable<iterator_t<Range>, const T2 &> &&
             indirect_binary_predicate<ranges::equal_to,
                                       projected<iterator_t<Range>, Projection>,
                                       const T1 *>)
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, const T1 &old_value, const T2 &new_value,
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range), old_value,
                          new_value, std::move(projection));

    return std::move(result);
  }
};

struct replace_if_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel, class T,
            class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
    requires indirectly_writable<Iterator, const T &>
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                Predicate predicate, const T &new_value,
                                Projection projection = {}) const {
    std::detail::ranges_replace_if_loop(
        first, last, predicate, new_value, projection);

    return std::move(first);
  }

  template <input_range Range, class T, class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
    requires indirectly_writable<iterator_t<Range>, const T &>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, Predicate predicate, const T &new_value,
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          std::move(predicate), new_value,
                          std::move(projection));

    return std::move(result);
  }
};

struct replace_copy_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel, class T1,
            class T2, output_iterator<const T2 &> Output,
            class Projection = identity>
    requires(indirectly_copyable<Iterator, Output> &&
             indirect_binary_predicate<
                 ranges::equal_to, projected<Iterator, Projection>, const T1 *>)
  constexpr replace_copy_result<Iterator, Output>
  operator()(Iterator first, Sentinel last, Output result, const T1 &old_value,
             const T2 &new_value, Projection projection = {}) const {
    std::detail::ranges_replace_copy_loop(
        first, last, result, old_value, new_value, projection);

    return {
        std::move(first),
        std::move(result),
    };
  }

  template <input_range Range, class T1, class T2,
            output_iterator<const T2 &> Output, class Projection = identity>
    requires(indirectly_copyable<iterator_t<Range>, Output> &&
             indirect_binary_predicate<ranges::equal_to,
                                       projected<iterator_t<Range>, Projection>,
                                       const T1 *>)
  constexpr replace_copy_result<borrowed_iterator_t<Range>, Output>
  operator()(Range &&range, Output result, const T1 &old_value,
             const T2 &new_value, Projection projection = {}) const {
    auto converted =
        (*this)(ranges::begin(range), ranges::end(range),
                std::move(result), old_value, new_value,
                std::move(projection));

    return {
        std::move(converted.in),
        std::move(converted.out),
    };
  }
};

struct replace_copy_if_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel, class T,
            output_iterator<const T &> Output, class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
    requires indirectly_copyable<Iterator, Output>
  constexpr replace_copy_if_result<Iterator, Output>
  operator()(Iterator first, Sentinel last, Output result, Predicate predicate,
             const T &new_value, Projection projection = {}) const {
    std::detail::ranges_replace_copy_if_loop(
        first, last, result, predicate, new_value, projection);

    return {
        std::move(first),
        std::move(result),
    };
  }

  template <input_range Range, class T, output_iterator<const T &> Output,
            class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
    requires indirectly_copyable<iterator_t<Range>, Output>
  constexpr replace_copy_if_result<borrowed_iterator_t<Range>, Output>
  operator()(Range &&range, Output result, Predicate predicate,
             const T &new_value, Projection projection = {}) const {
    auto converted = (*this)(ranges::begin(range), ranges::end(range),
                             std::move(result),
                             std::move(predicate), new_value,
                             std::move(projection));

    return {
        std::move(converted.in),
        std::move(converted.out),
    };
  }
};

inline constexpr replace_fn replace{};
inline constexpr replace_if_fn replace_if{};
inline constexpr replace_copy_fn replace_copy{};
inline constexpr replace_copy_if_fn replace_copy_if{};

} // namespace ranges

} // namespace std


#endif // FTL_REPLACE_HEADER
