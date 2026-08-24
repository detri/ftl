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
compiler, ABI, locale-data, codec, extended-floating, and platform-ABI blockers
in the final table.

Windows closure was re-certified on 2026-08-24 with the canonical Release
MSVC and Clang-CL configurations. Each compiler completed its full build and
passed all 388 tests in both normal and `FTL_REPLACE_STL` modes, including the
freestanding linkage and feature-macro checks. Linux GCC/Clang and native Apple
validation remain external to this Windows re-certification.

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

| Audit unit                              | Status               | Succinct remediation                                                                                                                                                                                                   |
|-----------------------------------------|----------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `<version>`                             | REMEDIATED           | Removed the stale `__cpp_lib_uses_allocator_construction` advertisement.                                                                                                                                               |
| `<limits>`                              | REMEDIATED           | Corrected integral `traps` and extended signaling-NaN representation.                                                                                                                                                  |
| `<initializer_list>`                    | REMEDIATED           | Avoided null-pointer subtraction for an empty MSVC initializer list.                                                                                                                                                   |
| `<exception>`                           | REMEDIATED           | Added the missing `bad_exception::what()` override in MSVC replacement mode.                                                                                                                                           |
| `<compare>`                             | REMEDIATED           | Made the third partial-fallback `<` expression a validity check and enforced boolean-testable synthesized ordering.                                                                                                    |
| `<system_error>`                        | REMEDIATED           | Added `operator<<` for `error_code` and the declared `system_error::what()` override.                                                                                                                                  |
| `<utility>`                             | REMEDIATED           | Enforced `integer_sequence`'s integer Mandate, restored cmp Mandates, and repaired synthesized ordering.                                                                                                               |
| `<tuple>`                               | REMEDIATED           | Certified existing `subrange` tuple-like support; restored exact comparison rules, direct non-materializing `tuple_cat`, and deleted dangling conversions.                                                             |
| `<optional>`                            | REMEDIATED           | Narrowed optional-value exclusions, restored Mandates, required the exact `or_else` result, and added reversed equality.                                                                                               |
| `<variant>`                             | REMEDIATED           | Closed tag and assignment guarantees, `visit<R>` and zero-variant visit, operator-specific relational dispatch (including independent `!=`), duplicate `get_if`, and invalid/non-destructible alternative Mandates.    |
| `<any>`                                 | REMEDIATED           | Permitted assignment of copyable in-place-tag values without routing through the tag-excluding constructor.                                                                                                            |
| `<expected>`                            | REMEDIATED           | Rejected invalid and non-destructible `T`/`E` (including nested `unexpected` errors), exposed monadic constraints in immediate context, and required implicitly-bool equality results.                                 |
| `<functional>`                          | REMEDIATED           | Completed `move_only_function`'s synopsis, null and empty-wrapper behavior, in-place construction, both callable forms, exclusions, immovable targets, and allocation-free function-pointer/reference-wrapper targets. |
| `<string>`                              | REMEDIATED + BLOCKED | Guarded throwing storage copies and made SSO swap traits-independent; numeric output remains blocked below.                                                                                                            |
| `<cwchar>`                              | BLOCKED              | Parsing/collation/time and wide-file conversion require the shared locale codec described below.                                                                                                                       |
| `<cuchar>`                              | BLOCKED              | Staged `size_t(-3)` UTF output depends on the shared restartable locale codec described below.                                                                                                                         |
| `<array>`                               | CERTIFIED            | Closed with the shared synthesized-three-way repair.                                                                                                                                                                   |
| `<vector>`                              | REMEDIATED           | Propagated equal-but-stateful allocators, made self-range append safe, and used copy fallback during reallocating middle insertion.                                                                                    |
| `<deque>`                               | REMEDIATED           | Propagated equal-but-stateful allocators and closed shared ordering; its `noexcept(is_always_equal)` signatures remain correct.                                                                                        |
| `<forward_list>` / `<list>`             | REMEDIATED           | Propagated equal-but-stateful allocators and closed shared ordering; their move/swap exception specifications remain correct.                                                                                          |
| `<set>` / `<multiset>`                  | REMEDIATED           | Removed over-`noexcept` tree move construction and closed shared ordering.                                                                                                                                             |
| `<map>` / `<multimap>`                  | REMEDIATED           | Honored hints in `try_emplace` / `insert_or_assign` to meet the well-positioned complexity bound.                                                                                                                      |
| `<queue>` / `priority_queue`, `<stack>` | CERTIFIED            | Closed after default `deque` / `vector` remediation.                                                                                                                                                                   |
| `<flat_set>` / `<flat_multiset>`        | CERTIFIED            | Historical bulk-insertion/guide repairs hold and shared ordering is closed; unconditional swap `noexcept` is ISO-correct.                                                                                              |
| `<flat_map>` / `<flat_multimap>`        | CERTIFIED            | Closed with shared ordering and default-vector remediation; unconditional swap `noexcept` is ISO-correct.                                                                                                              |
| `<iterator>` / range access             | REMEDIATED           | Gave normal and replacement `basic_const_iterator::operator<=>` the same direct constrained surface.                                                                                                                   |
| `<ranges>`                              | CERTIFIED            | Closed with the iterator mode-consistency repair; no independent defect remained.                                                                                                                                      |
| `<algorithm>`                           | CERTIFIED            | The required `ranges::minmax_result` alias was already present; the comparison dependency is closed.                                                                                                                   |
| `<numeric>`                             | REMEDIATED           | Implemented pointer `midpoint` without a potentially unrepresentable `b-a`.                                                                                                                                            |
| `<memory>` ownership                    | REMEDIATED + BLOCKED | Preserved an effective raw-pointer `shared_ptr` cleanup deleter until control-block commit; normal-mode constexpr allocation remains blocked below.                                                                    |
| `<cstdlib>` / `<stdlib.h>`              | REMEDIATED           | Gave Apple's namespace `abort` the required `noexcept` function type.                                                                                                                                                  |
| `<cmath>` core                          | BLOCKED              | A genuinely fused constant-evaluated `fma` needs a compiler intrinsic or software correctly-rounded backend; extended math remains blocked below.                                                                      |

## Known blockers and realistic paths

| Audit unit                                             | Path forward                                                                                                                                                                                                                  |
|--------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `<typeinfo>`                                           | Own or bridge the MSVC RTTI exception ABI so compiler-generated and FTL `bad_cast` / `bad_typeid` share one identity; until then certify the unaffected modes only.                                                           |
| `<string>` numeric output                              | Use dynamically sized `sprintf` / `swprintf`-equivalent formatting through the selected C locale rather than locale-independent `to_chars`.                                                                                   |
| `<cwchar>`, `<cuchar>`, `<iostream>`, `<cstdio>`       | Build one FTL-owned, locale-snapshotted restartable codec with per-stream/state-object `mbstate_t`; use native locale/iconv paths on POSIX and an owned Windows code-page state machine instead of `_mbtowc_l` / `_wctomb_l`. |
| `<generator>`                                          | Add a compiler/runtime coroutine-traits bridge for normal-mode range `elements_of`; replacement mode already owns the needed traits.                                                                                          |
| `<memory>` constexpr allocator                         | Seek compiler recognition for `ftl::allocator` or document normal-mode exclusion; the language privilege currently applies specifically to `std::allocator`, so replacement mode is the conforming path.                      |
| `<complex>`, `<cmath>` core and special functions      | Add per-extended-type runtime backends (for example binary128 via compiler-rt/libquadmath where available) and decline unsupported types explicitly rather than narrowing through `long double`.                              |
| `<chrono>` formatting/parsing, `<locale>`, `<iomanip>` | Generate and own locale data for composite date/time patterns, eras, and alternative digits; drive all four surfaces from that common grammar/data layer.                                                                     |
| `<filesystem>`                                         | Take a versioned Windows ABI break to native-wide `path::value_type` / `string_type` and `L'\\'`, retaining UTF-8 APIs as explicit conversion extensions.                                                                     |

The ordinary remediation ledger is now clear. The next practical phase is the
shared locale/codec/data runtime, which closes the largest blocker cluster plus
string numeric output. Extended floating support and the Windows filesystem
ABI are bounded follow-on projects. RTTI, pre-intrinsic constant-evaluated
`fma`, normal-mode allocator constant evaluation, and generator integration may
require compiler cooperation and remain explicitly mode-qualified.
