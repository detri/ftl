#ifdef FTL_REPLACE_STL
#include <filesystem>
#include <fstream>
#include <sstream>
namespace tested = std;
#else
#include <ftl/filesystem>
#include <ftl/fstream>
#include <ftl/sstream>
namespace tested = ftl;
#endif

static_assert(tested::filesystem::path::preferred_separator == '/');
static_assert(tested::is_same_v<tested::filesystem::path::value_type, char>);
static_assert(
    tested::ranges::enable_view<tested::filesystem::directory_iterator>);
static_assert(tested::ranges::enable_borrowed_range<
              tested::filesystem::recursive_directory_iterator>);
static_assert(requires(tested::filesystem::path p, tested::error_code ec,
                       tested::filesystem::file_status status) {
  tested::filesystem::absolute(p);
  tested::filesystem::canonical(p, ec);
  tested::filesystem::copy(p, p, tested::filesystem::copy_options::recursive,
                           ec);
  tested::filesystem::copy_file(p, p, ec);
  tested::filesystem::copy_symlink(p, p, ec);
  tested::filesystem::create_directory(p, p, ec);
  tested::filesystem::create_directories(p, ec);
  tested::filesystem::create_directory_symlink(p, p, ec);
  tested::filesystem::create_hard_link(p, p, ec);
  tested::filesystem::create_symlink(p, p, ec);
  tested::filesystem::current_path(p, ec);
  tested::filesystem::equivalent(p, p, ec);
  tested::filesystem::exists(status);
  tested::filesystem::file_size(p, ec);
  tested::filesystem::hard_link_count(p, ec);
  tested::filesystem::last_write_time(p, ec);
  tested::filesystem::permissions(p, tested::filesystem::perms::owner_read, ec);
  tested::filesystem::proximate(p, p, ec);
  tested::filesystem::read_symlink(p, ec);
  tested::filesystem::relative(p, p, ec);
  tested::filesystem::remove_all(p, ec);
  tested::filesystem::resize_file(p, 0, ec);
  tested::filesystem::space(p, ec);
  tested::filesystem::status(p, ec);
  tested::filesystem::symlink_status(p, ec);
  tested::filesystem::temp_directory_path(ec);
  tested::filesystem::weakly_canonical(p, ec);
});

bool ftl_test() {
  namespace fs = tested::filesystem;
  fs::path windows("C:/users/aaron/file.txt");
  if (windows.generic_string() != "C:/users/aaron/file.txt" ||
      windows.parent_path() != fs::path("C:/users/aaron") ||
      windows.stem() != fs::path("file") || windows.extension() != ".txt")
    return false;
  if (fs::path("a\\b").filename() != fs::path("a\\b"))
    return false;
  if (fs::path("a/./b/../c").lexically_normal() != fs::path("a/c"))
    return false;
  if (fs::path("foo/./bar/..").lexically_normal() != fs::path("foo/") ||
      fs::path("/a/d").lexically_relative("/a/b/c") != "../../d" ||
      fs::path("a/b/c").lexically_relative("a/b/c/x/y") != "../.." ||
      fs::path("a/../b").lexically_relative("a/b").empty() ||
      fs::path("a//b") != fs::path("a/b"))
    return false;
  fs::path utf8_path(L"caf\u00e9");
  if (utf8_path.u8string() != tested::u8string(u8"caf\u00e9"))
    return false;
  if (fs::u8path(tested::u8string(u8"caf\u00e9")) != utf8_path)
    return false;
  tested::stringstream quoted;
  fs::path quoted_path("a/\"b");
  quoted << quoted_path;
  fs::path quoted_result;
  quoted >> quoted_result;
  if (quoted_result != quoted_path ||
      tested::hash<fs::path>{}(quoted_path) != fs::hash_value(quoted_path))
    return false;

  fs::path root = fs::path("ftl_filesystem_test");
  tested::error_code ec;
  fs::remove_all(root, ec);
  if (!fs::create_directories(root / "a/b", ec) || ec)
    return false;
  {
    tested::ofstream out(root / "a/file.txt");
    out << "native filesystem";
  }
  if (!fs::is_regular_file(root / "a/file.txt", ec) || ec ||
      fs::file_size(root / "a/file.txt", ec) != 17 || ec)
    return false;
  if (!fs::copy_file(root / "a/file.txt", root / "a/copy.txt", ec) || ec)
    return false;
  if (fs::copy_file(root / "a/file.txt", root / "a/copy.txt",
                    fs::copy_options::skip_existing, ec) ||
      ec)
    return false;
  auto canonical_root = fs::canonical(root, ec);
  if (ec || !canonical_root.is_absolute())
    return false;
  bool symlinks_created = false;
  fs::create_symlink("file.txt", root / "a/link.txt", ec);
  if (!ec) {
    if (fs::read_symlink(root / "a/link.txt", ec) != "file.txt" || ec ||
        fs::canonical(root / "a/link.txt", ec) !=
            fs::canonical(root / "a/file.txt", ec) ||
        ec)
      return false;
    fs::copy_symlink(root / "a/link.txt", root / "a/link-copy.txt", ec);
    if (ec || !fs::is_symlink(root / "a/link-copy.txt", ec) || ec)
      return false;
    symlinks_created = true;
  } else {
    ec.clear();
  }
  unsigned entries = 0;
  for (fs::directory_iterator i(root / "a", ec);
       !ec && i != fs::directory_iterator(); i.increment(ec))
    ++entries;
  if (ec || entries != (symlinks_created ? 5u : 3u))
    return false;
  unsigned recursive_entries = 0;
  for (fs::recursive_directory_iterator i(root, ec);
       !ec && i != fs::recursive_directory_iterator(); i.increment(ec))
    ++recursive_entries;
  if (ec || recursive_entries != (symlinks_created ? 6u : 4u))
    return false;
  fs::recursive_directory_iterator shallow(root, ec);
  if (ec || shallow == fs::recursive_directory_iterator())
    return false;
  shallow.disable_recursion_pending();
  shallow.increment(ec);
  if (ec || shallow != fs::recursive_directory_iterator())
    return false;
  auto removed = fs::remove_all(root, ec);
  return removed >= 5 && !ec;
}
