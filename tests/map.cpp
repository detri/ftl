#ifdef FTL_REPLACE_STL
#include <array>
#include <concepts>
#include <map>
#include <memory_resource>
#include <ranges>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/concepts>
#include <ftl/map>
#include <ftl/memory_resource>
#include <ftl/ranges>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct probe {};
struct transparent_compare {
  using is_transparent = void;
  bool operator()(int left, int right) const { return left < right; }
  bool operator()(int, probe) const { return true; }
  bool operator()(probe, int) const { return false; }
};
struct throwing_compare {
  throwing_compare() = default;
  throwing_compare(throwing_compare &&) = default;
  throwing_compare &operator=(throwing_compare &&) noexcept(false) {
    return *this;
  }
  bool operator()(int left, int right) const { return left < right; }
};
template <class C> concept probe_findable = requires(C &c) { c.find(probe{}); };
template <class C> concept probe_erasable = requires(C &c) { c.erase(probe{}); };

static_assert(tested::bidirectional_iterator<tested::map<int, int>::iterator>);
static_assert(tested::ranges::bidirectional_range<tested::map<int, int>>);
static_assert(
    tested::uses_allocator_v<tested::map<int, int>,
                             tested::allocator<tested::pair<const int, int>>>);
static_assert(
    tested::is_same_v<tested::pmr::map<int, int>,
                      tested::map<int, int, tested::less<int>,
                                  tested::pmr::polymorphic_allocator<
                                  tested::pair<const int, int>>>>);
static_assert(probe_findable<tested::map<int, int, transparent_compare>>);
static_assert(probe_findable<tested::multimap<int, int, transparent_compare>>);
static_assert(!probe_findable<tested::map<int, int>>);
static_assert(probe_erasable<tested::map<int, int, transparent_compare>>);
static_assert(!noexcept(tested::declval<tested::map<int, int, throwing_compare> &>() =
                        tested::declval<tested::map<int, int, throwing_compare> &&>()));
static_assert(tested::is_same_v<
    typename tested::map<int, int, tested::less<int>>::node_type,
    typename tested::multimap<int, int, tested::greater<int>>::node_type>);
static_assert(!tested::is_constructible_v<
    typename tested::map<int, int>::value_compare, tested::less<int>>);
static_assert(!tested::is_invocable_v<
    typename tested::map<int, int>::value_compare, int, int>);
using deduced_map = decltype(tested::map(
    tested::declval<tested::pair<const int, int> *>(),
    tested::declval<tested::pair<const int, int> *>(),
    tested::declval<tested::allocator<tested::pair<const int, int>>>()));
static_assert(tested::is_same_v<deduced_map, tested::map<int, int>>);

bool ftl_test() {
  {
    tested::multimap<int, int, tested::greater<int>> source{
        {3, 30}, {2, 20}, {2, 21}, {1, 10}};
    tested::map<int, int> destination{{2, 99}};
    destination.merge(source);
    if (destination.size() != 3 || destination.at(1) != 10 ||
        destination.at(2) != 99 || destination.at(3) != 30 ||
        source.count(2) != 2)
      return false;
  }

  tested::map<int, int> values{{3, 30}, {1, 10}, {2, 20}, {2, 99}};
  if (values.size() != 3 || values.begin()->first != 1 || values.at(3) != 30)
    return false;
  int *stable = &values.find(2)->second;
  for (int i = 4; i != 100; ++i)
    values.try_emplace(i, i * 10);
  if (&values.find(2)->second != stable)
    return false;
  values[2] = 22;
  values.insert_or_assign(3, 33);
  tested::array<tested::pair<int, int>, 2> extra{{{101, 1}, {102, 2}}};
  values.insert_range(extra);
  auto node = values.extract(1);
  node.key() = 0;
  if (!values.insert(tested::move(node)).inserted || !values.contains(0))
    return false;
  if (tested::erase_if(values,
                       [](const auto &item) { return item.first > 3; }) != 98)
    return false;
  tested::multimap<int, int> duplicates{{1, 1}, {1, 2}, {2, 3}};
  if (duplicates.count(1) != 2)
    return false;
  auto range = duplicates.equal_range(1);
  return tested::distance(range.first, range.second) == 2;
}
