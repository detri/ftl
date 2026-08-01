#ifdef FTL_REPLACE_STL
#include <cerrno>
#else
#include <ftl/cerrno>
#endif

static_assert(EDOM > 0 && ERANGE > 0 && EILSEQ > 0);
static_assert(EDOM != ERANGE && EDOM != EILSEQ && ERANGE != EILSEQ);

bool ftl_test() {
    errno = EDOM;
    return errno == EDOM;
}
