#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/initializer_list>
#include <ftl/iterator>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

#ifndef __cpp_lib_ranges_as_rvalue
#error "__cpp_lib_ranges_as_rvalue is missing"
#endif

static_assert(__cpp_lib_ranges_as_rvalue == 202207L);

#ifndef __cpp_lib_ranges_to_container
#error "__cpp_lib_ranges_to_container is missing"
#endif

static_assert(__cpp_lib_ranges_to_container == 202202L);

template <class T, tested::size_t Capacity = 16> struct to_buffer_base {
  using value_type = T;

  using size_type = tested::size_t;

  T storage[Capacity]{};

  size_type count = 0;

  constexpr T *begin() noexcept { return storage; }

  constexpr const T *begin() const noexcept { return storage; }

  constexpr T *end() noexcept { return storage + count; }

  constexpr const T *end() const noexcept { return storage + count; }

  constexpr size_type size() const noexcept { return count; }

protected:
  template <class U> constexpr T &append_value(U &&value) {
    const auto index = count++;

    storage[index] = T(static_cast<U &&>(value));

    return storage[index];
  }

  template <tested::ranges::input_range R>
  constexpr void append_range(R &&range) {
    auto current = tested::ranges::begin(range);

    const auto bound = tested::ranges::end(range);

    for (; current != bound; ++current) {
      append_value(*current);
    }
  }
};

template <class T> struct to_sequence : to_buffer_base<T> {
  using base_type = to_buffer_base<T>;

  using typename base_type::size_type;

  size_type reserved = 0;

  constexpr void reserve(size_type count) noexcept { reserved = count; }

  constexpr size_type capacity() const noexcept { return 16; }

  constexpr size_type max_size() const noexcept { return 16; }

  template <class U> constexpr T &emplace_back(U &&value) {
    return this->append_value(static_cast<U &&>(value));
  }
};

struct direct_to_container : to_buffer_base<int> {
  int argument = 0;

  template <tested::ranges::input_range R>
  constexpr direct_to_container(R &&range, int value) : argument(value) {
    this->append_range(static_cast<R &&>(range));
  }
};

struct from_range_to_container : to_buffer_base<int> {
  int argument = 0;

  template <tested::ranges::input_range R>
  constexpr from_range_to_container(tested::from_range_t, R &&range, int value)
      : argument(value) {
    this->append_range(static_cast<R &&>(range));
  }
};

struct iterator_to_container : to_buffer_base<int> {
  int argument = 0;

  template <tested::input_iterator I, tested::sentinel_for<I> S>
  constexpr iterator_to_container(I first, S last, int value)
      : argument(value) {
    for (; first != last; ++first) {
      this->append_value(*first);
    }
  }
};

struct append_priority_container : to_buffer_base<int> {
  using size_type = tested::size_t;

  int route = 0;

  size_type reserved = 0;

  constexpr void reserve(size_type count) noexcept { reserved = count; }

  constexpr size_type capacity() const noexcept { return 16; }

  constexpr size_type max_size() const noexcept { return 16; }

  template <class U> constexpr int &emplace_back(U &&value) {
    route = 1;

    return this->append_value(static_cast<U &&>(value));
  }

  template <class U> constexpr void push_back(U &&value) {
    route = 2;

    this->append_value(static_cast<U &&>(value));
  }

  template <class U> constexpr int *emplace_hint(int *, U &&value) {
    route = 3;

    this->append_value(static_cast<U &&>(value));

    return this->end() - 1;
  }

  template <class U> constexpr int *insert(int *, U &&value) {
    route = 4;

    this->append_value(static_cast<U &&>(value));

    return this->end() - 1;
  }
};

struct push_back_to_container : to_buffer_base<int> {
  template <class U> constexpr void push_back(U &&value) {
    this->append_value(static_cast<U &&>(value));
  }
};

struct emplace_hint_to_container : to_buffer_base<int> {
  template <class U> constexpr int *emplace_hint(int *, U &&value) {
    this->append_value(static_cast<U &&>(value));

    return this->end() - 1;
  }
};

struct insert_to_container : to_buffer_base<int> {
  template <class U> constexpr int *insert(int *, U &&value) {
    this->append_value(static_cast<U &&>(value));

    return this->end() - 1;
  }
};

template <class T> struct deduced_to_container : to_buffer_base<T> {
  template <tested::ranges::input_range R>
  constexpr explicit deduced_to_container(R &&range) {
    this->append_range(static_cast<R &&>(range));
  }
};

template <tested::ranges::input_range R>
deduced_to_container(R &&)
    -> deduced_to_container<tested::ranges::range_value_t<R>>;

struct member_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept { return first; }

  constexpr const int *begin() const noexcept { return first; }

  constexpr int *end() noexcept { return last; }

  constexpr const int *end() const noexcept { return last; }

  constexpr tested::size_t size() const noexcept {
    return static_cast<tested::size_t>(last - first);
  }

  constexpr bool empty() const noexcept { return first == last; }

  constexpr int *data() noexcept { return first; }

  constexpr const int *data() const noexcept { return first; }
};

struct subtraction_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept { return first; }

  constexpr const int *begin() const noexcept { return first; }

  constexpr int *end() noexcept { return last; }

  constexpr const int *end() const noexcept { return last; }
};

struct iterator_empty_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept { return first; }

  constexpr int *end() noexcept { return last; }
};

struct borrowed_member_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept { return first; }

  constexpr int *end() noexcept { return last; }
};

struct explicitly_unsized_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr tested::size_t size() const noexcept { return 999; }

  constexpr int *begin() noexcept { return first; }

  constexpr int *end() noexcept { return last; }
};

struct move_only_range {
  int *first = nullptr;
  int *last = nullptr;

  constexpr move_only_range() = default;

  constexpr move_only_range(int *begin_value, int *end_value) noexcept
      : first(begin_value), last(end_value) {}

  move_only_range(const move_only_range &) = delete;

  move_only_range &operator=(const move_only_range &) = delete;

  constexpr move_only_range(move_only_range &&) noexcept = default;

  constexpr move_only_range &operator=(move_only_range &&) noexcept = default;

  constexpr int *begin() noexcept { return first; }

  constexpr const int *begin() const noexcept { return first; }

  constexpr int *end() noexcept { return last; }

  constexpr const int *end() const noexcept { return last; }

  constexpr tested::size_t size() const noexcept {
    return static_cast<tested::size_t>(last - first);
  }

  constexpr int *data() noexcept { return first; }

  constexpr const int *data() const noexcept { return first; }
};

struct interface_view : tested::ranges::view_interface<interface_view> {
  int *first = nullptr;
  int *last = nullptr;

  constexpr interface_view() = default;

  constexpr interface_view(int *begin_value, int *end_value) noexcept
      : first(begin_value), last(end_value) {}

  constexpr int *begin() noexcept { return first; }

  constexpr const int *begin() const noexcept { return first; }

  constexpr int *end() noexcept { return last; }

  constexpr const int *end() const noexcept { return last; }
};

struct base_view : tested::ranges::view_base {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept { return first; }

  constexpr int *end() noexcept { return last; }
};

struct opted_view {
  int *first = nullptr;
  int *last = nullptr;

  constexpr int *begin() noexcept { return first; }

  constexpr int *end() noexcept { return last; }
};

struct nullary_value {
  [[nodiscard]]
  constexpr int operator()() const noexcept {
    return 42;
  }
};

constexpr member_range evaluated_range(int *first, int *last,
                                       int &evaluations) {
  ++evaluations;

  return {first, last};
}

constexpr nullary_value evaluated_function(int &evaluations) {
  ++evaluations;

  return {};
}

using tuple_array = tested::tuple<int, long>[2];

using tuple_array_view = tested::ranges::ref_view<tuple_array>;

static_assert(
    tested::is_same_v<tested::ranges::keys_view<tuple_array_view>,
                      tested::ranges::elements_view<tuple_array_view, 0>>);

static_assert(
    tested::is_same_v<tested::ranges::values_view<tuple_array_view>,
                      tested::ranges::elements_view<tuple_array_view, 1>>);

static_assert(
    tested::is_same_v<
        tested::remove_cv_t<decltype(tested::ranges::views::pairwise)>,
        tested::remove_cv_t<decltype(tested::ranges::views::adjacent<2>)>>);

static_assert(tested::is_same_v<
              tested::remove_cv_t<
                  decltype(tested::ranges::views::pairwise_transform)>,
              tested::remove_cv_t<
                  decltype(tested::ranges::views::adjacent_transform<2>)>>);

static_assert(tested::is_same_v<decltype(tested::from_range),
                                const tested::from_range_t>);

namespace adl_test {
struct range {
  int *first = nullptr;
  int *last = nullptr;
};

constexpr int *begin(range &value) noexcept { return value.first; }

constexpr const int *begin(const range &value) noexcept { return value.first; }

constexpr int *end(range &value) noexcept { return value.last; }

constexpr const int *end(const range &value) noexcept { return value.last; }

constexpr tested::size_t size(const range &value) noexcept {
  return static_cast<tested::size_t>(value.last - value.first);
}
} // namespace adl_test

namespace reverse_adl_test {
struct range {
  int *first = nullptr;
  int *last = nullptr;
};

constexpr auto rbegin(range &value) noexcept {
  return tested::reverse_iterator<int *>(value.last);
}

constexpr auto rend(range &value) noexcept {
  return tested::reverse_iterator<int *>(value.first);
}
} // namespace reverse_adl_test

#ifdef FTL_REPLACE_STL

namespace std::ranges {
template <>
inline constexpr bool enable_borrowed_range<::borrowed_member_range> = true;

template <>
inline constexpr bool disable_sized_range<::explicitly_unsized_range> = true;

template <> inline constexpr bool enable_view<::opted_view> = true;
} // namespace std::ranges

#else

namespace ftl::ranges {
template <>
inline constexpr bool enable_borrowed_range<::borrowed_member_range> = true;

template <>
inline constexpr bool disable_sized_range<::explicitly_unsized_range> = true;

template <> inline constexpr bool enable_view<::opted_view> = true;
} // namespace ftl::ranges

#endif

template <class T>
concept can_begin_rvalue = requires { tested::ranges::begin(T{}); };

template <class T>
concept can_end_rvalue = requires { tested::ranges::end(T{}); };

template <class T>
concept can_data_rvalue = requires { tested::ranges::data(T{}); };

template <class T>
concept can_ref_view =
    requires(T &&value) { tested::ranges::ref_view{static_cast<T &&>(value)}; };

static_assert(tested::ranges::range<member_range>);
static_assert(tested::ranges::input_range<member_range>);
static_assert(tested::ranges::forward_range<member_range>);
static_assert(tested::ranges::bidirectional_range<member_range>);
static_assert(tested::ranges::random_access_range<member_range>);
static_assert(tested::ranges::contiguous_range<member_range>);
static_assert(tested::ranges::common_range<member_range>);
static_assert(tested::ranges::sized_range<member_range>);
static_assert(tested::ranges::sized_range<subtraction_range>);
static_assert(tested::ranges::output_range<member_range, int>);

static_assert(tested::ranges::borrowed_range<member_range &>);
static_assert(!tested::ranges::borrowed_range<member_range>);
static_assert(tested::ranges::borrowed_range<borrowed_member_range>);

static_assert(!can_begin_rvalue<member_range>);
static_assert(!can_end_rvalue<member_range>);
static_assert(!can_data_rvalue<member_range>);

static_assert(can_begin_rvalue<borrowed_member_range>);
static_assert(can_end_rvalue<borrowed_member_range>);
static_assert(can_data_rvalue<borrowed_member_range>);

static_assert(
    tested::is_same_v<tested::ranges::iterator_t<member_range>, int *>);

static_assert(
    tested::is_same_v<tested::ranges::sentinel_t<member_range>, int *>);

static_assert(tested::is_same_v<tested::ranges::const_iterator_t<member_range>,
                                tested::const_iterator<int *>>);

static_assert(tested::is_same_v<tested::ranges::const_sentinel_t<member_range>,
                                tested::const_sentinel<int *>>);

static_assert(tested::is_same_v<decltype(tested::ranges::cbegin(
                                    tested::declval<member_range &>())),
                                const int *>);

static_assert(tested::is_same_v<
              decltype(tested::ranges::cend(tested::declval<member_range &>())),
              const int *>);

static_assert(
    tested::is_same_v<tested::ranges::range_value_t<member_range>, int>);

static_assert(
    tested::is_same_v<tested::ranges::range_reference_t<member_range>, int &>);

static_assert(
    tested::is_same_v<tested::ranges::range_const_reference_t<member_range>,
                      const int &>);

static_assert(tested::detail::has_common_ref<const int &, int &>);
static_assert(tested::detail::has_common_ref<const int &&, int &>);

static_assert(tested::is_same_v<
              tested::ranges::range_rvalue_reference_t<member_range>, int &&>);

static_assert(
    tested::is_same_v<tested::ranges::range_difference_t<member_range>,
                      tested::ptrdiff_t>);

static_assert(
    tested::is_same_v<tested::ranges::borrowed_iterator_t<member_range>,
                      tested::ranges::dangling>);

static_assert(tested::is_same_v<
              tested::ranges::borrowed_iterator_t<member_range &>, int *>);

static_assert(tested::is_default_constructible_v<tested::ranges::dangling>);

static_assert(
    tested::is_constructible_v<tested::ranges::dangling, int *, int *>);

static_assert(tested::ranges::view<interface_view>);
static_assert(tested::ranges::view<base_view>);
static_assert(tested::ranges::view<opted_view>);
static_assert(!tested::ranges::view<member_range>);

static_assert(tested::ranges::viewable_range<member_range &>);
static_assert(tested::ranges::viewable_range<move_only_range>);
static_assert(!tested::ranges::viewable_range<tested::initializer_list<int>>);

static_assert(can_ref_view<member_range &>);
static_assert(!can_ref_view<member_range>);

static_assert(tested::ranges::view<tested::ranges::ref_view<member_range>>);

static_assert(
    tested::ranges::borrowed_range<tested::ranges::ref_view<member_range>>);

static_assert(
    tested::ranges::view<tested::ranges::owning_view<move_only_range>>);

static_assert(!tested::is_copy_constructible_v<
              tested::ranges::owning_view<move_only_range>>);

static_assert(tested::is_move_constructible_v<
              tested::ranges::owning_view<move_only_range>>);

static_assert(tested::ranges::view<tested::ranges::empty_view<int>>);

static_assert(tested::ranges::borrowed_range<tested::ranges::empty_view<int>>);

static_assert(tested::ranges::view<tested::ranges::single_view<int>>);

static_assert(tested::is_same_v<tested::ranges::views::all_t<member_range &>,
                                tested::ranges::ref_view<member_range>>);

static_assert(tested::is_same_v<tested::ranges::views::all_t<move_only_range>,
                                tested::ranges::owning_view<move_only_range>>);

static_assert(tested::is_same_v<tested::ranges::views::all_t<interface_view>,
                                interface_view>);

static_assert(
    tested::contiguous_iterator<tested::ranges::iterator_t<interface_view>>);

static_assert(tested::is_same_v<decltype(tested::ranges::data(
                                    tested::declval<interface_view &>())),
                                int *>);

static_assert(tested::is_same_v<decltype(tested::ranges::data(
                                    tested::declval<const interface_view &>())),
                                const int *>);

static_assert(
    tested::is_same_v<tested::add_pointer_t<
                          tested::ranges::range_reference_t<interface_view>>,
                      int *>);

static_assert(tested::ranges::contiguous_range<interface_view>);
static_assert(tested::ranges::contiguous_range<const interface_view>);

constexpr bool range_iterator_operations_work() {
  int values[] = {1, 2, 3, 4};
  int *iterator = values;

  tested::ranges::advance(iterator, 2);
  if (iterator != values + 2)
    return false;

  tested::ranges::advance(iterator, -1);
  if (iterator != values + 1)
    return false;

  tested::ranges::advance(iterator, values + 4);
  if (iterator != values + 4)
    return false;

  iterator = values;

  auto remainder =
      tested::ranges::advance(iterator, tested::ptrdiff_t{8}, values + 4);

  if (iterator != values + 4 || remainder != 4)
    return false;

  if (tested::ranges::distance(values, values + 4) != 4)
    return false;

  member_range range{values, values + 4};

  if (tested::ranges::distance(range) != 4)
    return false;

  if (tested::ranges::next(values) != values + 1)
    return false;

  if (tested::ranges::next(values, 3) != values + 3)
    return false;

  if (tested::ranges::next(values, values + 4) != values + 4)
    return false;

  if (tested::ranges::prev(values + 4) != values + 3)
    return false;

  if (tested::ranges::prev(values + 4, 2) != values + 2)
    return false;

  return true;
}

static_assert(range_iterator_operations_work());

constexpr bool ranges_constexpr_works() {
  int values[] = {1, 2, 3, 4};

  if (tested::ranges::begin(values) != values)
    return false;

  if (tested::ranges::end(values) != values + 4)
    return false;

  if (tested::ranges::size(values) != 4)
    return false;

  if (tested::ranges::ssize(values) != 4)
    return false;

  if (tested::ranges::empty(values))
    return false;

  if (tested::ranges::data(values) != values)
    return false;

  if (tested::ranges::cdata(values) != values)
    return false;

  member_range member{values, values + 4};

  if (tested::ranges::begin(member) != values)
    return false;

  if (tested::ranges::end(member) != values + 4)
    return false;

  if (tested::ranges::size(member) != 4)
    return false;

  if (tested::ranges::ssize(member) != 4)
    return false;

  if (tested::ranges::empty(member))
    return false;

  if (tested::ranges::data(member) != values)
    return false;

  if (tested::ranges::cdata(member) != values)
    return false;

  if (tested::ranges::cbegin(member) != values)
    return false;

  if (tested::ranges::cend(member) != values + 4)
    return false;

  const member_range constant_member{values, values + 4};

  if (tested::ranges::begin(constant_member) != values)
    return false;

  if (tested::ranges::end(constant_member) != values + 4)
    return false;

  subtraction_range subtraction{values, values + 4};

  if (tested::ranges::size(subtraction) != 4)
    return false;

  if (tested::ranges::data(subtraction) != values)
    return false;

  explicitly_unsized_range disabled{values, values + 4};

  // disable_sized_range suppresses the member size(), but pointer subtraction
  // still makes this a sized range.
  if (tested::ranges::size(disabled) != 4)
    return false;

  iterator_empty_range empty_range{values, values};

  if (!tested::ranges::empty(empty_range))
    return false;

  iterator_empty_range nonempty_range{values, values + 4};

  if (tested::ranges::empty(nonempty_range))
    return false;

  adl_test::range adl{values, values + 4};

  if (tested::ranges::begin(adl) != values)
    return false;

  if (tested::ranges::end(adl) != values + 4)
    return false;

  if (tested::ranges::size(adl) != 4)
    return false;

  if (*tested::ranges::rbegin(values) != 4)
    return false;

  if (*(tested::ranges::rend(values) - 1) != 1)
    return false;

  if (*tested::ranges::crbegin(values) != 4)
    return false;

  if (*(tested::ranges::crend(values) - 1) != 1)
    return false;

  tested::ranges::ref_view reference{member};

  if (&reference.base() != &member)
    return false;

  if (reference.begin() != values)
    return false;

  if (reference.end() != values + 4)
    return false;

  if (reference.size() != 4)
    return false;

  if (reference.data() != values)
    return false;

  auto all_reference = tested::ranges::views::all(member);

  static_assert(tested::is_same_v<decltype(all_reference),
                                  tested::ranges::ref_view<member_range>>);

  if (all_reference.begin() != values)
    return false;

  if (all_reference.end() != values + 4)
    return false;

  interface_view existing{values, values + 4};

  auto all_existing =
      tested::ranges::views::all(static_cast<interface_view &&>(existing));

  static_assert(tested::is_same_v<decltype(all_existing), interface_view>);

  if (all_existing.front() != 1)
    return false;

  if (all_existing.back() != 4)
    return false;

  if (all_existing[2] != 3)
    return false;

  if (all_existing.size() != 4)
    return false;

  if (all_existing.data() != values)
    return false;

  if (!static_cast<bool>(all_existing))
    return false;

  constexpr auto empty = tested::ranges::views::empty<int>;

  if (!empty.empty())
    return false;

  if (empty.size() != 0)
    return false;

  if (empty.begin() != nullptr)
    return false;

  if (empty.end() != nullptr)
    return false;

  if (empty.data() != nullptr)
    return false;

  auto single = tested::ranges::views::single(42);

  if (single.empty())
    return false;

  if (single.size() != 1)
    return false;

  if (*single.begin() != 42)
    return false;

  if (single.front() != 42)
    return false;

  if (single.back() != 42)
    return false;

  if (single[0] != 42)
    return false;

  tested::ranges::subrange sub(values, values + 4);

  if (sub.begin() != values)
    return false;

  if (sub.end() != values + 4)
    return false;

  if (sub.size() != 4)
    return false;

  auto next = sub.next(2);

  if (next.begin() != values + 2)
    return false;

  auto prev = next.prev(1);

  if (prev.begin() != values + 1)
    return false;

  return true;
}

static_assert(ranges_constexpr_works());

static_assert(tested::ranges::view<tested::ranges::subrange<int *>>);

static_assert(tested::ranges::borrowed_range<tested::ranges::subrange<int *>>);

static_assert(tested::is_same_v<
              decltype(tested::ranges::get<0>(
                  tested::declval<const tested::ranges::subrange<int *> &>())),
              int *>);

static_assert(tested::is_same_v<
              decltype(tested::ranges::get<1>(
                  tested::declval<const tested::ranges::subrange<int *> &>())),
              int *>);

struct identity_adaptor
    : tested::ranges::range_adaptor_closure<identity_adaptor> {
  template <tested::ranges::range R> constexpr R &&operator()(R &&range) const {
    return static_cast<R &&>(range);
  }
};

static_assert(
    tested::ranges::detail::range_adaptor_closure_object<identity_adaptor>);

constexpr bool pipeline_works() {
  int values[] = {1, 2, 3, 4};

  identity_adaptor identity{};

  auto result = values | identity | identity;

  return result == values;
}

static_assert(pipeline_works());

struct move_only {
  int value;

  constexpr move_only(int v) : value(v) {}

  move_only(const move_only &) = delete;

  move_only &operator=(const move_only &) = delete;

  constexpr move_only(move_only &&) = default;

  constexpr move_only &operator=(move_only &&) = default;
};

static_assert(tested::movable<move_only>);

constexpr bool movable_box_works() {
  tested::ranges::single_view<move_only> view(move_only{42});

  return view.front().value == 42;
}

static_assert(movable_box_works());

static_assert(tested::ranges::borrowed_range<
              tested::ranges::owning_view<borrowed_member_range>>);

constexpr bool view_interface_const_operations_work() {
  int values[] = {1, 2, 3, 4};

  const interface_view view{values, values + 4};

  if (view.empty())
    return false;

  if (view.front() != 1)
    return false;

  if (view.back() != 4)
    return false;

  if (view[2] != 3)
    return false;

  if (view.size() != 4)
    return false;

  if (view.data() != values)
    return false;

  if (!static_cast<bool>(view))
    return false;

  return true;
}

static_assert(view_interface_const_operations_work());

// subrange: unsized sentinel storage

struct unsized_sentinel {
  int *value = nullptr;

  constexpr unsized_sentinel() noexcept = default;

  constexpr explicit unsized_sentinel(int *pointer) noexcept : value(pointer) {}

  friend constexpr bool operator==(unsized_sentinel left,
                                   unsized_sentinel right) noexcept {
    return left.value == right.value;
  }

  friend constexpr bool operator!=(unsized_sentinel left,
                                   unsized_sentinel right) noexcept {
    return !(left == right);
  }

  friend constexpr bool operator==(int *iterator,
                                   unsized_sentinel sentinel) noexcept {
    return iterator == sentinel.value;
  }

  friend constexpr bool operator==(unsized_sentinel sentinel,
                                   int *iterator) noexcept {
    return sentinel.value == iterator;
  }

  friend constexpr bool operator!=(int *iterator,
                                   unsized_sentinel sentinel) noexcept {
    return !(iterator == sentinel);
  }

  friend constexpr bool operator!=(unsized_sentinel sentinel,
                                   int *iterator) noexcept {
    return !(sentinel == iterator);
  }
};

static_assert(tested::sentinel_for<unsized_sentinel, int *>);

constexpr bool unsized_subrange_works() {
  int values[] = {1, 2, 3, 4};

  tested::ranges::subrange<int *, unsized_sentinel,
                           tested::ranges::subrange_kind::unsized>
      range(values, unsized_sentinel{values + 4});

  if (range.begin() != values)
    return false;

  if (range.end().value != values + 4)
    return false;

  if (range.empty())
    return false;

  return true;
}

static_assert(unsized_subrange_works());

// subrange: explicit size storage

constexpr bool sized_subrange_storage_works() {
  int values[] = {1, 2, 3, 4};

  tested::ranges::subrange<int *, unsized_sentinel,
                           tested::ranges::subrange_kind::sized>
      range(values, unsized_sentinel{values + 4}, 4);

  if (range.size() != 4)
    return false;

  range.advance(2);

  if (range.begin() != values + 2)
    return false;

  if (range.size() != 2)
    return false;

  return true;
}

static_assert(sized_subrange_storage_works());

// subrange tuple conversion

constexpr bool subrange_tuple_conversion_works() {
  int values[] = {1, 2, 3};

  tested::ranges::subrange range(values, values + 3);

  auto pair = static_cast<tested::pair<int *, int *>>(range);

  return pair.first == values && pair.second == values + 3;
}

static_assert(subrange_tuple_conversion_works());

// subrange structured binding support

static_assert(requires(const tested::ranges::subrange<int *> &range) {
  tested::ranges::get<0>(range);
  tested::ranges::get<1>(range);
});

constexpr bool get_works() {
  int values[] = {1, 2, 3};

  tested::ranges::subrange range(values, values + 3);

  return tested::ranges::get<0>(range) == values &&
         tested::ranges::get<1>(range) == values + 3;
}

static_assert(get_works());

static_assert(requires(tested::ranges::subrange<int *> &range) {
  get<0>(range);
  get<1>(range);
});

static_assert(tested::tuple_size_v<tested::ranges::subrange<int *>> == 2);

constexpr bool get_subrange_works() {
  int values[] = {1, 2, 3};

  tested::ranges::subrange range(values, values + 3);

  return get<0>(range) == values && get<1>(range) == values + 3;
}

static_assert(get_subrange_works());

static_assert(
    tested::is_same_v<
        typename tested::tuple_element_t<0, tested::ranges::subrange<int *>>,
        int *>);

static_assert(
    tested::is_same_v<
        typename tested::tuple_element_t<1, tested::ranges::subrange<int *>>,
        int *>);

static_assert(tested::tuple_size_v<tested::ranges::subrange<int *>> == 2);

static_assert(
    tested::is_same_v<
        tested::tuple_element_t<0, tested::ranges::subrange<int *>>, int *>);

#ifdef FTL_REPLACE_STL

constexpr bool subrange_structured_binding_works() {
  int values[] = {1, 2, 3};

  tested::ranges::subrange range(values, values + 3);

  auto [first, last] = range;

  return first == values && last == values + 3;
}

static_assert(subrange_structured_binding_works());

#endif

// ref_view const behavior

constexpr bool ref_view_const_behavior_works() {
  int values[] = {1, 2, 3};

  member_range range{values, values + 3};

  const tested::ranges::ref_view<member_range> view(range);

  if (view.base().begin() != values)
    return false;

  if (view.begin() != values)
    return false;

  return true;
}

static_assert(ref_view_const_behavior_works());

// owning_view const access

constexpr bool owning_view_const_behavior_works() {
  int values[] = {1, 2, 3};

  tested::ranges::owning_view<member_range> view(
      member_range{values, values + 3});

  const auto &constant = view;

  if (constant.begin() != values)
    return false;

  if (constant.end() != values + 3)
    return false;

  return true;
}

static_assert(owning_view_const_behavior_works());

// movable_box move-only lifetime path

constexpr bool movable_box_move_only_path_works() {
  tested::ranges::single_view<move_only> first(move_only{42});

  auto second = static_cast<tested::ranges::single_view<move_only> &&>(first);

  return second.front().value == 42;
}

static_assert(movable_box_move_only_path_works());

constexpr bool ranges_small_surface_works() {
  int values[] = {1, 2, 3};

  int range_evaluations = 0;

  auto adjacent_zero = tested::ranges::views::adjacent<0>(
      evaluated_range(values, values + 3, range_evaluations));

  static_assert(tested::is_same_v<decltype(adjacent_zero),
                                  tested::ranges::empty_view<tested::tuple<>>>);

  if (range_evaluations != 1 || adjacent_zero.size() != 0) {
    return false;
  }

  int function_evaluations = 0;

  auto transform_zero = tested::ranges::views::adjacent_transform<0>(
      evaluated_range(values, values + 3, range_evaluations),
      evaluated_function(function_evaluations));

  static_assert(tested::is_same_v<decltype(transform_zero),
                                  tested::ranges::empty_view<int>>);

  if (range_evaluations != 2 || function_evaluations != 1 ||
      transform_zero.size() != 0) {
    return false;
  }

  /*
   * Verify the closure forms too.
   */
  auto piped_adjacent = evaluated_range(values, values + 3, range_evaluations) |
                        tested::ranges::views::adjacent<0>;

  auto piped_transform =
      evaluated_range(values, values + 3, range_evaluations) |
      tested::ranges::views::adjacent_transform<0>(
          evaluated_function(function_evaluations));

  if (piped_adjacent.size() != 0 || piped_transform.size() != 0 ||
      range_evaluations != 4 || function_evaluations != 2) {
    return false;
  }

  tested::ranges::subrange range{values, values + 3};

  if (tested::get<0>(range) != values || tested::get<1>(range) != values + 3) {
    return false;
  }

  return true;
}

static_assert(ranges_small_surface_works());

struct movable_value {
  int value = 0;

  constexpr movable_value() = default;

  constexpr explicit movable_value(int initial) : value(initial) {}

  movable_value(const movable_value &) = delete;

  movable_value &operator=(const movable_value &) = delete;

  constexpr movable_value(movable_value &&other) noexcept : value(other.value) {
    other.value = -1;
  }

  constexpr movable_value &operator=(movable_value &&other) noexcept {
    value = other.value;
    other.value = -1;
    return *this;
  }
};

using movable_array = movable_value (&)[3];

using movable_as_rvalue = decltype(tested::ranges::views::as_rvalue(
    tested::declval<movable_array>()));

static_assert(tested::ranges::view<movable_as_rvalue>);

static_assert(tested::ranges::random_access_range<movable_as_rvalue>);

static_assert(tested::ranges::common_range<movable_as_rvalue>);

static_assert(tested::ranges::sized_range<movable_as_rvalue>);

static_assert(tested::ranges::borrowed_range<movable_as_rvalue>);

static_assert(tested::is_same_v<
              tested::ranges::range_value_t<movable_as_rvalue>, movable_value>);

static_assert(
    tested::is_same_v<tested::ranges::range_reference_t<movable_as_rvalue>,
                      movable_value &&>);

static_assert(tested::is_same_v<tested::ranges::iterator_t<movable_as_rvalue>,
                                tested::move_iterator<movable_value *>>);

constexpr bool as_rvalue_view_works() {
  movable_value values[] = {movable_value{1}, movable_value{2},
                            movable_value{3}};

  auto view = values | tested::ranges::views::as_rvalue;

  if (view.size() != 3 || view.base().begin() != values) {
    return false;
  }

  auto iterator = view.begin();

  movable_value first{*iterator};

  if (first.value != 1 || values[0].value != -1) {
    return false;
  }

  ++iterator;

  movable_value second{*iterator};

  if (second.value != 2 || values[1].value != -1) {
    return false;
  }

  ++iterator;

  movable_value third{*iterator};

  if (third.value != 3 || values[2].value != -1) {
    return false;
  }

  ++iterator;

  if (iterator != view.end()) {
    return false;
  }

  return true;
}

static_assert(as_rvalue_view_works());

using to_source_array = int (&)[4];

static_assert(tested::is_same_v<decltype(tested::ranges::to<to_sequence<int>>(
                                    tested::declval<to_source_array>())),
                                to_sequence<int>>);

static_assert(
    tested::is_same_v<decltype(tested::ranges::to<deduced_to_container>(
                          tested::declval<to_source_array>())),
                      deduced_to_container<int>>);

constexpr bool equal_to_values(const auto &range, const int *expected,
                               tested::size_t count) {
  if (range.size() != count) {
    return false;
  }

  for (tested::size_t index = 0; index < count; ++index) {
    if (range.begin()[index] != expected[index]) {
      return false;
    }
  }

  return true;
}

constexpr bool ranges_to_works() {
  int values[] = {1, 2, 3, 4};

  {
    auto result = tested::ranges::to<direct_to_container>(values, 11);

    if (result.argument != 11 || !equal_to_values(result, values, 4)) {
      return false;
    }
  }

  {
    auto result = tested::ranges::to<from_range_to_container>(values, 12);

    if (result.argument != 12 || !equal_to_values(result, values, 4)) {
      return false;
    }
  }

  {
    auto result = tested::ranges::to<iterator_to_container>(values, 13);

    if (result.argument != 13 || !equal_to_values(result, values, 4)) {
      return false;
    }
  }

  {
    auto result = tested::ranges::to<append_priority_container>(values);

    if (result.route != 1 || result.reserved != 4 ||
        !equal_to_values(result, values, 4)) {
      return false;
    }
  }

  {
    auto result = tested::ranges::to<push_back_to_container>(values);

    if (!equal_to_values(result, values, 4)) {
      return false;
    }
  }

  {
    auto result = tested::ranges::to<emplace_hint_to_container>(values);

    if (!equal_to_values(result, values, 4)) {
      return false;
    }
  }

  {
    auto result = tested::ranges::to<insert_to_container>(values);

    if (!equal_to_values(result, values, 4)) {
      return false;
    }
  }

  {
    int first_values[] = {1, 2};

    int second_values[] = {3, 4, 5};

    member_range rows[] = {{first_values, first_values + 2},
                           {second_values, second_values + 3}};

    auto result = tested::ranges::to<to_sequence<to_sequence<int>>>(rows);

    if (result.size() != 2 ||
        !equal_to_values(result.storage[0], first_values, 2) ||
        !equal_to_values(result.storage[1], second_values, 3)) {
      return false;
    }
  }

  {
    auto result = values | tested::ranges::to<to_sequence<int>>();

    if (result.reserved != 4 || !equal_to_values(result, values, 4)) {
      return false;
    }
  }

  {
    auto result = values | tested::ranges::to<from_range_to_container>(21);

    if (result.argument != 21 || !equal_to_values(result, values, 4)) {
      return false;
    }
  }

  {
    auto result = tested::ranges::to<deduced_to_container>(values);

    static_assert(
        tested::is_same_v<decltype(result), deduced_to_container<int>>);

    if (!equal_to_values(result, values, 4)) {
      return false;
    }
  }

  {
    auto result = values | tested::ranges::to<deduced_to_container>();

    static_assert(
        tested::is_same_v<decltype(result), deduced_to_container<int>>);

    if (!equal_to_values(result, values, 4)) {
      return false;
    }
  }

  return true;
}

static_assert(ranges_to_works());

struct mutable_only_contiguous_range {
  int *first = nullptr;
  int *last = nullptr;

  int *begin() noexcept { return first; }

  int *end() noexcept { return last; }

  int *data() noexcept { return first; }
};

static_assert(tested::ranges::contiguous_range<mutable_only_contiguous_range>);

static_assert(!tested::ranges::range<const mutable_only_contiguous_range>);

static_assert(
    tested::is_same_v<decltype(tested::ranges::cdata(
                          tested::declval<mutable_only_contiguous_range &>())),
                      const int *>);

bool ftl_test() {
  int values[] = {1, 2, 3, 4};

  borrowed_member_range borrowed{values, values + 4};

  if (tested::ranges::begin(static_cast<borrowed_member_range &&>(borrowed)) !=
      values) {
    return false;
  }

  if (tested::ranges::end(static_cast<borrowed_member_range &&>(borrowed)) !=
      values + 4) {
    return false;
  }

  reverse_adl_test::range reversed{values, values + 4};

  if (*tested::ranges::rbegin(reversed) != 4)
    return false;

  if (*(tested::ranges::rend(reversed) - 1) != 1)
    return false;

  move_only_range movable{values, values + 4};

  auto owned =
      tested::ranges::views::all(static_cast<move_only_range &&>(movable));

  static_assert(
      tested::is_same_v<decltype(owned),
                        tested::ranges::owning_view<move_only_range>>);

  if (owned.begin() != values)
    return false;

  if (owned.end() != values + 4)
    return false;

  if (owned.size() != 4)
    return false;

  if (owned.data() != values)
    return false;

  auto moved_base = static_cast<decltype(owned) &&>(owned).base();

  if (moved_base.begin() != values)
    return false;

  if (moved_base.end() != values + 4)
    return false;

  tested::ranges::single_view<int> in_place_single(tested::in_place, 7);

  if (in_place_single.front() != 7)
    return false;

  if (in_place_single.back() != 7)
    return false;

  tested::ranges::dangling ignored{values, values + 4};
  (void)ignored;

  if (!ranges_small_surface_works()) {
    return false;
  }

  if (!as_rvalue_view_works()) {
    return false;
  }

  return ranges_constexpr_works() && range_iterator_operations_work();
}
