#include <unordered_map>
template <class> struct bad_allocator {
  using value_type = int;
};
std::unordered_map<int, int, std::hash<int>, std::equal_to<int>,
                   bad_allocator<int>>
    value;
