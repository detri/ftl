/*
 * <cassert> is required to reconsider NDEBUG every time it is included.
 *
 * Explicitly control NDEBUG here because Release/RelWithDebInfo builds
 * normally define it globally.
 */

#ifdef NDEBUG
#undef NDEBUG
#endif

#ifdef FTL_REPLACE_STL
#include <cassert>
#else
#include <ftl/cassert>
#endif

constexpr bool constexpr_assert_works() {
  assert(true);
  return true;
}

static_assert(constexpr_assert_works());

bool enabled_assert_evaluates() {
  int evaluations = 0;

  assert(++evaluations == 1);

  return evaluations == 1;
}

/*
 * Reinclude with assertions disabled.
 */

#define NDEBUG

#ifdef FTL_REPLACE_STL
#include <cassert>
#else
#include <ftl/cassert>
#endif

bool disabled_assert_does_not_evaluate() {
  int evaluations = 0;

  assert(++evaluations == 1);

  return evaluations == 0;
}

/*
 * Reinclude with assertions enabled again.
 */

#undef NDEBUG

#ifdef FTL_REPLACE_STL
#include <cassert>
#else
#include <ftl/cassert>
#endif

bool reenabled_assert_evaluates() {
  int evaluations = 0;

  assert(++evaluations == 1);

  return evaluations == 1;
}

bool ftl_test() {
  return enabled_assert_evaluates() && disabled_assert_does_not_evaluate() &&
         reenabled_assert_evaluates();
}
