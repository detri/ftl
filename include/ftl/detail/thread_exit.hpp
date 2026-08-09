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
};

inline void run_thread_exit_actions(void *value) noexcept {
  auto *action = static_cast<thread_exit_action *>(value);

  while (action != nullptr) {
    thread_exit_action *next = action->next_;

    action->invoke_(action);

    action = next;
  }
}

#if defined(_WIN32)

using windows_fls_callback = void(__stdcall *)(void *);

extern "C" __declspec(dllimport) unsigned long __stdcall
FlsAlloc(windows_fls_callback);

extern "C" __declspec(dllimport) void *__stdcall FlsGetValue(unsigned long);

extern "C" __declspec(dllimport) int __stdcall FlsSetValue(unsigned long,
                                                           void *);

inline void __stdcall thread_exit_dispatch(void *value) noexcept {
  run_thread_exit_actions(value);
}

inline bool native_thread_exit_key_create(uintptr_t &result) noexcept {
  constexpr unsigned long out_of_indexes = 0xffffffffUL;

  const unsigned long key = FlsAlloc(&thread_exit_dispatch);

  if (key == out_of_indexes)
    return false;

  result = static_cast<uintptr_t>(key);
  return true;
}

inline void *native_thread_exit_get(uintptr_t key) noexcept {
  return FlsGetValue(static_cast<unsigned long>(key));
}

inline bool native_thread_exit_set(uintptr_t key, void *value) noexcept {
  return FlsSetValue(static_cast<unsigned long>(key), value) != 0;
}

#elif defined(__linux__) || defined(__APPLE__)

#if defined(__APPLE__)
using posix_thread_exit_key = unsigned long;
#else
using posix_thread_exit_key = unsigned int;
#endif

extern "C" int pthread_key_create(posix_thread_exit_key *, void (*)(void *));

extern "C" void *pthread_getspecific(posix_thread_exit_key);

extern "C" int pthread_setspecific(posix_thread_exit_key, const void *);

inline void thread_exit_dispatch(void *value) noexcept {
  run_thread_exit_actions(value);
}

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

    if (state >= thread_exit_key_offset)
      return state - thread_exit_key_offset;

    if (state == thread_exit_key_failed)
      return invalid_thread_exit_key;

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

inline bool register_thread_exit_action(thread_exit_action *action) noexcept {
  const uintptr_t key = thread_exit_key();

  if (key == invalid_thread_exit_key)
    return false;

  auto *head = static_cast<thread_exit_action *>(native_thread_exit_get(key));

  action->next_ = head;

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
