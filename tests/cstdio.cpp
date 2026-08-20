#ifdef FTL_REPLACE_STL
#include <clocale>
#include <cstdio>
#include <cwchar>
#include <type_traits>
namespace tested = std;
#else
#include <cstdio>
#include <ftl/clocale>
#include <ftl/cstdio>
#include <ftl/cwchar>
#include <ftl/type_traits>
namespace tested = ftl;
static_assert(!tested::is_same_v<::FILE, tested::FILE>);
static_assert(tested::is_same_v<decltype(stdout), ::FILE *>);
#endif
using print_type = int (*)(const char *, ...);
static_assert(tested::is_same_v<
              decltype(static_cast<print_type>(&tested::printf)), print_type>);
bool cstdio_failure(const char *message) {
  tested::fprintf(::ftl_stdio_runtime::error_stream(),
                  "cstdio regression failed: %s\n", message);
  return false;
}
bool ftl_test() {
  char formatted[64]{};
  int written =
      tested::snprintf(formatted, sizeof(formatted), "%+06d %#x %.3s %.2f", 42,
                       0x2a, "stdio", 1.25);
  const char expected[] = "+00042 0x2a std 1.25";
  if (written != 20)
    return false;
  for (tested::size_t index = 0; index != sizeof(expected); ++index)
    if (formatted[index] != expected[index])
      return false;
  char truncated[5]{};
  if (tested::snprintf(truncated, sizeof(truncated), "abcdef") != 6 ||
      truncated[3] != 'd' || truncated[4] != '\0')
    return false;
  if (tested::snprintf(nullptr, 0, "%d", 1234) != 4)
    return false;
  char high_precision[800]{};
  if (tested::snprintf(high_precision, sizeof(high_precision), "%.700f", 1.0) !=
          702 ||
      high_precision[0] != '1' || high_precision[1] != '.' ||
      high_precision[701] != '0' || high_precision[702] != '\0')
    return cstdio_failure("high precision output");
  int count_written = -1;
  int format_result = tested::snprintf(
      formatted, sizeof(formatted), "%#.0f %.1a%n", 1.0, 3.0, &count_written);
  if (format_result != 11 || count_written != 11 || formatted[1] != '.' ||
      formatted[3] != '0' || formatted[4] != 'x' || formatted[7] != '8')
    return false;
  if (tested::snprintf(formatted, sizeof(formatted), "%#.6g", 1.25) != 7 ||
      formatted[0] != '1' || formatted[1] != '.' || formatted[6] != '0')
    return false;
  if (tested::snprintf(formatted, sizeof(formatted), "%#.3o %#.0o", 1u, 0u) !=
          5 ||
      formatted[0] != '0' || formatted[2] != '1' || formatted[4] != '0')
    return false;
  int decimal = 0;
  unsigned hexadecimal = 0;
  char word[8]{};
  double floating = 0;
  if (tested::sscanf(" -17 0x2a stdio 1.25", "%d %x %7s %lf", &decimal,
                     &hexadecimal, word, &floating) != 4 ||
      decimal != -17 || hexadecimal != 0x2a || word[4] != 'o' ||
      floating != 1.25)
    return false;
  char remainder = 0;
  floating = 0;
  if (tested::sscanf("1.25xyz", "%lf%c", &floating, &remainder) != 2 ||
      floating != 1.25 || remainder != 'x')
    return false;
  floating = 0;
  if (tested::sscanf("0x1.8p+1", "%la", &floating) != 1 || floating != 3.0)
    return false;
  floating = 0;
  double hexadecimal_floating = 0;
  if (tested::sscanf("+1.5 +0x1p+1", "%lf %la", &floating,
                     &hexadecimal_floating) != 2 ||
      floating != 1.5 || hexadecimal_floating != 2.0)
    return false;
  floating = 0;
  remainder = 0;
  if (tested::sscanf("nan(payload)x", "%lf%c", &floating, &remainder) != 2 ||
      floating == floating || remainder != 'x')
    return false;
  if (tested::sscanf("", "x") != EOF || tested::sscanf("", "%%") != EOF)
    return false;

#ifdef FTL_REPLACE_STL
  const char *path = "ftl-cstdio-replace.tmp";
  const char *renamed = "ftl-cstdio-replace-renamed.tmp";
#else
  const char *path = "ftl-cstdio-normal.tmp";
  const char *renamed = "ftl-cstdio-normal-renamed.tmp";
#endif
  tested::FILE *text_file = tested::fopen(path, "w+");
  char user_buffer[32];
  if (!text_file ||
      tested::setvbuf(text_file, user_buffer, _IOFBF, sizeof(user_buffer)) !=
          0 ||
      tested::fputs("a\nb", text_file) < 0 ||
      tested::setvbuf(text_file, nullptr, _IONBF, 0) == 0 ||
      tested::fclose(text_file) != 0)
    return false;
  text_file = tested::fopen(path, "ab+");
  if (!text_file || tested::fputc('c', text_file) != 'c')
    return false;
  text_file = tested::freopen(path, "rb", text_file);
  if (!text_file)
    return false;
  char physical[8]{};
  tested::size_t physical_size =
      tested::fread(physical, 1, sizeof(physical), text_file);
#if defined(_WIN32)
  if (physical_size != 5 || physical[0] != 'a' || physical[1] != '\r' ||
      physical[2] != '\n' || physical[3] != 'b' || physical[4] != 'c')
    return false;
#else
  if (physical_size != 4 || physical[0] != 'a' || physical[1] != '\n' ||
      physical[2] != 'b' || physical[3] != 'c')
    return false;
#endif
  if (!tested::feof(text_file) || tested::fputc('x', text_file) != EOF ||
      !tested::ferror(text_file))
    return false;
  tested::clearerr(text_file);
  if (tested::feof(text_file) || tested::ferror(text_file))
    return false;
  if (tested::fclose(text_file) != 0 || tested::rename(path, renamed) != 0 ||
      tested::remove(renamed) != 0)
    return false;

  tested::FILE *file = tested::tmpfile();
  if (!file)
    return false;
  const char text[] = "stdio";
  if (tested::fwrite(text, 1, 5, file) != 5 || tested::ftell(file) != 5 ||
      tested::fflush(nullptr) != 0)
    return false;
  tested::fpos_t end_position{};
  if (tested::fgetpos(file, &end_position) != 0)
    return false;
  tested::rewind(file);
  char empty[1] = {'x'};
  if (tested::fgets(empty, 1, file) != empty || empty[0] != '\0' ||
      tested::ftell(file) != 0 || tested::getc(file) != 's' ||
      tested::ungetc('s', file) != 's')
    return false;
  char read[6]{};
  bool okay = tested::fgets(read, sizeof(read), file) == read &&
              read[4] == 'o' && tested::fgetc(file) == EOF &&
              tested::feof(file) && tested::fsetpos(file, &end_position) == 0 &&
              tested::ftell(file) == 5;
  if (tested::fclose(file) != 0 || !okay)
    return false;

  const char *wide_locale =
#if defined(_WIN32)
      ".UTF-8";
#elif defined(__APPLE__)
      "en_US.UTF-8";
#else
      "C.utf8";
#endif
  if (tested::setlocale(LC_CTYPE, wide_locale) == nullptr)
    return cstdio_failure("UTF-8 locale selection");
  tested::FILE *wide_file = tested::tmpfile();
  if (!wide_file || tested::fputwc(L'\u00e9', wide_file) == WEOF ||
      tested::fputwc(L'X', wide_file) == WEOF)
    return cstdio_failure("wide output");
  tested::fpos_t wide_end{};
  if (tested::fgetpos(wide_file, &wide_end) != 0)
    return cstdio_failure("wide end position");
  tested::rewind(wide_file);
  if (tested::fgetwc(wide_file) != L'\u00e9' ||
      tested::ungetwc(L'\u00e9', wide_file) == WEOF)
    return cstdio_failure("wide input or pushback");
  tested::fpos_t pushed_position{};
  if (tested::fgetpos(wide_file, &pushed_position) != 0 ||
      pushed_position.position != 0 ||
      tested::fgetwc(wide_file) != L'\u00e9' ||
      tested::fgetwc(wide_file) != L'X' ||
      tested::fsetpos(wide_file, &wide_end) != 0)
    return cstdio_failure("wide position state or replay");
  return tested::fclose(wide_file) == 0;
}
