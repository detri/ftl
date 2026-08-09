// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_SYSTEM_ERROR_ERRC_HPP
#define FTL_SYSTEM_ERROR_ERRC_HPP

#if defined(_WIN32)

#define FTL_DETAIL_ERRC_VALUES(X)                                       \
  X(address_family_not_supported, 102)                                  \
  X(address_in_use, 100)                                                \
  X(address_not_available, 101)                                         \
  X(already_connected, 113)                                             \
  X(argument_list_too_long, 7)                                          \
  X(argument_out_of_domain, 33)                                         \
  X(bad_address, 14)                                                    \
  X(bad_file_descriptor, 9)                                             \
  X(bad_message, 104)                                                   \
  X(broken_pipe, 32)                                                    \
  X(connection_aborted, 106)                                            \
  X(connection_already_in_progress, 103)                                \
  X(connection_refused, 107)                                            \
  X(connection_reset, 108)                                              \
  X(cross_device_link, 18)                                              \
  X(destination_address_required, 109)                                  \
  X(device_or_resource_busy, 16)                                        \
  X(directory_not_empty, 41)                                            \
  X(executable_format_error, 8)                                         \
  X(file_exists, 17)                                                    \
  X(file_too_large, 27)                                                 \
  X(filename_too_long, 38)                                              \
  X(function_not_supported, 40)                                         \
  X(host_unreachable, 110)                                              \
  X(identifier_removed, 111)                                            \
  X(illegal_byte_sequence, 42)                                          \
  X(inappropriate_io_control_operation, 25)                             \
  X(interrupted, 4)                                                     \
  X(invalid_argument, 22)                                               \
  X(invalid_seek, 29)                                                   \
  X(io_error, 5)                                                        \
  X(is_a_directory, 21)                                                 \
  X(message_size, 115)                                                  \
  X(network_down, 116)                                                  \
  X(network_reset, 117)                                                 \
  X(network_unreachable, 118)                                           \
  X(no_buffer_space, 119)                                               \
  X(no_child_process, 10)                                               \
  X(no_link, 121)                                                       \
  X(no_lock_available, 39)                                              \
  X(no_message, 122)                                                    \
  X(no_protocol_option, 123)                                            \
  X(no_space_on_device, 28)                                             \
  X(no_such_device_or_address, 6)                                       \
  X(no_such_device, 19)                                                 \
  X(no_such_file_or_directory, 2)                                       \
  X(no_such_process, 3)                                                 \
  X(not_a_directory, 20)                                                \
  X(not_a_socket, 128)                                                  \
  X(not_connected, 126)                                                 \
  X(not_enough_memory, 12)                                              \
  X(not_supported, 129)                                                 \
  X(operation_canceled, 105)                                            \
  X(operation_in_progress, 112)                                         \
  X(operation_not_permitted, 1)                                         \
  X(operation_not_supported, 130)                                       \
  X(operation_would_block, 140)                                         \
  X(owner_dead, 133)                                                    \
  X(permission_denied, 13)                                              \
  X(protocol_error, 134)                                                \
  X(protocol_not_supported, 135)                                        \
  X(read_only_file_system, 30)                                          \
  X(resource_deadlock_would_occur, 36)                                  \
  X(resource_unavailable_try_again, 11)                                 \
  X(result_out_of_range, 34)                                            \
  X(state_not_recoverable, 127)                                         \
  X(text_file_busy, 139)                                                \
  X(timed_out, 138)                                                     \
  X(too_many_files_open_in_system, 23)                                  \
  X(too_many_files_open, 24)                                            \
  X(too_many_links, 31)                                                 \
  X(too_many_symbolic_link_levels, 114)                                 \
  X(value_too_large, 132)                                               \
  X(wrong_protocol_type, 136)

#elif defined(__APPLE__)

#define FTL_DETAIL_ERRC_VALUES(X)                                       \
  X(address_family_not_supported, 47)                                   \
  X(address_in_use, 48)                                                  \
  X(address_not_available, 49)                                          \
  X(already_connected, 56)                                              \
  X(argument_list_too_long, 7)                                          \
  X(argument_out_of_domain, 33)                                         \
  X(bad_address, 14)                                                    \
  X(bad_file_descriptor, 9)                                             \
  X(bad_message, 94)                                                    \
  X(broken_pipe, 32)                                                    \
  X(connection_aborted, 53)                                             \
  X(connection_already_in_progress, 37)                                 \
  X(connection_refused, 61)                                             \
  X(connection_reset, 54)                                               \
  X(cross_device_link, 18)                                              \
  X(destination_address_required, 39)                                   \
  X(device_or_resource_busy, 16)                                        \
  X(directory_not_empty, 66)                                            \
  X(executable_format_error, 8)                                         \
  X(file_exists, 17)                                                    \
  X(file_too_large, 27)                                                 \
  X(filename_too_long, 63)                                              \
  X(function_not_supported, 78)                                         \
  X(host_unreachable, 65)                                               \
  X(identifier_removed, 90)                                             \
  X(illegal_byte_sequence, 92)                                          \
  X(inappropriate_io_control_operation, 25)                             \
  X(interrupted, 4)                                                     \
  X(invalid_argument, 22)                                               \
  X(invalid_seek, 29)                                                   \
  X(io_error, 5)                                                        \
  X(is_a_directory, 21)                                                 \
  X(message_size, 40)                                                   \
  X(network_down, 50)                                                   \
  X(network_reset, 52)                                                  \
  X(network_unreachable, 51)                                            \
  X(no_buffer_space, 55)                                                \
  X(no_child_process, 10)                                               \
  X(no_link, 97)                                                        \
  X(no_lock_available, 77)                                              \
  X(no_message, 91)                                                     \
  X(no_protocol_option, 42)                                             \
  X(no_space_on_device, 28)                                             \
  X(no_such_device_or_address, 6)                                       \
  X(no_such_device, 19)                                                 \
  X(no_such_file_or_directory, 2)                                       \
  X(no_such_process, 3)                                                 \
  X(not_a_directory, 20)                                                \
  X(not_a_socket, 38)                                                   \
  X(not_connected, 57)                                                  \
  X(not_enough_memory, 12)                                              \
  X(not_supported, 45)                                                  \
  X(operation_canceled, 89)                                             \
  X(operation_in_progress, 36)                                          \
  X(operation_not_permitted, 1)                                         \
  X(operation_not_supported, 102)                                       \
  X(operation_would_block, 35)                                          \
  X(owner_dead, 105)                                                    \
  X(permission_denied, 13)                                              \
  X(protocol_error, 100)                                                \
  X(protocol_not_supported, 43)                                         \
  X(read_only_file_system, 30)                                          \
  X(resource_deadlock_would_occur, 11)                                  \
  X(resource_unavailable_try_again, 35)                                 \
  X(result_out_of_range, 34)                                            \
  X(state_not_recoverable, 104)                                         \
  X(text_file_busy, 26)                                                 \
  X(timed_out, 60)                                                      \
  X(too_many_files_open_in_system, 23)                                  \
  X(too_many_files_open, 24)                                            \
  X(too_many_links, 31)                                                 \
  X(too_many_symbolic_link_levels, 62)                                  \
  X(value_too_large, 84)                                                \
  X(wrong_protocol_type, 41)

#elif defined(__linux__)

#define FTL_DETAIL_ERRC_VALUES(X)                                       \
  X(address_family_not_supported, 97)                                   \
  X(address_in_use, 98)                                                  \
  X(address_not_available, 99)                                          \
  X(already_connected, 106)                                             \
  X(argument_list_too_long, 7)                                          \
  X(argument_out_of_domain, 33)                                         \
  X(bad_address, 14)                                                    \
  X(bad_file_descriptor, 9)                                             \
  X(bad_message, 74)                                                    \
  X(broken_pipe, 32)                                                    \
  X(connection_aborted, 103)                                            \
  X(connection_already_in_progress, 114)                                \
  X(connection_refused, 111)                                            \
  X(connection_reset, 104)                                              \
  X(cross_device_link, 18)                                              \
  X(destination_address_required, 89)                                   \
  X(device_or_resource_busy, 16)                                        \
  X(directory_not_empty, 39)                                            \
  X(executable_format_error, 8)                                         \
  X(file_exists, 17)                                                    \
  X(file_too_large, 27)                                                 \
  X(filename_too_long, 36)                                              \
  X(function_not_supported, 38)                                         \
  X(host_unreachable, 113)                                              \
  X(identifier_removed, 43)                                             \
  X(illegal_byte_sequence, 84)                                          \
  X(inappropriate_io_control_operation, 25)                             \
  X(interrupted, 4)                                                     \
  X(invalid_argument, 22)                                               \
  X(invalid_seek, 29)                                                   \
  X(io_error, 5)                                                        \
  X(is_a_directory, 21)                                                 \
  X(message_size, 90)                                                   \
  X(network_down, 100)                                                  \
  X(network_reset, 102)                                                 \
  X(network_unreachable, 101)                                           \
  X(no_buffer_space, 105)                                               \
  X(no_child_process, 10)                                               \
  X(no_link, 67)                                                        \
  X(no_lock_available, 37)                                              \
  X(no_message, 42)                                                     \
  X(no_protocol_option, 92)                                             \
  X(no_space_on_device, 28)                                             \
  X(no_such_device_or_address, 6)                                       \
  X(no_such_device, 19)                                                 \
  X(no_such_file_or_directory, 2)                                       \
  X(no_such_process, 3)                                                 \
  X(not_a_directory, 20)                                                \
  X(not_a_socket, 88)                                                   \
  X(not_connected, 107)                                                 \
  X(not_enough_memory, 12)                                              \
  X(not_supported, 95)                                                  \
  X(operation_canceled, 125)                                            \
  X(operation_in_progress, 115)                                         \
  X(operation_not_permitted, 1)                                         \
  X(operation_not_supported, 95)                                        \
  X(operation_would_block, 11)                                          \
  X(owner_dead, 130)                                                    \
  X(permission_denied, 13)                                              \
  X(protocol_error, 71)                                                 \
  X(protocol_not_supported, 93)                                         \
  X(read_only_file_system, 30)                                          \
  X(resource_deadlock_would_occur, 35)                                  \
  X(resource_unavailable_try_again, 11)                                 \
  X(result_out_of_range, 34)                                            \
  X(state_not_recoverable, 131)                                         \
  X(text_file_busy, 26)                                                 \
  X(timed_out, 110)                                                     \
  X(too_many_files_open_in_system, 23)                                  \
  X(too_many_files_open, 24)                                            \
  X(too_many_links, 31)                                                 \
  X(too_many_symbolic_link_levels, 40)                                  \
  X(value_too_large, 75)                                                \
  X(wrong_protocol_type, 91)

#else
#error "FTL errc mapping is not defined for this platform"
#endif

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
