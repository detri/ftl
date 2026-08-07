#ifdef FTL_REPLACE_STL
#include <cwctype>
#define FTL_WCTYPE_NAMESPACE std
#else
#include <ftl/cwctype>
#define FTL_WCTYPE_NAMESPACE ftl
#endif

using FTL_WCTYPE_NAMESPACE::wctrans_t;
using FTL_WCTYPE_NAMESPACE::wctype_t;
using FTL_WCTYPE_NAMESPACE::wint_t;

using FTL_WCTYPE_NAMESPACE::iswalnum;
using FTL_WCTYPE_NAMESPACE::iswalpha;
using FTL_WCTYPE_NAMESPACE::iswblank;
using FTL_WCTYPE_NAMESPACE::iswcntrl;
using FTL_WCTYPE_NAMESPACE::iswctype;
using FTL_WCTYPE_NAMESPACE::iswdigit;
using FTL_WCTYPE_NAMESPACE::iswgraph;
using FTL_WCTYPE_NAMESPACE::iswlower;
using FTL_WCTYPE_NAMESPACE::iswprint;
using FTL_WCTYPE_NAMESPACE::iswpunct;
using FTL_WCTYPE_NAMESPACE::iswspace;
using FTL_WCTYPE_NAMESPACE::iswupper;
using FTL_WCTYPE_NAMESPACE::iswxdigit;
using FTL_WCTYPE_NAMESPACE::wctype;

using FTL_WCTYPE_NAMESPACE::towctrans;
using FTL_WCTYPE_NAMESPACE::towlower;
using FTL_WCTYPE_NAMESPACE::towupper;
using FTL_WCTYPE_NAMESPACE::wctrans;

#undef FTL_WCTYPE_NAMESPACE
