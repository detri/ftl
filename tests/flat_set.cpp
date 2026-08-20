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

struct counting_less {
  int *comparisons{};

  bool operator()(int left, int right) const {
    ++*comparisons;
    return left < right;
  }
};

struct legacy_ordered {
  int value{};

  friend bool operator==(const legacy_ordered &left,
                         const legacy_ordered &right) {
    return left.value == right.value;
  }

  friend bool operator<(const legacy_ordered &left,
                        const legacy_ordered &right) {
    return left.value < right.value;
  }
};

template <class T>
concept has_spaceship =
    requires(const T &left, const T &right) { left <=> right; };

static_assert(!has_spaceship<legacy_ordered>);
template <class C>
concept flat_set_probe_findable = requires(C &c) { c.find(flat_set_probe{}); };
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
static_assert(
    flat_set_probe_findable<tested::flat_set<int, flat_set_transparent_less>>);
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
static_assert(
    tested::is_same_v<decltype(tested::flat_set(tested::vector<int>{})),
                      tested::flat_set<int>>);
static_assert(tested::is_same_v<decltype(tested::flat_set(
                                    pmr_set_vector{},
                                    tested::pmr::polymorphic_allocator<int>{})),
                                pmr_flat_set>);
static_assert(tested::is_const_v<tested::remove_reference_t<
                  tested::iter_reference_t<tested::flat_set<int>::iterator>>>);

using plain_flat_set = tested::flat_set<int>;
using plain_flat_multiset = tested::flat_multiset<int>;

static_assert(
    tested::is_same_v<decltype(tested::declval<plain_flat_set &>().operator=(
                          tested::initializer_list<int>{})),
                      plain_flat_set &>);

static_assert(
    tested::is_same_v<decltype(tested::declval<plain_flat_multiset &>()
                                   .operator=(tested::initializer_list<int>{})),
                      plain_flat_multiset &>);

using flat_set_range = tested::array<int, 4>;
using pmr_set_allocator = tested::pmr::polymorphic_allocator<int>;
using pmr_flat_multiset =
    tested::flat_multiset<int, tested::less<int>, pmr_set_vector>;

static_assert(
    tested::is_same_v<decltype(tested::flat_set(
                          tested::from_range, tested::declval<flat_set_range>(),
                          pmr_set_allocator{})),
                      pmr_flat_set>);

static_assert(
    tested::is_same_v<decltype(tested::flat_set(
                          tested::from_range, tested::declval<flat_set_range>(),
                          tested::less<int>{}, pmr_set_allocator{})),
                      pmr_flat_set>);

static_assert(
    tested::is_same_v<decltype(tested::flat_multiset(
                          tested::from_range, tested::declval<flat_set_range>(),
                          pmr_set_allocator{})),
                      pmr_flat_multiset>);

static_assert(
    tested::is_same_v<decltype(tested::flat_multiset(
                          tested::from_range, tested::declval<flat_set_range>(),
                          tested::less<int>{}, pmr_set_allocator{})),
                      pmr_flat_multiset>);

bool flat_set_bulk_insert_complexity_works() {
  constexpr int existing_count = 4096;
  constexpr int incoming_count = 32;

  tested::vector<int> existing;
  for (int i = 0; i != existing_count; ++i)
    existing.push_back(i * 2);

  tested::vector<int> unsorted;
  for (int i = incoming_count; i != 0; --i)
    unsorted.push_back((i - 1) * 2 + 1);

  int comparisons = 0;

  tested::flat_set<int, counting_less> general(tested::sorted_unique, existing,
                                               counting_less{&comparisons});

  comparisons = 0;
  general.insert(unsorted.begin(), unsorted.end());

  if (general.size() != existing_count + incoming_count || comparisons >= 12000)
    return false;

  tested::vector<int> sorted;
  for (int i = 0; i != incoming_count; ++i)
    sorted.push_back(i * 2 + 1);

  tested::flat_set<int, counting_less> tagged(tested::sorted_unique, existing,
                                              counting_less{&comparisons});

  comparisons = 0;
  tagged.insert(tested::sorted_unique, sorted.begin(), sorted.end());

  if (tagged.size() != existing_count + incoming_count || comparisons >= 12000)
    return false;

  return true;
}

bool flat_set_legacy_comparison_works() {
  tested::flat_set<legacy_ordered> left{
      legacy_ordered{1},
      legacy_ordered{3},
  };

  tested::flat_set<legacy_ordered> right{
      legacy_ordered{1},
      legacy_ordered{4},
  };

  if (!(left == left))
    return false;

  return (left <=> right) < 0;
}

bool ftl_test() {
  if (!flat_set_bulk_insert_complexity_works() ||
      !flat_set_legacy_comparison_works())
    return false;

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
