#ifdef FTL_REPLACE_STL
#include <array>
#include <memory>
#include <queue>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/memory>
#include <ftl/queue>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(tested::uses_allocator_v<tested::queue<int>, tested::allocator<int>>);
static_assert(tested::uses_allocator_v<tested::priority_queue<int>, tested::allocator<int>>);

bool ftl_test() {
  tested::array<int, 3> values{3, 1, 4};
  tested::queue first(tested::from_range, values);
  first.push(2);
  first.push_range(values);
  if (first.front() != 3 || first.back() != 4 || first.size() != 7) return false;
  first.pop();
  tested::queue second(tested::from_range, values);
  if (!(second == tested::queue(tested::from_range, values))) return false;

  tested::priority_queue heap(tested::from_range, values);
  if (heap.top() != 4) return false;
  heap.push(9);
  heap.emplace(2);
  heap.push_range(values);
  if (heap.top() != 9 || heap.size() != 8) return false;
  heap.pop();
  return heap.top() == 4;
}
