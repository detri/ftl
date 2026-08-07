#ifdef FTL_REPLACE_STL
#include <cstring>
namespace tested = std;
#else
#include <ftl/cstring>
namespace tested = ftl;
#endif

bool ftl_test() {
  using size_type = tested::size_t;

  // N4950 overload surface.
  using const_memchr_type = const void *(*)(const void *, int, size_type);
  using mutable_memchr_type = void *(*)(void *, int, size_type);

  using const_strchr_type = const char *(*)(const char *, int);
  using mutable_strchr_type = char *(*)(char *, int);

  using const_strpbrk_type = const char *(*)(const char *, const char *);
  using mutable_strpbrk_type = char *(*)(char *, const char *);

  using const_strrchr_type = const char *(*)(const char *, int);
  using mutable_strrchr_type = char *(*)(char *, int);

  using const_strstr_type = const char *(*)(const char *, const char *);
  using mutable_strstr_type = char *(*)(char *, const char *);

  const_memchr_type const_memchr =
      static_cast<const_memchr_type>(&tested::memchr);
  mutable_memchr_type mutable_memchr =
      static_cast<mutable_memchr_type>(&tested::memchr);

  const_strchr_type const_strchr =
      static_cast<const_strchr_type>(&tested::strchr);
  mutable_strchr_type mutable_strchr =
      static_cast<mutable_strchr_type>(&tested::strchr);

  const_strpbrk_type const_strpbrk =
      static_cast<const_strpbrk_type>(&tested::strpbrk);
  mutable_strpbrk_type mutable_strpbrk =
      static_cast<mutable_strpbrk_type>(&tested::strpbrk);

  const_strrchr_type const_strrchr =
      static_cast<const_strrchr_type>(&tested::strrchr);
  mutable_strrchr_type mutable_strrchr =
      static_cast<mutable_strrchr_type>(&tested::strrchr);

  const_strstr_type const_strstr =
      static_cast<const_strstr_type>(&tested::strstr);
  mutable_strstr_type mutable_strstr =
      static_cast<mutable_strstr_type>(&tested::strstr);

  static_cast<void>(const_memchr);
  static_cast<void>(mutable_memchr);
  static_cast<void>(const_strchr);
  static_cast<void>(mutable_strchr);
  static_cast<void>(const_strpbrk);
  static_cast<void>(mutable_strpbrk);
  static_cast<void>(const_strrchr);
  static_cast<void>(mutable_strrchr);
  static_cast<void>(const_strstr);
  static_cast<void>(mutable_strstr);

  // NULL.
  const void *null_pointer = NULL;
  if (null_pointer != nullptr) {
    return false;
  }

  // memcpy.
  {
    char source[] = "ftl";
    char destination[4]{};

    if (tested::memcpy(destination, source, sizeof(source)) != destination) {
      return false;
    }

    if (tested::strcmp(destination, "ftl") != 0) {
      return false;
    }
  }

  // memmove: overlapping move toward higher addresses.
  {
    char value[] = "abcdef";

    if (tested::memmove(value + 2, value, 4) != value + 2) {
      return false;
    }

    if (tested::strcmp(value, "ababcd") != 0) {
      return false;
    }
  }

  // memmove: overlapping move toward lower addresses.
  {
    char value[] = "abcdef";

    tested::memmove(value, value + 2, 4);

    if (tested::strcmp(value, "cdefef") != 0) {
      return false;
    }
  }

  // memset.
  {
    unsigned char bytes[4]{};

    if (tested::memset(bytes, 0xA5, sizeof(bytes)) != bytes) {
      return false;
    }

    for (unsigned char byte : bytes) {
      if (byte != 0xA5) {
        return false;
      }
    }
  }

  // memcmp, including unsigned-byte ordering.
  {
    const unsigned char equal_left[] = {1, 2, 3};
    const unsigned char equal_right[] = {1, 2, 3};

    if (tested::memcmp(equal_left, equal_right, 3) != 0) {
      return false;
    }

    const unsigned char lower[] = {0x7F};
    const unsigned char higher[] = {0x80};

    if (tested::memcmp(lower, higher, 1) >= 0) {
      return false;
    }

    if (tested::memcmp(higher, lower, 1) <= 0) {
      return false;
    }
  }

  // memchr overloads.
  {
    unsigned char mutable_bytes[] = {1, 2, 3, 4};
    const unsigned char const_bytes[] = {1, 2, 3, 4};

    void *mutable_result = tested::memchr(mutable_bytes, 3, 4);
    const void *const_result = tested::memchr(const_bytes, 3, 4);

    if (mutable_result != mutable_bytes + 2) {
      return false;
    }

    if (const_result != const_bytes + 2) {
      return false;
    }

    if (tested::memchr(const_bytes, 9, 4) != nullptr) {
      return false;
    }
  }

  // strcpy.
  {
    char destination[8]{};

    if (tested::strcpy(destination, "ftl") != destination) {
      return false;
    }

    if (tested::strcmp(destination, "ftl") != 0) {
      return false;
    }
  }

  // strncpy: zero padding.
  {
    char destination[6] = {'x', 'x', 'x', 'x', 'x', 'x'};

    if (tested::strncpy(destination, "hi", 5) != destination) {
      return false;
    }

    if (destination[0] != 'h' || destination[1] != 'i' ||
        destination[2] != '\0' || destination[3] != '\0' ||
        destination[4] != '\0' || destination[5] != 'x') {
      return false;
    }
  }

  // strncpy: truncation does not append a terminator.
  {
    char destination[4] = {'x', 'x', 'x', 'x'};

    tested::strncpy(destination, "abcdef", 3);

    if (destination[0] != 'a' || destination[1] != 'b' ||
        destination[2] != 'c' || destination[3] != 'x') {
      return false;
    }
  }

  // strcat.
  {
    char value[16] = "ft";

    if (tested::strcat(value, "l") != value) {
      return false;
    }

    if (tested::strcmp(value, "ftl") != 0) {
      return false;
    }
  }

  // strncat.
  {
    char value[16] = "ab";

    if (tested::strncat(value, "cdef", 2) != value) {
      return false;
    }

    if (tested::strcmp(value, "abcd") != 0) {
      return false;
    }
  }

  // strcmp.
  {
    if (tested::strcmp("abc", "abc") != 0) {
      return false;
    }

    if (tested::strcmp("abc", "abd") >= 0) {
      return false;
    }

    if (tested::strcmp("abd", "abc") <= 0) {
      return false;
    }
  }

  // strncmp.
  {
    if (tested::strncmp("abcdef", "abcxyz", 3) != 0) {
      return false;
    }

    if (tested::strncmp("abcdef", "abcxyz", 4) >= 0) {
      return false;
    }

    if (tested::strncmp("abcxyz", "abcdef", 4) <= 0) {
      return false;
    }
  }

  // C-locale collation.
  {
    if (tested::strcoll("abc", "abd") >= 0) {
      return false;
    }

    if (tested::strcoll("same", "same") != 0) {
      return false;
    }
  }

  // C-locale transformation.
  {
    char transformed[8]{};

    if (tested::strxfrm(transformed, "abc", sizeof(transformed)) != 3) {
      return false;
    }

    if (tested::strcmp(transformed, "abc") != 0) {
      return false;
    }
  }

  // strchr overloads, including searching for the terminator.
  {
    char mutable_value[] = "banana";
    const char const_value[] = "banana";

    char *mutable_result = tested::strchr(mutable_value, 'n');
    const char *const_result = tested::strchr(const_value, 'n');

    if (mutable_result != mutable_value + 2) {
      return false;
    }

    if (const_result != const_value + 2) {
      return false;
    }

    if (tested::strchr(const_value, '\0') != const_value + 6) {
      return false;
    }

    if (tested::strchr(const_value, 'x') != nullptr) {
      return false;
    }
  }

  // strcspn.
  {
    if (tested::strcspn("abc123", "0123456789") != 3) {
      return false;
    }

    if (tested::strcspn("abcdef", "xyz") != 6) {
      return false;
    }
  }

  // strpbrk overloads.
  {
    char mutable_value[] = "hello";
    const char const_value[] = "hello";

    char *mutable_result = tested::strpbrk(mutable_value, "xyzol");
    const char *const_result = tested::strpbrk(const_value, "xyzol");

    if (mutable_result != mutable_value + 2) {
      return false;
    }

    if (const_result != const_value + 2) {
      return false;
    }

    if (tested::strpbrk(const_value, "xyz") != nullptr) {
      return false;
    }
  }

  // strrchr overloads.
  {
    char mutable_value[] = "banana";
    const char const_value[] = "banana";

    char *mutable_result = tested::strrchr(mutable_value, 'a');
    const char *const_result = tested::strrchr(const_value, 'a');

    if (mutable_result != mutable_value + 5) {
      return false;
    }

    if (const_result != const_value + 5) {
      return false;
    }

    if (tested::strrchr(const_value, '\0') != const_value + 6) {
      return false;
    }
  }

  // strspn.
  {
    if (tested::strspn("aaab", "a") != 3) {
      return false;
    }

    if (tested::strspn("abc123", "abc") != 3) {
      return false;
    }
  }

  // strstr overloads and empty needle.
  {
    char mutable_value[] = "banana";
    const char const_value[] = "banana";

    char *mutable_result = tested::strstr(mutable_value, "ana");
    const char *const_result = tested::strstr(const_value, "ana");

    if (mutable_result != mutable_value + 1) {
      return false;
    }

    if (const_result != const_value + 1) {
      return false;
    }

    if (tested::strstr(const_value, "") != const_value) {
      return false;
    }

    if (tested::strstr(const_value, "xyz") != nullptr) {
      return false;
    }
  }

  // strtok.
  {
    char value[] = "::alpha:beta::gamma";

    char *token = tested::strtok(value, ":");

    if (token == nullptr || tested::strcmp(token, "alpha") != 0) {
      return false;
    }

    token = tested::strtok(nullptr, ":");

    if (token == nullptr || tested::strcmp(token, "beta") != 0) {
      return false;
    }

    token = tested::strtok(nullptr, ":");

    if (token == nullptr || tested::strcmp(token, "gamma") != 0) {
      return false;
    }

    if (tested::strtok(nullptr, ":") != nullptr) {
      return false;
    }
  }

  // strerror has implementation-defined text, but must give us a string.
  {
    char *message = tested::strerror(0);

    if (message == nullptr || tested::strlen(message) == 0) {
      return false;
    }
  }

  // strlen.
  {
    if (tested::strlen("") != 0) {
      return false;
    }

    if (tested::strlen("ftl") != 3) {
      return false;
    }
  }

  return true;
}