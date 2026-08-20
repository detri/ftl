// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_FENV_H_HEADER
#define FTL_FENV_H_HEADER

#define FE_INVALID 0x01
#define FE_DIVBYZERO 0x04
#define FE_OVERFLOW 0x08
#define FE_UNDERFLOW 0x10
#define FE_INEXACT 0x20
#define FE_ALL_EXCEPT 0x3d

#define FE_TONEAREST 0x000
#define FE_DOWNWARD 0x400
#define FE_UPWARD 0x800
#define FE_TOWARDZERO 0xc00

#if defined(__x86_64__) || defined(_M_X64)
struct fenv_t {
  unsigned short control;
  unsigned short status;
  unsigned int mxcsr;
};
#elif defined(__aarch64__) || defined(_M_ARM64)
struct fenv_t {
  unsigned long long control;
  unsigned long long status;
};
#else
#error "FTL <fenv.h> supports x86-64 and AArch64"
#endif

using fexcept_t = unsigned int;
#define FE_DFL_ENV ((const fenv_t *)-1)

namespace ftl_fenv_detail {

#if defined(__x86_64__) || defined(_M_X64)

#if defined(_MSC_VER) && !defined(__clang__)
extern "C" void __cdecl _fxsave64(void *);
extern "C" void __cdecl _fxrstor64(const void *);
#pragma intrinsic(_fxsave64, _fxrstor64)
#endif

inline unsigned short load16(const unsigned char *data) noexcept {
  return static_cast<unsigned short>(data[0] | (unsigned(data[1]) << 8));
}
inline unsigned int load32(const unsigned char *data) noexcept {
  return unsigned(data[0]) | (unsigned(data[1]) << 8) |
         (unsigned(data[2]) << 16) | (unsigned(data[3]) << 24);
}
inline void store16(unsigned char *data, unsigned short value) noexcept {
  data[0] = static_cast<unsigned char>(value);
  data[1] = static_cast<unsigned char>(value >> 8);
}
inline void store32(unsigned char *data, unsigned int value) noexcept {
  data[0] = static_cast<unsigned char>(value);
  data[1] = static_cast<unsigned char>(value >> 8);
  data[2] = static_cast<unsigned char>(value >> 16);
  data[3] = static_cast<unsigned char>(value >> 24);
}

inline void save(unsigned char *state) noexcept {
#if defined(_MSC_VER) && !defined(__clang__)
  _fxsave64(state);
#else
  __asm__ volatile("fxsave64 %0" : "=m"(*state) : : "memory");
#endif
}

inline void restore(const unsigned char *state) noexcept {
#if defined(_MSC_VER) && !defined(__clang__)
  _fxrstor64(state);
#else
  __asm__ volatile("fxrstor64 %0" : : "m"(*state) : "memory");
#endif
}

inline fenv_t get() noexcept {
  alignas(16) unsigned char state[512];
  save(state);
  return {load16(state), load16(state + 2), load32(state + 24)};
}

inline void set(const fenv_t &environment) noexcept {
  alignas(16) unsigned char state[512];
  save(state);
  store16(state, environment.control);
  store16(state + 2, static_cast<unsigned short>(
                         (load16(state + 2) & ~0x7fu) |
                         (environment.status & 0x7fu)));
  store32(state + 24,
          (load32(state + 24) & ~0xffffu) | (environment.mxcsr & 0xffffu));
  restore(state);
}

inline fenv_t defaults() noexcept { return {0x037f, 0, 0x1f80}; }

#else

inline unsigned long long read_control() noexcept {
  unsigned long long value;
  __asm__ volatile("mrs %0, fpcr" : "=r"(value) : : "memory");
  return value;
}
inline unsigned long long read_status() noexcept {
  unsigned long long value;
  __asm__ volatile("mrs %0, fpsr" : "=r"(value) : : "memory");
  return value;
}
inline void write_control(unsigned long long value) noexcept {
  __asm__ volatile("msr fpcr, %0" : : "r"(value) : "memory");
}
inline void write_status(unsigned long long value) noexcept {
  __asm__ volatile("msr fpsr, %0" : : "r"(value) : "memory");
}
inline fenv_t get() noexcept { return {read_control(), read_status()}; }
inline void set(const fenv_t &environment) noexcept {
  write_control(environment.control);
  write_status(environment.status);
}
inline fenv_t defaults() noexcept { return {0, 0}; }

inline unsigned int from_native(unsigned long long flags) noexcept {
  return unsigned(flags & 1u) | unsigned((flags & 2u) << 1) |
         unsigned((flags & 0x1cu) << 1);
}
inline unsigned long long to_native(unsigned int flags) noexcept {
  return (flags & 1u) | ((flags & 4u) >> 1) | ((flags & 0x38u) >> 1);
}

#endif

inline void raise_with_arithmetic(unsigned int exceptions) noexcept {
  volatile double left{};
  volatile double right{};
  volatile double result{};

  if ((exceptions & FE_INVALID) != 0) {
    left = 0.0;
    right = 0.0;
    result = left / right;
  }
  if ((exceptions & FE_DIVBYZERO) != 0) {
    left = 1.0;
    right = 0.0;
    result = left / right;
  }
  if ((exceptions & FE_OVERFLOW) != 0) {
    left = 0x1.fffffffffffffp+1023;
    right = left;
    result = left * right;
  }
  if ((exceptions & FE_UNDERFLOW) != 0) {
    left = 0x1p-1022;
    right = left;
    result = left * right;
  }
  if ((exceptions & FE_INEXACT) != 0) {
    left = 2.0;
    right = 3.0;
    result = left / right;
  }

  (void)result;
}

} // namespace ftl_fenv_detail

extern "C" inline int feclearexcept(int exceptions) noexcept {
  exceptions &= FE_ALL_EXCEPT;
#if defined(__x86_64__) || defined(_M_X64)
  auto environment = ftl_fenv_detail::get();
  environment.status &= static_cast<unsigned short>(~exceptions);
  environment.mxcsr &= ~unsigned(exceptions);
  ftl_fenv_detail::set(environment);
#else
  auto status = ftl_fenv_detail::read_status();
  status &= ~ftl_fenv_detail::to_native(unsigned(exceptions));
  ftl_fenv_detail::write_status(status);
#endif
  return 0;
}

extern "C" inline int fegetexceptflag(fexcept_t *flag,
                                       int exceptions) noexcept {
  if (!flag)
    return 1;
#if defined(__x86_64__) || defined(_M_X64)
  const auto environment = ftl_fenv_detail::get();
  *flag = unsigned(environment.status | environment.mxcsr) &
          unsigned(exceptions & FE_ALL_EXCEPT);
#else
  *flag = ftl_fenv_detail::from_native(ftl_fenv_detail::read_status()) &
          unsigned(exceptions & FE_ALL_EXCEPT);
#endif
  return 0;
}

extern "C" inline int feraiseexcept(int exceptions) noexcept {
  exceptions &= FE_ALL_EXCEPT;
  // Arithmetic execution is required here rather than merely editing sticky
  // status bits: when the caller has enabled a trap, the corresponding
  // operation must actually deliver it. The explicit status merge below also
  // keeps both architectural status stores coherent on masked configurations.
  ftl_fenv_detail::raise_with_arithmetic(unsigned(exceptions));
#if defined(__x86_64__) || defined(_M_X64)
  auto environment = ftl_fenv_detail::get();
  environment.status |= static_cast<unsigned short>(exceptions);
  environment.mxcsr |= unsigned(exceptions);
  ftl_fenv_detail::set(environment);
#else
  auto status = ftl_fenv_detail::read_status();
  status |= ftl_fenv_detail::to_native(unsigned(exceptions));
  ftl_fenv_detail::write_status(status);
#endif
  return 0;
}

extern "C" inline int fesetexceptflag(const fexcept_t *flag,
                                       int exceptions) noexcept {
  if (!flag)
    return 1;
  exceptions &= FE_ALL_EXCEPT;
  feclearexcept(exceptions);
  return feraiseexcept(int(*flag) & exceptions);
}

extern "C" inline int fetestexcept(int exceptions) noexcept {
  fexcept_t flags{};
  return fegetexceptflag(&flags, exceptions) == 0 ? int(flags) : 0;
}

extern "C" inline int fegetround() noexcept {
#if defined(__x86_64__) || defined(_M_X64)
  return ftl_fenv_detail::get().control & 0xc00;
#else
  const auto rounding = unsigned((ftl_fenv_detail::read_control() >> 22) & 3u);
  return rounding == 1 ? FE_UPWARD
         : rounding == 2 ? FE_DOWNWARD
         : rounding == 3 ? FE_TOWARDZERO
                         : FE_TONEAREST;
#endif
}

extern "C" inline int fesetround(int rounding) noexcept {
  if (rounding != FE_TONEAREST && rounding != FE_DOWNWARD &&
      rounding != FE_UPWARD && rounding != FE_TOWARDZERO)
    return 1;
#if defined(__x86_64__) || defined(_M_X64)
  auto environment = ftl_fenv_detail::get();
  environment.control = static_cast<unsigned short>(
      (environment.control & ~0xc00u) | unsigned(rounding));
  environment.mxcsr =
      (environment.mxcsr & ~0x6000u) | (unsigned(rounding) << 3);
  ftl_fenv_detail::set(environment);
#else
  const unsigned native = rounding == FE_UPWARD     ? 1u
                          : rounding == FE_DOWNWARD ? 2u
                          : rounding == FE_TOWARDZERO ? 3u
                                                     : 0u;
  auto control = ftl_fenv_detail::read_control();
  control = (control & ~(3ull << 22)) |
            (static_cast<unsigned long long>(native) << 22);
  ftl_fenv_detail::write_control(control);
#endif
  return 0;
}

extern "C" inline int fegetenv(fenv_t *environment) noexcept {
  if (!environment)
    return 1;
  *environment = ftl_fenv_detail::get();
  return 0;
}

extern "C" inline int fesetenv(const fenv_t *environment) noexcept {
  if (!environment)
    return 1;
  ftl_fenv_detail::set(environment == FE_DFL_ENV
                           ? ftl_fenv_detail::defaults()
                           : *environment);
  return 0;
}

extern "C" inline int feholdexcept(fenv_t *environment) noexcept {
  if (fegetenv(environment) != 0)
    return 1;
  auto held = *environment;
#if defined(__x86_64__) || defined(_M_X64)
  held.control |= 0x3f;
  held.status &= static_cast<unsigned short>(~0x3f);
  held.mxcsr = (held.mxcsr | (0x3fu << 7)) & ~0x3fu;
#else
  held.control &= ~(0x1full << 8);
  held.status &= ~0x1full;
#endif
  ftl_fenv_detail::set(held);
  return 0;
}

extern "C" inline int feupdateenv(const fenv_t *environment) noexcept {
  const int raised = fetestexcept(FE_ALL_EXCEPT);
  if (fesetenv(environment) != 0)
    return 1;
  return feraiseexcept(raised);
}

#endif
