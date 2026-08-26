# FTL support matrix

| Dimension         | Supported                                                                 |
|-------------------|---------------------------------------------------------------------------|
| Language          | C++23                                                                     |
| Compilers         | MSVC 19.51+, Clang/Clang-CL 20+, GCC 13+, AppleClang 21+                  |
| Operating systems | Windows, Linux, macOS                                                     |
| Architectures     | x86-64                                                                    |
| Usage model       | Whole-program `std` implementation selected through the FTL include path  |
| Exceptions        | Enabled or disabled with the compiler's native option                     |
| RTTI              | Enabled or disabled with the compiler's native option                     |
| Threads           | Enabled, or disabled by defining `FTL_NO_THREADS`                         |
| CI combinations   | MSVC and Clang-CL on Windows; GCC and Clang on Linux; AppleClang on macOS |
| ABI               | All-or-nothing source selection; no vendor-STL ABI compatibility          |
