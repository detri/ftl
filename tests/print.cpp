#ifdef FTL_REPLACE_STL
#include <print>
namespace tested = std;
#else
#include <ftl/print>
namespace tested = ftl;
#endif

namespace test_stdio_runtime {

using size_type = decltype(sizeof(0));

extern "C" {

FILE *tmpfile();

int fclose(FILE *);

int fflush(FILE *);

int fseek(FILE *, long, int);

size_type fread(void *, size_type, size_type, FILE *);

} // extern "C"

} // namespace test_stdio_runtime

static_assert(__cpp_lib_print >= 202207L);

bool equal_bytes(const char *left, const char *right, tested::size_t size) {
  for (tested::size_t index = 0; index < size; ++index) {
    if (left[index] != right[index]) {
      return false;
    }
  }

  return true;
}

tested::size_t text_size(const char *value) {
  tested::size_t result = 0;

  while (value[result] != '\0')
    ++result;

  return result;
}

bool stream_equals(FILE *stream, const char *expected,
                   tested::size_t expected_size) {
  if (test_stdio_runtime::fflush(stream) != 0) {
    return false;
  }

  if (test_stdio_runtime::fseek(stream, 0, 0) != 0) {
    return false;
  }

  char buffer[1024]{};

  if (expected_size > sizeof(buffer)) {
    return false;
  }

  const tested::size_t read =
      test_stdio_runtime::fread(buffer, 1, sizeof(buffer), stream);

  return read == expected_size && equal_bytes(buffer, expected, expected_size);
}

bool basic_print_works() {
  FILE *stream = test_stdio_runtime::tmpfile();

  if (stream == nullptr)
    return false;

  tested::print(stream, "value={} hex={:#x}", 42, 42);

  tested::println(stream, " padded={:04}", 7);

  constexpr char expected[] = "value=42 hex=0x2a"
                              " padded=0007\n";

  const bool result = stream_equals(stream, expected, sizeof(expected) - 1);

  if (test_stdio_runtime::fclose(stream) != 0) {
    return false;
  }

  return result;
}

bool vprint_works() {
  FILE *stream = test_stdio_runtime::tmpfile();

  if (stream == nullptr)
    return false;

  int value = 123;

  auto arguments = tested::make_format_args(value);

  tested::vprint_nonunicode(stream, "<{}>", arguments);

  tested::string text{"abc"};

  auto text_arguments = tested::make_format_args(text);

  tested::vprint_unicode(stream, "[{}]", text_arguments);

  constexpr char expected[] = "<123>[abc]";

  const bool result = stream_equals(stream, expected, sizeof(expected) - 1);

  if (test_stdio_runtime::fclose(stream) != 0) {
    return false;
  }

  return result;
}

bool unicode_print_works() {
  FILE *stream = test_stdio_runtime::tmpfile();

  if (stream == nullptr)
    return false;

  /*
   * Redirected/file output from
   * vprint_unicode remains UTF-8,
   * even on Windows.
   */
  tested::string unicode{"\xf0\x9f\xa4\xa1"
                         " "
                         "\xe2\x99\xa5"};

  auto store = tested::make_format_args(unicode);

  tested::vprint_unicode(stream, "{}", tested::format_args(store));

  constexpr char expected[] = "\xf0\x9f\xa4\xa1"
                              " "
                              "\xe2\x99\xa5";

  const bool result = stream_equals(stream, expected, sizeof(expected) - 1);

  if (test_stdio_runtime::fclose(stream) != 0) {
    return false;
  }

  return result;
}

bool embedded_null_print_works() {
  FILE *stream = test_stdio_runtime::tmpfile();

  if (stream == nullptr)
    return false;

  const char raw[] = {'a', '\0', 'b'};

  tested::string value(raw, 3);

  tested::print(stream, "{}", value);

  constexpr char expected[] = {'a', '\0', 'b'};

  const bool result = stream_equals(stream, expected, sizeof(expected));

  if (test_stdio_runtime::fclose(stream) != 0) {
    return false;
  }

  return result;
}

bool formatting_integration_works() {
  FILE *stream = test_stdio_runtime::tmpfile();

  if (stream == nullptr)
    return false;

  /*
   * This intentionally exercises
   * things inherited from <format>,
   * rather than giving <print> a
   * second formatting engine.
   */
  tested::println(stream, "{:^9} {:08x} {:.2f}", "FTL", 0xbeef, 3.14159);

  constexpr char expected[] = "   FTL    0000beef 3.14\n";

  const bool result = stream_equals(stream, expected, sizeof(expected) - 1);

  if (test_stdio_runtime::fclose(stream) != 0) {
    return false;
  }

  return result;
}

bool ftl_test() {
  if (!basic_print_works())
    return false;

  if (!vprint_works())
    return false;

  if (!unicode_print_works())
    return false;

  if (!embedded_null_print_works())
    return false;

  if (!formatting_integration_works())
    return false;

  return true;
}
