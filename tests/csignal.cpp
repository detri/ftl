#ifdef FTL_REPLACE_STL
#include <csignal>
namespace tested = std;
#else
#include <ftl/csignal>
namespace tested = ftl;
#endif

static_assert(sizeof(tested::sig_atomic_t) >= 1);

static_assert(SIGABRT > 0);
static_assert(SIGFPE > 0);
static_assert(SIGILL > 0);
static_assert(SIGINT > 0);
static_assert(SIGSEGV > 0);
static_assert(SIGTERM > 0);

#if defined(_WIN32)
static_assert(SIGABRT == 22);
#elif defined(__APPLE__) || defined(__linux__)
static_assert(SIGABRT == 6);
#endif

void handler(int) {}

static_assert(requires {
  tested::signal(SIGINT, handler);
  tested::raise(SIGINT);
});

bool ftl_test() { return SIG_DFL != SIG_ERR && SIG_IGN != SIG_ERR; }
