#ifdef FTL_REPLACE_STL
#include <generator>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <new>
#include <ranges>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/generator>
#include <ftl/iterator>
#include <ftl/memory>
#include <ftl/memory_resource>
#include <ftl/new>
#include <ftl/ranges>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

using value_generator =
    tested::generator<int>;

using reference_generator =
    tested::generator<int&>;

using const_reference_generator =
    tested::generator<const int&>;

using explicit_value_generator =
    tested::generator<int, int>;

static_assert(tested::is_same_v<
    typename value_generator::yielded,
    int&&
>);

static_assert(tested::is_same_v<
    typename reference_generator::yielded,
    int&
>);

static_assert(tested::is_same_v<
    typename const_reference_generator::yielded,
    const int&
>);

static_assert(tested::is_same_v<
    typename tested::generator<int&&>::yielded,
    int&&
>);

static_assert(tested::is_same_v<
    typename explicit_value_generator::yielded,
    const int&
>);

static_assert(
    !tested::is_default_constructible_v<
        value_generator
    >
);

static_assert(
    !tested::is_copy_constructible_v<
        value_generator
    >
);

static_assert(
    !tested::is_copy_assignable_v<
        value_generator
    >
);

static_assert(
    tested::is_nothrow_move_constructible_v<
        value_generator
    >
);

static_assert(
    tested::is_nothrow_move_assignable_v<
        value_generator
    >
);

static_assert(
    tested::ranges::view<
        value_generator
    >
);

static_assert(
    tested::ranges::input_range<
        value_generator
    >
);

static_assert(
    !tested::ranges::forward_range<
        value_generator
    >
);

static_assert(
    !tested::ranges::borrowed_range<
        value_generator
    >
);

static_assert(tested::is_same_v<
    tested::ranges::range_value_t<
        value_generator
    >,
    int
>);

static_assert(tested::is_same_v<
    tested::ranges::range_reference_t<
        value_generator
    >,
    int&&
>);

static_assert(tested::is_same_v<
    tested::ranges::range_reference_t<
        reference_generator
    >,
    int&
>);

using value_iterator =
    tested::ranges::iterator_t<
        value_generator
    >;

static_assert(
    tested::input_iterator<
        value_iterator
    >
);

static_assert(
    tested::sentinel_for<
        tested::default_sentinel_t,
        value_iterator
    >
);

static_assert(
    !tested::is_copy_constructible_v<
        value_iterator
    >
);

static_assert(
    tested::is_nothrow_move_constructible_v<
        value_iterator
    >
);

static_assert(tested::is_same_v<
    decltype(
        tested::declval<value_iterator&>()++
    ),
    void
>);

struct allocator_counts {
    tested::size_t allocations = 0;
    tested::size_t deallocations = 0;
    tested::size_t allocated_objects = 0;
    tested::size_t deallocated_objects = 0;
};

template<class T>
class counting_allocator {
public:
    using value_type = T;

    template<class>
    friend class counting_allocator;

    counting_allocator() = delete;

    explicit counting_allocator(
        allocator_counts& counts
    ) noexcept
        : counts_(
              tested::addressof(counts)
          ) {}

    template<class U>
    counting_allocator(
        const counting_allocator<U>& other
    ) noexcept
        : counts_(other.counts_) {}

    [[nodiscard]]
    T* allocate(tested::size_t count) {
        ++counts_->allocations;
        counts_->allocated_objects += count;

        return static_cast<T*>(
            ::operator new(
                count * sizeof(T)
            )
        );
    }

    void deallocate(
        T* pointer,
        tested::size_t count
    ) noexcept {
        ++counts_->deallocations;
        counts_->deallocated_objects += count;

        ::operator delete(pointer);
    }

    template<class U>
    friend bool operator==(
        const counting_allocator& left,
        const counting_allocator<U>& right
    ) noexcept {
        return left.counts_ == right.counts_;
    }

private:
    allocator_counts* counts_;
};

#ifdef FTL_REPLACE_STL

tested::generator<int> values() {
    /*
     * These are lvalues, so this tests the materializing
     * yield_value overload.
     */
    for (int value = 1; value <= 3; ++value) {
        co_yield value;
    }
}

tested::generator<int> direct_values() {
    /*
     * These test direct rvalue yielding.
     */
    co_yield 4;
    co_yield 5;
}

tested::generator<int> empty_values() {
    co_return;
}

tested::generator<int&> references(
    int& first,
    int& second
) {
    co_yield first;
    co_yield second;
}

tested::generator<const int&> const_references(
    const int& first,
    const int& second
) {
    co_yield first;
    co_yield second;
}

#if FTL_HAS_EXCEPTIONS

struct generator_test_exception {};

tested::generator<int> throwing_values() {
    co_yield 9;
    throw generator_test_exception{};
}

using byte_allocator =
    counting_allocator<
        unsigned char
    >;

tested::generator<
    int,
    void,
    byte_allocator
>
explicit_allocator_values(
    tested::allocator_arg_t,
    const byte_allocator& allocator
) {
    (void) allocator;

    co_yield 21;
    co_yield 22;
}

tested::generator<int>
erased_allocator_values(
    tested::allocator_arg_t,
    const byte_allocator& allocator
) {
    (void) allocator;

    co_yield 31;
    co_yield 32;
}

struct allocator_generator_owner {
    tested::generator<int>
    member_values(
        tested::allocator_arg_t,
        const byte_allocator& allocator
    ) const {
        (void) allocator;

        co_yield 41;
        co_yield 42;
    }
};

#endif

#endif

static_assert(tested::is_same_v<
    tested::pmr::generator<int>,
    tested::generator<
        int,
        void,
        tested::pmr::polymorphic_allocator<>
    >
>);

bool ftl_test() {
#ifdef FTL_REPLACE_STL
    {
        int expected = 1;

        for (int value : values()) {
            if (value != expected) {
                return false;
            }

            ++expected;
        }

        if (expected != 4) {
            return false;
        }
    }

    {
        auto sequence = empty_values();
        auto iterator = sequence.begin();

        if (iterator != sequence.end()) {
            return false;
        }
    }

    {
        int first = 10;
        int second = 20;

        for (int& value : references(first, second)) {
            value += 5;
        }

        if (first != 15 || second != 25) {
            return false;
        }
    }

    {
        const int first = 7;
        const int second = 11;
        int total = 0;

        for (
            const int& value :
            const_references(first, second)
        ) {
            total += value;
        }

        if (total != 18) {
            return false;
        }
    }

    {
        auto source = direct_values();
        auto iterator = source.begin();

        if (*iterator != 4) {
            return false;
        }

        /*
         * Existing iterators remain attached to the coroutine
         * after generator ownership moves.
         */
        auto destination =
            tested::move(source);

        ++iterator;

        if (
            iterator == destination.end() ||
            *iterator != 5
        ) {
            return false;
        }

        ++iterator;

        if (iterator != destination.end()) {
            return false;
        }
    }

    {
        allocator_counts counts;
        byte_allocator allocator{counts};

        {
            auto sequence =
                explicit_allocator_values(
                    tested::allocator_arg,
                    allocator
                );

            int expected = 21;

            for (int value : sequence) {
                if (value != expected) {
                    return false;
                }

                ++expected;
            }

            if (expected != 23) {
                return false;
            }

            if (
                counts.allocations != 1 ||
                counts.deallocations != 0
            ) {
                return false;
            }
        }

        if (
            counts.allocations != 1 ||
            counts.deallocations != 1 ||
            counts.allocated_objects !=
                counts.deallocated_objects
        ) {
            return false;
        }
    }

    {
        allocator_counts counts;
        byte_allocator allocator{counts};

        {
            auto sequence =
                erased_allocator_values(
                    tested::allocator_arg,
                    allocator
                );

            int total = 0;

            for (int value : sequence) {
                total += value;
            }

            if (total != 63) {
                return false;
            }
        }

        if (
            counts.allocations != 1 ||
            counts.deallocations != 1 ||
            counts.allocated_objects !=
                counts.deallocated_objects
        ) {
            return false;
        }
    }

    {
        allocator_counts counts;
        byte_allocator allocator{counts};
        allocator_generator_owner owner;

        {
            auto sequence =
                owner.member_values(
                    tested::allocator_arg,
                    allocator
                );

            int total = 0;

            for (int value : sequence) {
                total += value;
            }

            if (total != 83) {
                return false;
            }
        }

        if (
            counts.allocations != 1 ||
            counts.deallocations != 1 ||
            counts.allocated_objects !=
                counts.deallocated_objects
        ) {
            return false;
        }
    }

    {
        allocator_counts counts;
        byte_allocator allocator{counts};

        {
            auto source =
                erased_allocator_values(
                    tested::allocator_arg,
                    allocator
                );

            auto iterator = source.begin();

            if (*iterator != 31) {
                return false;
            }

            /*
             * Destroying a partially consumed generator must still
             * release its frame through the original allocator.
             */
        }

        if (
            counts.allocations != 1 ||
            counts.deallocations != 1 ||
            counts.allocated_objects !=
                counts.deallocated_objects
        ) {
            return false;
        }
    }

    {
        allocator_counts counts;
        byte_allocator allocator{counts};

            {
            auto sequence =
                erased_allocator_values(
                    tested::allocator_arg,
                    allocator
                );

            if (
                counts.allocations != 1 ||
                counts.deallocations != 0
            ) {
                return false;
            }

            // Deliberately never call begin().
            }

        if (
            counts.allocations != 1 ||
            counts.deallocations != 1 ||
            counts.allocated_objects !=
                counts.deallocated_objects
        ) {
            return false;
        }
    }

    {
        allocator_counts counts;
        byte_allocator allocator{counts};

            {
            auto source =
                erased_allocator_values(
                    tested::allocator_arg,
                    allocator
                );

            auto destination =
                tested::move(source);

            int total = 0;

            for (int value : destination) {
                total += value;
            }

            if (total != 63) {
                return false;
            }
            }

        if (
            counts.allocations != 1 ||
            counts.deallocations != 1 ||
            counts.allocated_objects !=
                counts.deallocated_objects
        ) {
            return false;
        }
    }

    {
        allocator_counts first_counts;
        allocator_counts second_counts;

        byte_allocator first_allocator{first_counts};
        byte_allocator second_allocator{second_counts};

            {
            auto first =
                erased_allocator_values(
                    tested::allocator_arg,
                    first_allocator
                );

            auto second =
                erased_allocator_values(
                    tested::allocator_arg,
                    second_allocator
                );

            second = tested::move(first);

            /*
             * Move assignment must destroy second's old frame immediately.
             */
            if (
                second_counts.allocations != 1 ||
                second_counts.deallocations != 1
            ) {
                return false;
            }

            int total = 0;

            for (int value : second) {
                total += value;
            }

            if (total != 63) {
                return false;
            }
            }

        if (
            first_counts.allocations != 1 ||
            first_counts.deallocations != 1 ||
            second_counts.allocations != 1 ||
            second_counts.deallocations != 1
        ) {
            return false;
        }
    }

#if FTL_HAS_EXCEPTIONS
    {
        bool caught = false;

        try {
            auto sequence = throwing_values();
            auto iterator = sequence.begin();

            if (*iterator != 9) {
                return false;
            }

            ++iterator;
        } catch (const generator_test_exception&) {
            caught = true;
        }

        if (!caught) {
            return false;
        }
    }
#endif
#endif

    return true;
}
