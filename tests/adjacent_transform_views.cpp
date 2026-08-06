#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct pair_sum {
  constexpr int operator()(int left, int right) const noexcept {
    return left + right;
  }
};

struct throwing_pair_sum {
  constexpr int operator()(int left, int right) const { return left + right; }
};

struct first_reference {
  constexpr int &operator()(int &first, int &) const noexcept { return first; }
};

struct mutable_pair_sum {
  constexpr int operator()(int left, int right) noexcept {
    return left + right;
  }
};

struct unary_function {
  constexpr int operator()(int value) const noexcept { return value; }
};

struct void_pair_function {
  constexpr void operator()(int, int) const noexcept {}
};

struct nullary_value {
  constexpr int operator()() const noexcept { return 42; }
};

struct triple_sum {
  constexpr int operator()(int first, int second, int third) const noexcept {
    return first + second + third;
  }
};

struct move_only_pair_sum {
  constexpr move_only_pair_sum() = default;

  move_only_pair_sum(const move_only_pair_sum &) = delete;

  move_only_pair_sum &operator=(const move_only_pair_sum &) = delete;

  constexpr move_only_pair_sum(move_only_pair_sum &&) noexcept = default;

  constexpr move_only_pair_sum &
  operator=(move_only_pair_sum &&) noexcept = default;

  constexpr int operator()(int left, int right) const noexcept {
    return left + right;
  }
};

/*
 * A non-common forward range verifies the sentinel
 * specialization separately from the common array path.
 */
struct forward_test_iterator {
  using value_type = int;

  using difference_type = tested::ptrdiff_t;

  using iterator_concept = tested::forward_iterator_tag;

  using iterator_category = tested::forward_iterator_tag;

  int *current = nullptr;

  constexpr int &operator*() const noexcept { return *current; }

  constexpr forward_test_iterator &operator++() noexcept {
    ++current;
    return *this;
  }

  constexpr forward_test_iterator operator++(int) noexcept {
    auto previous = *this;
    ++*this;
    return previous;
  }

  friend constexpr bool operator==(forward_test_iterator,
                                   forward_test_iterator) = default;
};

struct forward_test_sentinel {
  int *end = nullptr;

  friend constexpr bool operator==(forward_test_iterator current,
                                   forward_test_sentinel bound) noexcept {
    return current.current == bound.end;
  }
};

struct forward_test_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr forward_test_view() = default;

  constexpr forward_test_view(int *first_value, int *last_value) noexcept
      : first(first_value), last(last_value) {}

  constexpr forward_test_iterator begin() const noexcept { return {first}; }

  constexpr forward_test_sentinel end() const noexcept { return {last}; }
};

static_assert(tested::ranges::forward_range<forward_test_view>);

static_assert(!tested::ranges::common_range<forward_test_view>);

using int_array = int (&)[4];

using base_view = tested::ranges::views::all_t<int_array>;

template <class V, class F, tested::size_t N>
concept can_form_adjacent_transform =
    requires { typename tested::ranges::adjacent_transform_view<V, F, N>; };

static_assert(can_form_adjacent_transform<base_view, pair_sum, 2>);

static_assert(!can_form_adjacent_transform<base_view, pair_sum, 0>);

static_assert(!can_form_adjacent_transform<base_view, unary_function, 2>);

static_assert(!can_form_adjacent_transform<base_view, void_pair_function, 2>);

using sum_view =
    tested::ranges::adjacent_transform_view<base_view, pair_sum, 2>;

using sum_iterator = tested::ranges::iterator_t<sum_view>;

static_assert(tested::is_constructible_v<sum_view, base_view, pair_sum>);

static_assert(tested::ranges::view<sum_view>);

static_assert(tested::ranges::random_access_range<sum_view>);

static_assert(tested::ranges::common_range<sum_view>);

static_assert(tested::ranges::sized_range<sum_view>);

static_assert(tested::ranges::range<const sum_view>);

static_assert(!tested::ranges::borrowed_range<sum_view>);

static_assert(tested::is_same_v<tested::ranges::range_value_t<sum_view>, int>);

static_assert(
    tested::is_same_v<tested::ranges::range_reference_t<sum_view>, int>);

static_assert(tested::is_same_v<typename sum_iterator::iterator_concept,
                                tested::random_access_iterator_tag>);

/*
 * The traversal remains random access, but a prvalue
 * transformation has only an input legacy category.
 */
static_assert(tested::is_same_v<typename sum_iterator::iterator_category,
                                tested::input_iterator_tag>);

static_assert(noexcept(*tested::declval<const sum_iterator &>()));

using throwing_view =
    tested::ranges::adjacent_transform_view<base_view, throwing_pair_sum, 2>;

using throwing_iterator = tested::ranges::iterator_t<throwing_view>;

static_assert(!noexcept(*tested::declval<const throwing_iterator &>()));

using reference_view =
    tested::ranges::adjacent_transform_view<base_view, first_reference, 2>;

using reference_iterator = tested::ranges::iterator_t<reference_view>;

static_assert(tested::is_same_v<
              tested::ranges::range_reference_t<reference_view>, int &>);

static_assert(tested::is_same_v<typename reference_iterator::iterator_category,
                                tested::random_access_iterator_tag>);

using mutable_function_view =
    tested::ranges::adjacent_transform_view<base_view, mutable_pair_sum, 2>;

static_assert(tested::ranges::range<mutable_function_view>);

static_assert(!tested::ranges::range<const mutable_function_view>);

using non_common_view =
    tested::ranges::adjacent_transform_view<forward_test_view, pair_sum, 2>;

static_assert(tested::ranges::forward_range<non_common_view>);

static_assert(!tested::ranges::bidirectional_range<non_common_view>);

static_assert(!tested::ranges::common_range<non_common_view>);

static_assert(!tested::is_same_v<tested::ranges::iterator_t<non_common_view>,
                                 tested::ranges::sentinel_t<non_common_view>>);

using move_only_view =
    tested::ranges::adjacent_transform_view<base_view, move_only_pair_sum, 2>;

static_assert(
    tested::is_constructible_v<move_only_view, base_view, move_only_pair_sum>);

using zero_result = decltype(tested::ranges::views::adjacent_transform<0>(
    tested::declval<int_array>(), nullary_value{}));

static_assert(tested::is_same_v<zero_result, tested::ranges::empty_view<int>>);

constexpr bool adjacent_transform_view_works() {
  {
    int values[] = {1, 2, 3, 4};

    auto base = tested::ranges::views::all(values);

    using view_type =
        tested::ranges::adjacent_transform_view<decltype(base), pair_sum, 2>;

    view_type view{base, pair_sum{}};

    if (view.size() != 3) {
      return false;
    }

    auto first = view.begin();
    auto last = view.end();

    if (*first != 3 || first[1] != 5 || last - first != 3) {
      return false;
    }

    auto third = first + 2;

    if (*third != 7 || !(first < third) || !(third > first) ||
        !(first <= third) || !(third >= first)) {
      return false;
    }

    auto previous = last;

    --previous;

    if (*previous != 7) {
      return false;
    }

    const auto &const_view = view;

    if (const_view.size() != 3 || *const_view.begin() != 3) {
      return false;
    }

    auto copied_base = view.base();

    if (copied_base.begin() != values) {
      return false;
    }
  }

  {
    int values[] = {1, 2, 3, 4};

    auto references = values | tested::ranges::views::adjacent_transform<2>(
                                   first_reference{});

    for (int &value : references) {
      value += 10;
    }

    if (values[0] != 11 || values[1] != 12 || values[2] != 13 ||
        values[3] != 4) {
      return false;
    }
  }

  {
    int values[] = {1, 2, 3, 4};

    auto pairwise =
        values | tested::ranges::views::pairwise_transform(pair_sum{});

    const int expected[] = {3, 5, 7};

    tested::size_t index = 0;

    for (int value : pairwise) {
      if (index >= 3 || value != expected[index]) {
        return false;
      }

      ++index;
    }

    if (index != 3) {
      return false;
    }
  }

  {
    int values[] = {1, 2};

    auto view =
        tested::ranges::views::adjacent_transform<3>(values, triple_sum{});

    if (view.begin() != view.end() || view.size() != 0) {
      return false;
    }
  }

  {
    int values[] = {1, 2, 3, 4};

    forward_test_view base{values, values + 4};

    non_common_view view{base, pair_sum{}};

    const int expected[] = {3, 5, 7};

    auto iterator = view.begin();

    auto bound = view.end();

    tested::size_t index = 0;

    while (iterator != bound) {
      if (index >= 3 || *iterator != expected[index]) {
        return false;
      }

      ++iterator;
      ++index;
    }

    if (index != 3) {
      return false;
    }
  }

  {
    int values[] = {1, 2, 3, 4};

    auto zero =
        tested::ranges::views::adjacent_transform<0>(values, nullary_value{});

    if (zero.size() != 0) {
      return false;
    }
  }

  {
    int values[] = {1, 2, 3, 4};

    auto move_only = tested::ranges::views::adjacent_transform<2>(
        values, move_only_pair_sum{});

    if (move_only.size() != 3 || *move_only.begin() != 3) {
      return false;
    }
  }

  return true;
}

static_assert(adjacent_transform_view_works());

bool ftl_test() { return adjacent_transform_view_works(); }