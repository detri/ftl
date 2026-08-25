#include <expected>

struct explicit_boolean {
    explicit operator bool() const { return true; }
};

struct value {
    friend explicit_boolean operator==(value, value) { return {}; }
};

int main() {
    return std::expected<value, int>{} == value{};
}
