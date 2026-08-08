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
struct hash_probe {};
struct transparent_hash {
  using is_transparent = void;
  tested::size_t operator()(int value) const { return value; }
  tested::size_t operator()(hash_probe) const { return 0; }
};
struct transparent_equal {
  using is_transparent = void;
  bool operator()(int left, int right) const { return left == right; }
  bool operator()(int, hash_probe) const { return false; }
  bool operator()(hash_probe, int) const { return false; }
};
struct throwing_hash : transparent_hash {
  throwing_hash() = default;
  throwing_hash(throwing_hash &&) = default;
  throwing_hash &operator=(throwing_hash &&) noexcept(false) { return *this; }
};
struct throwing_equal : transparent_equal {
  throwing_equal() = default;
  throwing_equal(throwing_equal &&) = default;
  throwing_equal &operator=(throwing_equal &&) noexcept(false) { return *this; }
};
template <class C> concept hash_probe_findable = requires(C &c) {
  c.find(hash_probe{});
  c.equal_range(hash_probe{});
};
template <class C> struct iterator_like {
  operator typename C::iterator() const;
};
static_assert(
    tested::forward_iterator<tested::unordered_map<int, int>::iterator>);
static_assert(tested::ranges::forward_range<tested::unordered_map<int, int>>);
static_assert(
    tested::is_same_v<
        tested::pmr::unordered_map<int, int>,
        tested::unordered_map<
            int, int, tested::hash<int>, tested::equal_to<int>,
            tested::pmr::polymorphic_allocator<tested::pair<const int, int>>>>);
using transparent_umap =
    tested::unordered_map<int, int, transparent_hash, transparent_equal>;
static_assert(hash_probe_findable<transparent_umap>);
static_assert(hash_probe_findable<tested::unordered_multimap<
                  int, int, transparent_hash, transparent_equal>>);
static_assert(!hash_probe_findable<tested::unordered_map<int, int>>);
static_assert(tested::is_same_v<
              decltype(tested::declval<transparent_umap &>().erase(
                  iterator_like<transparent_umap>{})),
              transparent_umap::iterator>);
static_assert(!noexcept(
    tested::declval<tested::unordered_map<int, int, throwing_hash,
                                         throwing_equal> &>() =
    tested::declval<tested::unordered_map<int, int, throwing_hash,
                                         throwing_equal> &&>()));
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
