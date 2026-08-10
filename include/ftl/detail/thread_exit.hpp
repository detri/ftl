// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_THREAD_EXIT_HEADER
#define FTL_DETAIL_THREAD_EXIT_HEADER

#ifdef FTL_REPLACE_STL
#include <atomic>
#include <cstdint>
#define FTL_THREAD_EXIT_BEGIN_NAMESPACE namespace std::detail {
#define FTL_THREAD_EXIT_END_NAMESPACE }
#else
#include <ftl/atomic>
#include <ftl/cstdint>
#define FTL_THREAD_EXIT_BEGIN_NAMESPACE namespace ftl::detail {
#define FTL_THREAD_EXIT_END_NAMESPACE }
#endif

FTL_THREAD_EXIT_BEGIN_NAMESPACE

struct thread_exit_action {
  using invoke_type = void (*)(thread_exit_action *) noexcept;

  explicit thread_exit_action(invoke_type invoke) noexcept : invoke_(invoke) {}

  invoke_type invoke_;
  thread_exit_action *next_ = nullptr;

#if defined(__linux__) || defined(__APPLE__)
  // pthread TSD destructors have unspecified ordering.
  // Defer FTL's actions through one complete destructor
  // pass so language TLS destruction can finish first.
  bool exit_rearmed_ = false;
#endif
};

inline void run_thread_exit_actions(thread_exit_action *action) noexcept {
  while (action != nullptr) {
    thread_exit_action *next = action->next_;

    action->invoke_(action);

    action = next;
  }
}

#if defined(_WIN32)

// Use ordinary TLS storage for the per-thread action
// list. Cleanup is driven separately by a late PE TLS
// callback rather than by FLS callback ordering.
extern "C" __declspec(dllimport) unsigned long __stdcall TlsAlloc();

extern "C" __declspec(dllimport) void *__stdcall TlsGetValue(unsigned long);

extern "C" __declspec(dllimport) int __stdcall TlsSetValue(unsigned long,
                                                           void *);

inline bool native_thread_exit_key_create(uintptr_t &result) noexcept {
  constexpr unsigned long out_of_indexes = 0xffffffffUL;

  const unsigned long key = TlsAlloc();

  if (key == out_of_indexes)
    return false;

  result = static_cast<uintptr_t>(key);

  return true;
}

inline void *native_thread_exit_get(uintptr_t key) noexcept {
  return TlsGetValue(static_cast<unsigned long>(key));
}

inline bool native_thread_exit_set(uintptr_t key, void *value) noexcept {
  return TlsSetValue(static_cast<unsigned long>(key), value) != 0;
}

#elif defined(__linux__) || defined(__APPLE__)

#if defined(__APPLE__)
using posix_thread_exit_key = unsigned long;
#else
using posix_thread_exit_key = unsigned int;
#endif

inline void thread_exit_dispatch(void *value) noexcept;

extern "C" int pthread_key_create(posix_thread_exit_key *, void (*)(void *));

extern "C" void *pthread_getspecific(posix_thread_exit_key);

extern "C" int pthread_setspecific(posix_thread_exit_key, const void *);

inline bool native_thread_exit_key_create(uintptr_t &result) noexcept {
  posix_thread_exit_key key{};

  if (pthread_key_create(&key, &thread_exit_dispatch) != 0) {
    return false;
  }

  result = static_cast<uintptr_t>(key);

  return true;
}

inline void *native_thread_exit_get(uintptr_t key) noexcept {
  return pthread_getspecific(static_cast<posix_thread_exit_key>(key));
}

inline bool native_thread_exit_set(uintptr_t key, void *value) noexcept {
  return pthread_setspecific(static_cast<posix_thread_exit_key>(key), value) ==
         0;
}

#else

#error FTL thread-exit backend is not implemented for this platform

#endif

inline constexpr uintptr_t thread_exit_key_initializing = 1;

inline constexpr uintptr_t thread_exit_key_failed = 2;

inline constexpr uintptr_t thread_exit_key_offset = 3;

inline constexpr uintptr_t invalid_thread_exit_key = ~uintptr_t{0};

inline atomic<uintptr_t> thread_exit_key_state{0};

inline uintptr_t thread_exit_key() noexcept {
  for (;;) {
    uintptr_t state = thread_exit_key_state.load(memory_order_acquire);

    if (state >= thread_exit_key_offset) {
      return state - thread_exit_key_offset;
    }

    if (state == thread_exit_key_failed) {
      return invalid_thread_exit_key;
    }

    if (state == 0) {
      uintptr_t expected = 0;

      if (thread_exit_key_state.compare_exchange_strong(
              expected, thread_exit_key_initializing, memory_order_acq_rel,
              memory_order_acquire)) {
        uintptr_t key = 0;

        const bool created = native_thread_exit_key_create(key);

        const uintptr_t published =
            created ? key + thread_exit_key_offset : thread_exit_key_failed;

        thread_exit_key_state.store(published, memory_order_release);

        thread_exit_key_state.notify_all();

        return created ? key : invalid_thread_exit_key;
      }

      continue;
    }

    thread_exit_key_state.wait(thread_exit_key_initializing,
                               memory_order_acquire);
  }
}

#if defined(_WIN32)

using windows_tls_callback = void(__stdcall *)(void *, unsigned long, void *);

inline void __stdcall thread_exit_dispatch(void *, unsigned long reason,
                                           void *) noexcept {
  constexpr unsigned long process_detach = 0;

  constexpr unsigned long thread_detach = 3;

  if (reason != thread_detach && reason != process_detach) {
    return;
  }

  const uintptr_t key = thread_exit_key();

  if (key == invalid_thread_exit_key) {
    return;
  }

  auto *actions =
      static_cast<thread_exit_action *>(native_thread_exit_get(key));

  if (actions == nullptr)
    return;

  // Prevent accidental re-observation while
  // executing deferred actions.
  (void)native_thread_exit_set(key, nullptr);

  run_thread_exit_actions(actions);
}

// MSVC-compatible CRTs place their dynamic
// thread_local destructor callback earlier in
// the .CRT$XL* callback array. XLY is deliberately
// late, immediately before the XLZ terminator.
//
// Consequently this callback observes the thread
// after C++ thread-storage-duration destruction.
#if defined(_MSC_VER)

#pragma section(".CRT$XLY", long, read)

extern "C" {

__declspec(allocate(".CRT$XLY"))
__declspec(selectany) extern const windows_tls_callback
    ftl_thread_exit_tls_callback = &thread_exit_dispatch;
}

#if defined(_M_IX86)
#pragma comment(linker, "/include:__tls_used")
#pragma comment(linker, "/include:_ftl_thread_exit_tls_callback")
#else
#pragma comment(linker, "/include:_tls_used")
#pragma comment(linker, "/include:ftl_thread_exit_tls_callback")
#endif

#elif defined(__GNUC__)

extern "C" __attribute__((section(".CRT$XLY"), used))
const windows_tls_callback ftl_thread_exit_tls_callback = &thread_exit_dispatch;

#else

#error FTL Windows thread-exit TLS callback is not implemented for this compiler

#endif

#elif defined(__linux__) || defined(__APPLE__)

inline void thread_exit_dispatch(void *value) noexcept {
  auto *actions = static_cast<thread_exit_action *>(value);

  if (actions == nullptr)
    return;

  const uintptr_t key = thread_exit_key();

  // pthread key destructors are unordered relative
  // to one another. Reinstall the value once so the
  // actual FTL actions execute during a subsequent
  // destructor iteration.
  //
  // If another notify_all_at_thread_exit registration
  // happens from a TLS destructor after this callback,
  // that new head starts with exit_rearmed_ == false
  // and therefore receives its own complete pass.
  if (!actions->exit_rearmed_) {
    actions->exit_rearmed_ = true;

    if (key != invalid_thread_exit_key &&
        native_thread_exit_set(key, actions)) {
      return;
    }

    // If pthread_setspecific unexpectedly fails
    // during teardown, running now is preferable
    // to permanently leaving the mutex locked.
  }

  if (key != invalid_thread_exit_key) {
    (void)native_thread_exit_set(key, nullptr);
  }

  run_thread_exit_actions(actions);
}

#endif

inline bool register_thread_exit_action(thread_exit_action *action) noexcept {
  const uintptr_t key = thread_exit_key();

  if (key == invalid_thread_exit_key) {
    return false;
  }

  auto *head = static_cast<thread_exit_action *>(native_thread_exit_get(key));

  action->next_ = head;

#if defined(__linux__) || defined(__APPLE__)
  action->exit_rearmed_ = false;
#endif

  if (!native_thread_exit_set(key, action)) {
    action->next_ = nullptr;

    return false;
  }

  return true;
}

FTL_THREAD_EXIT_END_NAMESPACE

#undef FTL_THREAD_EXIT_BEGIN_NAMESPACE
#undef FTL_THREAD_EXIT_END_NAMESPACE

#endif // FTL_DETAIL_THREAD_EXIT_HEADER
