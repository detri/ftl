#ifdef FTL_REPLACE_STL
#include <iostream>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/iostream>
#include <ftl/type_traits>
namespace tested = ftl;
#endif
static_assert(tested::is_same_v<decltype(tested::cin), tested::istream>);
static_assert(tested::is_same_v<decltype(tested::cout), tested::ostream>);
bool ftl_test() {
  return tested::cin.rdbuf() != nullptr && tested::cout.rdbuf() != nullptr &&
         tested::cerr.rdbuf() != nullptr &&
         tested::cin.tie() == &tested::cout &&
         (tested::cerr.flags() & tested::ios_base::unitbuf) != 0;
}
