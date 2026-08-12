// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_TZDB_RUNTIME_HEADER
#define FTL_DETAIL_TZDB_RUNTIME_HEADER

#ifdef FTL_REPLACE_STL
#include <cstddef>
#include <detail/tzdb_core.hpp>
#include <type_traits>
#else
#include <ftl/cstddef>
#include <ftl/detail/tzdb_core.hpp>
#include <ftl/type_traits>
#endif

FTL_BEGIN_NAMESPACE

namespace detail::tzdb_runtime {

using ::ftl_tzdb_runtime_core::invalid_index;

using ::ftl_tzdb_runtime_core::link_ref;
using ::ftl_tzdb_runtime_core::zone_interval;
using ::ftl_tzdb_runtime_core::zone_ref;

using ::ftl_tzdb_runtime_core::local_lookup_result;
using ::ftl_tzdb_runtime_core::local_result_kind;

using ::ftl_tzdb_runtime_core::valid;

using ::ftl_tzdb_runtime_core::leap_count;
using ::ftl_tzdb_runtime_core::link_count;
using ::ftl_tzdb_runtime_core::zone_count;

using ::ftl_tzdb_runtime_core::leap_date_seconds;
using ::ftl_tzdb_runtime_core::leap_elapsed_at_sys;
using ::ftl_tzdb_runtime_core::leap_value;

using ::ftl_tzdb_runtime_core::leap_expiration;
using ::ftl_tzdb_runtime_core::precomputed_until;

using ::ftl_tzdb_runtime_core::version;

using ::ftl_tzdb_runtime_core::windows_zone_count;
using ::ftl_tzdb_runtime_core::windows_zone_target;

using ::ftl_tzdb_runtime_core::link_at;
using ::ftl_tzdb_runtime_core::zone_at;

using ::ftl_tzdb_runtime_core::link_name;
using ::ftl_tzdb_runtime_core::link_target;
using ::ftl_tzdb_runtime_core::zone_name;

using ::ftl_tzdb_runtime_core::find_link;
using ::ftl_tzdb_runtime_core::find_zone;
using ::ftl_tzdb_runtime_core::locate_zone;

using ::ftl_tzdb_runtime_core::interval_at;
using ::ftl_tzdb_runtime_core::lookup;
using ::ftl_tzdb_runtime_core::lookup_local;

} // namespace detail::tzdb_runtime

FTL_END_NAMESPACE

#endif
