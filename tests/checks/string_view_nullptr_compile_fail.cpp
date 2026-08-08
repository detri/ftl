#define FTL_REPLACE_STL
#include <string_view>

int main() {
  std::string_view value(nullptr);
  static_cast<void>(value);
}
