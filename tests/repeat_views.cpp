#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/cstddef>
#include <ftl/ranges>
#include <ftl/tuple>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct move_only_value {
  int value;

  constexpr explicit move_only_value(int value) : value(value) {}

  move_only_value(const move_only_value &) = delete;

  constexpr move_only_value(move_only_value &&) = default;

  constexpr move_only_value &operator=(move_only_value &&) = default;
};

struct piecewise_value {
  int first;
  int second;

  constexpr piecewise_value(int first, int second)
      : first(first), second(second) {}
};

template <class Value, class Bound>
concept can_form_repeat_view =
    requires { typename tested::ranges::repeat_view<Value, Bound>; };

template <class Value>
concept can_repeat = requires(Value &&value) {
  tested::ranges::views::repeat(static_cast<Value &&>(value));
};

template <class Value, class Bound>
concept can_repeat_bounded = requires(Value &&value, Bound &&bound) {
  tested::ranges::views::repeat(static_cast<Value &&>(value),
                                static_cast<Bound &&>(bound));
};

using unbounded_view = tested::ranges::repeat_view<int>;

using signed_bounded_view = tested::ranges::repeat_view<int, int>;

using unsigned_bounded_view = tested::ranges::repeat_view<int, unsigned short>;

static_assert(tested::ranges::random_access_range<unbounded_view>);

static_assert(tested::ranges::random_access_range<signed_bounded_view>);

static_assert(!tested::ranges::common_range<unbounded_view>);

static_assert(!tested::ranges::sized_range<unbounded_view>);

static_assert(tested::ranges::common_range<signed_bounded_view>);

static_assert(tested::ranges::sized_range<signed_bounded_view>);

static_assert(tested::same_as<tested::ranges::sentinel_t<unbounded_view>,
                              tested::unreachable_sentinel_t>);

static_assert(tested::same_as<tested::ranges::sentinel_t<signed_bounded_view>,
                              tested::ranges::iterator_t<signed_bounded_view>>);

static_assert(
    tested::same_as<tested::ranges::range_reference_t<signed_bounded_view>,
                    const int &>);

/*
 * Signed bounds are used directly as the iterator's
 * difference type.
 */
static_assert(tested::same_as<
              tested::ranges::range_difference_t<signed_bounded_view>, int>);

using unsigned_difference_type =
    tested::ranges::range_difference_t<unsigned_bounded_view>;

static_assert(tested::signed_integral<unsigned_difference_type>);

static_assert(sizeof(unsigned_difference_type) >= sizeof(unsigned short));

static_assert(tested::same_as<
              decltype(tested::declval<const signed_bounded_view &>().size()),
              unsigned int>);

static_assert(tested::same_as<
              decltype(tested::declval<const unsigned_bounded_view &>().size()),
              unsigned short>);

using bounded_iterator = tested::ranges::iterator_t<signed_bounded_view>;

static_assert(tested::random_access_iterator<bounded_iterator>);

static_assert(tested::same_as<decltype(tested::declval<bounded_iterator &>()++),
                              bounded_iterator>);

static_assert(
    tested::same_as<decltype(tested::declval<const bounded_iterator &>()[0]),
                    const int &>);

static_assert(noexcept(tested::declval<const unbounded_view &>().end()));

static_assert(!noexcept(tested::declval<const signed_bounded_view &>().end()));

static_assert(tested::same_as<decltype(tested::ranges::views::repeat(7)),
                              tested::ranges::repeat_view<int>>);

static_assert(
    tested::same_as<decltype(tested::ranges::views::repeat(
                        7, static_cast<unsigned short>(3))),
                    tested::ranges::repeat_view<int, unsigned short>>);

static_assert(can_form_repeat_view<int, int>);

static_assert(can_form_repeat_view<int, unsigned int>);

static_assert(!can_form_repeat_view<int, bool>);

static_assert(!can_form_repeat_view<int, double>);

static_assert(!can_form_repeat_view<const int, int>);

static_assert(can_repeat<int>);

static_assert(can_repeat<move_only_value>);

static_assert(!can_repeat<move_only_value &>);

static_assert(can_repeat_bounded<int, int>);

static_assert(can_repeat_bounded<int, unsigned short>);

static_assert(!can_repeat_bounded<int, bool>);

static_assert(!can_repeat_bounded<int, double>);

static_assert(tested::default_initializable<signed_bounded_view>);

static_assert(!tested::default_initializable<
              tested::ranges::repeat_view<move_only_value>>);

constexpr bool repeat_works() {
  auto bounded = tested::ranges::views::repeat(7, 3);

  if (bounded.size() != 3)
    return false;

  if (bounded[0] != 7 || bounded[2] != 7)
    return false;

  auto first = bounded.begin();
  auto last = bounded.end();

  if (last - first != 3)
    return false;

  if (first + 3 != last)
    return false;

  auto middle = last;

  --middle;

  if (*middle != 7)
    return false;

  middle -= 2;

  if (middle != first)
    return false;

  auto unsigned_bounded =
      tested::ranges::views::repeat(5, static_cast<unsigned short>(2));

  if (unsigned_bounded.size() != 2)
    return false;

  if (unsigned_bounded.end() - unsigned_bounded.begin() != 2)
    return false;

  auto unbounded = tested::ranges::views::repeat(4);

  if (*unbounded.begin() != 4)
    return false;

  if (*(unbounded.begin() + 100) != 4)
    return false;

  auto moved = tested::ranges::views::repeat(move_only_value{9}, 2);

  if ((*moved.begin()).value != 9)
    return false;

  tested::ranges::repeat_view<piecewise_value, int> piecewise(
      tested::piecewise_construct, tested::tuple<int, int>(2, 3),
      tested::tuple<int>(2));

  if (piecewise.size() != 2)
    return false;

  if ((*piecewise.begin()).first != 2)
    return false;

  if ((*piecewise.begin()).second != 3)
    return false;

  signed_bounded_view defaulted;

  if (defaulted.size() != 0)
    return false;

  if (*defaulted.begin() != 0)
    return false;

  return true;
}

static_assert(repeat_works());

bool ftl_test() { return repeat_works(); }