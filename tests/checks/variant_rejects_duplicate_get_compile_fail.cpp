#include <variant>

int main() {
    std::variant<int, int> value(std::in_place_index<0>, 1);
    return std::get<int>(value);
}
