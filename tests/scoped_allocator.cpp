#ifdef FTL_REPLACE_STL
#include <scoped_allocator>
namespace tested = std;
#else
#include <ftl/scoped_allocator>
namespace tested = ftl;
#endif

using test_size_t = decltype(sizeof(0));

template <class T> T *allocate_items(test_size_t count) {
  tested::allocator<T> alloc;
  return alloc.allocate(count);
}

template <class T>
void deallocate_items(T *pointer, test_size_t count) noexcept {
  tested::allocator<T> alloc;
  alloc.deallocate(pointer, count);
}

template <class T, int Tag> struct tagged_allocator {
  using value_type = T;

  template <class U> struct rebind {
    using other = tagged_allocator<U, Tag>;
  };

  tagged_allocator() = default;

  template <class U>
  tagged_allocator(const tagged_allocator<U, Tag> &) noexcept {}

  T *allocate(test_size_t count) { return allocate_items<T>(count); }

  void deallocate(T *pointer, test_size_t count) noexcept {
    deallocate_items(pointer, count);
  }

  template <class U, int OtherTag>
  friend constexpr bool
  operator==(const tagged_allocator &,
             const tagged_allocator<U, OtherTag> &) noexcept {
    return Tag == OtherTag;
  }
};

template <class T, bool CopyPropagate, bool MovePropagate, bool SwapPropagate,
          bool AlwaysEqual>
struct flag_allocator {
  using value_type = T;

  using propagate_on_container_copy_assignment =
      tested::bool_constant<CopyPropagate>;
  using propagate_on_container_move_assignment =
      tested::bool_constant<MovePropagate>;
  using propagate_on_container_swap = tested::bool_constant<SwapPropagate>;
  using is_always_equal = tested::bool_constant<AlwaysEqual>;

  template <class U> struct rebind {
    using other = flag_allocator<U, CopyPropagate, MovePropagate, SwapPropagate,
                                 AlwaysEqual>;
  };

  flag_allocator() = default;

  template <class U>
  flag_allocator(const flag_allocator<U, CopyPropagate, MovePropagate,
                                      SwapPropagate, AlwaysEqual> &) noexcept {}

  T *allocate(test_size_t count) { return allocate_items<T>(count); }

  void deallocate(T *pointer, test_size_t count) noexcept {
    deallocate_items(pointer, count);
  }

  friend constexpr bool operator==(const flag_allocator &,
                                   const flag_allocator &) noexcept {
    return true;
  }
};

struct accepted_outer_arg {};
struct rejected_outer_arg {};

template <class T> struct selective_allocator {
  using value_type = T;

  template <class U> struct rebind {
    using other = selective_allocator<U>;
  };

  selective_allocator() = default;

  template <class U>
  selective_allocator(const selective_allocator<U> &) noexcept {}

  explicit selective_allocator(accepted_outer_arg) noexcept {}

  template <class U>
  selective_allocator(const tagged_allocator<U, 1> &) noexcept {}

  template <class U> selective_allocator(tagged_allocator<U, 1> &&) noexcept {}

  T *allocate(test_size_t count) { return allocate_items<T>(count); }

  void deallocate(T *pointer, test_size_t count) noexcept {
    deallocate_items(pointer, count);
  }

  friend constexpr bool operator==(const selective_allocator &,
                                   const selective_allocator &) noexcept {
    return true;
  }
};

template <class T> struct throwing_copy_allocator {
  using value_type = T;

  template <class U> struct rebind {
    using other = throwing_copy_allocator<U>;
  };

  throwing_copy_allocator() = default;

  throwing_copy_allocator(const throwing_copy_allocator &) noexcept(false) {}

  throwing_copy_allocator(throwing_copy_allocator &&) noexcept(false) {}

  T *allocate(test_size_t count) { return allocate_items<T>(count); }

  void deallocate(T *pointer, test_size_t count) noexcept {
    deallocate_items(pointer, count);
  }

  friend constexpr bool operator==(const throwing_copy_allocator &,
                                   const throwing_copy_allocator &) noexcept {
    return true;
  }
};

inline int construct_calls = 0;
inline int destroy_calls = 0;

template <class T> struct hook_allocator {
  using value_type = T;

  template <class U> struct rebind {
    using other = hook_allocator<U>;
  };

  hook_allocator() = default;

  template <class U> hook_allocator(const hook_allocator<U> &) noexcept {}

  T *allocate(test_size_t count) { return allocate_items<T>(count); }

  void deallocate(T *pointer, test_size_t count) noexcept {
    deallocate_items(pointer, count);
  }

  template <class U, class... Args> void construct(U *pointer, Args &&...args) {
    ++construct_calls;
    tested::construct_at(pointer, tested::forward<Args>(args)...);
  }

  template <class U> void destroy(U *pointer) {
    ++destroy_calls;
    tested::destroy_at(pointer);
  }

  friend constexpr bool operator==(const hook_allocator &,
                                   const hook_allocator &) noexcept {
    return true;
  }
};

template <class T> struct wrapper_allocator {
  using value_type = T;

  template <class U> struct rebind {
    using other = wrapper_allocator<U>;
  };

  hook_allocator<T> leaf;

  wrapper_allocator() = default;

  template <class U>
  wrapper_allocator(const wrapper_allocator<U> &other) noexcept
      : leaf(other.leaf) {}

  T *allocate(test_size_t count) { return leaf.allocate(count); }

  void deallocate(T *pointer, test_size_t count) noexcept {
    leaf.deallocate(pointer, count);
  }

  hook_allocator<T> &outer_allocator() noexcept { return leaf; }

  const hook_allocator<T> &outer_allocator() const noexcept { return leaf; }

  friend constexpr bool operator==(const wrapper_allocator &,
                                   const wrapper_allocator &) noexcept {
    return true;
  }
};

struct aware {
  using allocator_type = tested::allocator<int>;

  int value;

  aware(tested::allocator_arg_t, const allocator_type &, int v) : value(v) {}
};

static_assert(tested::uses_allocator_v<aware, tested::allocator<int>>);

using adaptor = tested::scoped_allocator_adaptor<tested::allocator<aware>,
                                                 tested::allocator<int>>;

static_assert(tested::is_same_v<typename adaptor::value_type, aware>);

static_assert(tested::is_same_v<
              typename adaptor::inner_allocator_type,
              tested::scoped_allocator_adaptor<tested::allocator<int>>>);

// Propagation is OR across all allocators.
// is_always_equal is AND across all allocators.
using propagation_adaptor = tested::scoped_allocator_adaptor<
    flag_allocator<aware, false, false, false, true>,
    flag_allocator<int, true, true, true, false>>;

static_assert(
    propagation_adaptor::propagate_on_container_copy_assignment::value);

static_assert(
    propagation_adaptor::propagate_on_container_move_assignment::value);

static_assert(propagation_adaptor::propagate_on_container_swap::value);

static_assert(!propagation_adaptor::is_always_equal::value);

using all_equal_adaptor = tested::scoped_allocator_adaptor<
    flag_allocator<aware, false, false, false, true>,
    flag_allocator<int, false, false, false, true>>;

static_assert(all_equal_adaptor::is_always_equal::value);

// The forwarding outer-allocator constructor participates only
// when OuterAlloc is constructible from the supplied argument.
using selective_single =
    tested::scoped_allocator_adaptor<selective_allocator<int>>;

static_assert(tested::is_constructible_v<selective_single, accepted_outer_arg>);

static_assert(
    !tested::is_constructible_v<selective_single, rejected_outer_arg>);

using selective_multi =
    tested::scoped_allocator_adaptor<selective_allocator<aware>,
                                     tested::allocator<int>>;

static_assert(tested::is_constructible_v<selective_multi, accepted_outer_arg,
                                         tested::allocator<int>>);

static_assert(!tested::is_constructible_v<selective_multi, rejected_outer_arg,
                                          tested::allocator<int>>);

// Converting adaptor constructors have the corresponding
// OuterAlloc constructibility constraints.
using source_single =
    tested::scoped_allocator_adaptor<tagged_allocator<int, 1>>;

using rejected_single =
    tested::scoped_allocator_adaptor<tagged_allocator<int, 2>>;

static_assert(
    tested::is_constructible_v<selective_single, const source_single &>);

static_assert(tested::is_constructible_v<selective_single, source_single &&>);

static_assert(
    !tested::is_constructible_v<selective_single, const rejected_single &>);

static_assert(
    !tested::is_constructible_v<selective_single, rejected_single &&>);

using source_multi =
    tested::scoped_allocator_adaptor<tagged_allocator<aware, 1>,
                                     tested::allocator<int>>;

using rejected_multi =
    tested::scoped_allocator_adaptor<tagged_allocator<aware, 2>,
                                     tested::allocator<int>>;

static_assert(
    tested::is_constructible_v<selective_multi, const source_multi &>);

static_assert(tested::is_constructible_v<selective_multi, source_multi &&>);

static_assert(
    !tested::is_constructible_v<selective_multi, const rejected_multi &>);

static_assert(!tested::is_constructible_v<selective_multi, rejected_multi &&>);

// The synopsis declares both same-type constructors noexcept,
// regardless of the underlying allocator's exception specification.
using throwing_adaptor =
    tested::scoped_allocator_adaptor<throwing_copy_allocator<int>>;

static_assert(
    noexcept(throwing_adaptor(tested::declval<const throwing_adaptor &>())));

static_assert(
    noexcept(throwing_adaptor(tested::declval<throwing_adaptor &&>())));

// deallocate is explicitly noexcept.
static_assert(noexcept(tested::declval<adaptor &>().deallocate(
    tested::declval<typename adaptor::pointer>(), 1)));

// operator== exists only when both adaptors have the same
// InnerAllocs... pack.
template <class Left, class Right>
concept has_exact_scoped_equality = requires {
  static_cast<bool (*)(const Left &, const Right &) noexcept>(
      &tested::operator==);
};

using equality_outer = tagged_allocator<int, 10>;
using equality_inner_a = tagged_allocator<int, 11>;
using equality_inner_b = tagged_allocator<int, 12>;

using equality_a =
    tested::scoped_allocator_adaptor<equality_outer, equality_inner_a>;

using equality_b =
    tested::scoped_allocator_adaptor<equality_outer, equality_inner_b>;

static_assert(has_exact_scoped_equality<equality_a, equality_a>);

static_assert(!has_exact_scoped_equality<equality_a, equality_b>);

// Force OUTERMOST recursion:
// scoped_allocator -> wrapper_allocator -> hook_allocator.
//
// construct/destroy must reach hook_allocator's customization
// points rather than falling back directly to construct_at/destroy_at.
using hook_adaptor = tested::scoped_allocator_adaptor<wrapper_allocator<aware>,
                                                      tested::allocator<int>>;

bool ftl_test() {
  construct_calls = 0;
  destroy_calls = 0;

  hook_adaptor alloc;

  auto *pointer = alloc.allocate(1);

  alloc.construct(pointer, 42);

  bool result =
      pointer->value == 42 && construct_calls == 1 && destroy_calls == 0;

  alloc.destroy(pointer);

  result = result && construct_calls == 1 && destroy_calls == 1;

  alloc.deallocate(pointer, 1);

  return result;
}
