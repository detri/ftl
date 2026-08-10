// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_LOCALE_H_HEADER
#define FTL_LOCALE_H_HEADER

#ifndef NULL
#define NULL 0
#endif

//
// The C locale category values are part of the platform C-runtime ABI.
// Linux uses the POSIX/glibc ordering. Windows and Darwin use the
// traditional BSD/MSVCRT ordering.
//
#if defined(__linux__)

#define LC_CTYPE 0
#define LC_NUMERIC 1
#define LC_TIME 2
#define LC_COLLATE 3
#define LC_MONETARY 4
#define LC_ALL 6

#elif defined(_WIN32) || defined(__APPLE__)

#define LC_ALL 0
#define LC_COLLATE 1
#define LC_CTYPE 2
#define LC_MONETARY 3
#define LC_NUMERIC 4
#define LC_TIME 5

#else

#error "FTL <locale.h> does not know the platform locale category ABI"

#endif

struct lconv {
  char *decimal_point;
  char *thousands_sep;
  char *grouping;

  char *int_curr_symbol;
  char *currency_symbol;
  char *mon_decimal_point;
  char *mon_thousands_sep;
  char *mon_grouping;
  char *positive_sign;
  char *negative_sign;

  char int_frac_digits;
  char frac_digits;

  char p_cs_precedes;
  char p_sep_by_space;
  char n_cs_precedes;
  char n_sep_by_space;

  char p_sign_posn;
  char n_sign_posn;

  char int_p_cs_precedes;
  char int_p_sep_by_space;
  char int_n_cs_precedes;
  char int_n_sep_by_space;

  char int_p_sign_posn;
  char int_n_sign_posn;
};

extern "C" {

char *setlocale(int category, const char *locale);

lconv *localeconv();
}

#endif
