// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_ALGORITHM_RANDOM_HEADER
#define FTL_ALGORITHM_RANDOM_HEADER

#ifdef FTL_REPLACE_STL
#include <concepts>
#include <iterator>
#include <limits>
#include <ranges>
#include <type_traits>
#include <utility>
#else
#include <ftl/concepts>
#include <ftl/iterator>
#include <ftl/limits>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#endif

#ifdef FTL_REPLACE_STL
#define FTL_ALGORITHM_RANDOM_NAMESPACE std
#else
#define FTL_ALGORITHM_RANDOM_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

namespace detail {

template <class Generator>
concept algorithm_uniform_random_bit_generator =
    invocable<Generator &> && unsigned_integral<invoke_result_t<Generator &>> &&
    requires {
      { Generator::min() } -> same_as<invoke_result_t<Generator &>>;

      { Generator::max() } -> same_as<invoke_result_t<Generator &>>;

      requires bool_constant<(Generator::min() < Generator::max())>::value;
    };

template <unsigned_integral UInt, class Generator>
constexpr UInt algorithm_random_bounded(UInt bound, Generator &generator) {
  using generator_type = remove_reference_t<Generator>;

  using generator_result = invoke_result_t<generator_type &>;

  using generator_unsigned = make_unsigned_t<generator_result>;

  constexpr generator_unsigned generator_minimum =
      static_cast<generator_unsigned>(generator_type::min());

  constexpr generator_unsigned generator_maximum =
      static_cast<generator_unsigned>(generator_type::max());

  constexpr generator_unsigned generator_span =
      generator_maximum - generator_minimum;

  if (bound <= UInt{1}) {
    return UInt{0};
  }

  auto draw = [&]() -> generator_unsigned {
    return static_cast<generator_unsigned>(generator()) - generator_minimum;
  };

  /*
   * If one generator invocation has enough outcomes for
   * the requested bound, use rejection sampling directly.
   */
  if constexpr (numeric_limits<generator_unsigned>::digits >=
                numeric_limits<UInt>::digits) {
    const generator_unsigned target = static_cast<generator_unsigned>(bound);

    if (generator_span >= target - 1) {
      /*
       * Full-width generators have 2^digits outcomes, which
       * cannot itself be represented in generator_unsigned.
       */
      if (generator_span == numeric_limits<generator_unsigned>::max()) {
        const generator_unsigned threshold =
            static_cast<generator_unsigned>(generator_unsigned{0} - target) %
            target;

        generator_unsigned value;

        do {
          value = draw();
        } while (value < threshold);

        return static_cast<UInt>(value % target);
      }

      const generator_unsigned source_count = generator_span + 1;

      const generator_unsigned limit = source_count - source_count % target;

      generator_unsigned value;

      do {
        value = draw();
      } while (value >= limit);

      return static_cast<UInt>(value % target);
    }
  } else {
    const UInt source_count = static_cast<UInt>(generator_span) + 1;

    if (source_count >= bound) {
      const UInt limit = source_count - source_count % bound;

      UInt value;

      do {
        value = static_cast<UInt>(draw());
      } while (value >= limit);

      return value % bound;
    }
  }

  /*
   * The generator has fewer outcomes than the target range.
   * Build a mixed-radix value from multiple generator draws.
   *
   * At this point source_count < bound, so source_count is
   * representable in UInt even if the generator's full range
   * itself would not normally fit.
   */
  const UInt source_count = static_cast<UInt>(generator_span) + 1;

  const UInt quotient = bound / source_count;

  const UInt remainder = bound % source_count;

  const UInt high_bound = quotient + (remainder != 0 ? UInt{1} : UInt{0});

  for (;;) {
    const UInt high = algorithm_random_bounded(high_bound, generator);

    const UInt low = algorithm_random_bounded(source_count, generator);

    if (high < quotient || (high == quotient && low < remainder)) {
      return high * source_count + low;
    }
  }
}

template <class Iterator, class Sentinel>
constexpr iter_difference_t<Iterator> algorithm_random_distance(Iterator first,
                                                                Sentinel last) {
  iter_difference_t<Iterator> result = 0;

  for (; first != last; ++first) {
    ++result;
  }

  return result;
}

template <class RandomAccessIterator, class Generator>
void algorithm_shuffle_loop(RandomAccessIterator first,
                            RandomAccessIterator last, Generator &generator) {
  using difference_type =
      typename iterator_traits<RandomAccessIterator>::difference_type;

  using unsigned_difference = make_unsigned_t<difference_type>;

  const difference_type count = last - first;

  if (count <= 1) {
    return;
  }

  for (difference_type index = count - 1; index > 0; --index) {
    const unsigned_difference selected = algorithm_random_bounded(
        static_cast<unsigned_difference>(index + 1), generator);

    ranges::iter_swap(first + index,
                      first + static_cast<difference_type>(selected));
  }
}

template <class PopulationIterator, class PopulationSentinel,
          class SampleIterator, class Distance, class Generator>
SampleIterator
algorithm_sample_forward(PopulationIterator first, PopulationSentinel last,
                         SampleIterator out, Distance n, Generator &generator) {
  using population_difference = iter_difference_t<PopulationIterator>;

  using common_difference = common_type_t<population_difference, Distance>;

  using unsigned_difference = make_unsigned_t<common_difference>;

  if (n <= 0) {
    return out;
  }

  population_difference remaining = algorithm_random_distance(first, last);

  while (first != last && n > 0) {
    const auto selected = algorithm_random_bounded(
        static_cast<unsigned_difference>(remaining), generator);

    if (selected < static_cast<unsigned_difference>(n)) {
      *out = *first;
      ++out;
      --n;
    }

    ++first;
    --remaining;
  }

  return out;
}

template <class PopulationIterator, class PopulationSentinel,
          class SampleIterator, class Distance, class Generator>
SampleIterator algorithm_sample_reservoir(PopulationIterator first,
                                          PopulationSentinel last,
                                          SampleIterator out, Distance n,
                                          Generator &generator) {
  using population_difference = iter_difference_t<PopulationIterator>;

  using output_difference = iter_difference_t<SampleIterator>;

  using common_difference = common_type_t<population_difference, Distance>;

  using unsigned_difference = make_unsigned_t<common_difference>;

  if (n <= 0) {
    return out;
  }

  const unsigned_difference requested = static_cast<unsigned_difference>(n);

  unsigned_difference seen = 0;

  for (; first != last && seen < requested; ++first, ++seen) {
    out[static_cast<output_difference>(seen)] = *first;
  }

  if (first == last) {
    return out + static_cast<output_difference>(seen);
  }

  for (; first != last; ++first) {
    const unsigned_difference total = seen + 1;

    const unsigned_difference selected =
        algorithm_random_bounded(total, generator);

    if (selected < requested) {
      out[static_cast<output_difference>(selected)] = *first;
    }

    ++seen;
  }

  return out + static_cast<output_difference>(requested);
}

template <class PopulationIterator, class PopulationSentinel,
          class SampleIterator, class Distance, class Generator>
SampleIterator
algorithm_sample_loop(PopulationIterator first, PopulationSentinel last,
                      SampleIterator out, Distance n, Generator &generator) {
  if constexpr (forward_iterator<PopulationIterator>) {
    return algorithm_sample_forward(FTL_ALGORITHM_RANDOM_NAMESPACE::move(first),
                                    FTL_ALGORITHM_RANDOM_NAMESPACE::move(last),
                                    FTL_ALGORITHM_RANDOM_NAMESPACE::move(out),
                                    n, generator);
  } else {
    return algorithm_sample_reservoir(
        FTL_ALGORITHM_RANDOM_NAMESPACE::move(first),
        FTL_ALGORITHM_RANDOM_NAMESPACE::move(last),
        FTL_ALGORITHM_RANDOM_NAMESPACE::move(out), n, generator);
  }
}

} // namespace detail

template <class PopulationIterator, class SampleIterator, class Distance,
          class UniformRandomBitGenerator>
SampleIterator sample(PopulationIterator first, PopulationIterator last,
                      SampleIterator out, Distance n,
                      UniformRandomBitGenerator &&generator) {
  static_assert(is_integral_v<Distance>);

  return detail::algorithm_sample_loop(
      FTL_ALGORITHM_RANDOM_NAMESPACE::move(first),
      FTL_ALGORITHM_RANDOM_NAMESPACE::move(last),
      FTL_ALGORITHM_RANDOM_NAMESPACE::move(out), n, generator);
}

template <class RandomAccessIterator, class UniformRandomBitGenerator>
void shuffle(RandomAccessIterator first, RandomAccessIterator last,
             UniformRandomBitGenerator &&generator) {
  detail::algorithm_shuffle_loop(FTL_ALGORITHM_RANDOM_NAMESPACE::move(first),
                                 FTL_ALGORITHM_RANDOM_NAMESPACE::move(last),
                                 generator);
}

namespace ranges {

struct sample_fn {
  template <input_iterator Iterator, sentinel_for<Iterator> Sentinel,
            weakly_incrementable Output, class Generator>
    requires((forward_iterator<Iterator> || random_access_iterator<Output>) &&
             indirectly_copyable<Iterator, Output> &&
             FTL_ALGORITHM_RANDOM_NAMESPACE::detail::
                 algorithm_uniform_random_bit_generator<
                     remove_reference_t<Generator>>)
  Output operator()(Iterator first, Sentinel last, Output out,
                    iter_difference_t<Iterator> n,
                    Generator &&generator) const {
    return FTL_ALGORITHM_RANDOM_NAMESPACE::detail::algorithm_sample_loop(
        FTL_ALGORITHM_RANDOM_NAMESPACE::move(first),
        FTL_ALGORITHM_RANDOM_NAMESPACE::move(last),
        FTL_ALGORITHM_RANDOM_NAMESPACE::move(out), n, generator);
  }

  template <input_range Range, weakly_incrementable Output, class Generator>
    requires((forward_range<Range> || random_access_iterator<Output>) &&
             indirectly_copyable<iterator_t<Range>, Output> &&
             FTL_ALGORITHM_RANDOM_NAMESPACE::detail::
                 algorithm_uniform_random_bit_generator<
                     remove_reference_t<Generator>>)
  Output operator()(Range &&range, Output out, range_difference_t<Range> n,
                    Generator &&generator) const {
    return (*this)(
        ranges::begin(range), ranges::end(range),
        FTL_ALGORITHM_RANDOM_NAMESPACE::move(out), n,
        FTL_ALGORITHM_RANDOM_NAMESPACE::forward<Generator>(generator));
  }
};

struct shuffle_fn {
  template <random_access_iterator Iterator, sentinel_for<Iterator> Sentinel,
            class Generator>
    requires(permutable<Iterator> && FTL_ALGORITHM_RANDOM_NAMESPACE::detail::
                                         algorithm_uniform_random_bit_generator<
                                             remove_reference_t<Generator>>)
  Iterator operator()(Iterator first, Sentinel last,
                      Generator &&generator) const {
    Iterator end = first;

    while (end != last) {
      ++end;
    }

    FTL_ALGORITHM_RANDOM_NAMESPACE::detail::algorithm_shuffle_loop(first, end,
                                                                   generator);

    return end;
  }

  template <random_access_range Range, class Generator>
    requires(permutable<iterator_t<Range>> &&
             FTL_ALGORITHM_RANDOM_NAMESPACE::detail::
                 algorithm_uniform_random_bit_generator<
                     remove_reference_t<Generator>>)
  borrowed_iterator_t<Range> operator()(Range &&range,
                                        Generator &&generator) const {
    auto result =
        (*this)(ranges::begin(range), ranges::end(range),
                FTL_ALGORITHM_RANDOM_NAMESPACE::forward<Generator>(generator));

    return FTL_ALGORITHM_RANDOM_NAMESPACE::move(result);
  }
};

inline constexpr sample_fn sample{};
inline constexpr shuffle_fn shuffle{};

} // namespace ranges

FTL_END_NAMESPACE

#undef FTL_ALGORITHM_RANDOM_NAMESPACE

#endif // FTL_ALGORITHM_RANDOM_HEADER
