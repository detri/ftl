// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_SIGNAL_H_HEADER
#define FTL_SIGNAL_H_HEADER

#ifdef FTL_REPLACE_STL
#include <csignal>
#define FTL_SIGNAL_NAMESPACE std
#else
#include <ftl/csignal>
#define FTL_SIGNAL_NAMESPACE ftl
#endif

using FTL_SIGNAL_NAMESPACE::sig_atomic_t;

#undef FTL_SIGNAL_NAMESPACE

#endif // FTL_SIGNAL_H_HEADER
