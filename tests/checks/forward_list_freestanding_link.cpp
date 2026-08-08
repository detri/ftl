#include <forward_list>
template <class T> struct arena_allocator {
  using value_type = T;
  template <class U> struct rebind {
    using other = arena_allocator<U>;
  };
  arena_allocator() = default;
  template <class U> arena_allocator(const arena_allocator<U> &) {}
  alignas(T) static inline unsigned char storage[sizeof(T) * 16];
  static inline std::size_t used;
  T *allocate(std::size_t n) {
    auto *p = reinterpret_cast<T *>(storage) + used;
    used += n;
    return p;
  }
  void deallocate(T *, std::size_t) {}
  template <class U> bool operator==(const arena_allocator<U> &) const {
    return true;
  }
};
extern "C" int ftl_entry() {
  std::forward_list<int, arena_allocator<int>> values{1, 2};
  values.reverse();
  values.sort();
  return values.front() == 2;
}
