#ifdef FTL_REPLACE_STL
#include <array>
#include <concepts>
#include <flat_map>
#include <memory_resource>
#include <ranges>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/concepts>
#include <ftl/flat_map>
#include <ftl/memory_resource>
#include <ftl/ranges>
#include <ftl/type_traits>
namespace tested = ftl;
#endif
#if __cpp_lib_flat_map < 202207L
#error bad flat_map feature-test macro
#endif
struct flat_probe {};
struct flat_transparent_less {
  using is_transparent = void;
  bool operator()(int left, int right) const { return left < right; }
  bool operator()(int, flat_probe) const { return true; }
  bool operator()(flat_probe, int) const { return false; }
};
template <class C> concept flat_probe_findable = requires(C &c) {
  c.find(flat_probe{});
  c.at(flat_probe{});
};
struct counting_less {
  int *comparisons{};
  bool operator()(int left, int right) const {
    ++*comparisons;
    return left < right;
  }
};
struct throwing_key_vector : tested::vector<int> {
  using tested::vector<int>::vector;
  static inline bool throw_on_move = false;
  throwing_key_vector() = default;
  throwing_key_vector(throwing_key_vector &&other)
      : tested::vector<int>(tested::move(other)) {
    if (throw_on_move)
      throw 1;
  }
  throwing_key_vector &operator=(throwing_key_vector &&other) {
    if (throw_on_move)
      throw 1;
    tested::vector<int>::operator=(tested::move(other));
    return *this;
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
static_assert(flat_probe_findable<
              tested::flat_map<int, int, flat_transparent_less>>);
static_assert(!flat_probe_findable<tested::flat_map<int, int>>);
using pmr_vector = tested::vector<int, tested::pmr::polymorphic_allocator<int>>;
using pmr_flat_map =
    tested::flat_map<int, int, tested::less<int>, pmr_vector, pmr_vector>;
static_assert(tested::uses_allocator_v<pmr_flat_map,
                                       tested::pmr::polymorphic_allocator<int>>);
static_assert(tested::is_constructible_v<
              pmr_flat_map, tested::pmr::polymorphic_allocator<int>>);
static_assert(tested::is_constructible_v<
              pmr_flat_map, tested::sorted_unique_t, const pmr_vector &,
              const pmr_vector &, tested::pmr::polymorphic_allocator<int>>);
static_assert(tested::is_constructible_v<
              tested::flat_multimap<int, int, tested::less<int>, pmr_vector,
                                    pmr_vector>,
              tested::sorted_equivalent_t, const pmr_vector &,
              const pmr_vector &, tested::pmr::polymorphic_allocator<int>>);
static_assert(tested::is_same_v<
              decltype(tested::flat_map(tested::vector<int>{},
                                        tested::vector<long>{})),
              tested::flat_map<int, long>>);
static_assert(tested::is_same_v<
              decltype(tested::flat_map(
                  pmr_vector{}, pmr_vector{},
                  tested::pmr::polymorphic_allocator<int>{})),
              pmr_flat_map>);
bool ftl_test() {
  throwing_key_vector exceptional_keys{1, 2};
  tested::vector<int> exceptional_values{10, 20};
  tested::flat_map<int, int, tested::less<int>, throwing_key_vector>
      exceptional(tested::sorted_unique, tested::move(exceptional_keys),
                  tested::move(exceptional_values));
  throwing_key_vector::throw_on_move = true;
  try {
    tested::move(exceptional).extract();
    return false;
  } catch (...) {
    if (!exceptional.empty() || !exceptional.values().empty())
      return false;
  }
  throwing_key_vector::throw_on_move = false;
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
  tested::vector<int> sorted_keys;
  tested::vector<int> sorted_values;
  for (int i = 0; i != 512; ++i) {
    sorted_keys.push_back(i);
    sorted_values.push_back(i);
  }
  comparisons = 0;
  tested::flat_map<int, int, counting_less> linear(
      tested::move(sorted_keys), tested::move(sorted_values),
      counting_less{&comparisons});
  if (linear.size() != 512 || comparisons >= 2000)
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
