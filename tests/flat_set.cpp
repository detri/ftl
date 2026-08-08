#ifdef FTL_REPLACE_STL
#include <array>
#include <concepts>
#include <flat_set>
#include <ranges>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/concepts>
#include <ftl/flat_set>
#include <ftl/ranges>
#include <ftl/type_traits>
namespace tested = ftl;
#endif
static_assert(tested::random_access_iterator<tested::flat_set<int>::iterator>);
static_assert(tested::ranges::random_access_range<tested::flat_set<int>>);
static_assert(tested::is_same_v<tested::flat_set<int>::container_type,
                                tested::vector<int>>);
static_assert(tested::is_same_v<tested::flat_set<int>::reference, int &>);
static_assert(tested::is_const_v<tested::remove_reference_t<
                  tested::iter_reference_t<tested::flat_set<int>::iterator>>>);
bool ftl_test() {
  tested::flat_set<int> values{3, 1, 2, 2};
  if (values.size() != 3 || *values.begin() != 1)
    return false;
  tested::array<int, 2> extra{4, 5};
  values.insert_range(extra);
  if (!values.contains(4) || values.erase(2) != 1)
    return false;
  auto storage = tested::move(values).extract();
  tested::flat_set<int> restored(tested::sorted_unique, tested::move(storage));
  if (restored != tested::flat_set<int>{1, 3, 4, 5})
    return false;
  tested::flat_multiset<int> duplicates{2, 1, 1};
  return duplicates.count(1) == 2 &&
         tested::erase_if(duplicates, [](int value) { return value == 1; }) ==
             2;
}
