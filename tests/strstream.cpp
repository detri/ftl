#ifdef FTL_REPLACE_STL
#include <strstream>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/strstream>
#include <ftl/type_traits>
namespace tested = ftl;
#endif
static_assert(tested::is_base_of_v<tested::streambuf, tested::strstreambuf>);
bool ftl_test() {
  tested::ostrstream out;
  out << "legacy" << '\0';
  if (out.pcount() != 7)
    return false;
  char *value = out.str();
  bool okay = value && value[0] == 'l' && value[5] == 'y';
  out.freeze(false);
  char source[] = "41";
  tested::istrstream in(source);
  int number = 0;
  in >> number;
  if (number != 41)
    return false;
  char fixed[8]{};
  tested::strstream both(fixed, 8);
  both << "12";
  both.seekg(0);
  number = 0;
  both >> number;
  return okay && number == 12;
}
