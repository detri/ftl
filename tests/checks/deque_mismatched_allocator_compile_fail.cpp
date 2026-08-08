#define FTL_REPLACE_STL
#include <deque>
struct bad_allocator { using value_type = long; };
std::deque<int, bad_allocator> value;
