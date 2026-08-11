#ifdef FTL_REPLACE_STL
#include <istream>
#include <locale>
#include <ostream>
#include <string>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/istream>
#include <ftl/locale>
#include <ftl/ostream>
#include <ftl/string>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

class input_buffer : public tested::streambuf {
public:
  input_buffer(char *first, char *last) { setg(first, first, last); }
};

class output_buffer : public tested::streambuf {
public:
  tested::string text;
protected:
  int_type overflow(int_type value) override {
    if (traits_type::eq_int_type(value, traits_type::eof()))
      return traits_type::not_eof(value);
    text.push_back(traits_type::to_char_type(value));
    return value;
  }
  tested::streamsize xsputn(const char *source,
                            tested::streamsize count) override {
    text.append(source, static_cast<tested::size_t>(count));
    return count;
  }
};

class grouped_numpunct : public tested::numpunct<char> {
protected:
  char do_thousands_sep() const override { return ','; }
  tested::string do_grouping() const override { return tested::string(1, 3); }
};

template <class Facet>
constexpr bool facet_has_standard_base =
    tested::is_base_of_v<tested::locale::facet, Facet>;

static_assert(facet_has_standard_base<tested::num_get<char>>);
static_assert(facet_has_standard_base<tested::num_put<char>>);
static_assert(facet_has_standard_base<tested::money_get<char>>);
static_assert(facet_has_standard_base<tested::money_put<char>>);
static_assert(facet_has_standard_base<tested::time_get<char>>);
static_assert(facet_has_standard_base<tested::time_put<char>>);
static_assert(tested::is_base_of_v<tested::time_get<char>,
                                   tested::time_get_byname<char>>);
static_assert(tested::is_base_of_v<tested::time_put<char>,
                                   tested::time_put_byname<char>>);
static_assert(tested::sentinel_for<tested::default_sentinel_t,
                                   tested::istreambuf_iterator<char>>);

bool mandatory_classic_facets_exist() {
  const tested::locale &value = tested::locale::classic();
#define FTL_EXPECT_FACET(...)                                                \
  if (!tested::has_facet<__VA_ARGS__>(value)) return false
  FTL_EXPECT_FACET(tested::ctype<char>);
  FTL_EXPECT_FACET(tested::ctype<wchar_t>);
  FTL_EXPECT_FACET(tested::codecvt<char, char, tested::mbstate_t>);
  FTL_EXPECT_FACET(tested::codecvt<wchar_t, char, tested::mbstate_t>);
  FTL_EXPECT_FACET(tested::codecvt<char16_t, char8_t, tested::mbstate_t>);
  FTL_EXPECT_FACET(tested::codecvt<char32_t, char8_t, tested::mbstate_t>);
  FTL_EXPECT_FACET(tested::codecvt<char16_t, char, tested::mbstate_t>);
  FTL_EXPECT_FACET(tested::codecvt<char32_t, char, tested::mbstate_t>);
  FTL_EXPECT_FACET(tested::num_get<char>);
  FTL_EXPECT_FACET(tested::num_get<wchar_t>);
  FTL_EXPECT_FACET(tested::num_put<char>);
  FTL_EXPECT_FACET(tested::num_put<wchar_t>);
  FTL_EXPECT_FACET(tested::numpunct<char>);
  FTL_EXPECT_FACET(tested::numpunct<wchar_t>);
  FTL_EXPECT_FACET(tested::collate<char>);
  FTL_EXPECT_FACET(tested::collate<wchar_t>);
  FTL_EXPECT_FACET(tested::time_get<char>);
  FTL_EXPECT_FACET(tested::time_get<wchar_t>);
  FTL_EXPECT_FACET(tested::time_put<char>);
  FTL_EXPECT_FACET(tested::time_put<wchar_t>);
  FTL_EXPECT_FACET(tested::money_get<char>);
  FTL_EXPECT_FACET(tested::money_get<wchar_t>);
  FTL_EXPECT_FACET(tested::money_put<char>);
  FTL_EXPECT_FACET(tested::money_put<wchar_t>);
  FTL_EXPECT_FACET(tested::moneypunct<char, false>);
  FTL_EXPECT_FACET(tested::moneypunct<char, true>);
  FTL_EXPECT_FACET(tested::moneypunct<wchar_t, false>);
  FTL_EXPECT_FACET(tested::moneypunct<wchar_t, true>);
  FTL_EXPECT_FACET(tested::messages<char>);
  FTL_EXPECT_FACET(tested::messages<wchar_t>);
#undef FTL_EXPECT_FACET
  return true;
}

bool numeric_facets_round_trip() {
  output_buffer output;
  tested::ostream stream(&output);
  auto iterator = tested::ostreambuf_iterator<char>(stream);
  iterator = tested::use_facet<tested::num_put<char>>(stream.getloc()).put(
      iterator, stream, ' ', 12345L);
  if (iterator.failed() || output.text != "12345")
    return false;

  char input[] = {'1', '2', '3', '4', '5'};
  input_buffer buffer(input, input + 5);
  tested::istream source(&buffer);
  long value = 0;
  tested::ios_base::iostate error = tested::ios_base::goodbit;
  auto result = tested::use_facet<tested::num_get<char>>(source.getloc()).get(
      tested::istreambuf_iterator<char>(source),
      tested::istreambuf_iterator<char>(), source, error, value);
  return result == tested::istreambuf_iterator<char>() && value == 12345 &&
         (error & tested::ios_base::failbit) == 0 &&
         (error & tested::ios_base::eofbit) != 0;
}

bool numeric_locale_integration_works() {
  tested::locale grouped(tested::locale::classic(), new grouped_numpunct);
  output_buffer output;
  tested::ostream destination(&output);
  destination.imbue(grouped);
  destination << 12345L;
  if (output.text != "12,345")
    return false;
  input_buffer input(output.text.data(), output.text.data() + output.text.size());
  tested::istream source(&input);
  source.imbue(grouped);
  long value = 0;
  source >> value;
  return value == 12345 && !source.fail();
}

bool monetary_and_time_facets_work() {
  output_buffer output;
  tested::ostream stream(&output);
  tested::use_facet<tested::money_put<char>>(stream.getloc()).put(
      tested::ostreambuf_iterator<char>(stream), false, stream, ' ', 1234.0L);
  if (output.text != "1234")
    return false;

  input_buffer money_input(output.text.data(),
                           output.text.data() + output.text.size());
  tested::istream money_stream(&money_input);
  tested::string digits;
  tested::ios_base::iostate money_error = tested::ios_base::goodbit;
  tested::use_facet<tested::money_get<char>>(money_stream.getloc()).get(
      tested::istreambuf_iterator<char>(money_stream),
      tested::istreambuf_iterator<char>(), false, money_stream, money_error,
      digits);
  if (digits != "1234" || (money_error & tested::ios_base::failbit) != 0)
    return false;

  output.text.clear();
  tested::tm value{};
  value.tm_year = 123;
  value.tm_mon = 6;
  value.tm_mday = 9;
  tested::use_facet<tested::time_put<char>>(stream.getloc()).put(
      tested::ostreambuf_iterator<char>(stream), stream, ' ', &value, 'Y');
  if (output.text != "2023")
    return false;
  input_buffer time_input(output.text.data(),
                          output.text.data() + output.text.size());
  tested::istream time_stream(&time_input);
  tested::tm parsed{};
  tested::ios_base::iostate time_error = tested::ios_base::goodbit;
  tested::use_facet<tested::time_get<char>>(time_stream.getloc()).get_year(
      tested::istreambuf_iterator<char>(time_stream),
      tested::istreambuf_iterator<char>(), time_stream, time_error, &parsed);
  return parsed.tm_year == 123 &&
         (time_error & tested::ios_base::failbit) == 0;
}

bool named_and_category_construction_works() {
  tested::locale named("C");
  tested::locale numeric(tested::locale::classic(), "C",
                         tested::locale::numeric);
  tested::locale combined(tested::locale::classic(), named,
                          tested::locale::all);
  return named.name() == "C" && numeric.name() != "*" &&
         combined.name() != "*" &&
         tested::has_facet<tested::num_get<char>>(combined) &&
         tested::has_facet<tested::money_put<wchar_t>>(combined);
}

bool ftl_test() {
  return mandatory_classic_facets_exist() && numeric_facets_round_trip() &&
         numeric_locale_integration_works() && monetary_and_time_facets_work() &&
         named_and_category_construction_works();
}
