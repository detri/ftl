#include <variant>

struct visitor {
    int operator()(int) const;
    long operator()(long) const;
};

int main() {
    std::variant<int, long> value(1);
    return std::visit(visitor{}, value);
}
