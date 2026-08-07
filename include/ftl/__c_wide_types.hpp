// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_C_WIDE_TYPES_HEADER
#define FTL_C_WIDE_TYPES_HEADER

#ifdef FTL_REPLACE_STL
namespace std {
#else
namespace ftl {
#endif

/*
 * FTL owns its multibyte conversion state rather than inheriting the host
 * C runtime's ABI. The two words are sufficient for an accumulated code
 * point plus decoder/encoder state.
 */
struct mbstate_t {
  unsigned int state;
  unsigned int value;
};

/*
 * wint_t is implementation-defined. FTL uses a stable unsigned type capable
 * of representing every supported wide character plus WEOF.
 */
using wint_t = unsigned int;

} // namespace std or ftl

#endif // FTL_C_WIDE_TYPES_HEADER
