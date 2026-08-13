#ifdef FTL_REPLACE_STL
#include <array>
#include <concepts>
#include <forward_list>
#include <memory_resource>
#include <ranges>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/concepts>
#include <ftl/forward_list>
#include <ftl/memory_resource>
#include <ftl/ranges>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct forward_counted {
  inline static int alive;
  int value{};
  forward_counted() { ++alive; }
  explicit forward_counted(int v) : value(v) { ++alive; }
  forward_counted(const forward_counted &x) : value(x.value) { ++alive; }
  forward_counted(forward_counted &&x) noexcept : value(x.value) { ++alive; }
  forward_counted &operator=(const forward_counted &) = default;
  ~forward_counted() { --alive; }
  bool operator==(const forward_counted &) const = default;
  bool operator<(const forward_counted &x) const { return value < x.value; }
};

struct forward_allocation_state {
  inline static int allocations;
  inline static int deallocations;
};
template<class T> struct tracked_forward_allocator {
  using value_type = T;
  tracked_forward_allocator() = default;
  template<class U> tracked_forward_allocator(const tracked_forward_allocator<U>&) {}
  T* allocate(tested::size_t count) {
    ++forward_allocation_state::allocations;
    return static_cast<T*>(::operator new(count * sizeof(T)));
  }
  void deallocate(T* pointer, tested::size_t) {
    ++forward_allocation_state::deallocations;
    ::operator delete(pointer);
  }
  template<class U> bool operator==(const tracked_forward_allocator<U>&) const { return true; }
};

struct throwing_forward_value {
  inline static int alive;
  inline static int constructions_before_throw = -1;
  int value{};
  throwing_forward_value() {
    if (constructions_before_throw == 0) throw 1;
    if (constructions_before_throw > 0) --constructions_before_throw;
    ++alive;
  }
  explicit throwing_forward_value(int input) : value(input) { ++alive; }
  throwing_forward_value(const throwing_forward_value& other) : value(other.value) {
    if (constructions_before_throw == 0) throw 1;
    if (constructions_before_throw > 0) --constructions_before_throw;
    ++alive;
  }
  throwing_forward_value(throwing_forward_value&& other) noexcept(false) : value(other.value) { ++alive; }
  throwing_forward_value& operator=(const throwing_forward_value&) = default;
  ~throwing_forward_value() { --alive; }
  bool operator==(const throwing_forward_value&) const = default;
};
struct incomplete_forward;
struct incomplete_forward_owner {
  tested::forward_list<incomplete_forward> values;
  ~incomplete_forward_owner();
};
struct incomplete_forward {};
incomplete_forward_owner::~incomplete_forward_owner() = default;

static_assert(tested::forward_iterator<tested::forward_list<int>::iterator>);
static_assert(tested::ranges::forward_range<tested::forward_list<int>>);
static_assert(!tested::ranges::bidirectional_range<tested::forward_list<int>>);
static_assert(tested::is_same_v<tested::forward_list<int>::value_type, int>);
static_assert(tested::uses_allocator_v<tested::forward_list<int>,
                                       tested::allocator<int>>);
static_assert(
    tested::is_same_v<
        tested::pmr::forward_list<int>,
        tested::forward_list<int, tested::pmr::polymorphic_allocator<int>>>);

bool ftl_test() {
  {
    forward_allocation_state::allocations = 0;
    forward_allocation_state::deallocations = 0;
    throwing_forward_value::constructions_before_throw = 1;
    try {
      tested::forward_list<throwing_forward_value,
          tracked_forward_allocator<throwing_forward_value>> guarded(3);
      return false;
    } catch (...) {}
    throwing_forward_value::constructions_before_throw = -1;
    if (throwing_forward_value::alive != 0 ||
        forward_allocation_state::allocations != forward_allocation_state::deallocations)
      return false;
  }
  {
    tested::forward_list<throwing_forward_value> guarded;
    guarded.emplace_front(2);
    guarded.emplace_front(1);
    throwing_forward_value value(9);
    throwing_forward_value::constructions_before_throw = 1;
    try {
      guarded.insert_after(guarded.before_begin(), 3, value);
      return false;
    } catch (...) {}
    throwing_forward_value::constructions_before_throw = -1;
    auto current = guarded.begin();
    if (current++->value != 1 || current++->value != 2 || current != guarded.end())
      return false;
  }
  {
    tested::forward_list<throwing_forward_value> guarded;
    for (int value : {1, 4, 2, 3}) guarded.emplace_front(value);
    int comparisons = 2;
    try {
      guarded.sort([&](const auto& left, const auto& right) {
        if (comparisons-- == 0) throw 1;
        return left.value < right.value;
      });
      return false;
    } catch (...) {}
    int count = 0;
    for (const auto& ignored : guarded) { (void)ignored; ++count; }
    if (count != 4) return false;
  }

  tested::forward_list<int> values{3, 1, 1, 2};
  auto stable = ++values.begin();
  int *address = &*stable;
  values.push_front(4);
  values.emplace_after(values.before_begin(), 5);
  if (&*stable != address)
    return false;
  tested::array<int, 2> extra{7, 8};
  values.prepend_range(extra);
  values.insert_range_after(values.begin(), extra);
  values.sort();
  if (values != tested::forward_list<int>{1, 1, 2, 3, 4, 5, 7, 7, 8, 8})
    return false;
  if (values.unique() != 3 ||
      tested::erase_if(values, [](int x) { return x % 2 == 0; }) != 3)
    return false;
  tested::forward_list<int> other{0, 6, 9};
  values.merge(other);
  if (!other.empty() ||
      values != tested::forward_list<int>{0, 1, 3, 5, 6, 7, 9})
    return false;
  auto before = values.before_begin();
  ++before;
  tested::forward_list<int> moved;
  moved.splice_after(moved.before_begin(), values, before);
  if (moved.front() != 1 ||
      values != tested::forward_list<int>{0, 3, 5, 6, 7, 9})
    return false;
  values.reverse();
  values.resize(8, 2);
  values.resize(3);
  if (values != tested::forward_list<int>{9, 7, 6})
    return false;
  {
    tested::forward_list<forward_counted> objects;
    objects.emplace_front(1);
    objects.resize(4);
    objects.erase_after(objects.begin(), objects.end());
    if (forward_counted::alive != 1)
      return false;
  }
  return forward_counted::alive == 0;
}
