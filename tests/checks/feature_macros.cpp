#include <ftl/cstddef>
#include <ftl/utility>

#if __cpp_lib_byte != 201603L
#error <cstddef> must advertise std::byte
#endif

#if __cpp_lib_is_invocable != 201703L
#error <type_traits> must advertise invocation traits
#endif
#if __cpp_lib_is_scoped_enum != 202011L
#error <type_traits> must advertise scoped-enum detection
#endif
#if __cpp_lib_concepts != 202002L
#error <concepts> must advertise standard concepts
#endif
#if __cpp_lib_three_way_comparison != 201907L
#error <compare> must advertise three-way comparison
#endif
#if __cpp_lib_forward_like != 202207L
#error <utility> must advertise forward_like
#endif
#if __cpp_lib_unreachable != 202202L
#error <utility> must advertise unreachable
#endif

int main() {}
