#ifdef FTL_REPLACE_STL
#include <iterator>
namespace tested = std;
#else
#include <ftl/iterator>
namespace tested = ftl;
#endif

static_assert(tested::contiguous_iterator<int *>);
static_assert(tested::random_access_iterator<int *>);
static_assert(tested::same_as<tested::iter_value_t<const int *>, int>);
static_assert(
    tested::same_as<tested::iter_difference_t<int *>, tested::ptrdiff_t>);
static_assert(
    tested::same_as<tested::const_iterator<const int *>, const int *>);
static_assert(tested::same_as<tested::const_iterator<int *>,
                              tested::basic_const_iterator<int *>>);

static_assert(tested::is_same_v<
              tested::iterator_traits<const tested::byte *>::iterator_concept,
              tested::contiguous_iterator_tag>);

static_assert(
    tested::is_same_v<tested::detail::iter_concept_t<const tested::byte *>,
                      tested::contiguous_iterator_tag>);

static_assert(tested::contiguous_iterator<const tested::byte *>);

struct output {
  using value_type = int;
  int values[4]{};
  int count{};
  constexpr void push_back(int value) { values[count++] = value; }
};

template <class T>
concept has_iterator_category = requires { typename T::iterator_category; };

struct concept_only_input_iterator {
  using value_type = int;
  using difference_type = tested::ptrdiff_t;
  using iterator_concept = tested::input_iterator_tag;

  int *current = nullptr;

  constexpr int &operator*() const noexcept { return *current; }

  constexpr concept_only_input_iterator &operator++() noexcept {
    ++current;
    return *this;
  }

  constexpr void operator++(int) noexcept { ++current; }

  friend constexpr bool operator==(concept_only_input_iterator,
                                   concept_only_input_iterator) = default;
};

static_assert(
    !has_iterator_category<tested::move_iterator<concept_only_input_iterator>>);

static_assert(
    tested::is_same_v<typename tested::move_iterator<int *>::iterator_concept,
                      tested::random_access_iterator_tag>);

static_assert(
    tested::is_same_v<typename tested::move_iterator<int *>::iterator_category,
                      tested::random_access_iterator_tag>);

static_assert(tested::random_access_iterator<tested::move_iterator<int *>>);

static_assert(!tested::contiguous_iterator<tested::move_iterator<int *>>);

constexpr bool iterator_works() {
  int values[]{1, 2, 3, 4};
  auto p = tested::next(values, 2);
  if (*p != 3 || tested::distance(values, p) != 2)
    return false;
  tested::ranges::advance(p, -1);
  if (*p != 2 || *tested::rbegin(values) != 4)
    return false;
  tested::ranges::iter_swap(values, values + 3);
  tested::reverse_iterator reverse{values + 4};
  tested::move_iterator moving{values};
  tested::counted_iterator counted{values, 4};
  tested::basic_const_iterator constant{values};
  output out;
  auto sink = tested::back_inserter(out);
  *sink++ = *constant;
  return *reverse == 1 && *moving == 4 && counted.count() == 4 &&
         out.values[0] == 4 && values[0] == 4 && values[3] == 1 &&
         tested::size(values) == 4 && !tested::empty(values);
}
static_assert(iterator_works());
bool ftl_test() { return iterator_works(); }
