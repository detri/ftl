// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_HEAP_HEADER
#define FTL_HEAP_HEADER

#ifdef FTL_REPLACE_STL
#include <__execution/policy_access.hpp>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#else
#include <ftl/__execution/policy_access.hpp>
#include <ftl/functional>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_HEAP_NAMESPACE std
#else
#define FTL_HEAP_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace detail {

struct heap_less {
  template <class T, class U>
  constexpr bool operator()(T &&left, U &&right) const {
    return static_cast<T &&>(left) < static_cast<U &&>(right);
  }
};

template <class Comparator, class Projection, class T, class U>
constexpr bool heap_before(Comparator &comparator, Projection &projection,
                           T &&left, U &&right) {
  return FTL_HEAP_NAMESPACE::invoke(
      comparator,
      FTL_HEAP_NAMESPACE::invoke(projection, static_cast<T &&>(left)),
      FTL_HEAP_NAMESPACE::invoke(projection, static_cast<U &&>(right)));
}

template <class Iterator, class Comparator, class Projection>
constexpr void heap_sift_up(Iterator first, iter_difference_t<Iterator> length,
                            Comparator &comparator, Projection &projection) {
  if (length < 2) {
    return;
  }

  auto child = static_cast<iter_difference_t<Iterator>>(length - 1);

  while (child > 0) {
    const auto parent =
        static_cast<iter_difference_t<Iterator>>((child - 1) / 2);

    if (!heap_before(comparator, projection, *(first + parent),
                     *(first + child))) {
      return;
    }

    ranges::iter_swap(first + parent, first + child);

    child = parent;
  }
}

template <class Iterator, class Comparator, class Projection>
constexpr void heap_sift_down(Iterator first, iter_difference_t<Iterator> root,
                              iter_difference_t<Iterator> length,
                              Comparator &comparator, Projection &projection) {
  while (root < length / 2) {
    auto child = static_cast<iter_difference_t<Iterator>>(root * 2 + 1);

    if (child + 1 < length &&
        heap_before(comparator, projection, *(first + child),
                    *(first + child + 1))) {
      ++child;
    }

    if (!heap_before(comparator, projection, *(first + root),
                     *(first + child))) {
      return;
    }

    ranges::iter_swap(first + root, first + child);

    root = child;
  }
}

template <class Iterator, class Comparator, class Projection>
constexpr void push_heap_loop(Iterator first, Iterator last,
                              Comparator &comparator, Projection &projection) {
  heap_sift_up(first, last - first, comparator, projection);
}

template <class Iterator, class Comparator, class Projection>
constexpr void pop_heap_loop(Iterator first, Iterator last,
                             Comparator &comparator, Projection &projection) {
  const auto length = last - first;

  if (length < 2) {
    return;
  }

  Iterator final = last;
  --final;

  ranges::iter_swap(first, final);

  heap_sift_down(first, 0, static_cast<iter_difference_t<Iterator>>(length - 1),
                 comparator, projection);
}

template <class Iterator, class Comparator, class Projection>
constexpr void make_heap_loop(Iterator first, Iterator last,
                              Comparator &comparator, Projection &projection) {
  const auto length = last - first;

  if (length < 2) {
    return;
  }

  auto parent = static_cast<iter_difference_t<Iterator>>(length / 2);

  while (parent > 0) {
    --parent;

    heap_sift_down(first, parent, length, comparator, projection);
  }
}

template <class Iterator, class Comparator, class Projection>
constexpr void sort_heap_loop(Iterator first, Iterator last,
                              Comparator &comparator, Projection &projection) {
  while (last - first > 1) {
    pop_heap_loop(first, last, comparator, projection);

    --last;
  }
}

template <class Iterator, class Comparator, class Projection>
constexpr Iterator is_heap_until_loop(Iterator first, Iterator last,
                                      Comparator &comparator,
                                      Projection &projection) {
  const auto length = last - first;

  for (iter_difference_t<Iterator> child = 1; child < length; ++child) {
    const auto parent =
        static_cast<iter_difference_t<Iterator>>((child - 1) / 2);

    if (heap_before(comparator, projection, *(first + parent),
                    *(first + child))) {
      return first + child;
    }
  }

  return last;
}

template <class Iterator, class Sentinel>
constexpr Iterator heap_end(Iterator first, Sentinel last) {
  Iterator end = first;
  ranges::advance(end, last);
  return end;
}

} // namespace detail

template <class RandomAccessIterator>
constexpr void push_heap(RandomAccessIterator first,
                         RandomAccessIterator last) {
  detail::heap_less comparator{};
  identity projection{};

  detail::push_heap_loop(FTL_HEAP_NAMESPACE::move(first),
                         FTL_HEAP_NAMESPACE::move(last), comparator,
                         projection);
}

template <class RandomAccessIterator, class Comparator>
constexpr void push_heap(RandomAccessIterator first, RandomAccessIterator last,
                         Comparator comparator) {
  identity projection{};

  detail::push_heap_loop(FTL_HEAP_NAMESPACE::move(first),
                         FTL_HEAP_NAMESPACE::move(last), comparator,
                         projection);
}

template <class RandomAccessIterator>
constexpr void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
  detail::heap_less comparator{};
  identity projection{};

  detail::pop_heap_loop(FTL_HEAP_NAMESPACE::move(first),
                        FTL_HEAP_NAMESPACE::move(last), comparator, projection);
}

template <class RandomAccessIterator, class Comparator>
constexpr void pop_heap(RandomAccessIterator first, RandomAccessIterator last,
                        Comparator comparator) {
  identity projection{};

  detail::pop_heap_loop(FTL_HEAP_NAMESPACE::move(first),
                        FTL_HEAP_NAMESPACE::move(last), comparator, projection);
}

template <class RandomAccessIterator>
constexpr void make_heap(RandomAccessIterator first,
                         RandomAccessIterator last) {
  detail::heap_less comparator{};
  identity projection{};

  detail::make_heap_loop(FTL_HEAP_NAMESPACE::move(first),
                         FTL_HEAP_NAMESPACE::move(last), comparator,
                         projection);
}

template <class RandomAccessIterator, class Comparator>
constexpr void make_heap(RandomAccessIterator first, RandomAccessIterator last,
                         Comparator comparator) {
  identity projection{};

  detail::make_heap_loop(FTL_HEAP_NAMESPACE::move(first),
                         FTL_HEAP_NAMESPACE::move(last), comparator,
                         projection);
}

template <class RandomAccessIterator>
constexpr void sort_heap(RandomAccessIterator first,
                         RandomAccessIterator last) {
  detail::heap_less comparator{};
  identity projection{};

  detail::sort_heap_loop(FTL_HEAP_NAMESPACE::move(first),
                         FTL_HEAP_NAMESPACE::move(last), comparator,
                         projection);
}

template <class RandomAccessIterator, class Comparator>
constexpr void sort_heap(RandomAccessIterator first, RandomAccessIterator last,
                         Comparator comparator) {
  identity projection{};

  detail::sort_heap_loop(FTL_HEAP_NAMESPACE::move(first),
                         FTL_HEAP_NAMESPACE::move(last), comparator,
                         projection);
}

template <class RandomAccessIterator>
constexpr RandomAccessIterator is_heap_until(RandomAccessIterator first,
                                             RandomAccessIterator last) {
  detail::heap_less comparator{};
  identity projection{};

  return detail::is_heap_until_loop(FTL_HEAP_NAMESPACE::move(first),
                                    FTL_HEAP_NAMESPACE::move(last), comparator,
                                    projection);
}

template <class RandomAccessIterator, class Comparator>
constexpr RandomAccessIterator is_heap_until(RandomAccessIterator first,
                                             RandomAccessIterator last,
                                             Comparator comparator) {
  identity projection{};

  return detail::is_heap_until_loop(FTL_HEAP_NAMESPACE::move(first),
                                    FTL_HEAP_NAMESPACE::move(last), comparator,
                                    projection);
}

template <class ExecutionPolicy, class RandomAccessIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
RandomAccessIterator is_heap_until(ExecutionPolicy &&,
                                   RandomAccessIterator first,
                                   RandomAccessIterator last) {
  return [&]() noexcept {
    detail::heap_less comparator{};
    identity projection{};

    return detail::is_heap_until_loop(FTL_HEAP_NAMESPACE::move(first),
                                      FTL_HEAP_NAMESPACE::move(last),
                                      comparator, projection);
  }();
}

template <class ExecutionPolicy, class RandomAccessIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
RandomAccessIterator
is_heap_until(ExecutionPolicy &&, RandomAccessIterator first,
              RandomAccessIterator last, Comparator comparator) {
  return [&]() noexcept {
    identity projection{};

    return detail::is_heap_until_loop(FTL_HEAP_NAMESPACE::move(first),
                                      FTL_HEAP_NAMESPACE::move(last),
                                      comparator, projection);
  }();
}

template <class RandomAccessIterator>
constexpr bool is_heap(RandomAccessIterator first, RandomAccessIterator last) {
  return FTL_HEAP_NAMESPACE::is_heap_until(first, last) == last;
}

template <class RandomAccessIterator, class Comparator>
constexpr bool is_heap(RandomAccessIterator first, RandomAccessIterator last,
                       Comparator comparator) {
  return FTL_HEAP_NAMESPACE::is_heap_until(
             first, last, FTL_HEAP_NAMESPACE::move(comparator)) == last;
}

template <class ExecutionPolicy, class RandomAccessIterator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool is_heap(ExecutionPolicy &&policy, RandomAccessIterator first,
             RandomAccessIterator last) {
  return FTL_HEAP_NAMESPACE::is_heap_until(
             FTL_HEAP_NAMESPACE::forward<ExecutionPolicy>(policy), first,
             last) == last;
}

template <class ExecutionPolicy, class RandomAccessIterator, class Comparator>
  requires is_execution_policy_v<remove_cvref_t<ExecutionPolicy>>
bool is_heap(ExecutionPolicy &&policy, RandomAccessIterator first,
             RandomAccessIterator last, Comparator comparator) {
  return FTL_HEAP_NAMESPACE::is_heap_until(
             FTL_HEAP_NAMESPACE::forward<ExecutionPolicy>(policy), first, last,
             FTL_HEAP_NAMESPACE::move(comparator)) == last;
}

namespace ranges {

struct push_heap_fn {
  template <random_access_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Comparator = ranges::less, class Projection = identity>
    requires sortable<Iterator, Comparator, Projection>
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    Iterator end = FTL_HEAP_NAMESPACE::detail::heap_end(
        first, FTL_HEAP_NAMESPACE::move(last));

    FTL_HEAP_NAMESPACE::detail::push_heap_loop(first, end, comparator,
                                               projection);

    return end;
  }

  template <random_access_range Range, class Comparator = ranges::less,
            class Projection = identity>
    requires sortable<iterator_t<Range>, Comparator, Projection>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_HEAP_NAMESPACE::move(comparator),
                          FTL_HEAP_NAMESPACE::move(projection));

    return FTL_HEAP_NAMESPACE::move(result);
  }
};

struct pop_heap_fn {
  template <random_access_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Comparator = ranges::less, class Projection = identity>
    requires sortable<Iterator, Comparator, Projection>
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    Iterator end = FTL_HEAP_NAMESPACE::detail::heap_end(
        first, FTL_HEAP_NAMESPACE::move(last));

    FTL_HEAP_NAMESPACE::detail::pop_heap_loop(first, end, comparator,
                                              projection);

    return end;
  }

  template <random_access_range Range, class Comparator = ranges::less,
            class Projection = identity>
    requires sortable<iterator_t<Range>, Comparator, Projection>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_HEAP_NAMESPACE::move(comparator),
                          FTL_HEAP_NAMESPACE::move(projection));

    return FTL_HEAP_NAMESPACE::move(result);
  }
};

struct make_heap_fn {
  template <random_access_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Comparator = ranges::less, class Projection = identity>
    requires sortable<Iterator, Comparator, Projection>
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    Iterator end = FTL_HEAP_NAMESPACE::detail::heap_end(
        first, FTL_HEAP_NAMESPACE::move(last));

    FTL_HEAP_NAMESPACE::detail::make_heap_loop(first, end, comparator,
                                               projection);

    return end;
  }

  template <random_access_range Range, class Comparator = ranges::less,
            class Projection = identity>
    requires sortable<iterator_t<Range>, Comparator, Projection>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_HEAP_NAMESPACE::move(comparator),
                          FTL_HEAP_NAMESPACE::move(projection));

    return FTL_HEAP_NAMESPACE::move(result);
  }
};

struct sort_heap_fn {
  template <random_access_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Comparator = ranges::less, class Projection = identity>
    requires sortable<Iterator, Comparator, Projection>
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    Iterator end = FTL_HEAP_NAMESPACE::detail::heap_end(
        first, FTL_HEAP_NAMESPACE::move(last));

    FTL_HEAP_NAMESPACE::detail::sort_heap_loop(first, end, comparator,
                                               projection);

    return end;
  }

  template <random_access_range Range, class Comparator = ranges::less,
            class Projection = identity>
    requires sortable<iterator_t<Range>, Comparator, Projection>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_HEAP_NAMESPACE::move(comparator),
                          FTL_HEAP_NAMESPACE::move(projection));

    return FTL_HEAP_NAMESPACE::move(result);
  }
};

struct is_heap_until_fn {
  template <random_access_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_strict_weak_order<projected<Iterator, Projection>>
                Comparator = ranges::less>
  constexpr Iterator operator()(Iterator first, Sentinel last,
                                Comparator comparator = {},
                                Projection projection = {}) const {
    Iterator end = FTL_HEAP_NAMESPACE::detail::heap_end(
        first, FTL_HEAP_NAMESPACE::move(last));

    return FTL_HEAP_NAMESPACE::detail::is_heap_until_loop(
        FTL_HEAP_NAMESPACE::move(first), FTL_HEAP_NAMESPACE::move(end),
        comparator, projection);
  }

  template <random_access_range Range, class Projection = identity,
            indirect_strict_weak_order<projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr borrowed_iterator_t<Range>
  operator()(Range &&range, Comparator comparator = {},
             Projection projection = {}) const {
    auto result = (*this)(ranges::begin(range), ranges::end(range),
                          FTL_HEAP_NAMESPACE::move(comparator),
                          FTL_HEAP_NAMESPACE::move(projection));

    return FTL_HEAP_NAMESPACE::move(result);
  }
};

inline constexpr push_heap_fn push_heap{};
inline constexpr pop_heap_fn pop_heap{};
inline constexpr make_heap_fn make_heap{};
inline constexpr sort_heap_fn sort_heap{};
inline constexpr is_heap_until_fn is_heap_until{};

struct is_heap_fn {
  template <random_access_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Projection = identity,
            indirect_strict_weak_order<projected<Iterator, Projection>>
                Comparator = ranges::less>
  constexpr bool operator()(Iterator first, Sentinel last,
                            Comparator comparator = {},
                            Projection projection = {}) const {
    return ranges::is_heap_until(FTL_HEAP_NAMESPACE::move(first), last,
                                 FTL_HEAP_NAMESPACE::move(comparator),
                                 FTL_HEAP_NAMESPACE::move(projection)) == last;
  }

  template <random_access_range Range, class Projection = identity,
            indirect_strict_weak_order<projected<iterator_t<Range>, Projection>>
                Comparator = ranges::less>
  constexpr bool operator()(Range &&range, Comparator comparator = {},
                            Projection projection = {}) const {
    return (*this)(ranges::begin(range), ranges::end(range),
                   FTL_HEAP_NAMESPACE::move(comparator),
                   FTL_HEAP_NAMESPACE::move(projection));
  }
};

inline constexpr is_heap_fn is_heap{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_HEAP_NAMESPACE

#endif // FTL_HEAP_HEADER
