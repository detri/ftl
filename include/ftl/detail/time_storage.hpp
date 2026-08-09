// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_TIME_STORAGE_HEADER
#define FTL_DETAIL_TIME_STORAGE_HEADER

#ifdef FTL_REPLACE_STL
#include <__c_time_types.hpp>
#include <type_traits>
#else
#include <ftl/__c_time_types.hpp>
#include <ftl/type_traits>
#endif

FTL_BEGIN_NAMESPACE
namespace detail {
inline thread_local tm ctime_tm_storage{};
inline thread_local char ctime_text_storage[26]{};
} // namespace detail
FTL_END_NAMESPACE
#endif
