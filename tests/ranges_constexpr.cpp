#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <iterator>
#include <ranges>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/span>
#include <ftl/tuple>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct pointer_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr pointer_view() = default;

  constexpr pointer_view(int *first, int *last) noexcept
      : first(first), last(last) {}

  constexpr int *begin() noexcept { return first; }

  constexpr const int *begin() const noexcept { return first; }

  constexpr int *end() noexcept { return last; }

  constexpr const int *end() const noexcept { return last; }

  constexpr tested::size_t size() const noexcept {
    return static_cast<tested::size_t>(last - first);
  }
};

struct movable_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr movable_range() = default;

  constexpr movable_range(int *first, int *last) noexcept
      : first(first), last(last) {}

  movable_range(const movable_range &) = delete;

  movable_range &operator=(const movable_range &) = delete;

  constexpr movable_range(movable_range &&) noexcept = default;

  constexpr movable_range &operator=(movable_range &&) noexcept = default;

  constexpr int *begin() noexcept { return first; }

  constexpr const int *begin() const noexcept { return first; }

  constexpr int *end() noexcept { return last; }

  constexpr const int *end() const noexcept { return last; }

  constexpr tested::size_t size() const noexcept {
    return static_cast<tested::size_t>(last - first);
  }
};

struct input_iterator {
  using iterator_concept = tested::input_iterator_tag;

  using iterator_category = tested::input_iterator_tag;

  using value_type = int;

  using difference_type = tested::ptrdiff_t;

  int *current = nullptr;

  constexpr int &operator*() const noexcept { return *current; }

  constexpr input_iterator &operator++() noexcept {
    ++current;
    return *this;
  }

  constexpr void operator++(int) noexcept { ++current; }

  friend constexpr bool operator==(const input_iterator &,
                                   const input_iterator &) = default;
};

struct input_sentinel {
  int *last = nullptr;

  friend constexpr bool operator==(input_iterator current,
                                   input_sentinel bound) noexcept {
    return current.current == bound.last;
  }

  friend constexpr bool operator==(input_sentinel bound,
                                   input_iterator current) noexcept {
    return current == bound;
  }
};

struct input_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr input_view() = default;

  constexpr input_view(int *first, int *last) noexcept
      : first(first), last(last) {}

  constexpr input_iterator begin() noexcept { return input_iterator{first}; }

  constexpr input_sentinel end() noexcept { return input_sentinel{last}; }
};

struct forward_iterator {
  using iterator_concept = tested::forward_iterator_tag;

  using iterator_category = tested::forward_iterator_tag;

  using value_type = int;

  using difference_type = tested::ptrdiff_t;

  int *current = nullptr;

  constexpr int &operator*() const noexcept { return *current; }

  constexpr forward_iterator &operator++() noexcept {
    ++current;
    return *this;
  }

  constexpr forward_iterator operator++(int) noexcept {
    auto previous = *this;
    ++*this;
    return previous;
  }

  friend constexpr bool operator==(const forward_iterator &,
                                   const forward_iterator &) = default;
};

struct forward_sentinel {
  int *last = nullptr;

  friend constexpr bool operator==(forward_iterator current,
                                   forward_sentinel bound) noexcept {
    return current.current == bound.last;
  }

  friend constexpr bool operator==(forward_sentinel bound,
                                   forward_iterator current) noexcept {
    return current == bound;
  }
};

struct forward_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr forward_view() = default;

  constexpr forward_view(int *first, int *last) noexcept
      : first(first), last(last) {}

  constexpr forward_iterator begin() const noexcept {
    return forward_iterator{first};
  }

  constexpr forward_sentinel end() const noexcept {
    return forward_sentinel{last};
  }
};

struct sized_sentinel {
  int *last = nullptr;

  friend constexpr bool operator==(int *current,
                                   sized_sentinel bound) noexcept {
    return current == bound.last;
  }

  friend constexpr bool operator==(sized_sentinel bound,
                                   int *current) noexcept {
    return current == bound;
  }

  friend constexpr tested::ptrdiff_t operator-(sized_sentinel bound,
                                               int *current) noexcept {
    return bound.last - current;
  }

  friend constexpr tested::ptrdiff_t operator-(int *current,
                                               sized_sentinel bound) noexcept {
    return current - bound.last;
  }
};

struct non_common_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr non_common_view() = default;

  constexpr non_common_view(int *first, int *last) noexcept
      : first(first), last(last) {}

  constexpr int *begin() const noexcept { return first; }

  constexpr sized_sentinel end() const noexcept { return sized_sentinel{last}; }

  constexpr tested::size_t size() const noexcept {
    return static_cast<tested::size_t>(last - first);
  }
};

struct counting_sentinel {
  int *last = nullptr;
  int *comparisons = nullptr;

  friend constexpr bool operator==(int *current,
                                   counting_sentinel bound) noexcept {
    if (bound.comparisons != nullptr) {
      ++*bound.comparisons;
    }

    return current == bound.last;
  }

  friend constexpr bool operator==(counting_sentinel bound,
                                   int *current) noexcept {
    return current == bound;
  }
};

struct cached_reverse_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;
  int *comparisons = nullptr;

  constexpr int *begin() noexcept { return first; }

  constexpr counting_sentinel end() noexcept {
    return counting_sentinel{last, comparisons};
  }

  cached_reverse_view() = default;

  constexpr cached_reverse_view(int *arr, int *p, int *comparisons) noexcept
      : first(arr), last(p), comparisons(comparisons) {}
};

struct inner_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr inner_view() = default;

  constexpr inner_view(int *first, int *last) noexcept
      : first(first), last(last) {}

  constexpr int *begin() const noexcept { return first; }

  constexpr int *end() const noexcept { return last; }
};

struct outer_view : tested::ranges::view_base {
  inner_view *first = nullptr;
  inner_view *last = nullptr;

  constexpr outer_view() = default;

  constexpr outer_view(inner_view *first, inner_view *last) noexcept
      : first(first), last(last) {}

  constexpr inner_view *begin() const noexcept { return first; }

  constexpr inner_view *end() const noexcept { return last; }
};

struct doubled {
  constexpr int operator()(int value) const noexcept { return value * 2; }
};

struct identity_reference {
  constexpr int &operator()(int &value) const noexcept { return value; }
};

struct is_even {
  constexpr bool operator()(int value) const noexcept { return value % 2 == 0; }
};

struct less_than_four {
  constexpr bool operator()(int value) const noexcept { return value < 4; }
};

struct sum_values {
  constexpr int operator()(int left, int right) const noexcept {
    return left + right;
  }
};

struct first_reference {
  constexpr int &operator()(int &left, int &) const noexcept { return left; }
};

struct close_values {
  constexpr bool operator()(int left, int right) const noexcept {
    return right - left <= 1;
  }
};

struct counting_even {
  int *calls = nullptr;

  constexpr bool operator()(int value) const noexcept {
    ++*calls;
    return value % 2 == 0;
  }
};

struct counting_less_than {
  int *calls = nullptr;
  int limit = 0;

  constexpr bool operator()(int value) const noexcept {
    ++*calls;
    return value < limit;
  }
};

template <class Range> constexpr int sum_range(Range &&range) {
  int result = 0;

  auto current = tested::ranges::begin(range);

  const auto bound = tested::ranges::end(range);

  while (current != bound) {
    result += *current;
    ++current;
  }

  return result;
}

template <class Range>
constexpr bool equal_range(Range &&range, const int *expected,
                           tested::size_t count) {
  auto current = tested::ranges::begin(range);

  const auto bound = tested::ranges::end(range);

  for (tested::size_t index = 0; index < count; ++index, ++current) {
    if (current == bound || *current != expected[index]) {
      return false;
    }
  }

  return current == bound;
}

constexpr bool foundational_views_work() {
  int values[] = {1, 2, 3, 4};

  pointer_view base{values, values + 4};

  tested::ranges::ref_view reference{base};

  if (reference.begin() != values || reference.end() != values + 4 ||
      reference.size() != 4 || reference.front() != 1 ||
      reference.back() != 4 || reference[2] != 3) {
    return false;
  }

  auto all_reference = tested::ranges::views::all(base);

  if (all_reference.begin() != values || all_reference.end() != values + 4) {
    return false;
  }

  auto owning = tested::ranges::views::all(movable_range{values, values + 4});

  if (owning.size() != 4 || owning.front() != 1 || owning.back() != 4) {
    return false;
  }

  constexpr auto empty = tested::ranges::views::empty<int>;

  if (!empty.empty() || empty.size() != 0 || empty.begin() != nullptr ||
      empty.end() != nullptr || empty.data() != nullptr) {
    return false;
  }

  auto single = tested::ranges::views::single(42);

  if (single.empty() || single.size() != 1 || single.front() != 42 ||
      single.back() != 42 || single[0] != 42) {
    return false;
  }

  tested::ranges::subrange subrange(values, values + 4);

  if (subrange.size() != 4 || subrange.front() != 1 || subrange.back() != 4 ||
      subrange[3] != 4) {
    return false;
  }

  auto advanced = subrange.next(2);

  if (advanced.begin() != values + 2 || advanced.front() != 3) {
    return false;
  }

  advanced.advance(-1);

  if (advanced.begin() != values + 1 || advanced.front() != 2) {
    return false;
  }

  return true;
}

constexpr bool classic_views_work() {
  int values[] = {1, 2, 3, 4, 5, 6};

  pointer_view base{values, values + 6};

  auto transformed = base | tested::ranges::views::transform(doubled{});

  if (transformed.size() != 6 || transformed[0] != 2 || transformed[2] != 6 ||
      transformed[5] != 12) {
    return false;
  }

  const auto &constant_transformed = transformed;

  if (constant_transformed[1] != 4 ||
      constant_transformed.end() - constant_transformed.begin() != 6) {
    return false;
  }

  auto references =
      base | tested::ranges::views::transform(identity_reference{});

  references[4] = 9;

  if (values[4] != 9) {
    return false;
  }

  int input_values[] = {1, 2, 3, 4, 5, 6};

  auto filtered = input_view{input_values, input_values + 6} |
                  tested::ranges::views::filter(is_even{});

  auto filter_iterator = filtered.begin();

  if (*filter_iterator != 2) {
    return false;
  }

  ++filter_iterator;

  if (*filter_iterator != 4) {
    return false;
  }

  ++filter_iterator;

  if (*filter_iterator != 6) {
    return false;
  }

  ++filter_iterator;

  if (filter_iterator != filtered.end()) {
    return false;
  }

  auto taken = base | tested::ranges::views::take(3);

  if (taken.size() != 3 || taken[0] != 1 || taken[2] != 3) {
    return false;
  }

  auto taken_past_end = base | tested::ranges::views::take(20);

  if (taken_past_end.size() != 6 || taken_past_end.back() != 6) {
    return false;
  }

  auto taken_while = base | tested::ranges::views::take_while(less_than_four{});

  const int expected_take[] = {1, 2, 3};

  if (!equal_range(taken_while, expected_take, 3)) {
    return false;
  }

  auto dropped = base | tested::ranges::views::drop(2);

  if (dropped.size() != 4 || dropped[0] != 3 || dropped[3] != 6) {
    return false;
  }

  int drop_values[] = {1, 2, 3, 4, 5};

  auto dropped_while = input_view{drop_values, drop_values + 5} |
                       tested::ranges::views::drop_while(less_than_four{});

  const int expected_drop[] = {4, 5};

  if (!equal_range(dropped_while, expected_drop, 2)) {
    return false;
  }

  auto reversed = base | tested::ranges::views::reverse;

  if (reversed.size() != 6 || reversed[0] != 6 || reversed[5] != 1 ||
      reversed.end() - reversed.begin() != 6) {
    return false;
  }

  auto common =
      non_common_view{values, values + 6} | tested::ranges::views::common;

  if (common.size() != 6 || common.end() - common.begin() != 6 ||
      common.front() != 1 || common.back() != 6) {
    return false;
  }

  tested::pair<int, long> pairs[] = {{1, 10}, {2, 20}, {3, 30}};

  auto keys = pairs | tested::ranges::views::keys;

  auto pair_values = pairs | tested::ranges::views::values;

  keys[1] = 7;
  pair_values[2] = 99;

  if (pairs[1].first != 7 || pairs[2].second != 99 || keys.size() != 3) {
    return false;
  }

  auto constant = tested::ranges::views::as_const(base);

  static_assert(
      tested::same_as<tested::ranges::range_reference_t<decltype(constant)>,
                      const int &>);

  if (constant.size() != 6 || constant.front() != 1 || constant.back() != 6) {
    return false;
  }

  return true;
}

constexpr bool generated_views_work() {
  auto bounded = tested::ranges::views::iota(-2, 3);

  if (bounded.size() != 5 || bounded.front() != -2 || bounded.back() != 2 ||
      bounded[2] != 0 || bounded.end() - bounded.begin() != 5) {
    return false;
  }

  auto iota_iterator = bounded.begin();

  iota_iterator += 3;

  if (*iota_iterator != 1) {
    return false;
  }

  iota_iterator -= 2;

  if (*iota_iterator != -1) {
    return false;
  }

  auto unbounded = tested::ranges::views::iota(8);

  auto unbounded_iterator = unbounded.begin();

  unbounded_iterator += 20;

  if (*unbounded_iterator != 28 || unbounded_iterator == unbounded.end()) {
    return false;
  }

  auto repeated = tested::ranges::views::repeat(7, 4);

  if (repeated.size() != 4 || repeated[0] != 7 || repeated[3] != 7 ||
      repeated.end() - repeated.begin() != 4) {
    return false;
  }

  auto repeated_iterator = repeated.end();

  --repeated_iterator;

  if (*repeated_iterator != 7) {
    return false;
  }

  int values[] = {10, 20, 30, 40};

  auto counted = tested::ranges::views::counted(values + 1, 2);

  static_assert(tested::same_as<decltype(counted), tested::span<int>>);

  if (counted.size() != 2 || counted.data() != values + 1 || counted[0] != 20 ||
      counted[1] != 30) {
    return false;
  }

  auto strided = values | tested::ranges::views::stride(2);

  if (strided.size() != 2 || strided[0] != 10 || strided[1] != 30 ||
      strided.end() - strided.begin() != 2) {
    return false;
  }

  return true;
}

constexpr bool multi_range_views_work() {
  int left[] = {1, 2, 3};

  int right[] = {10, 20};

  auto zipped = tested::ranges::views::zip(left, right);

  if (zipped.size() != 2 || zipped.end() - zipped.begin() != 2) {
    return false;
  }

  auto first = zipped[0];

  if (tested::get<0>(first) != 1 || tested::get<1>(first) != 10) {
    return false;
  }

  tested::get<0>(first) = 8;

  if (left[0] != 8) {
    return false;
  }

  auto second = zipped[1];

  tested::get<1>(second) = 30;

  if (right[1] != 30) {
    return false;
  }

  tested::ranges::iter_swap(zipped.begin(), zipped.begin() + 1);

  if (left[0] != 2 || left[1] != 8 || right[0] != 30 || right[1] != 10) {
    return false;
  }

  auto enumerated = left | tested::ranges::views::enumerate;

  if (enumerated.size() != 3 || enumerated.end() - enumerated.begin() != 3) {
    return false;
  }

  auto enumerated_value = enumerated[1];

  if (tested::get<0>(enumerated_value) != 1 ||
      tested::get<1>(enumerated_value) != 8) {
    return false;
  }

  tested::get<1>(enumerated_value) = 12;

  if (left[1] != 12) {
    return false;
  }

  auto sums = tested::ranges::views::zip_transform(sum_values{}, left, right);

  if (sums.size() != 2 || sums[0] != 32 || sums[1] != 22) {
    return false;
  }

  const auto &constant_sums = sums;

  if (constant_sums[1] != 22) {
    return false;
  }

  auto references =
      tested::ranges::views::zip_transform(first_reference{}, left, right);

  references[0] = 50;

  if (left[0] != 50) {
    return false;
  }

  int numbers[] = {1, 2};

  char letters[] = {'a', 'b', 'c'};

  auto product = tested::ranges::views::cartesian_product(numbers, letters);

  if (product.size() != 6 || product.end() - product.begin() != 6) {
    return false;
  }

  auto product_value = product.begin()[4];

  if (tested::get<0>(product_value) != 2 ||
      tested::get<1>(product_value) != 'b') {
    return false;
  }

  const auto &constant_product = product;

  auto constant_value = constant_product.begin()[2];

  if (tested::get<0>(constant_value) != 1 ||
      tested::get<1>(constant_value) != 'c') {
    return false;
  }

  auto unit = tested::ranges::views::cartesian_product();

  if (unit.size() != 1 || unit.begin() == unit.end()) {
    return false;
  }

  auto unit_iterator = unit.begin();

  ++unit_iterator;

  return unit_iterator == unit.end();
}

constexpr bool window_views_work() {
  int values[] = {1, 2, 3, 4, 5};

  auto adjacent = values | tested::ranges::views::adjacent<2>;

  if (adjacent.size() != 4 || adjacent.end() - adjacent.begin() != 4) {
    return false;
  }

  auto adjacent_value = adjacent[1];

  if (tested::get<0>(adjacent_value) != 2 ||
      tested::get<1>(adjacent_value) != 3) {
    return false;
  }

  tested::get<1>(adjacent_value) = 8;

  if (values[2] != 8) {
    return false;
  }

  auto adjacent_sums =
      values | tested::ranges::views::adjacent_transform<2>(sum_values{});

  if (adjacent_sums.size() != 4 || adjacent_sums[0] != 3 ||
      adjacent_sums[1] != 10 || adjacent_sums[2] != 12 ||
      adjacent_sums[3] != 9) {
    return false;
  }

  auto slid = values | tested::ranges::views::slide(3);

  if (slid.size() != 3 || slid.end() - slid.begin() != 3) {
    return false;
  }

  auto first_window = slid[0];

  auto final_window = slid[2];

  if (first_window.size() != 3 || first_window[0] != 1 ||
      first_window[2] != 8 || final_window[0] != 8 || final_window[2] != 5) {
    return false;
  }

  int grouped_values[] = {1, 2, 3, 7, 8, 20};

  auto grouped =
      grouped_values | tested::ranges::views::chunk_by(close_values{});

  auto group = grouped.begin();

  const int first_group[] = {1, 2, 3};

  const int second_group[] = {7, 8};

  const int third_group[] = {20};

  if (!equal_range(*group, first_group, 3)) {
    return false;
  }

  ++group;

  if (!equal_range(*group, second_group, 2)) {
    return false;
  }

  ++group;

  if (!equal_range(*group, third_group, 1)) {
    return false;
  }

  ++group;

  if (group != grouped.end()) {
    return false;
  }

  --group;

  if (!equal_range(*group, third_group, 1)) {
    return false;
  }

  auto chunked = values | tested::ranges::views::chunk(2);

  if (chunked.size() != 3 || sum_range(chunked[0]) != 3 ||
      sum_range(chunked[1]) != 12 || sum_range(chunked[2]) != 5) {
    return false;
  }

  int input_values[] = {1, 2, 3, 4, 5};

  auto input_chunks = input_view{input_values, input_values + 5} |
                      tested::ranges::views::chunk(2);

  auto input_chunk = input_chunks.begin();

  if (sum_range(*input_chunk) != 3) {
    return false;
  }

  ++input_chunk;

  if (sum_range(*input_chunk) != 7) {
    return false;
  }

  ++input_chunk;

  if (sum_range(*input_chunk) != 5) {
    return false;
  }

  ++input_chunk;

  return input_chunk == input_chunks.end();
}

constexpr bool flattening_views_work() {
  int first_values[] = {1, 2};

  int second_values[] = {3};

  inner_view inners[] = {{},
                         {first_values, first_values + 2},
                         {second_values, second_values + 1},
                         {}};

  outer_view outer{inners, inners + 4};

  auto joined = outer | tested::ranges::views::join;

  auto joined_iterator = joined.begin();

  if (*joined_iterator != 1) {
    return false;
  }

  ++joined_iterator;

  if (*joined_iterator != 2) {
    return false;
  }

  ++joined_iterator;

  if (*joined_iterator != 3) {
    return false;
  }

  ++joined_iterator;

  if (joined_iterator != joined.end()) {
    return false;
  }

  --joined_iterator;

  if (*joined_iterator != 3) {
    return false;
  }

  const auto &constant_joined = joined;

  if (*constant_joined.begin() != 1) {
    return false;
  }

  int split_values[] = {1, 0, 2, 0, 0, 3, 0};

  auto split = split_values | tested::ranges::views::split(0);

  auto part = split.begin();

  const int one[] = {1};

  const int two[] = {2};

  const int three[] = {3};

  if (!equal_range(*part, one, 1)) {
    return false;
  }

  ++part;

  if (!equal_range(*part, two, 1)) {
    return false;
  }

  ++part;

  if (!equal_range(*part, nullptr, 0)) {
    return false;
  }

  ++part;

  if (!equal_range(*part, three, 1)) {
    return false;
  }

  ++part;

  if (!equal_range(*part, nullptr, 0)) {
    return false;
  }

  ++part;

  if (part != split.end()) {
    return false;
  }

  int lazy_values[] = {0, 1, 0, 2, 0};

  auto lazy = tested::ranges::views::lazy_split(lazy_values, 0);

  auto lazy_part = lazy.begin();

  if (!equal_range(*lazy_part, nullptr, 0)) {
    return false;
  }

  ++lazy_part;

  if (!equal_range(*lazy_part, one, 1)) {
    return false;
  }

  ++lazy_part;

  if (!equal_range(*lazy_part, two, 1)) {
    return false;
  }

  ++lazy_part;

  if (!equal_range(*lazy_part, nullptr, 0)) {
    return false;
  }

  ++lazy_part;

  return lazy_part == lazy.end();
}

constexpr bool cached_paths_work() {
  int filter_values[] = {1, 2, 3, 4};

  int filter_calls = 0;

  auto filtered = pointer_view{filter_values, filter_values + 4} |
                  tested::ranges::views::filter(counting_even{&filter_calls});

  auto first_filter_begin = filtered.begin();

  if (*first_filter_begin != 2 || filter_calls == 0) {
    return false;
  }

  const int calls_after_filter_begin = filter_calls;

  auto second_filter_begin = filtered.begin();

  if (second_filter_begin != first_filter_begin ||
      filter_calls != calls_after_filter_begin) {
    return false;
  }

  int drop_values[] = {1, 2, 3, 4, 5};

  int drop_calls = 0;

  auto dropped =
      pointer_view{drop_values, drop_values + 5} |
      tested::ranges::views::drop_while(counting_less_than{&drop_calls, 4});

  auto first_drop_begin = dropped.begin();

  if (*first_drop_begin != 4 || drop_calls == 0) {
    return false;
  }

  const int calls_after_drop_begin = drop_calls;

  auto second_drop_begin = dropped.begin();

  if (second_drop_begin != first_drop_begin ||
      drop_calls != calls_after_drop_begin) {
    return false;
  }

  int reverse_values[] = {1, 2, 3, 4};

  int comparisons = 0;

  auto reversed = tested::ranges::views::reverse(
      cached_reverse_view{reverse_values, reverse_values + 4, &comparisons});

  auto first_reverse_begin = reversed.begin();

  if (*first_reverse_begin != 4 || comparisons == 0) {
    return false;
  }

  const int comparisons_after_begin = comparisons;

  auto second_reverse_begin = reversed.begin();

  if (second_reverse_begin != first_reverse_begin ||
      comparisons != comparisons_after_begin) {
    return false;
  }

  return true;
}

static_assert(foundational_views_work());

static_assert(classic_views_work());

static_assert(generated_views_work());

static_assert(multi_range_views_work());

static_assert(window_views_work());

static_assert(flattening_views_work());

/*
 * These paths populate non-propagating caches. Replacement mode receives
 * the compiler's constexpr placement-new treatment for std::construct_at.
 * Normal FTL mode still executes the same checks at runtime.
 */
#ifdef FTL_REPLACE_STL

static_assert(cached_paths_work());

#endif

bool ftl_test() {
  return foundational_views_work() && classic_views_work() &&
         generated_views_work() && multi_range_views_work() &&
         window_views_work() && flattening_views_work() && cached_paths_work();
}