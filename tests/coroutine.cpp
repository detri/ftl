#ifdef FTL_REPLACE_STL
#include <coroutine>
#include <exception>
#include <functional>
#include <type_traits>
#include <utility>
namespace tested = std;
#else
#include <ftl/coroutine>
#include <ftl/exception>
#include <ftl/functional>
#include <ftl/type_traits>
#include <ftl/utility>
namespace tested = ftl;
#endif

#ifdef FTL_REPLACE_STL

struct task {
    struct promise_type {
        int result = 0;

        task get_return_object() noexcept {
            return task{
                tested::coroutine_handle<promise_type>::from_promise(*this)
            };
        }

        tested::suspend_always initial_suspend() const noexcept {
            return {};
        }

        tested::suspend_always final_suspend() const noexcept {
            return {};
        }

        void return_value(int value) noexcept {
            result = value;
        }

        void unhandled_exception() noexcept {
            tested::terminate();
        }
    };

    using handle_type = tested::coroutine_handle<promise_type>;

    explicit task(handle_type value) noexcept
        : handle(value) {}

    task(const task&) = delete;
    task& operator=(const task&) = delete;

    task(task&& other) noexcept
        : handle(tested::exchange(other.handle, nullptr)) {}

    task& operator=(task&& other) noexcept {
        if (this != &other) {
            if (handle)
                handle.destroy();

            handle = tested::exchange(other.handle, nullptr);
        }

        return *this;
    }

    ~task() {
        if (handle)
            handle.destroy();
    }

    int run() {
        handle.resume();
        return handle.promise().result;
    }

    handle_type handle;
};

task make_task(int value) {
    co_return value + 1;
}

#endif

struct test_promise {
    int value = 0;
};

struct traits_return {
    using promise_type = test_promise;
};

static_assert(tested::is_same_v<
    typename tested::coroutine_traits<traits_return, int>::promise_type,
    traits_return::promise_type>);

static_assert(tested::is_default_constructible_v<
    tested::coroutine_handle<>>);

#if FTL_REPLACE_STL
static_assert(tested::is_default_constructible_v<
    tested::coroutine_handle<task::promise_type>>);

static_assert(tested::is_convertible_v<
    tested::coroutine_handle<task::promise_type>,
    tested::coroutine_handle<>>);
#endif

static_assert(tested::is_same_v<
    tested::noop_coroutine_handle,
    tested::coroutine_handle<tested::noop_coroutine_promise>>);

static_assert(tested::suspend_never{}.await_ready());
static_assert(!tested::suspend_always{}.await_ready());

constexpr bool null_handle_works() {
    tested::coroutine_handle<> first;
    tested::coroutine_handle<> second = nullptr;

    if (first || second)
        return false;

    if (first.address() != nullptr || second.address() != nullptr)
        return false;

    if (!(first == second))
        return false;

    first = nullptr;
    return !first;
}

static_assert(null_handle_works());

struct no_promise_type {};

template<class T>
concept has_promise_type =
    requires { typename T::promise_type; };

static_assert(!has_promise_type<
    tested::coroutine_traits<no_promise_type>
>);

bool ftl_test() {
    tested::coroutine_handle<> first;
    tested::coroutine_handle<> second = nullptr;

    if (first || second)
        return false;

    if ((first <=> second) != tested::strong_ordering::equal)
        return false;

    if (first.address() != nullptr || second.address() != nullptr)
        return false;

    if (!(first == second))
        return false;

    auto noop = tested::noop_coroutine();

    if (&noop.promise() != &noop.promise())
        return false;

    if (!noop || noop.done() || noop.address() == nullptr)
        return false;

    auto erased_noop =
        static_cast<tested::coroutine_handle<>>(noop);

    if (erased_noop.address() != noop.address())
        return false;

    if (erased_noop != static_cast<tested::coroutine_handle<>>(noop))
        return false;

    noop.resume();
    noop();
    noop.destroy();

    tested::suspend_never never;
    tested::suspend_always always;

    if (!never.await_ready() || always.await_ready())
        return false;

#ifdef FTL_REPLACE_STL
    auto operation = make_task(41);

    if (!operation.handle || operation.handle.done())
        return false;

    auto erased =
        static_cast<tested::coroutine_handle<>>(operation.handle);

    if (erased.address() != operation.handle.address())
        return false;

    auto reconstructed =
        task::handle_type::from_address(operation.handle.address());

    if (&reconstructed.promise() != &operation.handle.promise())
        return false;

    if (operation.run() != 42)
        return false;

    if (!operation.handle.done())
        return false;

    tested::hash<task::handle_type> hasher;

    if (hasher(operation.handle) != hasher(reconstructed))
        return false;

    reconstructed = nullptr;

    if (reconstructed || reconstructed.address() != nullptr)
        return false;
#endif

    return true;
}
