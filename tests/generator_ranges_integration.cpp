#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <generator>
#include <ranges>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/generator>
#include <ftl/ranges>
#include <ftl/span>
#include <ftl/tuple>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

using value_generator = tested::generator<int>;

using reference_generator = tested::generator<int &>;

using enumerate_pipeline = decltype(tested::declval<value_generator>() |
                                    tested::ranges::views::enumerate);

using enumerate_iterator = tested::ranges::iterator_t<enumerate_pipeline>;

static_assert(tested::ranges::view<enumerate_pipeline>);

static_assert(tested::ranges::input_range<enumerate_pipeline>);

static_assert(!tested::ranges::forward_range<enumerate_pipeline>);

static_assert(!tested::ranges::common_range<enumerate_pipeline>);

static_assert(!tested::default_initializable<enumerate_iterator>);

template <class Range, class Adaptor>
concept pipeable = requires(Range &&range, Adaptor &&adaptor) {
  static_cast<Range &&>(range) | static_cast<Adaptor &&>(adaptor);
};

template <class Range>
concept rvalue_beginable = requires(Range &&range) {
  tested::ranges::begin(static_cast<Range &&>(range));
};

struct is_even {
  [[nodiscard]]
  constexpr bool operator()(int value) const noexcept {
    return value % 2 == 0;
  }
};

struct times_ten {
  [[nodiscard]]
  constexpr int operator()(int value) const noexcept {
    return value * 10;
  }
};

struct preserve_reference {
  [[nodiscard]]
  constexpr int &operator()(int &value) const noexcept {
    return value;
  }
};

struct add_values {
  [[nodiscard]]
  constexpr int operator()(int left, int right) const noexcept {
    return left + right;
  }
};

using take_two_adaptor = decltype(tested::ranges::views::take(2));

using value_pipeline = decltype(tested::declval<value_generator>() |
                                tested::ranges::views::filter(is_even{}) |
                                tested::ranges::views::transform(times_ten{}) |
                                tested::ranges::views::take(2));

using reference_pipeline =
    decltype(tested::declval<reference_generator>() |
             tested::ranges::views::transform(preserve_reference{}));

using row_span = tested::span<int>;

using row_generator = tested::generator<row_span>;

using join_pipeline =
    decltype(tested::declval<row_generator>() | tested::ranges::views::join);

using join_iterator = tested::ranges::iterator_t<join_pipeline>;

static_assert(tested::ranges::view<join_pipeline>);

static_assert(tested::ranges::input_range<join_pipeline>);

static_assert(!tested::ranges::forward_range<join_pipeline>);

static_assert(!tested::ranges::common_range<join_pipeline>);

static_assert(!tested::ranges::borrowed_range<join_pipeline>);

static_assert(tested::default_initializable<join_iterator>);

static_assert(
    tested::is_same_v<tested::ranges::range_value_t<join_pipeline>, int>);

static_assert(
    tested::is_same_v<tested::ranges::range_reference_t<join_pipeline>, int &>);

static_assert(tested::ranges::view<value_generator>);

static_assert(tested::ranges::input_range<value_generator>);

static_assert(!tested::ranges::forward_range<value_generator>);

static_assert(!tested::ranges::bidirectional_range<value_generator>);

static_assert(!tested::ranges::random_access_range<value_generator>);

static_assert(!tested::ranges::contiguous_range<value_generator>);

static_assert(!tested::ranges::common_range<value_generator>);

static_assert(!tested::ranges::sized_range<value_generator>);

static_assert(!tested::ranges::borrowed_range<value_generator>);

static_assert(tested::ranges::viewable_range<value_generator>);

static_assert(!tested::ranges::viewable_range<value_generator &>);

static_assert(!rvalue_beginable<value_generator>);

static_assert(pipeable<value_generator, take_two_adaptor>);

static_assert(!pipeable<value_generator &, take_two_adaptor>);

static_assert(tested::ranges::view<value_pipeline>);

static_assert(tested::ranges::input_range<value_pipeline>);

static_assert(!tested::ranges::forward_range<value_pipeline>);

static_assert(!tested::ranges::borrowed_range<value_pipeline>);

static_assert(
    tested::is_same_v<tested::ranges::range_value_t<value_pipeline>, int>);

static_assert(tested::ranges::input_range<reference_pipeline>);

static_assert(tested::is_same_v<
              tested::ranges::range_reference_t<reference_pipeline>, int &>);

using transform_only_pipeline =
    decltype(tested::declval<value_generator>() |
             tested::ranges::views::transform(times_ten{}));

using transform_only_iterator =
    tested::ranges::iterator_t<transform_only_pipeline>;

static_assert(tested::ranges::view<transform_only_pipeline>);

static_assert(tested::ranges::input_range<transform_only_pipeline>);

static_assert(!tested::default_initializable<transform_only_iterator>);

using stride_pipeline = decltype(tested::declval<value_generator>() |
                                 tested::ranges::views::stride(2));

using stride_iterator = tested::ranges::iterator_t<stride_pipeline>;

static_assert(tested::ranges::view<stride_pipeline>);

static_assert(tested::ranges::input_range<stride_pipeline>);

static_assert(!tested::ranges::forward_range<stride_pipeline>);

static_assert(!tested::default_initializable<stride_iterator>);

using chunk_pipeline = decltype(tested::declval<value_generator>() |
                                tested::ranges::views::chunk(2));

static_assert(tested::ranges::view<chunk_pipeline>);

static_assert(tested::ranges::input_range<chunk_pipeline>);

static_assert(!tested::ranges::forward_range<chunk_pipeline>);

static_assert(!tested::ranges::borrowed_range<chunk_pipeline>);

using six_int_array = int (&)[6];

using zip_transform_pipeline = decltype(tested::ranges::views::zip_transform(
    add_values{}, tested::declval<value_generator>(),
    tested::declval<six_int_array>()));

using zip_transform_iterator =
    tested::ranges::iterator_t<zip_transform_pipeline>;

static_assert(tested::ranges::view<zip_transform_pipeline>);

static_assert(tested::ranges::input_range<zip_transform_pipeline>);

static_assert(!tested::ranges::forward_range<zip_transform_pipeline>);

static_assert(!tested::ranges::common_range<zip_transform_pipeline>);

static_assert(!tested::ranges::borrowed_range<zip_transform_pipeline>);

static_assert(!tested::default_initializable<zip_transform_iterator>);

static_assert(tested::is_same_v<
              tested::ranges::range_value_t<zip_transform_pipeline>, int>);

static_assert(tested::is_same_v<
              tested::ranges::range_reference_t<zip_transform_pipeline>, int>);

#ifdef FTL_REPLACE_STL

tested::generator<int> integration_values() {
  for (int value = 1; value <= 6; ++value) {
    co_yield value;
  }
}

tested::generator<int &> integration_references(int &first, int &second) {
  co_yield first;
  co_yield second;
}

tested::generator<int> integration_five_values() {
  for (int value = 1; value <= 5; ++value) {
    co_yield value;
  }
}

tested::generator<row_span> integration_rows() {
  int first_values[] = {1, 2};

  int second_values[] = {3, 4, 5};

  row_span first{first_values};

  row_span empty{};

  row_span second{second_values};

  co_yield first;
  co_yield empty;
  co_yield second;
}

#endif

bool ftl_test() {
#ifdef FTL_REPLACE_STL
  {
    auto sequence = integration_values();

    auto current = tested::ranges::begin(sequence);

    const auto bound = tested::ranges::end(sequence);

    int expected = 1;

    while (current != bound) {
      if (*current != expected) {
        return false;
      }

      ++expected;
      ++current;
    }

    if (expected != 7) {
      return false;
    }
  }

  {
    auto pipeline = integration_values() |
                    tested::ranges::views::filter(is_even{}) |
                    tested::ranges::views::transform(times_ten{}) |
                    tested::ranges::views::take(2);

    const int expected[] = {20, 40};

    tested::size_t index = 0;

    for (int value : pipeline) {
      if (index >= 2 || value != expected[index]) {
        return false;
      }

      ++index;
    }

    if (index != 2) {
      return false;
    }
  }

  {
    int first = 10;
    int second = 20;

    auto pipeline = integration_references(first, second) |
                    tested::ranges::views::transform(preserve_reference{});

    for (int &value : pipeline) {
      value += 5;
    }

    if (first != 15 || second != 25) {
      return false;
    }
  }

  {
    auto pipeline = integration_values() | tested::ranges::views::enumerate;

    tested::size_t expected_index = 0;
    int expected_value = 1;

    for (auto entry : pipeline) {
      const auto index = tested::get<0>(entry);

      const int value = tested::get<1>(entry);

      if (index != expected_index || value != expected_value) {
        return false;
      }

      ++expected_index;
      ++expected_value;
    }

    if (expected_index != 6 || expected_value != 7) {
      return false;
    }
  }

  {
    auto pipeline = integration_five_values() | tested::ranges::views::chunk(2);

    const int expected[] = {1, 2, 3, 4, 5};

    const tested::size_t expected_chunk_sizes[] = {2, 2, 1};

    tested::size_t value_index = 0;
    tested::size_t chunk_index = 0;

    for (auto chunk : pipeline) {
      if (chunk_index >= 3) {
        return false;
      }

      tested::size_t chunk_size = 0;

      for (int value : chunk) {
        if (value_index >= 5 || value != expected[value_index]) {
          return false;
        }

        ++value_index;
        ++chunk_size;
      }

      if (chunk_size != expected_chunk_sizes[chunk_index]) {
        return false;
      }

      ++chunk_index;
    }

    if (value_index != 5 || chunk_index != 3) {
      return false;
    }
  }

  {
    int offsets[] = {10, 20, 30, 40};

    auto pipeline = tested::ranges::views::zip_transform(
        add_values{}, integration_values(), offsets);

    const int expected[] = {11, 22, 33, 44};

    tested::size_t index = 0;

    for (int value : pipeline) {
      if (index >= 4 || value != expected[index]) {
        return false;
      }

      ++index;
    }

    /*
     * The generator contains six values, but zip_transform
     * stops when the four-element offsets range ends.
     */
    if (index != 4) {
      return false;
    }
  }

  {
    auto pipeline = integration_rows() | tested::ranges::views::join;

    const int expected[] = {1, 2, 3, 4, 5};

    tested::size_t index = 0;

    for (int &value : pipeline) {
      if (index >= 5 || value != expected[index]) {
        return false;
      }

      value *= 10;
      ++index;
    }

    if (index != 5) {
      return false;
    }
  }
#endif

  return true;
}