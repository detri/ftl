// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_WAIT_NOTIFY_HEADER
#define FTL_DETAIL_WAIT_NOTIFY_HEADER

#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <cstdint>
#define FTL_WAIT_NOTIFY_BEGIN_NAMESPACE namespace std::detail {
#define FTL_WAIT_NOTIFY_END_NAMESPACE }
#else
#include <ftl/cstddef>
#include <ftl/cstdint>
#define FTL_WAIT_NOTIFY_BEGIN_NAMESPACE namespace ftl::detail {
#define FTL_WAIT_NOTIFY_END_NAMESPACE }
#endif

FTL_WAIT_NOTIFY_BEGIN_NAMESPACE

#if defined(_WIN32) && defined(_MSC_VER)

extern "C"
    __declspec(dllimport) int __stdcall WaitOnAddress(volatile void *, void *,
                                                      size_t, unsigned long);
extern "C" __declspec(dllimport) void __stdcall WakeByAddressSingle(void *);
extern "C" __declspec(dllimport) void __stdcall WakeByAddressAll(void *);

template <size_t Size>
inline constexpr bool wait_directly_supported =
    Size == 1 || Size == 2 || Size == 4 || Size == 8;

inline void platform_wait(const volatile void *address, uint64_t expected,
                          size_t size) noexcept {
  (void)WaitOnAddress(const_cast<volatile void *>(address), &expected, size,
                      0xffffffffUL);
}

inline void platform_wake_one(const volatile void *address, size_t) noexcept {
  WakeByAddressSingle(const_cast<void *>(address));
}

inline void platform_wake_all(const volatile void *address, size_t) noexcept {
  WakeByAddressAll(const_cast<void *>(address));
}

#elif defined(__linux__)

#if defined(__x86_64__)

inline long linux_futex(volatile uint32_t *address, long operation,
                        uint32_t value) noexcept {
  register long r10 __asm__("r10") = 0;
  register long r8 __asm__("r8") = 0;
  register long r9 __asm__("r9") = 0;

  long result;

  __asm__ volatile("syscall"
                   : "=a"(result)
                   : "a"(202L), "D"(address), "S"(operation),
                     "d"(static_cast<long>(value)), "r"(r10), "r"(r8), "r"(r9)
                   : "rcx", "r11", "memory");

  return result;
}

#define FTL_HAS_LINUX_FUTEX 1

#elif defined(__aarch64__)

inline long linux_futex(volatile uint32_t *address, long operation,
                        uint32_t value) noexcept {
  register long x0 __asm__("x0") = reinterpret_cast<long>(address);
  register long x1 __asm__("x1") = operation;
  register long x2 __asm__("x2") = static_cast<long>(value);
  register long x3 __asm__("x3") = 0;
  register long x4 __asm__("x4") = 0;
  register long x5 __asm__("x5") = 0;
  register long x8 __asm__("x8") = 98;

  __asm__ volatile("svc #0"
                   : "+r"(x0)
                   : "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5), "r"(x8)
                   : "memory");

  return x0;
}

#define FTL_HAS_LINUX_FUTEX 1

#else

#define FTL_HAS_LINUX_FUTEX 0

#endif

#if FTL_HAS_LINUX_FUTEX

template <size_t Size>
inline constexpr bool wait_directly_supported = Size == 4;

inline void platform_wait(const volatile void *address, uint64_t expected,
                          size_t) noexcept {
  (void)linux_futex(reinterpret_cast<volatile uint32_t *>(
                        const_cast<volatile void *>(address)),
                    0, static_cast<uint32_t>(expected));
}

inline void platform_wake_one(const volatile void *address, size_t) noexcept {
  (void)linux_futex(reinterpret_cast<volatile uint32_t *>(
                        const_cast<volatile void *>(address)),
                    1, 1);
}

inline void platform_wake_all(const volatile void *address, size_t) noexcept {
  (void)linux_futex(reinterpret_cast<volatile uint32_t *>(
                        const_cast<volatile void *>(address)),
                    1, 0x7fffffffU);
}

#else

template <size_t Size> inline constexpr bool wait_directly_supported = false;

inline void platform_wait(const volatile void *, uint64_t, size_t) noexcept {}

inline void platform_wake_one(const volatile void *, size_t) noexcept {}

inline void platform_wake_all(const volatile void *, size_t) noexcept {}

#endif

#elif defined(__APPLE__)

extern "C" int os_sync_wait_on_address(void *, uint64_t, size_t, int)
    __attribute__((weak_import));

extern "C" int os_sync_wake_by_address_any(void *, size_t, int)
    __attribute__((weak_import));

extern "C" int os_sync_wake_by_address_all(void *, size_t, int)
    __attribute__((weak_import));

template <size_t Size>
inline constexpr bool wait_directly_supported = Size == 4 || Size == 8;

inline void platform_wait(const volatile void *address, uint64_t expected,
                          size_t size) noexcept {
  if (os_sync_wait_on_address != nullptr) {
    (void)os_sync_wait_on_address(const_cast<void *>(address), expected, size,
                                  0);
  }
}

inline void platform_wake_one(const volatile void *address,
                              size_t size) noexcept {
  if (os_sync_wake_by_address_any != nullptr) {
    (void)os_sync_wake_by_address_any(const_cast<void *>(address), size, 0);
  }
}

inline void platform_wake_all(const volatile void *address,
                              size_t size) noexcept {
  if (os_sync_wake_by_address_all != nullptr) {
    (void)os_sync_wake_by_address_all(const_cast<void *>(address), size, 0);
  }
}

#else

template <size_t Size> inline constexpr bool wait_directly_supported = false;

inline void platform_wait(const volatile void *, uint64_t, size_t) noexcept {}

inline void platform_wake_one(const volatile void *, size_t) noexcept {}

inline void platform_wake_all(const volatile void *, size_t) noexcept {}

#endif

#if !(defined(_WIN32) && defined(_MSC_VER))

struct alignas(64) wait_bucket {
  uint32_t version = 0;
};

inline wait_bucket wait_buckets[64]{};

inline volatile uint32_t *
wait_bucket_for(const volatile void *address) noexcept {
  const auto bits =
      reinterpret_cast<uintptr_t>(const_cast<const void *>(address));

  const size_t index = static_cast<size_t>(((bits >> 4) ^ (bits >> 10)) & 63u);

  return &wait_buckets[index].version;
}

inline uint32_t wait_bucket_load(const volatile uint32_t *value) noexcept {
#if defined(__GNUC__) || defined(__clang__)
  return __atomic_load_n(value, __ATOMIC_SEQ_CST);
#else
  return *value;
#endif
}

inline void wait_bucket_bump(volatile uint32_t *value) noexcept {
#if defined(__GNUC__) || defined(__clang__)
  (void)__atomic_add_fetch(value, uint32_t{1}, __ATOMIC_SEQ_CST);
#else
  ++*value;
#endif
}

#endif

template <size_t Size, class Word, class LoadWord>
void atomic_wait_until_changed(const volatile void *address, Word old,
                               LoadWord load_word) noexcept {
  static_assert(Size == 1 || Size == 2 || Size == 4 || Size == 8);

  for (;;) {
    if (load_word() != old)
      return;

    if constexpr (wait_directly_supported<Size>) {
      platform_wait(address, static_cast<uint64_t>(old), Size);
    } else {
#if defined(_WIN32) && defined(_MSC_VER)
      static_assert(wait_directly_supported<Size>,
                    "Windows should directly wait on every "
                    "supported FTL atomic size");
#else
      volatile uint32_t *version = wait_bucket_for(address);

      const uint32_t observed = wait_bucket_load(version);

      // Close the lost-wakeup window between
      // observing the atomic and sleeping on
      // the proxy word.
      if (load_word() != old)
        return;

      platform_wait(version, observed, sizeof(uint32_t));
#endif
    }
  }
}

template <size_t Size>
void atomic_notify_one(const volatile void *address) noexcept {
  static_assert(Size == 1 || Size == 2 || Size == 4 || Size == 8);

  if constexpr (wait_directly_supported<Size>) {
    platform_wake_one(address, Size);
  } else {
#if defined(_WIN32) && defined(_MSC_VER)
    static_assert(wait_directly_supported<Size>,
                  "Windows should directly wake every "
                  "supported FTL atomic size");
#else
    volatile uint32_t *version = wait_bucket_for(address);

    wait_bucket_bump(version);

    // Multiple unrelated atomics can hash to this
    // bucket. Waking only one waiter could wake a
    // waiter for the wrong atomic and strand the
    // intended waiter.
    platform_wake_all(version, sizeof(uint32_t));
#endif
  }
}

template <size_t Size>
void atomic_notify_all(const volatile void *address) noexcept {
  static_assert(Size == 1 || Size == 2 || Size == 4 || Size == 8);

  if constexpr (wait_directly_supported<Size>) {
    platform_wake_all(address, Size);
  } else {
#if defined(_WIN32) && defined(_MSC_VER)
    static_assert(wait_directly_supported<Size>,
                  "Windows should directly wake every "
                  "supported FTL atomic size");
#else
    volatile uint32_t *version = wait_bucket_for(address);

    wait_bucket_bump(version);

    platform_wake_all(version, sizeof(uint32_t));
#endif
  }
}

#if defined(FTL_HAS_LINUX_FUTEX)
#undef FTL_HAS_LINUX_FUTEX
#endif

FTL_WAIT_NOTIFY_END_NAMESPACE

#undef FTL_WAIT_NOTIFY_BEGIN_NAMESPACE
#undef FTL_WAIT_NOTIFY_END_NAMESPACE

#endif // FTL_DETAIL_WAIT_NOTIFY_HEADER
