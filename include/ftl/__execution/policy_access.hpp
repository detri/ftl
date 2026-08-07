// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_EXECUTION_POLICY_ACCESS_HEADER
#define FTL_EXECUTION_POLICY_ACCESS_HEADER

#ifdef FTL_REPLACE_STL
#include <type_traits>
#else
#include <ftl/type_traits>
#endif

FTL_BEGIN_NAMESPACE

namespace execution {

class sequenced_policy {};
class parallel_policy {};
class parallel_unsequenced_policy {};
class unsequenced_policy {};

inline constexpr sequenced_policy seq{};
inline constexpr parallel_policy par{};
inline constexpr parallel_unsequenced_policy par_unseq{};
inline constexpr unsequenced_policy unseq{};

} // namespace execution

template <class T> struct is_execution_policy : false_type {};

template <>
struct is_execution_policy<execution::sequenced_policy> : true_type {};

template <>
struct is_execution_policy<execution::parallel_policy> : true_type {};

template <>
struct is_execution_policy<execution::parallel_unsequenced_policy> : true_type {
};

template <>
struct is_execution_policy<execution::unsequenced_policy> : true_type {};

template <class T>
inline constexpr bool is_execution_policy_v = is_execution_policy<T>::value;

FTL_END_NAMESPACE

#endif
