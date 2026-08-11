#ifdef FTL_REPLACE_STL
#include <iomanip>
#include <sstream>
namespace tested = std;
#else
#include <ftl/iomanip>
#include <ftl/sstream>
namespace tested = ftl;
#endif
bool ftl_test() {
  tested::ostringstream out;
  out << tested::setbase(16) << tested::setfill('0') << tested::setw(4) << 42;
  if (out.str() != "002a")
    return false;
  tested::string source = "a\\\"b";
  tested::ostringstream quoted;
  quoted << tested::quoted(source);
  if (quoted.str() != "\"a\\\\\\\"b\"")
    return false;
  tested::istringstream input(quoted.str());
  tested::string restored;
  input >> tested::quoted(restored);
  return restored == source;
}
