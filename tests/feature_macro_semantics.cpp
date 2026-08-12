#ifdef FTL_REPLACE_STL
#include <atomic>
#include <filesystem>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <new>
#include <ostream>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <vector>
namespace tested = std;
#else
#include <ftl/atomic>
#include <ftl/filesystem>
#include <ftl/iterator>
#include <ftl/limits>
#include <ftl/list>
#include <ftl/map>
#include <ftl/memory>
#include <ftl/new>
#include <ftl/ostream>
#include <ftl/queue>
#include <ftl/set>
#include <ftl/stack>
#include <ftl/string>
#include <ftl/string_view>
#include <ftl/type_traits>
#include <ftl/typeinfo>
#include <ftl/unordered_map>
#include <ftl/unordered_set>
#include <ftl/vector>
namespace tested = ftl;
#endif

/*
 * __cpp_lib_addressof_constexpr
 *
 * Do not accidentally use an overloaded operator&.
 */
struct hostile_address {
  constexpr hostile_address *operator&() noexcept { return nullptr; }

  constexpr const hostile_address *operator&() const noexcept {
    return nullptr;
  }
};

constexpr bool constexpr_addressof_works() {
  hostile_address value;

  return tested::addressof(value) != nullptr;
}

static_assert(constexpr_addressof_works());

/*
 * __cpp_lib_allocator_traits_is_always_equal
 *
 * Test both the empty-allocator default and an explicit override.
 */
template <class T> struct empty_allocator {
  using value_type = T;

  T *allocate(tested::size_t);
  void deallocate(T *, tested::size_t) noexcept;
};

template <class T> struct unequal_allocator {
  using value_type = T;
  using is_always_equal = tested::false_type;

  int state = 0;

  T *allocate(tested::size_t);
  void deallocate(T *, tested::size_t) noexcept;
};

static_assert(
    tested::allocator_traits<empty_allocator<int>>::is_always_equal::value);

static_assert(
    !tested::allocator_traits<unequal_allocator<int>>::is_always_equal::value);

/*
 * __cpp_lib_make_reverse_iterator
 * __cpp_lib_nonmember_container_access
 * __cpp_lib_ssize
 */
constexpr bool iterator_utility_features_work() {
  int values[] = {1, 2, 3};

  auto reverse = tested::make_reverse_iterator(values + 3);

  return *reverse == 3 && tested::size(values) == 3 &&
         tested::ssize(values) == 3 && tested::data(values) == values &&
         !tested::empty(values);
}

static_assert(iterator_utility_features_work());

/*
 * __cpp_lib_null_iterators
 *
 * Check both a contiguous/pointer-like iterator and a node iterator.
 */
bool null_iterators_work() {
  tested::vector<int>::iterator vector_first{};
  tested::vector<int>::iterator vector_second{};

  tested::list<int>::iterator list_first{};
  tested::list<int>::iterator list_second{};

  return vector_first == vector_second && list_first == list_second;
}

/*
 * __cpp_lib_list_remove_return_type
 */
struct remove_even {
  bool operator()(int value) const { return value % 2 == 0; }
};

using integer_list = tested::list<int>;

static_assert(
    tested::is_same_v<decltype(tested::declval<integer_list &>().remove(
                          tested::declval<const int &>())),
                      integer_list::size_type>);

static_assert(tested::is_same_v<decltype(tested::declval<integer_list &>()
                                             .remove_if(remove_even{})),
                                integer_list::size_type>);

/*
 * __cpp_lib_string_udls
 *
 * char8_t string/string_view literals simultaneously exercise part of
 * __cpp_lib_char8_t's string-library surface.
 */
namespace string_literal_semantics {

using namespace tested::literals::string_literals;
using namespace tested::literals::string_view_literals;

static_assert(tested::is_same_v<decltype("ftl"s), tested::string>);

static_assert(tested::is_same_v<decltype(u8"ftl"s), tested::u8string>);

static_assert(tested::is_same_v<decltype(u8"ftl"sv), tested::u8string_view>);

} // namespace string_literal_semantics

/*
 * __cpp_lib_char8_t
 *
 * P0482 added char8_t specializations/aliases across several library
 * facilities. P1423 then updated the macro value to 201907L for the
 * compatibility corrections.
 */

#ifndef ATOMIC_CHAR8_T_LOCK_FREE
#error char8_t atomic lock-free macro is missing
#endif

static_assert(
    tested::is_same_v<tested::atomic_char8_t, tested::atomic<char8_t>>);

static_assert(tested::numeric_limits<char8_t>::is_specialized);

static_assert(tested::is_same_v<
              typename tested::char_traits<char8_t>::char_type, char8_t>);

static_assert(
    tested::is_same_v<tested::u8string, tested::basic_string<char8_t>>);

static_assert(tested::is_same_v<tested::u8string_view,
                                tested::basic_string_view<char8_t>>);

static_assert(tested::is_same_v<
              tested::u8streampos,
              tested::fpos<typename tested::char_traits<char8_t>::state_type>>);

template <class Stream, class Value>
concept stream_insertable =
    requires(Stream &stream, Value value) { stream << value; };

/*
 * P1423 deliberately makes these ill-formed instead of allowing char8_t
 * to fall through to integer/pointer formatting.
 */
static_assert(!stream_insertable<tested::ostream, char8_t>);

static_assert(!stream_insertable<tested::ostream, const char8_t *>);

static_assert(!stream_insertable<tested::wostream, char8_t>);

static_assert(!stream_insertable<tested::wostream, const char8_t *>);

bool char8_filesystem_works() {
  const char8_t source[] = u8"ftl";

  auto path = tested::filesystem::u8path(source, source + 3);

  auto result = path.u8string();

  return result.size() == 3 && result[0] == u8'f' && result[1] == u8't' &&
         result[2] == u8'l';
}

/*
 * __cpp_lib_atomic_value_initialization
 */
bool atomic_value_initialization_works() {
  tested::atomic<int> value;
  tested::atomic_flag flag;

  return value.load() == 0 && !flag.test();
}

/*
 * __cpp_lib_adaptor_iterator_pair_constructor
 *
 * Exercise queue, priority_queue, and stack, including CTAD.
 */
bool adaptor_iterator_pair_constructors_work() {
  int values[] = {3, 1, 4};

  tested::queue queue(values, values + 3);

  static_assert(tested::is_same_v<decltype(queue), tested::queue<int>>);

  if (queue.size() != 3 || queue.front() != 3 || queue.back() != 4) {
    return false;
  }

  tested::stack stack(values, values + 3);

  static_assert(tested::is_same_v<decltype(stack), tested::stack<int>>);

  if (stack.size() != 3 || stack.top() != 4) {
    return false;
  }

  tested::priority_queue priority(values, values + 3);

  static_assert(
      tested::is_same_v<decltype(priority), tested::priority_queue<int>>);

  return priority.size() == 3 && priority.top() == 4;
}

/*
 * __cpp_lib_generic_associative_lookup
 * __cpp_lib_generic_unordered_lookup
 * __cpp_lib_associative_heterogeneous_erasure
 *
 * The probe deliberately cannot construct a key. Therefore these calls can
 * only succeed through the heterogeneous overloads.
 */
struct semantic_probe {
  int value;
};

struct semantic_key {
  int value;

  explicit semantic_key(int input) : value(input) {}

  semantic_key(const semantic_key &) = default;
  semantic_key(semantic_key &&) = default;
  semantic_key &operator=(const semantic_key &) = default;
  semantic_key &operator=(semantic_key &&) = default;
};

static_assert(!tested::is_constructible_v<semantic_key, semantic_probe>);

struct transparent_less {
  using is_transparent = void;

  bool operator()(const semantic_key &left,
                  const semantic_key &right) const noexcept {
    return left.value < right.value;
  }

  bool operator()(const semantic_key &left,
                  const semantic_probe &right) const noexcept {
    return left.value < right.value;
  }

  bool operator()(const semantic_probe &left,
                  const semantic_key &right) const noexcept {
    return left.value < right.value;
  }
};

struct transparent_hash {
  using is_transparent = void;

  tested::size_t operator()(const semantic_key &key) const noexcept {
    return static_cast<tested::size_t>(key.value);
  }

  tested::size_t operator()(const semantic_probe &key) const noexcept {
    return static_cast<tested::size_t>(key.value);
  }
};

struct transparent_equal {
  using is_transparent = void;

  bool operator()(const semantic_key &left,
                  const semantic_key &right) const noexcept {
    return left.value == right.value;
  }

  bool operator()(const semantic_key &left,
                  const semantic_probe &right) const noexcept {
    return left.value == right.value;
  }

  bool operator()(const semantic_probe &left,
                  const semantic_key &right) const noexcept {
    return left.value == right.value;
  }
};

using semantic_map = tested::map<semantic_key, int, transparent_less>;

using semantic_set = tested::set<semantic_key, transparent_less>;

using semantic_unordered_map =
    tested::unordered_map<semantic_key, int, transparent_hash,
                          transparent_equal>;

using semantic_unordered_set =
    tested::unordered_set<semantic_key, transparent_hash, transparent_equal>;

template <class Container>
concept has_heterogeneous_key_operations =
    requires(Container &container, semantic_probe probe) {
      container.find(probe);
      container.count(probe);
      container.contains(probe);
      container.erase(probe);
      container.extract(probe);
    };

static_assert(has_heterogeneous_key_operations<semantic_map>);

static_assert(has_heterogeneous_key_operations<semantic_set>);

static_assert(has_heterogeneous_key_operations<semantic_unordered_map>);

static_assert(has_heterogeneous_key_operations<semantic_unordered_set>);

bool heterogeneous_associative_operations_work() {
  semantic_map ordered_map;

  ordered_map.emplace(semantic_key{1}, 10);
  ordered_map.emplace(semantic_key{2}, 20);

  auto ordered_found = ordered_map.find(semantic_probe{2});

  if (ordered_found == ordered_map.end() || ordered_found->second != 20 ||
      ordered_map.erase(semantic_probe{1}) != 1) {
    return false;
  }

  semantic_set ordered_set;

  ordered_set.emplace(semantic_key{3});

  if (ordered_set.find(semantic_probe{3}) == ordered_set.end() ||
      ordered_set.erase(semantic_probe{3}) != 1) {
    return false;
  }

  semantic_unordered_map unordered_map;

  unordered_map.emplace(semantic_key{4}, 40);
  unordered_map.emplace(semantic_key{5}, 50);

  auto unordered_found = unordered_map.find(semantic_probe{5});

  if (unordered_found == unordered_map.end() || unordered_found->second != 50 ||
      unordered_map.erase(semantic_probe{4}) != 1) {
    return false;
  }

  semantic_unordered_set unordered_set;

  unordered_set.emplace(semantic_key{6});

  return unordered_set.find(semantic_probe{6}) != unordered_set.end() &&
         unordered_set.erase(semantic_probe{6}) == 1;
}

/*
 * __cpp_lib_destroying_delete
 *
 * The core language recognizes destroying delete specifically through
 * std::destroying_delete_t. Therefore actual delete-expression dispatch can
 * only be tested when FTL_REPLACE_STL installs the facility in namespace std.
 *
 * In normal mode, verify the FTL library surface exists.
 */

static_assert(tested::is_same_v<decltype(tested::destroying_delete),
                                const tested::destroying_delete_t>);

#ifdef FTL_REPLACE_STL

struct destroying_delete_probe {
  inline static bool destroyed = false;

  ~destroying_delete_probe() { destroyed = true; }

  static void operator delete(destroying_delete_probe *pointer,
                              tested::destroying_delete_t) noexcept {
    pointer->~destroying_delete_probe();
    ::operator delete(pointer);
  }
};

bool destroying_delete_works() {
  destroying_delete_probe::destroyed = false;

  auto *value = new destroying_delete_probe;

  delete value;

  return destroying_delete_probe::destroyed;
}

#else

bool destroying_delete_works() { return true; }

#endif

/*
 * __cpp_lib_constexpr_typeinfo
 *
 * This is intentionally the test expected to catch the current non-MSVC
 * replacement-mode hole until type_info::operator== is genuinely constexpr.
 */
#if defined(_CPPRTTI) || defined(__GXX_RTTI)

static_assert(typeid(int) == typeid(int));

static_assert(!(typeid(int) == typeid(long)));

#endif

bool ftl_test() {
  return null_iterators_work() && char8_filesystem_works() &&
         atomic_value_initialization_works() &&
         adaptor_iterator_pair_constructors_work() &&
         heterogeneous_associative_operations_work() &&
         destroying_delete_works();
}
