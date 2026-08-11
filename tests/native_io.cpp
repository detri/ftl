#ifdef FTL_REPLACE_STL
#include <detail/native_io.hpp>
#else
#include <ftl/detail/native_io.hpp>
#endif

bool ftl_test() {
  using namespace ftl::detail;
#ifdef FTL_REPLACE_STL
  const char *first_path = "ftl-native-io-replace.tmp";
  const char *second_path = "ftl-native-io-replace-renamed.tmp";
#else
  const char *first_path = "ftl-native-io-normal.tmp";
  const char *second_path = "ftl-native-io-normal-renamed.tmp";
#endif
  native_open_options options;
  options.access = native_file_access::read_write;
  options.creation = native_file_creation::create_always;
  native_file_handle handle;
  native_io_error error;
  if (!native_open_file(first_path, options, handle, error) || !handle.valid())
    return false;
  const char expected[] = "native-io";
  native_io_size transferred = 0;
  if (!native_write_file(handle, expected, sizeof(expected) - 1, transferred,
                         error) ||
      transferred != sizeof(expected) - 1)
    return false;
  native_io_offset position = -1;
  if (!native_seek_file(handle, 0, native_seek_origin::begin, position, error) ||
      position != 0)
    return false;
  char actual[sizeof(expected)]{};
  if (!native_read_file(handle, actual, sizeof(expected) - 1, transferred,
                        error) ||
      transferred != sizeof(expected) - 1)
    return false;
  for (native_io_size index = 0; index != transferred; ++index)
    if (actual[index] != expected[index])
      return false;
  if (!native_close_file(handle, error) ||
      !native_rename_file(first_path, second_path, error) ||
      !native_remove_file(second_path, error))
    return false;
  return true;
}
