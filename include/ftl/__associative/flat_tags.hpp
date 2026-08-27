#ifndef FTL_ASSOCIATIVE_FLAT_TAGS_HPP
#define FTL_ASSOCIATIVE_FLAT_TAGS_HPP
namespace std {
struct sorted_unique_t {
  explicit sorted_unique_t() = default;
};
inline constexpr sorted_unique_t sorted_unique{};
struct sorted_equivalent_t {
  explicit sorted_equivalent_t() = default;
};
inline constexpr sorted_equivalent_t sorted_equivalent{};
} // namespace std
#endif
