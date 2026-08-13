#include <cstdint>
#include <ratio>

auto invalid = std::ratio<1, INTMAX_MIN>::den;
