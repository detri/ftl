#include <forward_list>
template <class> struct bad_allocator {
  using value_type = long;
};
std::forward_list<int, bad_allocator<int>> value;
