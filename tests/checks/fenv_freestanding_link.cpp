#include <cfenv>

extern "C" int ftl_entry() {
  std::fenv_t environment{};
  if (std::fegetenv(&environment) != 0 ||
      std::feclearexcept(FE_ALL_EXCEPT) != 0 ||
      std::fesetround(FE_TONEAREST) != 0)
    return 1;
  return std::fesetenv(&environment);
}
