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

| Header               | Completed closure |
|----------------------|-------------------|
| `<cstddef>`          | Stage 1.1         |
| `<cstdint>`          | Stage 1.1         |
| `<initializer_list>` | Stage 1.1         |
| `<type_traits>`      | Stage 1.2         |
| `<utility>`          | Stage 1.2         |
| `<concepts>`         | Stage 1.2         |
| `<compare>`          | Stage 1.2         |
| `<limits>`           | Stage 1.3         |
| `<bit>`              | Stage 1.3         |
| `<version>`          | Stage 1.3         |
| `<source_location>`  | Stage 1.3         |
| `<stdfloat>`         | Stage 1.3         |
| `<new>`              | Stage 1.4         |
| `<exception>`        | Stage 1.4         |
| `<typeinfo>`         | Stage 1.4         |
| `<typeindex>`        | Stage 1.4         |
| `<cassert>`          | Stage 1.5         |
| `<cerrno>`           | Stage 1.5         |
| `<cfloat>`           | Stage 1.5         |
| `<climits>`          | Stage 1.5         |
| `<cstdarg>`          | Stage 1.5         |
| `<csetjmp>`          | Stage 1.5         |
| `<csignal>`          | Stage 1.5         |
| `<cstdlib>`          | Stage 1.5         |
| `<cinttypes>`        | Stage 1.5         |
| `<iterator>`         | Stage 2.1         |
| `<functional>`       | Stage 2.3         |
| `<memory>`           | Stage 2.4         |
| `<scoped_allocator>` | Stage 2.4         |
| `<memory_resource>`  | Stage 2.4         |
| `<optional>`         | Stage 2.5         |
| `<expected>`         | Stage 2.5         |
| `<variant>`          | Stage 2.5         |
| `<any>`              | Stage 2.5         |
| `<coroutine>`        | Stage 2.6.1       |
| `<generator>`        | Stage 2.6.4       |
| `<mdspan>`           | Stage 2.6.5       |
| `<span>`             | Stage 2.6.6       |
| `<ranges>`           | Stage 2.6.6       |
| `<array>`            | Stage 2.6.6       |
| `<tuple>`            | Stage 2.6.6       |
| `<execution>`        | Stage 3.1         |
| `<algorithm>`        | Stage 3.1         |
| `<numeric>`          | Stage 3.1         |
| `<cstring>`          | Stage 3.2         |
| `<cwchar>`           | Stage 3.2         |
| `<cwctype>`          | Stage 3.2         |
| `<cuchar>`           | Stage 3.2         |

Compiler coroutine syntax integrates with FTL only in FTL_REPLACE_STL mode
because coroutine transformation performs lookup through std::coroutine_traits.
Normal namespace mode still provides and tests the library types directly.

### Seeded headers

The following public headers exist but remain incomplete against their C++23
synopses:

| Header          | Implemented direction                                                                                            | Major remaining groups                                                                                             |
|-----------------|------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------|
| `<atomic>`      | integral atomics and memory orders                                                                               | generic/pointer atomics, `atomic_ref`, flag, fences, wait/notify, lock-free rules                                  |
| `<string_view>` | core `basic_string_view`, `char_traits` integration, and standard `out_of_range` behavior for checked operations | remaining searches, iterators/literals, I/O and hash integration, plus complete C++23 synopsis coverage            |
| `<stdexcept>`   | standard exception hierarchy, owned C-string messages, non-throwing copy operations                              | `basic_string` constructors, full synopsis and ABI/behavior audit                                                  |
| `<iosfwd>`      | `char_traits`, fundamental stream types, aliases, and stream-class forward declarations                          | complete C++23 forward-declaration and positioning-type inventory                                                  |
| `<streambuf>`   | input/get-area stream-buffer machinery needed by seeded input streams                                            | put area, positioning/seeking, locale, synchronization, putback, and complete synopsis                             |
| `<ios>`         | stream state, buffer association, state observers, and boolean conversion                                        | full `ios_base`/`basic_ios` formatting, locales, callbacks, ties, exception masks, and synopsis                    |
| `<istream>`     | core unformatted character input, bulk reads, stream-state propagation, and user-defined extraction integration  | sentry, formatted extraction, remaining unformatted overloads, positioning, synchronization, and complete synopsis |

`include/ftl/detail/rapidhash` is an implementation detail, not a standard
header or roadmap milestone.

Current test gaps are also explicit: `<type_traits>`, `<utility>`, and
`<concepts>` only receive aggregate compile coverage; every seeded header
still needs synopsis-level and cross-toolchain coverage.

### Absent C++23 headers

The following public headers do not exist. Compatibility/deprecated headers
remain required when C++23 still specifies them; they are not silently dropped.

| Area                      | Absent headers                                                                                                                                                     |
|---------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Concepts/types/vocabulary | `<bitset>`, `<ratio>`                                                                                                                                              |
| Algorithms/numerics       | `<numbers>`, `<random>`, `<valarray>`                                                                                                                              |
| Containers                | `<deque>`, `<flat_map>`, `<flat_set>`, `<forward_list>`, `<list>`, `<map>`, `<queue>`, `<set>`, `<stack>`, `<unordered_map>`, `<unordered_set>`, `<vector>`        |
| Text/encoding             | `<charconv>`, `<codecvt>`, `<string>`, `<regex>`; `<text_encoding>` is not C++23 and is therefore out of scope                                                     |
| Errors/time/localization  | `<chrono>`, `<system_error>`, `<stacktrace>`, `<locale>`, `<clocale>`, `<ctime>`                                                                                   |
| C numerics/text           | `<cfenv>`, `<cmath>`, `<complex>`                                                                                                                                  |
| I/O/formatting/files      | `<cstdio>`, `<fstream>`, `<iomanip>`, `<iostream>`, `<ostream>`, `<sstream>`, `<spanstream>`, `<strstream>`, `<syncstream>`, `<filesystem>`, `<format>`, `<print>` |
| Concurrency               | `<barrier>`, `<condition_variable>`, `<future>`, `<latch>`, `<mutex>`, `<semaphore>`, `<shared_mutex>`, `<stop_token>`, `<thread>`                                 |
| C compatibility           | `<stdatomic.h>`                                                                                                                                                    |

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
  -> stop_token + thread + mutex + condition_variable
  -> semaphore + latch + barrier + shared_mutex
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

**Status: active. Closures 1–2 complete.**

Take these closures in order:

1. `<execution>` + `<algorithm>` + `<numeric>` — **complete**
2. `<cstring>` + `<cwchar>` + `<cwctype>` + `<cuchar>` — **complete**
3. `<string_view>` + `<charconv>`
4. `<string>`
5. `<bitset>`

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

**Exit:** algorithms and owning/non-owning text are complete, allocation-aware,
and usable as dependencies by containers and diagnostics.

### Stage 4 — Containers

Take these closures in order:

1. `<vector>`
2. `<deque>` + `<queue>` + `<stack>`
3. `<forward_list>` + `<list>`
4. `<map>` + `<set>`
5. `<unordered_map>` + `<unordered_set>`
6. `<flat_map>` + `<flat_set>`

Adaptors ship with their first complete underlying containers; ordered and
unordered families each share one internal tree/hash-table implementation,
without exposing speculative policy abstractions.

**Exit:** every C++23 container and adaptor passes allocator propagation,
exception-safety, iterator-invalidation, heterogeneous-lookup, `constexpr`
where required, and complexity checks.

### Stage 5 — Numerics, clocks, and errors

Take these closures in order:

1. `<ratio>` + `<chrono>` + `<ctime>`
2. `<system_error>` + `<stdexcept>` + `<stacktrace>`
3. `<cfenv>` + `<cmath>` + `<numbers>` + `<complex>`
4. `<random>` + `<valarray>`

**Exit:** numeric and time facilities have specified edge behavior and do not
smuggle hosted runtime dependencies into supported freestanding builds.

### Stage 6 — Concurrency

Take these closures in order:

1. Finish `<atomic>` including wait/notify and the platform blocking layer,
   plus its C++23 `<stdatomic.h>` compatibility surface.
2. `<stop_token>` + `<thread>`
3. `<mutex>` + `<shared_mutex>` + `<condition_variable>`
4. `<semaphore>` + `<latch>` + `<barrier>`
5. `<future>`

**Exit:** the full memory model and synchronization API pass stress, sanitizer,
and platform shutdown/lifetime tests on every supported toolchain.

### Stage 7 — Localization, formatting, and I/O

Take these closures in order:

1. `<locale>` + `<clocale>` + deprecated `<codecvt>`
2. `<format>` + `<print>`
3. `<iosfwd>` + `<ios>` + `<streambuf>` + `<istream>` + `<ostream>`
4. `<iostream>` + `<fstream>` + `<sstream>` + `<spanstream>` +
   `<syncstream>` + `<iomanip>` + `<cstdio>` + deprecated `<strstream>`
5. `<filesystem>`
6. `<regex>`

This stage is deliberately late: it has the widest dependency surface and the
largest hosted-runtime boundary.

**Exit:** all remaining C++23 headers are complete, and the documented
freestanding subset fails clearly where the platform cannot provide a hosted
facility.

### Stage 8 — Replacement product and ABI

- Implement `ftl_replace_stl()` as a transitive per-target CMake choice.
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
