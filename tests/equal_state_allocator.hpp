template<class T>
struct equal_state_allocator {
  using value_type = T;
  using propagate_on_container_copy_assignment = tested::true_type;
  template<class U> struct rebind { using other = equal_state_allocator<U>; };

  int id{};

  equal_state_allocator() = default;
  explicit equal_state_allocator(int value) : id(value) {}
  template<class U>
  equal_state_allocator(const equal_state_allocator<U>& other) : id(other.id) {}

  T* allocate(tested::size_t count) {
    return static_cast<T*>(::operator new(count * sizeof(T)));
  }
  void deallocate(T* value, tested::size_t) { ::operator delete(value); }

  template<class U>
  bool operator==(const equal_state_allocator<U>&) const { return true; }
};
