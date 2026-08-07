#ifdef FTL_REPLACE_STL
#include <cuchar>
namespace tested = std;
#else
#include <ftl/cuchar>
namespace tested = ftl;
#endif

using mbrtoc8_type = tested::size_t (*)(char8_t *, const char *, tested::size_t,
                                        tested::mbstate_t *);

using c8rtomb_type = tested::size_t (*)(char *, char8_t, tested::mbstate_t *);

using mbrtoc16_type = tested::size_t (*)(char16_t *, const char *,
                                         tested::size_t, tested::mbstate_t *);

using c16rtomb_type = tested::size_t (*)(char *, char16_t, tested::mbstate_t *);

using mbrtoc32_type = tested::size_t (*)(char32_t *, const char *,
                                         tested::size_t, tested::mbstate_t *);

using c32rtomb_type = tested::size_t (*)(char *, char32_t, tested::mbstate_t *);

[[maybe_unused]]
constexpr mbrtoc8_type mbrtoc8_pointer = &tested::mbrtoc8;

[[maybe_unused]]
constexpr c8rtomb_type c8rtomb_pointer = &tested::c8rtomb;

[[maybe_unused]]
constexpr mbrtoc16_type mbrtoc16_pointer = &tested::mbrtoc16;

[[maybe_unused]]
constexpr c16rtomb_type c16rtomb_pointer = &tested::c16rtomb;

[[maybe_unused]]
constexpr mbrtoc32_type mbrtoc32_pointer = &tested::mbrtoc32;

[[maybe_unused]]
constexpr c32rtomb_type c32rtomb_pointer = &tested::c32rtomb;

static_assert(sizeof(tested::mbstate_t) != 0);

static_assert(sizeof(tested::size_t) == sizeof(decltype(sizeof(0))));

bool ftl_test() { return true; }
