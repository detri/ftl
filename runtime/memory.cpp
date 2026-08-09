using ftl_runtime_size_t = decltype(sizeof(0));

#if defined(__clang__)
#define FTL_RUNTIME_NO_BUILTIN __attribute__((no_builtin))
#elif defined(__GNUC__)
#define FTL_RUNTIME_NO_BUILTIN \
    __attribute__((optimize("no-tree-loop-distribute-patterns")))
#else
#define FTL_RUNTIME_NO_BUILTIN
#endif

extern "C" {

FTL_RUNTIME_NO_BUILTIN
void* memcpy(
    void* destination,
    const void* source,
    ftl_runtime_size_t count) {
#if defined(_MSC_VER) && !defined(__clang__)
  auto* output =
      static_cast<volatile unsigned char*>(
          destination);

  auto* input =
      static_cast<const volatile unsigned char*>(
          source);
#else
  auto* output =
      static_cast<unsigned char*>(
          destination);

  auto* input =
      static_cast<const unsigned char*>(
          source);
#endif

  for (ftl_runtime_size_t index = 0;
       index < count;
       ++index) {
    output[index] = input[index];
       }

  return destination;
}

FTL_RUNTIME_NO_BUILTIN
void* memmove(
    void* destination,
    const void* source,
    ftl_runtime_size_t count) {
  auto* output =
      static_cast<unsigned char*>(destination);

  auto* input =
      static_cast<const unsigned char*>(source);

  if (output == input || count == 0)
    return destination;

  if (output < input) {
    for (ftl_runtime_size_t index = 0; index < count; ++index)
      output[index] = input[index];
  } else {
    for (ftl_runtime_size_t index = count; index != 0; --index)
      output[index - 1] = input[index - 1];
  }

  return destination;
}

FTL_RUNTIME_NO_BUILTIN
void* memset(
    void* destination,
    int value,
    ftl_runtime_size_t count) {
  auto* output =
      static_cast<unsigned char*>(destination);

  const auto byte =
      static_cast<unsigned char>(value);

  for (ftl_runtime_size_t index = 0; index < count; ++index)
    output[index] = byte;

  return destination;
}

} // extern "C"

#undef FTL_RUNTIME_NO_BUILTIN