#ifdef FTL_REPLACE_STL
#include <array>
#include <concepts>
#include <memory_resource>
#include <ranges>
#include <set>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/concepts>
#include <ftl/memory_resource>
#include <ftl/ranges>
#include <ftl/set>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct set_probe {};
struct set_transparent_compare {
  using is_transparent = void;
  bool operator()(int left, int right) const { return left < right; }
  bool operator()(int, set_probe) const { return true; }
  bool operator()(set_probe, int) const { return false; }
};
struct controlled_set_compare {
  inline static int comparisons;
  inline static int before_throw = -1;
  bool operator()(int left, int right) const {
    ++comparisons;
    if (before_throw == 0) throw 1;
    if (before_throw > 0) --before_throw;
    return left < right;
  }
};
struct legacy_set_key {
  int value;
  friend bool operator==(const legacy_set_key&, const legacy_set_key&) = default;
  friend bool operator<(const legacy_set_key& a, const legacy_set_key& b) { return a.value < b.value; }
};
template <class C>
concept set_probe_findable = requires(C &c) { c.find(set_probe{}); };

static_assert(tested::bidirectional_iterator<tested::set<int>::iterator>);
static_assert(tested::ranges::bidirectional_range<tested::set<int>>);
static_assert(tested::is_const_v<tested::remove_reference_t<
                  tested::iter_reference_t<tested::set<int>::iterator>>>);
static_assert(
    tested::is_same_v<tested::pmr::set<int>,
                      tested::set<int, tested::less<int>,
                                  tested::pmr::polymorphic_allocator<int>>>);
static_assert(set_probe_findable<tested::set<int, set_transparent_compare>>);
static_assert(
    set_probe_findable<tested::multiset<int, set_transparent_compare>>);
static_assert(!set_probe_findable<tested::set<int>>);
static_assert(tested::is_same_v<
    typename tested::set<int, tested::less<int>>::node_type,
    typename tested::multiset<int, tested::greater<int>>::node_type>);
static_assert(tested::is_same_v<
    decltype(tested::declval<const tested::set<legacy_set_key>&>() <=>
             tested::declval<const tested::set<legacy_set_key>&>()),
    tested::weak_ordering>);
using deduced_set = decltype(tested::set(
    tested::declval<int *>(), tested::declval<int *>(),
    tested::declval<tested::allocator<int>>()));
static_assert(tested::is_same_v<deduced_set, tested::set<int>>);

bool ftl_test() {
  {
    tested::set<int, controlled_set_compare> source{1};
    tested::set<int, controlled_set_compare> destination{2};
    auto handle = source.extract(1);
    controlled_set_compare::before_throw = 0;
    try { destination.insert(tested::move(handle)); return false; }
    catch (...) {}
    controlled_set_compare::before_throw = -1;
    if (!handle || handle.value() != 1) return false;
  }
  {
    tested::multiset<int, tested::greater<int>> source{3, 2, 2, 1};
    tested::set<int, tested::less<int>> destination{2};
    destination.merge(source);
    if (destination != tested::set<int>{1, 2, 3} || source.count(2) != 2)
      return false;
  }
  {
    tested::set<int> source{1};
    tested::set<int, controlled_set_compare> destination{2};
    controlled_set_compare::before_throw = 0;
    try { destination.merge(source); return false; }
    catch (...) {}
    controlled_set_compare::before_throw = -1;
    if (!source.contains(1) || destination.size() != 1) return false;
  }
  {
    tested::set<int, controlled_set_compare> hinted;
    auto hint = hinted.end();
    controlled_set_compare::comparisons = 0;
    for (int value = 0; value != 20; ++value)
      hint = hinted.insert(hinted.end(), value);
    if (controlled_set_compare::comparisons > 40) return false;
  }

  tested::set<int> values{3, 1, 2, 2};
  if (values.size() != 3 || *values.begin() != 1)
    return false;
  const int *stable = &*values.find(2);
  for (int i = 4; i != 100; ++i)
    values.insert(i);
  if (&*values.find(2) != stable)
    return false;
  tested::array<int, 2> extra{101, 102};
  values.insert_range(extra);
  auto node = values.extract(1);
  node.value() = 0;
  if (!values.insert(tested::move(node)).inserted || !values.contains(0))
    return false;
  if (tested::erase_if(values, [](int value) { return value > 3; }) != 98)
    return false;
  tested::multiset<int> duplicates{1, 1, 2};
  return duplicates.count(1) == 2 && duplicates.erase(1) == 2;
}
