#ifdef FTL_REPLACE_STL
#include <filesystem>
#include <fstream>
namespace tested = std;
#else
#include <ftl/filesystem>
#include <ftl/fstream>
namespace tested = ftl;
#endif

static_assert(tested::filesystem::path::preferred_separator == '/');
static_assert(tested::is_same_v<tested::filesystem::path::value_type, char>);

bool ftl_test() {
  namespace fs = tested::filesystem;
  fs::path windows("C:\\users\\aaron\\file.txt");
  if (windows.generic_string() != "C:/users/aaron/file.txt" ||
      windows.parent_path() != fs::path("C:/users/aaron") ||
      windows.stem() != fs::path("file") || windows.extension() != ".txt")
    return false;
  if (fs::path("a/./b/../c").lexically_normal() != fs::path("a/c"))
    return false;

  fs::path root = fs::path("ftl_filesystem_test");
  tested::error_code ec;
  fs::remove_all(root, ec);
  if (!fs::create_directories(root / "a/b", ec) || ec)
    return false;
  {
    tested::ofstream out((root / "a/file.txt").c_str());
    out << "native filesystem";
  }
  if (!fs::is_regular_file(root / "a/file.txt", ec) || ec ||
      fs::file_size(root / "a/file.txt", ec) != 17 || ec)
    return false;
  if (!fs::copy_file(root / "a/file.txt", root / "a/copy.txt", ec) || ec)
    return false;
  unsigned entries = 0;
  for (fs::directory_iterator i(root / "a", ec);
       !ec && i != fs::directory_iterator(); i.increment(ec))
    ++entries;
  if (ec || entries != 3)
    return false;
  unsigned recursive_entries = 0;
  for (fs::recursive_directory_iterator i(root, ec);
       !ec && i != fs::recursive_directory_iterator(); i.increment(ec))
    ++recursive_entries;
  if (ec || recursive_entries != 4)
    return false;
  return fs::remove_all(root, ec) == 5 && !ec;
}
