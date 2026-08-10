#ifdef FTL_REPLACE_STL
#include <clocale>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/clocale>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(
    tested::is_same_v<decltype(tested::localeconv()), tested::lconv *>);

static_assert(
    tested::is_same_v<decltype(tested::setlocale(LC_ALL, nullptr)), char *>);

static_assert(LC_ALL != LC_COLLATE);
static_assert(LC_ALL != LC_CTYPE);
static_assert(LC_ALL != LC_MONETARY);
static_assert(LC_ALL != LC_NUMERIC);
static_assert(LC_ALL != LC_TIME);

bool ftl_test() {
  const char *current = tested::setlocale(LC_ALL, nullptr);

  if (current == nullptr) {
    return false;
  }

  tested::lconv *conventions = tested::localeconv();

  if (conventions == nullptr) {
    return false;
  }

  if (conventions->decimal_point == nullptr ||
      conventions->decimal_point[0] == '\0') {
    return false;
  }

  return true;
}
