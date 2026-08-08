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

static_assert(tested::bidirectional_iterator<tested::set<int>::iterator>);
static_assert(tested::ranges::bidirectional_range<tested::set<int>>);
static_assert(tested::is_const_v<tested::remove_reference_t<
                  tested::iter_reference_t<tested::set<int>::iterator>>>);
static_assert(
    tested::is_same_v<tested::pmr::set<int>,
                      tested::set<int, tested::less<int>,
                                  tested::pmr::polymorphic_allocator<int>>>);

bool ftl_test() {
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
