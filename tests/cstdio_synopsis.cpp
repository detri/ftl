#ifdef FTL_REPLACE_STL
#include <cstdio>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/cstdio>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

#if !defined(NULL) || !defined(_IOFBF) || !defined(_IOLBF) ||                  \
    !defined(_IONBF) || !defined(BUFSIZ) || !defined(EOF) ||                   \
    !defined(FOPEN_MAX) || !defined(FILENAME_MAX) || !defined(L_tmpnam) ||     \
    !defined(SEEK_CUR) || !defined(SEEK_END) || !defined(SEEK_SET) ||          \
    !defined(TMP_MAX)
#error "<cstdio> is missing a required macro"
#endif

#if !defined(stdin) || !defined(stdout) || !defined(stderr)
#error "<cstdio> is missing a standard stream macro"
#endif
static_assert(tested::is_same_v<decltype(stdin), tested::FILE *>);
static_assert(tested::is_same_v<decltype(stdout), tested::FILE *>);
static_assert(tested::is_same_v<decltype(stderr), tested::FILE *>);

using file = tested::FILE;
using size = tested::size_t;
using position = tested::fpos_t;
using args = tested::va_list;

static_assert(
    tested::is_same_v<decltype(&tested::remove), int (*)(const char *)>);
static_assert(tested::is_same_v<decltype(&tested::rename),
                                int (*)(const char *, const char *)>);
static_assert(tested::is_same_v<decltype(&tested::tmpfile), file *(*)()>);
static_assert(tested::is_same_v<decltype(&tested::tmpnam), char *(*)(char *)>);
static_assert(tested::is_same_v<decltype(&tested::fclose), int (*)(file *)>);
static_assert(tested::is_same_v<decltype(&tested::fflush), int (*)(file *)>);
static_assert(tested::is_same_v<decltype(&tested::fopen),
                                file *(*)(const char *, const char *)>);
static_assert(tested::is_same_v<decltype(&tested::freopen),
                                file *(*)(const char *, const char *, file *)>);
static_assert(
    tested::is_same_v<decltype(&tested::setbuf), void (*)(file *, char *)>);
static_assert(tested::is_same_v<decltype(&tested::setvbuf),
                                int (*)(file *, char *, int, size)>);
static_assert(tested::is_same_v<decltype(&tested::fprintf),
                                int (*)(file *, const char *, ...)>);
static_assert(tested::is_same_v<decltype(&tested::fscanf),
                                int (*)(file *, const char *, ...)>);
static_assert(
    tested::is_same_v<decltype(&tested::printf), int (*)(const char *, ...)>);
static_assert(
    tested::is_same_v<decltype(&tested::scanf), int (*)(const char *, ...)>);
static_assert(tested::is_same_v<decltype(&tested::snprintf),
                                int (*)(char *, size, const char *, ...)>);
static_assert(tested::is_same_v<decltype(&tested::sprintf),
                                int (*)(char *, const char *, ...)>);
static_assert(tested::is_same_v<decltype(&tested::sscanf),
                                int (*)(const char *, const char *, ...)>);
static_assert(tested::is_same_v<decltype(&tested::vfprintf),
                                int (*)(file *, const char *, args)>);
static_assert(tested::is_same_v<decltype(&tested::vfscanf),
                                int (*)(file *, const char *, args)>);
static_assert(
    tested::is_same_v<decltype(&tested::vprintf), int (*)(const char *, args)>);
static_assert(
    tested::is_same_v<decltype(&tested::vscanf), int (*)(const char *, args)>);
static_assert(tested::is_same_v<decltype(&tested::vsnprintf),
                                int (*)(char *, size, const char *, args)>);
static_assert(tested::is_same_v<decltype(&tested::vsprintf),
                                int (*)(char *, const char *, args)>);
static_assert(tested::is_same_v<decltype(&tested::vsscanf),
                                int (*)(const char *, const char *, args)>);
static_assert(tested::is_same_v<decltype(&tested::fgetc), int (*)(file *)>);
static_assert(tested::is_same_v<decltype(&tested::fgets),
                                char *(*)(char *, int, file *)>);
static_assert(
    tested::is_same_v<decltype(&tested::fputc), int (*)(int, file *)>);
static_assert(
    tested::is_same_v<decltype(&tested::fputs), int (*)(const char *, file *)>);
static_assert(tested::is_same_v<decltype(&tested::getc), int (*)(file *)>);
static_assert(tested::is_same_v<decltype(&tested::getchar), int (*)()>);
static_assert(tested::is_same_v<decltype(&tested::putc), int (*)(int, file *)>);
static_assert(tested::is_same_v<decltype(&tested::putchar), int (*)(int)>);
static_assert(
    tested::is_same_v<decltype(&tested::puts), int (*)(const char *)>);
static_assert(
    tested::is_same_v<decltype(&tested::ungetc), int (*)(int, file *)>);
static_assert(tested::is_same_v<decltype(&tested::fread),
                                size (*)(void *, size, size, file *)>);
static_assert(tested::is_same_v<decltype(&tested::fwrite),
                                size (*)(const void *, size, size, file *)>);
static_assert(
    tested::is_same_v<decltype(&tested::fgetpos), int (*)(file *, position *)>);
static_assert(
    tested::is_same_v<decltype(&tested::fseek), int (*)(file *, long, int)>);
static_assert(tested::is_same_v<decltype(&tested::fsetpos),
                                int (*)(file *, const position *)>);
static_assert(tested::is_same_v<decltype(&tested::ftell), long (*)(file *)>);
static_assert(tested::is_same_v<decltype(&tested::rewind), void (*)(file *)>);
static_assert(tested::is_same_v<decltype(&tested::clearerr), void (*)(file *)>);
static_assert(tested::is_same_v<decltype(&tested::feof), int (*)(file *)>);
static_assert(tested::is_same_v<decltype(&tested::ferror), int (*)(file *)>);
static_assert(
    tested::is_same_v<decltype(&tested::perror), void (*)(const char *)>);

bool ftl_test() { return true; }
