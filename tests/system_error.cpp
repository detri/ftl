#ifdef FTL_REPLACE_STL
#include <system_error>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/system_error>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(tested::is_error_condition_enum_v<tested::errc>);
static_assert(!tested::is_error_code_enum_v<tested::errc>);
static_assert(tested::is_base_of_v<tested::runtime_error, tested::system_error>);
static_assert(noexcept(tested::error_code{}));
static_assert(noexcept(tested::make_error_condition(tested::errc::permission_denied)));

bool contains(const char *text, const char *part) {
  for (; *text; ++text) {
    const char *left = text;
    const char *right = part;
    while (*left && *right && *left == *right) {
      ++left;
      ++right;
    }
    if (!*right)
      return true;
  }
  return false;
}

bool ftl_test() {
  tested::error_code code = tested::make_error_code(tested::errc::permission_denied);
  tested::error_condition condition = tested::errc::permission_denied;
  if (code.value() != static_cast<int>(tested::errc::permission_denied) ||
      code.category().name()[0] != 'g' || code != condition ||
      code.message().empty() || !code || !condition)
    return false;

  tested::error_code cleared{7, tested::generic_category()};
  cleared.clear();
  if (cleared || cleared.category() != tested::system_category())
    return false;

  tested::system_error error{code, "opening file"};
  return error.code() == code && contains(error.what(), "opening file") &&
         tested::hash<tested::error_code>{}(code) ==
             tested::hash<tested::error_code>{}(code);
}
