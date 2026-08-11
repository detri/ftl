#ifdef FTL_REPLACE_STL
#include <spanstream>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/spanstream>
#include <ftl/type_traits>
namespace tested = ftl;
#endif
#if __cpp_lib_spanstream != 202106L
#error bad spanstream feature macro
#endif
static_assert(tested::is_base_of_v<tested::streambuf, tested::spanbuf>);
static_assert(tested::is_base_of_v<tested::istream, tested::ispanstream>);
bool ftl_test() {
  char storage[8]{};
  tested::ospanstream out{tested::span<char>(storage)};
  out << "abc";
  if (out.span().size() != 3 || storage[2] != 'c')
    return false;
  out << "defghijk";
  if (!out.fail())
    return false;
  char input[] = {'4', '2'};
  tested::ispanstream in{tested::span<char>(input)};
  int value = 0;
  in >> value;
  if (value != 42)
    return false;
  char replacement[] = {'7'};
  in.span(tested::span<char>(replacement));
  in.clear();
  value = 0;
  in >> value;
  return value == 7;
}
