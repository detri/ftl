#ifdef FTL_REPLACE_STL
#include <stop_token>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/stop_token>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

struct increment_callback {
  int *value;

  void operator()() const noexcept { ++*value; }
};

static_assert(tested::is_default_constructible_v<tested::stop_token>);

static_assert(tested::is_copy_constructible_v<tested::stop_token>);

static_assert(tested::is_move_constructible_v<tested::stop_token>);

static_assert(tested::is_copy_constructible_v<tested::stop_source>);

static_assert(tested::is_move_constructible_v<tested::stop_source>);

using callback_type = tested::stop_callback<increment_callback>;

static_assert(!tested::is_copy_constructible_v<callback_type>);

static_assert(!tested::is_move_constructible_v<callback_type>);

static_assert(
    tested::is_same_v<callback_type::callback_type, increment_callback>);

bool default_token_works() {
  tested::stop_token token;

  return !token.stop_possible() && !token.stop_requested();
}

bool source_and_token_work() {
  tested::stop_source source;

  if (!source.stop_possible())
    return false;

  if (source.stop_requested())
    return false;

  tested::stop_token token = source.get_token();

  if (!token.stop_possible())
    return false;

  if (token.stop_requested())
    return false;

  return true;
}

bool nostopstate_works() {
  tested::stop_source source{tested::nostopstate};

  if (source.stop_possible())
    return false;

  if (source.stop_requested())
    return false;

  if (source.request_stop())
    return false;

  const tested::stop_token token = source.get_token();

  return !token.stop_possible() && !token.stop_requested();
}

bool source_copy_keeps_stop_possible() {
  tested::stop_token token;

  {
    tested::stop_source first;
    token = first.get_token();

    {
      tested::stop_source second = first;

      if (!token.stop_possible())
        return false;

      if (!(first == second))
        return false;
    }

    if (!token.stop_possible())
      return false;
  }

  return !token.stop_possible() && !token.stop_requested();
}

bool requested_state_remains_possible() {
  tested::stop_token token;

  {
    tested::stop_source source;
    token = source.get_token();

    if (!source.request_stop())
      return false;

    if (!token.stop_requested())
      return false;
  }

  return token.stop_possible() && token.stop_requested();
}

bool request_stop_is_one_shot() {
  tested::stop_source source;

  if (!source.request_stop())
    return false;

  if (source.request_stop())
    return false;

  return source.stop_possible() && source.stop_requested();
}

bool registered_callback_runs() {
  tested::stop_source source;
  tested::stop_token token = source.get_token();

  int count = 0;

  tested::stop_callback callback{token, increment_callback{&count}};

  if (count != 0)
    return false;

  if (!source.request_stop())
    return false;

  if (count != 1)
    return false;

  if (source.request_stop())
    return false;

  return count == 1;
}

bool destroyed_callback_is_unregistered() {
  tested::stop_source source;
  tested::stop_token token = source.get_token();

  int count = 0;

  {
    tested::stop_callback callback{token, increment_callback{&count}};
  }

  if (!source.request_stop())
    return false;

  return count == 0;
}

bool late_callback_runs_immediately() {
  tested::stop_source source;
  tested::stop_token token = source.get_token();

  if (!source.request_stop())
    return false;

  int count = 0;

  tested::stop_callback callback{token, increment_callback{&count}};

  return count == 1;
}

bool impossible_token_does_not_register() {
  tested::stop_token token;

  {
    tested::stop_source source;
    token = source.get_token();
  }

  if (token.stop_possible())
    return false;

  int count = 0;

  tested::stop_callback callback{token, increment_callback{&count}};

  return count == 0;
}

bool token_equality_works() {
  tested::stop_source source;

  tested::stop_token first = source.get_token();

  tested::stop_token second = source.get_token();

  tested::stop_token empty_a;
  tested::stop_token empty_b;

  if (!(first == second))
    return false;

  if (first == empty_a)
    return false;

  return empty_a == empty_b;
}

bool source_equality_works() {
  tested::stop_source first;
  tested::stop_source second = first;
  tested::stop_source third;

  if (!(first == second))
    return false;

  if (first == third)
    return false;

  tested::stop_source empty_a{tested::nostopstate};

  tested::stop_source empty_b{tested::nostopstate};

  return empty_a == empty_b;
}

bool token_swap_works() {
  tested::stop_source source;

  tested::stop_token engaged = source.get_token();

  tested::stop_token empty;

  tested::swap(engaged, empty);

  return !engaged.stop_possible() && empty.stop_possible();
}

bool source_swap_works() {
  tested::stop_source engaged;

  tested::stop_source empty{tested::nostopstate};

  tested::swap(engaged, empty);

  return !engaged.stop_possible() && empty.stop_possible();
}

bool multiple_callbacks_work() {
  tested::stop_source source;
  tested::stop_token token = source.get_token();

  int first = 0;
  int second = 0;
  int third = 0;

  tested::stop_callback a{token, increment_callback{&first}};

  tested::stop_callback b{token, increment_callback{&second}};

  tested::stop_callback c{token, increment_callback{&third}};

  if (!source.request_stop())
    return false;

  return first == 1 && second == 1 && third == 1;
}

bool moved_source_works() {
  tested::stop_source original;

  tested::stop_token token = original.get_token();

  tested::stop_source moved{tested::move(original)};

  if (original.stop_possible())
    return false;

  if (!moved.stop_possible())
    return false;

  if (!token.stop_possible())
    return false;

  if (!moved.request_stop())
    return false;

  return token.stop_requested();
}

bool moved_token_works() {
  tested::stop_source source;

  tested::stop_token original = source.get_token();

  tested::stop_token moved{tested::move(original)};

  return !original.stop_possible() && moved.stop_possible() &&
         moved == source.get_token();
}

bool ftl_test() {
  return default_token_works() && source_and_token_work() &&
         nostopstate_works() && source_copy_keeps_stop_possible() &&
         requested_state_remains_possible() && request_stop_is_one_shot() &&
         registered_callback_runs() && destroyed_callback_is_unregistered() &&
         late_callback_runs_immediately() &&
         impossible_token_does_not_register() && token_equality_works() &&
         source_equality_works() && token_swap_works() && source_swap_works() &&
         multiple_callbacks_work() && moved_source_works() &&
         moved_token_works();
}
