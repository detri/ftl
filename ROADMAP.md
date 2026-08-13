# FTL Roadmap

FTL is a C++23, header-only standard-library replacement for environments where
a hosted vendor STL is unavailable, undesirable, or incompatible with the
desired portability model.

It targets MSVC, Clang, and GCC and supports two usage modes:

- normal mode, with the library exposed through `ftl`
- `FTL_REPLACE_STL`, with FTL headers placed directly on the include path and
  the standard-library interface exposed through `std`

The goal is source-interface compatibility with the C++23 standard library,
not binary compatibility with MSVC STL, libstdc++, or libc++.

## Product boundary

FTL targets the complete C++23 standard-library source interface, including:

- public header synopses
- overload sets and deduction guides
- concepts and constraints
- customization-point behavior
- conditional `noexcept`
- `constexpr` behavior
- allocator semantics
- required specializations
- feature-test macros
- C compatibility headers
- normal and replacement-mode behavior

FTL does not promise ABI compatibility with vendor standard libraries.

Replacement mode is therefore an all-or-nothing choice across any binary
interface that exchanges standard-library types. Source-built dependencies may
participate in the same FTL replacement environment; incompatible prebuilt
libraries remain explicit ABI boundaries.

`optional<T&>` is currently provided as a C++26-compatible extension. It must
not alter the required C++23 `optional<T>` interface or semantics.

Named C++ standard-library modules are not currently implemented, and
`__cpp_lib_modules` is intentionally not advertised.

## Design rules

- Do not depend on a hosted vendor C++ standard library to implement FTL's own
  public standard-library surface.
- Prefer language features, compiler intrinsics, and existing FTL facilities
  over platform-specific dependencies.
- Keep platform ABI and operating-system integration behind private detail
  boundaries.
- Preserve zero-overhead properties expected from the standard library,
  including triviality, layout, `constexpr`, constraints, and conditional
  `noexcept`.
- Support MSVC, Clang, and GCC explicitly or fail clearly when a platform or
  toolchain facility is unsupported.
- Fix defects at the lowest responsible layer rather than introducing local
  workarounds in dependent facilities.
- Do not add speculative abstractions or unrelated dependencies.
- Later-standard facilities must remain explicit extensions until a later
  language-standard baseline is formally adopted.

## Completion standard

A public facility is considered complete only when:

1. Its entire C++23 synopsis is implemented.
2. Its specified semantics, constraints, overload participation, exception
   behavior, and customization rules are represented.
3. Relevant feature-test macros have the correct C++23 values and appear in
   every required header.
4. Normal and `FTL_REPLACE_STL` modes are both covered.
5. MSVC, Clang, and GCC pass the supported configuration matrix.
6. Relevant compile-fail, `constexpr`, triviality, layout, allocator,
   runtime-dependency, and behavioral tests exist.
7. Public dependencies used by the facility are themselves implemented rather
   than accidentally supplied by a hosted STL.

File presence alone is never evidence of completion.

A completed facility may be reopened whenever a standards audit, supported
compiler, supported platform, or product-integration test exposes a real
defect.

## Current C++23 status

The C++23 source-interface implementation is complete.

All C++23 public standard-library headers in FTL's declared scope are present
and have passed their implementation closures. No C++23 headers are currently
classified as seeded or absent.

The C compatibility-header surface is also implemented. Required `.h`
spellings route to FTL-owned facilities in both normal and replacement modes,
including the special compatibility headers such as:

- `<complex.h>`
- `<ctype.h>`
- `<iso646.h>`
- `<stdalign.h>`
- `<stdbool.h>`
- `<stddef.h>`
- `<stdio.h>`
- `<tgmath.h>`
- `<time.h>`

Normal-mode compatibility tests use the `ftl/` header spellings directly so
they cannot silently fall through to vendor CRT or STL headers. Replacement
mode tests resolve the corresponding unprefixed standard spellings through the
FTL replacement include path.

`<text_encoding>` is not a C++23 facility and is out of scope for the current
baseline.

## Conformance verification

A whole-library N4950 audit was performed after initial C++23 implementation
completion.

That audit exposed defects in several previously completed areas. The
remediation pass included:

- C++23 `<concepts>` customization and comparison semantics
- `ranges::swap` behavior
- heterogeneous comparison requirements
- feature-test macro values, ownership, and required header placement
- semantic validation of advertised feature-test macros
- `type_info` constant-evaluation equality
- final C++23 ranges semantics associated with
  `__cpp_lib_ranges == 202302L`
- P2609-adjusted indirect callable and common-reference behavior
- `<cassert>` repeated inclusion when `NDEBUG` changes
- missing and incorrectly routed C compatibility headers
- discovery and implementation of the missing `<cctype>` surface
- removal of normal-mode C-header paths that accidentally consumed hosted
  vendor C++ headers
- missing `<ctime>` `timespec`, `timespec_get`, and `TIME_UTC` facilities
- platform-correct `CLOCKS_PER_SEC`
- `<generator>` support for over-aligned allocator objects without changing
  the standard-mandated default-aligned rebound allocation unit
- stale, incorrect, or non-standard feature-test advertisement

The remediation findings are fixed and current tests are green.

A fresh whole-repository audit against N4950 remains required. It must be
performed from the standard and current repository state rather than merely
checking that the previous finding list was repaired.

Passing that clean re-audit is the final verification milestone for the C++23
surface.

## Runtime and portability model

FTL owns its C++ library implementation but necessarily crosses platform
runtime boundaries for facilities such as:

- allocation
- exceptions and RTTI
- threads and synchronization
- clocks and time
- locale
- terminal and file I/O
- filesystem
- stack traces
- entropy
- timezone discovery and data

Those boundaries should remain narrow, private, and explicit.

Existing examples include native Win32/POSIX filesystem operations, native
address-wait primitives for synchronization, private clock abstractions,
FTL-owned timezone handling, and independently owned regex and mathematical
implementations.

Platform implementation details must not leak into the public standard-library
interface.

## Replacement product

The next major development phase is turning the completed library surface into
a deliberate whole-program replacement product.

Priority work:

1. Implement `ftl_replace_stl()` as a transitive per-target CMake integration
   mechanism.

2. Make runtime dependencies feature-sensitive per target instead of attaching
   unnecessary hosted/runtime dependencies universally.

   One known example is the current UNIX `Threads::Threads` interface
   dependency, which cannot presently be removed merely by defining
   `FTL_NO_THREADS` on a consumer.

3. Define and document the runtime boundary for allocation, exceptions, RTTI,
   threading, locale, I/O, filesystem, timezone data, and other hosted
   services.

4. Define ABI versioning and supported interoperability rules.

5. Add whole-program replacement examples using non-trivial source-built
   dependency graphs.

6. Add replacement-mode integration tests designed specifically to detect
   accidental vendor-STL mixing across multiple translation units and
   dependencies.

7. Add ABI and layout regression checks for standard-library-facing public
   types.

8. Add sanitizer configurations across the supported compiler and platform
   matrix.

9. Add fuzzing for parser-heavy and stateful facilities, especially:

    - numeric and text conversion
    - format
    - regex
    - filesystem paths
    - chrono parsing
    - timezone data
    - locale-sensitive parsing

10. Audit initialization and destruction ordering across global runtime state,
    thread-local state, locale, I/O, allocation, filesystem, and timezone
    facilities.

11. Define explicit failure behavior for unsupported platform facilities so an
    unsupported target fails clearly instead of accidentally importing a
    hosted implementation.

12. Add cross-toolchain conformance sweeps comparing public synopsis,
    constraints, feature-test advertisement, and observable behavior without
    assuming vendor ABI or implementation details.

The replacement-product milestone is complete when a non-trivial application
and its source-built dependency graph can select FTL without accidental
vendor-STL mixing and with documented runtime and ABI boundaries.

### Later language standards

Adopt C++26 and future standards as explicit deltas from the completed C++23
baseline rather than reopening the library as another monolithic rewrite.

`optional<T&>` should remain classified as an extension until its corresponding
standard delta is formally adopted.

### FTL extensions

Potential opt-in system/game-oriented facilities may include:

- fixed-capacity containers
- alternative measured container layouts
- explicitly removable debug hardening
- other facilities justified by measured game/system workloads

Extensions must not compromise standard-compatible interfaces or complicate
replacement mode.

## Non-goals

- Binary compatibility with MSVC STL, libstdc++, or libc++.
- Passing FTL standard-library objects across incompatible prebuilt ABI
  boundaries.
- Mixing partial FTL replacements with vendor definitions in one translation
  unit.
- Depending on a hosted vendor C++ standard library to implement FTL's public
  standard-library surface.
- Treating implementation-detail utilities as public standard-library
  milestones.
- Claiming conformance from header presence or smoke tests alone.
- Hiding unsupported runtime or platform requirements behind accidental vendor
  dependencies.
- Adding speculative abstractions without a concrete standard, portability, or
  measured product requirement.
