#ifdef FTL_REPLACE_STL
#include <stdexcept>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/stdexcept>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(tested::is_base_of_v<tested::exception, tested::logic_error>);

static_assert(tested::is_base_of_v<tested::logic_error, tested::domain_error>);

static_assert(
    tested::is_base_of_v<tested::logic_error, tested::invalid_argument>);

static_assert(tested::is_base_of_v<tested::logic_error, tested::length_error>);

static_assert(tested::is_base_of_v<tested::logic_error, tested::out_of_range>);

static_assert(tested::is_base_of_v<tested::exception, tested::runtime_error>);

static_assert(tested::is_base_of_v<tested::runtime_error, tested::range_error>);

static_assert(
    tested::is_base_of_v<tested::runtime_error, tested::overflow_error>);

static_assert(
    tested::is_base_of_v<tested::runtime_error, tested::underflow_error>);

static_assert(tested::is_constructible_v<tested::logic_error, const char *>);

static_assert(!tested::is_convertible_v<const char *, tested::logic_error>);

static_assert(tested::is_nothrow_copy_constructible_v<tested::logic_error>);

static_assert(tested::is_nothrow_copy_assignable_v<tested::logic_error>);

static_assert(tested::is_nothrow_copy_constructible_v<tested::out_of_range>);

static_assert(tested::is_nothrow_copy_assignable_v<tested::out_of_range>);

static_assert(tested::is_nothrow_copy_constructible_v<tested::runtime_error>);

static_assert(tested::is_nothrow_copy_assignable_v<tested::runtime_error>);

#if FTL_HAS_EXCEPTIONS

constexpr bool same_text(const char *left, const char *right) noexcept {
  while (*left != '\0' && *right != '\0') {
    if (*left != *right) {
      return false;
    }

    ++left;
    ++right;
  }

  return *left == *right;
}

bool owned_message_works() {
  char message[] = {'m', 'u', 't', 'a', 'b', 'l', 'e', '\0'};

  tested::logic_error value{message};

  message[0] = 'x';

  return same_text(value.what(), "mutable");
}

bool copy_lifetime_works() {
  tested::logic_error retained{"replacement"};

  {
    tested::logic_error original{"shared message"};

    tested::logic_error copy{original};

    if (!same_text(copy.what(), "shared message")) {
      return false;
    }

    retained = original;
  }

  return same_text(retained.what(), "shared message");
}

bool hierarchy_works() {
  tested::domain_error domain{"domain"};

  tested::invalid_argument invalid{"invalid"};

  tested::length_error length{"length"};

  tested::out_of_range range{"range"};

  tested::range_error runtime_range{"runtime range"};

  tested::overflow_error overflow{"overflow"};

  tested::underflow_error underflow{"underflow"};

  return same_text(domain.what(), "domain") &&
         same_text(invalid.what(), "invalid") &&
         same_text(length.what(), "length") &&
         same_text(range.what(), "range") &&
         same_text(runtime_range.what(), "runtime range") &&
         same_text(overflow.what(), "overflow") &&
         same_text(underflow.what(), "underflow");
}

bool catch_works() {
  bool caught_logic = false;
  bool caught_exception = false;

  try {
    throw tested::out_of_range{"array::at"};
  } catch (const tested::logic_error &error) {
    caught_logic = same_text(error.what(), "array::at");
  }

  try {
    throw tested::overflow_error{"overflow"};
  } catch (const tested::exception &error) {
    caught_exception = same_text(error.what(), "overflow");
  }

  return caught_logic && caught_exception;
}

bool ftl_test() {
  return
      owned_message_works() &&
      copy_lifetime_works() &&
      hierarchy_works() &&
      catch_works();
}

#else

bool ftl_test() {
  return true;
}

#endif
