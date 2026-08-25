using ftl_runtime_size_t = decltype(sizeof(0));

#if defined(_MSC_VER) && !defined(__clang__) &&                         \
    (defined(_M_IX86) || defined(_M_X64))
extern "C" void __cdecl __movsb(unsigned char *, const unsigned char *,
                                 ftl_runtime_size_t);
extern "C" void __cdecl __stosb(unsigned char *, unsigned char,
                                 ftl_runtime_size_t);
#pragma intrinsic(__movsb)
#pragma intrinsic(__stosb)
#define FTL_RUNTIME_MSVC_BYTE_INTRINSICS 1
#else
#define FTL_RUNTIME_MSVC_BYTE_INTRINSICS 0
#endif

#if defined(__clang__)
#define FTL_RUNTIME_NO_BUILTIN __attribute__((no_builtin))
#elif defined(__GNUC__)
#define FTL_RUNTIME_NO_BUILTIN \
    __attribute__((optimize("no-tree-loop-distribute-patterns")))
#else
#define FTL_RUNTIME_NO_BUILTIN
#endif

#if defined(__clang__) || defined(__GNUC__)
namespace {

struct __attribute__((packed, may_alias)) runtime_word {
  ftl_runtime_size_t value;
};

FTL_RUNTIME_NO_BUILTIN
void copy_forward(unsigned char* output, const unsigned char* input,
                  ftl_runtime_size_t count) {
  constexpr ftl_runtime_size_t word_size = sizeof(runtime_word);
  constexpr ftl_runtime_size_t block_size = 4 * word_size;

  while (count >= block_size) {
    auto* out = reinterpret_cast<runtime_word*>(output);
    auto* in = reinterpret_cast<const runtime_word*>(input);
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
    out[3] = in[3];
    output += block_size;
    input += block_size;
    count -= block_size;
  }
  while (count >= word_size) {
    *reinterpret_cast<runtime_word*>(output) =
        *reinterpret_cast<const runtime_word*>(input);
    output += word_size;
    input += word_size;
    count -= word_size;
  }
  while (count-- != 0)
    *output++ = *input++;
}

FTL_RUNTIME_NO_BUILTIN
void copy_backward(unsigned char* output, const unsigned char* input,
                   ftl_runtime_size_t count) {
  constexpr ftl_runtime_size_t word_size = sizeof(runtime_word);
  constexpr ftl_runtime_size_t block_size = 4 * word_size;
  output += count;
  input += count;

  while (count >= block_size) {
    output -= block_size;
    input -= block_size;
    auto* out = reinterpret_cast<runtime_word*>(output);
    auto* in = reinterpret_cast<const runtime_word*>(input);
    out[3] = in[3];
    out[2] = in[2];
    out[1] = in[1];
    out[0] = in[0];
    count -= block_size;
  }
  while (count >= word_size) {
    output -= word_size;
    input -= word_size;
    *reinterpret_cast<runtime_word*>(output) =
        *reinterpret_cast<const runtime_word*>(input);
    count -= word_size;
  }
  while (count-- != 0)
    *--output = *--input;
}

FTL_RUNTIME_NO_BUILTIN
void fill_bytes(unsigned char* output, unsigned char byte,
                ftl_runtime_size_t count) {
  constexpr ftl_runtime_size_t word_size = sizeof(runtime_word);
  constexpr ftl_runtime_size_t block_size = 4 * word_size;
  const ftl_runtime_size_t repeated =
      (~ftl_runtime_size_t{0} / 0xffu) * byte;

  while (count >= block_size) {
    auto* out = reinterpret_cast<runtime_word*>(output);
    out[0].value = repeated;
    out[1].value = repeated;
    out[2].value = repeated;
    out[3].value = repeated;
    output += block_size;
    count -= block_size;
  }
  while (count >= word_size) {
    reinterpret_cast<runtime_word*>(output)->value = repeated;
    output += word_size;
    count -= word_size;
  }
  while (count-- != 0)
    *output++ = byte;
}

} // namespace
#endif

extern "C" {

void ftl_runtime_link_anchor() {}

#if !defined(_MSC_VER)
[[noreturn]] void __cxa_pure_virtual() {
  __builtin_trap();
}
#endif

FTL_RUNTIME_NO_BUILTIN
void* memcpy(
    void* destination,
    const void* source,
    ftl_runtime_size_t count) {
#if FTL_RUNTIME_MSVC_BYTE_INTRINSICS
  ::__movsb(static_cast<unsigned char*>(destination),
            static_cast<const unsigned char*>(source), count);
#else
#if defined(__clang__) || defined(__GNUC__)
  copy_forward(static_cast<unsigned char*>(destination),
               static_cast<const unsigned char*>(source), count);
#else
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
#endif
#endif

  return destination;
}

FTL_RUNTIME_NO_BUILTIN
void* memmove(
    void* destination,
    const void* source,
    ftl_runtime_size_t count) {
  if (destination == source || count == 0)
    return destination;

#if defined(_MSC_VER) && !defined(__clang__)
  auto* output = static_cast<volatile unsigned char*>(destination);
  auto* input = static_cast<const volatile unsigned char*>(source);
#else
  auto* output = static_cast<unsigned char*>(destination);
  auto* input = static_cast<const unsigned char*>(source);
#endif

  const auto output_address =
      reinterpret_cast<ftl_runtime_size_t>(destination);
  const auto input_address =
      reinterpret_cast<ftl_runtime_size_t>(source);

  if (output_address < input_address ||
      output_address - input_address >= count) {
#if FTL_RUNTIME_MSVC_BYTE_INTRINSICS
    ::__movsb(static_cast<unsigned char*>(destination),
              static_cast<const unsigned char*>(source), count);
#else
#if defined(__clang__) || defined(__GNUC__)
    copy_forward(static_cast<unsigned char*>(destination),
                 static_cast<const unsigned char*>(source), count);
#else
    for (ftl_runtime_size_t index = 0; index < count; ++index)
      output[index] = input[index];
#endif
#endif
  } else {
#if defined(__clang__) || defined(__GNUC__)
    copy_backward(static_cast<unsigned char*>(destination),
                  static_cast<const unsigned char*>(source), count);
#else
    for (ftl_runtime_size_t index = count; index != 0; --index)
      output[index - 1] = input[index - 1];
#endif
  }

  return destination;
}

FTL_RUNTIME_NO_BUILTIN
void* memset(
    void* destination,
    int value,
    ftl_runtime_size_t count) {
#if FTL_RUNTIME_MSVC_BYTE_INTRINSICS
  ::__stosb(static_cast<unsigned char*>(destination),
            static_cast<unsigned char>(value), count);
#else
#if defined(__clang__) || defined(__GNUC__)
  fill_bytes(static_cast<unsigned char*>(destination),
             static_cast<unsigned char>(value), count);
#else
#if defined(_MSC_VER) && !defined(__clang__)
  auto* output = static_cast<volatile unsigned char*>(destination);
#else
  auto* output =
      static_cast<unsigned char*>(destination);
#endif

  const auto byte =
      static_cast<unsigned char>(value);

  for (ftl_runtime_size_t index = 0; index < count; ++index)
    output[index] = byte;
#endif
#endif

  return destination;
}

} // extern "C"

#undef FTL_RUNTIME_NO_BUILTIN
#undef FTL_RUNTIME_MSVC_BYTE_INTRINSICS
