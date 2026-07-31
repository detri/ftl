#ifdef FTL_REPLACE_STL
#include <cstring>
namespace tested = std;
#else
#include <ftl/cstring>
namespace tested = ftl;
#endif

bool ftl_test() {
    char source[] = "ftl";
    char destination[4]{};
    tested::memcpy(destination, source, sizeof(source));
    return tested::strcmp(destination, "ftl") == 0;
}
