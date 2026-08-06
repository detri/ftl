#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <generator>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/generator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

using value_generator = tested::generator<int>;

using reference_generator = tested::generator<int &>;

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

using take_two_adaptor = decltype(tested::ranges::views::take(2));

using value_pipeline = decltype(tested::declval<value_generator>() |
                                tested::ranges::views::filter(is_even{}) |
                                tested::ranges::views::transform(times_ten{}) |
                                tested::ranges::views::take(2));

using reference_pipeline =
    decltype(tested::declval<reference_generator>() |
             tested::ranges::views::transform(preserve_reference{}));

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
#endif

  return true;
}