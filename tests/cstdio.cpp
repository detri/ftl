#ifdef FTL_REPLACE_STL
#include <cstdio>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/cstdio>
#include <ftl/type_traits>
namespace tested = ftl;
#endif
using print_type = int (*)(const char *, ...);
static_assert(tested::is_same_v<
              decltype(static_cast<print_type>(&tested::printf)), print_type>);
bool ftl_test() {
  tested::FILE *file = tested::tmpfile();
  if (!file)
    return false;
  const char text[] = "stdio";
  if (tested::fwrite(text, 1, 5, file) != 5)
    return false;
  if (tested::fseek(file, 0, SEEK_SET) != 0)
    return false;
  char read[6]{};
  bool okay = tested::fread(read, 1, 5, file) == 5 && read[4] == 'o';
  return tested::fclose(file) == 0 && okay;
}
