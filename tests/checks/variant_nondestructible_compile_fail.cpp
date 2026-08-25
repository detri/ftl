#include <variant>

struct nondestructible {
    ~nondestructible() = delete;
};

std::variant<nondestructible> value;
