#include <span>

int main() {
    const int values[4]{};
    std::span<int> invalid(values);
}
