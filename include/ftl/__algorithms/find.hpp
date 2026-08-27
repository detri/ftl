// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_FIND_HEADER
#define FTL_FIND_HEADER

#include <__execution/policy_access.hpp>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>


namespace std {

namespace detail {

template <class Iterator, class Sentinel, class T>
constexpr void find_loop(Iterator &first, const Sentinel &last,
                         const T &value) {
  for (; first != last; ++first) {
    if (*first == value) {
      return;
    }
  }
}

template <class Iterator, class Sentinel, class Predicate>
constexpr void find_if_loop(Iterator &first, const Sentinel &last,
                            Predicate &predicate) {
  for (; first != last; ++first) {
    if (predicate(*first)) {
      return;
    }
  }
}

template <class Iterator, class Sentinel, class Predicate>
constexpr void find_if_not_loop(Iterator &first, const Sentinel &last,
                                Predicate &predicate) {
  for (; first != last; ++first) {
    if (!predicate(*first)) {
      return;
    }
  }
}

template <class Iterator, class Sentinel, class T, class Projection>
constexpr void ranges_find_loop(Iterator &first, const Sentinel &last,
                                const T &value, Projection &projection) {
  for (; first != last; ++first) {
    if (std::invoke(projection, *first) == value) {
      return;
    }
  }
}

template <class Iterator, class Sentinel, class Predicate, class Projection>
constexpr void ranges_find_if_loop(Iterator &first, const Sentinel &last,
                                   Predicate &predicate,
                                   Projection &projection) {
  for (; first != last; ++first) {
    if (std::invoke(
            predicate, std::invoke(projection, *first))) {
      return;
    }
  }
}

template <class Iterator, class Sentinel, class Predicate, class Projection>
constexpr void ranges_find_if_not_loop(Iterator &first, const Sentinel &last,
                                       Predicate &predicate,
                                       Projection &projection) {
  for (; first != last; ++first) {
    if (!std::invoke(
            predicate, std::invoke(projection, *first))) {
      return;
    }
  }
}

template <class Iterator, class Sentinel, class T, class Projection>
constexpr ranges::subrange<Iterator>
ranges_find_last_loop(Iterator first, Sentinel last, const T &value,
                      Projection &projection) {
  Iterator found = first;
  bool has_found = false;

  for (; first != last; ++first) {
    if (std::invoke(projection, *first) == value) {
      found = first;
      has_found = true;
    }
  }

  if (!has_found) {
    found = first;
  }

  return {std::move(found), std::move(first)};
}

template <class Iterator, class Sentinel, class Predicate, class Projection>
constexpr ranges::subrange<Iterator>
ranges_find_last_if_loop(Iterator first, Sentinel last, Predicate &predicate,
                         Projection &projection) {
  Iterator found = first;
  bool has_found = false;

  for (; first != last; ++first) {
    if (std::invoke(
            predicate, std::invoke(projection, *first))) {
      found = first;
      has_found = true;
    }
  }

  if (!has_found) {
    found = first;
  }

  return {std::move(found), std::move(first)};
}

template <class Iterator, class Sentinel, class Predicate, class Projection>
constexpr ranges::subrange<Iterator>
ranges_find_last_if_not_loop(Iterator first, Sentinel last,
                             Predicate &predicate, Projection &projection) {
  Iterator found = first;
  bool has_found = false;

  for (; first != last; ++first) {
    if (!std::invoke(
            predicate, std::invoke(projection, *first))) {
      found = first;
      has_found = true;
    }
  }

  if (!has_found) {
    found = first;
  }

  return {std::move(found), std::move(first)};
}

} // namespace detail

template <class InputIterator, class T>
constexpr InputIterator find(InputIterator first, InputIterator last,
                             const T &value) {
  detail::find_loop(first, last, value);
  return first;
}

template <class ExecutionPolicy, class ForwardIterator, class T>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator find(ExecutionPolicy &&, ForwardIterator first,
                     ForwardIterator last, const T &value) {
  return [&]() noexcept {
    detail::find_loop(first, last, value);
    return first;
  }();
}

template <class InputIterator, class Predicate>
constexpr InputIterator find_if(InputIterator first, InputIterator last,
                                Predicate predicate) {
  detail::find_if_loop(first, last, predicate);
  return first;
}

template <class ExecutionPolicy, class ForwardIterator, class Predicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator find_if(ExecutionPolicy &&, ForwardIterator first,
                        ForwardIterator last, Predicate predicate) {
  return [&]() noexcept {
    detail::find_if_loop(first, last, predicate);
    return first;
  }();
}

template <class InputIterator, class Predicate>
constexpr InputIterator find_if_not(InputIterator first, InputIterator last,
                                    Predicate predicate) {
  detail::find_if_not_loop(first, last, predicate);
  return first;
}

template <class ExecutionPolicy, class ForwardIterator, class Predicate>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
ForwardIterator find_if_not(ExecutionPolicy &&, ForwardIterator first,
                            ForwardIterator last, Predicate predicate) {
  return [&]() noexcept {
    detail::find_if_not_loop(first, last, predicate);
    return first;
  }();
}

namespace ranges {

struct find_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel, class T,
            class Projection = identity>
    requires indirect_binary_predicate<
        ranges::equal_to, projected<Iterator, Projection>, const T *>
  constexpr Iterator operator()(Iterator first, Sentinel last, const T &value,
                                Projection projection = {}) const {
    std::detail::ranges_find_loop(first, last, value,
                                                 projection);

    return first;
  }

  template <input_range Range, class T, class Projection = identity>
    requires indirect_binary_predicate<
        ranges::equal_to, projected<iterator_t<Range>, Projection>, const T *>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, const T &value, Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range), value,
                          std::move(projection));

    return std::move(result);
  }
};

struct find_if_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                Predicate predicate,
                                Projection projection = {}) const {
    std::detail::ranges_find_if_loop(first, last, predicate,
                                                    projection);

    return first;
  }

  template <input_range Range, class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, Predicate predicate,
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          std::move(predicate),
                          std::move(projection));

    return std::move(result);
  }
};

struct find_if_not_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                Predicate predicate,
                                Projection projection = {}) const {
    std::detail::ranges_find_if_not_loop(first, last, predicate,
                                                        projection);

    return first;
  }

  template <input_range Range, class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, Predicate predicate,
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          std::move(predicate),
                          std::move(projection));

    return std::move(result);
  }
};

inline constexpr find_fn find{};
inline constexpr find_if_fn find_if{};
inline constexpr find_if_not_fn find_if_not{};

struct contains_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel, class T,
            class Projection = identity>
    requires indirect_binary_predicate<
        ranges::equal_to, projected<Iterator, Projection>, const T *>
  constexpr bool operator()(Iterator first, Sentinel last, const T &value,
                            Projection projection = {}) const {
    return ranges::find(std::move(first), last, value,
                        std::move(projection)) != last;
  }

  template <input_range Range, class T, class Projection = identity>
    requires indirect_binary_predicate<
        ranges::equal_to, projected<iterator_t<Range>, Projection>, const T *>
  constexpr bool operator()(Range &&range, const T &value,
                            Projection projection = {}) const {
    return (*this)(ranges::begin(range), ranges::end(range), value,
                   std::move(projection));
  }
};

inline constexpr contains_fn contains{};

struct find_last_fn {
  template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel, class T,
            class Projection = identity>
    requires indirect_binary_predicate<
        ranges::equal_to, projected<Iterator, Projection>, const T *>
  constexpr subrange<Iterator> operator()(Iterator first, Sentinel last,
                                          const T &value,
                                          Projection projection = {}) const {
    return std::detail::ranges_find_last_loop(
        std::move(first), std::move(last), value,
        projection);
  }

  template <forward_range Range, class T, class Projection = identity>
    requires indirect_binary_predicate<
        ranges::equal_to, projected<iterator_t<Range>, Projection>, const T *>
  constexpr borrowed_subrange_t<Range>
  operator()(Range &&range, const T &value, Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range), value,
                          std::move(projection));

    return std::move(result);
  }
};

struct find_last_if_fn {
  template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
  constexpr subrange<Iterator> operator()(Iterator first, Sentinel last,
                                          Predicate predicate,
                                          Projection projection = {}) const {
    return std::detail::ranges_find_last_if_loop(
        std::move(first), std::move(last),
        predicate, projection);
  }

  template <forward_range Range, class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
  constexpr borrowed_subrange_t<Range>
  operator()(Range &&range, Predicate predicate,
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          std::move(predicate),
                          std::move(projection));

    return std::move(result);
  }
};

struct find_last_if_not_fn {
  template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_unary_predicate<projected<Iterator, Projection>> Predicate>
  constexpr subrange<Iterator> operator()(Iterator first, Sentinel last,
                                          Predicate predicate,
                                          Projection projection = {}) const {
    return std::detail::ranges_find_last_if_not_loop(
        std::move(first), std::move(last),
        predicate, projection);
  }

  template <forward_range Range, class Projection = identity,
            indirect_unary_predicate<projected<iterator_t<Range>, Projection>>
                Predicate>
  constexpr borrowed_subrange_t<Range>
  operator()(Range &&range, Predicate predicate,
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          std::move(predicate),
                          std::move(projection));

    return std::move(result);
  }
};

inline constexpr find_last_fn find_last{};
inline constexpr find_last_if_fn find_last_if{};
inline constexpr find_last_if_not_fn find_last_if_not{};

} // namespace ranges

} // namespace std


#endif // FTL_FIND_HEADER
