# Freestanding Template Library

FTL is a C++23, header-only STL replacement experiment for game development,
container optimization, and environments where the hosted standard library is
unavailable. It targets MSVC, Clang, and GCC.

The long-term product direction and ABI boundaries are defined in
`ROADMAP.md`. Preserve them when making architectural or build-system changes.

## Design rules

- Keep public APIs compatible with the current C++ standard-library facility.
- Freestanding use is the point: do not depend on a hosted STL in `ftl` mode.
- Every header must also work with `FTL_REPLACE_STL`, where `include/ftl` is
  placed directly on the include path and declarations are provided in `std`.
- Prefer compiler intrinsics, language features, and existing FTL facilities
  over platform-specific runtime dependencies.
- Preserve zero-overhead properties that users reasonably expect from the STL,
  including triviality, `constexpr`, constraints, and conditional `noexcept`.
- Platform-specific code must cover MSVC, Clang, and GCC or fail clearly at
  configuration/compile time.
- Do not add speculative abstractions or dependencies.

## Layout and checks

- Public headers are extensionless files under `include/ftl`.
- Normal-mode tests are `tests/<header>.cpp`.
- Replacement-mode tests are `tests/replace_<header>.cpp`.
- Register runnable tests in `CMakeLists.txt`; compile-only coverage may use an
  object-library target.
- Build and run both modes after changing a public header.

`optional<T&>` is intentionally provided as a C++26-compatible extension while
the project baseline remains C++23.

# FTL Roadmap

## Product boundary

FTL targets the complete C++23 standard-library source interface on MSVC,
Clang, and GCC, in both namespaced and STL-replacement modes. Later-standard
facilities are separate extensions; today that means `optional<T&>` only.

FTL does not promise binary compatibility with vendor standard libraries.
Replacement mode is all-or-nothing within a binary interface: dependencies
that exchange standard-library types must be rebuilt with FTL, while prebuilt
libraries and platform components remain explicit ABI boundaries.

```cmake
# Namespaced mode
target_link_libraries(game PRIVATE ftl::ftl)

# Future replacement-mode integration
ftl_replace_stl(game)
```

## How progress is measured

A file existing under `include/ftl` does not make a header complete. A header
is complete only when:

1. Its entire C++23 synopsis is implemented, including overloads, constraints,
   deduction guides, feature-test macros, conditional `noexcept`, and required
   specializations.
2. It has focused normal-mode and `FTL_REPLACE_STL` tests.
3. Positive, negative/compile-fail, `constexpr`, triviality, layout, and
   allocation/runtime-dependency checks exist where applicable.
4. Both modes pass on MSVC, Clang, and GCC in the supported configuration
   matrix.
5. Every facility it uses from public transitive dependencies is implemented
   and covered; unrelated missing surface in those dependency headers does not
   block completion.

Status terms used below:

- **Seeded**: a header exists and implements a useful subset.
- **Absent**: no public header exists.
- **Complete**: satisfies the five gates above.

Before work starts on a closure, create a tracking issue and copy each
header's C++23 synopsis into it as a checklist. Include the relevant tests,
feature-test macros, and both usage modes in that issue. The roadmap tracks
dependency order, not hundreds of individual overloads.

## Current inventory

### Complete headers

| Header                 | Completed closure |
|------------------------|-------------------|
| `<cstddef>`            | Stage 1.1         |
| `<cstdint>`            | Stage 1.1         |
| `<initializer_list>`   | Stage 1.1         |
| `<type_traits>`        | Stage 1.2         |
| `<utility>`            | Stage 1.2         |
| `<concepts>`           | Stage 1.2         |
| `<compare>`            | Stage 1.2         |
| `<limits>`             | Stage 1.3         |
| `<bit>`                | Stage 1.3         |
| `<version>`            | Stage 1.3         |
| `<source_location>`    | Stage 1.3         |
| `<stdfloat>`           | Stage 1.3         |
| `<new>`                | Stage 1.4         |
| `<exception>`          | Stage 1.4         |
| `<typeinfo>`           | Stage 1.4         |
| `<typeindex>`          | Stage 1.4         |
| `<cassert>`            | Stage 1.5         |
| `<cerrno>`             | Stage 1.5         |
| `<cfloat>`             | Stage 1.5         |
| `<climits>`            | Stage 1.5         |
| `<cstdarg>`            | Stage 1.5         |
| `<csetjmp>`            | Stage 1.5         |
| `<csignal>`            | Stage 1.5         |
| `<cstdlib>`            | Stage 1.5         |
| `<cinttypes>`          | Stage 1.5         |
| `<iterator>`           | Stage 2.1         |
| `<functional>`         | Stage 2.3         |
| `<memory>`             | Stage 2.4         |
| `<scoped_allocator>`   | Stage 2.4         |
| `<memory_resource>`    | Stage 2.4         |
| `<optional>`           | Stage 2.5         |
| `<expected>`           | Stage 2.5         |
| `<variant>`            | Stage 2.5         |
| `<any>`                | Stage 2.5         |
| `<coroutine>`          | Stage 2.6.1       |
| `<generator>`          | Stage 2.6.4       |
| `<mdspan>`             | Stage 2.6.5       |
| `<span>`               | Stage 2.6.6       |
| `<ranges>`             | Stage 2.6.6       |
| `<array>`              | Stage 2.6.6       |
| `<tuple>`              | Stage 2.6.6       |
| `<execution>`          | Stage 3.1         |
| `<algorithm>`          | Stage 3.1         |
| `<numeric>`            | Stage 3.1         |
| `<cstring>`            | Stage 3.2         |
| `<cwchar>`             | Stage 3.2         |
| `<cwctype>`            | Stage 3.2         |
| `<cuchar>`             | Stage 3.2         |
| `<charconv>`           | Stage 3.3         |
| `<string_view>`        | Stage 3.3         |
| `<string>`             | Stage 3.4         |
| `<bitset>`             | Stage 3.5         |
| `<vector>`             | Stage 4.1         |
| `<deque>`              | Stage 4.2         |
| `<queue>`              | Stage 4.2         |
| `<stack>`              | Stage 4.2         |
| `<forward_list>`       | Stage 4.3         |
| `<list>`               | Stage 4.3         |
| `<set>`                | Stage 4.3         |
| `<map>`                | Stage 4.3         |
| `<unordered_set>`      | Stage 4.3         |
| `<unordered_map>`      | Stage 4.3         |
| `<flat_set>`           | Stage 4.3         |
| `<flat_map>`           | Stage 4.3         |
| `<ratio>`              | Stage 5.1         |
| `<ctime>`              | Stage 5.1         |
| `<stdexcept>`          | Stage 5.2         |
| `<system_error>`       | Stage 5.2         |
| `<cfenv>`              | Stage 5.3         |
| `<numbers>`            | Stage 5.3         |
| `<cmath>`              | Stage 5.3         |
| `<complex>`            | Stage 5.3         |
| `<random>`             | Stage 5.4         |
| `<valarray>`           | Stage 5.4         |
| `<atomic>`             | Stage 6.1         |
| `<stdatomic.h>`        | Stage 6.1         |
| `<stop_token>`         | Stage 6.2         |
| `<mutex>`              | Stage 6.3         |
| `<shared_mutex>`       | Stage 6.3         |
| `<condition_variable>` | Stage 6.3         |
| `<semaphore>`          | Stage 6.4         |
| `<latch>`              | Stage 6.4         |
| `<barrier>`            | Stage 6.4         |

Compiler coroutine syntax integrates with FTL only in FTL_REPLACE_STL mode
because coroutine transformation performs lookup through std::coroutine_traits.
Normal namespace mode still provides and tests the library types directly.

### Seeded headers

The following public headers exist but remain incomplete against their C++23
synopses:

| Header         | Implemented direction                                                                                           | Major remaining groups                                                                                             |
|----------------|-----------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------|
| `<thread>`     | complete thread/jthread runtime surface, IDs, hashing, stop-token integration, sleep/yield, and native backends | `thread::id` stream insertion and formatter after Stage 7                                                          |
| `<stacktrace>` | native capture, entries, allocator-aware container, comparisons, strings, PMR, and hashes                       | stream insertion and formatters after Stage 7; feature-test advertisement                                          |
| `<iosfwd>`     | `char_traits`, fundamental stream types, aliases, and stream-class forward declarations                         | complete C++23 forward-declaration and positioning-type inventory                                                  |
| `<streambuf>`  | input/get-area stream-buffer machinery needed by seeded input streams                                           | put area, positioning/seeking, locale, synchronization, putback, and complete synopsis                             |
| `<ios>`        | stream state, buffer association, state observers, and boolean conversion                                       | full `ios_base`/`basic_ios` formatting, locales, callbacks, ties, exception masks, and synopsis                    |
| `<istream>`    | core unformatted character input, bulk reads, stream-state propagation, and user-defined extraction integration | sentry, formatted extraction, remaining unformatted overloads, positioning, synchronization, and complete synopsis |
| `<chrono>`     | durations, time points, calendars, clocks, clock conversion, literals, and `hh_mm_ss`                           | tzdb; formatters, stream insertion, and `from_stream` after Stage 7                                                |

`include/ftl/detail/rapidhash` is an implementation detail, not a standard
header or roadmap milestone.

Current test gaps are also explicit: `<type_traits>`, `<utility>`, and
`<concepts>` only receive aggregate compile coverage; every seeded header
still needs synopsis-level and cross-toolchain coverage.

### Absent C++23 headers

The following public headers do not exist. Compatibility/deprecated headers
remain required when C++23 still specifies them; they are not silently dropped.

| Area                     | Absent headers                                                                                                                                                     |
|--------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Text/encoding            | `<codecvt>`, `<regex>`; `<text_encoding>` is not C++23 and is therefore out of scope                                                                               |
| Errors/time/localization | `<locale>`, `<clocale>`                                                                                                                                            |
| I/O/formatting/files     | `<cstdio>`, `<fstream>`, `<iomanip>`, `<iostream>`, `<ostream>`, `<sstream>`, `<spanstream>`, `<strstream>`, `<syncstream>`, `<filesystem>`, `<format>`, `<print>` |
| Concurrency              | `<future>`                                                                                                                                                         |

Freestanding C compatibility also requires deciding and documenting how the
corresponding `.h` spellings are supplied. That is part of the relevant
header's completion gate, not a separate implementation wave.

## Dependency web

Arrows mean “must be complete before.” This is the implementation order, not
the include graph of one vendor STL.

```text
cstddef + initializer_list + compiler/platform layer
  -> cstdint + limits + type_traits
  -> utility + concepts + compare + new + exception
     -> iterator
        -> span
           -> mdspan
        -> ranges
           -> complete span range integration
        -> coroutine
           -> generator
        -> tuple
           -> expected + optional + variant + any
        -> memory
           -> scoped_allocator + memory_resource
           -> vector + deque + list + forward_list
           -> string
        -> functional
           -> associative containers
     -> execution

iterator + ranges + functional + execution
  -> algorithm + numeric

cstring + cwchar + type_traits
  -> string_view
  -> charconv
  -> string
     -> system_error
     -> locale
     -> regex
     -> formatting and I/O

ratio + limits
  -> chrono
     -> thread synchronization and timed waits

atomic + memory + functional + tuple + chrono + system_error
  -> stop_token + thread + mutex + shared_mutex + condition_variable
  -> semaphore + latch + barrier
  -> future

cmath + limits + concepts
  -> complex + numbers + random + valarray

string + system_error + chrono
  -> filesystem
  -> stacktrace

string + locale + tuple + variant
  -> format -> print

iosfwd + string + locale + exception
  -> ios + streambuf
  -> istream + ostream
  -> iostream + fstream + sstream + syncstream + iomanip
```

`<span>` has a usable core independent of `<ranges>`. Its C++23 generic range constructor, range deduction guide,
and range customizations remain deferred to the final Stage 2.6 integration pass,
after the `<ranges>`, `<mdspan>`, and `<generator>` milestones are available together.

Some headers form delivery closures and should be completed together:

- `<utility>` + `<type_traits>` + `<concepts>` + `<compare>`: constraints and
  customization behavior cross all four.
- `<iterator>` + `<ranges>` + `<algorithm>`: iterator concepts, CPOs, borrowed
  ranges, and constrained algorithms must agree.
- `<tuple>` + tuple protocol in `<array>` + `<utility>`: `get`, `tuple_size`,
  `tuple_element`, pair construction, and common-reference rules are coupled.
- `<string_view>` + character traits in `<string>` foundations + hashing:
  avoid inventing a second traits implementation.
- `<atomic>` + `<thread>` waiting primitives: wait/notify and platform blocking
  support need one runtime design.
- `<locale>` + streams: facets and stream formatting are a single large
  closure and should not leak partial public APIs between stages.
- `<mutex>` + `<shared_mutex>` + `<condition_variable>`: exclusive/shared
  ownership, timed blocking, condition waiting, stop-aware wakeup, and
  thread-exit notification share the same parking, deadline, and lifetime
  machinery and are completed as one synchronization closure.

## Staged completion plan

Only one stage is active at a time. Within a stage, pick the smallest listed
closure whose prerequisites are green, create its synopsis checklist before
changing code, finish it in both modes on all three compilers, then take the
next closure. Do not start a higher stage to obtain a more interesting
container.

### Stage 0 — Make completion enforceable

**Status: complete.**

- Add the MSVC/Clang/GCC build matrix for normal and replacement modes.
- Give every seeded header its own two test targets.
- Establish reusable compile-fail, feature-macro, freestanding-link, and
  allocation/runtime-dependency checks.
- Record the supported platform/architecture/exceptions/RTTI/threads matrix.
- Audit `FTL_REPLACE_STL` so a test cannot accidentally consume a vendor header.

**Exit:** CI can prove both usage modes without hosted-STL leakage, and every
new closure begins with a synopsis checklist in its tracking issue.

### Stage 1 — Complete the scalar foundation

**Status: complete.**

Take these closures in order:

1. `<cstddef>` + `<initializer_list>` + `<cstdint>`
2. `<type_traits>` + `<utility>` + `<concepts>` + `<compare>`
3. `<limits>` + `<bit>` + `<version>` + `<source_location>` + `<stdfloat>`
4. `<new>` + `<exception>` + `<typeinfo>` + `<typeindex>`
5. `<cassert>` + `<cerrno>` + `<cfloat>` + `<climits>` + `<cstdarg>` +
   `<csetjmp>` + `<csignal>` + `<cstdlib>` + `<cinttypes>`

**Exit:** all later headers can express their types, constraints, comparison,
allocation, and failure contracts without filling holes in Stage 1.

### Stage 2 — Iteration and vocabulary

**Status: complete.**

Take these closures in order:

1. `<iterator>`
2. `<array>` + `<tuple>`
3. `<functional>`
4. `<memory>` + `<scoped_allocator>` + `<memory_resource>`
5. `<optional>` + `<expected>` + `<variant>` + `<any>`
6. `<coroutine>` + `<ranges>` + `<span>` + `<mdspan>` + `<generator>`

`optional<T&>` remains tested as an extension and must not distort the C++23
`optional<T>` ABI or constraints.

**Exit:** generic code can traverse, invoke, own, allocate, and compose values
without depending on a container or I/O subsystem.

### Stage 3 — Algorithms and text core

**Status: complete.**

Take these closures in order:

1. `<execution>` + `<algorithm>` + `<numeric>` — **complete**
2. `<cstring>` + `<cwchar>` + `<cwctype>` + `<cuchar>` — **complete**
3. `<string_view>` + `<charconv>` — **complete**
4. `<string>` — **complete**
5. `<bitset>` — **complete**

Stage 3.1 completes the C++23 algorithm and numeric surfaces together with the
execution-policy vocabulary required by their policy overloads. Standard
execution policies may currently use a sequential implementation strategy;
their specified overload participation and execution-policy semantics remain
part of the completed interface.

Stage 3.2 completes the C++23 narrow and wide C string, character
classification, and Unicode conversion surfaces, together with their required
C compatibility header spellings. FTL currently models the C locale for
collation, classification, and multibyte conversion; hosted wide-stream
operations remain an explicit platform C-runtime boundary.

Stage 3.3 completes the C++23 non-owning string and primitive numeric
conversion surfaces. `<string_view>` includes the complete C++23
`basic_string_view` synopsis, character-traits integration, hashing, literals,
range construction, and comparison/search facilities. `<charconv>` provides
allocation-free integer and floating-point conversion, including exact
decimal and hexadecimal parsing, precision formatting, shortest-roundtrip
formatting, `long double`, and the extended floating-point types exposed by
FTL when supported by the compiler. Both headers advertise their completed
C++23 feature-test macros.

Stage 3.4 completes the C++23 owning string surface. `<string>` includes the
complete `basic_string` synopsis, allocator-aware storage with small-string
optimization, C++23 range construction and mutation, comparisons,
concatenation, hashing, literals, numeric conversions, and the applicable
stream declarations. It is constexpr-capable and validated in both normal and
`FTL_REPLACE_STL` modes while preserving freestanding linkage.

Stage 3.5 completes the C++23 fixed-size bit sequence surface. `<bitset>`
includes constexpr construction and operations, proxy references, checked
integer conversions, string and string-view conversion, hashing, and the
specified non-member and stream operators. It covers zero-sized and partial
storage, oversized shifts, parsing and conversion errors, and both namespace
modes while preserving freestanding linkage.

**Exit:** algorithms and owning/non-owning text are complete, allocation-aware,
and usable as dependencies by containers and diagnostics.

### Stage 4 — Containers

**Status: complete.**

Take these closures in order:

1. `<vector>` — **complete**
2. `<deque>` + `<queue>` + `<stack>` — **complete**
3. `<forward_list>` + `<list>` — **complete**
4. `<map>` + `<set>` — **complete**
5. `<unordered_map>` + `<unordered_set>` — **complete**
6. `<flat_map>` + `<flat_set>` — **complete**

Adaptors ship with their first complete underlying containers; ordered and
unordered families each share one internal tree/hash-table implementation,
without exposing speculative policy abstractions.

**Exit:** every C++23 container and adaptor passes allocator propagation,
exception-safety, iterator-invalidation, heterogeneous-lookup, `constexpr`
where required, and complexity checks.

### Stage 5 — Numerics, clocks, and errors

**Status: in progress.**

Take these closures in order:

1. `<ratio>` + `<chrono>` + `<ctime>` — **computational closure seeded**;
   `<ratio>` and `<ctime>` complete
2. `<system_error>` + `<stdexcept>` + `<stacktrace>` — **runtime closure
   complete**
3. `<cfenv>` + `<cmath>` + `<numbers>` + `<complex>` — **OpenLibm real/complex
   core and the Boost.Math-derived C++23 special functions complete; the N4950
   synopsis audit is complete, the local MSVC, ClangCL, GCC, and Clang matrix
   is green across all supported floating-point formats, and the Apple AArch64
   runtime core is CI-validated**
4. `<random>` + `<valarray>` — **computational closure complete; the N4950
   synopsis audit and the local MSVC, ClangCL, GCC, and Clang matrix are green
   in normal and `FTL_REPLACE_STL` modes, including freestanding linkage**

**Exit:** numeric and time facilities have specified edge behavior and do not
smuggle hosted runtime dependencies into supported freestanding builds.

Stage 5.1 provides the complete C++23 `<ratio>` synopsis and an independently
owned `<ctime>` surface, including FTL's own `tm`, UTC conversion, textual time
storage, and platform-width `time_t`. Its runtime-only operations cross a small
C runtime boundary instead of importing a vendor C++ standard-library ABI.

The seeded `<chrono>` surface covers durations, time points, calendars,
`hh_mm_ss`, system/steady/UTC/TAI/GPS/file clocks, clock conversions, and
literals. It advertises `__cpp_lib_chrono == 201611L` until the later C++20
surface is complete. Local MSVC and Clang-CL builds pass the normal and
`FTL_REPLACE_STL` test suites; native GCC, Clang, and AppleClang coverage remains
part of the final completion gate.

Shared machinery is kept private: `detail/time_core.hpp` owns civil-calendar
arithmetic, `detail/time_storage.hpp` owns `<ctime>`'s thread-local result
storage, and `detail/clock_runtime.hpp` owns platform system/steady clock reads.
Future concurrency headers should consume the detail clock boundary directly
when they need timed waits, without including the full public `<chrono>` header.

`<chrono>` timezone database support is deferred: tzdb is a separate, large
hosted-data undertaking. After Stage 7 completes `<locale>`, `<format>`, and the
stream headers, return to `<chrono>` to add its formatters, stream insertion,
and `from_stream` interfaces. Only then, and after the separate tzdb closure,
can `<chrono>` move from seeded to complete.

Stage 5.2 completes the independently usable C++23 `<system_error>` and
`<stdexcept>` surfaces and the runtime `<stacktrace>` closure. Error codes,
conditions, categories, portable `errc` mappings, `system_error`, comparisons,
and rapid hashes follow N4950; category messages preserve `errno`. Stacktraces
capture native frame addresses on Windows and POSIX hosts, honor allocator and
skip/depth behavior, and provide the specified container, comparison, string,
PMR, and hash interfaces. Native symbol/source lookup is allowed to report no
information and currently does so.

The `<stacktrace>` stream insertion and formatter specializations depend on
Stage 7's `<ostream>` and `<format>` closures. They, and the
`__cpp_lib_stacktrace` advertisement, remain deferred until those headers are
complete; the runtime surface does not import vendor C++ library APIs.

Stage 5.3 starts with the freestanding, header-only C++23 `<numbers>` surface.
The remaining closure will be taken in dependency order:

1. Own `<fenv.h>` and `<cfenv>` surfaces over a private environment layer.
   x86-64 saves and restores both MXCSR and the x87 control/status state;
   AArch64 uses FPCR/FPSR. Compiler barriers surround environment mutations.
2. Adapt OpenLibm's MIT-licensed binary32/binary64 kernels and C99 complex
   functions into FTL-owned detail code, retaining upstream notices and file
   provenance. Add only the x86-64 and AArch64 selection needed by the
   supported matrix, plus x86 binary80 operations where `long double` uses it.
3. Build the `<cmath>` overload and classification surface over those kernels,
   then adapt the C++23 special mathematical functions from Boost.Math under
   the Boost Software License 1.0. Keep Boost policy/template machinery out of
   the public implementation unless a required function genuinely needs it.
4. Implement `<complex>` last: the value type and elementary arithmetic are
   local and header-only; transcendental functions route through the completed
   real kernels or adapted OpenLibm complex routines so branch cuts, signed
   zero, infinities, and NaNs have one source of truth.

Each imported source keeps its upstream license header and is listed in a
Stage 5.3 attribution file. Completion requires normal and `FTL_REPLACE_STL`
public-header checks, freestanding linkage, rounding-mode/exception tests, and
an accuracy suite measured in ULPs against high-precision reference vectors on
every supported floating-point format.

The N4950 audit covers the complete `<numbers>` and `<cfenv>` synopses,
`<cmath>` including arithmetic promotions, C++23 `constexpr` functions, and all
21 special-function families, and `<complex>` including conversion-rank and
mixed-arithmetic rules. The two `<complex>` stream operators remain staged with
Stage 7's stream closure so this freestanding header does not import an
incomplete I/O dependency.

Stage 5.4 provides the complete N4950 computational `<random>` and `<valarray>`
surfaces. Random engines match all nine mandated 10,000th values; distributions
have range, state, and statistical checks; and `valarray` covers owning arrays,
all four selector proxies, arithmetic, transcendental operations, and range
access. Random engine, adaptor, and distribution stream operators remain staged
with Stage 7's `<ostream>` closure, matching the existing `<complex>` boundary.
`random_device` uses native nondeterministic entropy on supported hosts:
`RtlGenRandom` on Windows, `getrandom` on x86-64 Linux, and `arc4random_buf` on
macOS. Only unknown freestanding targets retain the deterministic zero-entropy
fallback.

### Stage 6 — Concurrency

**Status: complete.**

Take these closures in order:

1. `<atomic>` + `<stdatomic.h>` — **complete**
2. `<stop_token>` + `<thread>` — **runtime closure complete**;
   `<stop_token>` complete, with `<thread>` stream insertion and formatting
   deferred to Stage 7
3. `<mutex>` + `<shared_mutex>` + `<condition_variable>` — **complete**
4. `<semaphore>` + `<latch>` + `<barrier>` — **complete**
5. `<future>` — **complete**

Stage 6.1 completes the C++23 atomic surface, including generic lock-free and
non-lock-free `atomic<T>`, `atomic_ref<T>`, integral, floating-point, pointer,
flag, smart-pointer, fence, wait/notify, free-function, feature-test, and
`<stdatomic.h>` compatibility facilities. Non-lock-free objects use an
address-striped blocking fallback over the same platform wait/wake layer used
by atomic waiting.

MSVC uses `WaitOnAddress`, Linux uses futex-backed waiting, and macOS uses its
native address-wait primitives. The implementation does not require a hosted
C++ standard-library atomic or synchronization implementation.

Clang currently provides no general primitive equivalent to GCC's
`__builtin_clear_padding` or MSVC's `__builtin_zero_non_value_bits`.
Consequently, on Clang toolchains without such a builtin, comparison of
otherwise lock-free atomic types containing padding cannot normalize
non-value bits before compare/exchange or wait. FTL treats this as a documented
compiler limitation rather than introducing a type-specific padding model.

Stage 6.2 provides the native runtime surface for `<stop_token>` and `<thread>`,
including stop states and callbacks, `thread`, `jthread`, thread IDs, hashing,
hardware-concurrency queries, yielding, sleeping, joining, detaching, and
stop-token integration. `<stop_token>` is complete. `<thread>` remains seeded
only because the C++23 `thread::id` stream insertion and formatter interfaces
depend on the Stage 7 stream and formatting closures.

The current CMake integration has one known feature-gating limitation on UNIX:
the `ftl` interface target links `Threads::Threads` unconditionally, even when
a consumer defines `FTL_NO_THREADS`. The macro still removes the thread API at
compile time, but consumer preprocessor definitions cannot currently remove
that already-declared transitive link requirement. Stage 8 will make runtime
link dependencies explicit per-target choices as part of the replacement
integration.

Stage 6.3 completes the C++23 synchronization surfaces for `<mutex>`,
`<shared_mutex>`, and `<condition_variable>`.

`<mutex>` provides ordinary, recursive, timed, and recursive timed mutexes;
the lock tags, `lock_guard`, `scoped_lock`, and `unique_lock`; generic
deadlock-avoiding `lock` and `try_lock`; and `once_flag`/`call_once`.

`<shared_mutex>` provides ordinary and timed shared mutexes together with
`shared_lock`, using FTL-owned reader/writer bookkeeping with writer-aware
blocking.

`<condition_variable>` provides `condition_variable`,
`condition_variable_any`, `cv_status`, predicate and timed waits,
stop-token-aware waits, and `notify_all_at_thread_exit`.

The synchronization implementation is FTL-owned rather than a wrapper around
a vendor C++ threading library. Mutexes, shared mutexes, and condition
variables build on the address-wait substrate introduced for `<atomic>`.
Windows uses `WaitOnAddress`, Linux uses futexes, and macOS uses native
address-wait operations. Timed waits share a private steady-clock deadline
layer, and public headers expose no platform synchronization ABI types.

Condition variables use generation counters over the same wait/wake machinery
to avoid lost notifications across the unlock-to-block transition.
`condition_variable_any` integrates with `<stop_token>` by registering a stop
callback that participates in the same generation protocol.

`notify_all_at_thread_exit` preserves the required ordering after destruction
of thread-storage-duration objects. Windows defers actions through a late PE
TLS callback ordered after dynamic TLS destruction. POSIX targets defer FTL
thread-exit actions through an additional pthread thread-specific-data
destructor pass so language TLS destruction completes before deferred mutex
unlock and notification.

Stage 6.4 completes the C++23 coordination facilities in `<semaphore>`,
`<latch>`, and `<barrier>`. They use the same FTL-owned atomic wait/wake and
deadline machinery as the earlier synchronization closures rather than
delegating to hosted standard-library primitives. Timed semaphore waits remain
deadline-aware, while latch and barrier coordination use FTL-owned atomic state
for waiter publication and phase transitions.

Stage 6.5 completes the C++23 `<future>` surface, including future error
vocabulary, `future`, `shared_future`, `promise`, `packaged_task`, launch
policies, and `async`, together with reference and `void` specializations,
allocator-aware promise construction, packaged-task deduction guides, and the
specified move-only ownership model.

Future shared states are FTL-owned and synchronize publication through the
existing atomic wait/wake substrate. Provider operations serialize competing
attempts to satisfy a state, broken promises publish the required stored
exception, and `future::get()` consumes its association even when retrieval
rethrows or moving the stored value fails.

Promise and packaged-task `_at_thread_exit` operations reuse the common
thread-exit runtime introduced by the synchronization closures. Results are
stored without being published and become ready only after the producing
thread's C++ thread-local destruction sequence has completed.

`async` supports eager `launch::async`, lazy `launch::deferred`, and the
combined default policy. The asynchronous path runs on FTL's native thread
implementation, while mixed-policy thread-creation failure may fall back to
deferred execution when the failure is `resource_unavailable_try_again`.
Deferred invocation executes exactly once on the first non-timed waiter.

Asynchronous shared states separately track user-visible future references from
internal implementation ownership. Releasing the final future or
`shared_future` associated with an asynchronous invocation therefore performs
the required completion synchronization, while releasing a non-final
`shared_future` does not spuriously block. Timed waits remain deadline-aware
through asynchronous thread completion, including thread-local destruction,
rather than transitioning to an unbounded join after the result itself becomes
ready.

Focused `<future>` coverage exercises value, reference, and `void` states;
exceptions and broken promises; one-shot retrieval; repeated shared-future
access; concurrent provider satisfaction; allocator-aware promises;
thread-exit publication and TLS ordering; packaged-task invocation, reset,
move-only callables, and deduction; and asynchronous/deferred execution,
lifetime, timing, exception, and destruction semantics in both normal and
replacement modes. N4950 defines no `<future>` feature-test macro, so the
completed facility does not introduce one.

**Exit:** all Stage 6 concurrency runtime closures are complete and pass the
supported compiler/platform matrix in normal and replacement modes. Remaining
`<thread>` stream insertion and formatter interfaces are intentionally deferred
to Stage 7 because they depend on the formatting and I/O closures rather than
on missing concurrency runtime functionality.

### Stage 7 — Localization, formatting, and I/O

Take these closures in order:

1. `<locale>` + `<clocale>` + deprecated `<codecvt>`
2. `<format>` + `<print>`
3. `<iosfwd>` + `<ios>` + `<streambuf>` + `<istream>` + `<ostream>`
4. Return to `<chrono>` for formatters, stream insertion, and `from_stream`.
5. `<iostream>` + `<fstream>` + `<sstream>` + `<spanstream>` +
   `<syncstream>` + `<iomanip>` + `<cstdio>` + deprecated `<strstream>`
6. `<filesystem>`
7. `<regex>`

This stage is deliberately late: it has the widest dependency surface and the
largest hosted-runtime boundary.

**Exit:** all remaining C++23 headers are complete, and the documented
freestanding subset fails clearly where the platform cannot provide a hosted
facility.

### Stage 8 — Replacement product and ABI

- Implement `ftl_replace_stl()` as a transitive per-target CMake choice.
- Make optional runtime link dependencies feature-sensitive per target instead
  of unconditional interface requirements such as `Threads::Threads`.
- Define the runtime library boundary for exceptions, allocation, threads,
  locale, I/O, and filesystem.
- Publish ABI versioning and supported interoperability rules.
- Add whole-program replacement examples, sanitizers, fuzzing, and ABI/layout
  regression checks.

**Exit:** a non-trivial application and its source-built dependency graph can
select FTL without accidental vendor-STL mixing.

## Rules for changing the order

A stage may be split into smaller closures, but a later stage may move earlier
only when:

1. Its complete transitive dependency closure is already green.
2. The move does not require a temporary public API or hosted-STL dependency.
3. The moved work can still satisfy the normal/replacement and three-compiler
   completion gates in one change series.

When blocked, fix the lowest incomplete dependency rather than adding a local
workaround. This keeps the graph converging instead of creating multiple eras
of half-compatible facilities.

## After C++23 completion

Only after Stage 8:

- Adopt later standards as explicit, separately tracked deltas.
- Add opt-in game/system facilities such as fixed-capacity containers,
  allocation-free variants, measured hash/container layouts, and removable
  debug hardening.

Extensions must not compromise the standard-compatible interface or complicate
replacement mode. Speculative abstractions and unmeasured divergences remain
out of scope.

## Non-goals

- Binary compatibility with MSVC STL, libstdc++, or libc++.
- Passing FTL standard-library objects across incompatible prebuilt ABI
  boundaries.
- Mixing partial FTL replacements with vendor definitions in one translation
  unit.
- Advancing by C++ publication era while lower dependency layers remain
  incomplete.
- Claiming header completion from smoke tests or file presence.
