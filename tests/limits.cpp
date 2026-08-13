#ifdef FTL_REPLACE_STL
#include <limits>
namespace tested = std;
#else
#include <ftl/limits>
namespace tested = ftl;
#endif

static_assert(!tested::numeric_limits<void>::is_specialized);
static_assert(tested::numeric_limits<bool>::min() == false);
static_assert(tested::numeric_limits<bool>::max() == true);
static_assert(tested::numeric_limits<unsigned char>::digits == 8);
static_assert(tested::numeric_limits<int>::is_signed);
static_assert(tested::numeric_limits<int>::min() < 0);
static_assert(tested::numeric_limits<unsigned>::min() == 0);
static_assert(tested::numeric_limits<unsigned>::max() > 0);
static_assert(tested::numeric_limits<const long long>::is_specialized);
static_assert(tested::numeric_limits<float>::is_iec559);
static_assert(tested::numeric_limits<float>::epsilon() > 0);
static_assert(tested::numeric_limits<double>::infinity() > tested::numeric_limits<double>::max());
#if defined(__SIZEOF_INT128__)
static_assert(tested::numeric_limits<__int128>::is_specialized);
static_assert(tested::numeric_limits<__int128>::digits == 127);
static_assert(tested::numeric_limits<unsigned __int128>::digits == 128);
static_assert(tested::numeric_limits<__int128>::min() < 0);
#endif

#ifndef FTL_REPLACE_STL
template<class T>
concept has_obsolete_denorm_members = requires {
    T::has_denorm;
    T::has_denorm_loss;
};
static_assert(!has_obsolete_denorm_members<tested::numeric_limits<float>>);
#endif

bool ftl_test() { return true; }
