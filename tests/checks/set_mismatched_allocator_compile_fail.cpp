#include <set>
template <class> struct bad_allocator {
  using value_type = long;
};
std::set<int, std::less<int>, bad_allocator<int>> value;
