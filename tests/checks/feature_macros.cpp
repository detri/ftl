#include <ftl/cstddef>

#if __cpp_lib_byte != 201603L
#error <cstddef> must advertise std::byte
#endif

#if __cplusplus < 202302L && (!defined(_MSVC_LANG) || _MSVC_LANG < 202302L)
#error FTL requires C++23
#endif

int main() {}
