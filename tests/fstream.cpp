#ifdef FTL_REPLACE_STL
#include <cstdio>
#include <fstream>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/cstdio>
#include <ftl/fstream>
#include <ftl/type_traits>
namespace tested = ftl;
#endif
static_assert(tested::is_base_of_v<tested::streambuf, tested::filebuf>);
static_assert(tested::is_base_of_v<tested::istream, tested::ifstream>);
static_assert(tested::is_base_of_v<tested::ostream, tested::ofstream>);
bool ftl_test() {
  const char *name =
#ifdef FTL_REPLACE_STL
      "ftl_fstream_replace.tmp";
#else
      "ftl_fstream_normal.tmp";
#endif
  {
    tested::ofstream out(name,
                         tested::ios_base::binary | tested::ios_base::trunc);
    if (!out.is_open())
      return false;
    out << "17 25";
    out.close();
    if (out.fail())
      return false;
  }
  tested::ifstream in(name, tested::ios_base::binary);
  int a = 0, b = 0;
  in >> a >> b;
  in.close();
  if (a != 17 || b != 25) {
    tested::remove(name);
    return false;
  }

  {
    tested::fstream update(name, tested::ios_base::in | tested::ios_base::out |
                                     tested::ios_base::binary);
    char first = 0;
    update.get(first);
    update.put('X');
    update.seekp(0, tested::ios_base::beg);
    update.put('Q');
    char following = 0;
    update.get(following);
    if (update.fail() || first != '1' || following != 'X') {
      tested::remove(name);
      return false;
    }
  }

  {
    tested::fstream sparse(name, tested::ios_base::in | tested::ios_base::out |
                                     tested::ios_base::binary |
                                     tested::ios_base::trunc);
    constexpr tested::streamoff large_position =
        static_cast<tested::streamoff>(3) * 1024 * 1024 * 1024;
    sparse.seekp(large_position, tested::ios_base::beg);
    sparse.put('z');
    if (sparse.fail() || sparse.tellp() != large_position + 1) {
      tested::remove(name);
      return false;
    }
  }

  const char *wide_name =
#ifdef FTL_REPLACE_STL
      "ftl_fstream_wide_replace.tmp";
#else
      "ftl_fstream_wide_normal.tmp";
#endif
  {
    tested::wofstream output(wide_name,
                             tested::ios_base::binary | tested::ios_base::trunc);
    output << L'\u00e9' << L'x';
    if (output.fail()) {
      tested::remove(name);
      tested::remove(wide_name);
      return false;
    }
  }
  {
    tested::wifstream input(wide_name, tested::ios_base::binary);
    wchar_t first = 0, repeated = 0, second = 0;
    input.get(first);
    input.unget();
    input.get(repeated);
    input.get(second);
    if (input.fail() || first != L'\u00e9' || repeated != first ||
        second != L'x') {
      tested::remove(name);
      tested::remove(wide_name);
      return false;
    }
  }

  tested::remove(name);
  tested::remove(wide_name);
  return true;
}
