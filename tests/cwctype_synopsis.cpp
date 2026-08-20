#ifdef FTL_REPLACE_STL
#include <cwctype>
namespace tested = std;
#else
#include <ftl/cwctype>
namespace tested = ftl;
#endif

#ifndef WEOF
#error "<cwctype> must define WEOF"
#endif

template <class T, class U> inline constexpr bool same_as_v = false;

template <class T> inline constexpr bool same_as_v<T, T> = true;

static_assert(same_as_v<decltype(WEOF), tested::wint_t>);

using classification_type = int (*)(tested::wint_t);

using iswctype_type = int (*)(tested::wint_t, tested::wctype_t);

using wctype_type = tested::wctype_t (*)(const char *);

using transformation_type = tested::wint_t (*)(tested::wint_t);

using towctrans_type = tested::wint_t (*)(tested::wint_t, tested::wctrans_t);

using wctrans_type = tested::wctrans_t (*)(const char *);

#define FTL_CHECK_CLASSIFICATION(name)                                         \
  [[maybe_unused]] constexpr classification_type name##_pointer = &tested::name

FTL_CHECK_CLASSIFICATION(iswalnum);
FTL_CHECK_CLASSIFICATION(iswalpha);
FTL_CHECK_CLASSIFICATION(iswblank);
FTL_CHECK_CLASSIFICATION(iswcntrl);
FTL_CHECK_CLASSIFICATION(iswdigit);
FTL_CHECK_CLASSIFICATION(iswgraph);
FTL_CHECK_CLASSIFICATION(iswlower);
FTL_CHECK_CLASSIFICATION(iswprint);
FTL_CHECK_CLASSIFICATION(iswpunct);
FTL_CHECK_CLASSIFICATION(iswspace);
FTL_CHECK_CLASSIFICATION(iswupper);
FTL_CHECK_CLASSIFICATION(iswxdigit);

#undef FTL_CHECK_CLASSIFICATION

[[maybe_unused]]
constexpr iswctype_type iswctype_pointer = &tested::iswctype;

[[maybe_unused]]
constexpr wctype_type wctype_pointer = &tested::wctype;

[[maybe_unused]]
constexpr transformation_type towlower_pointer = &tested::towlower;

[[maybe_unused]]
constexpr transformation_type towupper_pointer = &tested::towupper;

[[maybe_unused]]
constexpr towctrans_type towctrans_pointer = &tested::towctrans;

[[maybe_unused]]
constexpr wctrans_type wctrans_pointer = &tested::wctrans;

static_assert(sizeof(tested::wint_t) != 0);
static_assert(sizeof(tested::wctype_t) != 0);
static_assert(sizeof(tested::wctrans_t) != 0);

bool ftl_test() {
  return static_cast<tested::wint_t>(WEOF) != static_cast<tested::wint_t>(L'A');
}
