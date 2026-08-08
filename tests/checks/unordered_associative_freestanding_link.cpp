#include <unordered_map>
#include <unordered_set>
template <class T> struct arena_allocator {
  using value_type = T;
  template <class U> struct rebind {
    using other = arena_allocator<U>;
  };
  arena_allocator() = default;
  template <class U> arena_allocator(const arena_allocator<U> &) {}
  alignas(T) static inline unsigned char storage[sizeof(T) * 64];
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
  std::unordered_map<int, int, std::hash<int>, std::equal_to<int>,
                     arena_allocator<std::pair<const int, int>>>
      values;
  std::unordered_set<int, std::hash<int>, std::equal_to<int>,
                     arena_allocator<int>>
      keys;
  values.emplace(2, 3);
  keys.insert(4);
  return values.begin()->second + *keys.begin() == 7 ? 0 : 1;
}
