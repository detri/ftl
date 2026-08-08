#ifdef FTL_REPLACE_STL
#include <array>
#include <concepts>
#include <flat_set>
#include <memory_resource>
#include <ranges>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/concepts>
#include <ftl/flat_set>
#include <ftl/memory_resource>
#include <ftl/ranges>
#include <ftl/type_traits>
namespace tested = ftl;
#endif
#if __cpp_lib_flat_set < 202207L
#error bad flat_set feature-test macro
#endif
struct flat_set_probe {};
struct flat_set_transparent_less {
  using is_transparent = void;
  bool operator()(int left, int right) const { return left < right; }
  bool operator()(int, flat_set_probe) const { return true; }
  bool operator()(flat_set_probe, int) const { return false; }
};
template <class C> concept flat_set_probe_findable = requires(C &c) {
  c.find(flat_set_probe{});
};
struct throwing_vector : tested::vector<int> {
  using tested::vector<int>::vector;
  static inline bool throw_on_move = false;
  throwing_vector() = default;
  throwing_vector(throwing_vector &&other)
      : tested::vector<int>(tested::move(other)) {
    if (throw_on_move)
      throw 1;
  }
  throwing_vector &operator=(throwing_vector &&other) {
    if (throw_on_move)
      throw 1;
    tested::vector<int>::operator=(tested::move(other));
    return *this;
  }
};
static_assert(tested::random_access_iterator<tested::flat_set<int>::iterator>);
static_assert(tested::ranges::random_access_range<tested::flat_set<int>>);
static_assert(tested::is_same_v<tested::flat_set<int>::container_type,
                                tested::vector<int>>);
static_assert(tested::is_same_v<tested::flat_set<int>::reference, int &>);
static_assert(flat_set_probe_findable<
              tested::flat_set<int, flat_set_transparent_less>>);
static_assert(!flat_set_probe_findable<tested::flat_set<int>>);
using pmr_set_vector =
    tested::vector<int, tested::pmr::polymorphic_allocator<int>>;
using pmr_flat_set = tested::flat_set<int, tested::less<int>, pmr_set_vector>;
static_assert(tested::uses_allocator_v<
              pmr_flat_set, tested::pmr::polymorphic_allocator<int>>);
static_assert(tested::is_constructible_v<
              pmr_flat_set, tested::pmr::polymorphic_allocator<int>>);
static_assert(tested::is_constructible_v<
              pmr_flat_set, tested::sorted_unique_t, const pmr_set_vector &,
              tested::pmr::polymorphic_allocator<int>>);
static_assert(tested::is_constructible_v<
              tested::flat_multiset<int, tested::less<int>, pmr_set_vector>,
              tested::sorted_equivalent_t, const pmr_set_vector &,
              tested::pmr::polymorphic_allocator<int>>);
static_assert(tested::is_same_v<decltype(tested::flat_set(tested::vector<int>{})),
                                tested::flat_set<int>>);
static_assert(tested::is_same_v<
              decltype(tested::flat_set(
                  pmr_set_vector{},
                  tested::pmr::polymorphic_allocator<int>{})),
              pmr_flat_set>);
static_assert(tested::is_const_v<tested::remove_reference_t<
                  tested::iter_reference_t<tested::flat_set<int>::iterator>>>);
bool ftl_test() {
  throwing_vector throwing_storage{1, 2, 3};
  tested::flat_set<int, tested::less<int>, throwing_vector> exceptional(
      tested::sorted_unique, tested::move(throwing_storage));
  throwing_vector::throw_on_move = true;
  try {
    tested::move(exceptional).extract();
    return false;
  } catch (...) {
    if (!exceptional.empty())
      return false;
  }
  throwing_vector::throw_on_move = false;
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
