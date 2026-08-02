#include <span>

struct base {};
struct derived : base {};

int main() {
    derived values[4]{};
    std::span<base> invalid(values);
}