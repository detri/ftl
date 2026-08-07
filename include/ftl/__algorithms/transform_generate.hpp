// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_TRANSFORM_GENERATE_HEADER
#define FTL_TRANSFORM_GENERATE_HEADER

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
#define FTL_TRANSFORM_GENERATE_NAMESPACE std
#else
#define FTL_TRANSFORM_GENERATE_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace detail {

template <class Iterator, class Sentinel, class Output, class Operation>
constexpr void transform_unary_loop(Iterator &first, const Sentinel &last,
                                    Output &result, Operation &operation) {
  for (; first != last; ++first, ++result) {
    *result = operation(*first);
  }
}

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Output, class Operation>
constexpr void transform_binary_loop(Iterator1 &first1, const Sentinel1 &last1,
                                     Iterator2 &first2, const Sentinel2 &last2,
                                     Output &result, Operation &operation) {
  for (; first1 != last1 && first2 != last2; ++first1, ++first2, ++result) {
    *result = operation(*first1, *first2);
  }
}

template <class Iterator, class Sentinel, class Output, class Operation,
          class Projection>
constexpr void ranges_transform_unary_loop(Iterator &first,
                                           const Sentinel &last, Output &result,
                                           Operation &operation,
                                           Projection &projection) {
  for (; first != last; ++first, ++result) {
    *result = FTL_TRANSFORM_GENERATE_NAMESPACE::invoke(
        operation,
        FTL_TRANSFORM_GENERATE_NAMESPACE::invoke(projection, *first));
  }
}

template <class Iterator1, class Sentinel1, class Iterator2, class Sentinel2,
          class Output, class Operation, class Projection1, class Projection2>
constexpr void ranges_transform_binary_loop(
    Iterator1 &first1, const Sentinel1 &last1, Iterator2 &first2,
    const Sentinel2 &last2, Output &result, Operation &operation,
    Projection1 &projection1, Projection2 &projection2) {
  for (; first1 != last1 && first2 != last2; ++first1, ++first2, ++result) {
    *result = FTL_TRANSFORM_GENERATE_NAMESPACE::invoke(
        operation,
        FTL_TRANSFORM_GENERATE_NAMESPACE::invoke(projection1, *first1),
        FTL_TRANSFORM_GENERATE_NAMESPACE::invoke(projection2, *first2));
  }
}

template <class Output, class Sentinel, class Generator>
constexpr void generate_loop(Output &first, const Sentinel &last,
                             Generator &generator) {
  for (; first != last; ++first) {
    *first = generator();
  }
}

template <class Output, class Count, class Generator>
constexpr void generate_n_loop(Output &first, Count count,
                               Generator &generator) {
  auto remaining = +count;

  for (; remaining > 0; --remaining, ++first) {
    *first = generator();
  }
}

} // namespace detail

template <class InputIterator, class OutputIterator, class UnaryOperation>
constexpr OutputIterator transform(InputIterator first, InputIterator last,
                                   OutputIterator result,
                                   UnaryOperation operation) {
  detail::transform_unary_loop(first, last, result, operation);

  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class UnaryOperation>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator2 transform(ExecutionPolicy &&, ForwardIterator1 first,
                           ForwardIterator1 last, ForwardIterator2 result,
                           UnaryOperation operation) {
  return [&]() noexcept {
    detail::transform_unary_loop(first, last, result, operation);

    return result;
  }();
}

template <class InputIterator1, class InputIterator2, class OutputIterator,
          class BinaryOperation>
constexpr OutputIterator transform(InputIterator1 first1, InputIterator1 last1,
                                   InputIterator2 first2, OutputIterator result,
                                   BinaryOperation operation) {
  /*
   * The classic binary overload has only one ending iterator.
   * first2 is required to designate a sufficiently long range.
   */
  for (; first1 != last1; ++first1, ++first2, ++result) {
    *result = operation(*first1, *first2);
  }

  return result;
}

template <class ExecutionPolicy, class ForwardIterator1, class ForwardIterator2,
          class ForwardIterator, class BinaryOperation>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator transform(ExecutionPolicy &&, ForwardIterator1 first1,
                          ForwardIterator1 last1, ForwardIterator2 first2,
                          ForwardIterator result, BinaryOperation operation) {
  return [&]() noexcept {
    for (; first1 != last1; ++first1, ++first2, ++result) {
      *result = operation(*first1, *first2);
    }

    return result;
  }();
}

template <class ForwardIterator, class Generator>
constexpr void generate(ForwardIterator first, ForwardIterator last,
                        Generator generator) {
  detail::generate_loop(first, last, generator);
}

template <class ExecutionPolicy, class ForwardIterator, class Generator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
void generate(ExecutionPolicy &&, ForwardIterator first, ForwardIterator last,
              Generator generator) {
  [&]() noexcept { detail::generate_loop(first, last, generator); }();
}

template <class OutputIterator, class Size, class Generator>
constexpr OutputIterator generate_n(OutputIterator first, Size count,
                                    Generator generator) {
  detail::generate_n_loop(first, count, generator);
  return first;
}

template <class ExecutionPolicy, class ForwardIterator, class Size,
          class Generator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator generate_n(ExecutionPolicy &&, ForwardIterator first,
                           Size count, Generator generator) {
  return [&]() noexcept {
    detail::generate_n_loop(first, count, generator);
    return first;
  }();
}

namespace ranges {

template <class Iterator, class Output>
using unary_transform_result = in_out_result<Iterator, Output>;

template <class Iterator1, class Iterator2, class Output>
using binary_transform_result = in_in_out_result<Iterator1, Iterator2, Output>;

struct transform_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            weakly_incrementable Output, copy_constructible Function,
            class Projection = identity>
    requires indirectly_writable<
        Output, indirect_result_t<Function &, projected<Iterator, Projection>>>
  constexpr unary_transform_result<Iterator, Output>
  operator()(Iterator first, Sentinel last, Output result, Function operation,
             Projection projection = {}) const {
    FTL_TRANSFORM_GENERATE_NAMESPACE::detail::ranges_transform_unary_loop(
        first, last, result, operation, projection);

    return {
        FTL_TRANSFORM_GENERATE_NAMESPACE::move(first),
        FTL_TRANSFORM_GENERATE_NAMESPACE::move(result),
    };
  }

  template <input_range Range, weakly_incrementable Output,
            copy_constructible Function, class Projection = identity>
    requires indirectly_writable<
        Output,
        indirect_result_t<Function &, projected<iterator_t<Range>, Projection>>>
  constexpr unary_transform_result<borrowed_iterator_t<Range>, Output>
  operator()(Range &&range, Output result, Function operation,
             Projection projection = {}) const {
    auto converted =
        (*this)(ranges::begin(range), ranges::end(range),
                FTL_TRANSFORM_GENERATE_NAMESPACE::move(result),
                FTL_TRANSFORM_GENERATE_NAMESPACE::move(operation),
                FTL_TRANSFORM_GENERATE_NAMESPACE::move(projection));

    return {
        FTL_TRANSFORM_GENERATE_NAMESPACE::move(converted.in),
        FTL_TRANSFORM_GENERATE_NAMESPACE::move(converted.out),
    };
  }

  template <input_iterator Iterator1, sentinel_for<Iterator1> Sentinel1,
            input_iterator Iterator2, sentinel_for<Iterator2> Sentinel2,
            weakly_incrementable Output, copy_constructible Function,
            class Projection1 = identity, class Projection2 = identity>
    requires indirectly_writable<
        Output, indirect_result_t<Function &, projected<Iterator1, Projection1>,
                                  projected<Iterator2, Projection2>>>
  constexpr binary_transform_result<Iterator1, Iterator2, Output>
  operator()(Iterator1 first1, Sentinel1 last1, Iterator2 first2,
             Sentinel2 last2, Output result, Function operation,
             Projection1 projection1 = {}, Projection2 projection2 = {}) const {
    FTL_TRANSFORM_GENERATE_NAMESPACE::detail::ranges_transform_binary_loop(
        first1, last1, first2, last2, result, operation, projection1,
        projection2);

    return {
        FTL_TRANSFORM_GENERATE_NAMESPACE::move(first1),
        FTL_TRANSFORM_GENERATE_NAMESPACE::move(first2),
        FTL_TRANSFORM_GENERATE_NAMESPACE::move(result),
    };
  }

  template <input_range Range1, input_range Range2, weakly_incrementable Output,
            copy_constructible Function, class Projection1 = identity,
            class Projection2 = identity>
    requires indirectly_writable<
        Output, indirect_result_t<Function &,
                                  projected<iterator_t<Range1>, Projection1>,
                                  projected<iterator_t<Range2>, Projection2>>>
  constexpr binary_transform_result<borrowed_iterator_t<Range1>,
                                    borrowed_iterator_t<Range2>, Output>
  operator()(Range1 &&range1, Range2 &&range2, Output result,
             Function operation, Projection1 projection1 = {},
             Projection2 projection2 = {}) const {
    auto converted = (*this)(
        ranges::begin(range1), ranges::end(range1), ranges::begin(range2),
        ranges::end(range2), FTL_TRANSFORM_GENERATE_NAMESPACE::move(result),
        FTL_TRANSFORM_GENERATE_NAMESPACE::move(operation),
        FTL_TRANSFORM_GENERATE_NAMESPACE::move(projection1),
        FTL_TRANSFORM_GENERATE_NAMESPACE::move(projection2));

    return {
        FTL_TRANSFORM_GENERATE_NAMESPACE::move(converted.in1),
        FTL_TRANSFORM_GENERATE_NAMESPACE::move(converted.in2),
        FTL_TRANSFORM_GENERATE_NAMESPACE::move(converted.out),
    };
  }
};

struct generate_fn {
  template <input_or_output_iterator Output, sentinel_for<Output> Sentinel,
            copy_constructible Function>
    requires(invocable<Function &> &&
             indirectly_writable<Output, invoke_result_t<Function &>>)
  constexpr Output operator()(Output first, Sentinel last,
                              Function generator) const {
    FTL_TRANSFORM_GENERATE_NAMESPACE::detail::generate_loop(first, last,
                                                            generator);

    return FTL_TRANSFORM_GENERATE_NAMESPACE::move(first);
  }

  template <class Range, copy_constructible Function>
    requires(invocable<Function &> &&
             output_range<Range, invoke_result_t<Function &>>)
  constexpr borrowed_iterator_t<Range> operator()(Range &&range,
                                                  Function generator) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_TRANSFORM_GENERATE_NAMESPACE::move(generator));

    return FTL_TRANSFORM_GENERATE_NAMESPACE::move(result);
  }
};

struct generate_n_fn {
  template <input_or_output_iterator Output, copy_constructible Function>
    requires(invocable<Function &> &&
             indirectly_writable<Output, invoke_result_t<Function &>>)
  constexpr Output operator()(Output first, iter_difference_t<Output> count,
                              Function generator) const {
    FTL_TRANSFORM_GENERATE_NAMESPACE::detail::generate_n_loop(first, count,
                                                              generator);

    return FTL_TRANSFORM_GENERATE_NAMESPACE::move(first);
  }
};

inline constexpr transform_fn transform{};
inline constexpr generate_fn generate{};
inline constexpr generate_n_fn generate_n{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_TRANSFORM_GENERATE_NAMESPACE

#endif // FTL_TRANSFORM_GENERATE_HEADER