#include <any>

struct move_only {
    move_only() = default;
    move_only(const move_only&) = delete;
    move_only(move_only&&) = default;
};

std::any value = move_only{};
