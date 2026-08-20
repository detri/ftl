#define FTL_ERRNO_MACROS(X)                                                    \
  X(E2BIG)                                                                     \
  X(EACCES)                                                                    \
  X(EADDRINUSE)                                                                \
  X(EADDRNOTAVAIL)                                                             \
  X(EAFNOSUPPORT)                                                              \
  X(EAGAIN)                                                                    \
  X(EALREADY)                                                                  \
  X(EBADF)                                                                     \
  X(EBADMSG)                                                                   \
  X(EBUSY)                                                                     \
  X(ECANCELED)                                                                 \
  X(ECHILD)                                                                    \
  X(ECONNABORTED)                                                              \
  X(ECONNREFUSED)                                                              \
  X(ECONNRESET)                                                                \
  X(EDEADLK)                                                                   \
  X(EDESTADDRREQ)                                                              \
  X(EDOM)                                                                      \
  X(EEXIST)                                                                    \
  X(EFAULT)                                                                    \
  X(EFBIG)                                                                     \
  X(EHOSTUNREACH)                                                              \
  X(EIDRM)                                                                     \
  X(EILSEQ)                                                                    \
  X(EINPROGRESS)                                                               \
  X(EINTR)                                                                     \
  X(EINVAL)                                                                    \
  X(EIO)                                                                       \
  X(EISCONN)                                                                   \
  X(EISDIR)                                                                    \
  X(ELOOP)                                                                     \
  X(EMFILE)                                                                    \
  X(EMLINK)                                                                    \
  X(EMSGSIZE)                                                                  \
  X(ENAMETOOLONG)                                                              \
  X(ENETDOWN)                                                                  \
  X(ENETRESET)                                                                 \
  X(ENETUNREACH)                                                               \
  X(ENFILE)                                                                    \
  X(ENOBUFS)                                                                   \
  X(ENODEV)                                                                    \
  X(ENOENT)                                                                    \
  X(ENOEXEC)                                                                   \
  X(ENOLCK)                                                                    \
  X(ENOLINK)                                                                   \
  X(ENOMEM)                                                                    \
  X(ENOMSG)                                                                    \
  X(ENOPROTOOPT)                                                               \
  X(ENOSPC)                                                                    \
  X(ENOSYS)                                                                    \
  X(ENOTCONN)                                                                  \
  X(ENOTDIR)                                                                   \
  X(ENOTEMPTY)                                                                 \
  X(ENOTRECOVERABLE)                                                           \
  X(ENOTSOCK)                                                                  \
  X(ENOTSUP)                                                                   \
  X(ENOTTY)                                                                    \
  X(ENXIO)                                                                     \
  X(EOPNOTSUPP)                                                                \
  X(EOVERFLOW)                                                                 \
  X(EOWNERDEAD)                                                                \
  X(EPERM)                                                                     \
  X(EPIPE)                                                                     \
  X(EPROTO)                                                                    \
  X(EPROTONOSUPPORT)                                                           \
  X(EPROTOTYPE)                                                                \
  X(ERANGE)                                                                    \
  X(EROFS)                                                                     \
  X(ESPIPE)                                                                    \
  X(ESRCH)                                                                     \
  X(ETIMEDOUT)                                                                 \
  X(ETXTBSY)                                                                   \
  X(EWOULDBLOCK)                                                               \
  X(EXDEV)                                                                     \
  X(ENODATA)                                                                   \
  X(ENOSR)                                                                     \
  X(ENOSTR)                                                                    \
  X(ETIME)

#ifndef FTL_REPLACE_STL
#include <cerrno>

namespace host_errno {
#define FTL_CAPTURE_HOST_ERRNO(name) inline constexpr int value_##name = name;
FTL_ERRNO_MACROS(FTL_CAPTURE_HOST_ERRNO)
#undef FTL_CAPTURE_HOST_ERRNO
} // namespace host_errno

#include <ftl/cerrno>
#include <ftl/system_error>
namespace tested = ftl;
#else
#include <cerrno>
#include <system_error>
namespace tested = std;
#endif

#ifndef errno
#error "errno must be a macro"
#endif

#ifndef FTL_REPLACE_STL
#define FTL_CHECK_HOST_ERRNO(name)                                             \
  static_assert(FTL_DETAIL_ERRNO_##name == host_errno::value_##name);
FTL_ERRNO_MACROS(FTL_CHECK_HOST_ERRNO)
#undef FTL_CHECK_HOST_ERRNO
#endif

#define FTL_CHECK_PUBLIC_ERRNO(name)                                           \
  static_assert(name == FTL_DETAIL_ERRNO_##name);
FTL_ERRNO_MACROS(FTL_CHECK_PUBLIC_ERRNO)
#undef FTL_CHECK_PUBLIC_ERRNO
#undef FTL_ERRNO_MACROS

#define FTL_ERRC_ERRNO_PAIRS(X)                                                \
  X(address_family_not_supported, EAFNOSUPPORT)                                \
  X(address_in_use, EADDRINUSE)                                                \
  X(address_not_available, EADDRNOTAVAIL)                                      \
  X(already_connected, EISCONN)                                                \
  X(argument_list_too_long, E2BIG)                                             \
  X(argument_out_of_domain, EDOM)                                              \
  X(bad_address, EFAULT)                                                       \
  X(bad_file_descriptor, EBADF)                                                \
  X(bad_message, EBADMSG)                                                      \
  X(broken_pipe, EPIPE)                                                        \
  X(connection_aborted, ECONNABORTED)                                          \
  X(connection_already_in_progress, EALREADY)                                  \
  X(connection_refused, ECONNREFUSED)                                          \
  X(connection_reset, ECONNRESET)                                              \
  X(cross_device_link, EXDEV)                                                  \
  X(destination_address_required, EDESTADDRREQ)                                \
  X(device_or_resource_busy, EBUSY)                                            \
  X(directory_not_empty, ENOTEMPTY)                                            \
  X(executable_format_error, ENOEXEC)                                          \
  X(file_exists, EEXIST)                                                       \
  X(file_too_large, EFBIG)                                                     \
  X(filename_too_long, ENAMETOOLONG)                                           \
  X(function_not_supported, ENOSYS)                                            \
  X(host_unreachable, EHOSTUNREACH)                                            \
  X(identifier_removed, EIDRM)                                                 \
  X(illegal_byte_sequence, EILSEQ)                                             \
  X(inappropriate_io_control_operation, ENOTTY)                                \
  X(interrupted, EINTR)                                                        \
  X(invalid_argument, EINVAL)                                                  \
  X(invalid_seek, ESPIPE)                                                      \
  X(io_error, EIO)                                                             \
  X(is_a_directory, EISDIR)                                                    \
  X(message_size, EMSGSIZE)                                                    \
  X(network_down, ENETDOWN)                                                    \
  X(network_reset, ENETRESET)                                                  \
  X(network_unreachable, ENETUNREACH)                                          \
  X(no_buffer_space, ENOBUFS)                                                  \
  X(no_child_process, ECHILD)                                                  \
  X(no_link, ENOLINK)                                                          \
  X(no_lock_available, ENOLCK)                                                 \
  X(no_message, ENOMSG)                                                        \
  X(no_protocol_option, ENOPROTOOPT)                                           \
  X(no_space_on_device, ENOSPC)                                                \
  X(no_such_device_or_address, ENXIO)                                          \
  X(no_such_device, ENODEV)                                                    \
  X(no_such_file_or_directory, ENOENT)                                         \
  X(no_such_process, ESRCH)                                                    \
  X(not_a_directory, ENOTDIR)                                                  \
  X(not_a_socket, ENOTSOCK)                                                    \
  X(not_connected, ENOTCONN)                                                   \
  X(not_enough_memory, ENOMEM)                                                 \
  X(not_supported, ENOTSUP)                                                    \
  X(operation_canceled, ECANCELED)                                             \
  X(operation_in_progress, EINPROGRESS)                                        \
  X(operation_not_permitted, EPERM)                                            \
  X(operation_not_supported, EOPNOTSUPP)                                       \
  X(operation_would_block, EWOULDBLOCK)                                        \
  X(owner_dead, EOWNERDEAD)                                                    \
  X(permission_denied, EACCES)                                                 \
  X(protocol_error, EPROTO)                                                    \
  X(protocol_not_supported, EPROTONOSUPPORT)                                   \
  X(read_only_file_system, EROFS)                                              \
  X(resource_deadlock_would_occur, EDEADLK)                                    \
  X(resource_unavailable_try_again, EAGAIN)                                    \
  X(result_out_of_range, ERANGE)                                               \
  X(state_not_recoverable, ENOTRECOVERABLE)                                    \
  X(text_file_busy, ETXTBSY)                                                   \
  X(timed_out, ETIMEDOUT)                                                      \
  X(too_many_files_open_in_system, ENFILE)                                     \
  X(too_many_files_open, EMFILE)                                               \
  X(too_many_links, EMLINK)                                                    \
  X(too_many_symbolic_link_levels, ELOOP)                                      \
  X(value_too_large, EOVERFLOW)                                                \
  X(wrong_protocol_type, EPROTOTYPE)                                           \
  X(no_message_available, ENODATA)                                             \
  X(no_stream_resources, ENOSR)                                                \
  X(not_a_stream, ENOSTR)                                                      \
  X(stream_timeout, ETIME)

#define FTL_CHECK_ERRC_ERRNO(name, macro_name)                                 \
  static_assert(static_cast<int>(tested::errc::name) == macro_name);
FTL_ERRC_ERRNO_PAIRS(FTL_CHECK_ERRC_ERRNO)
#undef FTL_CHECK_ERRC_ERRNO
#undef FTL_ERRC_ERRNO_PAIRS

bool ftl_test() {
  errno = EILSEQ;
  return errno == EILSEQ;
}
