// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_C_WIDE_TYPES_HEADER
#define FTL_C_WIDE_TYPES_HEADER

namespace std {

/*
 * FTL owns its multibyte conversion state rather than inheriting the host
 * C runtime's ABI. It stores an accumulated code point plus enough pending
 * input for a restartable native multibyte conversion.
 */
struct mbstate_t {
  unsigned int state;
  unsigned int value;
  unsigned int pending_count;
  unsigned int native_active;
  unsigned char pending[16];
  alignas(8) unsigned char native_state[128];
};

/*
 * wint_t is implementation-defined. FTL uses a stable unsigned type capable
 * of representing every supported wide character plus WEOF.
 */
using wint_t = unsigned int;

} // namespace std or ftl

#endif // FTL_C_WIDE_TYPES_HEADER
