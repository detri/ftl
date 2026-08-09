#ifdef FTL_REPLACE_STL
#include <cfenv>
namespace tested = std;
#else
#include <ftl/cfenv>
namespace tested = ftl;
#endif

static_assert(sizeof(tested::fenv_t) >= sizeof(tested::fexcept_t));

bool ftl_test() {
  tested::fenv_t original{};
  if (tested::fegetenv(&original) != 0 ||
      tested::fesetround(FE_DOWNWARD) != 0 ||
      tested::fegetround() != FE_DOWNWARD ||
      tested::feclearexcept(FE_ALL_EXCEPT) != 0 ||
      tested::feraiseexcept(FE_INVALID | FE_INEXACT) != 0 ||
      tested::fetestexcept(FE_ALL_EXCEPT) != (FE_INVALID | FE_INEXACT)) {
    tested::fesetenv(&original);
    return false;
  }

  tested::fexcept_t flags{};
  tested::fenv_t held{};
  const bool works =
      tested::fegetexceptflag(&flags, FE_ALL_EXCEPT) == 0 &&
      flags == (FE_INVALID | FE_INEXACT) &&
      tested::feholdexcept(&held) == 0 &&
      tested::fetestexcept(FE_ALL_EXCEPT) == 0 &&
      tested::feraiseexcept(FE_OVERFLOW) == 0 &&
      tested::feupdateenv(&held) == 0 &&
      tested::fetestexcept(FE_ALL_EXCEPT) ==
          (FE_INVALID | FE_OVERFLOW | FE_INEXACT);
  return tested::fesetenv(&original) == 0 && works;
}
