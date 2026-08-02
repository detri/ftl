#include <span>

int main() {
    int values[3]{};
    std::span<int, 4> invalid(values);
}
