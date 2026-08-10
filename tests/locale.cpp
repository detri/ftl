#ifdef FTL_REPLACE_STL
#include <locale>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/locale>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(tested::time_base::no_order == 0);

static_assert(tested::time_base::dmy != tested::time_base::mdy);

static_assert(tested::time_base::mdy != tested::time_base::ymd);

static_assert(tested::time_base::ymd != tested::time_base::ydm);

struct counting_facet : tested::locale::facet {
  static tested::locale::id id;

  explicit counting_facet(int value, int *destructions = nullptr,
                          tested::size_t refs = 0)
      : tested::locale::facet(refs), value(value), destructions(destructions) {}

  ~counting_facet() override {
    if (destructions != nullptr)
      ++*destructions;
  }

  int value;
  int *destructions;
};

tested::locale::id counting_facet::id;

struct other_facet : tested::locale::facet {
  static tested::locale::id id;

  other_facet() : tested::locale::facet() {}
};

tested::locale::id other_facet::id;

static_assert(tested::locale::none == 0);

static_assert(tested::locale::all ==
              (tested::locale::collate | tested::locale::ctype |
               tested::locale::monetary | tested::locale::numeric |
               tested::locale::time | tested::locale::messages));

static_assert(!tested::is_copy_constructible_v<tested::locale::id>);

static_assert(!tested::is_copy_assignable_v<tested::locale::id>);

static_assert(tested::is_nothrow_copy_constructible_v<tested::locale>);

static_assert(tested::is_nothrow_copy_assignable_v<tested::locale>);

bool ftl_test() {
  const tested::locale &classic = tested::locale::classic();

  if (classic.name() != "C")
    return false;

  tested::locale base;

  if (!(base == classic))
    return false;

  if (tested::has_facet<counting_facet>(base))
    return false;

  //
  // Ordinary refs == 0 ownership.
  //
  int destructions = 0;

  {
    auto *facet = new counting_facet(42, &destructions);

    tested::locale custom(base, facet);

    if (custom.name() != "*")
      return false;

    if (!tested::has_facet<counting_facet>(custom))
      return false;

    if (!tested::has_facet<const counting_facet>(custom))
      return false;

    const counting_facet &used = tested::use_facet<counting_facet>(custom);

    if (&used != facet || used.value != 42)
      return false;

    const counting_facet &used_const =
        tested::use_facet<const counting_facet>(custom);

    if (&used_const != facet)
      return false;

    tested::locale copy = custom;

    if (!(copy == custom))
      return false;

    // A null facet pointer produces a copy, including its name state.
    tested::locale null_copy(custom, static_cast<counting_facet *>(nullptr));

    if (!(null_copy == custom))
      return false;

    if (destructions != 0)
      return false;
  }

  if (destructions != 1)
    return false;

  //
  // Distinct representations can share the same facet.
  //
  int shared_destructions = 0;

  {
    tested::locale first(base, new counting_facet(11, &shared_destructions));

    {
      tested::locale copy = first;

      if (tested::use_facet<counting_facet>(copy).value != 11)
        return false;

      if (shared_destructions != 0)
        return false;
    }

    if (shared_destructions != 0)
      return false;
  }

  if (shared_destructions != 1)
    return false;

  //
  // Replacing a facet keeps the old facet alive for locales that still
  // contain it.
  //
  int first_destructions = 0;
  int second_destructions = 0;

  {
    tested::locale first(base, new counting_facet(1, &first_destructions));

    {
      tested::locale second(first, new counting_facet(2, &second_destructions));

      if (tested::use_facet<counting_facet>(first).value != 1)
        return false;

      if (tested::use_facet<counting_facet>(second).value != 2)
        return false;

      if (first_destructions != 0 || second_destructions != 0)
        return false;
    }

    if (second_destructions != 1)
      return false;

    if (first_destructions != 0)
      return false;
  }

  if (first_destructions != 1)
    return false;

  //
  // refs != 0 means the locale does not destroy the facet.
  //
  int persistent_destructions = 0;

  auto *persistent = new counting_facet(7, &persistent_destructions, 1);

  {
    tested::locale value(base, persistent);

    if (tested::use_facet<counting_facet>(value).value != 7)
      return false;
  }

  if (persistent_destructions != 0)
    return false;

  delete persistent;

  if (persistent_destructions != 1)
    return false;

  //
  // combine takes exactly the requested facet from the other locale.
  //
  int left_destructions = 0;
  int right_destructions = 0;

  {
    tested::locale left(base, new counting_facet(10, &left_destructions));

    tested::locale right(base, new counting_facet(20, &right_destructions));

    tested::locale combined = left.combine<counting_facet>(right);

    if (combined.name() != "*")
      return false;

    if (tested::use_facet<counting_facet>(combined).value != 20)
      return false;

    if (tested::use_facet<counting_facet>(left).value != 10)
      return false;

    if (tested::use_facet<counting_facet>(right).value != 20)
      return false;
  }

  if (left_destructions != 1 || right_destructions != 1)
    return false;

  //
  // Missing use_facet throws bad_cast.
  //
  bool bad_cast_seen = false;

  try {
    (void)tested::use_facet<counting_facet>(base);
  } catch (const tested::bad_cast &) {
    bad_cast_seen = true;
  } catch (...) {
    return false;
  }

  if (!bad_cast_seen)
    return false;

  //
  // Missing combine facet throws runtime_error.
  //
  bool runtime_error_seen = false;

  try {
    (void)base.combine<counting_facet>(base);
  } catch (const tested::runtime_error &) {
    runtime_error_seen = true;
  } catch (...) {
    return false;
  }

  if (!runtime_error_seen)
    return false;

  //
  // global() controls subsequent default construction.
  //
  int global_destructions = 0;

  {
    tested::locale custom(base, new counting_facet(99, &global_destructions));

    tested::locale previous = tested::locale::global(custom);

    tested::locale current;

    const bool installed =
        current == custom && tested::has_facet<counting_facet>(current) &&
        tested::use_facet<counting_facet>(current).value == 99;

    // Restore before evaluating the result so failure never leaves the
    // test process using the custom global locale.
    tested::locale replaced = tested::locale::global(previous);

    if (!installed)
      return false;

    if (!(replaced == custom))
      return false;
  }

  if (global_destructions != 1)
    return false;

  return true;
}
