#ifdef FTL_REPLACE_STL
#include <execution>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/execution>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

#if !defined(__cpp_lib_execution)
#error "__cpp_lib_execution must be defined"
#endif

static_assert(__cpp_lib_execution >= 201902L);

using sequenced_policy = tested::execution::sequenced_policy;

using parallel_policy = tested::execution::parallel_policy;

using parallel_unsequenced_policy =
    tested::execution::parallel_unsequenced_policy;

using unsequenced_policy = tested::execution::unsequenced_policy;

static_assert(
    tested::is_same_v<tested::remove_cv_t<decltype(tested::execution::seq)>,
                      sequenced_policy>);

static_assert(
    tested::is_same_v<tested::remove_cv_t<decltype(tested::execution::par)>,
                      parallel_policy>);

static_assert(tested::is_same_v<
              tested::remove_cv_t<decltype(tested::execution::par_unseq)>,
              parallel_unsequenced_policy>);

static_assert(
    tested::is_same_v<tested::remove_cv_t<decltype(tested::execution::unseq)>,
                      unsequenced_policy>);

static_assert(!tested::is_same_v<sequenced_policy, parallel_policy>);

static_assert(
    !tested::is_same_v<sequenced_policy, parallel_unsequenced_policy>);

static_assert(!tested::is_same_v<sequenced_policy, unsequenced_policy>);

static_assert(!tested::is_same_v<parallel_policy, parallel_unsequenced_policy>);

static_assert(!tested::is_same_v<parallel_policy, unsequenced_policy>);

static_assert(
    !tested::is_same_v<parallel_unsequenced_policy, unsequenced_policy>);

static_assert(tested::is_execution_policy_v<sequenced_policy>);

static_assert(tested::is_execution_policy_v<parallel_policy>);

static_assert(tested::is_execution_policy_v<parallel_unsequenced_policy>);

static_assert(tested::is_execution_policy_v<unsequenced_policy>);

static_assert(!tested::is_execution_policy_v<int>);

static_assert(!tested::is_execution_policy_v<const sequenced_policy>);

static_assert(!tested::is_execution_policy_v<sequenced_policy &>);

static_assert(tested::is_execution_policy_v<
              tested::remove_cvref_t<decltype((tested::execution::seq))>>);

static_assert(tested::is_execution_policy_v<
              tested::remove_cvref_t<decltype((tested::execution::par))>>);

static_assert(
    tested::is_execution_policy_v<
        tested::remove_cvref_t<decltype((tested::execution::par_unseq))>>);

static_assert(tested::is_execution_policy_v<
              tested::remove_cvref_t<decltype((tested::execution::unseq))>>);

bool ftl_test() {
  const auto *first = &tested::execution::seq;
  const auto *second = &tested::execution::seq;
  const auto *third = &tested::execution::par;
  const auto *fourth = &tested::execution::par;

  return first == second && third == fourth;
}