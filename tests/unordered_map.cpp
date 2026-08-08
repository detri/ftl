#ifdef FTL_REPLACE_STL
#include <array>
#include <concepts>
#include <memory_resource>
#include <ranges>
#include <type_traits>
#include <unordered_map>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/concepts>
#include <ftl/memory_resource>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/unordered_map>
namespace tested = ftl;
#endif
static_assert(
    tested::forward_iterator<tested::unordered_map<int, int>::iterator>);
static_assert(tested::ranges::forward_range<tested::unordered_map<int, int>>);
static_assert(
    tested::is_same_v<
        tested::pmr::unordered_map<int, int>,
        tested::unordered_map<
            int, int, tested::hash<int>, tested::equal_to<int>,
            tested::pmr::polymorphic_allocator<tested::pair<const int, int>>>>);
bool ftl_test() {
  tested::unordered_map<int, int> values{{1, 10}, {2, 20}, {2, 99}};
  if (values.size() != 2 || values.at(1) != 10)
    return false;
  int *stable = &values.find(1)->second;
  for (int i = 3; i != 100; ++i)
    values.try_emplace(i, i);
  if (&values.find(1)->second != stable ||
      values.load_factor() > values.max_load_factor())
    return false;
  values.insert_or_assign(2, 22);
  tested::array<tested::pair<int, int>, 2> extra{{{101, 1}, {102, 2}}};
  values.insert_range(extra);
  auto node = values.extract(1);
  node.key() = 0;
  if (!values.insert(tested::move(node)).inserted || !values.contains(0))
    return false;
  auto bucket = values.bucket(2);
  bool local_found = false;
  for (auto i = values.begin(bucket); i != values.end(bucket); ++i)
    if (i->first == 2)
      local_found = true;
  if (!local_found || tested::erase_if(values, [](const auto &item) {
                        return item.first > 2;
                      }) != 99)
    return false;
  tested::unordered_multimap<int, int> duplicates{{1, 1}, {1, 2}, {2, 3}};
  auto range = duplicates.equal_range(1);
  return duplicates.count(1) == 2 &&
         tested::distance(range.first, range.second) == 2;
}
