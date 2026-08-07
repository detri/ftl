#include <cstring>

namespace {

volatile unsigned runtime_seed = 7;

} // namespace

extern "C" int ftl_entry() {
  unsigned char source[64]{};
  unsigned char destination[64]{};

  const unsigned seed = runtime_seed;
  const std::size_t count =
      static_cast<std::size_t>((seed & 15u) + 16u);

  for (std::size_t index = 0; index < count; ++index) {
    source[index] =
        static_cast<unsigned char>(seed + index + 1u);
  }

  // memcpy must not acquire a CRT dependency after optimization.
  std::memcpy(destination, source, count);

  if (std::memcmp(destination, source, count) != 0) {
    return 1;
  }

  // memmove must remain freestanding for both copy directions.
  std::memmove(destination + 1, destination, count);

  if (std::memcmp(destination + 1, source, count) != 0) {
    return 2;
  }

  std::memmove(destination, destination + 1, count);

  if (std::memcmp(destination, source, count) != 0) {
    return 3;
  }

  // memset is another canonical loop-to-libcall transform.
  std::memset(destination, static_cast<int>(seed), count);

  for (std::size_t index = 0; index < count; ++index) {
    if (destination[index] !=
        static_cast<unsigned char>(seed)) {
      return 4;
        }
  }

  // strlen is recognized as a loop idiom by modern optimizers too.
  char string[64]{};

  const std::size_t length =
      static_cast<std::size_t>((seed & 15u) + 1u);

  for (std::size_t index = 0; index < length; ++index) {
    string[index] =
        static_cast<char>('a' + ((seed + index) % 26u));
  }

  string[length] = '\0';

  if (std::strlen(string) != length) {
    return 5;
  }

  return 0;
}