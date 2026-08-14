#include <numbers>

int main() {
  [[maybe_unused]]
  auto invalid = std::numbers::pi_v<int>;
}
