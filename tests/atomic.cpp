#ifdef FTL_REPLACE_STL
#include <atomic>
#include <bit>
#include <cstdint>
#include <limits>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/atomic>
#include <ftl/bit>
#include <ftl/cstdint>
#include <ftl/limits>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

struct atomic_pair {
  tested::uint16_t first;
  tested::uint16_t second;
};

struct atomic_triple {
  tested::uint32_t first;
  tested::uint32_t second;
  tested::uint32_t third;
};

static_assert(sizeof(atomic_pair) == 4);
static_assert(tested::is_trivially_copyable_v<atomic_pair>);

static_assert(sizeof(atomic_triple) == 12);
static_assert(tested::is_trivially_copyable_v<atomic_triple>);

static_assert(!tested::atomic<atomic_triple>::is_always_lock_free);

static_assert(!tested::atomic_ref<atomic_triple>::is_always_lock_free);

static_assert(tested::atomic_ref<atomic_triple>::required_alignment >=
              alignof(atomic_triple));

struct padded_atomic_value {
  tested::uint8_t first;
  tested::uint32_t second;
};

static_assert(sizeof(padded_atomic_value) == 8);
static_assert(tested::is_trivially_copyable_v<padded_atomic_value>);

template <class Atomic>
concept has_volatile_store =
    requires(volatile Atomic &value, typename Atomic::value_type desired) {
      value.store(desired);
    };

template <class Atomic>
concept has_volatile_load =
    requires(const volatile Atomic &value) { value.load(); };

template <class Atomic>
concept has_volatile_exchange =
    requires(volatile Atomic &value, typename Atomic::value_type desired) {
      value.exchange(desired);
    };

template <class Atomic>
concept has_volatile_compare_exchange =
    requires(volatile Atomic &value, typename Atomic::value_type expected,
             typename Atomic::value_type desired) {
      value.compare_exchange_strong(expected, desired);
    };

template <class Atomic>
concept has_volatile_wait =
    requires(const volatile Atomic &value, typename Atomic::value_type old) {
      value.wait(old);
    };

static_assert(has_volatile_store<tested::atomic<int>>);

static_assert(has_volatile_load<tested::atomic<int>>);

static_assert(has_volatile_exchange<tested::atomic<int>>);

static_assert(has_volatile_compare_exchange<tested::atomic<int>>);

static_assert(!has_volatile_store<tested::atomic<atomic_triple>>);

static_assert(!has_volatile_load<tested::atomic<atomic_triple>>);

static_assert(!has_volatile_exchange<tested::atomic<atomic_triple>>);

static_assert(!has_volatile_compare_exchange<tested::atomic<atomic_triple>>);

// Volatile wait remains available even when the
// ordinary volatile atomic operations are constrained.
static_assert(has_volatile_wait<tested::atomic<atomic_triple>>);

static_assert(tested::atomic_bool::is_always_lock_free);
static_assert(tested::atomic_int::is_always_lock_free);
static_assert(tested::atomic<float>::is_always_lock_free);
static_assert(tested::atomic<double>::is_always_lock_free);
static_assert(tested::atomic<int *>::is_always_lock_free);
static_assert(tested::atomic<atomic_pair>::is_always_lock_free);

static_assert(tested::is_same_v<tested::atomic_int::difference_type, int>);

static_assert(tested::is_same_v<tested::atomic<float>::difference_type, float>);

static_assert(tested::is_same_v<tested::atomic<int *>::difference_type,
                                tested::ptrdiff_t>);

template <class T>
concept has_difference_type = requires { typename T::difference_type; };

template <class T>
concept has_fetch_add = requires(T value) { value.fetch_add(1); };

template <class T>
concept has_increment = requires(T value) {
  ++value;
  value++;
};

static_assert(tested::atomic_ref<int>::required_alignment >= alignof(int));

static_assert(tested::atomic_ref<double>::required_alignment >=
              alignof(double));

static_assert(tested::atomic_ref<atomic_pair>::required_alignment >=
              alignof(atomic_pair));

static_assert(tested::atomic_ref<int>::is_always_lock_free);
static_assert(tested::atomic_ref<float>::is_always_lock_free);
static_assert(tested::atomic_ref<double>::is_always_lock_free);
static_assert(tested::atomic_ref<int *>::is_always_lock_free);
static_assert(tested::atomic_ref<atomic_pair>::is_always_lock_free);

static_assert(tested::is_same_v<tested::atomic_ref<int>::value_type, int>);

static_assert(tested::is_same_v<tested::atomic_ref<int>::difference_type, int>);

static_assert(
    tested::is_same_v<tested::atomic_ref<float>::difference_type, float>);

static_assert(tested::is_same_v<tested::atomic_ref<int *>::difference_type,
                                tested::ptrdiff_t>);

static_assert(has_fetch_add<tested::atomic_ref<int>>);
static_assert(has_fetch_add<tested::atomic_ref<float>>);
static_assert(has_fetch_add<tested::atomic_ref<int *>>);
static_assert(!has_fetch_add<tested::atomic_ref<bool>>);
static_assert(!has_fetch_add<tested::atomic_ref<atomic_pair>>);

static_assert(!has_difference_type<tested::atomic_bool>);
static_assert(!has_difference_type<tested::atomic<atomic_pair>>);

static_assert(!has_fetch_add<tested::atomic_bool>);
static_assert(!has_fetch_add<tested::atomic<atomic_pair>>);

static_assert(has_fetch_add<tested::atomic_int>);
static_assert(has_fetch_add<tested::atomic<float>>);
static_assert(has_fetch_add<tested::atomic<int *>>);

static_assert(!has_fetch_add<tested::atomic<void *>>);

static_assert(has_increment<tested::atomic_int>);
static_assert(has_increment<tested::atomic<int *>>);
static_assert(!has_increment<tested::atomic<float>>);

static_assert(ATOMIC_BOOL_LOCK_FREE == 2);
static_assert(ATOMIC_CHAR_LOCK_FREE == 2);
static_assert(ATOMIC_SHORT_LOCK_FREE == 2);
static_assert(ATOMIC_INT_LOCK_FREE == 2);
static_assert(ATOMIC_LONG_LOCK_FREE == 2);
static_assert(ATOMIC_LLONG_LOCK_FREE == 2);
static_assert(ATOMIC_POINTER_LOCK_FREE == 2);

static_assert(tested::is_standard_layout_v<tested::atomic_flag>);

static_assert(tested::is_trivially_destructible_v<tested::atomic_flag>);

static_assert(!tested::is_copy_constructible_v<tested::atomic_flag>);

static_assert(tested::atomic_signed_lock_free::is_always_lock_free);

static_assert(tested::atomic_unsigned_lock_free::is_always_lock_free);

bool basic_operations_work() {
  tested::atomic_int value{10};

  if (value.load(tested::memory_order_relaxed) != 10)
    return false;

  value.store(20, tested::memory_order_release);

  if (value.load(tested::memory_order_acquire) != 20)
    return false;

  if (value.exchange(30) != 20)
    return false;

  if (value.load() != 30)
    return false;

  value = 40;

  return value.load() == 40 && static_cast<int>(value) == 40;
}

bool compare_exchange_works() {
  tested::atomic_int value{10};

  int expected = 10;

  if (!value.compare_exchange_strong(expected, 20, tested::memory_order_acq_rel,
                                     tested::memory_order_acquire)) {
    return false;
  }

  if (expected != 10 || value.load() != 20)
    return false;

  expected = 10;

  if (value.compare_exchange_strong(expected, 30))
    return false;

  if (expected != 20 || value.load() != 20)
    return false;

  expected = 20;

  while (!value.compare_exchange_weak(expected, 50,
                                      tested::memory_order_release,
                                      tested::memory_order_relaxed)) {
  }

  return value.load() == 50;
}

bool generic_atomic_works() {
  tested::atomic<atomic_pair> value{atomic_pair{1, 2}};

  const atomic_pair first = value.load();

  if (first.first != 1 || first.second != 2)
    return false;

  const atomic_pair old = value.exchange(atomic_pair{3, 4});

  if (old.first != 1 || old.second != 2)
    return false;

  atomic_pair expected{3, 4};

  if (!value.compare_exchange_strong(expected, atomic_pair{5, 6})) {
    return false;
  }

  const atomic_pair current = value.load();

  if (current.first != 5 || current.second != 6)
    return false;

  expected = atomic_pair{1, 2};

  if (value.compare_exchange_strong(expected, atomic_pair{7, 8})) {
    return false;
  }

  return expected.first == 5 && expected.second == 6;
}

bool arithmetic_works() {
  tested::atomic_int value{10};

  if (value.fetch_add(5) != 10 || value.load() != 15) {
    return false;
  }

  if (value.fetch_sub(3) != 15 || value.load() != 12) {
    return false;
  }

  if (value++ != 12 || value.load() != 13) {
    return false;
  }

  if (++value != 14 || value.load() != 14) {
    return false;
  }

  if (value-- != 14 || value.load() != 13) {
    return false;
  }

  if (--value != 12 || value.load() != 12) {
    return false;
  }

  if ((value += 8) != 20 || value.load() != 20) {
    return false;
  }

  return (value -= 5) == 15 && value.load() == 15;
}

bool signed_wraparound_works() {
  using limits = tested::numeric_limits<int>;

  tested::atomic_int value{limits::max()};

  if (value.fetch_add(1) != limits::max() || value.load() != limits::min()) {
    return false;
  }

  if (value.fetch_sub(1) != limits::min() || value.load() != limits::max()) {
    return false;
  }

  value.store(limits::min());

  if (value.fetch_sub(limits::min()) != limits::min() || value.load() != 0) {
    return false;
  }

  value.store(limits::max());

  if (++value != limits::min())
    return false;

  value.store(limits::min());

  return --value == limits::max();
}

bool bitwise_works() {
  tested::atomic_uint value{0b1100u};

  if (value.fetch_and(0b1010u) != 0b1100u || value.load() != 0b1000u) {
    return false;
  }

  if (value.fetch_or(0b0011u) != 0b1000u || value.load() != 0b1011u) {
    return false;
  }

  if (value.fetch_xor(0b1111u) != 0b1011u || value.load() != 0b0100u) {
    return false;
  }

  if ((value &= 0b0110u) != 0b0100u)
    return false;

  if ((value |= 0b1000u) != 0b1100u)
    return false;

  return (value ^= 0b1111u) == 0b0011u && value.load() == 0b0011u;
}

bool floating_works() {
  tested::atomic<float> value{1.5f};

  if (value.fetch_add(2.0f) != 1.5f)
    return false;

  if (value.load() != 3.5f)
    return false;

  if (value.fetch_sub(1.0f) != 3.5f)
    return false;

  if (value.load() != 2.5f)
    return false;

  if ((value += 0.5f) != 3.0f)
    return false;

  if ((value -= 1.25f) != 1.75f)
    return false;

  tested::atomic<double> wide{4.0};

  if (wide.fetch_add(2.5) != 4.0)
    return false;

  return wide.load() == 6.5;
}

bool floating_compare_uses_representation() {
  tested::atomic<float> value{-0.0f};

  float expected = 0.0f;

  if (value.compare_exchange_strong(expected, 1.0f)) {
    return false;
  }

  if constexpr (sizeof(float) == sizeof(tested::uint32_t)) {
    const auto expected_bits = tested::bit_cast<tested::uint32_t>(expected);

    const auto negative_zero_bits = tested::bit_cast<tested::uint32_t>(-0.0f);

    if (expected_bits != negative_zero_bits)
      return false;
  }

  expected = -0.0f;

  if (!value.compare_exchange_strong(expected, 1.0f)) {
    return false;
  }

  return value.load() == 1.0f;
}

bool pointer_works() {
  int values[8]{};

  tested::atomic<int *> pointer{values};

  if (pointer.load() != values)
    return false;

  if (pointer.fetch_add(2) != values)
    return false;

  if (pointer.load() != values + 2)
    return false;

  if (pointer++ != values + 2)
    return false;

  if (pointer.load() != values + 3)
    return false;

  if (++pointer != values + 4)
    return false;

  if (pointer.fetch_sub(2) != values + 4)
    return false;

  if (pointer.load() != values + 2)
    return false;

  if ((pointer += 3) != values + 5)
    return false;

  if ((pointer -= 4) != values + 1)
    return false;

  return pointer.load() == values + 1;
}

bool void_pointer_basic_operations_work() {
  int value = 42;
  void *address = &value;

  tested::atomic<void *> pointer{address};

  if (pointer.load() != address)
    return false;

  if (pointer.exchange(nullptr) != address)
    return false;

  if (pointer.load() != nullptr)
    return false;

  pointer.store(address);

  void *expected = address;

  return pointer.compare_exchange_strong(expected, nullptr) &&
         pointer.load() == nullptr;
}

bool bool_works() {
  tested::atomic_bool value{false};

  if (value.load())
    return false;

  value.store(true, tested::memory_order_release);

  if (!value.load(tested::memory_order_acquire)) {
    return false;
  }

  if (!value.exchange(false))
    return false;

  if (value.load())
    return false;

  bool expected = false;

  return value.compare_exchange_strong(expected, true) && value.load();
}

bool volatile_works() {
  volatile tested::atomic_int integer{7};

  if (integer.fetch_add(5) != 7 || integer.load() != 12) {
    return false;
  }

  volatile tested::atomic<float> floating{2.0f};

  if (floating.fetch_add(1.5f) != 2.0f || floating.load() != 3.5f) {
    return false;
  }

  int values[2]{};
  volatile tested::atomic<int *> pointer{values};

  if (pointer.fetch_add(1) != values || pointer.load() != values + 1) {
    return false;
  }

  return true;
}

bool atomic_ref_basic_works() {
  alignas(tested::atomic_ref<int>::required_alignment) int value = 10;

  tested::atomic_ref<int> reference{value};

  if (reference.load() != 10)
    return false;

  reference.store(20, tested::memory_order_release);

  if (reference.load(tested::memory_order_acquire) != 20) {
    return false;
  }

  if (reference.exchange(30) != 20)
    return false;

  if (reference.load() != 30)
    return false;

  reference = 40;

  return reference.load() == 40;
}

bool atomic_ref_copies_share_object() {
  alignas(tested::atomic_ref<int>::required_alignment) int value = 1;

  tested::atomic_ref<int> first{value};
  tested::atomic_ref<int> second{first};

  first.store(10);

  if (second.load() != 10)
    return false;

  second.store(20);

  return first.load() == 20;
}

bool atomic_ref_compare_exchange_works() {
  alignas(tested::atomic_ref<int>::required_alignment) int value = 10;

  tested::atomic_ref<int> reference{value};

  int expected = 10;

  if (!reference.compare_exchange_strong(expected, 20,
                                         tested::memory_order_acq_rel,
                                         tested::memory_order_acquire)) {
    return false;
  }

  if (reference.load() != 20)
    return false;

  expected = 5;

  if (reference.compare_exchange_strong(expected, 30)) {
    return false;
  }

  return expected == 20 && reference.load() == 20;
}

bool atomic_ref_generic_works() {
  alignas(tested::atomic_ref<atomic_pair>::required_alignment)
      atomic_pair value{1, 2};

  tested::atomic_ref<atomic_pair> reference{value};

  const atomic_pair first = reference.load();

  if (first.first != 1 || first.second != 2) {
    return false;
  }

  const atomic_pair old = reference.exchange(atomic_pair{3, 4});

  if (old.first != 1 || old.second != 2) {
    return false;
  }

  atomic_pair expected{3, 4};

  if (!reference.compare_exchange_strong(expected, atomic_pair{5, 6})) {
    return false;
  }

  const atomic_pair current = reference.load();

  return current.first == 5 && current.second == 6;
}

bool atomic_ref_integer_works() {
  alignas(tested::atomic_ref<int>::required_alignment) int value = 10;

  tested::atomic_ref<int> reference{value};

  if (reference.fetch_add(5) != 10 || reference.load() != 15) {
    return false;
  }

  if (reference.fetch_sub(3) != 15 || reference.load() != 12) {
    return false;
  }

  if (reference++ != 12 || reference.load() != 13) {
    return false;
  }

  if (++reference != 14 || reference.load() != 14) {
    return false;
  }

  if (reference-- != 14 || reference.load() != 13) {
    return false;
  }

  if (--reference != 12 || reference.load() != 12) {
    return false;
  }

  if ((reference += 8) != 20)
    return false;

  return (reference -= 5) == 15 && reference.load() == 15;
}

bool atomic_ref_signed_wraparound_works() {
  using limits = tested::numeric_limits<int>;

  alignas(tested::atomic_ref<int>::required_alignment) int value =
      limits::max();

  tested::atomic_ref<int> reference{value};

  if (reference.fetch_add(1) != limits::max() ||
      reference.load() != limits::min()) {
    return false;
  }

  if (reference.fetch_sub(1) != limits::min() ||
      reference.load() != limits::max()) {
    return false;
  }

  reference.store(limits::min());

  return --reference == limits::max();
}

bool atomic_ref_bitwise_works() {
  alignas(tested::atomic_ref<unsigned>::required_alignment) unsigned value =
      0b1100u;

  tested::atomic_ref<unsigned> reference{value};

  if (reference.fetch_and(0b1010u) != 0b1100u || reference.load() != 0b1000u) {
    return false;
  }

  if (reference.fetch_or(0b0011u) != 0b1000u || reference.load() != 0b1011u) {
    return false;
  }

  if (reference.fetch_xor(0b1111u) != 0b1011u || reference.load() != 0b0100u) {
    return false;
  }

  if ((reference |= 0b1000u) != 0b1100u)
    return false;

  if ((reference &= 0b1010u) != 0b1000u)
    return false;

  return (reference ^= 0b0011u) == 0b1011u && reference.load() == 0b1011u;
}

bool atomic_ref_floating_works() {
  alignas(tested::atomic_ref<float>::required_alignment) float value = 1.5f;

  tested::atomic_ref<float> reference{value};

  if (reference.fetch_add(2.0f) != 1.5f || reference.load() != 3.5f) {
    return false;
  }

  if (reference.fetch_sub(1.0f) != 3.5f || reference.load() != 2.5f) {
    return false;
  }

  if ((reference += 0.5f) != 3.0f)
    return false;

  return (reference -= 1.25f) == 1.75f && reference.load() == 1.75f;
}

bool atomic_ref_floating_representation_works() {
  alignas(tested::atomic_ref<float>::required_alignment) float value = -0.0f;

  tested::atomic_ref<float> reference{value};

  float expected = 0.0f;

  if (reference.compare_exchange_strong(expected, 1.0f)) {
    return false;
  }

  const auto expected_bits = tested::bit_cast<tested::uint32_t>(expected);

  const auto negative_zero_bits = tested::bit_cast<tested::uint32_t>(-0.0f);

  if (expected_bits != negative_zero_bits)
    return false;

  expected = -0.0f;

  return reference.compare_exchange_strong(expected, 1.0f) &&
         reference.load() == 1.0f;
}

bool atomic_ref_pointer_works() {
  int values[8]{};

  alignas(tested::atomic_ref<int *>::required_alignment) int *pointer = values;

  tested::atomic_ref<int *> reference{pointer};

  if (reference.fetch_add(2) != values || reference.load() != values + 2) {
    return false;
  }

  if (reference++ != values + 2 || reference.load() != values + 3) {
    return false;
  }

  if (++reference != values + 4)
    return false;

  if ((reference += 2) != values + 6)
    return false;

  if ((reference -= 5) != values + 1)
    return false;

  return reference.load() == values + 1;
}

bool atomic_ref_bool_works() {
  alignas(tested::atomic_ref<bool>::required_alignment) bool value = false;

  tested::atomic_ref<bool> reference{value};

  if (reference.load())
    return false;

  reference.store(true);

  if (!reference.load())
    return false;

  if (!reference.exchange(false))
    return false;

  bool expected = false;

  return reference.compare_exchange_strong(expected, true) && reference.load();
}

bool atomic_nonmembers_work() {
  tested::atomic_int value{10};

  if (!tested::atomic_is_lock_free(&value))
    return false;

  tested::atomic_store(&value, 20);

  if (tested::atomic_load(&value) != 20)
    return false;

  tested::atomic_store_explicit(&value, 30, tested::memory_order_release);

  if (tested::atomic_load_explicit(&value, tested::memory_order_acquire) !=
      30) {
    return false;
  }

  if (tested::atomic_exchange(&value, 40) != 30)
    return false;

  if (tested::atomic_exchange_explicit(&value, 50,
                                       tested::memory_order_acq_rel) != 40) {
    return false;
  }

  int expected = 50;

  if (!tested::atomic_compare_exchange_strong(&value, &expected, 60)) {
    return false;
  }

  expected = 10;

  if (tested::atomic_compare_exchange_weak_explicit(
          &value, &expected, 70, tested::memory_order_acq_rel,
          tested::memory_order_acquire)) {
    return false;
  }

  if (expected != 60)
    return false;

  if (tested::atomic_fetch_add(&value, 5) != 60)
    return false;

  if (tested::atomic_fetch_sub_explicit(&value, 3,
                                        tested::memory_order_relaxed) != 65) {
    return false;
  }

  if (tested::atomic_fetch_or(&value, 0x10) != 62)
    return false;

  if (tested::atomic_fetch_and(&value, 0x3f) != 62)
    return false;

  if (tested::atomic_fetch_xor(&value, 1) != 62)
    return false;

  return value.load() == 63;
}

bool atomic_nonmember_specializations_work() {
  tested::atomic<float> floating{1.5f};

  if (tested::atomic_fetch_add(&floating, 2.0f) != 1.5f) {
    return false;
  }

  if (tested::atomic_fetch_sub_explicit(&floating, 1.0f,
                                        tested::memory_order_relaxed) != 3.5f) {
    return false;
  }

  if (floating.load() != 2.5f)
    return false;

  int values[4]{};
  tested::atomic<int *> pointer{values};

  if (tested::atomic_fetch_add(&pointer, 2) != values) {
    return false;
  }

  if (pointer.load() != values + 2)
    return false;

  if (tested::atomic_fetch_sub(&pointer, 1) != values + 2) {
    return false;
  }

  return pointer.load() == values + 1;
}

bool atomic_nonmember_generic_works() {
  tested::atomic<atomic_pair> value{atomic_pair{1, 2}};

  const atomic_pair loaded = tested::atomic_load(&value);

  if (loaded.first != 1 || loaded.second != 2) {
    return false;
  }

  const atomic_pair old = tested::atomic_exchange(&value, atomic_pair{3, 4});

  if (old.first != 1 || old.second != 2) {
    return false;
  }

  atomic_pair expected{3, 4};

  if (!tested::atomic_compare_exchange_strong(&value, &expected,
                                              atomic_pair{5, 6})) {
    return false;
  }

  const atomic_pair current = value.load();

  return current.first == 5 && current.second == 6;
}

bool volatile_nonmembers_work() {
  volatile tested::atomic_int value{10};

  tested::atomic_store_explicit(&value, 20, tested::memory_order_release);

  if (tested::atomic_load_explicit(&value, tested::memory_order_acquire) !=
      20) {
    return false;
  }

  if (tested::atomic_fetch_add(&value, 2) != 20)
    return false;

  int expected = 22;

  return tested::atomic_compare_exchange_strong(&value, &expected, 30) &&
         value.load() == 30;
}

bool atomic_flag_works() {
  tested::atomic_flag flag = ATOMIC_FLAG_INIT;

  if (flag.test())
    return false;

  if (flag.test_and_set())
    return false;

  if (!flag.test())
    return false;

  if (!flag.test_and_set())
    return false;

  flag.clear(tested::memory_order_release);

  if (flag.test(tested::memory_order_acquire))
    return false;

  if (tested::atomic_flag_test_and_set(&flag))
    return false;

  if (!tested::atomic_flag_test(&flag))
    return false;

  tested::atomic_flag_clear_explicit(&flag, tested::memory_order_release);

  if (tested::atomic_flag_test_explicit(&flag, tested::memory_order_acquire)) {
    return false;
  }

  if (tested::atomic_flag_test_and_set_explicit(&flag,
                                                tested::memory_order_acq_rel)) {
    return false;
  }

  tested::atomic_flag_clear(&flag);

  return !flag.test();
}

bool volatile_atomic_flag_works() {
  volatile tested::atomic_flag flag{};

  if (flag.test())
    return false;

  if (tested::atomic_flag_test_and_set(&flag))
    return false;

  if (!tested::atomic_flag_test(&flag))
    return false;

  tested::atomic_flag_clear_explicit(&flag, tested::memory_order_release);

  return !tested::atomic_flag_test_explicit(&flag,
                                            tested::memory_order_acquire);
}

bool deprecated_atomic_init_works() {
  tested::atomic_int value = ATOMIC_VAR_INIT(5);

  if (value.load() != 5)
    return false;

  tested::atomic_init(&value, 10);

  if (value.load(tested::memory_order_relaxed) != 10) {
    return false;
  }

  volatile tested::atomic_int volatile_value{};

  tested::atomic_init(&volatile_value, 20);

  return volatile_value.load(tested::memory_order_relaxed) == 20;
}

bool fences_work() {
  tested::atomic_signal_fence(tested::memory_order_relaxed);
  tested::atomic_signal_fence(tested::memory_order_acquire);
  tested::atomic_signal_fence(tested::memory_order_release);
  tested::atomic_signal_fence(tested::memory_order_acq_rel);
  tested::atomic_signal_fence(tested::memory_order_seq_cst);

  tested::atomic_thread_fence(tested::memory_order_relaxed);
  tested::atomic_thread_fence(tested::memory_order_consume);
  tested::atomic_thread_fence(tested::memory_order_acquire);
  tested::atomic_thread_fence(tested::memory_order_release);
  tested::atomic_thread_fence(tested::memory_order_acq_rel);
  tested::atomic_thread_fence(tested::memory_order_seq_cst);

  return true;
}

bool kill_dependency_works() {
  int value = 42;

  return tested::kill_dependency(value) == 42;
}

bool atomic_wait_immediate_return_works() {
  tested::atomic_int integer{1};
  integer.wait(0);
  tested::atomic_wait(&integer, 0);
  tested::atomic_wait_explicit(&integer, 0, tested::memory_order_acquire);

  tested::atomic_uint8_t narrow{1};
  narrow.wait(0);

  tested::atomic_uint16_t medium{1};
  medium.wait(0);

  tested::atomic_uint64_t wide{1};
  wide.wait(0);

  tested::atomic<float> floating{-0.0f};

  // Numerically equal, representation unequal.
  floating.wait(0.0f);

  integer.notify_one();
  integer.notify_all();

  tested::atomic_notify_one(&integer);
  tested::atomic_notify_all(&integer);

  return true;
}

bool atomic_ref_wait_immediate_return_works() {
  alignas(tested::atomic_ref<int>::required_alignment) int integer = 1;

  tested::atomic_ref<int> reference{integer};

  reference.wait(0);
  reference.notify_one();
  reference.notify_all();

  alignas(tested::atomic_ref<float>::required_alignment) float floating = -0.0f;

  tested::atomic_ref<float> floating_reference{floating};

  floating_reference.wait(0.0f);

  return true;
}

bool atomic_flag_wait_immediate_return_works() {
  tested::atomic_flag flag{};

  flag.wait(true);

  tested::atomic_flag_wait(&flag, true);

  tested::atomic_flag_wait_explicit(&flag, true, tested::memory_order_acquire);

  flag.notify_one();
  flag.notify_all();

  tested::atomic_flag_notify_one(&flag);
  tested::atomic_flag_notify_all(&flag);

  return true;
}

bool platform_wait_backend_links() {
  alignas(4) tested::uint32_t word = 1;

  tested::detail::platform_wait(&word, 0, sizeof(word));

  tested::detail::platform_wake_one(&word, sizeof(word));

  tested::detail::platform_wake_all(&word, sizeof(word));

  return true;
}

bool pointer_min_difference_does_not_overflow() {
  int values[1]{};

  constexpr tested::ptrdiff_t minimum =
      tested::numeric_limits<tested::ptrdiff_t>::min();

  tested::atomic<int *> pointer{values};

  if (pointer.fetch_sub(minimum) != values)
    return false;

  // Reversing the same modular representation
  // operation must restore the original pointer.
  (void)pointer.fetch_add(minimum);

  if (pointer.load() != values)
    return false;

  alignas(tested::atomic_ref<int *>::required_alignment) int *referenced =
      values;

  tested::atomic_ref<int *> reference{referenced};

  if (reference.fetch_sub(minimum) != values)
    return false;

  (void)reference.fetch_add(minimum);

  return reference.load() == values;
}

bool non_lock_free_atomic_works() {
  const atomic_triple first{1, 2, 3};

  const atomic_triple second{4, 5, 6};

  const atomic_triple third{7, 8, 9};

  tested::atomic<atomic_triple> value{first};

  if (value.is_lock_free())
    return false;

  auto observed = value.load();

  if (observed.first != 1 || observed.second != 2 || observed.third != 3) {
    return false;
  }

  value.store(second);

  const auto old = value.exchange(third);

  if (old.first != 4 || old.second != 5 || old.third != 6) {
    return false;
  }

  atomic_triple expected = third;

  if (!value.compare_exchange_strong(expected, first)) {
    return false;
  }

  expected = second;

  if (value.compare_exchange_strong(expected, third)) {
    return false;
  }

  return expected.first == 1 && expected.second == 2 && expected.third == 3;
}

bool non_lock_free_atomic_ref_works() {
  alignas(tested::atomic_ref<atomic_triple>::required_alignment)
      atomic_triple storage{1, 2, 3};

  tested::atomic_ref<atomic_triple> first{storage};

  tested::atomic_ref<atomic_triple> second{storage};

  if (first.is_lock_free() || second.is_lock_free()) {
    return false;
  }

  second.store(atomic_triple{4, 5, 6});

  auto observed = first.load();

  if (observed.first != 4 || observed.second != 5 || observed.third != 6) {
    return false;
  }

  atomic_triple expected{4, 5, 6};

  if (!first.compare_exchange_strong(expected, atomic_triple{7, 8, 9})) {
    return false;
  }

  observed = second.load();

  if (observed.first != 7 || observed.second != 8 || observed.third != 9) {
    return false;
  }

  expected = atomic_triple{1, 2, 3};

  if (second.compare_exchange_strong(expected, atomic_triple{10, 11, 12})) {
    return false;
  }

  return expected.first == 7 && expected.second == 8 && expected.third == 9;
}

bool non_lock_free_free_functions_work() {
  tested::atomic<atomic_triple> value{atomic_triple{1, 2, 3}};

  tested::atomic_store(&value, atomic_triple{4, 5, 6});

  auto observed = tested::atomic_load(&value);

  if (observed.first != 4 || observed.second != 5 || observed.third != 6) {
    return false;
  }

  atomic_triple expected{4, 5, 6};

  if (!tested::atomic_compare_exchange_strong(&value, &expected,
                                              atomic_triple{7, 8, 9})) {
    return false;
  }

  observed = tested::atomic_load_explicit(&value, tested::memory_order_acquire);

  return observed.first == 7 && observed.second == 8 && observed.third == 9;
}

void poison_padding(padded_atomic_value &value, unsigned char byte) {
  auto *begin = reinterpret_cast<unsigned char *>(&value);

  auto *second = reinterpret_cast<unsigned char *>(&value.second);

  const tested::size_t second_offset =
      static_cast<tested::size_t>(second - begin);

  // Internal padding between first and second.
  for (tested::size_t index = sizeof(value.first); index < second_offset;
       ++index) {
    begin[index] = byte;
  }

  // Any tail padding after second.
  for (tested::size_t index = second_offset + sizeof(value.second);
       index < sizeof(value); ++index) {
    begin[index] = byte;
  }
}

#if defined(__clang__)
#if __has_builtin(__builtin_clear_padding)
#define FTL_TEST_ATOMIC_PADDING 1
#else
#define FTL_TEST_ATOMIC_PADDING 0
#endif
#else
#define FTL_TEST_ATOMIC_PADDING 1
#endif

bool atomic_padding_is_ignored() {
  padded_atomic_value stored{0x42, 0xc0defefe};

  padded_atomic_value expected{0x42, 0xc0defefe};

  poison_padding(stored, 0xaa);

  poison_padding(expected, 0x55);

  tested::atomic<padded_atomic_value> value{stored};

  return value.compare_exchange_strong(expected, padded_atomic_value{0, 0});
}

bool non_lock_free_atomic_init_works() {
  tested::atomic<atomic_triple> value{};

  volatile tested::atomic<atomic_triple> *volatile_value = &value;

  tested::atomic_init(volatile_value, atomic_triple{1, 2, 3});

  const auto observed = value.load();

  return observed.first == 1 && observed.second == 2 && observed.third == 3;
}

bool ftl_test() {
  return basic_operations_work() && compare_exchange_works() &&
         generic_atomic_works() && arithmetic_works() &&
         signed_wraparound_works() && bitwise_works() && floating_works() &&
         floating_compare_uses_representation() && pointer_works() &&
         void_pointer_basic_operations_work() && bool_works() &&
         volatile_works() && atomic_ref_basic_works() &&
         atomic_ref_copies_share_object() &&
         atomic_ref_compare_exchange_works() && atomic_ref_generic_works() &&
         atomic_ref_integer_works() && atomic_ref_signed_wraparound_works() &&
         atomic_ref_bitwise_works() && atomic_ref_floating_works() &&
         atomic_ref_floating_representation_works() &&
         atomic_ref_pointer_works() && atomic_ref_bool_works() &&
         atomic_nonmembers_work() && atomic_nonmember_specializations_work() &&
         atomic_nonmember_generic_works() && volatile_nonmembers_work() &&
         atomic_flag_works() && volatile_atomic_flag_works() &&
         deprecated_atomic_init_works() && fences_work() &&
         kill_dependency_works() && atomic_wait_immediate_return_works() &&
         atomic_ref_wait_immediate_return_works() &&
         atomic_flag_wait_immediate_return_works() &&
         platform_wait_backend_links() &&
         pointer_min_difference_does_not_overflow() &&
         non_lock_free_atomic_works() && non_lock_free_atomic_ref_works() &&
         non_lock_free_free_functions_work() &&
         non_lock_free_atomic_init_works()
#if FTL_TEST_ATOMIC_PADDING
         && atomic_padding_is_ignored()
#endif
      ;
}

#undef FTL_TEST_ATOMIC_PADDING