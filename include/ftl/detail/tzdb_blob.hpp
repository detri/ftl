// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_TZDB_BLOB_HEADER
#define FTL_DETAIL_TZDB_BLOB_HEADER

namespace ftl_tzdb_blob {

using size_type = decltype(sizeof(0));

const unsigned char *data() noexcept;
size_type size() noexcept;

} // namespace ftl_tzdb_blob

#endif