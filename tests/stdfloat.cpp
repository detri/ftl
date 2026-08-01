#ifdef FTL_REPLACE_STL
#include <stdfloat>
#include <limits>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/stdfloat>
#include <ftl/limits>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

#ifdef __STDCPP_FLOAT16_T__
static_assert(tested::is_same_v<tested::float16_t, decltype(0.0f16)>);
static_assert(tested::is_floating_point_v<tested::float16_t>);
static_assert(tested::numeric_limits<tested::float16_t>::is_specialized);
#endif
#ifdef __STDCPP_FLOAT32_T__
static_assert(tested::is_same_v<tested::float32_t, decltype(0.0f32)>);
static_assert(tested::is_floating_point_v<tested::float32_t>);
static_assert(tested::numeric_limits<tested::float32_t>::is_specialized);
#endif
#ifdef __STDCPP_FLOAT64_T__
static_assert(tested::is_same_v<tested::float64_t, decltype(0.0f64)>);
static_assert(tested::is_floating_point_v<tested::float64_t>);
static_assert(tested::numeric_limits<tested::float64_t>::is_specialized);
#endif
#ifdef __STDCPP_FLOAT128_T__
static_assert(tested::is_same_v<tested::float128_t, decltype(0.0f128)>);
static_assert(tested::is_floating_point_v<tested::float128_t>);
static_assert(tested::numeric_limits<tested::float128_t>::is_specialized);
#endif
#ifdef __STDCPP_BFLOAT16_T__
static_assert(tested::is_same_v<tested::bfloat16_t, decltype(0.0bf16)>);
static_assert(tested::is_floating_point_v<tested::bfloat16_t>);
static_assert(tested::numeric_limits<tested::bfloat16_t>::is_specialized);
#endif

bool ftl_test() { return true; }
