#include <valarray>

void operator delete(void*) noexcept {}

extern "C" int ftl_entry() {
  std::valarray<int> values;
  return values.size() == 0 ? 0 : 1;
}
