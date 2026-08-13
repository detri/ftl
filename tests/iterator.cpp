#ifdef FTL_REPLACE_STL
#include <iterator>
namespace tested = std;
#else
#include <ftl/iterator>
namespace tested = ftl;
#endif

static_assert(tested::contiguous_iterator<int *>);
static_assert(tested::random_access_iterator<int *>);
static_assert(tested::same_as<tested::iter_value_t<const int *>, int>);
static_assert(
    tested::same_as<tested::iter_difference_t<int *>, tested::ptrdiff_t>);
static_assert(
    tested::same_as<tested::const_iterator<const int *>, const int *>);
static_assert(tested::same_as<tested::const_iterator<int *>,
                              tested::basic_const_iterator<int *>>);

static_assert(tested::is_same_v<
              tested::iterator_traits<const tested::byte *>::iterator_concept,
              tested::contiguous_iterator_tag>);

static_assert(
    tested::is_same_v<tested::detail::iter_concept_t<const tested::byte *>,
                      tested::contiguous_iterator_tag>);

static_assert(tested::contiguous_iterator<const tested::byte *>);

struct output {
  using value_type = int;
  int values[4]{};
  int count{};
  constexpr void push_back(int value) { values[count++] = value; }
};

template <class T>
concept has_iterator_category = requires { typename T::iterator_category; };

struct concept_only_input_iterator {
  using value_type = int;
  using difference_type = tested::ptrdiff_t;
  using iterator_concept = tested::input_iterator_tag;

  int *current = nullptr;

  constexpr int &operator*() const noexcept { return *current; }

  constexpr concept_only_input_iterator &operator++() noexcept {
    ++current;
    return *this;
  }

  constexpr void operator++(int) noexcept { ++current; }

  friend constexpr bool operator==(concept_only_input_iterator,
                                   concept_only_input_iterator) = default;
};

static_assert(
    !has_iterator_category<tested::move_iterator<concept_only_input_iterator>>);

static_assert(
    tested::is_same_v<typename tested::move_iterator<int *>::iterator_concept,
                      tested::random_access_iterator_tag>);

static_assert(
    tested::is_same_v<typename tested::move_iterator<int *>::iterator_category,
                      tested::random_access_iterator_tag>);

static_assert(tested::random_access_iterator<tested::move_iterator<int *>>);

static_assert(!tested::contiguous_iterator<tested::move_iterator<int *>>);

struct bad_const_read_iterator {
  using value_type = int;
  using difference_type = tested::ptrdiff_t;

  int *current;

  int &operator*() { return *current; }
  long operator*() const { return *current; }
  bad_const_read_iterator &operator++();
  void operator++(int);
};

static_assert(!tested::indirectly_readable<bad_const_read_iterator>);

struct swappable_proxy {
  int *value;

  constexpr operator int() const { return *value; }

  friend constexpr void swap(swappable_proxy left, swappable_proxy right) {
    const int temporary = *left.value;
    *left.value = *right.value;
    *right.value = temporary;
  }
};

struct proxy_iterator {
  using value_type = int;
  using difference_type = tested::ptrdiff_t;

  int *current;

  constexpr swappable_proxy operator*() const { return {current}; }
};

struct legacy_random_iterator {
  using value_type = int;
  using difference_type = tested::ptrdiff_t;
  using pointer = int *;
  using reference = int &;
  using iterator_category = tested::random_access_iterator_tag;

  int *current;

  constexpr int &operator*() const { return *current; }
  constexpr legacy_random_iterator &operator++() {
    ++current;
    return *this;
  }
  constexpr void operator++(int) { ++current; }
  constexpr legacy_random_iterator &operator--() {
    --current;
    return *this;
  }
  constexpr legacy_random_iterator &operator+=(difference_type offset) {
    current += offset;
    return *this;
  }

  friend constexpr difference_type operator-(legacy_random_iterator left,
                                             legacy_random_iterator right) {
    return left.current - right.current;
  }
  friend constexpr bool operator==(legacy_random_iterator,
                                   legacy_random_iterator) = default;
};

static_assert(!tested::random_access_iterator<legacy_random_iterator>);

template <bool HasAddressCustomization>
struct fancy_contiguous_iterator {
  using value_type = int;
  using difference_type = tested::ptrdiff_t;
  using iterator_concept = tested::contiguous_iterator_tag;
  using iterator_category = tested::random_access_iterator_tag;
  using pointer = int *;
  using reference = int &;

  int *current = nullptr;

  constexpr int &operator*() const { return *current; }
  constexpr int &operator[](difference_type offset) const {
    return current[offset];
  }
  constexpr fancy_contiguous_iterator &operator++() {
    ++current;
    return *this;
  }
  constexpr fancy_contiguous_iterator operator++(int) {
    auto copy = *this;
    ++*this;
    return copy;
  }
  constexpr fancy_contiguous_iterator &operator--() {
    --current;
    return *this;
  }
  constexpr fancy_contiguous_iterator operator--(int) {
    auto copy = *this;
    --*this;
    return copy;
  }
  constexpr fancy_contiguous_iterator &operator+=(difference_type offset) {
    current += offset;
    return *this;
  }
  constexpr fancy_contiguous_iterator &operator-=(difference_type offset) {
    current -= offset;
    return *this;
  }

  friend constexpr fancy_contiguous_iterator
  operator+(fancy_contiguous_iterator value, difference_type offset) {
    return value += offset;
  }
  friend constexpr fancy_contiguous_iterator
  operator+(difference_type offset, fancy_contiguous_iterator value) {
    return value += offset;
  }
  friend constexpr fancy_contiguous_iterator
  operator-(fancy_contiguous_iterator value, difference_type offset) {
    return value -= offset;
  }
  friend constexpr difference_type
  operator-(fancy_contiguous_iterator left,
            fancy_contiguous_iterator right) {
    return left.current - right.current;
  }
  friend constexpr bool operator==(fancy_contiguous_iterator,
                                   fancy_contiguous_iterator) = default;
  friend constexpr bool operator<(fancy_contiguous_iterator left,
                                  fancy_contiguous_iterator right) {
    return left.current < right.current;
  }
  friend constexpr bool operator>(fancy_contiguous_iterator left,
                                  fancy_contiguous_iterator right) {
    return right < left;
  }
  friend constexpr bool operator<=(fancy_contiguous_iterator left,
                                   fancy_contiguous_iterator right) {
    return !(right < left);
  }
  friend constexpr bool operator>=(fancy_contiguous_iterator left,
                                   fancy_contiguous_iterator right) {
    return !(left < right);
  }
};

using unaddressable_contiguous_iterator = fancy_contiguous_iterator<false>;
using customized_contiguous_iterator = fancy_contiguous_iterator<true>;

#ifdef FTL_REPLACE_STL
namespace std {
#else
namespace ftl {
#endif
template <>
struct pointer_traits<::customized_contiguous_iterator> {
  static constexpr int *
  to_address(const ::customized_contiguous_iterator &value) noexcept {
    return value.current;
  }
};
}

static_assert(!tested::contiguous_iterator<unaddressable_contiguous_iterator>);
static_assert(tested::contiguous_iterator<customized_contiguous_iterator>);

struct pointer_sentinel {
  int *last = nullptr;

  friend constexpr bool operator==(int *iterator, pointer_sentinel sentinel) {
    return iterator == sentinel.last;
  }
};

using pointer_common_iterator = tested::common_iterator<int *, pointer_sentinel>;

constexpr bool common_iterator_copy_is_constexpr() {
  int values[]{1};
  pointer_common_iterator original{values};
  pointer_common_iterator copy{original};
  return *copy == 1;
}

static_assert(common_iterator_copy_is_constexpr());

struct prvalue_iterator {
  using value_type = int;
  using difference_type = tested::ptrdiff_t;

  int *current = nullptr;

  constexpr int operator*() const { return *current; }
  constexpr prvalue_iterator &operator++() {
    ++current;
    return *this;
  }
  constexpr void operator++(int) { ++current; }

  friend constexpr bool operator==(prvalue_iterator,
                                   prvalue_iterator) = default;
};

struct prvalue_sentinel {
  int *last = nullptr;

  friend constexpr bool operator==(prvalue_iterator iterator,
                                   prvalue_sentinel sentinel) {
    return iterator.current == sentinel.last;
  }
};

using prvalue_common_iterator =
    tested::common_iterator<prvalue_iterator, prvalue_sentinel>;

inline bool throw_common_iterator_copy = false;

struct throwing_common_input {
  using value_type = int;
  using difference_type = tested::ptrdiff_t;

  int *current = nullptr;

  throwing_common_input() = default;
  explicit throwing_common_input(int *value) : current(value) {}
  throwing_common_input(const throwing_common_input &other)
      : current(other.current) {
    if (throw_common_iterator_copy)
      throw 1;
  }
  throwing_common_input(throwing_common_input &&) = default;
  throwing_common_input &operator=(const throwing_common_input &) = default;
  throwing_common_input &operator=(throwing_common_input &&) = default;

  int &operator*() const { return *current; }
  throwing_common_input &operator++() {
    ++current;
    return *this;
  }
  void operator++(int) { ++current; }

  friend bool operator==(throwing_common_input,
                         throwing_common_input) = default;
};

struct throwing_common_sentinel {
  int *last = nullptr;

  friend bool operator==(const throwing_common_input &iterator,
                         throwing_common_sentinel sentinel) {
    return iterator.current == sentinel.last;
  }
};

constexpr bool iterator_works() {
  int values[]{1, 2, 3, 4};
  auto p = tested::next(values, 2);
  if (*p != 3 || tested::distance(values, p) != 2)
    return false;
  tested::ranges::advance(p, -1);
  if (*p != 2 || *tested::rbegin(values) != 4)
    return false;
  tested::ranges::iter_swap(values, values + 3);

  int proxy_values[]{5, 6};
  tested::ranges::iter_swap(proxy_iterator{proxy_values},
                            proxy_iterator{proxy_values + 1});
  if (proxy_values[0] != 6 || proxy_values[1] != 5)
    return false;

  legacy_random_iterator legacy_first{values};
  legacy_random_iterator legacy_last{values + 4};
  tested::advance(legacy_first, 3);
  if (*legacy_first != 1 || tested::distance(legacy_first, legacy_last) != 1)
    return false;

  pointer_common_iterator default_common;
  pointer_common_iterator sentinel_common{pointer_sentinel{values + 1}};
  if (default_common == sentinel_common)
    return false;

  prvalue_common_iterator prvalue{prvalue_iterator{values}};
  if (*prvalue.operator->().operator->() != 4 || *prvalue++ != 4 ||
      *prvalue != 2)
    return false;
  tested::reverse_iterator reverse{values + 4};
  tested::move_iterator moving{values};
  tested::counted_iterator counted{values, 4};
  tested::basic_const_iterator constant{values};
  output out;
  auto sink = tested::back_inserter(out);
  *sink++ = *constant;
  return *reverse == 1 && *moving == 4 && counted.count() == 4 &&
         out.values[0] == 4 && values[0] == 4 && values[3] == 1 &&
         tested::size(values) == 4 && !tested::empty(values);
}
static_assert(iterator_works());

bool common_iterator_exception_state_works() {
  int values[]{1, 2};
  using iterator =
      tested::common_iterator<throwing_common_input, throwing_common_sentinel>;

  iterator target{throwing_common_sentinel{values + 2}};
  iterator source{throwing_common_input{values}};
  throw_common_iterator_copy = true;

  try {
    target = source;
    return false;
  } catch (...) {
  }

  throw_common_iterator_copy = false;
  target = source;
  return *target == 1;
}

bool ftl_test() {
  return iterator_works() && common_iterator_exception_state_works();
}
