#include <vector>
struct wrong_allocator {
  using value_type = long;
  long* allocate(std::size_t);
  void deallocate(long*, std::size_t);
};
std::vector<int, wrong_allocator> invalid;
