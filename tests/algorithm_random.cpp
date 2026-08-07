#ifdef FTL_REPLACE_STL
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/algorithm>
#include <ftl/concepts>
#include <ftl/cstddef>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct test_generator {
  using result_type = unsigned;

  unsigned state = 1;

  static constexpr result_type min() noexcept { return 0; }

  static constexpr result_type max() noexcept { return ~result_type{0}; }

  result_type operator()() noexcept {
    state = state * 1664525u + 1013904223u;

    return state;
  }
};

struct tiny_generator {
  using result_type = unsigned;

  unsigned state = 0;

  static constexpr result_type min() noexcept { return 0; }

  static constexpr result_type max() noexcept { return 3; }

  result_type operator()() noexcept {
    const auto result = state;
    state = (state + 1) & 3u;
    return result;
  }
};

struct signed_generator {
  using result_type = int;

  static constexpr result_type min() noexcept { return 0; }

  static constexpr result_type max() noexcept { return 7; }

  result_type operator()() noexcept { return 0; }
};

struct constant_generator {
  using result_type = unsigned;

  static constexpr result_type min() noexcept { return 0; }

  static constexpr result_type max() noexcept { return 0; }

  result_type operator()() noexcept { return 0; }
};

struct input_cursor {
  using value_type = int;
  using difference_type = tested::ptrdiff_t;
  using iterator_concept = tested::input_iterator_tag;
  using iterator_category = tested::input_iterator_tag;

  int *current = nullptr;

  int &operator*() const noexcept { return *current; }

  input_cursor &operator++() noexcept {
    ++current;
    return *this;
  }

  void operator++(int) noexcept { ++current; }

  friend bool operator==(input_cursor left, input_cursor right) noexcept {
    return left.current == right.current;
  }
};

static_assert(tested::input_iterator<input_cursor>);

static_assert(!tested::forward_iterator<input_cursor>);

struct output_cursor {
  using difference_type = tested::ptrdiff_t;

  int *current = nullptr;

  int &operator*() const noexcept { return *current; }

  output_cursor &operator++() noexcept {
    ++current;
    return *this;
  }

  output_cursor operator++(int) noexcept {
    auto old = *this;
    ++current;
    return old;
  }
};

static_assert(tested::weakly_incrementable<output_cursor>);

static_assert(!tested::random_access_iterator<output_cursor>);

static_assert(tested::indirectly_copyable<int *, output_cursor>);

template <class Generator>
concept ranges_shuffle_accepts =
    requires(int *first, int *last, Generator &generator) {
      {
        tested::ranges::shuffle(first, last, generator)
      } -> tested::same_as<int *>;
    };

static_assert(ranges_shuffle_accepts<test_generator>);

static_assert(ranges_shuffle_accepts<tiny_generator>);

static_assert(!ranges_shuffle_accepts<signed_generator>);

static_assert(!ranges_shuffle_accepts<constant_generator>);

template <class Input, class Output>
concept ranges_sample_accepts =
    requires(Input first, Input last, Output out, test_generator &generator) {
      tested::ranges::sample(first, last, out, 2, generator);
    };

static_assert(ranges_sample_accepts<int *, output_cursor>);

static_assert(ranges_sample_accepts<input_cursor, int *>);

static_assert(!ranges_sample_accepts<input_cursor, output_cursor>);

struct temporary_range {
  int values[4] = {
      1,
      2,
      3,
      4,
  };

  int *begin() noexcept { return values; }

  int *end() noexcept { return values + 4; }
};

static_assert(tested::is_same_v<decltype(tested::ranges::shuffle(
                                    tested::declval<temporary_range>(),
                                    tested::declval<test_generator &>())),
                                tested::ranges::dangling>);

bool contains_once(const int *values, int count, int wanted) {
  int matches = 0;

  for (int index = 0; index < count; ++index) {
    if (values[index] == wanted) {
      ++matches;
    }
  }

  return matches == 1;
}

bool classic_shuffle_works() {
  int values[] = {
      1, 2, 3, 4, 5, 6,
  };

  test_generator generator{};

  tested::shuffle(values, values + 6, generator);

  for (int value = 1; value <= 6; ++value) {
    if (!contains_once(values, 6, value)) {
      return false;
    }
  }

  int one[] = {42};

  tested::shuffle(one, one + 1, generator);

  return one[0] == 42;
}

bool ranges_shuffle_works() {
  int values[] = {
      1, 2, 3, 4, 5,
  };

  test_generator generator{};

  auto end = tested::ranges::shuffle(values, generator);

  if (end != values + 5) {
    return false;
  }

  for (int value = 1; value <= 5; ++value) {
    if (!contains_once(values, 5, value)) {
      return false;
    }
  }

  return true;
}

bool classic_sample_forward_works() {
  int population[] = {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
  };

  int output[4] = {
      -1,
      -1,
      -1,
      -1,
  };

  test_generator generator{};

  auto end = tested::sample(population, population + 10, output, 4, generator);

  if (end != output + 4) {
    return false;
  }

  /*
   * Forward-population sampling is stable.
   */
  if (!(output[0] < output[1] && output[1] < output[2] &&
        output[2] < output[3])) {
    return false;
  }

  for (int index = 0; index < 4; ++index) {
    if (output[index] < 0 || output[index] >= 10) {
      return false;
    }
  }

  return true;
}

bool classic_sample_larger_than_population() {
  int population[] = {
      1,
      2,
      3,
  };

  int output[5] = {
      0, 0, 0, 0, 0,
  };

  test_generator generator{};

  auto end = tested::sample(population, population + 3, output, 9, generator);

  return end == output + 3 && output[0] == 1 && output[1] == 2 &&
         output[2] == 3;
}

bool classic_sample_zero_works() {
  int population[] = {
      1,
      2,
      3,
  };

  int output[] = {
      9,
      9,
      9,
  };

  test_generator generator{};

  auto end = tested::sample(population, population + 3, output, 0, generator);

  return end == output && output[0] == 9 && output[1] == 9 && output[2] == 9;
}

bool ranges_sample_forward_works() {
  int population[] = {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
  };

  int output[3] = {
      -1,
      -1,
      -1,
  };

  output_cursor out{output};

  test_generator generator{};

  auto end = tested::ranges::sample(population, out, 3, generator);

  if (end.current != output + 3) {
    return false;
  }

  return output[0] < output[1] && output[1] < output[2];
}

bool ranges_sample_input_works() {
  int population[] = {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
  };

  int output[4] = {
      -1,
      -1,
      -1,
      -1,
  };

  tiny_generator generator{};

  auto end = tested::ranges::sample(input_cursor{population},
                                    input_cursor{population + 12}, output, 4,
                                    generator);

  if (end != output + 4) {
    return false;
  }

  for (int index = 0; index < 4; ++index) {
    if (output[index] < 0 || output[index] >= 12) {
      return false;
    }

    for (int other = index + 1; other < 4; ++other) {
      if (output[index] == output[other]) {
        return false;
      }
    }
  }

  return true;
}

bool tiny_generator_shuffle_works() {
  /*
   * More positions than the generator has outcomes exercises
   * the multi-draw path in algorithm_random_bounded.
   */
  int values[] = {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
  };

  tiny_generator generator{};

  tested::shuffle(values, values + 10, generator);

  for (int value = 0; value < 10; ++value) {
    if (!contains_once(values, 10, value)) {
      return false;
    }
  }

  return true;
}

bool ftl_test() {
  return classic_shuffle_works() && ranges_shuffle_works() &&
         classic_sample_forward_works() &&
         classic_sample_larger_than_population() &&
         classic_sample_zero_works() && ranges_sample_forward_works() &&
         ranges_sample_input_works() && tiny_generator_shuffle_works();
}
