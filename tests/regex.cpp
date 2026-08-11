#ifdef FTL_REPLACE_STL
#include <regex>
#include <string>
#include <vector>
namespace tested = std;
#else
#include <ftl/regex>
#include <ftl/string>
#include <ftl/vector>
namespace tested = ftl;
#endif

struct regex4_traits : tested::regex_traits<char> {
  using base = tested::regex_traits<char>;
  using string_type = typename base::string_type;

  char translate(char c) const {
    return c;
  }

  char translate_nocase(char c) const {
    if (c >= 'A' && c <= 'Z')
      return static_cast<char>(c - 'A' + 'a');

    return c;
  }

  template <class It>
  string_type lookup_collatename(It first, It last) const {
    string_type s(first, last);

    if (s.size() == 1)
      return s;

    //
    // Pretend this locale has the traditional "ch" digraph.
    //
    if (s == "ch")
      return s;

    return {};
  }

  template <class It>
  string_type transform(It first, It last) const {
    string_type s(first, last);
    string_type out;

    out.reserve(s.size());

    //
    // Deliberately define:
    //
    //   c < b < a
    //
    // so the test can prove that collated ranges use transform()
    // rather than raw character-code ordering.
    //
    for (char c : s) {
      if (c == 'c')
        out.push_back('1');
      else if (c == 'b')
        out.push_back('2');
      else if (c == 'a')
        out.push_back('3');
      else
        out.push_back(c);
    }

    return out;
  }

  template <class It>
  string_type transform_primary(It first, It last) const {
    string_type s(first, last);

    for (auto &c : s)
      c = translate_nocase(c);

    return s;
  }
};

using regex4_regex =
    tested::basic_regex<char, regex4_traits>;

static bool basic_match() {
  tested::regex r("(a+)(b)");
  tested::cmatch m;
  if (!tested::regex_match("aaab", m, r))
    return false;
  if (m.size() != 3)
    return false;
  if (m[0].str() != "aaab")
    return false;
  if (m[1].str() != "aaa")
    return false;
  if (m[2].str() != "b")
    return false;
  return true;
}

static bool search_works() {
  tested::regex r("([a-z]+)([0-9]+)");
  tested::cmatch m;
  const char *s = "!!abc123??";
  if (!tested::regex_search(s, m, r))
    return false;
  if (m.position() != 2 || m.length() != 6)
    return false;
  if (m[1].str() != "abc" || m[2].str() != "123")
    return false;
  if (m.prefix().str() != "!!" || m.suffix().str() != "??")
    return false;
  return true;
}

static bool alternation_quantifiers() {
  using tested::regex_match;
  if (!regex_match("abbb", tested::regex("ab+")))
    return false;
  if (!regex_match("ac", tested::regex("a(b|c)")))
    return false;
  if (!regex_match("abab", tested::regex("(ab){2}")))
    return false;
  if (!regex_match("ababab", tested::regex("(ab){2,4}")))
    return false;
  if (regex_match("ababababab", tested::regex("(ab){2,4}")))
    return false;
  if (!regex_match("", tested::regex("a*")))
    return false;
  if (!regex_match("aaa", tested::regex("a+")))
    return false;
  if (!regex_match("a", tested::regex("ab?")))
    return false;
  return true;
}

static bool classes_and_escapes() {
  if (!tested::regex_match("abc_123", tested::regex("\\w+")))
    return false;
  if (!tested::regex_match("12345", tested::regex("\\d+")))
    return false;
  if (!tested::regex_match(" \t\n", tested::regex("\\s+")))
    return false;
  if (!tested::regex_match("abcXYZ", tested::regex("[a-zA-Z]+")))
    return false;
  if (!tested::regex_match("xyz", tested::regex("[^0-9]+")))
    return false;
  if (!tested::regex_match("abc", tested::regex("[[:alpha:]]+")))
    return false;
  if (!tested::regex_match("A", tested::regex("\\x41")))
    return false;
  return true;
}

static bool anchors_and_boundaries() {
  tested::regex r("^hello\\b");
  tested::cmatch m;
  if (!tested::regex_search("hello world", m, r))
    return false;
  if (tested::regex_search("xhello world", m, r))
    return false;

  tested::regex multi("^two$", tested::regex_constants::ECMAScript |
                                   tested::regex_constants::multiline);
  if (!tested::regex_search("one\ntwo\nthree", m, multi))
    return false;
  return m.str() == "two";
}

static bool icase_works() {
  tested::regex r("hello[ ]world", tested::regex_constants::ECMAScript |
                                       tested::regex_constants::icase);
  return tested::regex_match("HeLLo WoRLD", r);
}

static bool backref_works() {
  tested::regex r("(ab+)\\1");
  tested::cmatch m;
  if (!tested::regex_match("abbabb", m, r))
    return false;
  return m[1].str() == "abb";
}

static bool lookahead_works() {
  tested::cmatch m;
  tested::regex yes("foo(?=bar)");
  tested::regex no("foo(?!bar)");
  if (!tested::regex_search("xxfoobar", m, yes) || m.str() != "foo")
    return false;
  if (tested::regex_search("foobar", m, no))
    return false;
  return tested::regex_search("foobaz", m, no) && m.str() == "foo";
}

static bool nosubs_works() {
  tested::regex r("(a)(b)", tested::regex_constants::ECMAScript |
                                tested::regex_constants::nosubs);
  tested::cmatch m;
  if (r.mark_count() != 0)
    return false;
  if (!tested::regex_match("ab", m, r))
    return false;
  return m.size() == 1 && m[0].str() == "ab";
}

static bool replace_works() {
  tested::string s = "John Smith";
  tested::regex r("(John) (Smith)");
  if (tested::regex_replace(s, r, tested::string("$2, $1")) != "Smith, John")
    return false;

  tested::string x = "a1 b22 c333";
  tested::regex digits("\\d+");
  if (tested::regex_replace(x, digits, tested::string("<$&>")) !=
      "a<1> b<22> c<333>")
    return false;

  if (tested::regex_replace(x, digits, tested::string("X"),
                            tested::regex_constants::format_first_only) !=
      "aX b22 c333")
    return false;
  return true;
}

static bool iterator_works() {
  tested::string s = "a1 b22 c333";
  tested::regex r("([a-z])(\\d+)");
  tested::sregex_iterator i(s.begin(), s.end(), r), e;

  int n = 0;
  tested::string got;
  for (; i != e; ++i) {
    if (n++)
      got.push_back(',');
    got += (*i)[1].str();
    got += ':';
    got += (*i)[2].str();
  }
  return n == 3 && got == "a:1,b:22,c:333";
}

static bool token_iterator_works() {
  tested::string s = "one two  three";
  tested::regex ws("\\s+");
  tested::sregex_token_iterator i(s.begin(), s.end(), ws, -1), e;

  tested::vector<tested::string> v;
  for (; i != e; ++i)
    v.push_back(i->str());

  return v.size() == 3 && v[0] == "one" && v[1] == "two" && v[2] == "three";
}

static bool lazy_works() {
  tested::cmatch m;
  if (!tested::regex_search("aaaa", m, tested::regex("a+?")))
    return false;
  return m.str() == "a";
}

static bool zero_length_iterator_doesnt_loop() {
  tested::string s = "aa";
  tested::regex r("a*");
  tested::sregex_iterator i(s.begin(), s.end(), r), e;
  int n = 0;
  for (; i != e && n < 20; ++i)
    ++n;
  return i == e && n < 20;
}

static bool errors_work() {
  try {
    tested::regex broken("(abc");
  } catch (const tested::regex_error &e) {
    return e.code() == tested::regex_constants::error_paren;
  }
  return false;
}

static bool wide_works() {
  tested::wregex r(L"(ab)+");
  tested::wcmatch m;
  return tested::regex_match(L"abab", m, r) && m.str() == L"abab";
}

static bool synopsis_surface() {
  tested::csub_match cs;
  tested::wcsub_match wcs;
  tested::ssub_match ss;
  tested::wssub_match wss;
  (void)cs;
  (void)wcs;
  (void)ss;
  (void)wss;

  tested::pmr::cmatch pm;
  if (pm.ready())
    return false;

  using I = tested::string::const_iterator;
  static_assert(!tested::is_constructible_v<tested::regex_iterator<I>, I, I,
                                            tested::regex &&>);

  static_assert(!tested::is_constructible_v<tested::regex_token_iterator<I>, I,
                                            I, tested::regex &&, int>);

  return true;
}

static bool submatch_surface() {
  tested::cmatch m;
  if (!tested::regex_match("abc", m, tested::regex("(abc)")))
    return false;

  if (!(m[1] == "abc"))
    return false;
  if ((m[1] <=> "abc") != 0)
    return false;

  tested::string s = "abc";
  if (!(m[1] == s))
    return false;
  if ((m[1] <=> s) != 0)
    return false;

  tested::csub_match x = m[1], y;
  y = x;
  x.swap(y);

  return x == "abc" && y == "abc";
}

static bool match_results_surface() {
  tested::cmatch a, b;

  if (!(a == b))
    return false;

  tested::regex r("(a)(b)");
  if (!tested::regex_match("ab", a, r))
    return false;
  if (a == b)
    return false;

  if (!tested::regex_match("ab", b, r))
    return false;
  if (!(a == b))
    return false;

  tested::cmatch copy(a, a.get_allocator());
  if (!(copy == a))
    return false;

  tested::cmatch moved(tested::move(copy), a.get_allocator());
  if (!(moved == a))
    return false;

  tested::string fmt = "$2$1";
  if (a.format(fmt) != "ba")
    return false;

  tested::string out;
  a.format(tested::back_inserter(out), fmt);
  return out == "ba";
}

static bool imbue_invalidates() {
  tested::regex r("(abc)");
  if (!tested::regex_match("abc", r))
    return false;

  auto old = r.imbue(tested::locale::classic());
  (void)old;

  if (tested::regex_match("abc", r))
    return false;

  r.assign("(abc)");
  return tested::regex_match("abc", r);
}

static bool prev_avail_works() {
  tested::cmatch m;

  {
    const char s[] = "xa";
    tested::regex r("\\ba");

    if (!tested::regex_search(s + 1, s + 2, m, r))
      return false;

    if (tested::regex_search(s + 1, s + 2, m, r,
                             tested::regex_constants::match_prev_avail))
      return false;
  }

  {
    const char s[] = " a";
    tested::regex r("\\ba");

    auto f = tested::regex_constants::match_prev_avail |
             tested::regex_constants::match_not_bow;

    if (!tested::regex_search(s + 1, s + 2, m, r, f))
      return false;
  }

  {
    const char s[] = "xa";
    tested::regex r("^a");

    if (tested::regex_search(s + 1, s + 2, m, r,
                             tested::regex_constants::match_prev_avail))
      return false;
  }

  {
    const char s[] = "\na";
    tested::regex r("^a", tested::regex_constants::ECMAScript |
                              tested::regex_constants::multiline);

    auto f = tested::regex_constants::match_prev_avail |
             tested::regex_constants::match_not_bol;

    if (!tested::regex_search(s + 1, s + 2, m, r, f))
      return false;
  }

  return true;
}

static bool iterator_prefix_after_empty() {
  tested::string s = "ba";
  tested::regex r("a*");

  tested::sregex_iterator i(s.begin(), s.end(), r), e;
  if (i == e)
    return false;

  if ((*i)[0].str() != "")
    return false;
  if ((*i).position() != 0)
    return false;

  ++i;
  if (i == e)
    return false;

  if ((*i)[0].str() != "a")
    return false;
  if ((*i).position() != 1)
    return false;

  // The skipped 'b' belongs to the new match's prefix.
  if (i->prefix().str() != "b")
    return false;

  return true;
}

static bool token_array_constructor() {
  tested::string s = "a1 b22";
  tested::regex r("([a-z])(\\d+)");
  const int subs[] = {1, 2};

  tested::sregex_token_iterator i(s.begin(), s.end(), r, subs), e;

  tested::vector<tested::string> out;
  for (; i != e; ++i)
    out.push_back(i->str());

  return out.size() == 4 && out[0] == "a" && out[1] == "1" && out[2] == "b" &&
         out[3] == "22";
}

static bool cstring_replace_surface() {
  tested::regex r("(a)");
  tested::string fmt = "[$1]";

  if (tested::regex_replace("a a", r, fmt) != "[a] [a]")
    return false;

  return tested::regex_replace("a a", r, "X") == "X X";
}

static bool ecma_capture_clearing() {
  tested::regex r("(x)((a+)?(b+)?(c))*");
  tested::cmatch m;

  if (!tested::regex_match("xaacbbbcac", m, r))
    return false;

  if (m.size() != 6)
    return false;

  if (m[0].str() != "xaacbbbcac")
    return false;
  if (m[1].str() != "x")
    return false;
  if (m[2].str() != "ac")
    return false;
  if (m[3].str() != "a")
    return false;

  // b+ matched in an earlier iteration, but NOT in the final one.
  if (m[4].matched)
    return false;

  return m[5].str() == "c";
}

static bool ecma_undefined_backrefs() {
  tested::cmatch m;

  // Forward reference: capture #1 exists in the complete pattern but
  // has not participated yet, so \1 succeeds without consuming input.
  if (!tested::regex_match("a", m, tested::regex("\\1(a)")))
    return false;

  if (m.size() != 2 || m[1].str() != "a")
    return false;

  // Optional capture does not participate; its backref is empty.
  if (!tested::regex_match("", m, tested::regex("(a)?\\1")))
    return false;

  if (m[1].matched)
    return false;

  return true;
}

static bool ecma_decimal_backrefs() {
  tested::cmatch m;

  tested::regex ten("(a)(b)(c)(d)(e)(f)(g)(h)(i)(j)\\10");

  if (!tested::regex_match("abcdefghijj", m, ten))
    return false;

  if (m.size() != 11 || m[10].str() != "j")
    return false;

  try {
    tested::regex bad("(a)\\2");
    return false;
  } catch (const tested::regex_error &) {
  }

  try {
    // Parsed as backreference 10, not backreference 1 + '0'.
    tested::regex bad("(a)\\10");
    return false;
  } catch (const tested::regex_error &) {
  }

  try {
    // \0 cannot be followed by another decimal digit.
    tested::regex bad("\\01");
    return false;
  } catch (const tested::regex_error &) {
  }

  return true;
}

static bool ecma_empty_repeat_semantics() {
  tested::cmatch m;

  // Star is already allowed to stop at zero iterations, therefore an
  // empty extra iteration must not be taken.
  if (!tested::regex_match("", m, tested::regex("(a?)*")))
    return false;

  if (m.size() != 2 || m[1].matched)
    return false;

  // Plus must perform its mandatory first iteration. That iteration is
  // allowed to be empty, so capture #1 DOES participate.
  if (!tested::regex_match("", m, tested::regex("(a?)+")))
    return false;

  if (!m[1].matched || m[1].str() != "")
    return false;

  return true;
}

static bool ecma_choice_priority() {
  tested::cmatch m;

  tested::regex r("(aa|aabaac|ba|b|c)*");

  if (!tested::regex_search("aabaac", m, r))
    return false;

  // ECMAScript choice-point ordering chooses aa, then ba, and accepts
  // the successful sequel before reconsidering the earlier choice.
  return m[0].str() == "aaba" && m[1].str() == "ba";
}

static bool ecma_lookahead_backtracking() {
  tested::cmatch m;

  tested::regex r("(?=(a+))a*b\\1");

  if (!tested::regex_search("baaabac", m, r))
    return false;

  // Positive lookahead commits to its first successful internal match.
  // The outer matcher may fail and move to another start position, but
  // may not backtrack into that lookahead capture.
  return m[0].str() == "aba" && m[1].str() == "a";
}

static bool ecma_nested_lookahead_captures() {
  tested::cmatch m;

  tested::regex r("^(x)(?=(y)(?=z(.)))(yzq)$");

  if (!tested::regex_match("xyzq", m, r))
    return false;

  if (m.size() != 5)
    return false;

  return m[1].str() == "x" && m[2].str() == "y" && m[3].str() == "q" &&
         m[4].str() == "yzq";
}

static bool ecma_negative_assertion_capture() {
  tested::cmatch m;

  tested::regex r("^(?!((x)))\\1a$");

  if (!tested::regex_match("a", m, r))
    return false;

  if (m.size() != 3)
    return false;

  // Successful negative lookahead means its internal captures did not
  // participate. \1 therefore succeeds as an empty backreference.
  return !m[1].matched && !m[2].matched;
}

static bool ecma_nosubs_keeps_internal_captures() {
  tested::regex r("(a)\\1", tested::regex_constants::ECMAScript |
                                tested::regex_constants::nosubs);

  if (r.mark_count() != 0)
    return false;

  tested::cmatch m;

  if (!tested::regex_match("aa", m, r))
    return false;

  return m.size() == 1 && m[0].str() == "aa";
}

static bool ecma_invalid_syntax() {
  try {
    tested::regex r("(?=a)*");
    return false;
  } catch (const tested::regex_error &) {
  }

  try {
    tested::regex r("^+");
    return false;
  } catch (const tested::regex_error &) {
  }

  try {
    tested::regex r("\\c1");
    return false;
  } catch (const tested::regex_error &) {
  }

  try {
    // Definitely outside the representable range of char,
    // regardless of whether plain char is signed.
    tested::regex r("\\u0100");
    return false;
  } catch (const tested::regex_error &) {
  }

  // wchar_t can represent U+0100 on every FTL target.
  try {
    tested::wregex r(L"\\u0100");
    wchar_t input[] = {static_cast<wchar_t>(0x100), 0};

    if (!tested::regex_match(input, r))
      return false;
  } catch (...) {
    return false;
  }

  return true;
}

static bool regex_deduction_guide() {
  tested::vector<char> p{'a', '+'};

  tested::basic_regex r(p.begin(), p.end());

  static_assert(tested::is_same_v<decltype(r), tested::regex>);

  return tested::regex_match("aaa", r);
}

static bool posix_basic_grammar() {
  using namespace tested::regex_constants;

  tested::cmatch m;

  tested::regex grouped("\\(ab\\)\\{2,3\\}", basic);

  if (!tested::regex_match("abab", m, grouped))
    return false;

  if (m.size() != 2 || m[1].str() != "ab")
    return false;

  if (!tested::regex_match("ababab", grouped))
    return false;

  if (tested::regex_match("ab", grouped))
    return false;

  // +, (), {} are ordinary in BRE unless given their BRE syntax.
  if (!tested::regex_match("(ab)+", tested::regex("(ab)+", basic)))
    return false;

  tested::regex backref("\\(ab\\)\\1", basic);

  if (!tested::regex_match("abab", m, backref))
    return false;

  if (m[1].str() != "ab")
    return false;

  tested::regex anchored("^ab$", basic);

  return tested::regex_match("ab", anchored) &&
         !tested::regex_search("zab", anchored);
}

static bool posix_extended_grammar() {
  using namespace tested::regex_constants;

  tested::cmatch m;

  tested::regex r("(ab|cd)+", extended);

  if (!tested::regex_match("abcdcd", m, r))
    return false;

  if (m.size() != 2)
    return false;

  if (m[1].str() != "cd")
    return false;

  tested::regex interval("a{2,4}", extended);

  if (!tested::regex_match("aaa", interval))
    return false;

  if (tested::regex_match("aaaaa", interval))
    return false;

  // Escaping an ERE metacharacter makes it literal.
  if (!tested::regex_match("a+b", tested::regex("a\\+b", extended)))
    return false;

  // POSIX ERE has no BACKREF production.
  try {
    tested::regex bad("(ab)\\1", extended);
    return false;
  } catch (const tested::regex_error &) {
  }

  return true;
}

static bool posix_grep_grammar() {
  using namespace tested::regex_constants;

  tested::cmatch m;

  tested::regex r("cat\ndog", grep);

  if (!tested::regex_search("xxdogyy", m, r))
    return false;

  if (m.str() != "dog")
    return false;

  // Each newline-separated branch is a BRE.
  tested::regex bre("\\(ab\\)\\1\nx\\{2\\}", grep);

  if (!tested::regex_match("abab", bre))
    return false;

  if (!tested::regex_match("xx", bre))
    return false;

  // Empty lines become empty alternatives.
  tested::regex empty("\nfoo", grep);

  if (!tested::regex_search("bar", m, empty))
    return false;

  return m.str().empty();
}

static bool posix_egrep_grammar() {
  using namespace tested::regex_constants;

  tested::cmatch m;

  tested::regex r("cat+\ndog+", egrep);

  if (!tested::regex_match("cattt", r))
    return false;

  if (!tested::regex_match("doggg", r))
    return false;

  if (tested::regex_match("bird", r))
    return false;

  tested::regex alt("(ab|cd)+\nx{2,3}", egrep);

  if (!tested::regex_match("abcd", alt))
    return false;

  if (!tested::regex_match("xxx", alt))
    return false;

  return true;
}

static bool posix_awk_grammar() {
  using namespace tested::regex_constants;

  // \141 == 'a', \142 == 'b'
  tested::regex octal("\\141\\142+", awk);

  if (!tested::regex_match("abbb", octal))
    return false;

  tested::regex newline("\\n", awk);

  if (!tested::regex_match("\n", newline))
    return false;

  tested::regex control("\\t+", awk);

  if (!tested::regex_match("\t\t", control))
    return false;

  // AWK escapes are recognized inside bracket expressions too.
  tested::regex range("[\\141-\\143]+", awk);

  if (!tested::regex_match("abcabc", range))
    return false;

  if (tested::regex_match("d", range))
    return false;

  return true;
}

static bool posix_leftmost_longest() {
  using namespace tested::regex_constants;

  tested::cmatch m;

  // ECMAScript commits to first alternative.
  tested::regex ecma("a|aa", ECMAScript);

  if (!tested::regex_search("zaa", m, ecma))
    return false;

  if (m.position() != 1 || m.str() != "a")
    return false;

  // POSIX searches all paths at the same leftmost start and chooses
  // the longest one.
  tested::regex posix("a|aa", extended);

  if (!tested::regex_search("zaa", m, posix))
    return false;

  if (m.position() != 1 || m.str() != "aa")
    return false;

  tested::regex captured("(a|aa)(a?)", extended);

  if (!tested::regex_search("zaaa", m, captured))
    return false;

  if (m.str() != "aaa")
    return false;

  if (m[1].str() != "aa")
    return false;

  if (m[2].str() != "a")
    return false;

  return true;
}

static bool posix_dot_semantics() {
  using namespace tested::regex_constants;

  // ECMAScript dot excludes line terminators.
  if (tested::regex_match("\n", tested::regex(".", ECMAScript)))
    return false;

  // POSIX BRE/ERE dot is not the ECMAScript dot.
  if (!tested::regex_match("\n", tested::regex(".", basic)))
    return false;

  if (!tested::regex_match("\n", tested::regex(".", extended)))
    return false;

  if (!tested::regex_match("\n", tested::regex(".", awk)))
    return false;

  return true;
}

static bool posix_multiline_is_ecma_only() {
  using namespace tested::regex_constants;

  tested::cmatch m;

  tested::regex ecma("^b", ECMAScript | multiline);

  if (!tested::regex_search("a\nb", m, ecma))
    return false;

  tested::regex posix("^b", extended | multiline);

  // multiline has no POSIX grammar effect.
  if (tested::regex_search("a\nb", m, posix))
    return false;

  return true;
}

static bool posix_bracket_escape_rules() {
  using namespace tested::regex_constants;

  // In ordinary POSIX ERE bracket expressions, '\' does not introduce
  // ECMAScript \d / \w / \s character-class escapes.
  tested::regex e("[\\d]", extended);

  if (tested::regex_match("5", e))
    return false;

  if (!tested::regex_match("d", e))
    return false;

  if (!tested::regex_match("\\", e))
    return false;

  tested::regex b("[\\d]", basic);

  if (tested::regex_match("5", b))
    return false;

  if (!tested::regex_match("d", b))
    return false;

  return tested::regex_match("\\", b);
}

static bool posix_syntax_errors() {
  using namespace tested::regex_constants;

  try {
    tested::regex r("\\(abc", basic);
    return false;
  } catch (const tested::regex_error &) {
  }

  try {
    tested::regex r("(abc", extended);
    return false;
  } catch (const tested::regex_error &) {
  }

  try {
    tested::regex r("a\\{3,2\\}", basic);
    return false;
  } catch (const tested::regex_error &) {
  }

  try {
    tested::regex r("a{3,2}", extended);
    return false;
  } catch (const tested::regex_error &) {
  }

  try {
    tested::regex r("a|", extended);
    return false;
  } catch (const tested::regex_error &) {
  }

  return true;
}

static bool regex_traits_transform_surface() {
  tested::regex_traits<char> tr;

  tested::string s = "abc";

  const auto &facet = tested::use_facet<tested::collate<char>>(tr.getloc());

  auto expected = facet.transform(s.data(), s.data() + s.size());

  if (tr.transform(s.begin(), s.end()) != expected)
    return false;

  tested::string empty;

  auto expected_empty =
      facet.transform(empty.data(), empty.data() + empty.size());

  if (tr.transform(empty.begin(), empty.end()) != expected_empty)
    return false;

  //
  // The classic facet does not expose primary-weight decomposition.
  //
  tr.imbue(tested::locale::classic());

  tested::string one = "a";

  return tr.transform_primary(one.begin(), one.end()).empty();
}

static bool regex_traits_classname_case() {
  tested::regex_traits<char> tr;

  tested::string upper = "ALPHA";

  auto alpha = tr.lookup_classname(upper.begin(), upper.end());

  if (!alpha)
    return false;

  if (!tr.isctype('A', alpha) || !tr.isctype('z', alpha))
    return false;

  tested::string lower = "LoWeR";

  auto folded = tr.lookup_classname(lower.begin(), lower.end(), true);

  if (!folded)
    return false;

  return tr.isctype('A', folded) && tr.isctype('z', folded) &&
         !tr.isctype('7', folded);
}

static bool collating_element_works() {
  tested::cmatch m;

  //
  // FTL's default traits already recognizes the symbolic collating
  // name "newline".
  //
  tested::regex newline("[[.newline.]]");

  if (!tested::regex_match("\n", m, newline))
    return false;

  try {
    tested::regex bad("[[.definitely-not-a-collating-element.]]");

    return false;
  } catch (const tested::regex_error &e) {
    if (e.code() != tested::regex_constants::error_collate)
      return false;
  }

  return true;
}

static bool custom_collating_digraph() {
  regex4_regex r("[[.ch.]]");

  if (!tested::regex_match("ch", r))
    return false;

  if (tested::regex_match("c", r))
    return false;

  if (tested::regex_match("h", r))
    return false;

  regex4_regex neg("[^[.ch.]]");

  if (tested::regex_match("ch", neg))
    return false;

  if (!tested::regex_match("x", neg))
    return false;

  return true;
}

static bool equivalence_class_works() {
  //
  // Default regex_traits cannot manufacture a primary sort key, so
  // N4950 requires the equivalence expression to be rejected.
  //
  try {
    tested::regex bad("[[=a=]]");
    return false;
  } catch (const tested::regex_error &e) {
    if (e.code() != tested::regex_constants::error_collate)
      return false;
  }

  //
  // A custom Traits implementation that supplies primary keys makes
  // the exact same grammar usable.
  //
  regex4_regex r("[[=a=]]");

  if (!tested::regex_match("a", r))
    return false;

  if (!tested::regex_match("A", r))
    return false;

  if (tested::regex_match("b", r))
    return false;

  return true;
}

static bool collated_range_works() {
  using namespace tested::regex_constants;

  //
  // Raw code-point order says c-a is backwards.
  //
  try {
    regex4_regex bad("[c-a]");
    return false;
  } catch (const tested::regex_error &e) {
    if (e.code() != error_range)
      return false;
  }

  //
  // Our custom transform defines c < b < a.
  //
  regex4_regex r("[c-a]", ECMAScript | collate);

  if (!tested::regex_match("c", r))
    return false;

  if (!tested::regex_match("b", r))
    return false;

  if (!tested::regex_match("a", r))
    return false;

  if (tested::regex_match("d", r))
    return false;

  return true;
}

static bool collated_range_icase() {
  using namespace tested::regex_constants;

  regex4_regex r("[c-a]", ECMAScript | collate | icase);

  return tested::regex_match("C", r) && tested::regex_match("B", r) &&
         tested::regex_match("A", r) && !tested::regex_match("D", r);
}

bool ftl_test() {
  return basic_match() && search_works() && alternation_quantifiers() &&
         classes_and_escapes() && anchors_and_boundaries() && icase_works() &&
         backref_works() && lookahead_works() && nosubs_works() &&
         replace_works() && iterator_works() && token_iterator_works() &&
         lazy_works() && zero_length_iterator_doesnt_loop() && errors_work() &&
         wide_works() &&

         synopsis_surface() && submatch_surface() && match_results_surface() &&
         imbue_invalidates() && prev_avail_works() &&
         iterator_prefix_after_empty() && token_array_constructor() &&
         cstring_replace_surface() &&

         ecma_capture_clearing() && ecma_undefined_backrefs() &&
         ecma_decimal_backrefs() && ecma_empty_repeat_semantics() &&
         ecma_choice_priority() && ecma_lookahead_backtracking() &&
         ecma_nested_lookahead_captures() &&
         ecma_negative_assertion_capture() &&
         ecma_nosubs_keeps_internal_captures() && ecma_invalid_syntax() &&
         regex_deduction_guide() &&

         posix_basic_grammar() && posix_extended_grammar() &&
         posix_grep_grammar() && posix_egrep_grammar() && posix_awk_grammar() &&
         posix_leftmost_longest() && posix_dot_semantics() &&
         posix_multiline_is_ecma_only() && posix_bracket_escape_rules() &&
         posix_syntax_errors() &&

         regex_traits_transform_surface() && regex_traits_classname_case() &&
         collating_element_works() && custom_collating_digraph() &&
         equivalence_class_works() && collated_range_works() &&
         collated_range_icase();
}
