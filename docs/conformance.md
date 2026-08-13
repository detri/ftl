# C++23 Conformance Ledger

This document tracks exhaustive C++23 conformance review of FTL independently
from implementation progress in `ROADMAP.md`.

The normative baseline is **ISO/IEC 14882:2024**.

The initial audit baseline is:

```text
main@798e52c498f5630ecdc03c92456b99a1ff95aa97
```

A header being implemented, tested, or previously examined does not imply that
it is certified here.

## Status definitions

- **UNREVIEWED** — no exhaustive conformance audit has been completed.
- **AUDITING** — an exhaustive audit is currently in progress.
- **FAILED** — the audit completed and one or more known conformance defects
  remain.
- **CERTIFIED** — every applicable requirement in the audited scope was
  compared against FTL and no known divergence remains.
- **INVALIDATED** — previously certified, but later changes touched the header
  or a dependency relevant to its certification.

Headers not listed in the tables below are **UNREVIEWED**.

Certification is tied to the listed commit. A later change does not
automatically invalidate a certification unless it modifies the certified
facility or a dependency on which the certification relied.

## Certification requirements

Before a header or audit unit can be marked **CERTIFIED**, the review must
cover every applicable requirement, including:

- header synopsis and namespace placement
- overload sets and deduction guides
- template parameters and default arguments
- constraints, mandates, and overload participation
- `constexpr`, `consteval`, `explicit`, cv/ref qualifiers, and conditional
  `noexcept`
- Effects, Returns, Postconditions, Remarks, and observable state transitions
- exception guarantees
- complexity requirements
- allocator semantics where applicable
- customization points and ADL behavior where applicable
- required specializations
- iterator/range invalidation behavior where applicable
- feature-test macros
- freestanding/hosted requirements
- repeated-inclusion and header-interaction requirements
- normal FTL namespace mode
- `FTL_REPLACE_STL` mode

Source inspection is the primary conformance check. Tests provide evidence for
requirements whose behavior is better established mechanically, especially
overload participation, exception state, `constexpr`, `noexcept`, and
platform-specific behavior.

Finding a defect during an audit does not require fixing it immediately. Record
the defect, mark the unit **FAILED**, and continue remediation separately.

## Known failures from conformance pass 1

These facilities were already known to be nonconforming at
`798e52c498f5630ecdc03c92456b99a1ff95aa97`. They have not yet undergone their
dedicated exhaustive certification audit.

| Header        | Status     | Known issue summary                                                                                                   |
|---------------|------------|-----------------------------------------------------------------------------------------------------------------------|
| `<generator>` | **FAILED** | Normal-mode `elements_of` range `yield_value` implementation hole.                                                    |
| `<ostream>`   | **FAILED** | Missing C++23 ostream print family.                                                                                   |
| `<variant>`   | **FAILED** | Assignment exception semantics, participation constraints, `visit`, and other interface drift.                        |
| `<any>`       | **FAILED** | Incorrect `emplace` exception-state semantics and constructor constraint drift.                                       |
| `<optional>`  | **FAILED** | `or_else` participation constraints are incomplete.                                                                   |
| `<expected>`  | **FAILED** | `emplace`, `swap`, and monadic constraint/noexcept defects.                                                           |
| `<string>`    | **FAILED** | `resize_and_overwrite` invokes the operation with the wrong value category and accepts an overly broad result domain. |


These entries record known failures only. They do not mean the rest of each
header has been exhaustively audited.

## Large-header subdivision

Large facilities are certified incrementally rather than as one indivisible
header.

### `<ranges>`

Subunits will be added as they are audited, for example:

- range access
- range concepts and requirements
- range utilities
- range factories
- individual view/adaptor families
- range generators

`<ranges>` becomes **CERTIFIED** only when every required subunit is certified.

### `<algorithm>`

Subunits will follow the standard's organization, including:

- algorithm result types
- non-modifying sequence operations
- mutating sequence operations
- sorting and related operations
- ranges counterparts

`<algorithm>` becomes **CERTIFIED** only when every required subunit is
certified.

## Audit records

### `<cstddef>`

**Status:** FAILED  
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<cstddef>` synopsis
- `size_t`
- `ptrdiff_t`
- `max_align_t`
- `nullptr_t`
- `NULL`
- `offsetof`
- `byte`
- all byte shift and bitwise operations
- `to_integer`
- `__cpp_lib_byte`
- freestanding status
- repeated inclusion
- normal and replacement namespace modes

Confirmed matches:

- `size_t` is derived from the type of `sizeof`.
- `ptrdiff_t` is derived from the type of pointer subtraction.
- `nullptr_t` is `decltype(nullptr)`.
- `byte` is a scoped enumeration with underlying type `unsigned char`.
- `NULL` expands to a valid null pointer constant.
- `offsetof` has the required public macro surface; compiler-specific
  implementations provide the required offset behavior.
- byte operations have the required `constexpr` and `noexcept` properties.
- byte operation effects match the specified bitwise and shift behavior.
- `__cpp_lib_byte` is advertised as `201603L`.
- the header has no hosted-library dependency and is freestanding.
- include guards make repeated inclusion idempotent.

Known defect:

1. The shift operators and `to_integer` are constrained using a private
   `cstddef_integral` predicate that enumerates only the ordinary C++ integral
   types. The standard requires participation whenever
   `is_integral_v<IntType>` is true. On implementations where an extended
   integer type such as `__int128` satisfies `is_integral_v`, the standard
   overload participates while FTL's overload does not.

No remediation was performed as part of this audit.

### `<initializer_list>`

**Status:** FAILED  
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<initializer_list>` synopsis
- `initializer_list<E>` representation requirements
- `value_type`
- `reference`
- `const_reference`
- `size_type`
- `iterator`
- `const_iterator`
- default construction
- `size`
- `begin`
- `end`
- free `begin`
- free `end`
- compiler-facing initializer-list construction
- specialization prohibition
- signal-safety
- freestanding status
- repeated inclusion and vendor-header interaction
- normal and replacement namespace modes

Confirmed matches:

- `value_type` is `E`.
- `reference` and `const_reference` are `const E&`.
- `size_type` is `size_t`.
- `iterator` and `const_iterator` are `const E*`.
- default construction is `constexpr` and `noexcept` and produces an empty
  initializer list.
- `size()` is `constexpr`, `noexcept`, and constant time.
- `begin()` is `constexpr` and `noexcept` and returns the beginning of the
  compiler-provided backing array.
- `end()` is `constexpr` and `noexcept` and represents one-past-the-end.
- for an empty initializer list, `begin()` and `end()` are identical.
- free `begin(initializer_list<E>)` and `end(initializer_list<E>)` have the
  required signatures and forward to the corresponding members.
- both pointer-pair and pointer-plus-length internal representations permitted
  by the standard are used appropriately for the targeted compiler ABIs.
- the additional MSVC compiler-facing constructor is permitted implementation
  latitude under the standard-library member-function rules and does not alter
  standardized overload behavior.
- all specified operations are signal-safe.
- the entire facility is freestanding.
- include guards and vendor initializer-list guard interoperation make repeated
  inclusion idempotent.
- normal mode exposes the facility through `ftl` while retaining the required
  compiler-recognized `std::initializer_list` type.
- replacement mode exposes the required interface directly through `std`.

Known defect:

1. ISO 17.10.2 requires a program declaring an explicit or partial
   specialization of `initializer_list` to be ill-formed. FTL declares the
   class template without a compiler-supported no-specializations annotation or
   equivalent enforcement, so such declarations are not reliably diagnosed on
   the supported compilers.

No remediation was performed as part of this audit.

### `<cstdint>`

**Status:** FAILED  
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<cstdint>` synopsis
- exact-width signed and unsigned integer types
- least-width signed and unsigned integer types
- fast signed and unsigned integer types
- `intmax_t` and `uintmax_t`
- `intptr_t` and `uintptr_t`
- exact-width limit macros
- least-width limit macros
- fast-width limit macros
- greatest-width limit macros
- pointer-sized limit macros
- `PTRDIFF_MIN` and `PTRDIFF_MAX`
- `SIZE_MAX`
- `SIG_ATOMIC_MIN` and `SIG_ATOMIC_MAX`
- `WCHAR_MIN` and `WCHAR_MAX`
- `WINT_MIN` and `WINT_MAX`
- `INTN_C` and `UINTN_C`
- `INTMAX_C` and `UINTMAX_C`
- freestanding status
- repeated inclusion
- normal and replacement namespace modes
- interaction with implementation integer widths and data models

Confirmed matches:

- the required 8-, 16-, 32-, and 64-bit exact-width typedef-names are
  provided on the currently targeted implementations.
- least-width typedefs provide the required minimum widths.
- fast-width typedefs provide the required minimum widths on the currently
  exercised implementations.
- the non-MSVC implementation derives `intptr_t` and `uintptr_t` from the
  compiler's pointer-sized integer types.
- exact-width signed limit macros have the required values.
- `UINT32_MAX` and `UINT64_MAX` have the required values and expression types
  on the currently exercised 32-bit-int data models.
- `PTRDIFF_MIN`, `PTRDIFF_MAX`, and `SIZE_MAX` use compiler-provided limits
  outside the MSVC branch.
- the 8- and 16-bit `INTN_C` and `UINTN_C` macros correctly permit ordinary
  integer promotion to `int`.
- the facility is freestanding.
- include guards make repeated inclusion idempotent.
- normal and replacement namespace modes provide the intended FTL and
  standard-library surfaces respectively.

Known defects:

1. `UINT8_MAX` and `UINT16_MAX` are defined with an unsigned-int suffix.
   The C `<stdint.h>` requirements incorporated by C++23 require limit macro
   expressions to have the type obtained by integer promotion of the
   corresponding integer type. On implementations where `uint8_t` is
   `unsigned char` and `uint16_t` is `unsigned short` and both promote to
   `int`, these macros incorrectly have type `unsigned int`. The defect also
   affects `UINT_LEAST8_MAX`, `UINT_LEAST16_MAX`, and the MSVC
   `UINT_FAST8_MAX`.

2. `intmax_t`, `uintmax_t`, and their associated limit and constant macros
   are fixed to a 64-bit model. On implementation modes where a wider
   extended integer type such as `__int128` participates as an integer type,
   the greatest-width integer requirements are not met.

3. Pointer-sized aliases and/or their limit macros assume a 64-bit data
   model. The MSVC branch unconditionally makes `intptr_t` and `uintptr_t`
   64-bit, while all branches define `INTPTR_MIN`, `INTPTR_MAX`, and
   `UINTPTR_MAX` from the 64-bit limits. These definitions are incorrect on
   supported compiler targets with 32-bit pointers unless such targets are
   explicitly excluded from FTL's support contract.

4. The generic non-Apple GCC/Clang `INT64_C`, `UINT64_C`, `INTMAX_C`, and
   `UINTMAX_C` definitions hard-code `L`/`UL` suffixes. This assumes an LP64
   data model and gives the wrong expression type on ILP32 implementations
   where the corresponding 64-bit type is `long long`.

No remediation was performed as part of this audit.

### `<limits>`

**Status:** FAILED  
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<limits>` synopsis
- `float_round_style`
- primary `numeric_limits<T>` template
- cv-qualified specializations
- `numeric_limits<bool>`
- all required ordinary character and integer specializations
- extended integer requirements
- `float`, `double`, and `long double` specializations
- standardized extended floating-point specializations
- `min`
- `max`
- `lowest`
- `digits`
- `digits10`
- `max_digits10`
- `is_signed`
- `is_integer`
- `is_exact`
- `radix`
- `epsilon`
- `round_error`
- exponent properties
- infinity and NaN properties
- `denorm_min`
- `is_iec559`
- `is_bounded`
- `is_modulo`
- `traps`
- `tinyness_before`
- `round_style`
- constant-expression requirements
- signal-safety
- feature-test macro interaction
- freestanding status
- repeated inclusion
- normal and replacement namespace modes

Confirmed matches:

- `float_round_style` has the required enumerators and values.
- the primary `numeric_limits<T>` template provides the complete required
  surface and zero/value-initialized default values.
- required member functions are `constexpr` and `noexcept`.
- all static constexpr members are usable as constant expressions.
- all required standard fundamental integer and character specializations are
  present.
- `numeric_limits<bool>` matches its specially prescribed C++23 values.
- ordinary padding-free binary integer targets receive correct `min()`,
  `max()`, `lowest()`, signedness, exactness, radix, boundedness, and modulo
  properties.
- integer specializations return zero for properties not meaningful to integer
  types and use `round_toward_zero`.
- cv-qualified specializations expose values equal to the corresponding
  unqualified specialization.
- ordinary floating extrema, epsilon, representation digits, exponent bounds,
  and denormal minima are derived from compiler-provided representation
  constants.
- the facility is freestanding.
- include guards make repeated inclusion idempotent.
- normal and replacement namespace modes expose the intended interfaces.

Known defects:

1. `numeric_limits` specializations are not provided for implementation-defined
   extended integer arithmetic types. On compiler modes where types such as
   `__int128` and `unsigned __int128` are extended integer types, the primary
   template is selected and incorrectly reports `is_specialized == false`
   along with zero/default values for all other properties.

2. All integral specializations report `traps == false`. The standard requires
   this member to describe whether a value exists that can cause an arithmetic
   operation to trap at program start. On supported targets where integral
   operations such as division by zero trap, the required value is true.

3. Integral `digits` is computed as `sizeof(T) * 8 - is_signed`. This assumes
   an 8-bit byte and no padding bits rather than computing the number of
   non-sign radix digits required by the standard. It is therefore incorrect
   on implementations with non-8-bit bytes or padded integer representations.

4. The ordinary floating-point specializations hard-code infinity, NaN,
   ISO/IEC 60559 conformance, trapping, tinyness detection, and rounding-style
   properties. These properties are implementation-dependent and therefore
   become incorrect on supported compiler targets whose floating-point
   environment differs from the assumed IEEE binary, non-trapping,
   round-to-nearest model.

No remediation was performed as part of this audit.

### `<climits>`

**Status:** FAILED  
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<climits>` synopsis
- `CHAR_BIT`
- `SCHAR_MIN` and `SCHAR_MAX`
- `UCHAR_MAX`
- `CHAR_MIN` and `CHAR_MAX`
- `MB_LEN_MAX`
- `SHRT_MIN` and `SHRT_MAX`
- `USHRT_MAX`
- `INT_MIN` and `INT_MAX`
- `UINT_MAX`
- `LONG_MIN` and `LONG_MAX`
- `ULONG_MAX`
- `LLONG_MIN` and `LLONG_MAX`
- `ULLONG_MAX`
- integer-promotion requirements for macro expression types
- signed and unsigned plain `char`
- platform integer data models
- multibyte-runtime interaction
- freestanding status
- repeated inclusion
- interaction with corresponding C/runtime headers
- normal and replacement modes

Confirmed matches:

- the complete C++23 `<climits>` macro inventory is present.
- `CHAR_BIT` is derived from the compiler on GCC/Clang.
- GCC/Clang signed integer minima and maxima are derived from compiler-provided
  representation limits.
- GCC/Clang plain `char` limits correctly select signed or unsigned ranges
  according to the implementation's plain-char signedness.
- `INT_MAX`, `LONG_MAX`, and `LLONG_MAX` do not assume a fixed LP64 or LLP64
  data model on GCC/Clang.
- `UINT_MAX`, `ULONG_MAX`, and `ULLONG_MAX` have the required values and
  promoted expression types on the currently targeted ordinary data models.
- the MSVC integer limits match its standard LLP64 fundamental-type model.
- the MSVC limit macros use expression types consistent with the required
  integer promotions.
- the facility is freestanding.
- include guards make repeated direct inclusion idempotent.

Known defects:

1. On GCC/Clang, `UCHAR_MAX` and `USHRT_MAX` are constructed using
   `unsigned int` arithmetic. On ordinary implementations where
   `unsigned char` and `unsigned short` promote to `int`, C++23 requires
   these macros to have expression type `int`, but FTL gives them
   `unsigned int`. On targets where plain `char` is unsigned, the defect
   also propagates to `CHAR_MAX`.

2. `MB_LEN_MAX` is hard-coded to `5`. On Linux, FTL delegates its multibyte
   conversion functions and `MB_CUR_MAX` to glibc, whose supported locale
   model requires `MB_LEN_MAX == 16`. FTL therefore advertises a maximum
   multibyte-character length smaller than that supported by the runtime to
   which its own C conversion facilities delegate. This can also produce
   conflicting macro definitions when FTL and the platform C headers
   interact in normal mode.

No remediation was performed as part of this audit.

### `<cfloat>`

**Status:** CERTIFIED  
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<cfloat>` synopsis
- `FLT_ROUNDS`
- `FLT_EVAL_METHOD`
- `FLT_HAS_SUBNORM`
- `DBL_HAS_SUBNORM`
- `LDBL_HAS_SUBNORM`
- `FLT_RADIX`
- `FLT_MANT_DIG`
- `DBL_MANT_DIG`
- `LDBL_MANT_DIG`
- `FLT_DECIMAL_DIG`
- `DBL_DECIMAL_DIG`
- `LDBL_DECIMAL_DIG`
- `DECIMAL_DIG`
- `FLT_DIG`
- `DBL_DIG`
- `LDBL_DIG`
- minimum binary exponents
- minimum decimal exponents
- maximum binary exponents
- maximum decimal exponents
- `FLT_MAX`, `DBL_MAX`, and `LDBL_MAX`
- `FLT_EPSILON`, `DBL_EPSILON`, and `LDBL_EPSILON`
- `FLT_MIN`, `DBL_MIN`, and `LDBL_MIN`
- `FLT_TRUE_MIN`, `DBL_TRUE_MIN`, and `LDBL_TRUE_MIN`
- floating constant-expression requirements
- compiler evaluation-method interaction
- subnormal representation support
- MSVC floating representation
- GCC and Clang compiler-provided floating properties
- freestanding status
- repeated inclusion
- normal and replacement modes

Confirmed matches:

- the complete C++23 `<cfloat>` macro inventory is present.
- the facility is freestanding.
- GCC and Clang floating-point representation properties are derived from the
  compiler's corresponding predefined macros.
- GCC and Clang rounding and excess-evaluation properties are derived from
  compiler-provided values rather than assumed.
- exact representation limits, epsilon values, exponent ranges, decimal
  precision, and true minima are derived from compiler values on GCC/Clang.
- MSVC values agree with its documented binary floating-point model.
- MSVC `long double` characteristics correctly match its `double`
  characteristics.
- `FLT_ROUNDS` describes the supported implementation's default rounding
  behavior.
- `FLT_EVAL_METHOD` correctly describes the supported implementation's
  floating expression evaluation model.
- the three subnormal-support macros are provided with values appropriate to
  the corresponding implementation.
- all maximum, epsilon, minimum, and true-minimum macros have values appropriate
  to their corresponding floating-point types.
- include guards make repeated direct inclusion idempotent.
- no hosted C++ library dependency is introduced.

Known defects:

- None found.

Certification dependencies:

- compiler predefined floating-point representation macros on GCC and Clang
- documented MSVC fundamental floating-point representation and evaluation
  model

### `<source_location>`

**Status:** CERTIFIED  
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<source_location>` synopsis
- `source_location::current`
- direct-call source capture
- default-argument invocation-point capture
- default-member-initializer invocation-point capture
- presumed line-number behavior
- column behavior and unknown-column representation
- presumed file-name behavior
- function-name behavior
- interaction with `#line`
- default construction
- copy construction
- move construction
- copy assignment
- move assignment
- swappability
- destruction
- nothrow move requirements
- observable-state preservation across copies and moves
- `line`
- `column`
- `file_name`
- `function_name`
- null-terminated string requirements
- compiler source-location builtin contracts
- `__cpp_lib_source_location`
- freestanding status
- repeated inclusion
- normal and replacement namespace modes
- relevant `<cstdint>` dependency surface

Confirmed matches:

- the complete C++23 `<source_location>` public surface is present.
- `current()` is `static consteval` and `noexcept`.
- the default constructor and all observers have the required `constexpr` and
  `noexcept` properties.
- direct calls to `current()` capture the invocation source location through
  compiler-provided source-location builtins.
- default-argument uses capture the location of the invoking call rather than
  the location of the library declaration.
- default-member-initializer uses receive the compiler's corresponding
  invocation-point semantics.
- line information is obtained from the compiler's presumed source line.
- filename information is obtained from the compiler's presumed source file.
- function information is compiler-provided and yields an empty string when
  there is no enclosing function where required.
- Clang and MSVC provide column information through the compiler builtin path.
- GCC uses `0` when column information is unavailable, which is permitted by
  the standard.
- replacement mode's `__builtin_source_location` representation matches the
  compiler-defined implementation protocol.
- default-constructed objects contain valid but unspecified source-location
  state.
- file and function observers always return pointers to valid null-terminated
  byte strings.
- implicitly generated copy/move operations preserve every observable source
  field.
- nothrow move construction, move assignment, and swapping requirements are
  satisfied by the representation.
- all four observers return their corresponding captured field.
- `__cpp_lib_source_location` is advertised as `201907L`.
- the facility is freestanding.
- include guards make repeated inclusion idempotent.
- normal and replacement modes expose the intended namespace surface.

Known defects:

- None found.

Certification dependencies:

- compiler source-location builtin invocation semantics
- the `uint_least32_t` portion of `<cstdint>`

### `<stdfloat>`

**Status:** CERTIFIED  
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<stdfloat>` synopsis
- `float16_t`
- `float32_t`
- `float64_t`
- `float128_t`
- `bfloat16_t`
- conditional alias-presence requirements
- `__STDCPP_FLOAT16_T__`
- `__STDCPP_FLOAT32_T__`
- `__STDCPP_FLOAT64_T__`
- `__STDCPP_FLOAT128_T__`
- `__STDCPP_BFLOAT16_T__`
- corresponding floating-point literal suffixes
- binary16 representation requirements
- binary32 representation requirements
- binary64 representation requirements
- binary128 representation requirements
- bfloat16 representation requirements
- library feature-test macro requirements
- hosted/freestanding availability requirements
- repeated inclusion
- normal and replacement namespace modes
- interaction with `numeric_limits`

Confirmed matches:

- every C++23 `<stdfloat>` alias is present exactly when its corresponding
  implementation-provided `__STDCPP_*` macro is defined.
- `float16_t` is the type denoted by the standard `f16` literal suffix.
- `float32_t` is the type denoted by the standard `f32` literal suffix.
- `float64_t` is the type denoted by the standard `f64` literal suffix.
- `float128_t` is the type denoted by the standard `f128` literal suffix.
- `bfloat16_t` is the type denoted by the standard `bf16` literal suffix.
- using the corresponding literal type preserves the representation required
  by the language rules associated with each `__STDCPP_*` macro.
- FTL does not independently define or incorrectly advertise any of the
  implementation-provided extended-floating-point availability macros.
- no additional C++23 library feature-test macro is required for `<stdfloat>`.
- `<stdfloat>` is not part of the minimum header set required of a C++23
  freestanding implementation; FTL nevertheless provides it without a hosted
  library dependency.
- replacement mode provides the aliases in `std`.
- normal mode provides the corresponding aliases through `ftl`.
- include guards make repeated inclusion idempotent.
- no observable runtime behavior or state is introduced by the header.

Known defects:

- None found.

Certification dependencies:

- correctness of the compiler-provided `__STDCPP_FLOAT*_T__` and
  `__STDCPP_BFLOAT16_T__` predefined macros
- correctness of the corresponding compiler-supported standard floating-point
  literal suffixes

### `<coroutine>`

**Status:** CERTIFIED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<coroutine>` synopsis
- `coroutine_traits`
- positive and negative `promise_type` detection
- program-defined `coroutine_traits` specialization contract
- `coroutine_handle<void>`
- `coroutine_handle<Promise>`
- default construction
- null construction
- null assignment
- `from_promise`
- `from_address`
- `address`
- conversion from typed to erased handle
- `operator bool`
- `done`
- `operator()`
- `resume`
- `destroy`
- `promise`
- comparison operators
- hash support
- `noop_coroutine_promise`
- `coroutine_handle<noop_coroutine_promise>`
- `noop_coroutine_handle`
- `noop_coroutine`
- no-op handle conversion
- no-op resumption and destruction semantics
- no-op promise access
- no-op address requirements
- `suspend_never`
- `suspend_always`
- compiler coroutine builtin contracts
- `__cpp_impl_coroutine` dependency
- `__cpp_lib_coroutine`
- freestanding status
- repeated inclusion
- normal and replacement namespace modes
- relevant `<compare>`, `<functional>`, `<type_traits>`, `<utility>`,
  and `<cstddef>` dependency surfaces

Confirmed matches:

- the complete C++23 `<coroutine>` public surface is present.
- `coroutine_traits` exposes `R::promise_type` exactly when that qualified-id
  is valid and denotes a type.
- `coroutine_handle<void>` and `coroutine_handle<Promise>` provide the
  required construction, reset, export/import, observation, and resumption
  facilities.
- default- and null-constructed handles have null addresses.
- null assignment resets the stored coroutine address.
- typed handles convert to erased handles while preserving the coroutine
  address.
- `from_address(address())` reconstructs an equivalent handle under the
  standard's stated preconditions.
- `from_promise` and `promise` use the compiler coroutine-frame/promise
  translation facility and satisfy the required inverse association.
- `operator bool()` reflects whether the stored coroutine address is non-null.
- `done`, `resume`, and `destroy` delegate to compiler coroutine operations.
- equality compares coroutine addresses.
- three-way comparison uses `compare_three_way` on coroutine addresses.
- `hash<coroutine_handle<P>>` is enabled and hashes the underlying address.
- `noop_coroutine_handle` always converts to true and always reports
  `done() == false`.
- no-op typed resume, invocation, and destruction have no effects.
- conversion of a no-op handle to `coroutine_handle<>` preserves the same
  non-null frame address and the implementation provides no-op frame
  operations on the supported compiler paths.
- no-op promise access returns the associated promise object.
- `noop_coroutine()` returns a usable no-op handle; equality between handles
  from separate calls is not unnecessarily guaranteed.
- `suspend_never` and `suspend_always` match the required trivial-awaitable
  definitions.
- `__cpp_lib_coroutine` is advertised as `201902L`.
- unsupported compiler coroutine configurations are rejected rather than
  silently exposing a nonfunctional facility.
- the facility is freestanding.
- include guards make repeated inclusion idempotent.
- normal and replacement modes expose their intended namespace surfaces.

Known defects:

- None found.

Certification dependencies:

- compiler coroutine builtin semantics
- relevant `coroutine_handle` support portions of `<compare>` and `<functional>`
- required foundational type/utility facilities

### `<cstdarg>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<cstdarg>` synopsis
- `va_list`
- `va_start`
- `va_arg`
- `va_copy`
- `va_end`
- C++ default argument promotions
- rightmost named parameter requirements
- reference and incompatible final named parameter cases
- overloaded unary `operator&` interaction
- target-specific `va_list` representation
- argument alignment and advancement
- directly and indirectly passed variadic arguments
- GCC builtin implementation
- Clang builtin implementation
- MSVC x64 calling convention
- MSVC non-x64 calling-convention sensitivity
- freestanding status
- repeated inclusion
- normal and replacement namespace modes

Confirmed matches:

- the complete C++23 `<cstdarg>` public macro/type inventory is present.
- GCC and Clang use the compiler-provided target-specific `__builtin_va_list`.
- GCC and Clang delegate `va_start`, `va_arg`, `va_copy`, and `va_end` to
  the compiler's corresponding target-specific builtins.
- the builtin implementation naturally follows the ABI-selected argument-list
  representation rather than assuming a fixed stack layout.
- `va_start` receives the rightmost named parameter as required.
- the MSVC `va_start` definition does not apply unary `operator&` to the
  rightmost named parameter.
- MSVC `va_copy` produces an independently advancing copy for the pointer-style
  `va_list` representation used by that implementation path.
- the macros remain preprocessing macros rather than namespace members.
- the facility is freestanding.
- include guards make repeated inclusion idempotent.
- normal and replacement modes expose the intended `va_list` namespace
  surface.

Known defects:

1. The handwritten MSVC `va_arg` implementation assumes every variadic
   argument is stored inline in a slot whose size is `sizeof(T)` rounded up
   to eight bytes. Under the Microsoft x64 ABI, arguments that do not fit in
   eight bytes or are not 1, 2, 4, or 8 bytes are passed indirectly through
   a pointer-sized argument slot. FTL therefore interprets the pointer itself
   as the requested object and advances the argument list by the wrong amount
   for such types.

2. The handwritten MSVC argument traversal is selected solely by `_MSC_VER`
   and unconditionally uses an eight-byte slot model. Microsoft variadic
   calling conventions differ across x86, x64, ARM, and other targets, so the
   implementation is incorrect on supported non-x64 MSVC architectures.

No remediation was performed as part of this audit.

### `<new>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<new>` synopsis
- `bad_alloc`
- `bad_array_new_length`
- `destroying_delete_t`
- `destroying_delete`
- `align_val_t`
- `nothrow_t`
- `nothrow`
- `new_handler`
- `get_new_handler`
- `set_new_handler`
- throwing global `operator new`
- throwing global `operator new[]`
- aligned allocation
- nothrow allocation
- sized deallocation
- aligned deallocation
- nothrow deallocation
- placement `operator new`
- placement `operator new[]`
- placement delete forms
- replacement-function surface
- `launder`
- `launder` participation requirements
- pointer lifetime and optimization-barrier semantics
- hardware constructive interference size
- hardware destructive interference size
- feature-test macros
- freestanding status
- repeated inclusion
- normal and replacement modes
- relevant `<cstddef>`, `<exception>`, and `<type_traits>` dependency surfaces

Confirmed matches:

- the complete C++23 allocation/deallocation overload inventory is declared.
- throwing, aligned, nothrow, sized, and placement signatures have the
  required parameter and exception specifications.
- placement allocation returns its supplied pointer and performs no other
  action.
- placement deallocation performs no action.
- `align_val_t` has underlying type `size_t`.
- `nothrow_t` and `destroying_delete_t` have the required explicit default
  constructors.
- `destroying_delete` has the required inline constexpr form.
- `bad_array_new_length` derives from `bad_alloc`.
- the allocation exception classes return valid implementation-defined
  null-terminated strings from `what()`.
- `new_handler` has type `void (*)()`.
- `get_new_handler` and `set_new_handler` expose the required interface.
- normal mode delegates new-handler state to the host implementation rather
  than maintaining an incompatible parallel state.
- `launder` participates only for object types, satisfying the C++23 mandate
  excluding `void` and function types.
- GCC/Clang non-MSVC paths use `__builtin_launder` when available.
- hardware-interference constants have valid values on the currently
  exercised mainstream architectures.
- `__cpp_lib_hardware_interference_size` is `201703L`.
- `__cpp_lib_launder` is `201606L`.
- `__cpp_lib_destroying_delete` is `201806L`.
- the facility is freestanding.
- include guards make repeated inclusion idempotent.

Known defects:

1. `launder` returns its argument unchanged whenever `_MSC_VER` is defined.
   MSVC provides `::__builtin_launder`, and the vendor standard library uses
   that builtin to implement the required pointer lifetime/optimization
   barrier. FTL therefore fails to provide the required `launder` semantics
   under MSVC. The same branch is incorrectly selected by clang-cl, despite
   Clang also providing `__builtin_launder`.

2. `hardware_destructive_interference_size` and
   `hardware_constructive_interference_size` are unconditionally fixed at
    64. C++23 requires each value to be at least `alignof(max_align_t)`.
        This is valid on the currently exercised x86/x64/ARM64 targets but becomes
        nonconforming on any supported target whose `max_align_t` alignment exceeds
    64. The implementation neither derives the value from the target nor
        rejects unsupported architectures.

No remediation was performed as part of this audit.

### `<typeinfo>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<typeinfo>` synopsis
- `type_info`
- virtual destruction
- deleted copy construction
- deleted copy assignment
- constexpr equality
- `before`
- `hash_code`
- `name`
- implementation-defined RTTI collation
- equality/hash consistency
- RTTI-generated type information
- `bad_cast`
- `bad_typeid`
- failed reference `dynamic_cast`
- null-polymorphic `typeid`
- GCC/Clang Itanium-style RTTI representation
- Microsoft RTTI/VCRuntime representation
- runtime RTTI exception integration
- `__cpp_lib_constexpr_typeinfo`
- freestanding status
- repeated inclusion
- normal and replacement namespace modes
- relevant `<exception>` dependency surface

Confirmed matches:

- the complete C++23 `<typeinfo>` public inventory is present.
- `type_info` is noncopyable as required.
- `type_info` has a virtual destructor.
- `operator==` is `constexpr` and `noexcept`.
- runtime equality reports whether two objects describe the same type.
- MSVC runtime comparison, hashing, and naming delegate to the native RTTI
  ABI functions.
- the MSVC `type_info` representation matches the compiler/VCRuntime ABI
  shape required for compiler-generated RTTI objects.
- non-MSVC replacement-mode equality handles both canonical and non-unique
  RTTI name representations.
- `before` establishes an implementation-defined collation order.
- equal `type_info` objects produce equal `hash_code()` values within an
  execution.
- `name()` returns an implementation-defined null-terminated byte string.
- `bad_cast` and `bad_typeid` derive from the corresponding FTL/standard
  `exception` base in replacement mode.
- their `what()` functions return valid implementation-defined NTBS values.
- GCC/Clang normal mode aliases the host RTTI types directly.
- `__cpp_lib_constexpr_typeinfo` is advertised as `202106L`.
- the facility is freestanding.
- include guards make repeated inclusion idempotent.

Known defect:

1. In MSVC normal mode, `ftl::bad_cast` and `ftl::bad_typeid` are independent
   FTL classes rather than aliases to the Microsoft runtime's
   `std::bad_cast` and `std::bad_typeid`. Consequently, they do not denote
   the exception types generated by the language's RTTI operations: a failed
   reference `dynamic_cast` is not catchable as `ftl::bad_cast`, and a
   null-polymorphic `typeid` failure is not catchable as `ftl::bad_typeid`.
   The existing test suite skips these catch-path checks specifically on
   MSVC.

No remediation was performed as part of this audit.

### `<exception>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<exception>` synopsis
- `exception`
- exception copy semantics
- exception assignment semantics
- virtual destruction
- `exception::what`
- `bad_exception`
- `terminate_handler`
- `set_terminate`
- `get_terminate`
- `terminate`
- `uncaught_exceptions`
- `exception_ptr`
- nullable-pointer requirements
- exception-pointer copy/lifetime semantics
- exception-pointer equality
- exception-pointer thread-safety requirements
- `current_exception`
- `rethrow_exception`
- `make_exception_ptr`
- `nested_exception`
- `nested_exception::rethrow_nested`
- `nested_exception::nested_ptr`
- `throw_with_nested`
- `rethrow_if_nested`
- non-polymorphic `rethrow_if_nested` behavior
- inaccessible-base `rethrow_if_nested` behavior
- ambiguous-base `rethrow_if_nested` behavior
- MSVC exception-runtime ABI integration
- Itanium exception-runtime ABI integration
- Apple exception-runtime ABI integration
- exceptions-disabled declarations
- RTTI-disabled declarations
- `__cpp_lib_uncaught_exceptions`
- freestanding status
- repeated inclusion
- normal and replacement namespace modes
- relevant `<type_traits>` and `<utility>` dependency surfaces

Confirmed matches:

- the complete C++23 `<exception>` public surface is present.
- `exception` exposes the required nonthrowing construction, copy, assignment,
  destruction, and `what()` interface.
- exception copies preserve equivalent diagnostic messages.
- `bad_exception` derives from `exception` and provides a valid
  implementation-defined diagnostic string.
- `terminate_handler` has the required function-pointer type.
- terminate-handler state is delegated to the platform exception runtime.
- `set_terminate` returns the previous handler.
- `get_terminate` returns the current handler.
- `terminate` delegates to the platform termination mechanism.
- `uncaught_exceptions` reports the platform exception runtime's active
  uncaught-exception count.
- `exception_ptr` has null construction, copy, assignment, destruction,
  contextual boolean conversion, equality, and swapping semantics consistent
  with the required nullable-pointer model.
- non-null exception pointers compare equal when they refer to the same
  exception object.
- platform exception runtimes are used to retain and release captured
  exception objects rather than introducing independent exception ownership.
- `current_exception` delegates capture to the platform exception runtime.
- `rethrow_exception` delegates propagation to the platform exception runtime.
- `make_exception_ptr` implements the C++23 specified-as-if
  throw/catch/current_exception algorithm.
- `nested_exception` captures the currently handled exception at construction.
- `nested_ptr` returns the captured exception pointer.
- `rethrow_nested` rethrows the captured exception or calls `terminate` when
  none was captured.
- `throw_with_nested` publicly derives its wrapper from both the decayed
  exception type and `nested_exception` in the cases required by C++23.
- `rethrow_if_nested` correctly has no effect for non-polymorphic types.
- `__cpp_lib_uncaught_exceptions` is advertised as `201411L`.
- the facility is freestanding.
- include guards make repeated inclusion idempotent.
- normal and replacement modes expose their intended namespace surfaces.

Known defect:

1. `rethrow_if_nested` checks only whether the supplied type is polymorphic
   before forming a `dynamic_cast<const nested_exception*>`. C++23 requires
   the function to have no effect when `nested_exception` is an inaccessible
   or ambiguous base class of the argument type. In those cases the cast
   itself can be ill-formed, so FTL rejects programs for which the standard
   explicitly requires a well-formed no-op.

No remediation was performed as part of this audit.

### `<compare>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<compare>` synopsis
- `partial_ordering`
- `weak_ordering`
- `strong_ordering`
- category named values
- category conversions
- comparisons against literal zero
- reversed comparisons against literal zero
- named comparison predicates
- `common_comparison_category`
- `common_comparison_category_t`
- `three_way_comparable`
- `three_way_comparable_with`
- `compare_three_way_result`
- `compare_three_way_result_t`
- `compare_three_way`
- pointer comparison behavior
- `strong_order`
- `weak_order`
- `partial_order`
- ADL-only customization lookup
- floating-point strong ordering
- floating-point weak ordering
- floating-point partial ordering
- signed-zero handling
- NaN handling and equivalence classes
- `compare_strong_order_fallback`
- `compare_weak_order_fallback`
- `compare_partial_order_fallback`
- fallback participation
- evaluation-once requirements
- conditional `noexcept`
- feature-test macros
- freestanding status
- repeated inclusion
- normal and replacement modes
- relevant `<concepts>` and `<type_traits>` dependency surfaces

Confirmed matches:

- the complete C++23 `<compare>` public inventory is present.
- all three comparison category types provide the required named values.
- strong ordering converts to weak and partial ordering.
- weak ordering converts to partial ordering.
- category comparisons with literal zero produce the required ordered and
  unordered results.
- reversed zero/category three-way comparisons correctly reverse less and
  greater and preserve unordered.
- `is_eq`, `is_neq`, `is_lt`, `is_lteq`, `is_gt`, and `is_gteq` have the
  specified behavior.
- `common_comparison_category` selects the strongest category to which all
  category arguments convert and yields `strong_ordering` for an empty pack.
- `compare_three_way_result` probes the required const-lvalue `<=>`
  expression and omits `type` when that expression is not well-formed.
- `strong_order` gives ADL customization first priority.
- `strong_order` has a dedicated floating-point total-order implementation.
- floating strong ordering distinguishes signed zero and representation-level
  NaN ordering as required for IEC 60559 total ordering.
- the three comparison algorithm objects are inline constexpr CPOs.
- `__cpp_lib_three_way_comparison` is advertised as `201907L`.
- the facility is freestanding.
- include guards make repeated inclusion idempotent.

Known defects:

1. In normal mode, `compare_three_way` synthesizes a `strong_ordering` from
   `<` and `==` for classes and enums lacking `<=>`. C++23 constrains
   `compare_three_way` by `three_way_comparable_with` and, outside its pointer
   special case, requires it to return the forwarded `<=>` result. Legacy
   synthesis belongs to the fallback CPOs, so FTL accepts calls that the
   standardized facility requires to be ill-formed.

2. `weak_order` dispatches to `strong_order` before using `compare_three_way`
   and has no dedicated floating weak-order path. As a result, IEEE `-0.0`
   and `+0.0` compare as distinct instead of equivalent, and NaN payloads can
   remain distinguished despite C++23's prescribed weak-order equivalence
   classes.

3. `partial_order` dispatches to `weak_order` before `compare_three_way`,
   reversing the standard's precedence. For floating-point operands this can
   turn comparisons involving NaN from `partial_ordering::unordered` into an
   ordered weak/strong result.

4. The `weak_order` customization precedence is wrong for non-floating user
   types as well: C++23 prefers a usable `compare_three_way` result before
   falling back to `strong_order`, while FTL tries `strong_order` first.
   `partial_order` analogously prefers `weak_order` before the required
   `compare_three_way` step.

5. The fallback CPOs impose stronger concepts than C++23 requires.
   `compare_strong_order_fallback` and `compare_weak_order_fallback` require
   `totally_ordered_with` when their primary ordering CPO is unavailable,
   although the standard only requires boolean-testable `E == F` and `E < F`.
   `compare_partial_order_fallback` similarly requires
   `equality_comparable_with` rather than only the specified `E == F`,
   `E < F`, and `F < E` expressions. Valid fallback calls are therefore
   rejected.

6. `compare_three_way` does not implement the C++23 special strict-total-order
   handling for built-in pointer comparisons. Both modes ultimately use the
   language comparison operations directly rather than the implementation's
   required library strict total order over pointers.

No remediation was performed as part of this audit.

## Header ledger

| Header / unit        | Status        | Audited at                                 | ISO clauses                                                 | Notes                                                                                                                                                                                                                                        |
|----------------------|---------------|--------------------------------------------|-------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `<cstddef>`          | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.3, 16.4.3.2, 16.4.6.2, 17.2.1, 17.2.3–17.2.5, 17.3.2 | `byte` integral constraints are too narrow on implementations where extended integer types such as `__int128` satisfy `is_integral_v`.                                                                                                       |
| `<initializer_list>` | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 9.4.5, 16.4.6.2, 16.4.6.5, 17.10                            | Explicit and partial specializations of `initializer_list` are not prohibited as required by 17.10.2.                                                                                                                                        |
| `<cstdint>`          | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 6.8.2, 16.4.6.2, 17.4.1; ISO/IEC 9899:2018 7.20             | Limit-macro expression-type defects; greatest-width integer handling stops at 64 bits; pointer limits and integer-constant suffixes assume a 64-bit data model.                                                                              |
| `<limits>`           | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 6.8.2–6.8.3, 17.3.3–17.3.5                                  | Missing extended-integer specializations; incorrect integral `traps`; integer `digits` assumes 8-bit padding-free representations; floating implementation properties are hard-coded.                                                        |
| `<climits>`          | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 6.8.2, 7.3.7, 17.3.6; ISO/IEC 9899:2018 5.2.4.2.1           | GCC/Clang `UCHAR_MAX`/`USHRT_MAX` have incorrect promoted types; `MB_LEN_MAX` is hard-coded below the maximum supported by the delegated glibc multibyte runtime.                                                                            |
| `<cfloat>`           | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 17.3.7; ISO/IEC 9899:2018 5.2.4.2.2                         | Complete C++23 floating-point implementation-property macro surface matches the supported compiler implementations.                                                                                                                          |
| `<stdfloat>`         | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 5.13.4, 6.8.3, 15.11, 17.4.2                                | Conditional extended floating-point aliases exactly follow the implementation-provided `__STDCPP_*` facilities and corresponding standard literal types.                                                                                     |
| `<source_location>`  | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.8                                              | Call-site capture semantics, observers, special-member requirements, feature-test macro, and freestanding surface match C++23.                                                                                                               |
| `<coroutine>`        | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 9.5.4, 16.4.2.5, 17.12                                      | Coroutine traits, handles, builtin frame/promise interaction, comparison/hash support, no-op coroutine, trivial awaitables, feature-test macro, and freestanding surface match C++23.                                                        |
| `<cstdarg>`          | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 7.6.1.3, 16.4.2.5, 17.13.2; ISO/IEC 9899:2018 7.16          | Handwritten MSVC `va_arg` does not implement indirect aggregate passing and assumes an x64-style 8-byte argument layout on every MSVC architecture.                                                                                          |
| `<new>`              | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 6.7.3, 6.7.5.5, 16.4.2.5, 17.6                              | `launder` is not a compiler lifetime/optimization barrier on MSVC or clang-cl; hardware interference sizes assume a universal 64-byte lower bound without constraining target architectures.                                                 |
| `<typeinfo>`         | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 7.6.1.7–7.6.1.8, 16.4.2.5, 17.7                             | MSVC normal mode defines independent `ftl::bad_cast` and `ftl::bad_typeid` classes rather than aliases to the runtime RTTI exception types, so they do not catch exceptions produced by failed `dynamic_cast` and null-polymorphic `typeid`. |
| `<exception>`        | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 14.6, 16.4.2.5, 17.9                                        | `rethrow_if_nested` forms a `dynamic_cast` for every polymorphic type and therefore fails to provide the required no-op behavior when `nested_exception` is an inaccessible or ambiguous base.                                               |
| `<compare>`          | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 7.6.8, 11.10.3, 16.3.3.3.5, 17.11, 22.10.8.8                | Normal-mode `compare_three_way` incorrectly synthesizes legacy comparisons; weak/partial-order CPO dispatch and floating semantics are incorrect; fallback CPO constraints are too strong; pointer total-order semantics are missing.        |
| `<concepts>`         | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.3.2, 18.1–18.7                                 | `same_as` lacks the required symmetric subsumption because its two `is_same_v` operands are distinct atomic constraints; arithmetic concepts also inherit `<type_traits>`'s missing extended-integer recognition.                            |

## Audit policy

Audits and remediation are intentionally separate activities.

A failed audit records defects without requiring them to be fixed immediately.
After remediation, only the affected failed unit needs to be re-audited.

Previously certified units are not re-audited during later broad passes unless:

1. their implementation changed,
2. a relevant dependency changed,
3. a newly discovered library-wide requirement invalidates the previous audit,
   or
4. evidence establishes that the original certification was incorrect.

This ledger is intended to make the conformance process cumulative rather than
repetitive.