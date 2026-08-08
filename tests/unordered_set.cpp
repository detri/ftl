#ifdef FTL_REPLACE_STL
#include <array>
#include <concepts>
#include <memory_resource>
#include <ranges>
#include <type_traits>
#include <unordered_set>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/concepts>
#include <ftl/memory_resource>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/unordered_set>
namespace tested = ftl;
#endif
struct set_hash_probe {};
struct set_transparent_hash {
  using is_transparent = void;
  tested::size_t operator()(int value) const { return value; }
  tested::size_t operator()(set_hash_probe) const { return 0; }
};
struct set_transparent_equal {
  using is_transparent = void;
  bool operator()(int left, int right) const { return left == right; }
  bool operator()(int, set_hash_probe) const { return false; }
  bool operator()(set_hash_probe, int) const { return false; }
};
template <class C> concept set_hash_probe_findable = requires(C &c) {
  c.find(set_hash_probe{});
};
static_assert(tested::forward_iterator<tested::unordered_set<int>::iterator>);
static_assert(tested::ranges::forward_range<tested::unordered_set<int>>);
static_assert(
    tested::is_const_v<tested::remove_reference_t<
                  tested::iter_reference_t<tested::unordered_set<int>::iterator>>>);
static_assert(set_hash_probe_findable<tested::unordered_set<
                  int, set_transparent_hash, set_transparent_equal>>);
static_assert(set_hash_probe_findable<tested::unordered_multiset<
                  int, set_transparent_hash, set_transparent_equal>>);
static_assert(!set_hash_probe_findable<tested::unordered_set<int>>);
bool ftl_test() {
  tested::unordered_set<int> values{1, 2, 2};
  const int *stable = &*values.find(1);
  for (int i = 3; i != 100; ++i)
    values.insert(i);
  if (&*values.find(1) != stable)
    return false;
  tested::array<int, 2> extra{101, 102};
  values.insert_range(extra);
  auto node = values.extract(1);
  node.value() = 0;
  if (!values.insert(tested::move(node)).inserted || !values.contains(0))
    return false;
  if (tested::erase_if(values, [](int value) { return value > 2; }) != 99)
    return false;
  tested::unordered_multiset<int> duplicates{1, 1, 2};
  return duplicates.count(1) == 2 && duplicates.erase(1) == 2;
}
