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
struct counting_less {
  int *comparisons{};
  bool operator()(int left, int right) const {
    ++*comparisons;
    return left < right;
  }
};
static_assert(
    tested::random_access_iterator<tested::flat_map<int, int>::iterator>);
static_assert(tested::ranges::random_access_range<tested::flat_map<int, int>>);
static_assert(tested::is_same_v<tested::flat_map<int, int>::size_type,
                                tested::size_t>);
static_assert(tested::is_same_v<tested::flat_map<int, int>::difference_type,
                                tested::ptrdiff_t>);
static_assert(tested::is_same_v<tested::flat_map<int, int>::key_container_type,
                                tested::vector<int>>);
static_assert(
    tested::is_same_v<tested::flat_map<int, int>::mapped_container_type,
                      tested::vector<int>>);
bool ftl_test() {
  tested::vector<int> keys;
  tested::vector<int> mapped;
  for (int i = 511; i >= 0; --i) {
    keys.push_back(i);
    mapped.push_back(i * 2);
  }
  int comparisons = 0;
  tested::flat_map<int, int, counting_less> measured(
      tested::move(keys), tested::move(mapped), counting_less{&comparisons});
  if (measured.size() != 512 || measured.begin()->second != 0 ||
      comparisons >= 20000)
    return false;
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
