# C++23 Fresh Conformance Audit

**Baseline:** ISO/IEC 14882:2024  
**Source audited:** working tree based on `b05f17ccc5d3e9f1dcb56f99f188b67e0ae739d5`  
**Audit date:** 2026-08-24

This ledger supersedes the status table in `docs/conformance.md`. Each unit was
re-read against the local ISO text and the current public header and supporting
implementation; prior remediation notes and tests were corroborating evidence,
not certification by themselves. Named library modules remain outside the
source-interface product boundary explicitly stated in `ROADMAP.md`.

The fresh findings were remediated against the ISO/IEC 14882:2024 text in
`C:\Users\Aaron\Documents\CppStandards`. All ordinary remediation and
dependency rows below now certify. The remaining limitations are the explicit
extended-floating blocker in the final table. Earlier mode-qualified compiler
and ABI limitations applied only to the discontinued parallel `ftl` namespace
mode.

The replacement-only Windows closure was re-certified on 2026-08-26 with the
canonical Release MSVC and Clang-CL configurations. Each compiler completed its
full build and passed all 197 tests, including the freestanding linkage and
feature-macro checks. Native WSL2 GCC and Clang likewise completed their full
builds and passed all 197 tests. AppleClang remains covered by the CI matrix.

## Certified

| Area                          | Certified audit units                                                                                                                                                                                                                                                                                 |
|-------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Language and C compatibility  | `<cstddef>`, `<climits>`, `<cstdint>`, `<cfloat>`, `<stdfloat>`, `<cstdarg>`, `<csignal>` / `<signal.h>`, `<cerrno>` / `<errno.h>`, `<new>`, `<source_location>`, `<coroutine>`, `<csetjmp>` / `<setjmp.h>`, `<iso646.h>`, `<stdalign.h>` / `<stdbool.h>`, `<cassert>` / `<assert.h>`                 |
| Metaprogramming and utilities | `<concepts>`, `<type_traits>`, `<ratio>`, `<bitset>`, `<typeindex>`, `<execution>`, `<charconv>`, `<format>` core/ranges, `<bit>`                                                                                                                                                                     |
| Text                          | `<string_view>` / character traits, `<cctype>`, `<cwctype>`, `<cstring>`                                                                                                                                                                                                                              |
| Containers                    | `<unordered_set>` / `<unordered_multiset>`, `<unordered_map>` / `<unordered_multimap>`, `<span>`, `<mdspan>`                                                                                                                                                                                          |
| Memory                        | `<memory>` specialized algorithms, `<memory_resource>`, `<scoped_allocator>`                                                                                                                                                                                                                          |
| Numerics and time             | `<cfenv>` / `<fenv.h>`, `<random>`, `<valarray>`, `<numbers>`; `<chrono>` core durations/clocks, civil calendar/`hh_mm_ss`, clock conversion, time-zone database/`zoned_time`                                                                                                                         |
| Localization and I/O          | `<codecvt>`, `<clocale>` / `<locale.h>`, `<iosfwd>` / `<ios>`, `<streambuf>`, `<istream>` / `<ostream>`, `<print>`, `<sstream>`, `<strstream>`, `<spanstream>`, `<fstream>`, `<syncstream>`                                                                                                           |
| Hosted and concurrency        | `<cinttypes>` / `<inttypes.h>`, `<ctime>` / `<time.h>`, C ABI support units, `<regex>`, `<atomic>` / `<stdatomic.h>`, `<stop_token>`, `<thread>` / `jthread`, `<mutex>` / `<shared_mutex>` / `<condition_variable>`, `<semaphore>`, `<latch>`, `<barrier>`, `<future>`, `<stdexcept>`, `<stacktrace>` |

## Remediation results

| Audit unit                              | Status     | Succinct remediation                                                                                                                                                                                                   |
|-----------------------------------------|------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `<version>`                             | REMEDIATED | Removed the stale `__cpp_lib_uses_allocator_construction` advertisement.                                                                                                                                               |
| `<limits>`                              | REMEDIATED | Corrected integral `traps` and extended signaling-NaN representation.                                                                                                                                                  |
| `<initializer_list>`                    | REMEDIATED | Avoided null-pointer subtraction for an empty MSVC initializer list.                                                                                                                                                   |
| `<exception>`                           | REMEDIATED | Added the missing `bad_exception::what()` override for the MSVC standard-library configuration.                                                                                                                        |
| `<compare>`                             | REMEDIATED | Made the third partial-fallback `<` expression a validity check and enforced boolean-testable synthesized ordering.                                                                                                    |
| `<system_error>`                        | REMEDIATED | Added `operator<<` for `error_code` and the declared `system_error::what()` override.                                                                                                                                  |
| `<utility>`                             | REMEDIATED | Enforced `integer_sequence`'s integer Mandate, restored cmp Mandates, and repaired synthesized ordering.                                                                                                               |
| `<tuple>`                               | REMEDIATED | Certified existing `subrange` tuple-like support; restored exact comparison rules, direct non-materializing `tuple_cat`, and deleted dangling conversions.                                                             |
| `<optional>`                            | REMEDIATED | Narrowed optional-value exclusions, restored Mandates, required the exact `or_else` result, and added reversed equality.                                                                                               |
| `<variant>`                             | REMEDIATED | Closed tag and assignment guarantees, `visit<R>` and zero-variant visit, operator-specific relational dispatch (including independent `!=`), duplicate `get_if`, and invalid/non-destructible alternative Mandates.    |
| `<any>`                                 | REMEDIATED | Permitted assignment of copyable in-place-tag values without routing through the tag-excluding constructor.                                                                                                            |
| `<expected>`                            | REMEDIATED | Rejected invalid and non-destructible `T`/`E` (including nested `unexpected` errors), exposed monadic constraints in immediate context, and required implicitly-bool equality results.                                 |
| `<functional>`                          | REMEDIATED | Completed `move_only_function`'s synopsis, null and empty-wrapper behavior, in-place construction, both callable forms, exclusions, immovable targets, and allocation-free function-pointer/reference-wrapper targets. |
| `<string>`                              | REMEDIATED | Guarded throwing storage copies, made SSO swap traits-independent, and routed numeric output through dynamically sized current-C-locale formatting.                                                                    |
| `<cwchar>`                              | REMEDIATED | Routed parsing, collation, time, and wide-file conversion through snapshotted native locale handles and restartable per-state conversion.                                                                              |
| `<cuchar>`                              | REMEDIATED | Added restartable locale conversion plus the required staged `size_t(-3)` UTF-8 and UTF-16 code-unit output.                                                                                                           |
| `<array>`                               | CERTIFIED  | Closed with the shared synthesized-three-way repair.                                                                                                                                                                   |
| `<vector>`                              | REMEDIATED | Propagated equal-but-stateful allocators, made self-range append safe, and used copy fallback during reallocating middle insertion.                                                                                    |
| `<deque>`                               | REMEDIATED | Propagated equal-but-stateful allocators and closed shared ordering; its `noexcept(is_always_equal)` signatures remain correct.                                                                                        |
| `<forward_list>` / `<list>`             | REMEDIATED | Propagated equal-but-stateful allocators and closed shared ordering; their move/swap exception specifications remain correct.                                                                                          |
| `<set>` / `<multiset>`                  | REMEDIATED | Removed over-`noexcept` tree move construction and closed shared ordering.                                                                                                                                             |
| `<map>` / `<multimap>`                  | REMEDIATED | Honored hints in `try_emplace` / `insert_or_assign` to meet the well-positioned complexity bound.                                                                                                                      |
| `<queue>` / `priority_queue`, `<stack>` | CERTIFIED  | Closed after default `deque` / `vector` remediation.                                                                                                                                                                   |
| `<flat_set>` / `<flat_multiset>`        | CERTIFIED  | Historical bulk-insertion/guide repairs hold and shared ordering is closed; unconditional swap `noexcept` is ISO-correct.                                                                                              |
| `<flat_map>` / `<flat_multimap>`        | CERTIFIED  | Closed with shared ordering and default-vector remediation; unconditional swap `noexcept` is ISO-correct.                                                                                                              |
| `<iterator>` / range access             | REMEDIATED | Gave `basic_const_iterator::operator<=>` the required direct constrained surface.                                                                                                                                      |
| `<ranges>`                              | CERTIFIED  | Closed with the iterator repair; no independent defect remained.                                                                                                                                                       |
| `<algorithm>`                           | CERTIFIED  | The required `ranges::minmax_result` alias was already present; the comparison dependency is closed.                                                                                                                   |
| `<numeric>`                             | REMEDIATED | Implemented pointer `midpoint` without a potentially unrepresentable `b-a`.                                                                                                                                            |
| `<memory>` ownership                    | REMEDIATED | Preserved an effective raw-pointer `shared_ptr` cleanup deleter until control-block commit; replacement-only `std::allocator` has the required compiler constexpr-allocation treatment.                                |
| `<cstdlib>` / `<stdlib.h>`              | REMEDIATED | Wrapped Apple's non-`noexcept` global C `abort` declaration with the required `noexcept` namespace function.                                                                                                           |
| `<cmath>` core                          | REMEDIATED | Added exact extended-type overloads, native binary128 runtime dispatch, mixed-rank overload selection, and explicit rejection of extended `nexttoward`; constant evaluation uses compiler builtins where available.   |
| `<filesystem>`                          | REMEDIATED | Changed the Windows ABI to native-wide `path::value_type`, `string_type`, and `L'\\'`, while retaining explicit UTF-8 conversion APIs and native-wide filesystem/stream I/O.                                           |

## Closed blocker

| Audit unit                                        | Resolution                                                                                                                                                                                                                |
|---------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `<complex>`, `<cmath>` core and special functions | Added exact overloads for each compiler-advertised extended type. Binary128 uses native compiler/libm entry points; narrower interchange types evaluate in a non-narrowing standard type, and unsupported calls are rejected. |

The ordinary remediation ledger is now clear, including extended floating
support across the compiler-advertised C++23 interchange types.
The former MSVC RTTI identity, generator integration, and constexpr allocator
limitations disappear with removal of the parallel `ftl` namespace mode.
