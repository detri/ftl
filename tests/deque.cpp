#ifdef FTL_REPLACE_STL
#include <array>
#include <concepts>
#include <deque>
#include <memory_resource>
#include <ranges>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/compare>
#include <ftl/concepts>
#include <ftl/deque>
#include <ftl/memory_resource>
#include <ftl/ranges>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct counted_deque {
  inline static int alive;
  int value{};
  counted_deque() { ++alive; }
  explicit counted_deque(int v) : value(v) { ++alive; }
  counted_deque(const counted_deque& v) : value(v.value) { ++alive; }
  counted_deque(counted_deque&& v) noexcept : value(v.value) { ++alive; }
  counted_deque& operator=(const counted_deque&) = default;
  counted_deque& operator=(counted_deque&&) = default;
  ~counted_deque() { --alive; }
  bool operator==(const counted_deque&) const = default;
  bool operator<(const counted_deque& other) const { return value < other.value; }
};

struct deque_allocation_state {
  inline static int allocations;
  inline static int deallocations;
};

template<class T> struct tracked_deque_allocator {
  using value_type = T;
  tracked_deque_allocator() = default;
  template<class U> tracked_deque_allocator(const tracked_deque_allocator<U>&) {}
  T* allocate(tested::size_t count) {
    ++deque_allocation_state::allocations;
    return static_cast<T*>(::operator new(count * sizeof(T)));
  }
  void deallocate(T* pointer, tested::size_t) {
    ++deque_allocation_state::deallocations;
    ::operator delete(pointer);
  }
  template<class U> bool operator==(const tracked_deque_allocator<U>&) const { return true; }
};

struct throwing_deque_value {
  inline static int alive;
  inline static int defaults_before_throw = -1;
  inline static int moves_before_throw = -1;
  int value{};
  throwing_deque_value() {
    if (defaults_before_throw == 0) throw 1;
    if (defaults_before_throw > 0) --defaults_before_throw;
    ++alive;
  }
  explicit throwing_deque_value(int input) : value(input) { ++alive; }
  throwing_deque_value(const throwing_deque_value& other) : value(other.value) { ++alive; }
  throwing_deque_value(throwing_deque_value&& other) noexcept(false) : value(other.value) {
    if (moves_before_throw == 0) throw 1;
    if (moves_before_throw > 0) --moves_before_throw;
    ++alive;
    other.value = -1;
  }
  throwing_deque_value& operator=(const throwing_deque_value&) = default;
  throwing_deque_value& operator=(throwing_deque_value&&) = default;
  ~throwing_deque_value() { --alive; }
};

struct incomplete_deque;
struct incomplete_deque_owner { tested::deque<incomplete_deque> values; ~incomplete_deque_owner(); };
struct incomplete_deque {};
incomplete_deque_owner::~incomplete_deque_owner() = default;

static_assert(tested::random_access_iterator<tested::deque<int>::iterator>);
static_assert(tested::ranges::random_access_range<tested::deque<int>>);
static_assert(!tested::ranges::contiguous_range<tested::deque<int>>);
static_assert(tested::is_same_v<tested::deque<int>::value_type, int>);
static_assert(tested::uses_allocator_v<tested::deque<int>, tested::allocator<int>>);
static_assert(tested::is_same_v<tested::pmr::deque<int>, tested::deque<int, tested::pmr::polymorphic_allocator<int>>>);

bool ftl_test() {
  {
    deque_allocation_state::allocations = 0;
    deque_allocation_state::deallocations = 0;
    throwing_deque_value::defaults_before_throw = 1;
    try {
      tested::deque<throwing_deque_value,
                    tracked_deque_allocator<throwing_deque_value>> guarded(3);
      return false;
    } catch (...) {}
    throwing_deque_value::defaults_before_throw = -1;
    if (throwing_deque_value::alive != 0 ||
        deque_allocation_state::allocations != deque_allocation_state::deallocations)
      return false;
  }

  {
    tested::deque<throwing_deque_value> guarded;
    guarded.emplace_back(1);
    guarded.emplace_back(2);
    guarded.emplace_back(3);
    throwing_deque_value::moves_before_throw = 1;
    try {
      guarded.emplace(guarded.begin() + 1, throwing_deque_value(9));
      return false;
    } catch (...) {}
    throwing_deque_value::moves_before_throw = -1;
    if (guarded.size() != 3 || guarded[0].value != 1 ||
        guarded[1].value != 2 || guarded[2].value != 3)
      return false;
  }

  tested::deque<int> values{2, 3};
  values.push_front(1);
  int& stable = values[1];
  for (int i = 0; i < 700; ++i) values.push_back(i + 4);
  for (int i = 0; i < 700; ++i) values.push_front(-i);
  if (&stable != &values[701] || stable != 2 || values.front() != -699 || values.back() != 703) return false;
  auto preserved = values.begin() + 800;
  int* preserved_address = &*preserved;
  values.pop_front();
  if (&*preserved != preserved_address) return false;

  tested::array<int, 3> extra{8, 9, 10};
  values.assign({1, 4});
  values.insert(values.begin() + 1, 2, 2);
  values.insert_range(values.begin() + 3, extra);
  values.prepend_range(extra);
  values.append_range(extra);
  if (values != tested::deque<int>{8, 9, 10, 1, 2, 2, 8, 9, 10, 4, 8, 9, 10}) return false;
  if (tested::erase(values, 2) != 2 || tested::erase_if(values, [](int v) { return v == 9; }) != 3) return false;
  if (values.at(0) != 8 || values[values.size() - 1] != 10) return false;

  { tested::deque<counted_deque> objects; objects.emplace_front(1); objects.emplace_back(2); objects.resize(5); objects.erase(objects.begin() + 1, objects.end() - 1); if (counted_deque::alive != 2) return false; }
  return counted_deque::alive == 0;
}
