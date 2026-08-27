// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_TIME_STORAGE_HEADER
#define FTL_DETAIL_TIME_STORAGE_HEADER

#include <__c_time_types.hpp>
#include <type_traits>

namespace std {
namespace detail {
inline thread_local tm ctime_tm_storage{};
inline thread_local char ctime_text_storage[26]{};
} // namespace detail
} // namespace std
#endif
