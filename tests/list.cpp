#ifdef FTL_REPLACE_STL
#include <array>
#include <concepts>
#include <list>
#include <memory_resource>
#include <ranges>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/concepts>
#include <ftl/list>
#include <ftl/memory_resource>
#include <ftl/ranges>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct list_counted {
  inline static int alive;
  int value{};
  list_counted() { ++alive; }
  explicit list_counted(int v) : value(v) { ++alive; }
  list_counted(const list_counted &x) : value(x.value) { ++alive; }
  list_counted(list_counted &&x) noexcept : value(x.value) { ++alive; }
  list_counted &operator=(const list_counted &) = default;
  ~list_counted() { --alive; }
  bool operator==(const list_counted &) const = default;
  bool operator<(const list_counted &x) const { return value < x.value; }
};
struct list_allocation_state { inline static int allocations; inline static int deallocations; };
template<class T> struct tracked_list_allocator {
  using value_type = T;
  tracked_list_allocator() = default;
  template<class U> tracked_list_allocator(const tracked_list_allocator<U>&) {}
  T* allocate(tested::size_t n) { ++list_allocation_state::allocations; return static_cast<T*>(::operator new(n * sizeof(T))); }
  void deallocate(T* p, tested::size_t) { ++list_allocation_state::deallocations; ::operator delete(p); }
  template<class U> bool operator==(const tracked_list_allocator<U>&) const { return true; }
};
struct throwing_list_value {
  inline static int alive;
  inline static int constructions_before_throw = -1;
  int value{};
  throwing_list_value() { if (constructions_before_throw == 0) throw 1; if (constructions_before_throw > 0) --constructions_before_throw; ++alive; }
  explicit throwing_list_value(int v) : value(v) { ++alive; }
  throwing_list_value(const throwing_list_value& x) : value(x.value) { if (constructions_before_throw == 0) throw 1; if (constructions_before_throw > 0) --constructions_before_throw; ++alive; }
  throwing_list_value(throwing_list_value&& x) noexcept(false) : value(x.value) { ++alive; }
  throwing_list_value& operator=(const throwing_list_value&) = default;
  ~throwing_list_value() { --alive; }
  bool operator==(const throwing_list_value&) const = default;
};
struct incomplete_list;
struct incomplete_list_owner {
  tested::list<incomplete_list> values;
  ~incomplete_list_owner();
};
struct incomplete_list {};
incomplete_list_owner::~incomplete_list_owner() = default;

static_assert(tested::bidirectional_iterator<tested::list<int>::iterator>);
static_assert(tested::ranges::bidirectional_range<tested::list<int>>);
static_assert(!tested::ranges::random_access_range<tested::list<int>>);
static_assert(tested::ranges::sized_range<tested::list<int>>);
static_assert(tested::is_same_v<tested::list<int>::value_type, int>);
static_assert(
    tested::uses_allocator_v<tested::list<int>, tested::allocator<int>>);
static_assert(tested::is_same_v<
              tested::pmr::list<int>,
              tested::list<int, tested::pmr::polymorphic_allocator<int>>>);

bool ftl_test() {
  {
    list_allocation_state::allocations = list_allocation_state::deallocations = 0;
    throwing_list_value::constructions_before_throw = 1;
    try { tested::list<throwing_list_value, tracked_list_allocator<throwing_list_value>> guarded(3); return false; }
    catch (...) {}
    throwing_list_value::constructions_before_throw = -1;
    if (throwing_list_value::alive != 0 || list_allocation_state::allocations != list_allocation_state::deallocations) return false;
  }
  {
    tested::list<throwing_list_value> guarded;
    guarded.emplace_back(1); guarded.emplace_back(2);
    throwing_list_value value(9);
    throwing_list_value::constructions_before_throw = 1;
    try { guarded.insert(guarded.begin(), 3, value); return false; }
    catch (...) {}
    throwing_list_value::constructions_before_throw = -1;
    if (guarded.size() != 2 || guarded.front().value != 1 || guarded.back().value != 2) return false;
  }
  {
    tested::list<throwing_list_value> guarded;
    for (int value : {1, 4, 2, 3}) guarded.emplace_back(value);
    int comparisons = 2;
    try { guarded.sort([&](const auto& l, const auto& r) { if (comparisons-- == 0) throw 1; return l.value < r.value; }); return false; }
    catch (...) {}
    if (guarded.size() != 4) return false;
  }

  tested::list<int> values{3, 1, 1, 2};
  auto stable = ++values.begin();
  int *address = &*stable;
  values.push_front(4);
  values.push_back(5);
  if (&*stable != address || values.front() != 4 || values.back() != 5)
    return false;
  tested::array<int, 2> extra{7, 8};
  values.prepend_range(extra);
  values.append_range(extra);
  auto p = ++values.begin();
  values.insert_range(p, extra);
  values.sort();
  if (values.unique() != 5 ||
      tested::erase_if(values, [](int x) { return x % 2 == 0; }) != 3)
    return false;
  tested::list<int> other{0, 6, 9};
  values.merge(other);
  if (!other.empty() || values != tested::list<int>{0, 1, 3, 5, 6, 7, 9})
    return false;
  tested::list<int> moved;
  auto first = ++values.begin();
  auto last = first;
  tested::advance(last, 2);
  moved.splice(moved.begin(), values, first, last);
  if (moved != tested::list<int>{1, 3} || values.size() != 5)
    return false;
  values.reverse();
  values.resize(8, 2);
  values.resize(3);
  if (values != tested::list<int>{9, 7, 6})
    return false;
  {
    tested::list<list_counted> objects;
    objects.emplace_front(1);
    objects.emplace_back(2);
    objects.resize(5);
    objects.erase(++objects.begin(), --objects.end());
    if (list_counted::alive != 2)
      return false;
  }
  return list_counted::alive == 0;
}
