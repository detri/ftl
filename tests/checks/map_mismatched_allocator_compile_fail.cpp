#include <map>
template <class> struct bad_allocator {
  using value_type = int;
};
std::map<int, int, std::less<int>, bad_allocator<int>> value;
