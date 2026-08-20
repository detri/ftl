#ifdef FTL_REPLACE_STL
#include <memory>
namespace tested = std;
#else
#include <ftl/memory>
namespace tested = ftl;
#endif
#define ftl tested

struct object {
  int value;
  constexpr explicit object(int input) : value(input) {}
};
struct base {};
struct derived : base {};
struct fancy_pointer {
  object *value{};
  constexpr explicit operator bool() const noexcept { return value; }
  constexpr object &operator*() const noexcept { return *value; }
  constexpr object *operator->() const noexcept { return value; }
  constexpr object &operator[](ftl::size_t index) const noexcept {
    return value[index];
  }
  friend constexpr bool operator==(fancy_pointer pointer,
                                   ftl::nullptr_t) noexcept {
    return pointer.value == nullptr;
  }
};
struct fancy_delete {
  using pointer = fancy_pointer;
  void operator()(pointer) const noexcept {}
};

struct counting_delete {
  int *count;
  void operator()(object *pointer) const noexcept {
    ++*count;
    delete pointer;
  }
};
struct enabled : ftl::enable_shared_from_this<enabled> {
  int value;
  explicit enabled(int input) : value(input) {}
};
struct allocator_aware {
  using allocator_type = ftl::allocator<int>;
  int value;
  allocator_aware(ftl::allocator_arg_t, const allocator_type &, int input)
      : value(input) {}
};
struct atomic_alias_owner {
  object first{1};
  object second{2};
};

struct allocation_counts {
  int allocations{};
  int deallocations{};
  int constructions{};
  int destructions{};
  alignas(256) unsigned char storage[8192]{};
  ftl::size_t used{};
};

template <class T, class Observed = T> struct observing_allocator {
  using value_type = T;
  template <class U> struct rebind {
    using other = observing_allocator<U, Observed>;
  };
  allocation_counts *counts{};

  observing_allocator() = default;
  explicit observing_allocator(allocation_counts &value) : counts(&value) {}
  template <class U>
  observing_allocator(const observing_allocator<U, Observed> &other)
      : counts(other.counts) {}

  T *allocate(ftl::size_t count) {
    ++counts->allocations;
    const auto base = reinterpret_cast<ftl::uintptr_t>(counts->storage);
    const auto current = base + counts->used;
    const auto aligned = (current + alignof(T) - 1) & ~(alignof(T) - 1);
    const ftl::size_t next =
        static_cast<ftl::size_t>(aligned - base) + sizeof(T) * count;
    if (next > sizeof(counts->storage))
      throw ftl::bad_alloc{};
    counts->used = next;
    return reinterpret_cast<T *>(aligned);
  }
  void deallocate(T *pointer, ftl::size_t) noexcept {
    (void)pointer;
    ++counts->deallocations;
  }
  template <class U, class... Args>
  void construct(U *pointer, Args &&...args) {
    if constexpr (ftl::is_same_v<ftl::remove_cv_t<U>, Observed>)
      ++counts->constructions;
    ::new (static_cast<void *>(pointer)) U(
        ftl::forward<Args>(args)...);
  }
  template <class U> void destroy(U *pointer) noexcept {
    if constexpr (ftl::is_same_v<ftl::remove_cv_t<U>, Observed>)
      ++counts->destructions;
    pointer->~U();
  }

  template <class, class> friend struct observing_allocator;
  template <class U>
  friend bool operator==(const observing_allocator &left,
                         const observing_allocator<U, Observed> &right) noexcept {
    return left.counts == right.counts;
  }
};

struct alignas(128) over_aligned_value {
  int value{};
};

void make_object(object **output) { *output = new object{31}; }
void replace_object(object **output) {
  delete *output;
  *output = new object{32};
}

static_assert(ftl::is_default_constructible_v<ftl::default_delete<object>>);
static_assert(ftl::is_constructible_v<ftl::default_delete<const object[]>,
                                      ftl::default_delete<object[]>>);
static_assert(ftl::is_constructible_v<ftl::unique_ptr<base>,
                                      ftl::unique_ptr<derived> &&>);
static_assert(
    ftl::is_assignable_v<ftl::unique_ptr<base> &, ftl::unique_ptr<derived> &&>);
static_assert(ftl::is_constructible_v<ftl::unique_ptr<const object[]>,
                                      ftl::unique_ptr<object[]> &&>);
static_assert(!ftl::is_constructible_v<ftl::unique_ptr<base[]>,
                                       ftl::unique_ptr<derived[]> &&>);
static_assert(sizeof(ftl::unique_ptr<object>) == sizeof(object *));
static_assert(sizeof(ftl::unique_ptr<object[]>) == sizeof(object *));
static_assert(ftl::is_same_v<ftl::unique_ptr<object, fancy_delete>::pointer,
                             fancy_pointer>);
static_assert(ftl::is_constructible_v<ftl::unique_ptr<object, fancy_delete>,
                                      fancy_pointer>);
static_assert(ftl::is_constructible_v<ftl::unique_ptr<object[], fancy_delete>,
                                      fancy_pointer>);

static_assert(ftl::is_constructible_v<ftl::unique_ptr<object>,
                                      ftl::unique_ptr<object> &&>);

static_assert(ftl::is_assignable_v<ftl::unique_ptr<object> &,
                                   ftl::unique_ptr<object> &&>);

static_assert(ftl::is_constructible_v<
              ftl::shared_ptr<object[]>, object *,
              ftl::default_delete<object[]>>);

static_assert(ftl::is_constructible_v<ftl::shared_ptr<const object[]>,
                                      ftl::shared_ptr<object[]>>);

static_assert(!ftl::is_constructible_v<ftl::shared_ptr<base[]>,
                                       ftl::shared_ptr<derived[]>>);

bool lifetime_works() {
  union storage {
    unsigned char bytes[sizeof(object)];
    object value;
    storage() : bytes{} {}
    ~storage() {}
  } slot;

  auto *value = ftl::construct_at(&slot.value, 42);
  const bool result = value->value == 42;
  ftl::destroy_at(value);
  return result;
}

void algorithms_compile(object *source, object *destination) {
  auto end = ftl::uninitialized_copy_n(source, 1, destination);
  ftl::destroy(destination, end);

  auto result = ftl::uninitialized_move_n(source, 1, destination);
  ftl::destroy(destination, result.second);

  ftl::uninitialized_fill_n(destination, 1, object{7});
  ftl::destroy_n(destination, 1);
}

bool shared_ownership_works() {
  auto first = ftl::make_shared<object>(42);
  ftl::weak_ptr<object> weak = first;
  auto second = weak.lock();
  return first.use_count() == 2 && second->value == 42 && !weak.expired();
}

bool unique_ownership_works() {
  int deletes = 0;
  {
    ftl::unique_ptr<object, counting_delete> owner{new object{1},
                                                   counting_delete{&deletes}};
    owner.reset(new object{2});
    if (deletes != 1 || owner->value != 2)
      return false;
    auto moved = ftl::move(owner);
    if (owner || !moved || moved->value != 2)
      return false;
  }
  return deletes == 2;
}

bool weak_lifetime_works() {
  ftl::weak_ptr<object> weak;
  {
    auto owner = ftl::make_shared<object>(9);
    weak = ftl::weak_ptr<object>{owner};
    if (weak.expired() || weak.lock()->value != 9)
      return false;
  }
  return weak.expired() && !weak.lock();
}

bool allocator_works() {
  ftl::allocator<object> allocator;
  using traits = ftl::allocator_traits<decltype(allocator)>;
  auto allocation = traits::allocate_at_least(allocator, 2);
  traits::construct(allocator, allocation.ptr, 11);
  traits::construct(allocator, allocation.ptr + 1, 12);
  const bool result = allocation.count >= 2 && allocation.ptr[0].value == 11 &&
                      allocation.ptr[1].value == 12;
  traits::destroy(allocator, allocation.ptr + 1);
  traits::destroy(allocator, allocation.ptr);
  traits::deallocate(allocator, allocation.ptr, allocation.count);
  return result;
}

bool allocate_shared_works() {
  auto owner = ftl::allocate_shared<enabled>(ftl::allocator<enabled>{}, 17);
  auto self = owner->shared_from_this();
  return self.get() == owner.get() && self->value == 17 &&
         owner.use_count() == 2 && !owner->weak_from_this().expired();
}

static_assert(
    ftl::is_constructible_v<ftl::shared_ptr<base>, ftl::shared_ptr<derived>>);
static_assert(
    ftl::is_constructible_v<ftl::weak_ptr<base>, ftl::shared_ptr<derived>>);

static_assert(requires(object *location) {
  ftl::construct_at(location, 42);
  ftl::destroy_at(location);
});

static_assert(ftl::uses_allocator_v<allocator_aware, ftl::allocator<int>>);
static_assert(ftl::is_same_v<decltype(ftl::make_unique<object>(1)),
                             ftl::unique_ptr<object>>);
static_assert(ftl::is_same_v<decltype(ftl::make_shared<object[]>(2)),
                             ftl::shared_ptr<object[]>>);

bool extended_memory_works() {
  auto array = ftl::make_shared<object[]>(2, object{8});
  auto owner = ftl::make_shared<derived>();
  ftl::shared_ptr<base> base_owner = owner;
  auto down = ftl::static_pointer_cast<derived>(base_owner);
  ftl::atomic<ftl::shared_ptr<derived>> atomic_owner{owner};
  auto loaded = atomic_owner.load();
  ftl::unique_ptr<object> output;
  make_object(ftl::out_ptr(output));
  replace_object(ftl::inout_ptr(output));
  auto aware =
      ftl::make_obj_using_allocator<allocator_aware>(ftl::allocator<int>{}, 19);
  return array[1].value == 8 && down.get() == owner.get() &&
         loaded.get() == owner.get() && output->value == 32 &&
         aware.value == 19;
}

bool allocator_backed_shared_arrays_work() {
  allocation_counts counts;
  observing_allocator<int> allocator{counts};
  int row[2] = {4, 9};
  {
    auto matrix = ftl::allocate_shared<int[][2]>(allocator, 3, row);
    if (matrix[0][0] != 4 || matrix[1][1] != 9 || matrix[2][0] != 4 ||
        counts.constructions < 6)
      return false;
  }
  if (counts.destructions != 6 || counts.constructions != 6 ||
      counts.allocations != counts.deallocations)
    return false;

  allocation_counts empty_counts;
  {
    auto empty = ftl::allocate_shared<int[]>(
        observing_allocator<int>{empty_counts}, 0);
    if (empty.get() == nullptr)
      return false;
  }
  if (empty_counts.allocations != empty_counts.deallocations)
    return false;

  allocation_counts aligned_counts;
  {
    auto values = ftl::allocate_shared<over_aligned_value[]>(
        observing_allocator<over_aligned_value>{aligned_counts}, 2);
    if (reinterpret_cast<ftl::uintptr_t>(values.get()) %
            alignof(over_aligned_value) !=
        0)
      return false;
  }
  return aligned_counts.constructions == 2 &&
         aligned_counts.destructions == 2 &&
         aligned_counts.allocations == aligned_counts.deallocations;
}

template <class Pointer,
          bool = tested::is_void_v<
              typename tested::pointer_traits<Pointer>::element_type>>
struct has_pointer_to_helper;

template <class Pointer> struct has_pointer_to_helper<Pointer, true> {
  static constexpr bool value = false;
};

template <class Pointer> struct has_pointer_to_helper<Pointer, false> {
private:
  using element_type = typename tested::pointer_traits<Pointer>::element_type;

public:
  static constexpr bool value = requires(element_type &object) {
    tested::pointer_traits<Pointer>::pointer_to(object);
  };
};

template <class Pointer>
inline constexpr bool has_pointer_to_v = has_pointer_to_helper<Pointer>::value;

static_assert(has_pointer_to_v<int *>);
static_assert(has_pointer_to_v<const int *>);

static_assert(!has_pointer_to_v<void *>);
static_assert(!has_pointer_to_v<const void *>);
static_assert(!has_pointer_to_v<volatile void *>);
static_assert(!has_pointer_to_v<const volatile void *>);

bool atomic_shared_ptr_ownership_equivalence_works() {
  object storage{42};

  auto no_delete = [](object *) noexcept {};

  ftl::shared_ptr<object> first{&storage, no_delete};

  ftl::shared_ptr<object> different_owner{&storage, no_delete};

  ftl::atomic<ftl::shared_ptr<object>> value{first};

  auto expected = different_owner;

  auto desired = ftl::make_shared<object>(99);

  // Same raw pointer, different ownership:
  // these are NOT equivalent.
  if (value.compare_exchange_strong(expected, desired)) {
    return false;
  }

  if (expected.get() != first.get())
    return false;

  return !expected.owner_before(first) && !first.owner_before(expected);
}

bool atomic_weak_ptr_stored_pointer_equivalence_works() {
  auto owner = ftl::make_shared<atomic_alias_owner>();

  ftl::shared_ptr<object> first{owner, &owner->first};

  ftl::shared_ptr<object> second{owner, &owner->second};

  ftl::weak_ptr<object> weak_first{first};
  ftl::weak_ptr<object> weak_second{second};

  ftl::atomic<ftl::weak_ptr<object>> value{weak_first};

  auto expected = weak_second;

  // Same ownership, different stored pointer:
  // these are also NOT equivalent.
  if (value.compare_exchange_strong(expected, ftl::weak_ptr<object>{})) {
    return false;
  }

  auto observed = expected.lock();

  return observed.get() == &owner->first;
}

bool atomic_smart_pointer_assignment_works() {
  auto first = ftl::make_shared<object>(1);

  auto second = ftl::make_shared<object>(2);

  ftl::atomic<ftl::shared_ptr<object>> value{first};

  value = second;

  auto loaded = value.load();

  return loaded.get() == second.get() && loaded->value == 2;
}

bool shared_array_deleter_works() {
  int deletes = 0;

  struct deleter {
    int *deletes;

    void operator()(int *pointer) const noexcept {
      ++*deletes;
      delete[] pointer;
    }
  };

  {
    ftl::shared_ptr<int[]> owner{new int[3]{1, 2, 3}, deleter{&deletes}};

    if (owner[0] != 1 || owner[1] != 2 || owner[2] != 3)
      return false;
  }

  return deletes == 1;
}

bool unique_same_type_move_assignment_works() {
  auto first = ftl::make_unique<object>(11);
  auto second = ftl::make_unique<object>(22);

  first = ftl::move(second);

  return first && !second && first->value == 22;
}

bool ftl_test() {
  return lifetime_works() && shared_ownership_works() &&
         unique_ownership_works() && weak_lifetime_works() &&
         allocator_works() && allocate_shared_works() &&
         extended_memory_works() &&
         allocator_backed_shared_arrays_work() &&
         atomic_shared_ptr_ownership_equivalence_works() &&
         atomic_weak_ptr_stored_pointer_equivalence_works() &&
         atomic_smart_pointer_assignment_works() &&
         shared_array_deleter_works() &&
         unique_same_type_move_assignment_works();
}
