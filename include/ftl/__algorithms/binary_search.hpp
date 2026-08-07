// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_BINARY_SEARCH_HEADER
#define FTL_BINARY_SEARCH_HEADER

#ifdef FTL_REPLACE_STL
#include <functional>
#include <iterator>
#include <ranges>
#include <utility>
#else
#include <ftl/functional>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/utility>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_BINARY_SEARCH_NAMESPACE std
#else
#define FTL_BINARY_SEARCH_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace detail {

struct binary_search_less {
  template <class T, class U>
  constexpr bool operator()(T &&left, U &&right) const {
    return static_cast<T &&>(left) < static_cast<U &&>(right);
  }
};

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

    if (FTL_BINARY_SEARCH_NAMESPACE::invoke(
            comparator,
            FTL_BINARY_SEARCH_NAMESPACE::invoke(projection, *middle), value)) {
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

    if (!FTL_BINARY_SEARCH_NAMESPACE::invoke(
            comparator, value,
            FTL_BINARY_SEARCH_NAMESPACE::invoke(projection, *middle))) {
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
      FTL_BINARY_SEARCH_NAMESPACE::move(lower),
      FTL_BINARY_SEARCH_NAMESPACE::move(upper),
  };
}

template <class Iterator, class Sentinel, class T, class Comparator,
          class Projection>
constexpr bool binary_search_loop(Iterator first, Sentinel last, const T &value,
                                  Comparator &comparator,
                                  Projection &projection) {
  Iterator result = lower_bound_loop(FTL_BINARY_SEARCH_NAMESPACE::move(first),
                                     last, value, comparator, projection);

  return result != last &&
         !FTL_BINARY_SEARCH_NAMESPACE::invoke(
             comparator, value,
             FTL_BINARY_SEARCH_NAMESPACE::invoke(projection, *result));
}

} // namespace detail

template <class ForwardIterator, class T>
constexpr ForwardIterator lower_bound(ForwardIterator first,
                                      ForwardIterator last, const T &value) {
  detail::binary_search_less comparator{};
  identity projection{};

  return detail::lower_bound_loop(FTL_BINARY_SEARCH_NAMESPACE::move(first),
                                  FTL_BINARY_SEARCH_NAMESPACE::move(last),
                                  value, comparator, projection);
}

template <class ForwardIterator, class T, class Comparator>
constexpr ForwardIterator lower_bound(ForwardIterator first,
                                      ForwardIterator last, const T &value,
                                      Comparator comparator) {
  identity projection{};

  return detail::lower_bound_loop(FTL_BINARY_SEARCH_NAMESPACE::move(first),
                                  FTL_BINARY_SEARCH_NAMESPACE::move(last),
                                  value, comparator, projection);
}

template <class ForwardIterator, class T>
constexpr ForwardIterator upper_bound(ForwardIterator first,
                                      ForwardIterator last, const T &value) {
  detail::binary_search_less comparator{};
  identity projection{};

  return detail::upper_bound_loop(FTL_BINARY_SEARCH_NAMESPACE::move(first),
                                  FTL_BINARY_SEARCH_NAMESPACE::move(last),
                                  value, comparator, projection);
}

template <class ForwardIterator, class T, class Comparator>
constexpr ForwardIterator upper_bound(ForwardIterator first,
                                      ForwardIterator last, const T &value,
                                      Comparator comparator) {
  identity projection{};

  return detail::upper_bound_loop(FTL_BINARY_SEARCH_NAMESPACE::move(first),
                                  FTL_BINARY_SEARCH_NAMESPACE::move(last),
                                  value, comparator, projection);
}

template <class ForwardIterator, class T>
constexpr pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T &value) {
  detail::binary_search_less comparator{};
  identity projection{};

  auto result = detail::equal_range_loop(
      FTL_BINARY_SEARCH_NAMESPACE::move(first),
      FTL_BINARY_SEARCH_NAMESPACE::move(last), value, comparator, projection);

  return {
      FTL_BINARY_SEARCH_NAMESPACE::move(result.begin()),
      FTL_BINARY_SEARCH_NAMESPACE::move(result.end()),
  };
}

template <class ForwardIterator, class T, class Comparator>
constexpr pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T &value,
            Comparator comparator) {
  identity projection{};

  auto result = detail::equal_range_loop(
      FTL_BINARY_SEARCH_NAMESPACE::move(first),
      FTL_BINARY_SEARCH_NAMESPACE::move(last), value, comparator, projection);

  return {
      FTL_BINARY_SEARCH_NAMESPACE::move(result.begin()),
      FTL_BINARY_SEARCH_NAMESPACE::move(result.end()),
  };
}

template <class ForwardIterator, class T>
constexpr bool binary_search(ForwardIterator first, ForwardIterator last,
                             const T &value) {
  detail::binary_search_less comparator{};
  identity projection{};

  return detail::binary_search_loop(FTL_BINARY_SEARCH_NAMESPACE::move(first),
                                    FTL_BINARY_SEARCH_NAMESPACE::move(last),
                                    value, comparator, projection);
}

template <class ForwardIterator, class T, class Comparator>
constexpr bool binary_search(ForwardIterator first, ForwardIterator last,
                             const T &value, Comparator comparator) {
  identity projection{};

  return detail::binary_search_loop(FTL_BINARY_SEARCH_NAMESPACE::move(first),
                                    FTL_BINARY_SEARCH_NAMESPACE::move(last),
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
    return FTL_BINARY_SEARCH_NAMESPACE::detail::lower_bound_loop(
        FTL_BINARY_SEARCH_NAMESPACE::move(first),
        FTL_BINARY_SEARCH_NAMESPACE::move(last), value, comparator, projection);
  }

  template <forward_range Range, class T, class Projection = identity,
            indirect_strict_weak_order<const T *,
                                       projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, const T &value, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range), value,
                          FTL_BINARY_SEARCH_NAMESPACE::move(comparator),
                          FTL_BINARY_SEARCH_NAMESPACE::move(projection));

    return FTL_BINARY_SEARCH_NAMESPACE::move(result);
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
    return FTL_BINARY_SEARCH_NAMESPACE::detail::upper_bound_loop(
        FTL_BINARY_SEARCH_NAMESPACE::move(first),
        FTL_BINARY_SEARCH_NAMESPACE::move(last), value, comparator, projection);
  }

  template <forward_range Range, class T, class Projection = identity,
            indirect_strict_weak_order<const T *,
                                       projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, const T &value, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range), value,
                          FTL_BINARY_SEARCH_NAMESPACE::move(comparator),
                          FTL_BINARY_SEARCH_NAMESPACE::move(projection));

    return FTL_BINARY_SEARCH_NAMESPACE::move(result);
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
    return FTL_BINARY_SEARCH_NAMESPACE::detail::equal_range_loop(
        FTL_BINARY_SEARCH_NAMESPACE::move(first),
        FTL_BINARY_SEARCH_NAMESPACE::move(last), value, comparator, projection);
  }

  template <forward_range Range, class T, class Projection = identity,
            indirect_strict_weak_order<const T *,
                                       projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr borrowed_subrange_t<Range>
  operator()(Range &&range, const T &value, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range), value,
                          FTL_BINARY_SEARCH_NAMESPACE::move(comparator),
                          FTL_BINARY_SEARCH_NAMESPACE::move(projection));

    return FTL_BINARY_SEARCH_NAMESPACE::move(result);
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
    return FTL_BINARY_SEARCH_NAMESPACE::detail::binary_search_loop(
        FTL_BINARY_SEARCH_NAMESPACE::move(first),
        FTL_BINARY_SEARCH_NAMESPACE::move(last), value, comparator, projection);
  }

  template <forward_range Range, class T, class Projection = identity,
            indirect_strict_weak_order<const T *,
                                       projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr bool operator()(Range &&range, const T &value,
                            Comparator comparator = {},
                            Projection projection = {}) const {
    return (*this)(ranges::begin(range), ranges::end(range), value,
                   FTL_BINARY_SEARCH_NAMESPACE::move(comparator),
                   FTL_BINARY_SEARCH_NAMESPACE::move(projection));
  }
};

inline constexpr lower_bound_fn lower_bound{};
inline constexpr upper_bound_fn upper_bound{};
inline constexpr equal_range_fn equal_range{};
inline constexpr binary_search_fn binary_search{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_BINARY_SEARCH_NAMESPACE

#endif // FTL_BINARY_SEARCH_HEADER
