// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_CURRENT_ZONE_HEADER
#define FTL_DETAIL_CURRENT_ZONE_HEADER

namespace ftl_current_zone_runtime {

using size_type = decltype(sizeof(0));

enum class status : unsigned char {
  success,
  unavailable,
  buffer_too_small,
};

struct result {
  status state = status::unavailable;
  size_type size = 0;
};

result current_zone_name(char *buffer, size_type capacity) noexcept;

} // namespace ftl_current_zone_runtime

#endif
