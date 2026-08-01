#ifdef FTL_REPLACE_STL
#include <csignal>
namespace tested = std;
#else
#include <ftl/csignal>
namespace tested = ftl;
#endif

static_assert(sizeof(tested::sig_atomic_t) >= 1);
static_assert(SIGABRT > 0 && SIGFPE > 0 && SIGILL > 0 && SIGINT > 0);
static_assert(SIGSEGV > 0 && SIGTERM > 0);

void handler(int) {}
static_assert(requires { tested::signal(SIGINT, handler); tested::raise(SIGINT); });

bool ftl_test() { return SIG_DFL != SIG_ERR && SIG_IGN != SIG_ERR; }
