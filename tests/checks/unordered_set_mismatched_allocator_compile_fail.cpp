#include <unordered_set>
template <class> struct bad_allocator {
  using value_type = long;
};
std::unordered_set<int, std::hash<int>, std::equal_to<int>, bad_allocator<int>>
    value;
