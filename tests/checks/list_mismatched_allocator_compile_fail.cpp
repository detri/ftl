#include <list>
template <class> struct bad_allocator {
  using value_type = long;
};
std::list<int, bad_allocator<int>> value;
