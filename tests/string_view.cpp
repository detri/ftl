#ifdef FTL_REPLACE_STL
#include <string_view>
namespace tested = std;
#else
#include <ftl/string_view>
namespace tested = ftl;
#endif

constexpr tested::string_view text{"freestanding"};
static_assert(text.size() == 12);
static_assert(text.starts_with("free"));
static_assert(text.ends_with("standing"));
static_assert(text.contains("stand"));
static_assert(text.substr(4, 5) == "stand");

bool ftl_test() { return true; }
