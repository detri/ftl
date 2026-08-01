#include <ftl/cstddef>

#if __cpp_lib_byte != 201603L
#error <cstddef> must advertise std::byte
#endif

int main() {}
