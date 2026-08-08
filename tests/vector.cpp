#ifdef FTL_REPLACE_STL
#include <array>
#include <concepts>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/concepts>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
#include <ftl/vector>
namespace tested = ftl;
#endif

struct counted {
  inline static int alive;
  int value{};
  counted() { ++alive; }
  explicit counted(int input) : value(input) { ++alive; }
  counted(const counted& other) : value(other.value) { ++alive; }
  counted(counted&& other) noexcept : value(other.value) { ++alive; }
  counted& operator=(const counted&) = default;
  counted& operator=(counted&&) = default;
  ~counted() { --alive; }
  bool operator==(const counted&) const = default;
  bool operator<(const counted& other) const { return value < other.value; }
};

struct incomplete;
struct incomplete_owner {
  tested::vector<incomplete> values;
  ~incomplete_owner();
};
struct incomplete {};
incomplete_owner::~incomplete_owner() = default;

template<class T, bool Copy, bool Move, bool Swap>
struct stateful_allocator {
  using value_type = T;
  using propagate_on_container_copy_assignment = tested::bool_constant<Copy>;
  using propagate_on_container_move_assignment = tested::bool_constant<Move>;
  using propagate_on_container_swap = tested::bool_constant<Swap>;
  template<class U> struct rebind { using other = stateful_allocator<U, Copy, Move, Swap>; };
  int id{};
  constexpr stateful_allocator() = default;
  constexpr explicit stateful_allocator(int value) : id(value) {}
  template<class U> constexpr stateful_allocator(const stateful_allocator<U, Copy, Move, Swap>& other) : id(other.id) {}
  [[nodiscard]] T* allocate(tested::size_t count) { return static_cast<T*>(::operator new(count * sizeof(T))); }
  void deallocate(T* value, tested::size_t) { ::operator delete(value); }
  template<class U> constexpr bool operator==(const stateful_allocator<U, Copy, Move, Swap>& other) const { return id == other.id; }
};

constexpr bool constexpr_vector() {
  tested::vector<int> values{1, 2, 3};
  values.reserve(12);
  values.emplace(values.begin() + 1, 9);
  values.insert(values.end(), 2, 4);
  values.erase(values.begin() + 2);
  values.resize(8, 7);
  values.pop_back();
  tested::array<int, 2> tail{5, 6};
  values.append_range(tail);
  values.insert_range(values.begin(), tail);
  tested::erase(values, 4);
  return values.front() == 5 && values.back() == 6 && values.size() == 9;
}

constexpr bool constexpr_bool_vector() {
  tested::vector<bool> bits{true, false, true};
  bits.push_back(false);
  bits.insert(bits.begin() + 1, true);
  bits.flip();
  bits.erase(bits.begin() + 2);
  return bits.size() == 4 && !bits[0] && !bits[1] && !bits[2] && bits[3];
}

static_assert(tested::contiguous_iterator<tested::vector<int>::iterator>);
static_assert(tested::random_access_iterator<tested::vector<bool>::iterator>);
static_assert(!tested::contiguous_iterator<tested::vector<bool>::iterator>);
static_assert(tested::ranges::contiguous_range<tested::vector<int>>);
static_assert(constexpr_vector());
static_assert(constexpr_bool_vector());
static_assert(__cpp_lib_containers_ranges == 202202L);
static_assert(__cpp_lib_constexpr_vector == 201907L);
static_assert(__cpp_lib_incomplete_container_elements == 201505L);

bool ftl_test() {
  tested::vector<int> values{2, 3};
  values.reserve(20);
  auto data = values.data();
  values.insert(values.begin(), 1);
  values.emplace_back(4);
  tested::array<int, 2> tail{5, 6};
  values.append_range(tail);
  values.insert_range(values.begin() + 2, tail);
  if (values.data() != data || values != tested::vector<int>{1, 2, 5, 6, 3, 4, 5, 6}) return false;
  if (tested::erase_if(values, [](int value) { return value % 2 == 0; }) != 4) return false;
  values.shrink_to_fit();
  if (values.capacity() != values.size()) return false;

  { tested::vector<counted> objects; objects.emplace_back(1); objects.resize(5); objects.erase(objects.begin() + 1, objects.end()); if (counted::alive != 1) return false; }
  if (counted::alive != 0) return false;

  using copy_allocator = stateful_allocator<int, true, false, false>;
  tested::vector<int, copy_allocator> source({1, 2}, copy_allocator(2));
  tested::vector<int, copy_allocator> target({3}, copy_allocator(1));
  target = source;
  if (target.get_allocator().id != 2) return false;

  tested::vector<bool> bits(17, true);
  bits[8] = false;
  auto proxy = bits[0];
  swap(proxy, bits[8]);
  bits.flip();
  const auto copied_bits = bits;
  return bits.front() && !bits[8] && bits.capacity() % 8 == 0 &&
         tested::hash<tested::vector<bool>>{}(bits) ==
             tested::hash<tested::vector<bool>>{}(copied_bits);
}
