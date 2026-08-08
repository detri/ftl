#ifndef FTL_ASSOCIATIVE_FLAT_TAGS_HPP
#define FTL_ASSOCIATIVE_FLAT_TAGS_HPP
FTL_BEGIN_NAMESPACE
struct sorted_unique_t {
  explicit sorted_unique_t() = default;
};
inline constexpr sorted_unique_t sorted_unique{};
struct sorted_equivalent_t {
  explicit sorted_equivalent_t() = default;
};
inline constexpr sorted_equivalent_t sorted_equivalent{};
FTL_END_NAMESPACE
#endif
