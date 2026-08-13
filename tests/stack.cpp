#ifdef FTL_REPLACE_STL
#include <array>
#include <format>
#include <memory>
#include <stack>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/format>
#include <ftl/memory>
#include <ftl/stack>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(tested::uses_allocator_v<tested::stack<int>, tested::allocator<int>>);
struct fake_stack_allocator {
  using value_type = int;
  int *allocate(tested::size_t);
};
template<class A> concept stack_allocator_guide = requires(int *first, A allocator) {
  tested::stack(first, first, allocator);
};
static_assert(!stack_allocator_guide<fake_stack_allocator>);

bool ftl_test() {
  tested::array<int, 3> values{1, 2, 3};
  tested::stack first(tested::from_range, values);
  if (first.top() != 3 || first.size() != 3) return false;
  first.push(4);
  first.emplace(5);
  first.push_range(values);
  if (first.top() != 3 || first.size() != 8) return false;
  first.pop();
  tested::stack second(tested::from_range, values);
  if (tested::format("{}", second) != "[1, 2, 3]") return false;
  tested::stack third(tested::from_range, values);
  return second == third && second <= third;
}
