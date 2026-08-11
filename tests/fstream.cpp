#ifdef FTL_REPLACE_STL
#include <cstdio>
#include <fstream>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/cstdio>
#include <ftl/fstream>
#include <ftl/type_traits>
namespace tested = ftl;
#endif
static_assert(tested::is_base_of_v<tested::streambuf, tested::filebuf>);
static_assert(tested::is_base_of_v<tested::istream, tested::ifstream>);
static_assert(tested::is_base_of_v<tested::ostream, tested::ofstream>);
bool ftl_test() {
  const char *name =
#ifdef FTL_REPLACE_STL
      "ftl_fstream_replace.tmp";
#else
      "ftl_fstream_normal.tmp";
#endif
  {
    tested::ofstream out(name,
                         tested::ios_base::binary | tested::ios_base::trunc);
    if (!out.is_open())
      return false;
    out << "17 25";
    out.close();
    if (out.fail())
      return false;
  }
  tested::ifstream in(name, tested::ios_base::binary);
  int a = 0, b = 0;
  in >> a >> b;
  in.close();
  tested::remove(name);
  return a == 17 && b == 25;
}
