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
struct alternate_int_hash { tested::size_t operator()(int value) const { return static_cast<tested::size_t>(value * 17); } };
struct alternate_int_equal { bool operator()(int a, int b) const { return a == b; } };
struct grouped_key { int identity; int group; friend bool operator==(const grouped_key&, const grouped_key&) = default; };
struct grouped_hash { tested::size_t operator()(const grouped_key& value) const { return value.group; } };
struct grouped_equal { bool operator()(const grouped_key& a, const grouped_key& b) const { return a.group == b.group; } };
struct controlled_hash {
  inline static int before_throw = -1;
  tested::size_t operator()(int value) const {
    if (before_throw == 0) throw 1;
    if (before_throw > 0) --before_throw;
    return value;
  }
};
struct unordered_allocation_state { inline static int allocations; inline static int deallocations; };
template<class T> struct tracked_unordered_allocator {
  using value_type = T;
  tracked_unordered_allocator() = default;
  template<class U> tracked_unordered_allocator(const tracked_unordered_allocator<U>&) {}
  T* allocate(tested::size_t n) { ++unordered_allocation_state::allocations; return static_cast<T*>(::operator new(n * sizeof(T))); }
  void deallocate(T* p, tested::size_t) { ++unordered_allocation_state::deallocations; ::operator delete(p); }
  template<class U> bool operator==(const tracked_unordered_allocator<U>&) const { return true; }
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
static_assert(tested::is_same_v<
    typename tested::unordered_set<int>::node_type,
    typename tested::unordered_multiset<int, alternate_int_hash,
                                         alternate_int_equal>::node_type>);
using deduced_unordered_set = decltype(tested::unordered_set(
    tested::declval<int *>(), tested::declval<int *>(), tested::size_t{},
    tested::declval<tested::allocator<int>>()));
static_assert(tested::is_same_v<deduced_unordered_set,
                                tested::unordered_set<int>>);
bool ftl_test() {
  {
    tested::unordered_set<int, controlled_hash> source{1};
    tested::unordered_set<int, controlled_hash> destination{2};
    auto handle = source.extract(1);
    controlled_hash::before_throw = 0;
    try { destination.insert(tested::move(handle)); return false; }
    catch (...) {}
    controlled_hash::before_throw = -1;
    if (!handle || handle.value() != 1) return false;
  }
  {
    using allocation_set = tested::unordered_set<int, controlled_hash,
        tested::equal_to<int>, tracked_unordered_allocator<int>>;
    allocation_set guarded(8);
    unordered_allocation_state::allocations = unordered_allocation_state::deallocations = 0;
    controlled_hash::before_throw = 0;
    try { guarded.insert(1); return false; }
    catch (...) {}
    controlled_hash::before_throw = -1;
    if (unordered_allocation_state::allocations != 1 ||
        unordered_allocation_state::deallocations != 1) return false;
  }
  {
    tested::unordered_multiset<int, alternate_int_hash, alternate_int_equal> source{1, 2, 2, 3};
    tested::unordered_set<int> destination{2};
    destination.merge(source);
    if (destination.size() != 3 || source.count(2) != 2) return false;
  }
  {
    tested::unordered_set<grouped_key, grouped_hash, grouped_equal> left{{1, 7}};
    tested::unordered_set<grouped_key, grouped_hash, grouped_equal> right{{2, 7}};
    if (left == right) return false;
  }

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
