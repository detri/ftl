#include <ftl/detail/current_zone.hpp>
#include <ftl/detail/tzdb_runtime.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(_WIN32)

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#elif defined(__APPLE__)

#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>

#else

#include <unistd.h>

#endif

namespace ftl_current_zone_runtime {
namespace {

result copy_name(const char *source, char *buffer,
                 size_type capacity) noexcept {
  if (source == nullptr || source[0] == '\0') {
    return {
        status::unavailable,
        0,
    };
  }

  const size_type length = std::strlen(source);

  if (buffer == nullptr || capacity <= length) {
    return {
        status::buffer_too_small,
        length,
    };
  }

  std::memcpy(buffer, source, length);

  buffer[length] = '\0';

  return {
      status::success,
      length,
  };
}

#if !defined(_WIN32)

result extract_zoneinfo_name(const char *path, char *buffer,
                             size_type capacity) noexcept {
  if (path == nullptr)
    return {
        status::unavailable,
        0,
    };

  const char *candidate = nullptr;
  const char *scan = path;

  /*
   * Keep the final /zoneinfo/ occurrence.
   *
   * This handles normal Linux paths:
   *
   *   /usr/share/zoneinfo/America/New_York
   *
   * and Darwin's versioned database paths:
   *
   *   /var/db/timezone/.../zoneinfo/America/New_York
   */
  while (true) {
    const char *found = std::strstr(scan, "/zoneinfo/");

    if (found == nullptr)
      break;

    candidate = found + sizeof("/zoneinfo/") - 1;

    scan = candidate;
  }

  if (candidate == nullptr || candidate[0] == '\0') {
    return {
        status::unavailable,
        0,
    };
  }

  /*
   * Some Unix installations route localtime through
   * the posix/ or right/ subtrees. Those directories
   * describe the same geographical zone identity.
   */
  if (std::strncmp(candidate, "posix/", 6) == 0 ||
      std::strncmp(candidate, "right/", 6) == 0) {
    candidate += 6;
  }

  return copy_name(candidate, buffer, capacity);
}

result try_realpath_zone(const char *path, char *buffer,
                         size_type capacity) noexcept {
  char *resolved = ::realpath(path, nullptr);

  if (resolved == nullptr) {
    return {
        status::unavailable,
        0,
    };
  }

  const result discovered = extract_zoneinfo_name(resolved, buffer, capacity);

  std::free(resolved);

  return discovered;
}

result try_first_line(const char *path, char *buffer,
                      size_type capacity) noexcept {
  std::FILE *file = std::fopen(path, "r");

  if (file == nullptr) {
    return {
        status::unavailable,
        0,
    };
  }

  char value[1024]{};

  const bool have_line = std::fgets(value, sizeof(value), file) != nullptr;

  std::fclose(file);

  if (!have_line) {
    return {
        status::unavailable,
        0,
    };
  }

  char *begin = value;

  while (*begin == ' ' || *begin == '\t' || *begin == '\r' || *begin == '\n') {
    ++begin;
  }

  char *end = begin + std::strlen(begin);

  while (end != begin) {
    const char character = end[-1];

    if (character != ' ' && character != '\t' && character != '\r' &&
        character != '\n') {
      break;
    }

    --end;
  }

  *end = '\0';

  return copy_name(begin, buffer, capacity);
}

#endif

#if defined(__APPLE__)

result apple_system_zone(char *buffer, size_type capacity) noexcept {
  CFTimeZoneRef zone = CFTimeZoneCopySystem();

  if (zone == nullptr) {
    return {
        status::unavailable,
        0,
    };
  }

  CFStringRef name = CFTimeZoneGetName(zone);

  if (name == nullptr) {
    CFRelease(zone);

    return {
        status::unavailable,
        0,
    };
  }

  const CFIndex length = CFStringGetLength(name);

  const CFIndex maximum =
      CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);

  if (maximum < 0) {
    CFRelease(zone);

    return {
        status::unavailable,
        0,
    };
  }

  char *temporary =
      static_cast<char *>(std::malloc(static_cast<size_type>(maximum + 1)));

  if (temporary == nullptr) {
    CFRelease(zone);

    return {
        status::unavailable,
        0,
    };
  }

  const Boolean converted =
      CFStringGetCString(name, temporary, maximum + 1, kCFStringEncodingUTF8);

  CFRelease(zone);

  if (!converted) {
    std::free(temporary);

    return {
        status::unavailable,
        0,
    };
  }

  /*
   * CoreFoundation can use GMT/UTC as its system
   * identifier. Normalize those to the canonical
   * IANA zone we definitely carry.
   */
  const char *source = temporary;

  if (std::strcmp(temporary, "GMT") == 0 ||
      std::strcmp(temporary, "UTC") == 0) {
    source = "Etc/UTC";
  }

  const result discovered = copy_name(source, buffer, capacity);

  std::free(temporary);

  return discovered;
}

#endif

} // namespace

result current_zone_name(char *buffer, size_type capacity) noexcept {
#if defined(_WIN32)

  DYNAMIC_TIME_ZONE_INFORMATION
  information{};

  const DWORD state = GetDynamicTimeZoneInformation(&information);

  if (state == TIME_ZONE_ID_INVALID) {
    return {
        status::unavailable,
        0,
    };
  }

  if (information.TimeZoneKeyName[0] == L'\0') {
    return {
        status::unavailable,
        0,
    };
  }

  char windows_name[256]{};

  const int converted = WideCharToMultiByte(
      CP_UTF8, WC_ERR_INVALID_CHARS, information.TimeZoneKeyName, -1,
      windows_name, static_cast<int>(sizeof(windows_name)), nullptr, nullptr);

  if (converted <= 1) {
    return {
        status::unavailable,
        0,
    };
  }

  const char *iana_name = ftl::detail::tzdb_runtime::windows_zone_target(
      windows_name, static_cast<size_type>(converted - 1));

  /*
   * Some older configurations have historically
   * exposed this spelling rather than the "UTC"
   * Windows key.
   */
  if (iana_name == nullptr &&
      std::strcmp(windows_name, "Coordinated Universal Time") == 0) {
    iana_name = "Etc/UTC";
  }

  if (iana_name == nullptr) {
    return {
        status::unavailable,
        0,
    };
  }

  return copy_name(iana_name, buffer, capacity);

#else

  /*
   * Fast path for conventional Unix installations.
   *
   * This also works on normal macOS installations,
   * including Apple's versioned
   * /var/db/timezone/.../zoneinfo paths.
   */
  result discovered = try_realpath_zone("/etc/localtime", buffer, capacity);

  if (discovered.state != status::unavailable) {
    return discovered;
  }

#if defined(__APPLE__)

  /*
   * Darwin fallback: ask CoreFoundation rather than
   * assuming Apple's filesystem layout remains
   * Linux-shaped forever.
   */
  discovered = apple_system_zone(buffer, capacity);

  if (discovered.state != status::unavailable) {
    return discovered;
  }

#endif

  /*
   * Debian-style configuration.
   */
  discovered = try_first_line("/etc/timezone", buffer, capacity);

  if (discovered.state != status::unavailable) {
    return discovered;
  }

  /*
   * BSD-family configuration fallback.
   */
  discovered = try_first_line("/var/db/zoneinfo", buffer, capacity);

  if (discovered.state != status::unavailable) {
    return discovered;
  }

  return {
      status::unavailable,
      0,
  };

#endif
}

} // namespace ftl_current_zone_runtime
