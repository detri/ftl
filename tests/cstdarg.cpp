#ifdef FTL_REPLACE_STL
#include <cstdarg>
namespace tested = std;
#else
#include <ftl/cstdarg>
namespace tested = ftl;
#endif

int sum(int count, ...) {
    tested::va_list arguments;
    va_start(arguments, count);
    tested::va_list copy;
    va_copy(copy, arguments);
    int result = 0;
    for (int index = 0; index < count; ++index) result += va_arg(copy, int);
    va_end(copy);
    va_end(arguments);
    return result;
}

struct large_argument {
    long long first;
    long long second;
};

large_argument read_large(int marker, ...) {
    tested::va_list arguments;
    va_start(arguments, marker);
    auto result = va_arg(arguments, large_argument);
    va_end(arguments);
    return result;
}

bool ftl_test() {
    const auto large = read_large(0, large_argument{11, 17});
    return sum(3, 2, 3, 4) == 9 && large.first == 11 && large.second == 17;
}
