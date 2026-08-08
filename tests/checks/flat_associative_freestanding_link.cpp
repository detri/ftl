#include <flat_map>
#include <flat_set>
template <class T> struct arena_allocator {
  using value_type = T;
  template <class U> struct rebind {
    using other = arena_allocator<U>;
  };
  arena_allocator() = default;
  template <class U> arena_allocator(const arena_allocator<U> &) {}
  alignas(T) static inline unsigned char storage[sizeof(T) * 32];
  static inline std::size_t used;
  T *allocate(std::size_t count) {
    auto *result = reinterpret_cast<T *>(storage) + used;
    used += count;
    return result;
  }
  void deallocate(T *, std::size_t) {}
  template <class U> bool operator==(const arena_allocator<U> &) const {
    return true;
  }
};
extern "C" int ftl_entry() {
  using keys = std::vector<int, arena_allocator<int>>;
  using mapped = std::vector<int, arena_allocator<int>>;
  std::flat_map<int, int, std::less<int>, keys, mapped> values;
  std::flat_set<int, std::less<int>, keys> set_values;
  values.emplace(2, 3);
  set_values.insert(4);
  return values.begin()->second + *set_values.begin() == 7 ? 0 : 1;
}
