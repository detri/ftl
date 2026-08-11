/* FTL adaptation support for OpenLibm v0.8.7. */
#include <openlibm_math.h>

const union ftl_olm_infinity_storage ftl_olm_infinity = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x7f}};

const union ftl_olm_nan_storage ftl_olm_nan_value = {
    {0x00, 0x00, 0xc0, 0x7f}};

#ifdef _MSC_VER
/* Required by MSVC objects that perform floating-point operations. */
extern "C" {
__declspec(selectany) int _fltused = 0x9875;
}
#endif
