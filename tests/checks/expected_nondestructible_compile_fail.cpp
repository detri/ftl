#include <expected>

struct nondestructible {
    ~nondestructible() = delete;
};

std::expected<nondestructible, int> value;
