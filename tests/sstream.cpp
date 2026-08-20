#ifdef FTL_REPLACE_STL
#include <limits>
#include <sstream>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/limits>
#include <ftl/sstream>
#include <ftl/type_traits>
namespace tested = ftl;
#endif
static_assert(tested::is_base_of_v<tested::streambuf, tested::stringbuf>);
static_assert(tested::is_base_of_v<tested::istream, tested::istringstream>);
static_assert(tested::is_base_of_v<tested::ostream, tested::ostringstream>);
static_assert(tested::is_base_of_v<tested::iostream, tested::stringstream>);
bool ftl_test() {
  tested::stringbuf checked("abc", tested::ios_base::in | tested::ios_base::out);
  if (checked.pubseekoff(tested::numeric_limits<tested::streamoff>::max(),
                         tested::ios_base::cur,
                         tested::ios_base::in) != tested::streampos(-1))
    return false;
  tested::ostringstream out;
  out << 12 << ' ' << 34;
  if (out.str() != "12 34")
    return false;
  tested::istringstream in(out.str());
  int a = 0, b = 0;
  in >> a >> b;
  if (a != 12 || b != 34)
    return false;
  tested::stringstream both("abc", tested::ios_base::in |
                                       tested::ios_base::out |
                                       tested::ios_base::ate);
  both << 'd';
  if (both.view() != "abcd")
    return false;
  both.seekg(1, tested::ios_base::beg);
  char c{};
  both.get(c);
  if (c != 'b')
    return false;
  tested::stringstream moved(static_cast<tested::stringstream &&>(both));
  return moved.rdbuf() != nullptr && moved.view() == "abcd";
}
