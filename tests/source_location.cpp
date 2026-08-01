#ifdef FTL_REPLACE_STL
#include <source_location>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/source_location>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

constexpr auto location = tested::source_location::current();
static_assert(location.line() > 0);
#if (defined(FTL_REPLACE_STL) && __has_builtin(__builtin_source_location)) || \
    defined(__clang__) || defined(_MSC_VER)
static_assert(location.column() > 0);
#endif
static_assert(tested::is_same_v<decltype(location.column()), tested::uint_least32_t>);
static_assert(location.file_name()[0] != '\0');
static_assert(tested::is_trivially_copyable_v<tested::source_location>);
static_assert(noexcept(tested::source_location::current()));

bool ftl_test() { return true; }
