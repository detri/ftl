#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <mdspan>
#include <span>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/mdspan>
#include <ftl/span>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

using scalar_extents = tested::extents<int>;

using static_extents = tested::extents<int, 2, 3, 4>;

using mixed_extents =
    tested::extents<int, 2, tested::dynamic_extent, 4, tested::dynamic_extent>;

using dynamic_extents = tested::dextents<int, 3>;

static_assert(scalar_extents::rank() == 0);

static_assert(scalar_extents::rank_dynamic() == 0);

static_assert(static_extents::rank() == 3);

static_assert(static_extents::rank_dynamic() == 0);

static_assert(static_extents::static_extent(0) == 2);

static_assert(static_extents::static_extent(1) == 3);

static_assert(static_extents::static_extent(2) == 4);

static_assert(mixed_extents::rank() == 4);

static_assert(mixed_extents::rank_dynamic() == 2);

static_assert(mixed_extents::static_extent(0) == 2);

static_assert(mixed_extents::static_extent(1) == tested::dynamic_extent);

static_assert(mixed_extents::static_extent(2) == 4);

static_assert(mixed_extents::static_extent(3) == tested::dynamic_extent);

static_assert(dynamic_extents::rank() == 3);

static_assert(dynamic_extents::rank_dynamic() == 3);

static_assert(tested::is_same_v<typename mixed_extents::index_type, int>);

static_assert(
    tested::is_same_v<typename mixed_extents::size_type, unsigned int>);

static_assert(
    tested::is_same_v<typename mixed_extents::rank_type, tested::size_t>);

static_assert(tested::is_trivially_copyable_v<scalar_extents>);

static_assert(tested::is_trivially_copyable_v<static_extents>);

static_assert(tested::is_trivially_copyable_v<mixed_extents>);

static_assert(tested::is_constructible_v<mixed_extents, int, int>);

static_assert(tested::is_constructible_v<mixed_extents, int, int, int, int>);

static_assert(!tested::is_constructible_v<mixed_extents, int>);

static_assert(!tested::is_constructible_v<mixed_extents, int, int, int>);

static_assert(tested::is_nothrow_constructible_v<mixed_extents, int, int>);

constexpr bool mdspan_extents_works() {
  scalar_extents scalar;

  if (scalar.rank() != 0 || scalar.rank_dynamic() != 0) {
    return false;
  }

  static_extents fixed;

  if (fixed.extent(0) != 2 || fixed.extent(1) != 3 || fixed.extent(2) != 4) {
    return false;
  }

  /*
   * Passing rank_dynamic() values supplies only the dynamic
   * dimensions.
   */
  mixed_extents compact{3, 5};

  if (compact.extent(0) != 2 || compact.extent(1) != 3 ||
      compact.extent(2) != 4 || compact.extent(3) != 5) {
    return false;
  }

  /*
   * Passing rank() values supplies every dimension. Static
   * positions are validated by the constructor's preconditions
   * and only dynamic positions are stored.
   */
  mixed_extents complete{2, 7, 4, 11};

  if (complete.extent(0) != 2 || complete.extent(1) != 7 ||
      complete.extent(2) != 4 || complete.extent(3) != 11) {
    return false;
  }

  dynamic_extents dynamic;

  if (dynamic.extent(0) != 0 || dynamic.extent(1) != 0 ||
      dynamic.extent(2) != 0) {
    return false;
  }

  dynamic_extents initialized{6, 7, 8};

  if (initialized.extent(0) != 6 || initialized.extent(1) != 7 ||
      initialized.extent(2) != 8) {
    return false;
  }

  tested::extents deduced{9, 10, 11};

  static_assert(tested::is_same_v<decltype(deduced),
                                  tested::dextents<tested::size_t, 3>>);

  if (deduced.extent(0) != 9 || deduced.extent(1) != 10 ||
      deduced.extent(2) != 11) {
    return false;
  }

  return true;
}

static_assert(mdspan_extents_works());

bool ftl_test() { return mdspan_extents_works(); }