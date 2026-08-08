#include <deque>

template<class T> struct arena_allocator {
  using value_type = T;
  template<class U> struct rebind { using other = arena_allocator<U>; };
  arena_allocator() = default;
  template<class U> arena_allocator(const arena_allocator<U>&) {}
  alignas(T) static inline unsigned char storage[sizeof(T) * 256];
  T* allocate(std::size_t) { return reinterpret_cast<T*>(storage); }
  void deallocate(T*, std::size_t) {}
  template<class U> bool operator==(const arena_allocator<U>&) const { return true; }
};

extern "C" int ftl_entry() {
  std::deque<int, arena_allocator<int>> values;
  values.push_front(1);
  values.push_back(2);
  return values.front() + values.back() == 3 ? 0 : 1;
}
