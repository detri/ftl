#pragma once

#include <ftl/detail/tzdb_core.hpp>

namespace ftl_tzdb_tail {

ftl_tzdb_runtime_core::zone_interval
lookup(ftl_tzdb_runtime_core::zone_ref zone, long long timestamp,
       const ftl_tzdb_runtime_core::zone_interval &seed) noexcept;

} // namespace ftl_tzdb_tail
