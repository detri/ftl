#ifdef FTL_REPLACE_STL
#include <cstdlib>
namespace tested = std;
#else
#include <ftl/cstdlib>
namespace tested = ftl;
#endif

#if __cpp_lib_constexpr_cmath != 202202L
#error <cstdlib> must advertise constexpr integer math
#endif

static_assert(sizeof(tested::size_t) == sizeof(void*));
static_assert(tested::abs(-7) == 7 && tested::labs(-8L) == 8L);
static_assert(tested::abs(-2.5) == 2.5);
static_assert(noexcept(tested::abort()));
static_assert(noexcept(tested::_Exit(0)));
static_assert(noexcept(tested::quick_exit(0)));
static_assert(noexcept(tested::atexit(nullptr)));
static_assert(noexcept(tested::at_quick_exit(nullptr)));
static_assert(tested::llabs(-9LL) == 9LL);
static_assert(tested::div(-7, 3).quot == -2 && tested::div(-7, 3).rem == -1);
static_assert(tested::ldiv(7L, 3L).quot == 2);
static_assert(tested::lldiv(7LL, 3LL).rem == 1);
static_assert(EXIT_SUCCESS != EXIT_FAILURE && RAND_MAX >= 32767);

int throwing_compare(const void *, const void *) { throw 19; }
int integer_compare(const void *left, const void *right) {
  const int a = *static_cast<const int *>(left);
  const int b = *static_cast<const int *>(right);
  return (a > b) - (a < b);
}

bool ftl_test() {
    void* memory = tested::malloc(16);
    tested::free(memory);
    if (MB_CUR_MAX < 1 || tested::atoi("42") != 42 ||
        tested::strtol("17", nullptr, 10) != 17)
      return false;
    int ordered[] = {4, 1, 3, 2};
    tested::qsort(ordered, 4, sizeof(int), integer_compare);
    const int key = 3;
    const auto *found = static_cast<const int *>(
        tested::bsearch(&key, ordered, 4, sizeof(int), integer_compare));
    if (ordered[0] != 1 || ordered[1] != 2 || ordered[2] != 3 ||
        ordered[3] != 4 || !found || *found != 3)
      return false;
#if FTL_HAS_EXCEPTIONS
    int values[] = {2, 1};
    try {
      tested::qsort(values, 2, sizeof(int), throwing_compare);
      return false;
    } catch (...) {
    }
#endif
    return true;
}
