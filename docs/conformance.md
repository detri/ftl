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

## Large-header subdivision

Large facilities were audited by coherent standard subunits rather than as one
indivisible header. Their ledger status is the aggregate of every required
subunit.

### `<ranges>`

The completed review covers:

- range access
- range concepts and requirements
- range utilities
- range factories
- individual view/adaptor families
- range generators

`<ranges>` becomes **CERTIFIED** only when every required subunit is certified.

### `<algorithm>`

The completed review follows the standard's organization, including:

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

### `<type_traits>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<type_traits>` synopsis
- helper classes and `integral_constant` callable behavior
- primary, composite, and unary type categories
- constructibility, assignability, destructibility, and swappability traits
- triviality and nothrow variants
- type property queries
- type relationships and invocation traits
- cv, reference, sign, array, pointer, and other transformations
- `common_type`, `basic_common_reference`, and `common_reference`
- logical operator traits and short-circuiting
- member-relationship functions
- constant-evaluation detection
- extended integer and floating-point types
- incomplete-type requirements and non-referenceable types
- feature-test macros
- signal-safety and freestanding status
- repeated inclusion
- normal and replacement namespace modes

Confirmed matches:

- the complete C++23 public inventory is present on toolchains providing the
  required C++23 compiler trait intrinsics.
- `integral_constant` provides the required conversion and call operators,
  nested aliases, and static value.
- cv, reference, array, pointer, decay, identity, and conditional
  transformations have the specified results.
- primary and composite categories correctly cover the standard fundamental
  types, pointers, references, arrays, functions, members, classes, unions,
  and enumerations.
- compiler intrinsics are used for properties whose semantics cannot in
  general be implemented as portable library expressions.
- `common_type` follows the decay, conditional-expression, const-lvalue
  fallback, and variadic reduction sequence.
- `common_reference` provides the reference, customization, conditional-result,
  and `common_type` precedence required by C++23, including the P2609-adjusted
  conversion checks used by the audited implementation.
- `conjunction` and `disjunction` short-circuit and preserve the selected base
  characteristic rather than reducing every result to `bool_constant`.
- invocation traits account for function objects, member functions, member
  objects, reference wrappers, and pointer-like receivers.
- `is_constant_evaluated` is equivalent to the specified `if consteval`
  operation.
- the member-relationship functions use compiler builtins where available and
  have the required `constexpr` and `noexcept` interface.
- feature-test macros for the implemented type-trait facilities have their
  required C++23 values.
- all functions in the facility are signal-safe, and the header is
  freestanding.
- include guards make repeated inclusion idempotent.

Known defects:

1. `is_integral` enumerates only the standard integer types. On GCC and Clang,
   extended integer types such as `__int128` and `unsigned __int128` are
   integral types, but `is_integral_v` reports `false`. This also makes
   `is_arithmetic`, `is_fundamental`, `is_scalar`, `is_signed`, and
   `is_unsigned` incorrect for those types and prevents `make_signed` and
   `make_unsigned` from providing their required extended-integer results.

2. The copy/move constructibility and assignability convenience traits form
   `const T&`, `T&&`, or `T&` without first checking whether `T` is a
   referenceable type. Instantiating the copy, move, trivial, or nothrow
   variants with `void` is therefore ill-formed instead of producing the
   required `false` result. A direct MSVC compile reproduced this failure for
   all twelve affected constructibility and assignability traits.

3. The swappability probe declares only the scalar `swap(T&, T&)` overload and
   omits the standard array overload from its overload-resolution context.
   The one-argument `is_swappable<T[N]>` traits mask this with recursive partial
   specializations, but `is_swappable_with_v<int(&)[N], int(&)[N]>` and its
   nothrow counterpart incorrectly report `false` even though the required
   array swap expressions are well-formed.

No remediation was performed as part of this audit.

### `<ratio>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<ratio>` synopsis
- `ratio` normalization and member interface
- zero and most-negative denominator handling
- numerator and denominator representability requirements
- addition, subtraction, multiplication, and division
- intermediate and normalized-result overflow handling
- equality and relational comparisons
- comparison overflow avoidance
- conditional and unconditional SI aliases
- freestanding status
- repeated inclusion
- normal and replacement namespace modes

Confirmed matches:

- ordinary `ratio<N, D>` specializations normalize the sign into the numerator,
  reduce by the greatest common divisor, expose positive denominators, and
  provide the required canonical `type` alias.
- a zero denominator is diagnosed.
- multiplication cross-cancels operands before multiplying, and addition
  reduces denominators before forming its intermediate products.
- comparison uses a continued-fraction-style unsigned algorithm that avoids
  overflowing cross-products.
- all unconditional SI aliases from `atto` through `exa` have the specified
  values on the supported 64-bit `intmax_t` targets.
- the facility is freestanding and repeated inclusion is idempotent.

Known defects:

1. C++23 specifies `ratio_add`, `ratio_subtract`, `ratio_multiply`, and
   `ratio_divide` as alias templates that directly denote the normalized result
   ratio. FTL defines class templates under those names whose result is nested
   in `type`, then adds non-standard `_t` aliases. Consequently, required uses
   such as `ratio_add<R1, R2>::num` are ill-formed and the public synopsis does
   not match the standard.

2. The six required comparison class templates `ratio_equal`,
   `ratio_not_equal`, `ratio_less`, `ratio_less_equal`, `ratio_greater`, and
   `ratio_greater_equal` are absent. Only their `_v` variable templates are
   provided, so code using the required trait types is ill-formed.

3. `ratio` does not reject `INTMAX_MIN` as either template argument. C++23
   requires the program to be ill-formed when the absolute value of `N` or `D`
   is not representable by `intmax_t`. FTL's unsigned absolute-value helper
   instead permits that magnitude and can expose an invalid negative
   denominator after conversion back to `intmax_t`.

4. The conditional `yocto`, `zepto`, `zetta`, and `yotta` aliases are always
   omitted. C++23 requires each alias to exist when its specified constants are
   representable by `intmax_t`; the implementation does not make their
   presence depend on the target's `intmax_t` width.

No remediation was performed as part of this audit.

### `<utility>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<utility>` synopsis
- scalar and array `swap`
- `exchange`
- `forward`, `forward_like`, `move`, and `move_if_noexcept`
- `as_const` and `declval`
- integer comparison functions and `in_range`
- `to_underlying` and `unreachable`
- compile-time integer sequences
- complete `pair` synopsis
- pair construction, assignment, swapping, comparison, and deduction
- pair-like construction and assignment
- dangling-reference prevention
- piecewise construction
- tuple-like pair access
- `common_type` and `basic_common_reference` pair specializations
- feature-test macros
- signal-safety and freestanding status
- repeated inclusion
- normal and replacement namespace modes

Confirmed matches:

- the complete C++23 public name inventory is present.
- scalar `swap`, `exchange`, `forward`, `move`, `move_if_noexcept`, `as_const`,
  and `to_underlying` have the required effects and exception specifications.
- `forward_like` applies the model type's constness and value category without
  propagating unrelated qualifiers from the model.
- `declval` has the required unevaluated-only declaration and supports
  incomplete types.
- integer sequences use compiler pack-generation facilities and expose the
  required type, value type, and constant-time `size()` interface.
- ordinary `pair` construction and assignment perform the specified
  element-wise operations, including the C++23 const-qualified assignment and
  swap overloads.
- pair-like construction and assignment support the applicable library tuple
  and two-element array types while dedicated overloads cover pairs.
- pair tuple protocol, type-based and index-based `get`, deduction, piecewise
  construction, `make_pair`, and common-type/common-reference specializations
  are present.
- `unreachable` maps to the supported compiler's unreachable intrinsic.
- the forward/move helpers are signal-safe, the header is freestanding, and
  repeated inclusion is idempotent.

Known defects:

1. The integer comparison functions are constrained with `is_integral_v`
   instead of enforcing the standard's mandate that the operands are integer
   types usable with `make_unsigned`. FTL therefore accepts forbidden calls
   involving `bool`, `char`, `wchar_t`, `char8_t`, `char16_t`, and `char32_t`.
   Conversely, the `<type_traits>` extended-integer defect rejects required
   calls involving types such as `__int128`.

2. The array overload of `swap` lacks the required
   `is_swappable_v<T>` constraint. It can participate in overload resolution
   for arrays whose elements are not swappable, with failure deferred into its
   exception specification or body instead of being excluded as specified.

3. `pair` three-way comparison calls `compare_three_way` directly instead of
   the standard's exposition-only `synth-three-way`. In replacement mode this
   rejects element types that provide only `==` and `<`, although C++23 pair
   comparison is required to synthesize a weak ordering for such legacy
   element types. In normal mode it works only because `<compare>` itself
   incorrectly performs that synthesis, so the apparent behavior depends on a
   separate known conformance defect.

4. Pair dangling-reference prevention is compiled out whenever the compiler
   does not expose `reference_constructs_from_temporary` as an intrinsic. On
   such a supported compiler, constructors that C++23 requires to be defined
   as deleted remain viable and can create dangling reference members.

5. The C++23 pair-like constructor and assignment operators explicitly exclude
   `ranges::subrange` specializations. FTL's pair-like classification acquires
   `subrange` through `<ranges>` and makes these operations viable; the test
   suite currently asserts the non-standard construction behavior.

No remediation was performed as part of this audit.

### `<tuple>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<tuple>` synopsis
- exposition-only `tuple-like` and `pair-like` domains
- complete constructor and allocator-extended constructor sets
- constructor disambiguation, explicitness, and dangling-reference deletion
- copy, move, converting, pair, and tuple-like assignment
- member and non-member swap
- deduction guides
- `make_tuple`, `forward_as_tuple`, `tie`, `ignore`, and `tuple_cat`
- `apply` and `make_from_tuple`
- tuple size and element traits
- index-based and type-based element access
- tuple and tuple-like equality and three-way comparison
- common-type and common-reference specializations
- allocator-related traits
- feature-test macros
- freestanding status and repeated inclusion
- normal and replacement namespace modes

Confirmed matches:

- the full C++23 public name and overload inventory is present.
- element storage preserves reference element types and uses no-unique-address
  storage for eligible value elements.
- ordinary, converting, pair, allocator-extended, and library tuple-like
  constructors perform element-wise initialization with the required value
  categories and conditional explicitness in the common cases.
- copy, move, converting, pair, and const-qualified assignment families are
  present and perform element-wise assignment.
- allocator-extended construction dispatches among leading-allocator,
  trailing-allocator, and ordinary element construction forms.
- index-based `get`, tuple size and element traits, deduction guides,
  `make_tuple`, `forward_as_tuple`, `tie`, and `ignore` provide their required
  interfaces.
- equality short-circuits in element order, and tuple ordering implements
  synthesized three-way comparison for legacy element types.
- tuple/tuple and tuple/other-library-tuple-like common type and common
  reference specializations are present.
- member and non-member swap are element-wise with conditional exception
  specifications.
- the facility is freestanding and repeated inclusion is idempotent.

Known defects:

1. C++23 explicitly excludes `ranges::subrange` from the tuple-like
   constructors and assignment operators. FTL classifies `subrange` as a tuple
   source and accepts it for both operations; the existing tuple tests assert
   this non-standard construction behavior.

2. The required specialization
   `uses_allocator<tuple<Types...>, Alloc> : true_type` is missing. Since
   `tuple` has no `allocator_type` member, FTL's primary trait reports `false`,
   preventing generic uses-allocator construction from recognizing tuple's
   allocator-extended constructors.

3. Type-based rvalue `get<T>` implements its return with `move(get<T>(value))`.
   When the unique element type `T` is itself an lvalue reference, the required
   return type `T&&` collapses to an lvalue reference but the expression is an
   rvalue, making calls such as `get<int&>(tuple<int&>&&)` ill-formed.

4. `apply`, `make_from_tuple`, and `tuple_cat` are not constrained to the
   exposition-only C++23 `tuple-like` domain of `array`, `pair`, `tuple`, and
   `ranges::subrange`. They accept user-defined types that merely provide a
   tuple protocol, adding overload participation that the standard does not
   provide.

5. Tuple dangling-reference prevention, including `make_from_tuple`'s
   one-element mandate, is disabled whenever the compiler lacks the
   `reference_constructs_from_temporary` intrinsic. Constructors and calls
   that C++23 requires to be rejected can therefore remain viable on a
   supported toolchain.

6. The tuple-like common-type and common-reference partial specializations
   omit the standard's decay constraints. They can match cv-qualified
   tuple-like arguments directly instead of allowing the primary traits'
   required decay step to control participation.

No remediation was performed as part of this audit.

### `<optional>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<optional>` synopsis
- admissible `T` types and contained-object storage
- `nullopt_t`, `nullopt`, and `bad_optional_access`
- default, copy, move, in-place, value, and converting constructors
- constructor explicitness, participation, triviality, and exception behavior
- destructor and all assignment families
- `emplace`, `swap`, and `reset` state transitions and exception guarantees
- pointer, dereference, engagement, throwing, and fallback observers
- all cv/ref-qualified `and_then`, `transform`, and `or_else` overloads
- optional/optional, optional/nullopt, and optional/value comparisons
- synthesized and legacy relational support
- non-member swap and `make_optional`
- hash enablement and engaged-value equivalence
- `optional<T&>` extension isolation from the C++23 `optional<T>` surface
- feature-test macro
- repeated inclusion and normal/replacement namespace modes

Confirmed matches:

- `optional<T>` stores its contained object directly without dynamic
  allocation and preserves trivial destruction, copy, and move properties in
  the audited ordinary cases.
- default and `nullopt` construction produce the disengaged state, while
  in-place and value construction directly initialize an engaged value.
- copy, move, converting assignment, `reset`, and `emplace` implement the
  required engagement-state transitions, including disengagement if
  `emplace` construction throws.
- pointer, dereference, engagement, `value`, and `value_or` observers preserve
  the required value categories and throw `bad_optional_access` for an empty
  checked access.
- member and non-member swap handle all four engagement combinations and have
  the required conditional exception behavior for conforming element traits.
- `and_then` has all four cv/ref-qualified overloads and requires an optional
  result as a mandate.
- optional/optional equality, optional/nullopt comparison, deduction,
  `make_optional`, and engaged-value hashing have their required ordinary
  behavior.
- `nullopt_t` is non-default-constructible, non-aggregate, and not
  initializer-list-constructible.
- `optional<T&>` remains a separately specialized extension and does not make
  rvalue-reference optionals constructible.
- `__cpp_lib_optional` has the C++23 value `202110L`, and repeated inclusion is
  idempotent.

Known defects:

1. The twelve C++23 legacy relational overloads for optional/optional and
   optional/value comparisons (`!=`, `<`, `>`, `<=`, and `>=`, including both
   value operand orders) are absent. Rewriting from `==` and `<=>` does not
   replace these overloads for element types that provide only the
   corresponding pre-C++20 operators, so required comparisons are rejected.

2. Both `or_else` overloads omit their participation constraints. The
   `const&` overload must require an invocable nullary function and a
   copy-constructible `T`; the `&&` overload must require invocability and a
   move-constructible `T`. FTL instead leaves invalid calls to fail in the
   function body and can report them as viable during overload probing.

3. `transform` constructs its result through optional's ordinary in-place
   constructor, whose `is_constructible` constraint requires construction from
   the invocation result as an argument. C++23 deliberately permits an
   immovable result initialized directly from a prvalue and states that no
   movability is required. FTL rejects that required case.

4. The optional/value three-way comparison checks only that `left <=> right`
   is a valid expression. It omits the required
   `three_way_comparable_with<T, U>` constraint and excludes only exact
   `optional` specializations rather than every type derived from an optional.
   Its overload participation is therefore broader than specified.

5. The optional/optional three-way comparison likewise lacks the declared
   `three_way_comparable_with<T, U>` constraint and derives its return type by
   directly probing `compare_three_way`, inheriting that facility's known
   nonconforming participation behavior.

6. `hash<optional<T>>` removes both const and volatile qualifiers before
   testing and invoking the element hash. C++23 removes only `const`; FTL can
   therefore enable and use `hash<optional<volatile T>>` even when
   `hash<volatile T>` is disabled.

No remediation was performed as part of this audit.

### `<variant>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<variant>` synopsis
- admissible and duplicate alternative types
- `variant_npos`, `monostate`, and `bad_variant_access`
- default, copy, move, converting, type-tagged, and index-tagged construction
- overload-set selection for converting construction and assignment
- copy, move, and converting assignment state/exception semantics
- destruction, `emplace`, value status, and swap
- variant size and alternative traits with cv propagation
- index-based and type-based `get`, `get_if`, and `holds_alternative`
- single and multi-variant visitation, explicit-result visitation, and derived
  variant arguments
- equality, legacy relational operators, and three-way comparison
- non-member swap
- monostate comparison and hash support
- variant hash enablement and value/index contribution
- feature-test macro
- repeated inclusion and normal/replacement namespace modes

Confirmed matches:

- the complete public C++23 name inventory is present.
- variant stores exactly one in-place alternative or the valueless state and
  supports duplicate alternative types through index-based operations.
- default construction initializes alternative zero when it is default
  constructible.
- copy/move construction and destruction preserve the required triviality for
  ordinary trivially copyable alternatives.
- converting construction uses an overload-set model that rejects narrowing
  list-initialization and selects a unique alternative.
- type- and index-based access preserve cv/ref value categories and throw
  `bad_variant_access` on an active-index mismatch; pointer access returns null
  for mismatches and null variant pointers.
- value status, equality, legacy relational comparison, and index ordering
  handle the valueless state in the required order.
- monostate comparison and hashing are present.
- `variant_npos` has the required all-bits-one `size_t` value, and repeated
  inclusion is idempotent.

Known defects:

1. Copy assignment destroys the current alternative before copy-constructing
   a different alternative in every case. C++23 conditionally constructs a
   temporary variant and move-assigns it when copy construction can throw but
   move construction cannot, preserving the target's value if the copy throws.
   FTL instead becomes valueless and violates the specified exception-state
   semantics.

2. Converting assignment requires only assignability of the selected
   alternative. C++23 also requires the selected type to be constructible from
   the argument because assignment must be able to change the active
   alternative. FTL leaves that failure to an unconstrained `emplace` body.

3. The initializer-list type-based `emplace` overload and both index-based
   `emplace` overloads omit their uniqueness, index-bound, and constructibility
   constraints. Invalid specializations participate and then fail while
   instantiating internal type selection or construction.

4. Deduced-result `visit` chooses the invocation result for alternative zero as
   its internal return type and implicitly converts every other alternative's
   result to it. C++23 mandates that every invocation have exactly the same
   return type and value category; convertible-but-different results must be
   rejected.

5. Visitation accepts only exact `variant` specializations. C++23's `as-variant`
   rules also accept publicly and unambiguously derived variant arguments, but
   FTL's `get` and size dispatch cannot deduce through such derived types.

6. Swap for different active indices is implemented as three whole-variant
   moves and assignments. This does not implement the standard's conditional
   choice of which operand is moved through the temporary and can make the
   wrong operand valueless when an alternative move construction throws.

7. The three-way comparison depends on FTL's nonconforming
   `three_way_comparable` and `compare_three_way` implementations, so its
   declared participation and pointer/ordering behavior inherit the known
   `<concepts>` and `<compare>` defects.

Library-quality defect:

- `hash<variant<...>>` combines the element hash and active index with a plain
  XOR and an additive constant even though FTL already provides RapidHash
  mixing helpers. This is not an ISO violation, but it needlessly preserves
  correlations and produces weaker distribution for structured element hashes.

No remediation was performed as part of this audit.

### `<any>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<any>` synopsis
- empty, copy, move, value, and in-place construction
- contained-type decay and copy-constructibility requirements
- copy, move, and value assignment exception guarantees
- destruction, reset, swap, and `emplace`
- `has_value` and `type`
- all five `any_cast` overloads and their mandates
- RTTI and RTTI-disabled type identification
- `bad_any_cast`, non-member swap, and `make_any`
- allocation/storage strategy
- feature-test macro
- repeated inclusion and normal/replacement namespace modes

Confirmed matches:

- default construction is constant-evaluated, nonthrowing, and empty.
- copy construction clones the contained value, move construction transfers
  ownership without throwing, and copy/value assignment use a temporary to
  provide the required no-effects guarantee on construction failure.
- contained values are decayed and required to be copy constructible.
- reset destroys the contained object, swap exchanges complete states without
  throwing, and destruction releases the contained object.
- RTTI-enabled `type()` returns the contained type or `typeid(void)` for an
  empty object.
- pointer `any_cast` returns null for null, empty, or mismatched operands;
  value/reference casts preserve the required source value category and throw
  `bad_any_cast` on a type mismatch.
- `bad_any_cast`, both `make_any` overloads, and non-member swap are present.
- `__cpp_lib_any` has the required value `201606L`, and repeated inclusion is
  idempotent.

Known defects:

1. Both `emplace` overloads construct a replacement `any` and swap only after
   successful construction. C++23 requires `reset()` before construction and
   requires the `any` to be empty if construction throws. FTL instead preserves
   the previous value, producing the wrong exception-state semantics.

2. The forwarding value constructor fails to exclude values whose decayed type
   is a specialization of `in_place_type_t`. Such tag objects are forbidden by
   the standard constructor constraints but can be stored by FTL when the
   dedicated in-place overload is not selected.

3. The public `type()` observer is conditionally removed based directly on
   compiler RTTI macros rather than maintaining the required `<any>` synopsis.
   In an RTTI-disabled supported configuration the class therefore lacks a
   standard-required member; conversely, defining FTL's own `FTL_NO_RTTI` while
   compiler RTTI remains enabled does not suppress this RTTI dependency.

Library-quality defect:

- Every non-empty `any`, including an `int`, performs a separate heap
  allocation and uses a virtual dispatch object. C++23 recommends avoiding
  dynamic allocation for small nothrow-movable values; a small-object path
  would remove substantial allocation and indirection overhead.

No remediation was performed as part of this audit.

### `<expected>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<expected>` synopsis
- `unexpect_t`, `unexpect`, `unexpected<E>`, and deduction
- `bad_expected_access<void>` and `bad_expected_access<E>`
- admissible value and error types
- `expected<T, E>` and `expected<void, E>` construction and destruction
- converting expected/unexpected construction and participation
- copy, move, value, and unexpected assignment
- value/error state transitions and rollback guarantees
- observers, `value_or`, and `error_or`
- `emplace`, member swap, and non-member swap
- all cv/ref-qualified `and_then`, `or_else`, `transform`, and
  `transform_error` overloads for value and void specializations
- expected/expected, expected/value, and expected/unexpected equality
- triviality and conditional exception specifications
- feature-test macro
- repeated inclusion and normal/replacement namespace modes

Confirmed matches:

- the complete C++23 public name and broad overload inventory is present.
- `unexpected<E>` supplies the required construction, observers, equality,
  swap, and deduction behavior.
- `bad_expected_access<E>` owns and exposes its error while deriving from the
  void specialization and ultimately from `exception`.
- ordinary value/error construction, copy/move construction, destruction, and
  direct observers preserve the active state and required value categories.
- both value and void specializations preserve trivial special members for
  ordinary trivial value/error types.
- `value()` throws a `bad_expected_access<E>` containing the active error,
  while unchecked value/error observers have the required interfaces.
- `expected<void, E>` provides its dedicated construction, assignment,
  observer, monadic, `emplace`, swap, and equality surfaces.
- equality with another expected, a value, and an unexpected object is present
  in the applicable specializations.
- `__cpp_lib_expected` advertises the corrected C++23 monadic value `202211L`,
  and repeated inclusion is idempotent.

Known defects:

1. The non-initializer-list `emplace` overload is not declared `noexcept`, even
   though its constraint requires `is_nothrow_constructible_v<T, Args...>` and
   C++23 specifies the operation as nonthrowing. The initializer-list overload
   has the required exception specification, so the two overloads expose an
   inconsistent public interface.

2. `swap` omits the required participation condition that at least one of the
   value or error types be nothrow move constructible. It accepts pairs for
   which both cross-state moves can throw, where the standard overload is not
   available.

3. Cross-state swap always moves the error-bearing operand into a temporary and
   then performs a fixed sequence of replacements. C++23 chooses the direction
   according to which alternative is nothrow move constructible and specifies
   rollback behavior. FTL can destroy the wrong active member or fail to
   restore the original states when construction throws.

4. `and_then` checks only that the invocation result is some specialization of
   `expected`. C++23 also mandates that its `error_type` is exactly `E` and
   requires construction of the propagated error for the overload's cv/ref
   category. FTL accepts mismatched error types until its body fails or performs
   a non-standard conversion.

5. `or_else` does not require an expected result whose `value_type` is exactly
   `T`, nor does it model all required value propagation construction
   conditions. Mismatched value types can participate and be constructed via
   FTL's broader converting paths.

6. `transform` and `transform_error` construct their mapped results through
   ordinary in-place constructors, adding constructibility-from-an-argument
   requirements. C++23 permits direct initialization from immovable prvalues;
   FTL rejects those required value and error result types.

7. The monadic overloads generally encode specification mandates only through
   downstream constructor expressions and omit the exact object-type,
   unexpected-type, and cv/ref propagation requirements. This changes overload
   viability and diagnostics for invalid or boundary result types in both
   `expected<T, E>` and `expected<void, E>`.

8. Several state-changing assignments rely on the library's custom replacement
   helpers and on the incomplete nothrow conditions above. Their participation
   and rollback behavior do not cover the precise C++23 alternatives for
   preserving the original active state when construction throws.

No remediation was performed as part of this audit.

### `<bitset>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<bitset>` synopsis and proxy-reference interface
- default, integer, string, and character-pointer construction
- bitwise operations, shifts, mutation, element access, and conversions
- string conversion and all observers
- hash support
- formatted stream extraction and insertion
- zero-sized bitsets, bounds and overflow errors, and exception specifications
- constexpr coverage and feature-test macro
- repeated inclusion and normal/replacement namespace modes

Confirmed matches:

- fixed-size storage, integer construction, bit ordering, masking of unused
  storage bits, and zero-sized bitsets have the required observable behavior.
- string construction uses the supplied traits for comparisons, validates the
  full effective input range, maps its rightmost used character to bit zero,
  and reports the required argument and range errors.
- bitwise assignment and non-member operations, shifts, whole-set and
  single-bit mutation, checked `test`, unchecked subscripting, integer
  conversions, string conversion, equality, and observers are present with
  the required broad semantics and exception specifications.
- stream insertion widens the two digits and delegates through `to_string` as
  specified; ordinary stream extraction stops before a non-bit character and
  handles empty and zero-width bitsets correctly.
- `hash<bitset<N>>` is enabled and hashes the canonical word representation
  with the existing RapidHash implementation. No weak or ad-hoc hash-combining
  quality defect was found in this header.
- `__cpp_lib_constexpr_bitset` has the C++23 value `202207L`, and repeated
  inclusion is idempotent.

Known defects:

1. The C++23 synopsis declares a private `constexpr reference() noexcept` for
   `bitset<N>::reference`. FTL declares only its private two-argument
   constructor and the public copy constructor, so the required default
   constructor does not exist.

2. The character-pointer constructor adds requirements that `charT` be
   trivially copyable, standard-layout, and trivially default constructible.
   The specified constructor has no such constraints; in particular, the
   extra default-construction requirement removes otherwise valid
   character-like types from overload resolution instead of providing the
   standard interface and letting the specified `basic_string<charT>`
   expression determine validity.

3. `operator>>` performs raw `streambuf` reads and string growth without the
   exception-catching protocol required of a formatted input function. An
   exception from `sgetc`, `sbumpc`, or `push_back` therefore escapes directly
   instead of first setting the appropriate stream error state and then
   rethrowing only when that state is enabled in the stream's exception mask.

No remediation was performed as part of this audit.

### `<functional>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Audit subdivisions reviewed:

- header synopsis, function-object requirements, `invoke`, and `invoke_r`
- `reference_wrapper`, `ref`, `cref`, and common-reference specializations
- arithmetic, comparison, constrained comparison, logical, bitwise, and
  identity function objects
- `not_fn`, `bind_front`, `bind_back`, classic `bind`, placeholders, and
  `mem_fn`
- `bad_function_call`, `function`, and deduction guides
- every cv/ref/noexcept specialization of `move_only_function`
- default, Boyer-Moore, and Boyer-Moore-Horspool searchers
- primary and standard scalar/pointer `hash` specializations
- feature-test macros, repeated inclusion, and normal/replacement modes

Confirmed matches:

- the ordinary `INVOKE` cases for callables, member functions, member objects,
  pointers, derived objects, and reference wrappers are represented, and
  `invoke_r` exposes the C++23 constraint and conditional exception surface.
- `reference_wrapper` has reference-only construction, conversion/access,
  invocation, helper functions, deduction, and C++23 common-reference support.
- the typed and transparent arithmetic, comparison, logical, bitwise, and
  identity objects are present; pointer ordering routes through the library's
  total-order helper rather than raw relational results.
- `not_fn`, partial application, placeholders, classic binding, and `mem_fn`
  provide their principal public names and value-category overload families.
- `function` and all twelve C++23 `move_only_function` signature forms expose
  the principal construction, assignment, invocation, capacity, and swap
  interfaces.
- the three standard searcher types and their required stored state are
  present and return the specified match pairs for ordinary inputs.
- scalar, enum, pointer, null-pointer, and floating hash paths use the existing
  RapidHash implementation rather than a weak arithmetic combiner.
- the applicable C++23 feature-test macros are advertised and repeated
  inclusion is idempotent.

Known conformance defects:

1. `bind<R>` simply calls the non-`R` overload and discards `R`. Its result
   therefore uses ordinary `INVOKE` instead of `INVOKE<R>`: explicit result
   conversion, `void` result discarding, and overload viability do not follow
   the specified operation.

2. The classic bind wrapper forwards its own value category into the stored
   callable and bound tuple. The standard instead invokes cv-qualified lvalue
   state entities. FTL's rvalue overload tries to pass rvalue tuple elements
   to a resolver taking `T&`, making valid rvalue wrapper calls ill-formed, and
   its forwarding model would move stored state where the standard does not.

3. `function(F&&)` omits the mandate `is_constructible_v<decay_t<F>, F>` from
   its interface. Types that are copy constructible and callable but cannot be
   initialized from the supplied argument participate and fail in the body
   rather than having the specified mandate behavior.

4. `function(F&&)` allocates for every non-null target, including function
   pointers and `reference_wrapper` specializations. C++23 requires those two
   cases to throw nothing. The declared-noexcept reference-wrapper assignment
   constructs through that allocating path and can consequently terminate on
   allocation failure.

5. `function::target_type()` is removed whenever `FTL_HAS_RTTI` is false. It is
   an unconditional member of the standard synopsis; disabling a compiler
   feature does not permit the public member to disappear.

6. The callable deduction guide handles member `operator()` types but has no
   `function_guide` case for a static call operator's ordinary function-pointer
   type. C++23 explicitly requires deduction for a well-formed static
   `operator()` of the prescribed form.

7. The constrained partial specialization `hash<T>` accepts cv-qualified
   integral and enumeration types because `is_integral_v<const T>` and
   `is_enum_v<const E>` remain true. The standard scalar specializations are
   for the listed cv-unqualified types, and cv-qualified `hash` instantiations
   are not thereby enabled.

8. Floating hashes process the complete object representation after only
   normalizing signed zero. Equal floating values can have distinct object
   representations, notably padding bytes in x87-style `long double`; FTL can
   therefore produce different hash values for equal keys, violating the Hash
   requirement.

9. The comparison function objects inherit the pointer-ordering and CPO
   defects recorded for `<compare>`, so the `<functional>` entry points do not
   independently meet their required total-order and constrained-comparison
   semantics.

Library-quality defects (non-normative unless stated above):

1. `function` and `move_only_function` heap-allocate every callable and use a
   virtual dispatch object. There is no small-object buffer even for tiny,
   nothrow-movable callables; this misses the standard's recommended practice
   and adds allocation and indirection to the common case.

2. Both Boyer-Moore searchers store their hash object but never invoke it or
   build skip tables. They delegate to the same quadratic naive scan as
   `default_searcher`, so their algorithm names and hash parameters provide no
   performance benefit. The loose normative worst-case predicate bound is not
   exceeded, but this is a material implementation-quality defect.

No remediation was performed as part of this audit.

### `<typeindex>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<typeindex>` synopsis
- construction and implicit copy/move operations
- equality, legacy relational operations, and three-way comparison
- `hash_code` and `name`
- `hash<type_index>`
- repeated inclusion and normal/replacement namespace modes

Confirmed matches:

- `type_index` stores a pointer to the supplied `type_info`, is nonthrowingly
  constructible, and retains the expected implicit copy/move operations.
- equality and three-way comparison have the specified `type_info`-based
  effects and nonthrowing interfaces.
- `hash_code()` and `name()` delegate directly to the wrapped `type_info`.
- `hash<type_index>` returns exactly `value.hash_code()` as required.
- repeated inclusion is idempotent.

Known defect:

1. The required member functions `operator<`, `operator>`, `operator<=`, and
   `operator>=` are absent. Ordinary relational expressions can be rewritten
   through the provided `operator<=>`, but C++23 still specifies these four
   named members. Their absence is observable when selecting a member function
   explicitly or taking its address, and therefore is a public-synopsis defect.

No remediation was performed as part of this audit.

### `<execution>`

**Status:** CERTIFIED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<execution>` synopsis
- `is_execution_policy` and `is_execution_policy_v`
- all four standard execution-policy types
- `seq`, `par`, `par_unseq`, and `unseq`
- feature-test macro
- repeated inclusion and normal/replacement namespace modes

Confirmed matches:

- the four policy classes are distinct public types in `execution` and provide
  the unspecified implementation-owned class definitions permitted by C++23.
- `is_execution_policy<T>` is a unary type trait whose base characteristic is
  true exactly for FTL's four cv-unqualified policy types; cv-qualified and
  reference forms remain false as specified.
- each global policy object is an inline constant expression of its associated
  policy type and has stable program-wide identity under the inline-variable
  model.
- `__cpp_lib_execution` has the C++23 value `201902L` and repeated inclusion is
  idempotent.

Policy-taking algorithm overloads and their execution/exception semantics are
audited with their owning `<algorithm>`, `<numeric>`, and `<memory>` clauses;
they are not evidence for or against this header-synopsis certification.

### `<charconv>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<charconv>` synopsis and `chars_format` bitmask operations
- `to_chars_result` and `from_chars_result`
- every required ordinary integer overload, deletion for `bool`, bases 2--36,
  signs, buffer exhaustion, invalid input, and range errors
- shortest, format-selected, and precision-selected floating output
- decimal, scientific, fixed, hexadecimal, infinity, and NaN input
- rounding, subnormal, overflow, underflow, and unchanged-destination rules
- conditional standard extended floating-point overloads
- constexpr and feature-test surfaces
- repeated inclusion and normal/replacement namespace modes

Confirmed matches:

- `chars_format` has the required enumerators and complete bitmask interface,
  and the two result aggregates contain exactly the specified data and
  comparison members.
- ordinary integer output handles minimum signed values without overflow,
  emits lower-case digits, honors all permitted bases, and reports insufficient
  output space with the required result.
- ordinary integer input rejects a leading plus sign and unsigned minus sign,
  consumes the complete matching digit sequence on range error, and leaves the
  destination unchanged on both invalid input and range failure.
- the floating engine implements decimal and hexadecimal grammars without
  locale dependence or allocation, preserves the destination on failure, and
  covers shortest-round-trip, explicit precision, correct rounding, subnormal,
  infinity, and NaN behavior.
- conditionally supported standard extended floating types receive all three
  output forms and the input overload.
- `__cpp_lib_to_chars` is `201611L` and
  `__cpp_lib_constexpr_charconv` is `202207L`; repeated inclusion is
  idempotent.

Known defect:

1. C++23's `integer-type` placeholder requires overloads for every
   cv-unqualified signed and unsigned integer type, including implementation
   extended integer types. FTL spells out only the ordinary signed/unsigned
   types and `char`. Supported extended integers such as `__int128` therefore
   have no exact `to_chars` or `from_chars` overload and instead either convert
   to a narrower ordinary type or fail overload resolution. This is the same
   underlying extended-integer coverage gap recorded for `<type_traits>` and
   `<limits>`.

No separate library-quality defect was found in the conversion algorithms.
No remediation was performed as part of this audit.

### `<format>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Audit subdivisions reviewed:

- complete header synopsis, format-string grammar, and error reporting
- `basic_format_string` and constant-evaluation validation
- all narrow/wide, locale/non-locale formatting function families
- formatter requirements, `formattable`, parse context, and format context
- character, string, boolean, integer, floating, and pointer formatters
- Unicode field width, precision, and escaped character/string formatting
- `range_format`, `format_kind`, `range_formatter`, and default map/set/string
  range formatters
- `basic_format_arg`, argument stores, argument lists, visitation, and mappings
- pair and tuple formatters
- `format_error`, feature-test macros, repeated inclusion, and both namespace
  modes

Confirmed matches:

- the complete C++23 public type, alias, concept, function, overload, and
  result-aggregate inventory is present for both `char` and `wchar_t`.
- compile-time format strings validate replacement-field indexing and invoke
  each selected formatter's parse function during constant evaluation.
- automatic/manual indexing state, dynamic width and precision, fill/alignment,
  signs, alternate forms, zero padding, locale selection, and presentation-type
  validation follow the specified grammar and failure model.
- built-in formatter specializations cover the required ordinary arithmetic,
  character/string, null, and void-pointer types, including debug escaping and
  Unicode-aware estimated field widths.
- formatting arguments use the required mapped scalar/string/pointer forms and
  a handle for user-defined types; empty arguments, visitation, lookup, and
  context output/locale access have the specified interfaces.
- range kind selection, recursion prevention, const-range preference, range
  presentation modes, custom separators/brackets, and pair/tuple formatting
  match the C++23 range-formatting model.
- `__cpp_lib_format` and `__cpp_lib_format_ranges` both advertise `202207L`,
  and repeated inclusion is idempotent.
- focused MSVC Release builds and tests passed for `ftl_normal_format` and
  `ftl_replace_format` during this audit.

Known conformance defect:

1. C++23 requires an enabled formatter specialization for each cv-unqualified
   arithmetic type other than the excluded character types. FTL's integer and
   floating partial specializations depend on its incomplete `is_integral_v`
   and `is_floating_point_v` classification. Implementation-provided extended
   arithmetic types that those traits omit consequently have no required
   formatter and cannot be represented by the intended scalar argument
   mapping. This is the downstream formatting manifestation of the extended
   arithmetic defects recorded for `<type_traits>` and `<limits>`.

Library-quality defect (non-normative):

1. `vformat_to`, `format_to`, and every `format_to_n` overload first render the
   complete result into a newly allocated `basic_string` and then copy it to
   the requested output iterator. `formatted_size` likewise materializes the
   complete string merely to obtain its size. This adds avoidable allocation,
   storage, and copying, and defeats the streaming advantage of the iterator
   APIs (especially when `format_to_n` requests only a short prefix).

No remediation was performed as part of this audit.

### `<bit>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete freestanding `<bit>` synopsis
- `bit_cast` constraints, object-representation effects, and constexpr surface
- `byteswap` participation and representation reversal
- power-of-two queries, rotations, leading/trailing counts, and population count
- zero, full-width, negative-rotation, and overflow-precondition boundaries
- `endian` values on every supported target family
- feature-test macros, repeated inclusion, and normal/replacement modes

Confirmed matches:

- `bit_cast` has the required size and trivial-copy constraints and delegates
  to the compiler lifetime-aware builtin with the specified nonthrowing,
  constant-evaluation-capable interface.
- for ordinary unsigned integer types, power-of-two operations, rotations, and
  bit counts produce the specified results at zero and width boundaries;
  `bit_ceil` intentionally has no syntactic `noexcept` despite its Throws:
  Nothing element, matching the standard synopsis.
- `byteswap` reverses every byte for the supported padding-free ordinary
  integral representations and preserves the source type.
- `endian::native` selects little endian for the supported Windows, Linux, and
  Apple little-endian targets while keeping distinct `little` and `big`
  enumerators.
- all five C++23 bit facility feature-test macros have their required values,
  the header is freestanding, and repeated inclusion is idempotent.

Known conformance defect:

1. The unsigned-integer algorithms and `byteswap` constrain participation
   through FTL's incomplete `is_integral_v` classification. Implementation
   extended integer types such as unsigned `__int128` are unsigned integer
   types under C++23 but do not satisfy `detail::bit_unsigned`, and their signed
   or unsigned forms do not reach `byteswap`. Required overloads are therefore
   missing on supported GCC/Clang targets.

Library-quality defect (non-normative):

1. Population count, leading/trailing-zero count, and byte swap are implemented
   as per-bit or per-byte loops at runtime. The supported compilers expose
   direct intrinsics for these operations, with constexpr fallbacks easily
   retained; failing to use them can turn a single target instruction into a
   data-dependent loop in foundational bit primitives.

No remediation was performed as part of this audit.

### `<string_view>` and character traits

**Status:** CERTIFIED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- character-traits requirements and all five standard specializations
- complete `<string_view>` synopsis and required aliases
- construction, deduction, trivial copyability, iterators, and range opt-ins
- capacity, element access, modifiers, copying, substring, and comparison
- every search overload and empty/boundary case
- non-member equality and three-way comparison category selection
- stream insertion declaration, hash support, and literal suffixes
- feature-test macros, repeated inclusion, and normal/replacement modes

Confirmed matches:

- `char_traits` supplies the complete type and operation inventory, unsigned
  comparison for narrow `char`, noncolliding EOF representations on supported
  targets, overlap-safe move, constexpr copying/search, and the specified
  standard character type mappings.
- `basic_string_view` has the exact C++23 constructor and deduction-guide
  constraints, remains trivially copyable, and is opted into both `view` and
  `borrowed_range`.
- iterator, capacity, access, modifier, copy, substring, compare, prefix,
  suffix, containment, and all six search families match their required
  boundaries, exception behavior, and constant-time/linear complexity bounds.
- equality and three-way comparison use the supplied traits and propagate the
  permitted traits comparison category.
- all five view aliases, hash specializations, and `sv` literal overloads are
  present, and the applicable C++23 feature-test macros have the required
  values.
- repeated inclusion is idempotent.

Library-quality defect (non-normative):

1. Every string-view hash calls RapidHash with `value.size()` as the byte
   length. That is correct only for one-byte character types. `wstring_view`,
   `u16string_view`, and `u32string_view` hash only the first `size()` bytes
   rather than `size() * sizeof(char_type)`, producing severe avoidable
   collision clusters even though the minimum equal-values-equal-hash contract
   remains satisfied.

No remediation was performed as part of this audit.

### `<string>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Audit subdivisions reviewed:

- complete `<string>` synopsis, aliases, deduction guides, and allocator model
- all constructors, assignments, iterator families, capacity, and access
- every append, assign, insert, erase, replace, range, and overwrite overload
- invalidation, length errors, and the general no-effects exception guarantee
- search, substring, comparison, prefix/suffix/containment, and string-view
  interoperability
- concatenation allocator selection, comparisons, swap, erase, and erase-if
- stream declarations and `getline`
- all narrow/wide numeric conversions
- PMR aliases, hash support, literal suffixes, and feature-test macros
- repeated inclusion and normal/replacement modes

Confirmed matches:

- the principal C++23 constructor/assignment and range-overload inventory is
  present, including `from_range`, `assign_range`, `append_range`,
  `insert_range`, and `replace_with_range` with the standard compatible-range
  predicate.
- storage maintains the null-terminator and size/capacity invariants, supports
  small-string storage, uses allocator traits for allocation/deallocation, and
  implements copy selection plus conditional move/swap propagation in the
  ordinary allocator cases.
- iterator, access, search, substring, comparison, containment, and mutation
  results match the required behavior for nonthrowing standard character
  traits, including self-referential pointer inputs through temporary storage.
- concatenation selects an lvalue copy or an rvalue operand consistent with the
  specified allocator provenance in the ordinary overload families.
- non-member comparison, swap, erasure, stream declarations, `getline`, PMR
  aliases, all five string aliases, literals, and the C++23 range/string
  feature-test macros are present.

Known conformance defects:

1. `resize_and_overwrite` invokes `operation(data(), count)` as an lvalue.
   C++23 specifies `std::move(op)(p, m)`, so move-only or rvalue-qualified
   operations are rejected. It also accepts any result explicitly convertible
   to `size_type`, including non-integer-like results, instead of enforcing the
   specified integer-like mandate.

2. The deprecated no-argument `void reserve()` member in Annex D remains part
   of the C++23 library interface. FTL provides only `reserve(size_type)`, so
   source using the retained deprecated member is ill-formed.

3. Copy assignment propagates the allocator only when
   `propagate_on_container_copy_assignment` is true *and* the allocators compare
   unequal. The allocator-aware container rules require copy assignment of the
   allocator whenever propagation is true. Equal allocator objects may still
   carry distinct observable state, which FTL incorrectly retains.

4. C++23 gives every throwing `basic_string` member a no-effects guarantee.
   Several in-place paths (`append(count, c)`, `replace`, and dependent
   insert/append operations) reallocate or move existing characters before all
   potentially throwing user-supplied `traits` operations finish. A throwing
   `Traits::assign`, `Traits::move`, or `Traits::copy` can leave changed
   capacity or partially modified contents instead of the original string.

5. C++23 specifies floating `to_string` and `to_wstring` in terms of
   `sprintf`/`swprintf`-style conversions, including the active C locale's
   decimal-point behavior. FTL routes both through locale-independent
   `to_chars` and widens the narrow result, so their output diverges after a C
   locale with a non-period decimal separator is selected.

Library-quality defect (non-normative):

1. String hash specializations pass `v.size()` rather than
   `v.size() * sizeof(value_type)` to RapidHash. Wide, UTF-16, and UTF-32 strings
   therefore hash only a prefix of their representation and suffer the same
   avoidable collision clustering recorded for `<string_view>`.

No remediation was performed as part of this audit.

### `<cctype>`

**Status:** CERTIFIED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<cctype>` and ISO C `<ctype.h>` name inventory
- C linkage and namespace exposure
- classification and case-mapping signatures
- EOF and unsigned-character input domain
- active C locale behavior through the platform runtime
- repeated inclusion and normal/replacement modes

Confirmed matches:

- all twelve required classification functions and both case-conversion
  functions have the exact C signatures and C language linkage.
- every global function is made available in the selected C++ namespace
  without introducing a second implementation or accidentally importing a
  hosted C++ header.
- behavior, valid input domain, EOF handling, and C locale dependence are
  supplied by the supported platform C runtime as permitted for the C-library
  facility.
- repeated inclusion is idempotent.

### `<cwctype>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<cwctype>` and ISO C `<wctype.h>` synopsis
- `wint_t`, `wctype_t`, `wctrans_t`, and `WEOF`
- all wide classification and transformation functions
- property/transform descriptor lookup and application
- WEOF handling and active C locale semantics
- repeated inclusion and normal/replacement modes

Confirmed matches:

- the complete public name and broad function-signature inventory is present.
- the twelve standard property names and two standard transform names produce
  nonzero descriptors, unknown names produce zero, and descriptors round-trip
  through `iswctype` and `towctrans` for the basic execution character set.
- WEOF is not classified as an ordinary ASCII character by the implementation.
- repeated inclusion is idempotent.

Known defects:

1. Every classification and transformation operation is implemented with
   fixed ASCII comparisons. The C wide-character functions are sensitive to
   the currently installed C locale; after selecting a locale with non-ASCII
   wide letters, FTL still rejects those letters and never performs their case
   mappings. `wctype`/`wctrans` descriptors inherit the same semantic defect.

2. `WEOF` is defined as `(~0u)`, an expression of type `unsigned int`, rather
   than a constant expression of type `wint_t`. On supported targets where
   `wint_t` is narrower than `unsigned int`, the macro has the wrong type and
   its unconverted value differs from the implementation's wide EOF value.

No remediation was performed as part of this audit.

### `<cstring>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<cstring>` and ISO C `<string.h>` synopsis
- byte copying, movement, comparison, search, and filling
- string copy, concatenation, comparison, search, spans, and tokenization
- collation and transformation under the active C locale
- `strerror`, `strlen`, `NULL`, and `size_t`
- C++ const/non-const adjusted overload sets
- `<string.h>` compatibility routing
- repeated inclusion and normal/replacement modes

Confirmed matches:

- the complete public name inventory and all five C++ adjusted const/mutable
  search overload pairs are present with the required signatures.
- `memcpy`, overlap-safe `memmove`, `memset`, unsigned-byte `memcmp`, copying,
  concatenation, bounded operations, searching (including terminators), span
  operations, and tokenization match the ordinary ISO C effects and returns.
- `strerror` provides stable non-null implementation-defined text, and
  `strlen`, `NULL`, and `size_t` have the required surface.
- the compatibility `<string.h>` spelling re-exports the same FTL-owned
  facilities rather than consuming a hosted C++ header.
- repeated inclusion is idempotent.

Known conformance defect:

1. `strcoll` is always `strcmp`, and `strxfrm` is always an identity copy.
   Both functions are required to use the collation category of the currently
   selected C locale. After `setlocale(LC_COLLATE, ...)` selects a non-C
   collation, FTL continues to report bytewise ordering and an identity
   transform, so the null-terminated sequence utility semantics diverge.

Library-quality defects (non-normative):

1. On GCC and Clang the header deliberately suppresses builtin recognition and
   implements `memcpy`, `memmove`, `memset`, `memcmp`, and `strlen` with
   byte-at-a-time loops. This prevents the compiler or platform runtime from
   selecting vectorized and size-specialized primitives on common hot paths.

2. `strstr` uses the naive restart-at-every-character algorithm, giving
   quadratic behavior on repetitive haystacks and needles despite mature
   linear-time or skip-based implementations being available.

No remediation was performed as part of this audit.

### `<cwchar>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Audit subdivisions reviewed:

- complete `<cwchar>` and ISO C `<wchar.h>` synopsis and macro/type surface
- wide numeric input conversion
- byte/wide conversion, restartable conversion state, and sequence conversion
- wide string/memory copying, comparison, search, tokenization, and length
- locale collation/transformation and wide time formatting
- wide formatted input/output and stream orientation
- wide character file I/O and pushback
- C++ const/non-const adjusted overloads
- repeated inclusion and normal/replacement modes

Confirmed matches:

- the complete C++23 public name and broad overload inventory is present,
  including the adjusted mutable/const search overloads.
- ordinary ASCII numeric parsing, wide string/memory manipulation, searching,
  tokenization, buffer `swprintf`, basic stream orientation, and basic execution
  character I/O have the expected principal effects.
- integer limits, `NULL`, `size_t`, `mbstate_t`, and the required file/time
  types are exposed without importing a hosted C++ header.
- repeated inclusion is idempotent.

Known defects:

1. `btowc`, `wctob`, `mbrtowc`, `mbrlen`, `wcrtomb`, `mbsrtowcs`, and
   `wcsrtombs` implement an ASCII-only one-byte encoding and reset the state
   after every character. These functions are defined by the current C locale's
   multibyte encoding and conversion state. UTF-8 and other multibyte locales
   therefore reject valid sequences or emit incorrect bytes, and incomplete
   multibyte sequences can never be resumed correctly.

2. The `wcsto*` conversion engine recognizes only hard-coded ASCII whitespace,
   punctuation, and a period radix character. C++ adopts the locale-sensitive
   C conversions, so parsing diverges for locales with other whitespace or
   decimal-point conventions.

3. `wcscoll` is permanently `wcscmp`, and `wcsxfrm` is permanently an identity
   transformation. They ignore the active `LC_COLLATE` category just like the
   corresponding narrow functions in `<cstring>`.

4. `wcsftime` uses embedded English weekday/month names and treats the `E` and
   `O` modifiers as C-locale no-ops regardless of the selected locale. Required
   locale-specific names, representations, and alternative forms are absent.

5. Wide file output accepts only values at or below `0x7f` and writes each as a
   single byte; wide input and pushback use the reciprocal one-byte path. A
   wide-oriented stream in a multibyte locale must convert through its
   associated `mbstate_t`. Valid non-ASCII wide I/O consequently fails or is
   misrepresented.

6. `WEOF` is defined as the unsigned-int expression `(~0u)` rather than a
   constant expression of type `wint_t`, repeating the observable type/value
   defect recorded for `<cwctype>` on targets with narrower `wint_t`.

Library-quality defect (non-normative):

1. The header suppresses compiler builtin recognition for its wide memory and
   string primitives and uses scalar loops, while `wcsstr` uses a naive
   quadratic search. This prevents optimized platform lowering for foundational
   operations in the same way as `<cstring>`.

No remediation was performed as part of this audit.

### `<cuchar>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `<cuchar>` and ISO C UTF conversion synopsis
- `mbrtoc8` / `c8rtomb`
- `mbrtoc16` / `c16rtomb`
- `mbrtoc32` / `c32rtomb`
- null source/destination rules, zero-length input, error returns, and `errno`
- restartable state, UTF-8 code-unit staging, and UTF-16 surrogate staging
- repeated inclusion and normal/replacement modes

Confirmed matches:

- all six required functions, `mbstate_t`, and `size_t` are present with the
  required signatures.
- ASCII characters, null characters, reset requests, zero-length incomplete
  input, encoding-error reporting, invalid UTF-8 continuation detection, and
  lone UTF-16 surrogate rejection have the expected broad behavior.
- the reverse UTF-8 and UTF-16 paths retain partial code-unit state across
  calls rather than treating each code unit as an independent scalar.
- repeated inclusion is idempotent.

Known defects:

1. All three multibyte-to-UTF functions delegate to a one-byte ASCII helper.
   They ignore the active C locale's multibyte encoding and reject every
   non-ASCII sequence, including valid UTF-8 under a UTF-8 locale.

2. `mbrtoc8` and `mbrtoc16` do not implement the required staged output for a
   scalar represented by multiple UTF code units. They never retain a decoded
   scalar for subsequent calls and never return `size_t(-3)` while emitting
   the remaining UTF-8 code units or the low UTF-16 surrogate without
   consuming more input.

3. `c8rtomb`, `c16rtomb`, and `c32rtomb` recognize some partial Unicode input
   structure but deliberately return `EILSEQ` when a valid non-ASCII scalar is
   complete. In a locale capable of representing that scalar they must emit
   the corresponding multibyte character and return its byte count.

No remediation was performed as part of this audit.

### `<array>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete C++23 synopsis, aggregate and zero-length specializations
- element access, iterators, capacity, fill, swap, and comparisons
- deduction guide, `to_array`, and tuple protocol
- constraints, exception specifications, complexity, and freestanding status
- feature-test macros, repeated inclusion, and normal/replacement modes

Confirmed matches:

- the aggregate representation, public types, element/iterator/capacity
  operations, zero-length observable behavior, deduction guide, tuple
  protocol, and both `to_array` overloads match the required surface.
- bounds-checked access reports `out_of_range` with exceptions and terminates
  in the library's exception-free configuration; ordinary operations have the
  required constant complexity and `constexpr` availability.
- swap participation and its conditional exception specification account for
  the element type, including the zero-length specialization.
- the header is freestanding and repeated inclusion is idempotent.

Known defect:

1. Relational comparison uses a private `array_compare` helper that selects
   any syntactically valid `operator<=>`, without requiring its result to be a
   comparison category convertible to `partial_ordering`. For a type whose
   spaceship returns an unrelated type, the standard `synth-three-way`
   operation falls back to legacy `<`, while FTL selects the unusable result
   and then attempts `category::equivalent`. The array comparison is therefore
   ill-formed for a permitted element type. This is the same shared comparison
   defect already recorded for `<compare>`.

No remediation was performed as part of this audit.

### `<vector>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- sequence-container and allocator-aware-container requirements
- complete primary-template and `vector<bool>` synopses
- construction, assignment, capacity, access, modifiers, invalidation, and
  exception guarantees
- range construction/insertion, deduction guides, erasure, and comparisons
- `vector<bool>` proxy, iterators, hash, and required formatter specialization
- feature-test macros, repeated inclusion, and normal/replacement modes

Confirmed matches:

- the primary template and packed-bool specialization expose the principal
  C++23 constructors, range operations, accessors, modifiers, deduction
  guides, erasure functions, and allocator-aware types.
- storage is contiguous for the primary template; the bool specialization has
  the required proxy assignment and swap surface, random-access iterators, and
  masks unused trailing bits.
- growth is geometric, length checks respect allocator and difference limits,
  and the ordinary successful operations meet their required broad effects.
- both implementations are `constexpr`-enabled and repeated inclusion is
  idempotent.

Known defects:

1. Reallocation is committed before the new element or elements are
   constructed. If `emplace_back`, `push_back`, or an enlarging `resize` then
   throws while constructing the appended value, the original allocation has
   already been destroyed and capacity has changed. This violates the required
   no-effects guarantee (outside the standard's narrow throwing-move
   exception).

2. For the same reason, `push_back(v[i])` and analogous `emplace_back`
   arguments that refer into the vector become dangling when growth occurs;
   FTL then constructs the new element through the invalid reference. Such
   self-referential insertion is within the standard operation's domain.

3. The C++23 constrained `formatter` specialization for
   `vector<bool, A>::reference` is absent. Consequently a bool proxy cannot be
   formatted through the interface required by 24.3.12.2.

Library-quality defect (non-normative):

1. `hash<vector<bool>>` correctly selects RapidHash, but supplies the number of
   storage blocks as its byte length. It therefore hashes only a small prefix
   of the packed representation (often one or a few bytes), creating severe
   avoidable collision clusters despite the stronger hash already being
   available.

No remediation was performed as part of this audit.

### `<deque>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- sequence-container, reversible-container, and allocator-aware requirements
- complete synopsis, construction/assignment, capacity, access, and modifiers
- iterator semantics and invalidation, range operations, and deduction guides
- exception guarantees, complexity, comparisons, erasure, and PMR alias
- feature-test macros, repeated inclusion, and normal/replacement modes

Confirmed matches:

- the C++23 public type and overload inventory, including range construction,
  assignment, prepend/append/insertion, erasure functions, deduction guides,
  and the PMR alias, is present.
- the segmented storage model provides constant-time indexed access and
  end insertion in the ordinary amortized case; iterators expose the required
  random-access surface.
- allocator propagation branches, capacity/access operations, successful
  modifier effects, and comparison result selection broadly match the audited
  requirements.
- repeated inclusion is idempotent.

Known defects:

1. The allocating constructors have no ownership guard around their append
   loops. If allocation or element construction throws after one or more
   blocks have been installed, the `deque` destructor is not invoked for the
   incompletely constructed object and the pointer-map member does not destroy
   elements or deallocate the pointed-to blocks. Constructed elements and raw
   block allocations therefore leak.

2. `ensure_back` passes a freshly allocated block directly to the internal
   pointer vector's `push_back`. If growing that vector throws, the block
   pointer has not been installed anywhere and is leaked. This violates the
   required no-effects/resource-safety behavior of end insertion.

3. Middle insertion and erasure rebuild a replacement deque by unconditionally
   moving the existing elements out of the source before the operation can
   succeed. For copyable types whose move construction throws, the source can
   be left partly moved-from even though the standard's no-effects guarantee
   applies; the permitted unspecified-effects escape is limited to the stated
   non-CopyInsertable throwing-move case.

No remediation was performed as part of this audit.

### `<forward_list>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- sequence/allocator-aware requirements and the complete C++23 synopsis
- constructors, assignment, iterators, access, and all modifiers
- splice, remove, unique, merge, sort, reverse, and their complexity rules
- range operations, deduction guides, comparisons, erasure, and PMR alias
- exception safety, allocator ownership, repeated inclusion, and both modes

Confirmed matches:

- the required public surface, including C++23 range overloads, deduction
  guides, free erasure, and the PMR alias, is present.
- node insertion and erasure have constant per-node structural work; successful
  splice, removal, uniqueness, merge, reverse, and comparison operations have
  the expected broad effects and iterator stability.
- allocator propagation paths and conditional exception specifications match
  the container's principal requirements.
- repeated inclusion is idempotent.

Known defects:

1. Every multi-element constructor links nodes directly into the object as it
   progresses. If a later allocation or element construction throws, the class
   destructor is not run for the incompletely constructed object and no guard
   owns the already linked nodes, so their elements and allocations leak.

2. Count, iterator, and range insertion similarly commit each node before the
   whole operation succeeds. A later exception leaves a partial insertion,
   contrary to the operation's required no-effects guarantee.

3. `sort` recursively disconnects sublists and assigns the final head only
   after `merge_sort` returns. If the comparison function throws, disconnected
   nodes are no longer reachable from the container. The standard permits an
   unspecified resulting order but requires every element to remain in the
   list; FTL can leak and lose elements instead.

No remediation was performed as part of this audit.

### `<list>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- sequence/allocator-aware requirements and the complete C++23 synopsis
- construction, assignment, iterators, capacity, access, and modifiers
- splice, remove, unique, merge, sort, reverse, and complexity requirements
- range operations, deduction guides, comparisons, erasure, and PMR alias
- exception safety, allocator ownership, repeated inclusion, and both modes

Confirmed matches:

- the public overload inventory, C++23 range additions, bidirectional iterator
  surface, constant-time size, deduction guides, erasure, and PMR alias are
  present.
- successful node insertion, erasure, splice, removal, uniqueness, merge, and
  reverse operations have the required principal effects and preserve the
  expected iterators.
- allocator propagation branches and the container-specific move/swap
  exception specifications broadly match C++23.
- repeated inclusion is idempotent.

Known defects:

1. Multi-element constructors link nodes directly into the object without a
   cleanup guard. If a later allocation or element constructor throws, the
   incomplete `list` does not run its destructor and all nodes already linked
   into its sentinel leak.

2. Count, iterator, and range insertion commit nodes incrementally. An
   exception after the first successful insertion leaves a partial result,
   violating the required no-effects guarantee.

3. `sort` moves half the nodes into a local list and recursively sorts both
   halves. If the comparison function throws, stack unwinding destroys the
   local half and its elements. C++23 permits the order to become unspecified
   but requires all elements to remain in the list.

No remediation was performed as part of this audit.

### `<set>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- associative, reversible, and allocator-aware container requirements
- complete `set` and `multiset` synopses and unique/equivalent-key semantics
- constructors, assignment, insertion, extraction, merge, lookup, and erasure
- node handles, iterator stability, comparison, complexity, and PMR aliases
- heterogeneous operations, range additions, deduction guides, feature macros,
  repeated inclusion, and normal/replacement modes

Confirmed matches:

- the principal ordered set/multiset types, const iterator model, observers,
  lookup operations, range additions, extraction, erasure, and PMR aliases are
  present.
- the AVL tree maintains logarithmic-height lookup and ordinary insertion and
  erasure; successful unique/equivalent-key placement and iterator traversal
  have the expected broad behavior.
- transparent lookup and heterogeneous erasure are conditionally exposed from
  a comparator's `is_transparent` marker.
- repeated inclusion is idempotent.

Known defects:

1. Tree insertion allocates and constructs a node before invoking the
   comparator, but the raw node has no guard. If comparison throws during the
   search, the node and its element leak. Multi-element constructors compound
   this: an exception also leaks every node already linked into the incomplete
   tree because its destructor is not run.

2. Node-handle insertion releases the node from the handle before comparison.
   A throwing comparator therefore empties the handle and leaks the extracted
   node, rather than preserving ownership. Merge inherits the same element-loss
   path after extracting a source node.

3. Merge accepts only the identical internal base specialization. C++23
   requires overloads across `set`/`multiset` and across different comparator
   types with the same key and allocator; those calls are absent.

4. Allocator-only iterator-range, `from_range`, and initializer-list
   constructor overloads and their corresponding deduction guides are absent.
   Supplying an allocator in the standard third-argument position instead tries
   to bind it as the comparator.

5. The inherited initializer-list assignment operator returns the internal
   base type by reference, rather than `set&` or `multiset&` as specified.

6. Hinted insertion ignores its hint and always performs a full tree search,
   so it does not meet the required amortized-constant complexity when the
   element is inserted immediately before the hint.

7. Non-member ordering calls `compare_three_way` directly rather than the
   required `synth-three-way`. Set element types that meet the legacy `<`
   requirements but do not provide a three-way comparison consequently lose
   the mandated fallback ordering.

8. The node handle embeds a value-initialized `Allocator` even while empty,
   imposing default constructibility not required of the container allocator.
   Its unconditional `noexcept` move/swap operations also ignore allocator
   propagation and throwing allocator operations.

No remediation was performed as part of this audit.

### `<map>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- associative, reversible, and allocator-aware container requirements
- complete `map` and `multimap` synopses and key/value semantics
- construction, assignment, element access, modifiers, node handles, and merge
- lookup, comparisons, complexity, deduction guides, erasure, and PMR aliases
- heterogeneous/range additions, feature macros, repeated inclusion, and both
  namespace modes

Confirmed matches:

- principal unique/equivalent map operations, accessors, observers, ordinary
  lookup, range additions, node extraction, erasure, and PMR aliases are
  present.
- successful tree operations provide the expected broad key ordering,
  logarithmic lookup, stable traversal, and allocator ownership in ordinary
  same-allocator cases.
- `try_emplace` avoids constructing a mapped value when a key already exists,
  and heterogeneous lookup/erasure follows the transparent marker.
- repeated inclusion is idempotent.

Known defects:

1. The shared tree's unguarded pre-search node allocation leaks when the
   comparator throws, and throwing multi-element construction leaks every
   previously linked node of the incomplete map.

2. Node-handle insertion empties the handle before potentially throwing
   comparisons. The node then leaks; merge can likewise extract and lose an
   element instead of retaining ownership in one of the containers.

3. Merge is missing every cross-comparator and `map`/`multimap` counterpart
   overload required by the synopsis.

4. The allocator-only iterator-range, `from_range`, and initializer-list
   constructors and associated deduction guides are missing. Their standard
   argument forms are misinterpreted as comparator arguments.

5. Inherited initializer-list assignment returns the internal base type rather
   than the required `map&` or `multimap&`.

6. `value_compare` is an alias to a helper with a public constructor and a
   templated call operator. The specified nested class has a protected
   constructor and compares exactly two `value_type` objects, making both
   access control and overload participation observably different.

7. Hints are ignored, violating the amortized-constant well-positioned-hint
   complexity guarantee, and non-member ordering calls `compare_three_way`
   rather than providing `synth-three-way` fallback for legacy-orderable key
   and mapped types.

8. The shared node handle unnecessarily requires a default-constructible
   allocator and declares allocator-moving/swapping operations unconditionally
   `noexcept`, irrespective of the required allocator propagation conditions.

No remediation was performed as part of this audit.

### `<unordered_set>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- unordered, allocator-aware, and reversible container requirements
- complete `unordered_set` / `unordered_multiset` synopses
- construction, assignment, insertion, node handles, merge, and erasure
- lookup, bucket/local-iterator interface, hash policy, equality, and complexity
- heterogeneous/range additions, deduction guides, PMR aliases, feature macros,
  repeated inclusion, and both namespace modes

Confirmed matches:

- the principal global/local iterator, bucket, observer, lookup, hash-policy,
  range, extraction, erasure, and PMR surfaces are present.
- successful insertions cache the supplied hash result, preserve equivalent
  elements contiguously, grow the bucket array geometrically, and provide the
  expected average-case lookup structure.
- transparent lookup and heterogeneous erasure require transparent hash and
  equality markers.
- repeated inclusion is idempotent.

Known defects:

1. A node is allocated before invoking `Hash`, `KeyEqual`, or bucket growth and
   is held only by a raw pointer. If hashing, equality, or rehash allocation
   throws, insertion leaks the node. Throwing multi-element construction also
   leaks all nodes already linked into the incomplete table.

2. Node-handle insertion releases ownership before those throwing operations.
   The handle is emptied and its node leaks on failure; merge inherits the same
   loss after extracting a source element.

3. Merge only accepts the identical internal specialization. The required
   `unordered_set`/`unordered_multiset` counterpart overloads and variants with
   different hash or predicate types are missing.

4. Several allocator-only constructor forms and deduction guides required by
   the synopsis are absent, and inherited initializer-list assignment returns
   the internal base type instead of the concrete container type.

5. Set equality compares only each container's counts under its own key
   equivalence relation. The standard comparison is based on element equality
   within corresponding equivalent-key groups. Containers with a
   case-insensitive predicate, for example, can compare equal even when their
   stored keys are not `==`.

6. Empty node handles value-initialize an `Allocator`, requiring allocator
   default construction that C++23 does not impose, while node move/swap is
   declared unconditionally `noexcept` despite allocator propagation rules.

Library-quality defect (non-normative):

1. Bucket counts grow only as powers of two and raw hash values are reduced by
   modulo without final mixing. Hashers with weak low bits therefore produce
   severe clustering. FTL already carries RapidHash machinery that could
   cheaply avalanche the user hash before power-of-two bucket reduction.

No remediation was performed as part of this audit.

### `<unordered_map>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- unordered, allocator-aware, and reversible container requirements
- complete `unordered_map` / `unordered_multimap` synopses
- construction, assignment, access, insertion, node handles, merge, and erasure
- lookup, buckets/local iterators, observers, hash policy, equality, complexity
- heterogeneous/range additions, deduction guides, PMR aliases, feature macros,
  repeated inclusion, and normal/replacement modes

Confirmed matches:

- principal map access, global/local iterator, lookup, observer, bucket,
  hash-policy, range, node extraction, erasure, and PMR interfaces are present.
- `try_emplace` avoids mapped construction for an existing key; successful
  insertions cache hashes and retain equivalent groups contiguously.
- geometric bucket growth and transparent heterogeneous operations provide the
  intended broad average-case behavior.
- repeated inclusion is idempotent.

Known defects:

1. Unguarded pre-operation node allocation leaks if hashing, equality, or
   rehash allocation throws. Construction of a multi-element table likewise
   leaks all previously linked nodes if a later insertion fails.

2. Node-handle insertion gives up ownership before the throwing hash/equality/
   growth path; failure empties the handle and leaks the node. Merge can lose
   an extracted source element through the same path.

3. All cross-hash, cross-predicate, and `unordered_map`/`unordered_multimap`
   merge overloads required by C++23 are absent.

4. Allocator-only constructor forms and deduction guides are incomplete, and
   inherited initializer-list assignment returns the implementation base
   rather than `unordered_map&` or `unordered_multimap&`.

5. `unordered_multimap` equality checks each left element only for the
   existence of some equal element in the right equivalent-key group. It does
   not consume matches, so `{(k,1),(k,1)}` can compare equal to
   `{(k,1),(k,2)}`, contrary to the required group permutation comparison.

6. Node handles impose default construction on the allocator and use
   unconditional `noexcept` allocator move/swap operations, repeating the
   ordered-container node-handle defects.

Library-quality defect (non-normative):

1. The power-of-two bucket sequence consumes raw low hash bits with no
   avalanche step. This unnecessarily magnifies weak-bit patterns even though
   the library's RapidHash implementation could mix the supplied hash before
   bucket selection.

No remediation was performed as part of this audit.

### `<queue>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `queue` and `priority_queue` synopses
- constructors, allocator-aware construction, deduction guides, and traits
- access, push/pop/emplace, C++23 `push_range`, swap, and comparisons
- heap construction/restoration, constraints, effects, and complexity
- container-adaptor formatter specializations, feature macros, repeated
  inclusion, and normal/replacement modes

Confirmed matches:

- the principal queue and priority-queue overload sets, protected underlying
  state, iterator/range construction, allocator construction, accessors, and
  modifiers are present.
- priority-queue construction and range insertion restore the heap using the
  specified broad `make_heap`/`push_heap`/`pop_heap` operations.
- ordinary queue comparisons forward to the underlying container, and the
  conditional swap specifications account for container/comparator swapping.
- repeated inclusion is idempotent.

Known defects:

1. The mandatory C++23 `formatter` specializations for `queue` and
   `priority_queue` are absent. Neither adaptor can be formatted through the
   required range-based adaptor formatting interface.

2. Allocator deduction-guide participation is approximated by checking only
   for a `value_type` and an `allocate(n)` expression. This accepts types that
   do not meet the standard's allocator detection requirements and makes the
   guides participate in calls where they must be excluded.

3. The default `deque`-backed queue inherits `<deque>`'s audited construction,
   insertion, and exception-resource defects; the default vector-backed
   priority queue inherits `<vector>`'s growth rollback and self-aliasing
   defects.

No remediation was performed as part of this audit.

### `<stack>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete stack synopsis, construction, and allocator-aware construction
- deduction guides, uses-allocator specialization, access, and modifiers
- C++23 `push_range`, comparisons, swap, constraints, and exception specs
- adaptor formatting, feature macros, repeated inclusion, and both modes

Confirmed matches:

- the required underlying-container model, principal constructor inventory,
  accessors, modifiers, comparisons, swap, deduction guides, and allocator
  trait specialization are present.
- `push_range` uses `append_range` when available and otherwise appends through
  the container's back insertion surface as required.
- repeated inclusion is idempotent.

Known defects:

1. The required C++23 `formatter<stack<T, Container>, charT>` specialization is
   missing.

2. The allocator deduction guides use an underconstrained expression test
   (`value_type` plus `allocate`) instead of the standard allocator detection
   condition, so non-allocator types can incorrectly select allocator guides.

3. The default deque-backed specialization inherits the audited `<deque>`
   exception-safety and resource-ownership failures.

No remediation was performed as part of this audit.

### `<flat_set>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `flat_set` / `flat_multiset` synopses and adaptor requirements
- underlying-container, comparator, iterator, and invariant requirements
- all constructor/tag/range/allocator forms and deduction guides
- insertion, erasure, lookup, extraction, replacement, comparisons, and swap
- exception invariant restoration, complexity, traits, feature macros,
  repeated inclusion, and both namespace modes

Confirmed matches:

- the principal C++23 type inventory, random-access const iterator model,
  sorted tags, observers, lookup, erasure, extraction/replacement, PMR-agnostic
  uses-allocator trait, and free operations are present.
- ordinary construction normalizes unsorted input, removes comparator-equivalent
  duplicates for the unique adaptor, and successful modifiers preserve sorted
  order.
- exception handlers deliberately empty the adaptor where necessary, which is
  permitted as a means of restoring the invariant.
- repeated inclusion is idempotent.

Known defects:

1. Iterator-range and range insertion insert into the underlying sequence one
   element at a time. With the default vector this takes quadratic work, rather
   than the required `N + M log M` complexity. The overloads carrying
   `sorted_unique_t` or `sorted_equivalent_t` also ignore the sorted-input
   opportunity and fail their linear complexity requirement.

2. The initializer-list assignment operator is inherited from the internal
   base and returns `flat_set_base&`, not the required `flat_set&` or
   `flat_multiset&`.

3. Non-member ordering uses `compare_three_way` directly rather than the
   specified synthesized three-way operation, rejecting valid legacy-ordered
   key types without `<=>`.

4. Allocator detection for deduction-guide disambiguation checks only for a
   nested `value_type`, so unrelated types can be classified as allocators and
   select or suppress the wrong guides. Several allocator-bearing iterator,
   range, and initializer-list deduction forms are also absent.

No remediation was performed as part of this audit.

### `<flat_map>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `flat_map` / `flat_multimap` synopses and adaptor requirements
- parallel key/mapped containers, proxy iterators, and invariant requirements
- constructor/tag/range/allocator forms, deduction guides, and traits
- access, insertion, assignment, erasure, lookup, extraction, and replacement
- comparisons, swap, exception restoration, complexity, feature macros,
  repeated inclusion, and both namespace modes

Confirmed matches:

- principal type aliases, proxy reference/iterator surface, sorted tags,
  accessors, observers, heterogeneous operations, extraction/replacement, and
  free operations are present.
- successful operations keep key and mapped sequences aligned and sorted;
  failure paths generally clear both sequences to restore the required
  invariant when one underlying-container operation has already succeeded.
- ordinary unsorted construction computes a key ordering and moves aligned
  key/mapped values into normalized storage.
- repeated inclusion is idempotent.

Known defects:

1. General and sorted iterator/range insertion repeatedly performs binary
   search followed by a linear insertion into each underlying sequence. This
   is quadratic for default vectors and violates both the general
   `N + M log M` bound and the sorted-input linear bound.

2. Inherited initializer-list assignment returns the internal base type rather
   than `flat_map&` or `flat_multimap&`.

3. Relational ordering calls `compare_three_way` directly instead of using the
   required synthesized fallback, so valid key/mapped types supporting only
   legacy ordering make the operation ill-formed.

4. Allocator-like detection is only a nested-`value_type` test, and the
   deduction-guide inventory omits allocator-bearing iterator, range, and
   initializer-list forms required by the synopsis.

5. The default paired storage inherits `<vector>`'s audited growth rollback
   and self-aliasing defects; these are observable through adaptor insertion
   before its catch-and-clear recovery can provide the operation required by
   the underlying container.

No remediation was performed as part of this audit.

### `<span>`

**Status:** CERTIFIED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete C++23 synopsis, element/extent mandates, and specializations
- all iterator/count, iterator/sentinel, array, range, and converting constructors
- deduction guides, explicitness, constraints, and lifetime/borrowing rules
- compile-time and runtime subviews, observers, accessors, and iterators
- `as_bytes`, `as_writable_bytes`, representation extents, and cv restrictions
- range enablement, feature macro, freestanding status, repeated inclusion, and
  normal/replacement modes

Confirmed matches:

- fixed and dynamic storage, construction/convertibility rules, deduction,
  view/borrowed-range enablement, and the complete observer/access surface match
  C++23.
- subview result extents and pointer/size computations follow the static and
  dynamic formulas, with the standard precondition domain left to callers.
- byte views compute representation extents correctly and reject volatile or
  non-writable element types as required.
- iterator aliases, const/reverse access, `constexpr`/`noexcept`, feature-test
  macro, freestanding availability, and repeated inclusion match the audited
  requirements.

No remediation was required.

### `<mdspan>`

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete `extents`, `dextents`, layouts, accessor, and `mdspan` synopses
- index/extents mandates, constructor constraints, conversions, and deduction
- left/right/stride mapping requirements, offsets, span sizes, and observers
- accessor semantics and multidimensional element access
- static/runtime uniqueness, exhaustiveness, striding, comparison, and swap
- feature macro, freestanding status, repeated inclusion, and both modes

Confirmed matches:

- the major C++23 facility inventory is present, including rank-zero handling,
  dynamic extent storage, all three standard layouts, default accessor, mdspan
  construction/access, and deduction guides.
- left/right offset and stride formulas, stride-layout span-size and
  exhaustiveness logic, mapping comparisons, and multidimensional access match
  the standard formulas over their specified precondition domains.
- explicitness and nothrow-conversion conditions broadly follow the extents and
  mdspan constructor requirements.
- the header is freestanding and repeated inclusion is idempotent.

Known defect:

1. `extents` recognizes index types through FTL's incomplete `is_integral_v`.
   Implementation-provided extended signed and unsigned integer types are
   valid mdspan index types, but are rejected by the class mandate. This is the
   shared extended-integer defect recorded for `<type_traits>` and affects
   `extents`, `dextents`, every standard mapping, and `mdspan` itself.

No remediation was performed as part of this audit.

### `<iterator>` and range access

**Status:** FAILED

**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`

Reviewed:

- complete C++23 `<iterator>` synopsis and associated-type machinery
- iterator traits, concepts, C++17 compatibility requirements, and tags
- `ranges::iter_move`, `ranges::iter_swap`, and indirect callable/algorithm
  concepts
- classic and ranges iterator primitives
- reverse, insert, const, move, common, counted, and sentinel adaptors
- stream iterators and stream-buffer iterators
- range-access CPOs, range aliases/concepts, view enablement, and `from_range`
- constraints, overload participation, complexity, feature macros,
  freestanding surface, repeated inclusion, and normal/replacement modes

Confirmed matches:

- the broad C++23 public inventory is present, including C++23 constant
  iterators/sentinels, stream iterators, range access, and the indirect
  callable and algorithm concepts.
- pointer traits, ordinary iterator-trait synthesis, `iter_move`'s principal
  ADL/dereference ordering, range begin/end/size/empty selection, and the
  successful paths of the common iterator adaptors broadly follow the standard.
- stream iterator extraction/insertion, buffer advancement, end detection, and
  output failure tracking match their principal specified effects.
- repeated inclusion is idempotent.

Known defects:

1. `indirectly_readable` checks only relationships among associated types. It
   omits the required expressions on a `const` iterator proving that `*i` and
   `ranges::iter_move(i)` actually have those types. Iterators with a missing
   or different const dereference can therefore satisfy the concept.

2. `contiguous_iterator` tests `addressof(*i)` instead of requiring
   `to_address(i)`. Random-access iterators with no valid contiguous
   `to_address` customization can be misclassified as contiguous, and
   `ranges::data` repeats the wrong address-of-dereference fallback.

3. `ranges::iter_swap` implements only ADL `iter_swap` and the final
   move/exchange fallback. It omits the standard intermediate
   `ranges::swap(*i1, *i2)` branch. Swappable proxy references that are not
   indirectly-movable-storable consequently fail or take the wrong operation.

4. Classic `advance` and `distance` dispatch with C++20 iterator concepts
   instead of the `iterator_category` required by their C++17 iterator domain.
   A valid legacy bidirectional/random-access iterator that does not model the
   corresponding C++20 concept gets incorrect negative-distance behavior or
   linear rather than constant complexity.

5. `common_iterator` default construction activates its sentinel member. The
   specified state is a value-initialized iterator alternative, so equality,
   dereferenceability state, and subsequent behavior differ immediately.

6. `common_iterator::operator->` has no readable-value proxy. When dereference
   returns a prvalue and the underlying iterator lacks `operator->`, FTL tries
   to take the address of that temporary. Its input-only postincrement likewise
   returns the underlying `i++` directly instead of supplying the required
   postfix proxy when the old readable value must be preserved.

7. `common_iterator` changes union alternatives by destroying the old member
   before constructing the new one, without a valueless-state guard. If the
   construction throws, its discriminator names an inactive member and later
   destruction has undefined behavior. Placement-new use also prevents the
   promised constant-evaluation behavior on the supported C++23 model where
   the standard facility uses constexpr-capable variant state management.

8. Reverse-iterator comparison overloads and several move/common/counted
   adaptor customizations lack the synopsis constraints. They appear to
   participate for incompatible underlying iterator pairs and fail later in
   function bodies rather than being excluded from overload resolution.

9. Normal-mode `basic_const_iterator` and `move_iterator` ordering routes
   through FTL's already-failed `compare_three_way` implementation, while
   replacement mode uses direct constrained `<=>`. This gives the adaptors
   different participation and pointer/fallback semantics between modes.

10. Counted-iterator concept/category propagation consults only member types
    on the underlying iterator in its primary class bases. Valid iterators whose
    category is synthesized by `iterator_traits` lose the required adaptor
    category/concept surface.

11. Integer-like range-size detection and signed-distance aliases inherit
    `<type_traits>`'s rejection of implementation-provided extended integer
    types, excluding valid size results and breaking `ssize` type formation.

No remediation was performed as part of this audit.

### Audit record: `<ranges>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 26.2--26.7

The audit covered the range concepts and utilities, view interface and closure
machinery, range factories, all C++23 range adaptors, and `ranges::to`. The
declared facility inventory and feature-test macros are present, but the
following defects prevent certification:

1. Range concepts and view constraints inherit `<iterator>`'s incomplete
   `indirectly_readable`, contiguous-addressing, iterator-category, and adaptor
   comparison rules. Consequently otherwise-valid proxy and fancy-pointer
   ranges can be rejected or given the wrong operations throughout the view
   surface.
2. `subrange`, `iota_view`, zip-family sizing, and related size conversions
   implement the standard's integer-like machinery with `make_signed` and
   `make_unsigned`. Because FTL's arithmetic traits omit implementation-provided
   extended integers, valid extended-integer bounds, differences, and sizes are
   rejected or fail during substitution.
3. `views::counted` selects its contiguous result using the broken FTL
   `contiguous_iterator` concept. An iterator whose required address is supplied
   by `to_address` but differs from `addressof(*i)` is therefore misclassified,
   so this factory does not reliably produce the mandated `span` result.
4. View iterators that delegate movement, swapping, or ordering to the shared
   iterator facilities inherit the missing proxy `iter_swap` branch and the
   normal/replacement-mode comparison mismatch. The same valid view pipeline
   can therefore have different participation or fail only in one namespace
   mode.

No separate material library-quality defect was identified beyond the shared
iterator machinery during this pass.

### Audit record: `<algorithm>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 27.2--27.8

Reviewed against the complete header synopsis and the standard's semantic
families:

- every algorithm result class and alias
- non-modifying sequence operations, including C++23 contains, find-last,
  starts/ends-with, and fold families
- copying, moving, transforming, replacing, removing, permutation, sampling,
  and shifting operations
- partition, sorting, selection, binary-search, merge, set, heap, min/max,
  lexicographical, and permutation operations
- classic, execution-policy, iterator/sentinel, and range overload sets
- complexity, stability, return/dangling behavior, feature-test macros, and
  normal/replacement namespace modes

The audit found:

1. The required public alias template `ranges::minmax_result<T>` is absent.
   FTL defines `min_max_result<T>` and returns it directly from the range
   min/max callables, but the named synopsis member is independently required.
2. Every mutating or sorting implementation that uses `ranges::iter_swap`
   inherits the shared CPO's missing `ranges::swap(*i1, *i2)` proxy branch.
   Valid permutable proxy iterators can therefore satisfy the algorithm's
   specified constraints and still fail in reverse, rotate, partition, heap,
   sort, selection, shuffle, and permutation operations.
3. Range algorithm participation inherits the incomplete
   `indirectly_readable` checks from `<iterator>`. This admits some iterators
   without all required const-dereference common-reference relationships and
   rejects other valid proxy iterators, so the constrained overload surface is
   not equivalent to the synopsis.
4. The default `lexicographical_compare_three_way` delegates to FTL's
   nonconforming `compare_three_way`; its pointer total ordering and comparison
   participation therefore inherit the defects recorded for `<compare>`.
5. Algorithms that use the shared iterator movement primitives inherit their
   category-dispatch defect. In particular, valid legacy iterators whose
   C++20 concept classification differs from `iterator_category` can receive
   behavior or complexity different from the classic-algorithm requirements.

Library-quality observation: `stable_sort` and `stable_partition` always use
the allocation-free recursive/rotation fallback. This is a valid low-memory
strategy, but it leaves their higher comparison/swap cost in place even when
auxiliary storage is readily available, unlike the usual adaptive standard
library implementations.

### Audit record: `<numeric>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 27.9--27.10

Reviewed the complete generalized numeric-operation synopsis: accumulate,
reduce, inner product, transform-reduce, all scan variants, adjacent difference,
classic and ranges iota, gcd/lcm, midpoint, execution-policy overloads,
mandates, overlap rules, ordering latitude, complexity, feature-test macros,
and both namespace modes.

The audit found:

1. `gcd` and `lcm` use FTL's incomplete `is_integral` and `make_unsigned`
   machinery. They reject implementation-provided extended integer types even
   though those are integer types covered by the mandates.
2. The pointer overload of `midpoint` evaluates `(b - a) / 2`. For two elements
   of the same array whose distance is not representable in `ptrdiff_t`, that
   subtraction has undefined behavior, but the library contract still requires
   the pointer to element `i + (j-i)/2`. The implementation therefore does not
   meet the no-overflow intent of the facility on sufficiently large arrays.
3. `ranges::iota` and policy/generalized operations inherit the iterator
   concept and category defects recorded for `<iterator>`, including incorrect
   participation for extended difference types and some proxy iterators.

The sequential implementation of policy reductions/scans is permitted by the
execution-policy latitude. No separate material library-quality defect was
identified in this header.

### Audit record: `<memory>` utilities, allocators, and ownership

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 20.2--20.3, 20.6--20.7, 20.9--20.11

Reviewed address/alignment and explicit-lifetime utilities, uses-allocator
construction, pointer and allocator traits, `allocator`, unique/shared/weak
ownership, casts, deleter access, owner ordering, atomic smart pointers,
`make`/`allocate` factories including arrays and overwrite variants, and
`out_ptr`/`inout_ptr` adaptation.

Certification fails because:

1. `start_lifetime_as` and `start_lifetime_as_array` are only
   `reinterpret_cast`s. They do not perform the implicit object creation,
   representation copying, or lifetime-start operation specified by 20.2.6,
   so use on storage that does not already contain live `T` objects remains
   undefined despite the advertised C++23 facility.
2. Non-array `unique_ptr` converting assignment tests convertibility from
   `U*` instead of `unique_ptr<U, E>::pointer`, rejecting or accepting the
   wrong fancy-pointer specializations. Its member and non-member `swap` also
   expose a conditional exception specification instead of the required
   unconditional `noexcept` contract.
3. Converting `shared_ptr` and `weak_ptr` operations use only conversion between
   the two `element_type*` types. This loses the standard's bounded/unbounded
   array compatibility rules and, for example, admits conversions from an
   unknown-bound owner to an incompatible known-bound specialization.
4. Construction of a `shared_ptr` from `unique_ptr` releases the source inside
   a delegating-constructor argument list. If another argument evaluation (such
   as moving a throwing deleter) fails after `release()`, no owner remains to
   reclaim the pointer. Related raw-pointer construction can call a deleter
   after it has been moved from when control-block construction throws, rather
   than guaranteeing the specified cleanup with the supplied deleter.
5. Every array `allocate_shared` and `allocate_shared_for_overwrite` overload
   discards its allocator argument and delegates to `make_shared`. Storage is
   obtained with ordinary `new`/`operator new`, so allocator selection,
   construction, and deallocation requirements are all violated. The manual
   value-filled array paths additionally multiply `sizeof(element) * count`
   without overflow checking and use unaligned `operator new` for potentially
   over-aligned elements.
6. `owner_before` compares unrelated control-block pointers with built-in `<`.
   That comparison has no required total ordering for unrelated objects and
   therefore does not provide the implementation-defined strict weak ownership
   ordering required across shared and weak pointers.
7. `allocator<T>::allocate` uses a constant-evaluation `new T[count]` path only
   for trivially default-constructible `T`. For other object types it directly
   calls `::operator new`, which is not a permitted constant-expression
   allocation in this context, despite the C++23 `constexpr` allocator
   contract.

Library-quality observations:

- Ordinary raw-pointer `shared_ptr` construction always allocates a separate
  virtual control block, and destruction/deallocation always dispatch through
  virtual functions. This is conventional for erased deleters but leaves no
  small/control-block pooling path for common default-deleter ownership.
- Atomic shared/weak pointers serialize all operations on a per-object lock;
  this is conforming because they report `is_lock_free() == false`, but provides
  no lock-free fast path on platforms with double-width atomics.

### Audit record: `<memory>` specialized algorithms

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 27.11

This record covers only the Clause 27 raw-memory algorithms; the remaining
`<memory>` facilities retain their own audit scope.

The classic uninitialized construction, copy, move, fill, construction-at, and
destruction families are present with rollback loops. The remediation is large
because the complete C++23 ranges counterpart is absent:

- exposition-only nothrow input/forward iterator and range constraints
- `ranges::uninitialized_default_construct` and `_n`
- `ranges::uninitialized_value_construct` and `_n`
- `ranges::uninitialized_copy`, `_n`, and their result aliases
- `ranges::uninitialized_move`, `_n`, and their result aliases
- `ranges::uninitialized_fill` and `_n`
- `ranges::construct_at`
- `ranges::destroy_at`, `destroy`, and `destroy_n`

Additional defects found in the implemented classic surface:

1. `destroy_at`, `destroy`, and `destroy_n` are declared unconditionally
   `noexcept`, whereas the classic C++23 declarations are potentially throwing.
   A throwing destructor therefore terminates instead of propagating from these
   overloads.
2. Classic default construction forms the object type from
   `remove_reference_t<decltype(*current)>` rather than the specified
   `iterator_traits<NoThrowForwardIterator>::value_type`; valid legacy iterator
   types for which those differ receive the wrong construction semantics.
3. `construct_at` adds a conditional `noexcept` specification absent from the
   standard declaration, making its observable exception specification depend
   on the constructor.

No separate material library-quality defect was identified in this subunit.

### Audit record: `<cstdlib>` / `<stdlib.h>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 17.2.2, 17.5, 20.2.12, 23.5.6,
27.12, 28.5.10, 28.7.2; ISO/IEC 9899:2018 7.22

The complete macro/type inventory, termination and environment functions,
allocation, numeric and multibyte conversions, C algorithms, low-quality random
number generator, arithmetic overloads, and C compatibility exports were
reviewed.

Certification fails because:

1. FTL imports one pair of C-runtime `bsearch`/`qsort` declarations whose
   comparator parameter has C language linkage. C++23 specifies an additional
   overload of each function whose comparator has C++ language linkage, and
   requires exceptions from that comparator to propagate. Those overloads are
   absent on implementations that distinguish the function types. The dual
   C/C++-linkage `atexit` and `at_quick_exit` handler overloads are likewise not
   supplied.
2. The required floating-point `abs` overload is absent from `<cstdlib>`.
   Only the three integral overloads are declared; the floating overload exists
   separately in `<cmath>`, which this header neither includes nor re-exports.
3. Except for MSVC namespace wrappers, `atexit` and `at_quick_exit` are imported
   from declarations lacking the required `noexcept`. On Apple, `abort`,
   `_Exit`, and `quick_exit` also deliberately omit their required `noexcept`
   specifications, so the public function types do not match the synopsis.

The C-linkage overloads otherwise delegate their search/sort semantics and
complex object-representation handling to the platform runtime. The allocation,
conversion, and low-quality random facilities similarly delegate where
appropriate. No separate material library-quality defect was identified.

### Audit record: `<cfenv>` / `<fenv.h>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 28.3; ISO/IEC 9899:2018 7.6

The complete macro, type, and function inventory is present for the supported
x86-64 and AArch64 targets. Rounding modes, saved environments, status flags,
default environment restoration, per-thread hardware state, C linkage, and
normal/replacement namespace exposure were reviewed.

`feraiseexcept` is nonconforming when exception traps are enabled. It merely
sets the x87/MXCSR or FPSR sticky status bits and restores/writes the register;
it does not execute or otherwise trigger the requested floating-point
exception. Consequently an enabled exception can fail to trap even though the
function reports success. `feupdateenv`, which restores an environment and then
calls `feraiseexcept`, inherits the same defect for exceptions saved from the
previous environment.

No separate material library-quality defect was identified.

### Audit record: `<complex>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 28.2, 28.4

Reviewed the class template and conversions, arithmetic and comparison
operators, scalar overloads, value operations, transcendental functions,
stream extraction/insertion, literals, feature-test macros, and the supported
floating specializations in both namespace modes.

The audit found:

1. The required converting assignment
   `template<class X> complex& operator=(const complex<X>&)` is absent. Only the
   same-type implicit copy assignment and converting compound assignments are
   available.
2. The arithmetic convenience overloads (`real`, `imag`, `arg`, `norm`,
   `conj`, `proj`, and heterogeneous `pow`) inherit the incomplete arithmetic
   traits and reject implementation-provided extended integer and floating
   types that meet their specified arithmetic-type participation.
3. Formatted complex insertion builds a custom temporary stream but copies only
   flags, locale, and precision. The specified `basic_ostringstream`/`copyfmt`
   route also transfers fill character, width, and registered formatting
   callbacks; observable user-defined formatting state is therefore lost or
   applied at the wrong stage.

Library-quality observation: complex multiplication uses the direct
`ac-bd`/`ad+bc` formula without scaling or special-value recovery, causing
avoidable intermediate overflow and NaN results for large finite operands.

### Audit record: `<random>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 28.2, 28.5

Reviewed:

- uniform-random-bit-generator requirements
- `seed_seq`, required generation transform, and parameter recovery
- linear-congruential, Mersenne Twister, and subtract-with-carry engines
- discard-block, independent-bits, and shuffle-order adaptors
- predefined engines and their required sequences
- `random_device` and supported platform entropy paths
- `generate_canonical`
- every uniform, Bernoulli, Poisson, normal, sampling, and piecewise
  distribution, including parameter objects, state/reset, bounds, equality,
  serialization, and range/statistical behavior
- normal and replacement modes and header interactions

The audit found:

1. Engine templates, `seed_seq`'s initializer-list constructor, and every
   integer distribution implement the standard's integer-type mandates with
   FTL's incomplete `is_integral`/`is_unsigned`/`make_unsigned` traits.
   Implementation-provided extended integer types are rejected throughout the
   facility.
2. Real-valued distributions similarly inherit incomplete floating-point type
   classification for implementation-provided extended floating types.
3. `generate_canonical` returns `result / factor` without guarding against
   floating-point rounding to exactly `1`. Its specified result is in `[0,1)`;
   the direct function can therefore violate its postcondition even though
   `uniform_real_distribution` separately clamps its own result.
4. Seed-sequence and engine constraints recognize any type with a suitably
   shaped `generate` expression rather than the complete seed-sequence
   requirements. Constructors and `seed` members can participate for types
   that do not satisfy the required result type, unsigned 32-bit production,
   and non-convertibility relationships.

Library-quality observations:

- modular multiplication performs a bit-at-a-time add/double loop for every
  linear-congruential step instead of using available widened multiplication;
  `discard(n)` also advances every engine one value at a time instead of using
  skip-ahead arithmetic where available
- `discrete_distribution` linearly scans every probability on every draw;
  maintaining a cumulative binary-search table or alias table would avoid
  pathological cost for large fixed distributions
- several compound distributions reconstruct helper distributions per sample,
  discarding reusable setup and cached normal variates

### Audit record: `<valarray>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 28.2, 28.6

Reviewed the complete value-array interface, unary/binary/math operations,
shifts, reductions, resize/apply, slice and generalized-slice descriptors,
slice/mask/indirect proxy classes, subset ordering and assignment, non-member
access/swap, size preconditions, and both namespace modes.

The audit found:

1. The value, pointer, initializer-list, copy, and proxy constructors allocate
   an array and then assign elements in the constructor body. If an element
   assignment throws, the `valarray` destructor is not entered and the raw
   allocation is leaked, violating the library-wide no-resource-leak rule.
2. `end(valarray&)` and its const overload compute `data() + size()`. The empty
   representation stores a null pointer, so even the zero offset performs
   pointer arithmetic on null rather than producing a valid empty iterator
   range.
3. `shift` and `cshift` convert `size_t` indices and the array size to
   `long long`. Arrays whose size exceeds `LLONG_MAX` produce
   implementation-defined conversions and incorrect indexing, although the
   specified operations are expressed over the full `size_t` domain.
4. Construction and resizing value-initialize all storage and then assign the
   requested values. Besides the leak above, this imposes default construction
   on paths whose specified element initialization only requires construction
   from the supplied value; valid value types without a default constructor are
   rejected.

Library-quality observations: every arithmetic expression eagerly allocates
and fills a full temporary, and every slice/mask access materializes a heap
allocated index `valarray`. The standard explicitly permits proxy/expression
representations, so fused evaluation and compact strided views could eliminate
substantial allocation and memory traffic.

### Audit record: `<numbers>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 28.8

All thirteen variable templates, their `double` aliases, namespace placement,
constant values, inline/constexpr properties, feature-test macro, and both
namespace modes were reviewed. The values carry sufficient source precision
for the supported `float`, `double`, and `long double` formats.

The variable templates enforce their floating-point mandate through FTL's
incomplete `is_floating_point`. Consequently implementation-provided extended
floating-point types are rejected even though the synopsis permits every
cv-unqualified floating-point type. This shared classification defect prevents
certification; no additional defect or material quality issue was found.

### Audit record: `<cmath>` core

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 28.2, 28.7.1--28.7.5; ISO/IEC 9899:2018 7.12

Reviewed the complete C and C++ overload inventory, common arithmetic argument
rules, floating evaluation types and macros, error reporting, classification
and quiet comparison functions, absolute values, three-argument `hypot`,
`lerp`, constexpr paths, special values, rounding modes, feature-test macros,
and both namespace modes.

The audit found:

1. All generic arithmetic overloads and their common-result calculation are
   limited to FTL's incomplete ordinary arithmetic traits and a hand-written
   `float`/`double`/`long double` rank table. Required overloads for
   implementation-provided extended integer and floating-point types are
   missing or choose the wrong common floating type.
2. `float_t` and `double_t` are fixed to `float` and `double` instead of
   following the implementation's floating-point evaluation method. Compiler
   modes with excess precision therefore expose the wrong typedefs.
3. The constexpr implementations of `fmod` and `remainder` first compute
   `x / y`. A finite ratio can overflow even though the required remainder is
   finite, producing infinity and then NaN instead of the specified result.
   `remquo` inherits the same reduction failure.
4. On Clang, constant-evaluated `fma` is implemented as `x * y + z`, with two
   roundings and an observable intermediate overflow, rather than the required
   fused operation with unbounded intermediate precision and one rounding.
5. `isgreater`, `isgreaterequal`, `isless`, `islessequal`, and
   `islessgreater` use ordinary C++ relational expressions. Those expressions
   can raise `FE_INVALID` for signaling NaNs, whereas these classification
   comparisons are required to perform their comparisons without that
   exception.
6. Constant-evaluated sign-bit inspection assumes particular little-endian
   object layouts and a fixed x87 sign-byte location. It is not derived from
   the supported type representation and is wrong for other conforming
   extended floating representations.

Library-quality observation: constexpr scaling and decomposition use
one-step-per-exponent loops rather than representation-based exponent
manipulation, causing avoidable compile-time work and evaluator-limit failures
for large exponents.

### Audit record: `<cmath>` mathematical special functions

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 28.7.6

The full C++23 special-function inventory is present: Laguerre, Legendre and
Hermite polynomials; beta; complete and incomplete elliptic integrals;
cylindrical and spherical Bessel/Neumann functions; exponential integral;
Riemann zeta; and spherical associated Legendre functions. The formulas,
domain gates, NaN handling, standard float/double/long-double overloads, and
arithmetic convenience overloads were reviewed.

Certification fails because:

1. Every generic overload again rejects implementation-provided extended
   floating and integer types through the incomplete arithmetic traits and
   rank machinery.
2. `beta` evaluates `exp(lgamma(x) + lgamma(y) - lgamma(x + y))` directly.
   For large finite positive arguments, `x + y` and the intermediate log-gamma
   terms can overflow to infinity, yielding `inf - inf` and NaN even when the
   mathematical beta value is a well-defined finite value or should underflow
   to zero with a range error.
Library-quality observation: several polynomial and series kernels use
unscaled forward recurrences. Large permitted orders can overflow intermediate
values or lose substantially more precision than scaled/backward recurrence
methods before the final result itself is out of range.

### Audit record: `<chrono>` durations, time points, and clocks

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 29.2--29.7

This foundational chrono subunit covers time-related traits, duration values,
duration construction/arithmetic/conversion/comparison, predefined durations,
duration literals and rounding algorithms, time points, clock requirements,
the system/steady/high-resolution clocks, and their conversions. Calendar,
time-zone, formatting, and parsing facilities remain separate audit subunits.

The audit found:

1. `duration::operator<=>` does not use the common representation's comparison
   result. It selects `partial_ordering` only when FTL recognizes the
   representation as floating point and otherwise forces `strong_ordering`,
   then implements comparison with two `<` expressions. Floating NaNs are
   consequently reported as equivalent rather than unordered, and custom
   representations with weak or partial ordering lose their required category
   and semantics.
2. `treat_as_floating_point` inherits the incomplete floating-point trait.
   Durations using implementation-provided extended floating representations
   are treated as integral, incorrectly disabling fractional conversions,
   modulo participation, rounding constraints, and comparison behavior.
3. `is_clock` becomes true whenever six nested names/expressions merely exist.
   It does not verify the required relationships among `rep`, `period`,
   `duration`, `time_point`, the exact `is_steady` type, and `now()`'s return
   type, so non-clock types are reported as clocks.
4. Duration common-type and conversion arithmetic inherits `<ratio>`'s
   nonconforming interface and overflow/domain handling, causing valid period
   combinations to fail or acquire an incorrect common period.

No separate material library-quality defect was identified in this subunit.

### Audit record: `<chrono>` civil calendar and `hh_mm_ss`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 29.8--29.10

Reviewed every civil-calendar field and composite type, named constants,
validity rules, conventional `/` syntax, day/month/year arithmetic, civil-day
conversion, weekday indexing, last-day calculations, `hh_mm_ss`, and the
12/24-hour utility functions.

The audit found:

1. `month += months`, `weekday += days`, `year += years`, and the composite
   month/year operations first add their underlying `int` counts. Valid
   operands near the representation limits can overflow signed `int` before
   modular reduction, producing undefined behavior instead of the specified
   wrapped calendar field.
2. The `year` arithmetic members similarly narrow the sum to `short` only
   after signed addition. Large representable `years` operands can overflow in
   the promoted expression rather than producing the implementation's required
   stored calendar value.
3. `hh_mm_ss` inherits the duration floating-representation classification
   defect. For an extended floating representation it takes the integral path,
   truncating subseconds through `duration_cast` instead of preserving the
   specified duration value.

No separate material library-quality defect was identified in this subunit.

### Audit record: `<chrono>` UTC/TAI/GPS/file clocks and clock conversion

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 29.7.3--29.7.6, 29.7.10, 29.11.8

Reviewed the UTC epoch and leap-second mapping, positive and negative leap
handling, TAI and GPS epochs, the implementation-defined file clock mapping,
all standard `clock_time_conversion` specializations, `clock_cast` route
selection and ambiguity, `leap_second_info`, and the `leap_second` interface.

Certification fails because `clock_cast` has no associated constraint. The
standard constrains the function to participate only when at least one of its
five candidate conversion expressions is well-formed, but this implementation
declares it unconditionally and checks availability with a `static_assert`
inside the function body. Consequently a requires-expression or other
immediate-context availability test incorrectly accepts an impossible clock
conversion; actually instantiating the body then produces a hard error instead
of normal constraint failure.

No separate material library-quality defect was identified in this subunit.

### Audit record: `<chrono>` time-zone database and `zoned_time`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 29.11.1--29.11.7, 29.11.9

Reviewed the generated IANA database and post-horizon rule evaluator, zone and
link lookup, system/local interval classification, ambiguous and nonexistent
local-time conversion, diagnostic exceptions, database-list lifetime and
synchronization, hosted current-zone discovery, remote database hooks,
`zoned_traits`, every `zoned_time` constructor/observer/assignment, deduction
guides, and `local_time_format`.

Certification fails because the name-taking `zoned_time` constructors do not
implement their specified constructibility constraints. They require only that
`traits::locate_zone(string_view{})` be a well-formed expression. C++23 instead
requires the relevant `zoned_time` construction from that expression's return
type (and the accompanying time/`choose` arguments) to be well-formed. A custom
`zoned_traits` whose lookup exists but returns an incompatible representation
therefore makes these constructors participate when the standard removes them;
using one then fails in the constructor body rather than by constraint.

No separate material library-quality defect was identified in this subunit.

### Audit record: `<chrono>` formatting

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 22.14, 29.12

Reviewed the formatter specializations for durations, `hh_mm_ss`, every civil
calendar type, system/local/UTC/TAI/GPS/file time points, local-time metadata,
`sys_info`, `local_info`, and `zoned_time`; the chrono format grammar,
field-availability checks, localization, padding, dynamic width/precision,
negative durations, and all conversion-specifier dispatch were also traced.

Certification fails because:

1. The chrono format parser rejects a nonempty `chrono-specs` unless its first
   character is `%`. The grammar permits any sequence of conversion specs and
   literal characters, including a literal first, so valid specifications such
   as a textual prefix followed by `%Y` throw `format_error`.
2. Localized time-of-day formatting is not implemented. Even with the `L`
   option, `%p` and `%r` unconditionally emit the hard-coded C-locale `AM` or
   `PM` representation, and `E`/`O` alternatives are discarded. C++23 requires
   these locale-dependent replacements to use the formatting locale.
3. Chrono precision participation inherits the incomplete floating-point
   classification: durations with implementation-provided extended floating
   representations reject a precision specification even though the standard
   permits it for floating-point `rep` types.

Library-quality observation: each chrono formatter materializes the entire
formatted value in an allocating `basic_string` before copying it to the format
context. This defeats allocation-free output iterators and makes
`formatted_size` pay for full formatting and storage rather than direct
counting, the same architecture-level issue recorded for `<format>` generally.

### Audit record: `<chrono>` parsing

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 29.13

Reviewed the `from_stream` overload set for durations, system/local/UTC/TAI/GPS
time points, calendar fields and dates; parse manipulators; width and modifier
grammar; field consistency, week-date and ordinal-date resolution; fractional
seconds, offsets, abbreviations, stream-state changes, and transactional output
assignment.

Certification fails because locale-dependent composite conversions are
hard-coded to the C locale. `%c`, `%x`, `%X`, and `%r` recursively parse fixed
English/C-locale layouts regardless of the stream's locale, while C++23
requires the locale's date/time and 12-hour representations and requires the
`E` forms to use its alternate representations. The parser likewise discards
most `E` and `O` modifiers on numeric fields instead of interpreting the
locale's alternative representations.

No separate material library-quality defect was identified in this subunit.

### Audit record: `<locale>` localization library

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 16.4.6.10, 30.1--30.4, D.27

Reviewed locale identity, naming, composition and category replacement; facet
ownership and lookup; classic and named ctype, codecvt, numeric punctuation,
numeric conversion and collation; time parsing/formatting; monetary punctuation
and conversion; message catalogs; standard specializations and convenience
interfaces. The native named-locale boundaries and RapidHash-based collation
hashing were included in the review.

Certification fails because:

1. The process-global C++ locale is a plain shared pointer. Default locale
   construction reads it while `locale::global` replaces and releases it with
   no lock or atomic publication. Concurrent calls therefore have a data race
   and can retain a state after another thread has freed it, contrary to the
   library-wide data-race requirements.
2. Both classic and named `time_get` perform case-sensitive weekday and month
   matching, while the required parsing is case-insensitive. The classic
   matcher accepts only its exact English capitalization.
3. `time_get_byname` overrides localized month and weekday names but not the
   locale's date order, date format, time format, AM/PM form, or `E`/`O`
   alternatives. Its inherited `%x`, `%X`, `%c`, and related operations remain
   fixed C-locale layouts for every named locale.
4. `money_get` consumes thousands separators whenever grouping is nonempty but
   never records or verifies their placement against `moneypunct::grouping()`.
   C++23 requires optional separators, when present, to be checked after all
   format components have been read and to set `failbit` when misplaced.
5. The `long double` monetary overload copies at most 511 parsed digits into a
   fixed buffer and treats conversion of that prefix as success. Valid longer
   monetary sequences therefore silently produce the value of a truncated
   input. Conversely `money_put(long double)` formats into a fixed 512-byte
   buffer and silently emits a truncated prefix for finite `long double`
   values whose integral representation is longer.
6. The C++23 deprecated `<locale>` surface is incomplete: class templates
   `wstring_convert` and `wbuffer_convert` are entirely absent. Deprecation does
   not remove these required interfaces from the C++23 library.

Library-quality observation: `has_facet` and `use_facet` linearly scan a vector
of facet-ID pointers on every call. Stream numeric, monetary, time, formatting,
and character operations repeatedly pay this search; assigning each
`locale::id` an index would provide the conventional constant-time hot path.
The collation hash itself does use RapidHash over either the source range or
the named locale's transformed collation key and is not a quality finding.

### Audit record: `<codecvt>` deprecated Unicode conversion facets

**Status:** CERTIFIED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, D.26

The deprecated C++23 header synopsis, `codecvt_mode`, and the
`codecvt_utf8`, `codecvt_utf16`, and `codecvt_utf8_utf16` templates were
checked for Unicode scalar validation, UCS versus UTF-16 element semantics,
maximum-code enforcement, header generation/consumption, byte order,
incremental state, partial/error positioning, `length`, `encoding`,
`always_noconv`, and `max_length`. The audited implementation matches the
required C++23 surface and behavior. No material library-quality defect was
identified.

### Audit record: `<clocale>` / `<locale.h>`

**Status:** CERTIFIED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 17.14.2, 30.5; ISO/IEC 9899:2018 7.11

The C and C++ header spellings expose the platform-ABI category macros,
complete platform-layout `lconv`, and C-linked `setlocale` and `localeconv`.
Namespace placement, replacement routing, C-runtime ownership, and the
platform-specific glibc, UCRT/BSD, and Darwin field ordering were reviewed.
The surface matches the supported C++23 targets. No material library-quality
defect was identified.

### Audit record: `<iosfwd>` / `<ios>` iostream foundations

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 16.4.6.10, 31.2--31.5

The forward declaration and alias inventory, positioning types, `ios_base`
flags/state/modes, failures, callbacks, locale and format state, extensible
storage, `copyfmt`, `basic_ios`, manipulators, error reporting, and move/swap
contracts were reviewed.

Certification fails in the allocation-failure path of `ios_base::iword` and
`pword`. Both return references to process-wide static fallback objects after
setting `badbit`, and reset those objects on each failure. Failures on distinct
stream objects can consequently race and overwrite one another's referenced
values, violating the required independence/data-race guarantees for distinct
stream objects. The fallback storage must at least be isolated per stream or
per thread for the lifetime promised by the storage functions.

No separate material library-quality defect was identified in this subunit.

### Audit record: `<streambuf>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 31.6

Reviewed the full public/protected interface, locale handling, copy/assignment
and swap, get/put area access, buffering and positioning, bulk and scalar
transfer, putback, and default virtual-function behavior.

Certification fails because protected `basic_streambuf::swap` exchanges only
the six get/put pointers and leaves each object's locale unchanged. C++23
requires it to exchange all data members; this includes the locale established
by `pubimbue`. Derived stream-buffer swaps therefore retain the wrong locale
while exchanging the associated character areas.

Library-quality observation: the default `xsgetn` and `xsputn` implementations
perform one virtual/scalar character operation per element even when a get or
put area contains a contiguous run. Bulk-copying available runs before falling
back to `underflow`/`overflow` would avoid substantial virtual-dispatch and
branch overhead without changing customization semantics.

### Audit record: `<istream>` / `<ostream>` formatted and unformatted I/O

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 31.7.1--31.7.2, 31.7.5--31.7.7

Reviewed sentries, formatted arithmetic and character/string operations,
stream-buffer transfer, every unformatted input/output member, character-array
constraints, rvalue forwarding operators, positioning, manipulators, `gcount`,
state transitions, and the required exception-catching/rethrow rules.

Certification fails because the implementation generally calls stream-buffer
virtual functions and locale facets directly, without the exception-catching
protocol required for formatted and unformatted I/O. For example `get`,
`getline`, `ignore`, `peek`, `read`, `readsome`, `putback`, `unget`, `sync`,
`put`, `write`, `flush`, and positioning operations allow an exception from a
custom buffer to escape without first setting `badbit`; formatted numeric
operations have the same problem for throwing facets. The standard requires
these functions to record the appropriate stream state and rethrow only when
the exception mask requests it. The stream-buffer insertion overload contains
a one-off catch path, but even that records `failbit` where its specified
exception path requires `badbit`.

No separate material library-quality defect was identified beyond the
stream-buffer scalar-transfer issue recorded in the preceding unit.

### Audit record: `<iomanip>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 31.7.3, 31.7.8--31.7.9

The flag/base/fill/precision/width manipulators, monetary and time
manipulators, and all quoted input/output overloads were reviewed. The direct
manipulator and quoted surfaces match their C++23 interfaces, but certification
fails because `get_money`, `put_money`, `get_time`, and `put_time` expose the
nonconforming locale facets recorded for `<locale>` and do not supply the
formatted-I/O exception/state protocol missing from the stream layer. Thus
mis-grouped monetary input is accepted, named time formats remain C-locale,
long monetary values truncate, and throwing facets can escape without the
required stream state.

Library-quality observation: quoted output always constructs a complete
escaped temporary string before inserting it, adding an avoidable allocation
and copy for an operation that can compute padding and emit delimiters/escapes
directly.

### Audit record: `<print>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 22.14, 31.7.4, 31.7.10

The complete C++23 FILE/stdout, variadic/type-erased, Unicode/non-Unicode, and
newline overload inventory is present. FILE error translation, terminal
detection, Windows UTF-16 console conversion, redirected/POSIX byte output,
and the feature-test macro were reviewed. Certification nevertheless inherits
`<format>`'s missing extended-arithmetic formatter participation, so otherwise
valid print calls for implementation-provided extended integer and floating
types are rejected.

Library-quality observation: every call first materializes the complete result
in an allocating `string`; `println` may then reallocate merely to append its
newline. Direct formatting to a buffered FILE sink would reduce peak memory,
copies, and latency for large output.

### Audit record: `<sstream>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 31.8

Reviewed the complete allocator-aware constructor and `str` overload sets,
view access, open modes, high-water tracking, get/put areas, growth, putback,
seeking, moves, swaps, and all three owning stream wrappers.

Certification fails because rebuilding the put area narrows its `size_t` put
position to `int` for `pbump`. A valid string buffer whose put position exceeds
`INT_MAX` therefore acquires an incorrect pointer after construction, growth,
seek, move, swap, or `str` replacement. The public sequence size is expressed
in `streamsize`/`size_t`; the protected `int` interface does not license
narrowing valid large buffers, so the implementation must advance in safe
chunks or otherwise preserve the full position. Extreme seek arithmetic also
computes `base + offset` in signed `off_type` before checking the range, allowing
undefined overflow instead of returning the failure position.

The owning streams additionally inherit the base stream-buffer locale-swap and
formatted/unformatted exception-state failures. No separate material
library-quality defect was identified.

### Audit record: `<spanstream>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 31.9

The C++23 span-buffer and input/output/bidirectional wrapper synopses, borrowed
read-only input adaptation, modes, span replacement/observation, positioning,
move, swap, and feature-test macro were reviewed. Certification fails for the
same unchecked `size_t`-to-`int` put-position narrowing and signed seek-addition
overflow as `<sstream>`. Spans can legally exceed `INT_MAX`, and valid positions
must not be corrupted merely because `basic_streambuf::pbump` takes `int`.
The wrappers also inherit the shared locale-swap and stream exception-state
defects. No separate material library-quality defect was identified.

### Audit record: `<fstream>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 31.10

Reviewed file-buffer ownership, open-mode mapping including `noreplace`, close
and codecvt unshift, buffering, narrow/wide input and output, putback,
read/write direction changes, locale state, seeking, move/swap, filesystem-path
overloads, and the three owning file-stream wrappers.

Certification fails because:

1. File positioning is implemented with `fseek` and `ftell` and narrows every
   `streamoff` to `long`. On supported 64-bit Windows, `long` is 32 bits, so
   valid offsets and file positions beyond that range are truncated or fail.
   The standard stream position types are not limited to the platform's
   32-bit `long` interface.
2. Update streams do not track and perform the required synchronization or
   positioning transition when changing between input and output. The
   implementation directly alternates `fgetc` and `fwrite`/`fputc` on one C
   update stream, which violates the C stream sequencing requirements and can
   produce undefined or stale-position behavior.
3. Wide-character `pbackfail` accepts only values at most `0x7f` and pushes a
   single byte with `ungetc`. It cannot restore a non-ASCII character's external
   multibyte sequence or conversion state, so ordinary successful wide input
   cannot be put back as required even when it is the immediately preceding
   character.

The wrappers also inherit the stream-buffer locale-swap and stream
exception-state failures. Library-quality observation: wide bulk output loops
through the full virtual conversion/write path one character at a time rather
than converting useful blocks, multiplying facet lookup and C-runtime calls.

### Audit record: `<syncstream>` and synchronized-output manipulators

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 31.7.7, 31.11

Reviewed synchronized-buffer construction, allocator propagation, pending
output, emission/flush state, wrapped-buffer lock identity, move/swap and
destruction; `basic_osyncstream`; and `emit_on_flush`, `noemit_on_flush`, and
`flush_emit`.

Certification fails because:

1. The three synchronized-output manipulators in `<ostream>` are stubs.
   `emit_on_flush` and `noemit_on_flush` never change a wrapped `basic_syncbuf`,
   and `flush_emit` merely calls `flush` instead of requesting and immediately
   performing emission.
2. `basic_osyncstream::emit` directly calls `sb_.emit()` without behaving as an
   unformatted output function. It does not construct a sentry and does not
   catch an exception from the wrapped buffer, set `badbit`, and apply the
   stream's exception mask as required.
3. The inherited protected buffer swap again leaves the locale behind while
   exchanging synchronized-buffer state.

Library-quality observation: lock identity is maintained by an append-only
process-global vector of individually heap-allocated entries. Every previously
seen wrapped-buffer address leaks permanently, and each first lookup scans all
prior addresses. A lifetime-safe keyed registry or lock ownership associated
with the wrapped buffer would avoid unbounded memory and lookup growth.

### Audit record: `<iostream>` standard stream objects

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 31.2.4, 31.4

Reviewed object definitions and initialization lifetime, required ties and
`unitbuf`, shared standard-error buffers, C-stream synchronization, final
flushing, narrow/wide buffer behavior, putback, and concurrent access.

Certification fails because:

1. The wide standard buffers are ASCII byte adapters. Wide output rejects every
   value above `0x7f`, and wide input reads one raw byte as one `wchar_t` rather
   than performing the associated C stream's multibyte/wide conversion.
   `wcin`, `wcout`, `wcerr`, and `wclog` therefore cannot provide ordinary
   locale-sensitive wide I/O.
2. Synchronized standard-stream operations have no stream-level locking.
   Although individual C FILE calls may be internally synchronized, formatted
   operations also read and modify shared C++ stream state such as width and
   error flags outside those calls. Concurrent insertions/extractions can race,
   contrary to the explicit thread-safety guarantee for the synchronized eight
   standard stream objects.

The objects additionally inherit the general formatted/unformatted exception
state defects. No separate material library-quality defect was identified.

### Audit record: `<filesystem>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 31.12, D.29

Reviewed path construction and conversion, pathname parsing/decomposition,
iteration and hashing, status and directory-entry caching, directory iterators,
recursive traversal, error reporting, and the filesystem operation set against
the supported Windows and POSIX native backends.

Certification fails because:

1. The template path constructors, assignments, appends, and concatenations are
   constrained only by incidental expressions such as `source.data()` or
   `iterator_traits<It>::value_type`. They accept sources outside the specified
   encoded-character `Source`/iterator domain and do not provide the required
   participation constraints.
2. Native Windows pathname decomposition recognizes only `/` as a directory
   separator. A native path such as `C:\\dir\\file` consequently has a drive
   root-name but no root-directory and is reported as relative, while its
   filename and parent decomposition retain the backslashes as ordinary
   characters. `make_preferred()` cannot repair this because normalization is
   empty and `preferred_separator` is unconditionally `/`.
3. Several overloads that report through `error_code&` allocate outside a
   catch boundary. For example, `absolute`, `canonical`, `current_path`, and
   `read_symlink` construct allocating strings/paths after the native call, and
   `weakly_canonical`, `create_directories`, `remove_all`, `relative`,
   `proximate`, and `copy` perform further allocating operations. Allocation
   failure can therefore escape instead of being reported through `ec` as
   required by the filesystem error-reporting rules.
4. `directory_iterator::open` constructs each yielded `directory_entry` through
   its refreshing path constructor. That constructor directly calls
   `refresh(error_code&)`, contrary to the explicit requirement that directory
   iterator constructors and non-const members not directly or indirectly call
   any `directory_entry` refresh function. The native enumeration callback's
   already available file-kind information is discarded.
5. `copy` silently returns success when its source resolves to
   `file_type::not_found`: `status` clears `ec` for that status, after which the
   early `!exists(source)` return leaves `ec` clear. Copying a nonexistent source
   is required to report an error.

Library-quality observations:

- `hash_value(path)` uses a handwritten byte-at-a-time FNV-1a-style hash even
  though the repository already provides RapidHash. This leaves a public hash
  specialization on a materially weaker, slower ad-hoc primitive.
- Path comparison, iteration, and hashing repeatedly rescan the pathname to
  rediscover components; iterator construction also snapshots the complete
  directory into a heap-allocated vector and refreshes every entry eagerly.
  Large paths and directories therefore incur avoidable superlinear scanning,
  allocation, metadata calls, and latency before the first result.

### Audit record: `<cstdio>` / `<stdio.h>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 16.4.6.10, 31.13.1; C 7.21

Reviewed the complete synopsis and compatibility namespace, standard streams,
file lifecycle and buffering, byte I/O, positioning, temporary files, error
state, formatted output, and formatted input against the C17 facilities adopted
by C++23.

Certification fails because:

1. Formatted floating output and input use locale-independent `to_chars` and
   `from_chars` with a literal `.` radix character. They neither emit nor
   recognize the active C locale's decimal-point character as required by the
   `fprintf` and `fscanf` families.
2. The `l`-qualified `%c` and `%s` conversions do not perform the required
   multibyte/wide-character conversions with an initial conversion state.
   Scanning merely casts each input byte to `wchar_t`; the corresponding output
   path likewise narrows wide code units instead of using the active multibyte
   encoding. Valid non-ASCII text therefore fails even in a UTF-8 locale.
3. Floating formatting and scanning are capped by fixed 512-byte local arrays.
   A valid large field precision is reported as failure or truncated to 511
   input characters rather than processing the requested conversion subject to
   the API's ordinary output-size and resource limits.
4. `fgetpos` stores an `mbstate_t`, but `fsetpos` ignores that saved state and
   restores only the byte offset. Positioning a multibyte-oriented stream does
   not restore the parse state represented by `fpos_t`.
5. In normal FTL namespace mode, `<ftl/cstdio>` defines `stdin`, `stdout`, and
   `stderr` only when `FTL_REPLACE_STL` is active. The header's own synopsis is
   therefore missing the three required standard-stream macros unless an
   unrelated vendor header happened to define them first.

Library-quality observation: floating conversions are routed through fixed
stack buffers and then copied to the destination sink. A growable/direct sink
would both remove the artificial precision ceiling and avoid the extra staging
copy.

### Audit record: `<cinttypes>` / `<inttypes.h>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 17.4.1, 31.13.2; C 7.8

The `imaxdiv_t`, constexpr arithmetic functions, four conversion functions,
PRI/SCN macro families, conditional type-width spellings, and compatibility
header exposure are complete for the supported data models. Certification
nevertheless fails because the header includes and exposes the already-audited
`<cstdint>` model whose greatest-width integer types and associated limits are
incorrect when the implementation provides wider extended integers. No
separate material library-quality defect was identified.

### Audit record: `<regex>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 16.4.6.15, 32

Reviewed constants and errors, `regex_traits`, all six grammars, parsing and
matching, `basic_regex`, sub-matches and results, formatting/replacement,
algorithm overloads, and both iterator types.

Certification fails because:

1. `basic_regex` defaults its move constructor and move assignment across the
   pattern, traits object, compiled-program vectors, and scalar `valid_` flag.
   Moving transfers the vectors but copies `valid_`, leaving the source marked
   valid with an empty compiled program. A subsequent match on this supposedly
   valid-but-unspecified moved-from object indexes the empty instruction array;
   the required valid moved-from state is not preserved.
2. The mandated unconditional `noexcept` move operations directly move an
   arbitrary user-supplied `traits_type`. A traits type can meet the regex-traits
   requirements while having a throwing move operation; that exception causes
   termination instead of the library providing the specified non-throwing
   `basic_regex` move.
3. `basic_regex::swap` is implemented as three whole-object moves. Its cost and
   exception behavior therefore include arbitrary `traits_type` move operations
   and cannot meet the specified constant-time complexity for all conforming
   traits specializations.

Library-quality observations:

- Every match/search first copies the entire bidirectional target range into a
  newly allocated contiguous string. Search then restarts the VM at each
  candidate offset, and result construction walks from the original iterator
  once per capture to reconstruct positions. This adds avoidable allocation and
  repeated traversal before returning iterator-based results.
- `sub_match` comparisons materialize complete temporary strings rather than
  comparing their iterator ranges directly.

### Audit record: `<atomic>` / `<stdatomic.h>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 17.3.2, 33.5

Reviewed memory orders and fences, generic/integral/floating/pointer atomics,
`atomic_ref`, flags, lock-free properties and aliases, value-representation
comparison, waiting/notification, free-function APIs, smart-pointer
specializations, and the C compatibility header.

Certification fails because the integral atomic specialization is selected
through the repository's incomplete `is_integral_v`. Implementation-provided
extended integer types such as GCC and Clang `__int128` therefore instantiate
only the generic atomic interface and omit the required integral `fetch_add`,
`fetch_sub`, bitwise fetch operations, and compound operators. The associated
constraints and free functions inherit the same missing participation.

Library-quality observations:

- Non-lock-free object operations and indirect waits share fixed arrays of only
  64 process-global cache-line buckets. Hash collisions serialize unrelated
  atomics; because a proxy bucket cannot identify which object a waiter belongs
  to, even `notify_one` must increment shared state and wake every waiter in the
  bucket.
- On Apple systems where the weak-imported address-wait functions are absent,
  the implementation still classifies 4- and 8-byte waits as directly
  supported. The platform wait then returns immediately and `atomic::wait`
  busy-spins until the value changes instead of falling back to a blocking proxy.

### Audit record: `<stop_token>`

**Status:** CERTIFIED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 33.3

The stop state/source/token ownership model, source counting, request
linearization, callback registration and immediate invocation, callback
execution ordering, cross-thread destruction wait, same-thread self-destruction
escape, comparisons, swaps, constraints, and feature-test surface match C++23.
No material library-quality defect was identified.

### Audit record: `<thread>` / `jthread`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 33.2.4, 33.4

Thread identity and ownership, construction/invocation, join/detach failure
handling, native handles, `jthread` stop-token dispatch and destruction, thread
ID formatting/hashing, and `this_thread` operations were reviewed.

Certification fails because `this_thread::sleep_for` first applies
`ceil<nanoseconds>` to the complete positive duration and then converts the
result to `uint64_t`. A sufficiently large but representable duration overflows
the signed `nanoseconds::rep`; the resulting non-positive value makes the
function return immediately instead of blocking for at least the requested
relative timeout. The inherited duration conversion/classification defects
also apply to implementation-provided extended representation types.

Library-quality observation: every newly launched thread requires a separate
heap allocation for its decayed invocation tuple, even where the native launch
backend could carry a small inline start record or consume caller-owned startup
storage synchronously.

### Audit record: mutexes, locks, and condition variables

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Headers:** `<mutex>`, `<shared_mutex>`, `<condition_variable>`
**Applicable clauses:** 16.4.2.5, 33.2.4--33.2.5, 33.6--33.7

Reviewed basic, recursive, timed, and shared mutex state machines; generic
`lock`/`try_lock`; guard, scoped, unique, and shared ownership wrappers;
`call_once`; ordinary, arbitrary-lock, timed, predicate, and stop-token
condition waits; notification; and thread-exit notification.

Certification fails because the required native-handle surface is absent.
`mutex`, `recursive_mutex`, `timed_mutex`, `recursive_timed_mutex`,
`shared_mutex`, and `shared_timed_mutex` provide neither their
implementation-defined `native_handle_type` nor `native_handle()`. The same two
required members are missing from `condition_variable`. Programs using the
standard interoperation interfaces therefore fail at the synopsis level.

Timed lock and wait overloads additionally inherit the recorded chrono
extended-representation classification failures. No separate material
library-quality defect was identified in the locking algorithms.

### Audit record: `<semaphore>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 33.8

The counter bounds, release/acquire synchronization, spurious `try_acquire`
permission, timeout/release race closure, aliases, and feature-test surface
match C++23 for supported duration representations. Certification fails because
the timed overloads inherit chrono's rejection/misclassification of
implementation-provided extended representation types. No separate material
library-quality defect was identified.

### Audit record: `<latch>`

**Status:** CERTIFIED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 33.9

Maximum count, construction, countdown synchronization, waiting,
`arrive_and_wait`, deleted ownership operations, and the feature-test surface
match C++23. No material library-quality defect was identified.

### Audit record: `<barrier>`

**Status:** CERTIFIED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 33.10

Arrival tokens, phase accounting, completion invocation and ordering, waiting,
drop behavior, constraints, maximum count, and the C++23 feature-test value
match the relaxed phase-completion specification.

Library-quality observation: all arrivals serialize on one internal mutex, and
the last arrival executes the arbitrary completion function while retaining
that mutex. This makes a highly contended barrier a centralized scalability
bottleneck and lengthens every blocked arrival by the completion duration.

### Audit record: `<future>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 33.2.4, 33.10

Reviewed error codes/exceptions, shared-state readiness and abandonment,
futures and shared futures, promises and thread-exit publication,
`packaged_task`, allocator use, deferred and asynchronous launch, result and
exception transfer, timed waiting, and last-reference worker synchronization.

Certification fails because:

1. The `packaged_task` converting constructor checks
   `is_invocable_r_v<R, F&, ArgTypes...>` instead of the stored callable type
   `decay_t<F>&`. In particular, a const source object whose non-const callable
   copy is invocable is rejected even though decay-copying it produces the exact
   callable object on which the task is specified to invoke.
2. `future::wait_for`, `shared_future::wait_for`, and corresponding absolute
   waits inherit chrono's rejection/misclassification of implementation-provided
   extended duration representation types.

Library-quality observation: each `packaged_task` allocates its shared state and
its virtual callable model separately, with no small-callable storage. Even a
function pointer therefore requires two heap allocations and an indirect
virtual call per invocation.

### Audit record: `<stdexcept>`

**Status:** CERTIFIED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 17.7, 19.2

The complete logic/runtime exception hierarchy, string and C-string
constructors, virtual destruction, non-throwing copy/assignment, stable shared
message ownership, and `what()` behavior match C++23. No material
library-quality defect was identified.

### Audit record: `<system_error>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 19.5

Reviewed categories, codes and conditions, enum customization, comparisons,
messages, `system_error`, standard `errc`, and hash support.

Certification fails on Windows because `system_category` treats native Win32
error numbers as though they were C `errno` values. `message()` passes them to
`strerror`, and `default_error_condition()` maps a value to `generic_category`
merely when the same integer occurs anywhere in `errc`. Win32 values require
platform message lookup and an explicit semantic mapping; numeric coincidence
can currently produce both incorrect text and false equivalence (for example,
Win32 error 5 is access denied, not generic `errc` value 5).

No separate material library-quality defect was identified. The code and
condition hash specializations correctly combine category identity and value
with RapidHash.

### Audit record: `<generator>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 26.8

The template mandates, reference/value relationships, view and input-iterator
surface, coroutine lifetime, active recursive-yield stack, exception transfer,
allocator-aware frame allocation, promise operations, range flattening, and
PMR alias were reviewed in both namespace modes.

Certification fails because the general
`yield_value(ranges::elements_of<R, Alloc>)` overload has no definition in
normal FTL namespace mode. Its body is compiled only under
`FTL_REPLACE_STL`; normal mode leaves a declaration ending in a semicolon.
Consequently, a valid `co_yield ranges::elements_of(range)` selects the
required overload but cannot instantiate its deduced return type, making the
ordinary public-header mode unusable for non-generator recursive ranges.

No separate material library-quality defect was identified.

### Audit record: `<stacktrace>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 19.6

The entry value model and queries, capture overloads, allocator-aware storage,
const iteration, comparison, swapping, string and stream conversion,
formatting, PMR aliases, and RapidHash-based hash support were reviewed.

Certification fails because every capture is unconditionally limited to 128
frames. The no-argument `current()` is specified in terms of the available
stacktrace of the complete current evaluation, and `current(skip)` is specified
as a suffix of that trace; neither overload permits an arbitrary 128-frame
truncation. The bounded overload also mishandles skipping on POSIX: it asks
`backtrace` for only `min(max_depth, 128)` entries and then removes `skip + 1`
from that already-short result, rather than capturing enough entries to return
up to `max_depth` frames after the skipped prefix. Any positive skip therefore
reduces the requested result length, and a skip at least as large as the fixed
capture count always produces an empty trace even when deeper frames exist.

Library-quality observation: entries never attempt symbol or debug-information
lookup. `description()` and `source_file()` are always empty,
`source_line()` is always zero, and user-facing conversion prints only a raw
address. Although unavailable information may be represented this way, the
facility discards the principal diagnostic value expected from a stacktrace on
platforms where symbolization is available.

### Audit record: `<strstream>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, D.15

The deprecated buffer constructors and ownership states, freezing, dynamic
growth, putback, seeking, legacy input/output/combined stream wrappers, and
their buffer accessors were reviewed.

Certification fails because:

1. `strstreambuf` omits the required `underflow()` override. Ordinary writes
   that fit the current put area advance `pptr()` without updating the stored
   get-area high-water mark, and the inherited base implementation reports EOF
   instead of extending `egptr()` up to the current write position. Characters
   successfully written to a dynamic or combined stream can therefore remain
   unreadable through its input sequence.
2. The constructor taking `gnext_arg`, `n`, and non-null `pbeg_arg` must call
   `setg(gnext_arg, gnext_arg, pbeg_arg)` and
   `setp(pbeg_arg, pbeg_arg + N)`. The implementation instead establishes the
   put area as `[gnext_arg, gnext_arg + N)` and bumps its current pointer to
   `pbeg_arg`. This gives the wrong `pbase()`, `epptr()`, `pcount()`, and output
   extent whenever the two pointers differ.
3. In the absence of user allocation callbacks, dynamic array growth is
   specified in terms of `new char[n]` and `delete[]`. The implementation uses
   `malloc` and `free`, bypassing replaceable allocation functions, the new
   handler, and the required C++ array-allocation behavior.

Library-quality observation: dynamic growth uses unchecked doubling and copies
the complete old capacity rather than only the initialized/high-water portion.
Large buffers can wrap the capacity calculation, while normal growth needlessly
copies unused storage.

### Audit record: `<ctime>` / `<time.h>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 17.2.2, 17.14; ISO/IEC 9899:2018 7.27

The C time types/macros, process and calendar clocks, UTC acquisition, broken
down time conversion, text conversion, formatting, namespace placement, and C
compatibility exports were reviewed.

Certification fails because:

1. `difftime` evaluates `end - beginning` in `time_t` before conversion to
   `double`. Opposite-extreme signed values overflow even though `difftime`
   exists to express the elapsed seconds in its floating result without
   requiring a representable `time_t` subtraction.
2. The handwritten `gmtime` arithmetic can overflow for extreme negative
   `time_t` values when the floored day count is multiplied back by 86400. It
   also narrows an unrestricted civil year into `tm_year` without detecting
   that the result is not representable as `int`; the C conversion is required
   to return a null pointer when the result cannot be represented.
3. `timespec_get` uses signed remainder directly for `tv_nsec`. If the system
   time is before the epoch, the remainder is negative rather than normalized
   into the required nanosecond subsecond range with a correspondingly adjusted
   `tv_sec`.
4. The private public support header `<__c_time_types.hpp>` defines `tm` with
   only the nine standard fields, then passes that object to the native POSIX
   `mktime` and `strftime` ABIs through erased `void*` declarations. The
   supported glibc and Darwin `struct tm` layouts append native timezone fields.
   The runtime can read or write beyond the FTL object (notably for `%z`/`%Z`
   and normalization), causing both incorrect time-zone behavior and an
   out-of-bounds ABI violation.

No separate material library-quality defect was identified.

### Audit record: assertions and C compatibility utility headers

**Status:** MIXED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Headers:** `<cassert>`, `<assert.h>`, `<cerrno>`, `<errno.h>`, `<csetjmp>`,
`<setjmp.h>`, `<csignal>`, `<signal.h>`, `<iso646.h>`, `<stdalign.h>`,
`<stdbool.h>`
**Applicable clauses:** 16.4.2.5, 17.2.2, 17.3.2, 17.12--17.13, C.6,
D.13; ISO/IEC 9899:2018 7.2, 7.5, 7.13, 7.14

The assertion macro's repeated-inclusion/NDEBUG behavior, error macros and
thread-local lvalue, jump-buffer surface, signal types/functions/macros,
alternative-token compatibility header, and deprecated C compatibility macros
were reviewed for each supported platform.

`<cassert>` / `<assert.h>`, `<csetjmp>` / `<setjmp.h>`, `<iso646.h>`,
`<stdalign.h>`, and `<stdbool.h>` are certified. The assertion header correctly
keeps its macro definition outside the include guard; the jump buffer storage
is sufficiently sized and aligned for the supported x86-64 Windows and Linux
ABIs; the empty alternative-token header and deprecated compatibility macros
match their C++23 specifications.

`<cerrno>` / `<errno.h>` fail on macOS because `EILSEQ` is hard-coded to the
Linux value 84 for every non-Windows platform; Darwin's C runtime uses 92.
Code comparing delegated runtime failures with the macro therefore observes a
different condition.

`<csignal>` / `<signal.h>` fail on Windows because `SIGABRT` is hard-coded to
the POSIX value 6. The supported Microsoft CRT defines `SIGABRT` as 22, so the
header passes the wrong signal number to its delegated `signal` and `raise`
functions.

No separate material library-quality defect was identified.

### Audit record: `<version>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 10.1, 15.5, 16.4.2.5, 17.3.2

The complete macro name/value inventory was mechanically compared with the
C++23 `<version>` synopsis and with feature-test definitions emitted by the
other public headers. The intentionally absent `__cpp_lib_modules` macro is
consistent with `ROADMAP.md`'s explicit exclusion of named standard-library
modules from FTL's current product boundary; all other present synopsis values
match exactly.

Certification fails because the library violates the synopsis's cross-header
consistency rule for `__cpp_lib_ranges`: `<version>` correctly reports
`202302L`, while including `<ranges>` alone reports only `202110L`. The latter
value advertises an older revision even though `<ranges>` is explicitly one of
the corresponding headers for the C++23 value.

The additional implementation-reserved
`__cpp_lib_uses_allocator_construction` macro is not treated as a defect. No
separate material library-quality defect was identified.

### Audit record: `<memory_resource>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 20.4

The resource base protocol, default/new-delete/null resources,
`polymorphic_allocator`, equality, allocation overflow handling, and
uses-allocator construction were reviewed together with both pool resources
and the monotonic resource.

Certification fails because:

1. The pool resources merely retain and linearly reuse individual upstream
   allocations. Both members of `pool_options` are ignored: no effective
   defaults replace zero in `options()`, `max_blocks_per_chunk` never controls
   replenishment, and allocations above `largest_required_pool_block` are not
   passed through as non-pooled allocations. This does not implement the
   option-controlled pooling behavior required by 20.4.5.
2. When `monotonic_buffer_resource` exhausts its current buffer, it allocates
   exactly the requested block and returns it without making that allocation
   the new current buffer. It has no `next_buffer_size` state or growth factor,
   so every subsequent request that does not fit the original buffer performs
   another upstream allocation, contrary to 20.4.6. Constructors taking an
   `initial_size` also allocate immediately even though the specified initial
   state has a null current buffer and uses that value for the first lazily
   acquired buffer.

Library-quality observations:

- Each new pooled block causes two upstream allocations, one for the block and
  another for a linked-list metadata node, and allocation/deallocation scan the
  complete list. The nominal pool therefore lacks both chunk amortization and
  size-class lookup.
- `synchronized_pool_resource` protects every operation with an unbounded tight
  `atomic<bool>` exchange loop. A preempted holder makes all contenders consume
  CPU continuously, with no blocking wait, backoff, yield, or fairness.

### Audit record: `<scoped_allocator>`

**Status:** FAILED
**Audited at:** `798e52c498f5630ecdc03c92456b99a1ff95aa97`
**Applicable clauses:** 16.4.2.5, 20.5

The recursive allocator representation, rebinding, allocation forwarding,
uses-allocator construction, copy selection, comparisons, propagation traits,
and constructor surface were reviewed.

Certification fails because the copy-assignment, move-assignment, and swap
propagation member types combine the outer and inner allocator traits with
logical AND. Each is required to be `true_type` when the corresponding trait is
true for *any* allocator in the outer/inner set, so these must use logical OR.
Containers using a mixed propagation stack consequently observe the wrong
allocator propagation contract. The converting constructors also omit their
specified `is_constructible_v` constraints, allowing overload participation to
depend on hard instantiation failure instead of the required constrained
interface.

No separate material library-quality defect was identified.

## Audit closure evidence

The audit inventory and ledger were closed against the public tree at
`798e52c498f5630ecdc03c92456b99a1ff95aa97` as follows:

- All 137 files directly under `include/ftl` have an exact `<header>` or
  explicitly named support-unit entry in the Header ledger. Grouped rows are
  used only for C compatibility wrappers or facilities whose requirements are
  shared across multiple headers.
- All 185 feature-test macro names/values in the C++23 `<version>` synopsis were
  mechanically compared with `<version>`. Excluding the deliberately
  out-of-scope named-modules advertisement, every supplied value matches. All
  270 synopsis mappings from a macro to another public FTL header were then
  checked directly; the sole mismatch is the recorded `__cpp_lib_ranges`
  value in `<ranges>`.
- Focused normal and `FTL_REPLACE_STL` tests covering the final substantive
  audit set (`<csetjmp>`, `<ctime>`, `<version>`, `<scoped_allocator>`,
  `<memory_resource>`, `<generator>`, `<stacktrace>`, and `<strstream>`) passed
  in the canonical MSVC, Clang-CL, WSL GCC, and WSL Clang build folders. These
  tests corroborate the supported baseline behavior; they do not negate
  standard requirements outside their assertions, which remain recorded as
  failures above.
- `git diff --check` is clean. The audit changed documentation only, so no full
  build/test closure was required after the focused evidence runs.

## Header ledger

| Header / unit                                                 | Status        | Audited at                                 | ISO clauses                                                             | Notes                                                                                                                                                                                                                                                                                                                                      |
|---------------------------------------------------------------|---------------|--------------------------------------------|-------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `<cstddef>` / `<stddef.h>`                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.3, 16.4.3.2, 16.4.6.2, 17.2.1, 17.2.3–17.2.5, 17.3.2             | `byte` integral constraints are too narrow on implementations where extended integer types such as `__int128` satisfy `is_integral_v`; the C wrapper faithfully re-exports the affected type surface.                                                                                                                                      |
| `<version>`                                                   | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 10.1, 16.4.2.5, 17.3.2                                                  | `<version>` reports `__cpp_lib_ranges == 202302L` while the corresponding `<ranges>` header alone reports the obsolete `202110L` value. Named library modules and `__cpp_lib_modules` are explicitly outside FTL's declared product boundary.                                                                                              |
| `<initializer_list>`                                          | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 9.4.5, 16.4.6.2, 16.4.6.5, 17.10                                        | Explicit and partial specializations of `initializer_list` are not prohibited as required by 17.10.2.                                                                                                                                                                                                                                      |
| `<cstdint>` / `<stdint.h>`                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 6.8.2, 16.4.6.2, 17.4.1; ISO/IEC 9899:2018 7.20                         | Limit-macro expression-type defects; greatest-width integer handling stops at 64 bits; pointer limits and integer-constant suffixes assume a 64-bit data model.                                                                                                                                                                            |
| `<limits>`                                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 6.8.2–6.8.3, 17.3.3–17.3.5                                              | Missing extended-integer specializations; incorrect integral `traps`; integer `digits` assumes 8-bit padding-free representations; floating implementation properties are hard-coded.                                                                                                                                                      |
| `<climits>` / `<limits.h>`                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 6.8.2, 7.3.7, 17.3.6; ISO/IEC 9899:2018 5.2.4.2.1                       | GCC/Clang `UCHAR_MAX`/`USHRT_MAX` have incorrect promoted types; `MB_LEN_MAX` is hard-coded below the maximum supported by the delegated glibc multibyte runtime.                                                                                                                                                                          |
| `<cfloat>` / `<float.h>`                                      | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 17.3.7; ISO/IEC 9899:2018 5.2.4.2.2                                     | Complete C++23 floating-point implementation-property macro surface and its C compatibility wrapper match the supported compiler implementations.                                                                                                                                                                                          |
| `<stdfloat>`                                                  | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 5.13.4, 6.8.3, 15.11, 17.4.2                                            | Conditional extended floating-point aliases exactly follow the implementation-provided `__STDCPP_*` facilities and corresponding standard literal types.                                                                                                                                                                                   |
| `<source_location>`                                           | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.8                                                          | Call-site capture semantics, observers, special-member requirements, feature-test macro, and freestanding surface match C++23.                                                                                                                                                                                                             |
| `<coroutine>`                                                 | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 9.5.4, 16.4.2.5, 17.12                                                  | Coroutine traits, handles, builtin frame/promise interaction, comparison/hash support, no-op coroutine, trivial awaitables, feature-test macro, and freestanding surface match C++23.                                                                                                                                                      |
| `<cstdarg>` / `<stdarg.h>`                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 7.6.1.3, 16.4.2.5, 17.13.2; ISO/IEC 9899:2018 7.16                      | Handwritten MSVC `va_arg` does not implement indirect aggregate passing and assumes an x64-style 8-byte argument layout on every MSVC architecture; the C wrapper inherits it.                                                                                                                                                             |
| `<cassert>` / `<assert.h>`                                    | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.12; C 7.2                                                  | Repeated inclusion redefines `assert` from the current `NDEBUG` state; enabled evaluation and platform diagnostic termination paths match the required macro behavior.                                                                                                                                                                     |
| `<cerrno>` / `<errno.h>`                                      | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.13.3; C 7.5                                                | The non-Windows branch hard-codes Linux `EILSEQ == 84`; Darwin uses 92, so macOS runtime failures do not match the public macro.                                                                                                                                                                                                           |
| `<csetjmp>` / `<setjmp.h>`                                    | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.13.4; C 7.13                                               | Macro/function placement, jump-buffer storage, and non-local return surface match the supported x86-64 platform ABIs.                                                                                                                                                                                                                      |
| `<csignal>` / `<signal.h>`                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.13.5; C 7.14                                               | `SIGABRT` is fixed at POSIX value 6 on Windows, but the delegated Microsoft CRT uses 22.                                                                                                                                                                                                                                                   |
| `<iso646.h>`                                                  | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | C.6                                                                     | Correctly empty in C++, where the alternative tokens are keywords and no C macros are supplied.                                                                                                                                                                                                                                            |
| `<stdalign.h>` / `<stdbool.h>`                                | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | D.13; C 7.15, 7.18                                                      | The deprecated “is defined” compatibility macros are present with value 1, without redefining C++ keywords.                                                                                                                                                                                                                                |
| `<new>`                                                       | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 6.7.3, 6.7.5.5, 16.4.2.5, 17.6                                          | `launder` is not a compiler lifetime/optimization barrier on MSVC or clang-cl; hardware interference sizes assume a universal 64-byte lower bound without constraining target architectures.                                                                                                                                               |
| `<typeinfo>`                                                  | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 7.6.1.7–7.6.1.8, 16.4.2.5, 17.7                                         | MSVC normal mode defines independent `ftl::bad_cast` and `ftl::bad_typeid` classes rather than aliases to the runtime RTTI exception types, so they do not catch exceptions produced by failed `dynamic_cast` and null-polymorphic `typeid`.                                                                                               |
| `<exception>`                                                 | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 14.6, 16.4.2.5, 17.9                                                    | `rethrow_if_nested` forms a `dynamic_cast` for every polymorphic type and therefore fails to provide the required no-op behavior when `nested_exception` is an inaccessible or ambiguous base.                                                                                                                                             |
| `<compare>`                                                   | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 7.6.8, 11.10.3, 16.3.3.3.5, 17.11, 22.10.8.8                            | Normal-mode `compare_three_way` incorrectly synthesizes legacy comparisons; weak/partial-order CPO dispatch and floating semantics are incorrect; fallback CPO constraints are too strong; pointer total-order semantics are missing.                                                                                                      |
| `<concepts>`                                                  | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.3.2, 18.1–18.7                                             | `same_as` lacks the required symmetric subsumption because its two `is_same_v` operands are distinct atomic constraints; arithmetic concepts also inherit `<type_traits>`'s missing extended-integer recognition.                                                                                                                          |
| `<type_traits>`                                               | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 6.8, 16.4.2.5, 16.4.4.3, 17.13.5, 21.3, 22.10.4                         | Extended integers are not recognized; copy/move convenience traits are ill-formed for non-referenceable types; `is_swappable_with` omits array swap from its lookup context.                                                                                                                                                               |
| `<ratio>`                                                     | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 21.4                                                          | Arithmetic facilities have the wrong trait-class-plus-`_t` interface; comparison trait classes are missing; invalid most-negative arguments are accepted; conditional SI aliases never appear.                                                                                                                                             |
| `<utility>`                                                   | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 16.4.4.3, 17.13.5, 21.2, 22.2â€“22.3                          | Integer comparisons accept forbidden character and boolean types and miss extended integers; array `swap` is unconstrained; pair ordering does not use `synth-three-way`; dangling prevention is conditional; `subrange` is accepted.                                                                                                      |
| `<tuple>`                                                     | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 20.2.8.1, 22.4                                                | `subrange` is incorrectly accepted by constructors/assignment; tuple is not uses-allocator-aware; type-based rvalue `get` fails for reference elements; tuple algorithms are over-broad; dangling checks are conditional.                                                                                                                  |
| `<optional>`                                                  | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.11, 22.5                                                   | Legacy relational overloads are missing; `or_else` is unconstrained; `transform` rejects immovable results; three-way comparison participation is too broad; volatile hash enablement is wrong.                                                                                                                                            |
| `<variant>`                                                   | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.11, 22.6                                                   | Copy assignment and swap have wrong exception-state behavior; converting assignment and `emplace` participation are wrong; `visit` coerces heterogeneous results and rejects derived variants; comparison inherits ordering defects.                                                                                                       |
| `<any>`                                                       | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.7, 22.7                                                    | `emplace` preserves the old value on failure instead of becoming empty; the value constructor accepts in-place tags; `type()` is conditionally absent. Every value is heap allocated rather than using small-object storage.                                                                                                               |
| `<expected>`                                                  | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 22.8                                                          | `emplace` has wrong `noexcept`; swap participation and rollback are incorrect; monadic result relationships are incomplete; mapped immovable results are rejected; state-changing assignment guarantees drift.                                                                                                                             |
| `<bitset>`                                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 22.9                                                          | The proxy's required private default constructor is absent; the pointer constructor is overconstrained; formatted extraction bypasses required stream error-state handling when internal operations throw.                                                                                                                                 |
| `<functional>`                                                | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.11, 22.10                                                  | `bind<R>` ignores `R`; classic bind mishandles wrapper value categories; `function` constraints, no-allocation cases, RTTI surface, and static-callable deduction drift; hashes enable cv keys and hash raw floats. Quality: no SBO and fake quadratic Boyer-Moore searchers.                                                              |
| `<typeindex>`                                                 | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 22.11                                                         | The four explicitly required legacy relational member functions are absent; expression rewriting through `<=>` does not supply their named member interfaces.                                                                                                                                                                              |
| `<execution>`                                                 | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 22.12                                                         | Complete policy-type, trait, global-object, feature-test, and namespace surface matches C++23; policy-taking algorithm semantics remain scoped to their owning algorithm headers.                                                                                                                                                          |
| `<charconv>`                                                  | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 22.13                                                         | Integer conversions omit exact overloads for implementation-provided extended signed and unsigned integer types; the ordinary integer and floating conversion engines otherwise match the audited C++23 surface.                                                                                                                           |
| `<format>`                                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 22.14                                                         | Extended arithmetic formatter coverage inherits incomplete type classification. Quality: output-iterator and size operations materialize a complete intermediate string instead of formatting/counting directly.                                                                                                                           |
| `<bit>`                                                       | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 22.15                                                         | Extended signed/unsigned integer participation is missing. Quality: runtime population/count-zero/byte-swap primitives use scalar loops instead of available compiler intrinsics.                                                                                                                                                          |
| `<string_view>` / character traits                            | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 23.2--23.3                                                    | Complete traits and string-view surface matches C++23. Quality: non-byte view hashes pass character count rather than representation byte count to RapidHash, creating avoidable collision clusters.                                                                                                                                       |
| `<string>`                                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 23.4, D.25                                                    | `resize_and_overwrite` callback/result semantics drift; deprecated `reserve()` is absent; copy allocator propagation and throwing-traits guarantees are wrong; floating conversions ignore the C locale. Quality: wide hashes use character count as byte count.                                                                           |
| `<cctype>` / `<ctype.h>`                                      | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 17.14.2, 23.5.1; ISO/IEC 9899:2018 7.4                                  | Complete C-linked classification and case-conversion surface delegates semantics, input-domain handling, and active-locale behavior to the supported platform C runtime.                                                                                                                                                                   |
| `<cwctype>` / `<wctype.h>`                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 17.14.4, 23.5.2; ISO/IEC 9899:2018 7.30.2                               | Wide classification, descriptors, and transforms are hard-coded to ASCII instead of following the active C locale; `WEOF` is an `unsigned int` expression rather than a `wint_t` constant.                                                                                                                                                 |
| `<cstring>` / `<string.h>`                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 17.14.4, 23.5.3; ISO/IEC 9899:2018 7.24                                 | `strcoll` and `strxfrm` ignore the active C collation locale. Quality: builtin suppression forces byte loops for memory primitives, and `strstr` is a naive quadratic search.                                                                                                                                                              |
| `<cwchar>` / `<wchar.h>`                                      | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 17.14.4, 23.5.4--23.5.6; ISO/IEC 9899:2018 7.29                         | Multibyte conversion and wide I/O are ASCII-only; numeric parsing, collation, and time formatting ignore active locales; `WEOF` has the wrong expression type. Quality: scalar primitives and naive search.                                                                                                                                |
| `<cuchar>` / `<uchar.h>`                                      | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 17.14.5, 23.5.5--23.5.6; ISO/IEC 9899:2018 7.28                         | Conversion is ASCII-only and ignores the active multibyte locale; `mbrtoc8`/`mbrtoc16` lack staged `size_t(-3)` output; reverse conversion rejects completed valid non-ASCII scalars.                                                                                                                                                      |
| `<array>`                                                     | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.2, 24.3.2, 24.3.7                                          | Comparison accepts an arbitrary syntactically valid spaceship result instead of applying `synth-three-way`, so permitted element types with an unrelated `<=>` result make array comparison ill-formed.                                                                                                                                    |
| `<vector>`                                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.2, 24.3.6, 24.3.11--24.3.12                                | Growth commits before appended construction, breaking exception guarantees and self-aliasing insertion; the bool-proxy formatter is missing. Quality: packed hashing passes blocks as bytes.                                                                                                                                               |
| `<deque>`                                                     | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.2, 24.3.3, 24.3.8                                          | Throwing construction can leak installed blocks/elements; failure while growing the block map leaks a newly allocated block; middle modifiers move from the source before success and break required rollback.                                                                                                                             |
| `<forward_list>`                                              | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.2, 24.3.4, 24.3.9                                          | Throwing construction leaks linked nodes; multi-element insertions partially commit instead of rolling back; a throwing sort comparison can disconnect and lose nodes rather than preserving every element.                                                                                                                                |
| `<list>`                                                      | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.2, 24.3.5, 24.3.10                                         | Throwing construction leaks linked nodes; multi-element insertions partially commit instead of rolling back; a throwing sort comparison destroys the detached half instead of preserving every element.                                                                                                                                    |
| `<set>`                                                       | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.2, 24.4.3, 24.4.6--24.4.7                                  | Comparator exceptions leak nodes; merge variants and allocator-only construction are incomplete; inherited assignment returns the base; hints and legacy comparison fallback fail complexity/interface requirements; node handles overconstrain allocators.                                                                                |
| `<map>`                                                       | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.2, 24.4.2, 24.4.4--24.4.5                                  | Shared tree/node leaks; merge and allocator-only overload sets are incomplete; inherited assignment and `value_compare` have wrong interfaces; hints and fallback ordering fail; node handles impose invalid allocator assumptions.                                                                                                        |
| `<unordered_set>`                                             | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.2.8, 24.5.3, 24.5.6--24.5.7                                | Hash/equality/growth exceptions leak nodes; node insertion and merge lose ownership; overloads and node allocator semantics drift; equality uses key-equivalence counts instead of element equality. Quality: unmixed power-of-two bucketing.                                                                                              |
| `<unordered_map>`                                             | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.2.8, 24.5.2, 24.5.4--24.5.5                                | Throwing hash-table operations leak nodes; merge/constructor/assignment interfaces are incomplete; multimap equality does not compare group multiplicities; node allocator semantics drift. Quality: unmixed power-of-two bucketing.                                                                                                       |
| `<queue>`                                                     | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.6.2, 24.6.6--24.6.7, 24.6.13                               | Queue and priority-queue formatters are missing; allocator deduction guides are underconstrained; default underlying containers carry the recorded deque/vector exception and ownership defects.                                                                                                                                           |
| `<stack>`                                                     | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.6.3, 24.6.8, 24.6.13                                       | The required adaptor formatter is absent; allocator deduction guides accept non-allocators; the default deque-backed specialization inherits deque's exception-safety/resource failures.                                                                                                                                                   |
| `<flat_set>`                                                  | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.6.5, 24.6.11--24.6.12                                      | Range and sorted insertion are quadratic instead of meeting `N + M log M`/linear bounds; inherited assignment and fallback ordering drift; allocator guide detection/inventory is incomplete.                                                                                                                                              |
| `<flat_map>`                                                  | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.6.4, 24.6.9--24.6.10                                       | Range and sorted insertion violate required complexity; inherited assignment and synthesized ordering drift; allocator guides are incomplete/underconstrained; default storage inherits vector failure behavior.                                                                                                                           |
| `<span>`                                                      | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.7.2                                                        | Complete fixed/dynamic span construction, deduction, subviews, observation, iteration, borrowed-view enablement, and object-representation byte views match C++23 and the freestanding surface.                                                                                                                                            |
| `<mdspan>`                                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 24.7.3                                                        | Extents' index-type mandate inherits incomplete integral classification, rejecting implementation-provided extended signed and unsigned integer types throughout the mdspan facility.                                                                                                                                                      |
| `<iterator>` / range access                                   | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 25.2--25.7                                                    | Concept expression checks, contiguous addressing, proxy `iter_swap`, legacy primitive dispatch, common-iterator state/proxies/safety, adaptor constraints/categories, comparison-mode parity, and extended integer range sizing are defective.                                                                                             |
| `<ranges>`                                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 26.2--26.7                                                    | View concepts and operations inherit iterator CPO/category defects; integer-like range sizing rejects extended integers; `views::counted` can select the wrong result; proxy swapping and comparison participation differ by mode.                                                                                                         |
| `<generator>`                                                 | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 26.8                                                          | The general `yield_value(elements_of<R, Alloc>)` definition is compiled only in replacement mode; normal namespace mode leaves it declaration-only and cannot flatten valid non-generator input ranges.                                                                                                                                    |
| `<algorithm>`                                                 | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 27.2--27.8                                                    | Missing `ranges::minmax_result`; proxy swapping, indirect-readability constraints, legacy category dispatch, and default three-way comparison inherit shared defects. Quality: stable algorithms never use adaptive auxiliary storage.                                                                                                     |
| `<numeric>`                                                   | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 27.9--27.10                                                   | `gcd`/`lcm` reject extended integers; pointer `midpoint` relies on potentially unrepresentable pointer subtraction; ranges iota and generalized operations inherit iterator classification defects.                                                                                                                                        |
| `<memory>`                                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 20.2--20.3, 20.6--20.7, 20.9--20.11, 27.11                    | Lifetime-start functions are casts only; smart-pointer conversion/swap/exception ownership rules drift; array `allocate_shared` ignores allocators; owner ordering is not total; nontrivial constexpr allocator use fails. The complete C++23 ranges raw-memory surface is also absent and classic algorithms have exception/type defects. |
| `<memory_resource>`                                           | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 20.4                                                          | Pool options never affect behavior or report effective defaults; the pool is individual-allocation retention rather than chunk replenishment; monotonic growth never adopts or grows newly acquired buffers. Quality: two upstream allocations and linear scans per fresh pooled block; tight spinlock synchronization.                    |
| `<scoped_allocator>`                                          | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 20.5                                                          | Recursive propagation traits use AND instead of the required “true for any allocator” OR, and converting constructors omit their specified `is_constructible_v` constraints.                                                                                                                                                               |
| `<cstdlib>` / `<stdlib.h>`                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.2.2, 17.5, 20.2.12, 23.5.6, 27.12, 28.5.10, 28.7.2; C 7.22 | C++-linkage algorithm/handler overloads and floating `abs` are absent; termination registration lacks required `noexcept`, as do several Apple termination imports.                                                                                                                                                                        |
| `<cfenv>` / `<fenv.h>`                                        | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 28.3; ISO/IEC 9899:2018 7.6                                   | `feraiseexcept` only writes sticky flags and does not trigger enabled floating-point traps; `feupdateenv` inherits this failure when re-raising saved exceptions.                                                                                                                                                                          |
| `<complex>` / `<complex.h>`                                   | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 28.2, 28.4                                                    | Converting assignment is missing; arithmetic convenience overloads reject extended arithmetic types; stream insertion does not reproduce `copyfmt` state. Quality: multiplication has avoidable overflow/NaN behavior.                                                                                                                     |
| `<random>`                                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 28.2, 28.5                                                    | Extended integer/floating template arguments are rejected; `generate_canonical` can round to `1`; seed-sequence participation is too weak. Quality: bitwise modular arithmetic, linear discard, linear discrete sampling, and discarded caches.                                                                                            |
| `<valarray>`                                                  | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 28.2, 28.6                                                    | Throwing construction leaks storage; empty `end` performs null-pointer arithmetic; shifts narrow sizes to `long long`; value construction incorrectly requires default construction. Quality: eager expression and selector allocation.                                                                                                    |
| `<numbers>`                                                   | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 28.8                                                          | The complete constant inventory and supported-format values match, but variable templates reject implementation-provided extended floating-point types through incomplete type classification.                                                                                                                                             |
| `<cmath>` / `<math.h>` / `<tgmath.h>` core                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 28.2, 28.7.1--28.7.5; C 7.12                                  | Extended overloads/ranks and evaluation typedefs are wrong; constexpr remainder reduction overflows; Clang `fma` is unfused; quiet comparisons can raise invalid; sign inspection assumes fixed layouts.                                                                                                                                   |
| `<cmath>` special functions                                   | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 28.7.6                                                        | Extended arithmetic overloads are missing; beta can turn finite or underflowing results into NaN through overflowing log-gamma arithmetic. Quality: unscaled forward recurrence harms large-order stability.                                                                                                                               |
| `<chrono>` duration/time/clock core                           | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 29.2--29.7                                                    | Duration comparison maps NaNs to equivalent and loses custom ordering categories; extended floating reps are misclassified; `is_clock` checks only name existence; ratio defects propagate into periods and conversions.                                                                                                                   |
| `<chrono>` calendar / `hh_mm_ss`                              | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 29.8--29.10                                                   | Calendar arithmetic can overflow before modular reduction; year arithmetic narrows too late; extended floating durations are misclassified and take the truncating integral subseconds path.                                                                                                                                               |
| `<chrono>` UTC/TAI/GPS/file clocks and conversion             | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 29.7.3--29.7.6, 29.7.10, 29.11.8                              | `clock_cast` is declared without its specified availability constraint, so detection contexts accept impossible conversions that later hard-error through body `static_assert`s.                                                                                                                                                           |
| `<chrono>` time-zone database / `zoned_time`                  | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 29.11.1--29.11.7, 29.11.9                                     | Name-taking `zoned_time` constructors test only that `zoned_traits::locate_zone` exists, not the standard's required constructibility from its result and the remaining arguments.                                                                                                                                                         |
| `<chrono>` formatting                                         | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 22.14, 29.12                                                  | Rejects valid chrono specs beginning with a literal; localized `%p`/`%r` and `E`/`O` alternatives remain C-locale output; extended floating duration precision is rejected. Quality: all output is staged through an allocating string.                                                                                                    |
| `<chrono>` parsing                                            | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 29.13                                                         | Locale-dependent `%c`, `%x`, `%X`, and `%r` use fixed C-locale layouts, and most `E`/`O` alternative numeric representations are ignored.                                                                                                                                                                                                  |
| `<locale>`                                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 16.4.6.10, 30.1--30.4, D.27                                   | Global-locale access races; time parsing is case-sensitive and named date/time layouts remain C-locale; monetary grouping is not validated; fixed conversion buffers truncate long values; `wstring_convert`/`wbuffer_convert` are absent. Quality: facet lookup is linear; collation hashing correctly uses RapidHash.                    |
| `<codecvt>`                                                   | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, D.26                                                          | Deprecated Unicode facet inventory, UCS/UTF-16 distinctions, modes, limits, state, headers, byte order, result positioning, and observers match C++23.                                                                                                                                                                                     |
| `<clocale>` / `<locale.h>`                                    | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.14.2, 30.5; C 7.11                                         | Complete category macros, platform-layout `lconv`, and C-linked `setlocale`/`localeconv` surface match the supported platform ABIs.                                                                                                                                                                                                        |
| `<iosfwd>` / `<ios>`                                          | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 16.4.6.10, 31.2--31.5                                         | `iword`/`pword` allocation failures return shared process-wide fallback references, allowing distinct stream objects and threads to overwrite/race each other's storage.                                                                                                                                                                   |
| `<streambuf>`                                                 | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 31.6                                                          | Protected `basic_streambuf::swap` fails to exchange locales. Quality: default bulk transfer loops through scalar virtual operations instead of copying available contiguous runs.                                                                                                                                                          |
| `<istream>` / `<ostream>`                                     | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 31.7.1--31.7.2, 31.7.5--31.7.7                                | Most formatted/unformatted operations fail to catch buffer/facet exceptions, set `badbit`, and conditionally rethrow; stream-buffer insertion records `failbit` instead of `badbit`.                                                                                                                                                       |
| `<iomanip>`                                                   | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 31.7.3, 31.7.8--31.7.9                                        | Monetary/time manipulators inherit locale parsing, truncation, and exception-state defects. Quality: quoted output constructs a complete escaped temporary.                                                                                                                                                                                |
| `<print>`                                                     | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 22.14, 31.7.4, 31.7.10                                        | Complete print/println/vprint interface and terminal routing, but extended arithmetic calls inherit formatter rejection. Quality: every call stages the whole result in a string, and `println` may reallocate for one newline.                                                                                                            |
| `<sstream>`                                                   | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 31.8                                                          | Put positions above `INT_MAX` are narrowed through `pbump(int)`, and seek addition can overflow before validation; wrappers inherit locale-swap and exception-state failures.                                                                                                                                                              |
| `<strstream>`                                                 | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, D.15                                                          | Missing `underflow` hides newly written characters; external-buffer construction establishes the wrong put area; default growth uses `malloc`/`free` instead of `new[]`/`delete[]`. Quality: unchecked capacity doubling and full-capacity copies.                                                                                         |
| `<spanstream>`                                                | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 31.9                                                          | Large span put positions narrow to `int`, seek addition can overflow before validation, and wrappers inherit shared buffer/stream defects.                                                                                                                                                                                                 |
| `<fstream>`                                                   | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 31.10                                                         | Positions narrow to 32-bit `long` on Windows; update streams omit required read/write transitions; wide putback rejects non-ASCII multibyte sequences. Quality: wide bulk output converts one character at a time.                                                                                                                         |
| `<syncstream>`                                                | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 31.7.7, 31.11                                                 | Emit manipulators are stubs; `osyncstream::emit` omits sentry/exception-state semantics; swap inherits locale loss. Quality: the global lock registry leaks and first lookup is linear.                                                                                                                                                    |
| `<iostream>`                                                  | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 31.2.4, 31.4                                                  | Wide standard streams are ASCII byte adapters; synchronized standard stream operations lack locking around shared C++ formatting/error state; shared exception-state defects propagate.                                                                                                                                                    |
| `<filesystem>`                                                | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 31.12, D.29                                                   | Path source constraints, Windows-native decomposition, non-throwing error reporting, directory-iterator refresh rules, and missing-source copy errors are defective. Quality: ad-hoc FNV hashing despite RapidHash, repeated path rescans, and eager whole-directory snapshots.                                                            |
| `<cstdio>` / `<stdio.h>`                                      | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 16.4.6.10, 31.13.1; C 7.21                                    | Formatted numeric I/O ignores the C locale; wide conversions are byte casts; fixed 512-byte arrays cap precision; `fsetpos` drops conversion state; normal mode omits standard-stream macros.                                                                                                                                              |
| `<cinttypes>` / `<inttypes.h>`                                | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.4.1, 31.13.2; C 7.8                                        | The direct C++23 inventory matches the supported data models, but the included `<cstdint>` greatest-width integer model and associated limits remain nonconforming for wider extended integers.                                                                                                                                            |
| `<ctime>` / `<time.h>`                                        | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.2.2, 17.14; C 7.27                                         | `difftime` can overflow before conversion; handwritten `gmtime` overflows/narrows; pre-epoch `timespec_get` can produce negative nanoseconds; the shortened POSIX `tm` ABI can cause native runtime out-of-bounds access.                                                                                                                  |
| `<__c_file_types.hpp>` / `<__c_wide_types.hpp>` support units | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.2.2, 17.14.4                                               | The opaque C stream handle and implementation-defined owned multibyte state/wide integer types correctly support their parent headers; conversion algorithm defects are recorded under `<cuchar>` and `<cwchar>`.                                                                                                                          |
| `<__c_time_types.hpp>` support unit                           | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.14; C 7.27                                                 | Its nine-field `tm` object is passed to glibc/Darwin APIs whose ABI appends timezone fields, permitting out-of-bounds runtime reads/writes and breaking `%z`/`%Z`.                                                                                                                                                                         |
| `<regex>`                                                     | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 16.4.6.15, 32                                                 | Defaulted moves leave the source marked valid after moving out its program; custom traits break non-throwing moves and constant-time swap. Quality: whole-input copies, repeated capture traversal, and allocating sub-match comparisons.                                                                                                  |
| `<atomic>` / `<stdatomic.h>`                                  | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.3.2, 33.5                                                  | Extended integers miss the integral atomic specialization and its arithmetic/bitwise operations. Quality: 64 shared fallback buckets serialize unrelated objects and Apple weak-import fallback can busy-spin.                                                                                                                             |
| `<stop_token>`                                                | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 33.3                                                          | Stop-state ownership, request linearization, callback registration/execution/destruction, constraints, comparisons, and swaps match C++23.                                                                                                                                                                                                 |
| `<thread>` / `jthread`                                        | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 33.2.4, 33.4                                                  | Large positive `sleep_for` durations can overflow nanoseconds and return immediately; extended duration reps inherit chrono classification failures. Quality: every launch heap-allocates its invocation tuple.                                                                                                                            |
| `<mutex>` / `<shared_mutex>` / `<condition_variable>`         | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 33.2.4--33.2.5, 33.6--33.7                                    | Every native-backed mutex type and `condition_variable` omits required `native_handle_type`/`native_handle()` interfaces; timed operations inherit chrono extended-representation defects.                                                                                                                                                 |
| `<semaphore>`                                                 | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 33.8                                                          | Core counting and wakeup semantics match, but timed acquisition inherits chrono's rejection/misclassification of extended duration representation types.                                                                                                                                                                                   |
| `<latch>`                                                     | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 33.9                                                          | Count, synchronization, waiting, arrival, maximum, ownership, and feature-test semantics match C++23.                                                                                                                                                                                                                                      |
| `<barrier>`                                                   | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 33.10                                                         | Phase, arrival-token, completion, wait, drop, maximum, and feature-test semantics match C++23. Quality: every arrival and the completion serialize under one mutex.                                                                                                                                                                        |
| `<future>`                                                    | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 33.2.4, 33.10                                                 | `packaged_task` tests the source rather than stored decayed callable's invocability; timed waits inherit chrono extended-representation failures. Quality: two allocations and virtual dispatch per packaged task.                                                                                                                         |
| `<stdexcept>`                                                 | **CERTIFIED** | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 17.7, 19.2                                                    | Complete exception hierarchy and shared stable-message semantics match C++23, including non-throwing copy and assignment.                                                                                                                                                                                                                  |
| `<system_error>`                                              | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 19.5                                                          | Windows system-category messages and default conditions incorrectly interpret Win32 error numbers as `errno`/`errc` numbers. Hash support correctly uses RapidHash.                                                                                                                                                                        |
| `<stacktrace>`                                                | **FAILED**    | `798e52c498f5630ecdc03c92456b99a1ff95aa97` | 16.4.2.5, 19.6                                                          | Capture is capped at 128 frames; POSIX bounded capture subtracts `skip` from an already depth-limited request and can return too few or no frames. Quality: entries never symbolize addresses or expose available source information.                                                                                                      |

## Audit policy

Audits and remediation are intentionally separate activities.

The audit also records material library-quality defects discovered during
source inspection, such as avoidable allocation, weak ad-hoc hash combining,
or unnecessary runtime dependencies when an existing FTL facility provides a
clearly stronger implementation. These observations are labeled separately
and do not by themselves determine the ISO certification status.

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
