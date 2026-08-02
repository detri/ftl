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
    object* value{};
    constexpr explicit operator bool() const noexcept { return value; }
    constexpr object& operator*() const noexcept { return *value; }
    constexpr object* operator->() const noexcept { return value; }
    constexpr object& operator[](ftl::size_t index) const noexcept {
        return value[index];
    }
    friend constexpr bool operator==(
        fancy_pointer pointer, ftl::nullptr_t) noexcept {
        return pointer.value == nullptr;
    }
};
struct fancy_delete {
    using pointer = fancy_pointer;
    void operator()(pointer) const noexcept {}
};

struct counting_delete {
    int* count;
    void operator()(object* pointer) const noexcept {
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
    allocator_aware(ftl::allocator_arg_t, const allocator_type&, int input)
        : value(input) {}
};

void make_object(object** output) { *output = new object{31}; }
void replace_object(object** output) { delete *output; *output = new object{32}; }

static_assert(ftl::is_default_constructible_v<ftl::default_delete<object>>);
static_assert(ftl::is_constructible_v<
              ftl::default_delete<const object[]>,
              ftl::default_delete<object[]>>);
static_assert(ftl::is_constructible_v<
              ftl::unique_ptr<base>, ftl::unique_ptr<derived>&&>);
static_assert(ftl::is_assignable_v<
              ftl::unique_ptr<base>&, ftl::unique_ptr<derived>&&>);
static_assert(ftl::is_constructible_v<
              ftl::unique_ptr<const object[]>,
              ftl::unique_ptr<object[]>&&>);
static_assert(!ftl::is_constructible_v<
              ftl::unique_ptr<base[]>, ftl::unique_ptr<derived[]>&&>);
static_assert(sizeof(ftl::unique_ptr<object>) == sizeof(object*));
static_assert(sizeof(ftl::unique_ptr<object[]>) == sizeof(object*));
static_assert(ftl::is_same_v<
              ftl::unique_ptr<object, fancy_delete>::pointer,
              fancy_pointer>);
static_assert(ftl::is_constructible_v<
              ftl::unique_ptr<object, fancy_delete>, fancy_pointer>);
static_assert(ftl::is_constructible_v<
              ftl::unique_ptr<object[], fancy_delete>, fancy_pointer>);

bool lifetime_works() {
    union storage {
        unsigned char bytes[sizeof(object)];
        object value;
        storage() : bytes{} {}
        ~storage() {}
    } slot;

    auto* value = ftl::construct_at(&slot.value, 42);
    const bool result = value->value == 42;
    ftl::destroy_at(value);
    return result;
}

void algorithms_compile(object* source, object* destination) {
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
    return first.use_count() == 2 && second->value == 42 &&
           !weak.expired();
}

bool unique_ownership_works() {
    int deletes = 0;
    {
        ftl::unique_ptr<object, counting_delete> owner{
            new object{1}, counting_delete{&deletes}};
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
    const bool result =
        allocation.count >= 2 &&
        allocation.ptr[0].value == 11 &&
        allocation.ptr[1].value == 12;
    traits::destroy(allocator, allocation.ptr + 1);
    traits::destroy(allocator, allocation.ptr);
    traits::deallocate(allocator, allocation.ptr, allocation.count);
    return result;
}

bool allocate_shared_works() {
    auto owner = ftl::allocate_shared<enabled>(
        ftl::allocator<enabled>{}, 17);
    auto self = owner->shared_from_this();
    return self.get() == owner.get() &&
           self->value == 17 &&
           owner.use_count() == 2 &&
           !owner->weak_from_this().expired();
}

static_assert(ftl::is_constructible_v<
              ftl::shared_ptr<base>, ftl::shared_ptr<derived>>);
static_assert(ftl::is_constructible_v<
              ftl::weak_ptr<base>, ftl::shared_ptr<derived>>);

static_assert(requires(object* location) {
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
    auto aware = ftl::make_obj_using_allocator<allocator_aware>(
        ftl::allocator<int>{}, 19);
    return array[1].value == 8 && down.get() == owner.get() &&
           loaded.get() == owner.get() && output->value == 32 &&
           aware.value == 19;
}

bool ftl_test() {
    return lifetime_works() &&
           shared_ownership_works() &&
           unique_ownership_works() &&
           weak_lifetime_works() &&
           allocator_works() &&
           allocate_shared_works() &&
           extended_memory_works();
}
