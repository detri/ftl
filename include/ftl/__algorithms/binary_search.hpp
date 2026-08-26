// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_BINARY_SEARCH_HEADER
#define FTL_BINARY_SEARCH_HEADER

#include <functional>
#include <iterator>
#include <ranges>
#include <utility>


namespace std {

namespace detail {

using binary_search_less = std::less<>;

template <class Iterator, class Sentinel, class T, class Comparator,
          class Projection>
constexpr Iterator lower_bound_loop(Iterator first, Sentinel last,
                                    const T &value, Comparator &comparator,
                                    Projection &projection) {
  auto length = ranges::distance(first, last);

  while (length > 0) {
    const auto half = static_cast<iter_difference_t<Iterator>>(length / 2);

    Iterator middle = first;
    ranges::advance(middle, half);

    if (std::invoke(
            comparator,
            std::invoke(projection, *middle), value)) {
      first = middle;
      ++first;

      length = static_cast<iter_difference_t<Iterator>>(length - half - 1);
    } else {
      length = half;
    }
  }

  return first;
}

template <class Iterator, class Sentinel, class T, class Comparator,
          class Projection>
constexpr Iterator upper_bound_loop(Iterator first, Sentinel last,
                                    const T &value, Comparator &comparator,
                                    Projection &projection) {
  auto length = ranges::distance(first, last);

  while (length > 0) {
    const auto half = static_cast<iter_difference_t<Iterator>>(length / 2);

    Iterator middle = first;
    ranges::advance(middle, half);

    if (!std::invoke(
            comparator, value,
            std::invoke(projection, *middle))) {
      first = middle;
      ++first;

      length = static_cast<iter_difference_t<Iterator>>(length - half - 1);
    } else {
      length = half;
    }
  }

  return first;
}

template <class Iterator, class Sentinel, class T, class Comparator,
          class Projection>
constexpr ranges::subrange<Iterator>
equal_range_loop(Iterator first, Sentinel last, const T &value,
                 Comparator &comparator, Projection &projection) {
  Iterator lower = lower_bound_loop(first, last, value, comparator, projection);

  Iterator upper = upper_bound_loop(lower, last, value, comparator, projection);

  return {
      std::move(lower),
      std::move(upper),
  };
}

template <class Iterator, class Sentinel, class T, class Comparator,
          class Projection>
constexpr bool binary_search_loop(Iterator first, Sentinel last, const T &value,
                                  Comparator &comparator,
                                  Projection &projection) {
  Iterator result = lower_bound_loop(std::move(first),
                                     last, value, comparator, projection);

  return result != last &&
         !std::invoke(
             comparator, value,
             std::invoke(projection, *result));
}

} // namespace detail

template <class ForwardIterator, class T>
constexpr ForwardIterator lower_bound(ForwardIterator first,
                                      ForwardIterator last, const T &value) {
  detail::binary_search_less comparator{};
  identity projection{};

  return detail::lower_bound_loop(std::move(first),
                                  std::move(last),
                                  value, comparator, projection);
}

template <class ForwardIterator, class T, class Comparator>
constexpr ForwardIterator lower_bound(ForwardIterator first,
                                      ForwardIterator last, const T &value,
                                      Comparator comparator) {
  identity projection{};

  return detail::lower_bound_loop(std::move(first),
                                  std::move(last),
                                  value, comparator, projection);
}

template <class ForwardIterator, class T>
constexpr ForwardIterator upper_bound(ForwardIterator first,
                                      ForwardIterator last, const T &value) {
  detail::binary_search_less comparator{};
  identity projection{};

  return detail::upper_bound_loop(std::move(first),
                                  std::move(last),
                                  value, comparator, projection);
}

template <class ForwardIterator, class T, class Comparator>
constexpr ForwardIterator upper_bound(ForwardIterator first,
                                      ForwardIterator last, const T &value,
                                      Comparator comparator) {
  identity projection{};

  return detail::upper_bound_loop(std::move(first),
                                  std::move(last),
                                  value, comparator, projection);
}

template <class ForwardIterator, class T>
constexpr pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T &value) {
  detail::binary_search_less comparator{};
  identity projection{};

  auto result = detail::equal_range_loop(
      std::move(first),
      std::move(last), value, comparator, projection);

  return {
      std::move(result.begin()),
      std::move(result.end()),
  };
}

template <class ForwardIterator, class T, class Comparator>
constexpr pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T &value,
            Comparator comparator) {
  identity projection{};

  auto result = detail::equal_range_loop(
      std::move(first),
      std::move(last), value, comparator, projection);

  return {
      std::move(result.begin()),
      std::move(result.end()),
  };
}

template <class ForwardIterator, class T>
constexpr bool binary_search(ForwardIterator first, ForwardIterator last,
                             const T &value) {
  detail::binary_search_less comparator{};
  identity projection{};

  return detail::binary_search_loop(std::move(first),
                                    std::move(last),
                                    value, comparator, projection);
}

template <class ForwardIterator, class T, class Comparator>
constexpr bool binary_search(ForwardIterator first, ForwardIterator last,
                             const T &value, Comparator comparator) {
  identity projection{};

  return detail::binary_search_loop(std::move(first),
                                    std::move(last),
                                    value, comparator, projection);
}

namespace ranges {

struct lower_bound_fn {
  template <
      forward_iterator Iterator, sentinel_for<Iterator> Sentinel, class T,
      class Projection = identity,
      indirect_strict_weak_order<const T *, projected<Iterator, Projection>>
          Comparator = ranges::less>
  constexpr Iterator operator()(Iterator first, Sentinel last, const T &value,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    return std::detail::lower_bound_loop(
        std::move(first),
        std::move(last), value, comparator, projection);
  }

  template <forward_range Range, class T, class Projection = identity,
            indirect_strict_weak_order<const T *,
                                       projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, const T &value, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range), value,
                          std::move(comparator),
                          std::move(projection));

    return std::move(result);
  }
};

struct upper_bound_fn {
  template <
      forward_iterator Iterator, sentinel_for<Iterator> Sentinel, class T,
      class Projection = identity,
      indirect_strict_weak_order<const T *, projected<Iterator, Projection>>
          Comparator = ranges::less>
  constexpr Iterator operator()(Iterator first, Sentinel last, const T &value,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    return std::detail::upper_bound_loop(
        std::move(first),
        std::move(last), value, comparator, projection);
  }

  template <forward_range Range, class T, class Projection = identity,
            indirect_strict_weak_order<const T *,
                                       projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, const T &value, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range), value,
                          std::move(comparator),
                          std::move(projection));

    return std::move(result);
  }
};

struct equal_range_fn {
  template <
      forward_iterator Iterator, sentinel_for<Iterator> Sentinel, class T,
      class Projection = identity,
      indirect_strict_weak_order<const T *, projected<Iterator, Projection>>
          Comparator = ranges::less>
  constexpr subrange<Iterator>
  operator()(Iterator first, Sentinel last, const T &value,
             Comparator comparator = {}, Projection projection = {}) const {
    return std::detail::equal_range_loop(
        std::move(first),
        std::move(last), value, comparator, projection);
  }

  template <forward_range Range, class T, class Projection = identity,
            indirect_strict_weak_order<const T *,
                                       projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr borrowed_subrange_t<Range>
  operator()(Range &&range, const T &value, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range), value,
                          std::move(comparator),
                          std::move(projection));

    return std::move(result);
  }
};

struct binary_search_fn {
  template <
      forward_iterator Iterator, sentinel_for<Iterator> Sentinel, class T,
      class Projection = identity,
      indirect_strict_weak_order<const T *, projected<Iterator, Projection>>
          Comparator = ranges::less>
  constexpr bool operator()(Iterator first, Sentinel last, const T &value,
                            Comparator comparator = {},
                            Projection projection = {}) const {
    return std::detail::binary_search_loop(
        std::move(first),
        std::move(last), value, comparator, projection);
  }

  template <forward_range Range, class T, class Projection = identity,
            indirect_strict_weak_order<const T *,
                                       projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr bool operator()(Range &&range, const T &value,
                            Comparator comparator = {},
                            Projection projection = {}) const {
    return (*this)(ranges::begin(range), ranges::end(range), value,
                   std::move(comparator),
                   std::move(projection));
  }
};

inline constexpr lower_bound_fn lower_bound{};
inline constexpr upper_bound_fn upper_bound{};
inline constexpr equal_range_fn equal_range{};
inline constexpr binary_search_fn binary_search{};

} // namespace ranges

} // namespace std


#endif // FTL_BINARY_SEARCH_HEADER
