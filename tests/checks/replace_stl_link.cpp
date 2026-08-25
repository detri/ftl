#include <string>

extern "C" void ftl_runtime_link_anchor();

int main() {
  ftl_runtime_link_anchor();
  const std::string value(64, 'f');
  return value.size() == 64 ? 0 : 1;
}
