#ifdef FTL_REPLACE_STL
#include <cstring>
namespace tested = std;
#else
#include <ftl/cstring>
namespace tested = ftl;
#endif

#ifndef NULL
#error "<cstring> must define NULL"
#endif

template <class T, class U> inline constexpr bool same_as_v = false;

template <class T> inline constexpr bool same_as_v<T, T> = true;

static_assert(same_as_v<tested::size_t, decltype(sizeof(0))>);

// Character sequence copying.
using memcpy_type = void *(*)(void *, const void *, tested::size_t);

using memmove_type = void *(*)(void *, const void *, tested::size_t);

using strcpy_type = char *(*)(char *, const char *);

using strncpy_type = char *(*)(char *, const char *, tested::size_t);

using strcat_type = char *(*)(char *, const char *);

using strncat_type = char *(*)(char *, const char *, tested::size_t);

// Comparisons.
using memcmp_type = int (*)(const void *, const void *, tested::size_t);

using strcmp_type = int (*)(const char *, const char *);

using strcoll_type = int (*)(const char *, const char *);

using strncmp_type = int (*)(const char *, const char *, tested::size_t);

using strxfrm_type = tested::size_t (*)(char *, const char *, tested::size_t);

// Search functions with the C++ overload sets.
using const_memchr_type = const void *(*)(const void *, int, tested::size_t);

using mutable_memchr_type = void *(*)(void *, int, tested::size_t);

using const_strchr_type = const char *(*)(const char *, int);

using mutable_strchr_type = char *(*)(char *, int);

using strcspn_type = tested::size_t (*)(const char *, const char *);

using const_strpbrk_type = const char *(*)(const char *, const char *);

using mutable_strpbrk_type = char *(*)(char *, const char *);

using const_strrchr_type = const char *(*)(const char *, int);

using mutable_strrchr_type = char *(*)(char *, int);

using strspn_type = tested::size_t (*)(const char *, const char *);

using const_strstr_type = const char *(*)(const char *, const char *);

using mutable_strstr_type = char *(*)(char *, const char *);

// Miscellaneous.
using strtok_type = char *(*)(char *, const char *);

using memset_type = void *(*)(void *, int, tested::size_t);

using strerror_type = char *(*)(int);

using strlen_type = tested::size_t (*)(const char *);

[[maybe_unused]]
constexpr memcpy_type memcpy_pointer = &tested::memcpy;

[[maybe_unused]]
constexpr memmove_type memmove_pointer = &tested::memmove;

[[maybe_unused]]
constexpr strcpy_type strcpy_pointer = &tested::strcpy;

[[maybe_unused]]
constexpr strncpy_type strncpy_pointer = &tested::strncpy;

[[maybe_unused]]
constexpr strcat_type strcat_pointer = &tested::strcat;

[[maybe_unused]]
constexpr strncat_type strncat_pointer = &tested::strncat;

[[maybe_unused]]
constexpr memcmp_type memcmp_pointer = &tested::memcmp;

[[maybe_unused]]
constexpr strcmp_type strcmp_pointer = &tested::strcmp;

[[maybe_unused]]
constexpr strcoll_type strcoll_pointer = &tested::strcoll;

[[maybe_unused]]
constexpr strncmp_type strncmp_pointer = &tested::strncmp;

[[maybe_unused]]
constexpr strxfrm_type strxfrm_pointer = &tested::strxfrm;

[[maybe_unused]]
constexpr const_memchr_type const_memchr_pointer =
    static_cast<const_memchr_type>(&tested::memchr);

[[maybe_unused]]
constexpr mutable_memchr_type mutable_memchr_pointer =
    static_cast<mutable_memchr_type>(&tested::memchr);

[[maybe_unused]]
constexpr const_strchr_type const_strchr_pointer =
    static_cast<const_strchr_type>(&tested::strchr);

[[maybe_unused]]
constexpr mutable_strchr_type mutable_strchr_pointer =
    static_cast<mutable_strchr_type>(&tested::strchr);

[[maybe_unused]]
constexpr strcspn_type strcspn_pointer = &tested::strcspn;

[[maybe_unused]]
constexpr const_strpbrk_type const_strpbrk_pointer =
    static_cast<const_strpbrk_type>(&tested::strpbrk);

[[maybe_unused]]
constexpr mutable_strpbrk_type mutable_strpbrk_pointer =
    static_cast<mutable_strpbrk_type>(&tested::strpbrk);

[[maybe_unused]]
constexpr const_strrchr_type const_strrchr_pointer =
    static_cast<const_strrchr_type>(&tested::strrchr);

[[maybe_unused]]
constexpr mutable_strrchr_type mutable_strrchr_pointer =
    static_cast<mutable_strrchr_type>(&tested::strrchr);

[[maybe_unused]]
constexpr strspn_type strspn_pointer = &tested::strspn;

[[maybe_unused]]
constexpr const_strstr_type const_strstr_pointer =
    static_cast<const_strstr_type>(&tested::strstr);

[[maybe_unused]]
constexpr mutable_strstr_type mutable_strstr_pointer =
    static_cast<mutable_strstr_type>(&tested::strstr);

[[maybe_unused]]
constexpr strtok_type strtok_pointer = &tested::strtok;

[[maybe_unused]]
constexpr memset_type memset_pointer = &tested::memset;

[[maybe_unused]]
constexpr strerror_type strerror_pointer = &tested::strerror;

[[maybe_unused]]
constexpr strlen_type strlen_pointer = &tested::strlen;

bool ftl_test() {
  void *null_pointer = NULL;
  return null_pointer == nullptr;
}
