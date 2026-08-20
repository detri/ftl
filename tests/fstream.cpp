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

class two_byte_wide_codecvt final
    : public tested::codecvt<wchar_t, char, tested::mbstate_t> {
protected:
  result do_out(state_type &, const intern_type *from,
                const intern_type *from_end, const intern_type *&from_next,
                extern_type *to, extern_type *to_end,
                extern_type *&to_next) const override {
    from_next = from;
    to_next = to;
    while (from_next != from_end) {
      if (*from_next == L'\u00e9') {
        if (to_end - to_next < 2)
          return partial;
        *to_next++ = static_cast<char>(0xc3);
        *to_next++ = static_cast<char>(0xa9);
      } else {
        if (to_next == to_end || static_cast<unsigned long>(*from_next) > 0x7f)
          return to_next == to_end ? partial : error;
        *to_next++ = static_cast<char>(*from_next);
      }
      ++from_next;
    }
    return ok;
  }

  result do_in(state_type &, const extern_type *from,
               const extern_type *from_end, const extern_type *&from_next,
               intern_type *to, intern_type *to_end,
               intern_type *&to_next) const override {
    from_next = from;
    to_next = to;
    if (from == from_end)
      return partial;
    if (to == to_end)
      return partial;
    if (static_cast<unsigned char>(*from) == 0xc3) {
      if (from_end - from < 2)
        return partial;
      if (static_cast<unsigned char>(from[1]) != 0xa9)
        return error;
      *to_next++ = L'\u00e9';
      from_next += 2;
    } else {
      if (static_cast<unsigned char>(*from) > 0x7f)
        return error;
      *to_next++ = static_cast<unsigned char>(*from_next++);
    }
    return ok;
  }

  result do_unshift(state_type &, extern_type *to, extern_type *,
                    extern_type *&to_next) const override {
    to_next = to;
    return noconv;
  }
  int do_encoding() const noexcept override { return 0; }
  bool do_always_noconv() const noexcept override { return false; }
  int do_length(state_type &, const extern_type *from,
                const extern_type *from_end,
                tested::size_t maximum) const override {
    tested::size_t count = 0;
    const extern_type *current = from;
    while (current != from_end && count != maximum) {
      current += static_cast<unsigned char>(*current) == 0xc3 ? 2 : 1;
      if (current > from_end)
        break;
      ++count;
    }
    return static_cast<int>(current - from);
  }
  int do_max_length() const noexcept override { return 2; }
};

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
  tested::locale wide_locale(tested::locale::classic(),
                             new two_byte_wide_codecvt);
  {
    tested::wofstream output(wide_name,
                             tested::ios_base::binary | tested::ios_base::trunc);
    output.imbue(wide_locale);
    output << L'\u00e9' << L'x';
    if (output.fail()) {
      tested::remove(name);
      tested::remove(wide_name);
      return false;
    }
  }
  {
    tested::wifstream input(wide_name, tested::ios_base::binary);
    input.imbue(wide_locale);
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
