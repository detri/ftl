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
static_assert(tested::forward_iterator<tested::unordered_set<int>::iterator>);
static_assert(tested::ranges::forward_range<tested::unordered_set<int>>);
static_assert(
    tested::is_const_v<tested::remove_reference_t<
        tested::iter_reference_t<tested::unordered_set<int>::iterator>>>);
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
