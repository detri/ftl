#ifdef FTL_REPLACE_STL
#include <array>
#include <cstddef>
#include <limits>
#include <mdspan>
#include <span>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/array>
#include <ftl/cstddef>
#include <ftl/limits>
#include <ftl/mdspan>
#include <ftl/span>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

#if __cpp_lib_mdspan != 202207L
#error <mdspan> must advertise the C++23 mdspan facility
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

using left_scalar_mapping = tested::layout_left::mapping<scalar_extents>;

using left_static_mapping = tested::layout_left::mapping<static_extents>;

using left_partial_mapping =
    tested::layout_left::mapping<partially_dynamic_extents>;

using stride_scalar_mapping = tested::layout_stride::mapping<scalar_extents>;

using stride_static_mapping = tested::layout_stride::mapping<static_extents>;

using rank_one_extents = tested::extents<int, tested::dynamic_extent>;

using left_rank_one_mapping = tested::layout_left::mapping<rank_one_extents>;

using right_rank_one_mapping = tested::layout_right::mapping<rank_one_extents>;

static_assert(tested::is_trivial_v<tested::layout_left>);
static_assert(tested::is_trivial_v<tested::layout_stride>);

static_assert(tested::is_trivially_copyable_v<left_scalar_mapping>);
static_assert(tested::is_trivially_copyable_v<left_static_mapping>);
static_assert(tested::is_trivially_copyable_v<stride_scalar_mapping>);
static_assert(tested::is_trivially_copyable_v<stride_static_mapping>);

static_assert(
    tested::is_same_v<left_static_mapping::layout_type, tested::layout_left>);

static_assert(tested::is_same_v<stride_static_mapping::layout_type,
                                tested::layout_stride>);

static_assert(left_static_mapping::is_always_unique());
static_assert(left_static_mapping::is_always_exhaustive());
static_assert(left_static_mapping::is_always_strided());

static_assert(stride_static_mapping::is_always_unique());
static_assert(!stride_static_mapping::is_always_exhaustive());
static_assert(stride_static_mapping::is_always_strided());

static_assert(!noexcept(left_static_mapping{}.stride(0)));
static_assert(noexcept(static_mapping{}.stride(0)));
static_assert(noexcept(stride_static_mapping{}.stride(0)));

static_assert(
    tested::is_convertible_v<left_rank_one_mapping, right_rank_one_mapping>);

static_assert(
    tested::is_convertible_v<right_rank_one_mapping, left_rank_one_mapping>);

static_assert(!tested::is_constructible_v<left_static_mapping, static_mapping>);

static_assert(!tested::is_constructible_v<static_mapping, left_static_mapping>);

static_assert(
    tested::is_convertible_v<left_static_mapping, stride_static_mapping>);

static_assert(tested::is_convertible_v<static_mapping, stride_static_mapping>);

static_assert(
    tested::is_constructible_v<left_static_mapping, stride_static_mapping>);

static_assert(
    tested::is_constructible_v<static_mapping, stride_static_mapping>);

static_assert(
    !tested::is_convertible_v<stride_static_mapping, left_static_mapping>);

static_assert(!tested::is_convertible_v<stride_static_mapping, static_mapping>);

static_assert(!tested::is_nothrow_constructible_v<left_static_mapping,
                                                  stride_static_mapping>);

static_assert(
    tested::is_nothrow_constructible_v<static_mapping, stride_static_mapping>);

static_assert(
    tested::is_convertible_v<stride_scalar_mapping, left_scalar_mapping>);

static_assert(tested::is_convertible_v<stride_scalar_mapping, scalar_mapping>);

static_assert(tested::is_same_v<decltype(stride_static_mapping{}.strides()),
                                tested::array<int, 3>>);

struct offset_layout {
  template <class Extents> class mapping {
  public:
    using extents_type = Extents;
    using index_type = typename extents_type::index_type;
    using size_type = typename extents_type::size_type;
    using rank_type = typename extents_type::rank_type;
    using layout_type = offset_layout;

    constexpr mapping() noexcept = default;

    constexpr mapping(const extents_type &extents,
                      index_type offset = 0) noexcept
        : extents_(extents), offset_(offset) {}

    [[nodiscard]] constexpr const extents_type &extents() const noexcept {
      return extents_;
    }

    [[nodiscard]] constexpr index_type required_span_size() const noexcept {
      for (rank_type rank = 0; rank < extents_type::rank(); ++rank) {
        if (extents_.extent(rank) == 0)
          return 0;
      }

      index_type result = 1;

      for (rank_type rank = 0; rank < extents_type::rank(); ++rank)
        result *= extents_.extent(rank);

      return offset_ + result;
    }

    template <class... Indices>
      requires(sizeof...(Indices) == extents_type::rank() &&
               (tested::is_convertible_v<Indices, index_type> && ...) &&
               (tested::is_nothrow_constructible_v<index_type, Indices> && ...))
    [[nodiscard]] constexpr index_type
    operator()(Indices... indices) const noexcept {
      index_type result = offset_;
      rank_type rank = 0;

      ((result += static_cast<index_type>(indices) * stride(rank++)), ...);

      return result;
    }

    [[nodiscard]] static constexpr bool is_always_unique() noexcept {
      return true;
    }

    [[nodiscard]] static constexpr bool is_always_exhaustive() noexcept {
      return false;
    }

    [[nodiscard]] static constexpr bool is_always_strided() noexcept {
      return true;
    }

    [[nodiscard]] static constexpr bool is_unique() noexcept { return true; }

    [[nodiscard]] constexpr bool is_exhaustive() const noexcept {
      return offset_ == 0;
    }

    [[nodiscard]] static constexpr bool is_strided() noexcept { return true; }

    [[nodiscard]] constexpr index_type stride(rank_type rank) const noexcept {
      index_type result = 1;

      for (rank_type current = rank + 1; current < extents_type::rank();
           ++current) {
        result *= extents_.extent(current);
      }

      return result;
    }

    friend constexpr bool operator==(const mapping &left,
                                     const mapping &right) noexcept {
      return left.extents_ == right.extents_ && left.offset_ == right.offset_;
    }

  private:
    FTL_NO_UNIQUE_ADDRESS extents_type extents_{};
    index_type offset_ = 0;
  };
};

using custom_mapping = offset_layout::mapping<static_extents>;

static_assert(
    tested::is_constructible_v<stride_static_mapping, custom_mapping>);

static_assert(!tested::is_convertible_v<custom_mapping, stride_static_mapping>);

static_assert(custom_mapping::is_always_unique());
static_assert(!custom_mapping::is_always_exhaustive());
static_assert(custom_mapping::is_always_strided());

constexpr bool mdspan_remaining_layouts_work() {
  left_scalar_mapping left_scalar;

  if (left_scalar.required_span_size() != 1)
    return false;

  if (left_scalar() != 0)
    return false;

  left_static_mapping left;

  if (left.required_span_size() != 24)
    return false;

  if (left.stride(0) != 1 || left.stride(1) != 2 || left.stride(2) != 6) {
    return false;
  }

  if (left(0, 0, 0) != 0)
    return false;

  if (left(1, 2, 3) != 23)
    return false;

  left_partial_mapping partial{partially_dynamic_extents{3}};

  if (partial.required_span_size() != 24)
    return false;

  if (partial.stride(0) != 1 || partial.stride(1) != 2 ||
      partial.stride(2) != 6) {
    return false;
  }

  if (partial(1, 2, 3) != 23)
    return false;

  rank_one_extents rank_one{7};
  left_rank_one_mapping left_one{rank_one};
  right_rank_one_mapping right_one = left_one;
  left_rank_one_mapping restored_one = right_one;

  if (right_one.required_span_size() != 7 ||
      restored_one.required_span_size() != 7) {
    return false;
  }

  stride_scalar_mapping stride_scalar;

  if (stride_scalar.required_span_size() != 1)
    return false;

  if (!stride_scalar.is_exhaustive())
    return false;

  if (stride_scalar() != 0)
    return false;

  if (stride_scalar.strides().size() != 0)
    return false;

  stride_static_mapping row_major;

  if (row_major.required_span_size() != 24)
    return false;

  if (row_major.stride(0) != 12 || row_major.stride(1) != 4 ||
      row_major.stride(2) != 1) {
    return false;
  }

  if (row_major(1, 2, 3) != 23)
    return false;

  if (!row_major.is_exhaustive())
    return false;

  auto row_major_strides = row_major.strides();

  if (row_major_strides[0] != 12 || row_major_strides[1] != 4 ||
      row_major_strides[2] != 1) {
    return false;
  }

  tested::array<int, 3> left_strides{1, 2, 6};
  stride_static_mapping column_major{static_extents{}, left_strides};

  if (column_major.required_span_size() != 24)
    return false;

  if (column_major(1, 2, 3) != 23)
    return false;

  if (!column_major.is_exhaustive())
    return false;

  if (!(column_major == left))
    return false;

  if (!(left == column_major))
    return false;

  tested::array<int, 3> padded_strides{20, 5, 1};
  stride_static_mapping padded{static_extents{}, padded_strides};

  if (padded.required_span_size() != 34)
    return false;

  if (padded(1, 2, 3) != 33)
    return false;

  if (padded.is_exhaustive())
    return false;

  if (padded == row_major)
    return false;

  int permuted_values[] = {4, 8, 1};
  tested::span permuted_strides{permuted_values};

  stride_static_mapping permuted{static_extents{}, permuted_strides};

  if (permuted.required_span_size() != 24)
    return false;

  if (!permuted.is_exhaustive())
    return false;

  if (permuted(1, 2, 3) != 23)
    return false;

  stride_static_mapping from_left = left;
  stride_static_mapping from_right = static_mapping{};

  if (!(from_left == left))
    return false;

  if (!(from_right == static_mapping{}))
    return false;

  left_static_mapping left_restored{from_left};
  static_mapping right_restored{from_right};

  if (!(left_restored == left))
    return false;

  if (!(right_restored == static_mapping{}))
    return false;

  using empty_extents = tested::extents<int, 2, tested::dynamic_extent, 4>;

  using empty_stride_mapping = tested::layout_stride::mapping<empty_extents>;

  empty_stride_mapping empty;

  if (empty.required_span_size() != 0)
    return false;

  if (!empty.is_exhaustive())
    return false;

  if (empty.stride(0) != 0 || empty.stride(1) != 4 || empty.stride(2) != 1) {
    return false;
  }

  custom_mapping custom_zero{static_extents{}, 0};

  if (custom_zero.required_span_size() != 24)
    return false;

  if (custom_zero.stride(0) != 12 || custom_zero.stride(1) != 4 ||
      custom_zero.stride(2) != 1) {
    return false;
  }

  if (custom_zero(1, 2, 3) != 23)
    return false;

  if (!custom_zero.is_unique() || !custom_zero.is_exhaustive() ||
      !custom_zero.is_strided()) {
    return false;
  }

  /*
   * Conversion from a nonstandard qualifying mapping is explicit.
   * OFFSET(custom_zero) is zero, satisfying the converting
   * constructor's precondition.
   */
  stride_static_mapping from_custom{custom_zero};

  if (from_custom.extents() != custom_zero.extents())
    return false;

  if (from_custom.stride(0) != 12 || from_custom.stride(1) != 4 ||
      from_custom.stride(2) != 1) {
    return false;
  }

  if (!(from_custom == custom_zero))
    return false;

  if (!(custom_zero == from_custom))
    return false;

  custom_mapping custom_shifted{static_extents{}, 1};

  if (custom_shifted.required_span_size() != 25)
    return false;

  if (custom_shifted.is_exhaustive())
    return false;

  if (custom_shifted(0, 0, 0) != 1 || custom_shifted(1, 2, 3) != 24) {
    return false;
  }

  if (from_custom == custom_shifted)
    return false;

  if (custom_shifted == from_custom)
    return false;

  return true;
}

static_assert(mdspan_remaining_layouts_work());

using mdspan_fixed_extents = tested::extents<tested::size_t, 2, 3>;
using mdspan_dynamic_extents = tested::dextents<tested::size_t, 2>;

using fixed_mdspan = tested::mdspan<int, mdspan_fixed_extents>;

using const_fixed_mdspan = tested::mdspan<const int, mdspan_fixed_extents>;

using dynamic_mdspan = tested::mdspan<int, mdspan_dynamic_extents>;

using stride_mdspan =
    tested::mdspan<int, mdspan_fixed_extents, tested::layout_stride>;

using scalar_mdspan = tested::mdspan<int, tested::extents<tested::size_t>>;

using zero_mdspan =
    tested::mdspan<int, tested::extents<tested::size_t, 2, 0, 3>>;

struct shifted_accessor {
  using offset_policy = shifted_accessor;
  using element_type = int;
  using reference = int &;
  using data_handle_type = int *;

  tested::size_t shift = 0;

  constexpr reference access(data_handle_type pointer,
                             tested::size_t index) const noexcept {
    return pointer[index + shift];
  }

  constexpr data_handle_type offset(data_handle_type pointer,
                                    tested::size_t index) const noexcept {
    return pointer + index + shift;
  }
};

using shifted_mdspan = tested::mdspan<int, mdspan_fixed_extents,
                                      tested::layout_right, shifted_accessor>;

static_assert(
    tested::is_same_v<fixed_mdspan::extents_type, mdspan_fixed_extents>);
static_assert(
    tested::is_same_v<fixed_mdspan::layout_type, tested::layout_right>);
static_assert(tested::is_same_v<fixed_mdspan::accessor_type,
                                tested::default_accessor<int>>);
static_assert(
    tested::is_same_v<fixed_mdspan::mapping_type,
                      tested::layout_right::mapping<mdspan_fixed_extents>>);
static_assert(tested::is_same_v<fixed_mdspan::element_type, int>);
static_assert(tested::is_same_v<fixed_mdspan::value_type, int>);
static_assert(tested::is_same_v<fixed_mdspan::index_type, tested::size_t>);
static_assert(tested::is_same_v<fixed_mdspan::size_type, tested::size_t>);
static_assert(tested::is_same_v<fixed_mdspan::rank_type, tested::size_t>);
static_assert(tested::is_same_v<fixed_mdspan::data_handle_type, int *>);
static_assert(tested::is_same_v<fixed_mdspan::reference, int &>);

static_assert(fixed_mdspan::rank() == 2);
static_assert(fixed_mdspan::rank_dynamic() == 0);
static_assert(fixed_mdspan::static_extent(0) == 2);
static_assert(fixed_mdspan::static_extent(1) == 3);

static_assert(dynamic_mdspan::rank() == 2);
static_assert(dynamic_mdspan::rank_dynamic() == 2);

static_assert(tested::is_default_constructible_v<dynamic_mdspan>);
static_assert(!tested::is_default_constructible_v<fixed_mdspan>);
static_assert(!tested::is_default_constructible_v<scalar_mdspan>);

static_assert(tested::is_constructible_v<fixed_mdspan, int *>);
static_assert(tested::is_constructible_v<dynamic_mdspan, int *, tested::size_t,
                                         tested::size_t>);

static_assert(tested::is_constructible_v<dynamic_mdspan, int *,
                                         tested::array<tested::size_t, 2>>);

static_assert(tested::is_constructible_v<dynamic_mdspan, int *,
                                         tested::span<tested::size_t, 2>>);

static_assert(tested::is_convertible_v<fixed_mdspan, const_fixed_mdspan>);
static_assert(!tested::is_convertible_v<const_fixed_mdspan, fixed_mdspan>);

static_assert(tested::is_constructible_v<fixed_mdspan, dynamic_mdspan>);
static_assert(!tested::is_convertible_v<dynamic_mdspan, fixed_mdspan>);

static_assert(tested::is_convertible_v<fixed_mdspan, stride_mdspan>);

static_assert(tested::is_trivially_copyable_v<fixed_mdspan>);
static_assert(tested::is_nothrow_move_constructible_v<fixed_mdspan>);
static_assert(tested::is_nothrow_move_assignable_v<fixed_mdspan>);
static_assert(tested::is_nothrow_swappable_v<fixed_mdspan>);

static_assert(fixed_mdspan::is_always_unique());
static_assert(fixed_mdspan::is_always_exhaustive());
static_assert(fixed_mdspan::is_always_strided());

template <class Mdspan>
concept accepts_mdspan_array_index =
    requires(const Mdspan &value,
             const tested::array<tested::size_t, Mdspan::rank()> &indices) {
      value[indices];
    };

template <class Mdspan>
concept accepts_mdspan_span_index = requires(
    const Mdspan &value, tested::span<tested::size_t, Mdspan::rank()> indices) {
  value[indices];
};

static_assert(accepts_mdspan_array_index<fixed_mdspan>);
static_assert(accepts_mdspan_span_index<fixed_mdspan>);

constexpr bool mdspan_class_works() {
  int values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

  fixed_mdspan fixed{values};

  if (fixed.rank() != 2 || fixed.rank_dynamic() != 0)
    return false;

  if (fixed.extent(0) != 2 || fixed.extent(1) != 3)
    return false;

  if (fixed.size() != 6 || fixed.empty())
    return false;

  if (fixed.data_handle() != values)
    return false;

  if (fixed[0, 0] != 0 || fixed[0, 2] != 2 || fixed[1, 0] != 3 ||
      fixed[1, 2] != 5) {
    return false;
  }

  fixed[1, 1] = 40;

  if (values[4] != 40)
    return false;

  tested::array<tested::size_t, 2> array_index{1, 2};

  if (fixed[array_index] != 5)
    return false;

  tested::size_t span_index_values[] = {1, 0};
  tested::span span_index{span_index_values};

  if (fixed[span_index] != 3)
    return false;

  mdspan_dynamic_extents dynamic_extents{2, 3};
  dynamic_mdspan from_extents{values, dynamic_extents};

  if (from_extents.extent(0) != 2 || from_extents.extent(1) != 3 ||
      from_extents[1, 1] != 40) {
    return false;
  }

  dynamic_mdspan from_values{values, 2, 3};

  if (from_values.size() != 6 || from_values[1, 2] != 5)
    return false;

  tested::array<tested::size_t, 2> extent_array{2, 3};
  dynamic_mdspan from_array{values, extent_array};

  if (from_array.size() != 6 || from_array[0, 1] != 1)
    return false;

  tested::size_t extent_values[] = {2, 3};
  tested::span extent_span{extent_values};
  dynamic_mdspan from_span{values, extent_span};

  if (from_span.size() != 6 || from_span[1, 0] != 3)
    return false;

  fixed_mdspan::mapping_type right_mapping{mdspan_fixed_extents{}};
  fixed_mdspan from_mapping{values, right_mapping};

  if (from_mapping.mapping().required_span_size() != 6 ||
      from_mapping[1, 2] != 5) {
    return false;
  }

  shifted_accessor shifted_access{1};
  shifted_mdspan shifted{values, right_mapping, shifted_access};

  if (shifted.accessor().shift != 1)
    return false;

  if (shifted[0, 0] != 1 || shifted[1, 2] != 6)
    return false;

  const_fixed_mdspan const_view = fixed;

  if (const_view.data_handle() != values || const_view[1, 1] != 40) {
    return false;
  }

  fixed_mdspan restored{from_values};

  if (restored.extent(0) != 2 || restored.extent(1) != 3 ||
      restored[1, 2] != 5) {
    return false;
  }

  stride_mdspan strided = fixed;

  if (strided.stride(0) != 3 || strided.stride(1) != 1 || strided[1, 2] != 5) {
    return false;
  }

  if (!fixed.is_unique() || !fixed.is_exhaustive() || !fixed.is_strided()) {
    return false;
  }

  scalar_mdspan scalar{values};

  if (scalar.rank() != 0 || scalar.size() != 1 || scalar.empty() ||
      scalar.operator[]() != 0) {
    return false;
  }

  zero_mdspan zero{values};

  if (zero.size() != 0 || !zero.empty())
    return false;

  dynamic_mdspan empty;

  if (empty.size() != 0 || !empty.empty() || empty.data_handle() != nullptr) {
    return false;
  }

  int other_values[] = {20, 21, 22, 23, 24, 25};

  fixed_mdspan left{values};
  fixed_mdspan right{other_values};

  using tested::swap;
  swap(left, right);

  if (left.data_handle() != other_values || right.data_handle() != values ||
      left[1, 2] != 25 || right[1, 2] != 5) {
    return false;
  }

  return true;
}

static_assert(mdspan_class_works());

constexpr bool mdspan_deduction_guides_work() {
  int values[] = {0, 1, 2, 3, 4, 5};
  int *pointer = values;

  tested::mdspan from_c_array{values};

  static_assert(tested::is_same_v<
                decltype(from_c_array),
                tested::mdspan<int, tested::extents<tested::size_t, 6>>>);

  if (from_c_array.extent(0) != 6 || from_c_array[5] != 5) {
    return false;
  }

  tested::mdspan from_pointer{pointer};

  static_assert(
      tested::is_same_v<decltype(from_pointer),
                        tested::mdspan<int, tested::extents<tested::size_t>>>);

  if (from_pointer.size() != 1 || from_pointer.operator[]() != 0) {
    return false;
  }

  tested::mdspan from_integrals{pointer, 2, 3};

  static_assert(tested::is_same_v<
                decltype(from_integrals),
                tested::mdspan<int, tested::dextents<tested::size_t, 2>>>);

  if (from_integrals[1, 2] != 5)
    return false;

  tested::size_t span_values[] = {2, 3};
  tested::span dimensions_span{span_values};
  tested::mdspan from_span{pointer, dimensions_span};

  static_assert(tested::is_same_v<
                decltype(from_span),
                tested::mdspan<int, tested::dextents<tested::size_t, 2>>>);

  if (from_span[1, 1] != 4)
    return false;

  tested::array<tested::size_t, 2> dimensions_array{2, 3};
  tested::mdspan from_array{pointer, dimensions_array};

  static_assert(tested::is_same_v<
                decltype(from_array),
                tested::mdspan<int, tested::dextents<tested::size_t, 2>>>);

  if (from_array[0, 2] != 2)
    return false;

  tested::extents<int, 2, 3> fixed_extents_value;
  tested::mdspan from_extents{pointer, fixed_extents_value};

  static_assert(
      tested::is_same_v<decltype(from_extents),
                        tested::mdspan<int, tested::extents<int, 2, 3>>>);

  if (from_extents[1, 2] != 5)
    return false;

  tested::layout_left::mapping<tested::extents<int, 2, 3>> left_mapping;

  tested::mdspan from_mapping{pointer, left_mapping};

  static_assert(
      tested::is_same_v<decltype(from_mapping),
                        tested::mdspan<int, tested::extents<int, 2, 3>,
                                       tested::layout_left>>);

  if (from_mapping[1, 2] != 5)
    return false;

  shifted_accessor accessor{1};
  tested::layout_right::mapping<mdspan_fixed_extents> right_mapping;

  tested::mdspan from_accessor{pointer, right_mapping, accessor};

  static_assert(tested::is_same_v<decltype(from_accessor), shifted_mdspan>);

  if (from_accessor[0, 0] != 1 || from_accessor.accessor().shift != 1) {
    return false;
  }

  return true;
}

static_assert(mdspan_deduction_guides_work());

using static_zero_tail_extents =
    tested::extents<int, tested::numeric_limits<int>::max(), 2, 0>;

using static_zero_tail_left =
    tested::layout_left::mapping<static_zero_tail_extents>;

using static_zero_tail_right =
    tested::layout_right::mapping<static_zero_tail_extents>;

static_assert(static_zero_tail_left{}.required_span_size() == 0);
static_assert(static_zero_tail_right{}.required_span_size() == 0);

constexpr bool mdspan_zero_tail_works() {
  tested::dextents<int, 3> extents{tested::numeric_limits<int>::max(), 2, 0};

  tested::layout_left::mapping<decltype(extents)> left{extents};
  tested::layout_right::mapping<decltype(extents)> right{extents};

  return left.required_span_size() == 0 && right.required_span_size() == 0;
}

static_assert(mdspan_zero_tail_works());

bool ftl_test() {
  return mdspan_extents_works() && mdspan_extents_conversions_work() &&
         mdspan_layout_right_works() && mdspan_remaining_layouts_work() &&
         mdspan_class_works() && mdspan_deduction_guides_work() &&
         mdspan_zero_tail_works();
}
