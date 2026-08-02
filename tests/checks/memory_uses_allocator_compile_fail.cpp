#include <memory>

struct invalid {
    using allocator_type = std::allocator<int>;
    explicit invalid(int);
};

auto value = std::make_obj_using_allocator<invalid>(std::allocator<int>{}, 1);
