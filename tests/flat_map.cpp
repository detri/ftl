#ifdef FTL_REPLACE_STL
#include <array>
#include <concepts>
#include <flat_map>
#include <ranges>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/concepts>
#include <ftl/flat_map>
#include <ftl/ranges>
#include <ftl/type_traits>
namespace tested = ftl;
#endif
static_assert(
    tested::random_access_iterator<tested::flat_map<int, int>::iterator>);
static_assert(tested::ranges::random_access_range<tested::flat_map<int, int>>);
bool ftl_test() {
  tested::flat_map<int, int> values{{3, 30}, {1, 10}, {2, 20}, {2, 99}};
  if (values.size() != 3 || values.begin()->first != 1 || values.at(3) != 30)
    return false;
  values[2] = 22;
  values.insert_or_assign(3, 33);
  tested::array<tested::pair<int, int>, 2> extra{{{4, 40}, {5, 50}}};
  values.insert_range(extra);
  if (!values.contains(4) || values.erase(1) != 1)
    return false;
  auto storage = tested::move(values).extract();
  tested::flat_map<int, int> restored(tested::sorted_unique,
                                      tested::move(storage.keys),
                                      tested::move(storage.values));
  if (restored.size() != 4 || restored.keys().front() != 2 ||
      restored.values().front() != 22)
    return false;
  tested::flat_multimap<int, int> duplicates{{1, 1}, {1, 2}, {2, 3}};
  return duplicates.count(1) == 2 &&
         tested::erase_if(
             duplicates, [](const auto &item) { return item.first == 1; }) == 2;
}
