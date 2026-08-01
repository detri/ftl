#include <utility>

struct target {};
struct source { operator target() const; };

std::pair<const target&, int> invalid{source{}, 2};
