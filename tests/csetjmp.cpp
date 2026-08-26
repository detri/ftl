#include <csetjmp>
namespace tested = std;

bool ftl_test() {
    tested::jmp_buf environment{};
    int result = setjmp(environment);
    if (result == 0) tested::longjmp(environment, 7);
    return result == 7;
}
