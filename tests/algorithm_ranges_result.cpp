#ifdef FTL_REPLACE_STL
#include <algorithm>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/algorithm>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct move_only {
  int value = 0;

  constexpr explicit move_only(int source) noexcept : value(source) {}

  move_only(const move_only &) = delete;
  move_only &operator=(const move_only &) = delete;

  constexpr move_only(move_only &&other) noexcept : value(other.value) {
    other.value = -1;
  }

  constexpr move_only &operator=(move_only &&other) noexcept {
    value = other.value;
    other.value = -1;
    return *this;
  }
};

struct moved_value {
  int value = 0;

  constexpr moved_value(move_only &&source) noexcept : value(source.value) {
    source.value = -1;
  }
};

static_assert(tested::is_aggregate_v<tested::ranges::in_fun_result<int, int>>);

static_assert(tested::is_aggregate_v<tested::ranges::in_in_result<int, int>>);

static_assert(tested::is_aggregate_v<tested::ranges::in_out_result<int, int>>);

static_assert(
    tested::is_aggregate_v<tested::ranges::in_in_out_result<int, int, int>>);

static_assert(
    tested::is_aggregate_v<tested::ranges::in_out_out_result<int, int, int>>);

static_assert(tested::is_aggregate_v<tested::ranges::min_max_result<int>>);
static_assert(tested::is_same_v<tested::ranges::minmax_result<int>,
                                tested::ranges::min_max_result<int>>);

static_assert(tested::is_aggregate_v<tested::ranges::in_found_result<int>>);

static_assert(
    tested::is_aggregate_v<tested::ranges::in_value_result<int, int>>);

static_assert(
    tested::is_aggregate_v<tested::ranges::out_value_result<int, int>>);

constexpr bool aggregate_members_work() {
  tested::ranges::in_fun_result first{1, 2};
  tested::ranges::in_in_result second{3, 4};
  tested::ranges::in_out_result third{5, 6};

  tested::ranges::in_in_out_result fourth{7, 8, 9};

  tested::ranges::in_out_out_result fifth{10, 11, 12};

  tested::ranges::min_max_result sixth{13, 14};

  tested::ranges::in_found_result seventh{15, true};

  tested::ranges::in_value_result eighth{16, 17};

  tested::ranges::out_value_result ninth{18, 19};

  return first.in == 1 && first.fun == 2 && second.in1 == 3 &&
         second.in2 == 4 && third.in == 5 && third.out == 6 &&
         fourth.in1 == 7 && fourth.in2 == 8 && fourth.out == 9 &&
         fifth.in == 10 && fifth.out1 == 11 && fifth.out2 == 12 &&
         sixth.min == 13 && sixth.max == 14 && seventh.in == 15 &&
         seventh.found && eighth.in == 16 && eighth.value == 17 &&
         ninth.out == 18 && ninth.value == 19;
}

static_assert(aggregate_members_work());

constexpr bool structured_bindings_work() {
  auto [first_in, first_fun] = tested::ranges::in_fun_result{1, 2};

  auto [second_in1, second_in2] = tested::ranges::in_in_result{3, 4};

  auto [third_in, third_out] = tested::ranges::in_out_result{5, 6};

  auto [fourth_in1, fourth_in2, fourth_out] =
      tested::ranges::in_in_out_result{7, 8, 9};

  auto [fifth_in, fifth_out1, fifth_out2] =
      tested::ranges::in_out_out_result{10, 11, 12};

  auto [sixth_min, sixth_max] = tested::ranges::min_max_result{13, 14};

  auto [seventh_in, seventh_found] = tested::ranges::in_found_result{15, true};

  auto [eighth_in, eighth_value] = tested::ranges::in_value_result{16, 17};

  auto [ninth_out, ninth_value] = tested::ranges::out_value_result{18, 19};

  return first_in == 1 && first_fun == 2 && second_in1 == 3 &&
         second_in2 == 4 && third_in == 5 && third_out == 6 &&
         fourth_in1 == 7 && fourth_in2 == 8 && fourth_out == 9 &&
         fifth_in == 10 && fifth_out1 == 11 && fifth_out2 == 12 &&
         sixth_min == 13 && sixth_max == 14 && seventh_in == 15 &&
         seventh_found && eighth_in == 16 && eighth_value == 17 &&
         ninth_out == 18 && ninth_value == 19;
}

static_assert(structured_bindings_work());

constexpr bool const_lvalue_conversions_work() {
  const tested::ranges::in_fun_result<int, short> first{1, 2};

  const tested::ranges::in_in_result<int, short> second{3, 4};

  const tested::ranges::in_out_result<int, short> third{5, 6};

  const tested::ranges::in_in_out_result<int, short, signed char> fourth{7, 8,
                                                                         9};

  const tested::ranges::in_out_out_result<int, short, signed char> fifth{10, 11,
                                                                         12};

  const tested::ranges::min_max_result<short> sixth{13, 14};

  const tested::ranges::in_found_result<int> seventh{15, true};

  const tested::ranges::in_value_result<int, short> eighth{16, 17};

  const tested::ranges::out_value_result<int, short> ninth{18, 19};

  tested::ranges::in_fun_result<long, long> first_result = first;

  tested::ranges::in_in_result<long, long> second_result = second;

  tested::ranges::in_out_result<long, long> third_result = third;

  tested::ranges::in_in_out_result<long, long, long> fourth_result = fourth;

  tested::ranges::in_out_out_result<long, long, long> fifth_result = fifth;

  tested::ranges::min_max_result<long> sixth_result = sixth;

  tested::ranges::in_found_result<long> seventh_result = seventh;

  tested::ranges::in_value_result<long, long> eighth_result = eighth;

  tested::ranges::out_value_result<long, long> ninth_result = ninth;

  return first_result.in == 1 && first_result.fun == 2 &&
         second_result.in1 == 3 && second_result.in2 == 4 &&
         third_result.in == 5 && third_result.out == 6 &&
         fourth_result.in1 == 7 && fourth_result.in2 == 8 &&
         fourth_result.out == 9 && fifth_result.in == 10 &&
         fifth_result.out1 == 11 && fifth_result.out2 == 12 &&
         sixth_result.min == 13 && sixth_result.max == 14 &&
         seventh_result.in == 15 && seventh_result.found &&
         eighth_result.in == 16 && eighth_result.value == 17 &&
         ninth_result.out == 18 && ninth_result.value == 19;
}

static_assert(const_lvalue_conversions_work());

using in_fun_move_source = tested::ranges::in_fun_result<move_only, move_only>;

using in_fun_move_target =
    tested::ranges::in_fun_result<moved_value, moved_value>;

using in_in_move_source = tested::ranges::in_in_result<move_only, move_only>;

using in_in_move_target =
    tested::ranges::in_in_result<moved_value, moved_value>;

using in_out_move_source = tested::ranges::in_out_result<move_only, move_only>;

using in_out_move_target =
    tested::ranges::in_out_result<moved_value, moved_value>;

using in_in_out_move_source =
    tested::ranges::in_in_out_result<move_only, move_only, move_only>;

using in_in_out_move_target =
    tested::ranges::in_in_out_result<moved_value, moved_value, moved_value>;

using in_out_out_move_source =
    tested::ranges::in_out_out_result<move_only, move_only, move_only>;

using in_out_out_move_target =
    tested::ranges::in_out_out_result<moved_value, moved_value, moved_value>;

using min_max_move_source = tested::ranges::min_max_result<move_only>;

using min_max_move_target = tested::ranges::min_max_result<moved_value>;

using in_found_move_source = tested::ranges::in_found_result<move_only>;

using in_found_move_target = tested::ranges::in_found_result<moved_value>;

using in_value_move_source =
    tested::ranges::in_value_result<move_only, move_only>;

using in_value_move_target =
    tested::ranges::in_value_result<moved_value, moved_value>;

using out_value_move_source =
    tested::ranges::out_value_result<move_only, move_only>;

using out_value_move_target =
    tested::ranges::out_value_result<moved_value, moved_value>;

static_assert(
    !tested::is_convertible_v<const in_fun_move_source &, in_fun_move_target>);

static_assert(
    tested::is_convertible_v<in_fun_move_source &&, in_fun_move_target>);

static_assert(
    !tested::is_convertible_v<const in_in_move_source &, in_in_move_target>);

static_assert(
    tested::is_convertible_v<in_in_move_source &&, in_in_move_target>);

static_assert(
    !tested::is_convertible_v<const in_out_move_source &, in_out_move_target>);

static_assert(
    tested::is_convertible_v<in_out_move_source &&, in_out_move_target>);

static_assert(!tested::is_convertible_v<const in_in_out_move_source &,
                                        in_in_out_move_target>);

static_assert(
    tested::is_convertible_v<in_in_out_move_source &&, in_in_out_move_target>);

static_assert(!tested::is_convertible_v<const in_out_out_move_source &,
                                        in_out_out_move_target>);

static_assert(tested::is_convertible_v<in_out_out_move_source &&,
                                       in_out_out_move_target>);

static_assert(!tested::is_convertible_v<const min_max_move_source &,
                                        min_max_move_target>);

static_assert(
    tested::is_convertible_v<min_max_move_source &&, min_max_move_target>);

static_assert(!tested::is_convertible_v<const in_found_move_source &,
                                        in_found_move_target>);

static_assert(
    tested::is_convertible_v<in_found_move_source &&, in_found_move_target>);

static_assert(!tested::is_convertible_v<const in_value_move_source &,
                                        in_value_move_target>);

static_assert(
    tested::is_convertible_v<in_value_move_source &&, in_value_move_target>);

static_assert(!tested::is_convertible_v<const out_value_move_source &,
                                        out_value_move_target>);

static_assert(
    tested::is_convertible_v<out_value_move_source &&, out_value_move_target>);

constexpr bool rvalue_conversions_work() {
  in_fun_move_source first{move_only{1}, move_only{2}};

  in_in_move_source second{move_only{3}, move_only{4}};

  in_out_move_source third{move_only{5}, move_only{6}};

  in_in_out_move_source fourth{move_only{7}, move_only{8}, move_only{9}};

  in_out_out_move_source fifth{move_only{10}, move_only{11}, move_only{12}};

  min_max_move_source sixth{move_only{13}, move_only{14}};

  in_found_move_source seventh{move_only{15}, true};

  in_value_move_source eighth{move_only{16}, move_only{17}};

  out_value_move_source ninth{move_only{18}, move_only{19}};

  in_fun_move_target first_result = tested::move(first);

  in_in_move_target second_result = tested::move(second);

  in_out_move_target third_result = tested::move(third);

  in_in_out_move_target fourth_result = tested::move(fourth);

  in_out_out_move_target fifth_result = tested::move(fifth);

  min_max_move_target sixth_result = tested::move(sixth);

  in_found_move_target seventh_result = tested::move(seventh);

  in_value_move_target eighth_result = tested::move(eighth);

  out_value_move_target ninth_result = tested::move(ninth);

  return first_result.in.value == 1 && first_result.fun.value == 2 &&
         second_result.in1.value == 3 && second_result.in2.value == 4 &&
         third_result.in.value == 5 && third_result.out.value == 6 &&
         fourth_result.in1.value == 7 && fourth_result.in2.value == 8 &&
         fourth_result.out.value == 9 && fifth_result.in.value == 10 &&
         fifth_result.out1.value == 11 && fifth_result.out2.value == 12 &&
         sixth_result.min.value == 13 && sixth_result.max.value == 14 &&
         seventh_result.in.value == 15 && seventh_result.found &&
         eighth_result.in.value == 16 && eighth_result.value.value == 17 &&
         ninth_result.out.value == 18 && ninth_result.value.value == 19 &&
         first.in.value == -1 && first.fun.value == -1 &&
         second.in1.value == -1 && second.in2.value == -1 &&
         third.in.value == -1 && third.out.value == -1 &&
         fourth.in1.value == -1 && fourth.in2.value == -1 &&
         fourth.out.value == -1 && fifth.in.value == -1 &&
         fifth.out1.value == -1 && fifth.out2.value == -1 &&
         sixth.min.value == -1 && sixth.max.value == -1 &&
         seventh.in.value == -1 && eighth.in.value == -1 &&
         eighth.value.value == -1 && ninth.out.value == -1 &&
         ninth.value.value == -1;
}

static_assert(rvalue_conversions_work());

bool ftl_test() {
  return aggregate_members_work() && structured_bindings_work() &&
         const_lvalue_conversions_work() && rvalue_conversions_work();
}
