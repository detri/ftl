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
