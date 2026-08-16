#ifdef FTL_REPLACE_STL
#include <cinttypes>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/cinttypes>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

using narrow_signed_conversion = tested::intmax_t (*)(const char *, char **,
                                                      int);

using narrow_unsigned_conversion = tested::uintmax_t (*)(const char *, char **,
                                                         int);

using wide_signed_conversion = tested::intmax_t (*)(const wchar_t *, wchar_t **,
                                                    int);

using wide_unsigned_conversion = tested::uintmax_t (*)(const wchar_t *,
                                                       wchar_t **, int);

static_assert(
    tested::is_same_v<decltype(&tested::strtoimax), narrow_signed_conversion>);

static_assert(tested::is_same_v<decltype(&tested::strtoumax),
                                narrow_unsigned_conversion>);

static_assert(
    tested::is_same_v<decltype(&tested::wcstoimax), wide_signed_conversion>);

static_assert(
    tested::is_same_v<decltype(&tested::wcstoumax), wide_unsigned_conversion>);

static_assert(
    tested::is_same_v<decltype(tested::imaxdiv_t{}.quot), tested::intmax_t>);

static_assert(
    tested::is_same_v<decltype(tested::imaxdiv_t{}.rem), tested::intmax_t>);

static_assert(tested::imaxabs(tested::intmax_t{-7}) == tested::intmax_t{7});

constexpr auto positive_division =
    tested::imaxdiv(tested::intmax_t{17}, tested::intmax_t{5});

static_assert(positive_division.quot == 3);
static_assert(positive_division.rem == 2);

constexpr auto negative_division =
    tested::imaxdiv(tested::intmax_t{-17}, tested::intmax_t{5});

static_assert(negative_division.quot == -3);
static_assert(negative_division.rem == -2);

template <decltype(sizeof(0)) N>
constexpr bool all_formats_present(const char *const (&formats)[N]) {
  for (const char *format : formats) {
    if (format == nullptr || *format == '\0')
      return false;
  }

  return true;
}

constexpr const char *pri_formats[] = {
    PRId8,       PRIi8,       PRIo8,       PRIu8,       PRIx8,
    PRIX8,       PRId16,      PRIi16,      PRIo16,      PRIu16,
    PRIx16,      PRIX16,      PRId32,      PRIi32,      PRIo32,
    PRIu32,      PRIx32,      PRIX32,      PRId64,      PRIi64,
    PRIo64,      PRIu64,      PRIx64,      PRIX64,

    PRIdLEAST8,  PRIiLEAST8,  PRIoLEAST8,  PRIuLEAST8,  PRIxLEAST8,
    PRIXLEAST8,  PRIdLEAST16, PRIiLEAST16, PRIoLEAST16, PRIuLEAST16,
    PRIxLEAST16, PRIXLEAST16, PRIdLEAST32, PRIiLEAST32, PRIoLEAST32,
    PRIuLEAST32, PRIxLEAST32, PRIXLEAST32, PRIdLEAST64, PRIiLEAST64,
    PRIoLEAST64, PRIuLEAST64, PRIxLEAST64, PRIXLEAST64,

    PRIdFAST8,   PRIiFAST8,   PRIoFAST8,   PRIuFAST8,   PRIxFAST8,
    PRIXFAST8,   PRIdFAST16,  PRIiFAST16,  PRIoFAST16,  PRIuFAST16,
    PRIxFAST16,  PRIXFAST16,  PRIdFAST32,  PRIiFAST32,  PRIoFAST32,
    PRIuFAST32,  PRIxFAST32,  PRIXFAST32,  PRIdFAST64,  PRIiFAST64,
    PRIoFAST64,  PRIuFAST64,  PRIxFAST64,  PRIXFAST64,

    PRIdMAX,     PRIiMAX,     PRIoMAX,     PRIuMAX,     PRIxMAX,
    PRIXMAX,     PRIdPTR,     PRIiPTR,     PRIoPTR,     PRIuPTR,
    PRIxPTR,     PRIXPTR,
};

constexpr const char *scn_formats[] = {
    SCNd8,       SCNi8,       SCNo8,       SCNu8,       SCNx8,
    SCNd16,      SCNi16,      SCNo16,      SCNu16,      SCNx16,
    SCNd32,      SCNi32,      SCNo32,      SCNu32,      SCNx32,
    SCNd64,      SCNi64,      SCNo64,      SCNu64,      SCNx64,

    SCNdLEAST8,  SCNiLEAST8,  SCNoLEAST8,  SCNuLEAST8,  SCNxLEAST8,
    SCNdLEAST16, SCNiLEAST16, SCNoLEAST16, SCNuLEAST16, SCNxLEAST16,
    SCNdLEAST32, SCNiLEAST32, SCNoLEAST32, SCNuLEAST32, SCNxLEAST32,
    SCNdLEAST64, SCNiLEAST64, SCNoLEAST64, SCNuLEAST64, SCNxLEAST64,

    SCNdFAST8,   SCNiFAST8,   SCNoFAST8,   SCNuFAST8,   SCNxFAST8,
    SCNdFAST16,  SCNiFAST16,  SCNoFAST16,  SCNuFAST16,  SCNxFAST16,
    SCNdFAST32,  SCNiFAST32,  SCNoFAST32,  SCNuFAST32,  SCNxFAST32,
    SCNdFAST64,  SCNiFAST64,  SCNoFAST64,  SCNuFAST64,  SCNxFAST64,

    SCNdMAX,     SCNiMAX,     SCNoMAX,     SCNuMAX,     SCNxMAX,
    SCNdPTR,     SCNiPTR,     SCNoPTR,     SCNuPTR,     SCNxPTR,
};

static_assert(all_formats_present(pri_formats));
static_assert(all_formats_present(scn_formats));

bool ftl_test() {
  {
    const char input[] = "-123456789012345";
    char *end = nullptr;

    const auto result = tested::strtoimax(input, &end, 10);

    if (result != tested::intmax_t{-123456789012345LL})
      return false;

    if (end == nullptr || *end != '\0')
      return false;
  }

  {
    const char input[] = "fedcba";
    char *end = nullptr;

    const auto result = tested::strtoumax(input, &end, 16);

    if (result != tested::uintmax_t{0xfedcba})
      return false;

    if (end == nullptr || *end != '\0')
      return false;
  }

  {
    const wchar_t input[] = L"-7654321";
    wchar_t *end = nullptr;

    const auto result = tested::wcstoimax(input, &end, 10);

    if (result != tested::intmax_t{-7654321})
      return false;

    if (end == nullptr || *end != L'\0')
      return false;
  }

  {
    const wchar_t input[] = L"7654321";
    wchar_t *end = nullptr;

    const auto result = tested::wcstoumax(input, &end, 10);

    if (result != tested::uintmax_t{7654321})
      return false;

    if (end == nullptr || *end != L'\0')
      return false;
  }

  return true;
}
