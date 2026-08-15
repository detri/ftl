// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_SYSTEM_ERROR_ERRC_HPP
#define FTL_SYSTEM_ERROR_ERRC_HPP

#ifdef FTL_REPLACE_STL
#include <__system_error/errno_values.hpp>
#else
#include <ftl/__system_error/errno_values.hpp>
#endif

#define FTL_DETAIL_ERRC_VALUES(X)                                              \
  X(address_family_not_supported, FTL_DETAIL_ERRNO_EAFNOSUPPORT)               \
  X(address_in_use, FTL_DETAIL_ERRNO_EADDRINUSE)                               \
  X(address_not_available, FTL_DETAIL_ERRNO_EADDRNOTAVAIL)                     \
  X(already_connected, FTL_DETAIL_ERRNO_EISCONN)                               \
  X(argument_list_too_long, FTL_DETAIL_ERRNO_E2BIG)                            \
  X(argument_out_of_domain, FTL_DETAIL_ERRNO_EDOM)                             \
  X(bad_address, FTL_DETAIL_ERRNO_EFAULT)                                      \
  X(bad_file_descriptor, FTL_DETAIL_ERRNO_EBADF)                               \
  X(bad_message, FTL_DETAIL_ERRNO_EBADMSG)                                     \
  X(broken_pipe, FTL_DETAIL_ERRNO_EPIPE)                                       \
  X(connection_aborted, FTL_DETAIL_ERRNO_ECONNABORTED)                         \
  X(connection_already_in_progress, FTL_DETAIL_ERRNO_EALREADY)                 \
  X(connection_refused, FTL_DETAIL_ERRNO_ECONNREFUSED)                         \
  X(connection_reset, FTL_DETAIL_ERRNO_ECONNRESET)                             \
  X(cross_device_link, FTL_DETAIL_ERRNO_EXDEV)                                 \
  X(destination_address_required, FTL_DETAIL_ERRNO_EDESTADDRREQ)               \
  X(device_or_resource_busy, FTL_DETAIL_ERRNO_EBUSY)                           \
  X(directory_not_empty, FTL_DETAIL_ERRNO_ENOTEMPTY)                           \
  X(executable_format_error, FTL_DETAIL_ERRNO_ENOEXEC)                         \
  X(file_exists, FTL_DETAIL_ERRNO_EEXIST)                                      \
  X(file_too_large, FTL_DETAIL_ERRNO_EFBIG)                                    \
  X(filename_too_long, FTL_DETAIL_ERRNO_ENAMETOOLONG)                          \
  X(function_not_supported, FTL_DETAIL_ERRNO_ENOSYS)                           \
  X(host_unreachable, FTL_DETAIL_ERRNO_EHOSTUNREACH)                           \
  X(identifier_removed, FTL_DETAIL_ERRNO_EIDRM)                                \
  X(illegal_byte_sequence, FTL_DETAIL_ERRNO_EILSEQ)                            \
  X(inappropriate_io_control_operation, FTL_DETAIL_ERRNO_ENOTTY)               \
  X(interrupted, FTL_DETAIL_ERRNO_EINTR)                                       \
  X(invalid_argument, FTL_DETAIL_ERRNO_EINVAL)                                 \
  X(invalid_seek, FTL_DETAIL_ERRNO_ESPIPE)                                     \
  X(io_error, FTL_DETAIL_ERRNO_EIO)                                            \
  X(is_a_directory, FTL_DETAIL_ERRNO_EISDIR)                                   \
  X(message_size, FTL_DETAIL_ERRNO_EMSGSIZE)                                   \
  X(network_down, FTL_DETAIL_ERRNO_ENETDOWN)                                   \
  X(network_reset, FTL_DETAIL_ERRNO_ENETRESET)                                 \
  X(network_unreachable, FTL_DETAIL_ERRNO_ENETUNREACH)                         \
  X(no_buffer_space, FTL_DETAIL_ERRNO_ENOBUFS)                                 \
  X(no_child_process, FTL_DETAIL_ERRNO_ECHILD)                                 \
  X(no_link, FTL_DETAIL_ERRNO_ENOLINK)                                         \
  X(no_lock_available, FTL_DETAIL_ERRNO_ENOLCK)                                \
  X(no_message, FTL_DETAIL_ERRNO_ENOMSG)                                       \
  X(no_protocol_option, FTL_DETAIL_ERRNO_ENOPROTOOPT)                          \
  X(no_space_on_device, FTL_DETAIL_ERRNO_ENOSPC)                               \
  X(no_such_device_or_address, FTL_DETAIL_ERRNO_ENXIO)                         \
  X(no_such_device, FTL_DETAIL_ERRNO_ENODEV)                                   \
  X(no_such_file_or_directory, FTL_DETAIL_ERRNO_ENOENT)                        \
  X(no_such_process, FTL_DETAIL_ERRNO_ESRCH)                                   \
  X(not_a_directory, FTL_DETAIL_ERRNO_ENOTDIR)                                 \
  X(not_a_socket, FTL_DETAIL_ERRNO_ENOTSOCK)                                   \
  X(not_connected, FTL_DETAIL_ERRNO_ENOTCONN)                                  \
  X(not_enough_memory, FTL_DETAIL_ERRNO_ENOMEM)                                \
  X(not_supported, FTL_DETAIL_ERRNO_ENOTSUP)                                   \
  X(operation_canceled, FTL_DETAIL_ERRNO_ECANCELED)                            \
  X(operation_in_progress, FTL_DETAIL_ERRNO_EINPROGRESS)                       \
  X(operation_not_permitted, FTL_DETAIL_ERRNO_EPERM)                           \
  X(operation_not_supported, FTL_DETAIL_ERRNO_EOPNOTSUPP)                      \
  X(operation_would_block, FTL_DETAIL_ERRNO_EWOULDBLOCK)                       \
  X(owner_dead, FTL_DETAIL_ERRNO_EOWNERDEAD)                                   \
  X(permission_denied, FTL_DETAIL_ERRNO_EACCES)                                \
  X(protocol_error, FTL_DETAIL_ERRNO_EPROTO)                                   \
  X(protocol_not_supported, FTL_DETAIL_ERRNO_EPROTONOSUPPORT)                  \
  X(read_only_file_system, FTL_DETAIL_ERRNO_EROFS)                             \
  X(resource_deadlock_would_occur, FTL_DETAIL_ERRNO_EDEADLK)                   \
  X(resource_unavailable_try_again, FTL_DETAIL_ERRNO_EAGAIN)                   \
  X(result_out_of_range, FTL_DETAIL_ERRNO_ERANGE)                              \
  X(state_not_recoverable, FTL_DETAIL_ERRNO_ENOTRECOVERABLE)                   \
  X(text_file_busy, FTL_DETAIL_ERRNO_ETXTBSY)                                  \
  X(timed_out, FTL_DETAIL_ERRNO_ETIMEDOUT)                                     \
  X(too_many_files_open_in_system, FTL_DETAIL_ERRNO_ENFILE)                    \
  X(too_many_files_open, FTL_DETAIL_ERRNO_EMFILE)                              \
  X(too_many_links, FTL_DETAIL_ERRNO_EMLINK)                                   \
  X(too_many_symbolic_link_levels, FTL_DETAIL_ERRNO_ELOOP)                     \
  X(value_too_large, FTL_DETAIL_ERRNO_EOVERFLOW)                               \
  X(wrong_protocol_type, FTL_DETAIL_ERRNO_EPROTOTYPE)                          \
  X(no_message_available, FTL_DETAIL_ERRNO_ENODATA)                            \
  X(no_stream_resources, FTL_DETAIL_ERRNO_ENOSR)                               \
  X(not_a_stream, FTL_DETAIL_ERRNO_ENOSTR)                                     \
  X(stream_timeout, FTL_DETAIL_ERRNO_ETIME)

#ifdef FTL_REPLACE_STL
namespace std {
#else
namespace ftl {
#endif

enum class errc {
#define FTL_DETAIL_ERRC_ENUM(name, value) name = value,
  FTL_DETAIL_ERRC_VALUES(FTL_DETAIL_ERRC_ENUM)
#undef FTL_DETAIL_ERRC_ENUM
};

namespace detail {
constexpr bool is_errc_value(int value) noexcept {
  return false
#define FTL_DETAIL_ERRC_MATCH(name, number) || value == number
      FTL_DETAIL_ERRC_VALUES(FTL_DETAIL_ERRC_MATCH)
#undef FTL_DETAIL_ERRC_MATCH
          ;
}
} // namespace detail

#ifdef FTL_REPLACE_STL
} // namespace std
#else
} // namespace ftl
#endif

#undef FTL_DETAIL_ERRC_VALUES

#endif // FTL_SYSTEM_ERROR_ERRC_HPP
