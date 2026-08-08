#include <vector>

template<class T> struct arena_allocator {
  using value_type = T;
  template<class U> struct rebind { using other = arena_allocator<U>; };
  alignas(T) static inline unsigned char storage[sizeof(T) * 8];
  T* allocate(std::size_t) { return reinterpret_cast<T*>(storage); }
  void deallocate(T*, std::size_t) {}
  template<class U> bool operator==(const arena_allocator<U>&) const { return true; }
};

extern "C" int ftl_entry() {
  std::vector<int, arena_allocator<int>> values;
  values.reserve(4);
  values.push_back(1);
  values.push_back(2);
  return values[0] + values[1] == 3 ? 0 : 1;
}
