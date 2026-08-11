#ifdef FTL_REPLACE_STL
#include <cstdio>
#include <type_traits>
namespace tested = std;
#else
#include <cstdio>
#include <ftl/cstdio>
#include <ftl/type_traits>
namespace tested = ftl;
static_assert(!tested::is_same_v<::FILE, tested::FILE>);
static_assert(tested::is_same_v<decltype(stdout), ::FILE *>);
#endif
using print_type = int (*)(const char *, ...);
static_assert(tested::is_same_v<
              decltype(static_cast<print_type>(&tested::printf)), print_type>);
bool ftl_test() {
  char formatted[64]{};
  int written = tested::snprintf(formatted, sizeof(formatted),
                                 "%+06d %#x %.3s %.2f", 42, 0x2a, "stdio",
                                 1.25);
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
  int count_written = -1;
  if (tested::snprintf(formatted, sizeof(formatted), "%#.0f %.1a%n", 1.0,
                       3.0, &count_written) != 12 ||
      count_written != 12 || formatted[1] != '.' || formatted[3] != '0' ||
      formatted[4] != 'x' || formatted[7] != '8')
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
      tested::setvbuf(text_file, user_buffer, _IOFBF, sizeof(user_buffer)) != 0 ||
      tested::fputs("a\nb", text_file) < 0 ||
      tested::setvbuf(text_file, nullptr, _IONBF, 0) == 0 ||
      tested::fclose(text_file) != 0)
    return false;
  text_file = tested::fopen(path, "ab+");
  if (!text_file || tested::fputc('c', text_file) != 'c' ||
      tested::fclose(text_file) != 0)
    return false;
  text_file = tested::fopen(path, "rb");
  char physical[8]{};
  tested::size_t physical_size = tested::fread(physical, 1, sizeof(physical), text_file);
#if defined(_WIN32)
  if (physical_size != 5 || physical[0] != 'a' || physical[1] != '\r' ||
      physical[2] != '\n' || physical[3] != 'b' || physical[4] != 'c')
    return false;
#else
  if (physical_size != 4 || physical[0] != 'a' || physical[1] != '\n' ||
      physical[2] != 'b' || physical[3] != 'c')
    return false;
#endif
  if (tested::fclose(text_file) != 0 || tested::rename(path, renamed) != 0 ||
      tested::remove(renamed) != 0)
    return false;

  tested::FILE *file = tested::tmpfile();
  if (!file)
    return false;
  const char text[] = "stdio";
  if (tested::fwrite(text, 1, 5, file) != 5)
    return false;
  if (tested::fseek(file, 0, SEEK_SET) != 0)
    return false;
  char read[6]{};
  bool okay = tested::fread(read, 1, 5, file) == 5 && read[4] == 'o';
  return tested::fclose(file) == 0 && okay;
}
