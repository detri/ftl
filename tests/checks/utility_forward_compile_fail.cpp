#include <utility>

int&& invalid = std::forward<int&>(0);
