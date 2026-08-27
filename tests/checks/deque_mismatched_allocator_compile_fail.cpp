#include <deque>
struct bad_allocator { using value_type = long; };
std::deque<int, bad_allocator> value;
