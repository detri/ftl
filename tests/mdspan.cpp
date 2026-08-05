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

using static_2x3 = tested::extents<int, 2, 3>;
using static_2x4 = tested::extents<int, 2, 4>;
using dynamic_2d =
    tested::extents<int, tested::dynamic_extent, tested::dynamic_extent>;

using wide_dynamic = tested::extents<long long, tested::dynamic_extent>;
using narrow_dynamic = tested::extents<int, tested::dynamic_extent>;

static_assert(tested::is_convertible_v<static_2x3, dynamic_2d>);
static_assert(tested::is_constructible_v<static_2x3, dynamic_2d>);
static_assert(!tested::is_convertible_v<dynamic_2d, static_2x3>);

static_assert(!tested::is_constructible_v<static_2x4, static_2x3>);

static_assert(tested::is_convertible_v<narrow_dynamic, wide_dynamic>);
static_assert(tested::is_constructible_v<narrow_dynamic, wide_dynamic>);
static_assert(!tested::is_convertible_v<wide_dynamic, narrow_dynamic>);

static_assert(tested::is_constructible_v<mixed_extents, tested::array<int, 2>>);
static_assert(tested::is_convertible_v<tested::array<int, 2>, mixed_extents>);

static_assert(tested::is_constructible_v<mixed_extents, tested::array<int, 4>>);
static_assert(!tested::is_convertible_v<tested::array<int, 4>, mixed_extents>);

static_assert(tested::is_constructible_v<mixed_extents, tested::span<int, 2>>);
static_assert(tested::is_convertible_v<tested::span<int, 2>, mixed_extents>);

static_assert(tested::is_constructible_v<mixed_extents, tested::span<int, 4>>);
static_assert(!tested::is_convertible_v<tested::span<int, 4>, mixed_extents>);

constexpr bool mdspan_extents_conversions_work() {
  static_2x3 fixed;
  dynamic_2d dynamic = fixed;

  if (dynamic.extent(0) != 2 || dynamic.extent(1) != 3)
    return false;

  dynamic_2d matching{2, 3};
  static_2x3 restored{matching};

  if (restored.extent(0) != 2 || restored.extent(1) != 3)
    return false;

  tested::array<int, 2> compact_values{5, 7};
  mixed_extents from_compact_array = compact_values;

  if (from_compact_array.extent(0) != 2 || from_compact_array.extent(1) != 5 ||
      from_compact_array.extent(2) != 4 || from_compact_array.extent(3) != 7)
    return false;

  tested::array<int, 4> complete_values{2, 11, 4, 13};
  mixed_extents from_complete_array{complete_values};

  if (from_complete_array.extent(0) != 2 ||
      from_complete_array.extent(1) != 11 ||
      from_complete_array.extent(2) != 4 || from_complete_array.extent(3) != 13)
    return false;

  int compact_data[] = {17, 19};
  tested::span compact_span{compact_data};
  mixed_extents from_compact_span = compact_span;

  if (from_compact_span.extent(1) != 17 || from_compact_span.extent(3) != 19)
    return false;

  int complete_data[] = {2, 23, 4, 29};
  tested::span complete_span{complete_data};
  mixed_extents from_complete_span{complete_span};

  if (from_complete_span.extent(1) != 23 || from_complete_span.extent(3) != 29)
    return false;

  if (!(fixed == dynamic))
    return false;

  if (fixed == static_2x4{})
    return false;

  tested::extents<int, 2> rank_one;
  if (fixed == rank_one)
    return false;

  return true;
}

static_assert(mdspan_extents_conversions_work());

using int_accessor = tested::default_accessor<int>;
using const_int_accessor = tested::default_accessor<const int>;

static_assert(tested::is_trivially_copyable_v<int_accessor>);
static_assert(tested::is_same_v<int_accessor::offset_policy, int_accessor>);
static_assert(tested::is_same_v<int_accessor::element_type, int>);
static_assert(tested::is_same_v<int_accessor::reference, int &>);
static_assert(tested::is_same_v<int_accessor::data_handle_type, int *>);

static_assert(tested::is_constructible_v<const_int_accessor, int_accessor>);
static_assert(tested::is_convertible_v<int_accessor, const_int_accessor>);
static_assert(!tested::is_constructible_v<int_accessor, const_int_accessor>);

using scalar_mapping = tested::layout_right::mapping<scalar_extents>;
using static_mapping = tested::layout_right::mapping<static_extents>;

using dynamic_mapping = tested::layout_right::mapping<
    tested::dextents<long long, static_extents::rank()>>;

using partially_dynamic_extents =
    tested::extents<int, 2, tested::dynamic_extent, 4>;

using partially_dynamic_mapping =
    tested::layout_right::mapping<partially_dynamic_extents>;

static_assert(tested::is_trivial_v<tested::layout_right>);
static_assert(tested::is_trivially_copyable_v<scalar_mapping>);
static_assert(tested::is_trivially_copyable_v<static_mapping>);
static_assert(tested::is_same_v<static_mapping::extents_type, static_extents>);
static_assert(tested::is_same_v<static_mapping::index_type, int>);
static_assert(tested::is_same_v<static_mapping::size_type, unsigned int>);
static_assert(tested::is_same_v<static_mapping::rank_type, tested::size_t>);
static_assert(
    tested::is_same_v<static_mapping::layout_type, tested::layout_right>);

static_assert(static_mapping::is_always_unique());
static_assert(static_mapping::is_always_exhaustive());
static_assert(static_mapping::is_always_strided());

static_assert(tested::is_convertible_v<static_mapping, dynamic_mapping>);
static_assert(tested::is_constructible_v<static_mapping, dynamic_mapping>);
static_assert(!tested::is_convertible_v<dynamic_mapping, static_mapping>);

template <class Mapping>
concept has_stride = requires(const Mapping &mapping) { mapping.stride(0); };

template <class Mapping>
concept accepts_two_indices =
    requires(const Mapping &mapping) { mapping(0, 0); };

template <class Left, class Right>
concept mapping_equality_comparable =
    requires(const Left &left, const Right &right) { left == right; };

static_assert(!has_stride<scalar_mapping>);
static_assert(has_stride<static_mapping>);
static_assert(!accepts_two_indices<static_mapping>);

static_assert(mapping_equality_comparable<static_mapping, dynamic_mapping>);
static_assert(!mapping_equality_comparable<
              static_mapping,
              tested::layout_right::mapping<tested::extents<int, 2>>>);

constexpr bool mdspan_layout_right_works() {
  int values[] = {1, 2, 3, 4};

  int_accessor accessor;

  if (accessor.access(values, 2) != 3)
    return false;

  accessor.access(values, 1) = 20;

  if (values[1] != 20)
    return false;

  if (accessor.offset(values, 3) != values + 3)
    return false;

  const_int_accessor const_accessor = accessor;

  if (const_accessor.access(values, 1) != 20)
    return false;

  scalar_mapping scalar;

  if (scalar.required_span_size() != 1)
    return false;

  if (scalar() != 0)
    return false;

  static_mapping fixed;

  if (fixed.required_span_size() != 24)
    return false;

  if (fixed.stride(0) != 12 || fixed.stride(1) != 4 || fixed.stride(2) != 1) {
    return false;
  }

  if (fixed(0, 0, 0) != 0)
    return false;

  if (fixed(1, 2, 3) != 23)
    return false;

  partially_dynamic_extents dynamic_extents{3};
  partially_dynamic_mapping partial{dynamic_extents};

  if (partial.required_span_size() != 24)
    return false;

  if (partial.stride(0) != 12 || partial.stride(1) != 4 ||
      partial.stride(2) != 1) {
    return false;
  }

  if (partial(1, 2, 3) != 23)
    return false;

  partially_dynamic_mapping empty{partially_dynamic_extents{0}};

  if (empty.required_span_size() != 0)
    return false;

  if (empty.stride(0) != 0 || empty.stride(1) != 4 || empty.stride(2) != 1) {
    return false;
  }

  dynamic_mapping converted = fixed;

  if (converted.required_span_size() != 24)
    return false;

  if (!(converted == fixed))
    return false;

  static_mapping restored{converted};

  if (!(restored == fixed))
    return false;

  return true;
}

static_assert(mdspan_layout_right_works());

bool ftl_test() {
  return mdspan_extents_works() && mdspan_extents_conversions_work() &&
         mdspan_layout_right_works();
}