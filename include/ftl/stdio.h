// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_STDIO_H_HEADER
#define FTL_STDIO_H_HEADER

#ifdef FTL_REPLACE_STL
#include <cstdio>
#define FTL_STDIO_NAMESPACE std
#else
#include <ftl/cstdio>
#define FTL_STDIO_NAMESPACE ftl
#endif

using FTL_STDIO_NAMESPACE::FILE;
using FTL_STDIO_NAMESPACE::fpos_t;
using FTL_STDIO_NAMESPACE::size_t;

using FTL_STDIO_NAMESPACE::remove;
using FTL_STDIO_NAMESPACE::rename;
using FTL_STDIO_NAMESPACE::tmpfile;
using FTL_STDIO_NAMESPACE::tmpnam;

using FTL_STDIO_NAMESPACE::fclose;
using FTL_STDIO_NAMESPACE::fflush;
using FTL_STDIO_NAMESPACE::fopen;
using FTL_STDIO_NAMESPACE::freopen;
using FTL_STDIO_NAMESPACE::setbuf;
using FTL_STDIO_NAMESPACE::setvbuf;

using FTL_STDIO_NAMESPACE::fprintf;
using FTL_STDIO_NAMESPACE::fscanf;
using FTL_STDIO_NAMESPACE::printf;
using FTL_STDIO_NAMESPACE::scanf;
using FTL_STDIO_NAMESPACE::snprintf;
using FTL_STDIO_NAMESPACE::sprintf;
using FTL_STDIO_NAMESPACE::sscanf;

using FTL_STDIO_NAMESPACE::vfprintf;
using FTL_STDIO_NAMESPACE::vfscanf;
using FTL_STDIO_NAMESPACE::vprintf;
using FTL_STDIO_NAMESPACE::vscanf;
using FTL_STDIO_NAMESPACE::vsnprintf;
using FTL_STDIO_NAMESPACE::vsprintf;
using FTL_STDIO_NAMESPACE::vsscanf;

using FTL_STDIO_NAMESPACE::fgetc;
using FTL_STDIO_NAMESPACE::fgets;
using FTL_STDIO_NAMESPACE::fputc;
using FTL_STDIO_NAMESPACE::fputs;
using FTL_STDIO_NAMESPACE::getc;
using FTL_STDIO_NAMESPACE::getchar;
using FTL_STDIO_NAMESPACE::putc;
using FTL_STDIO_NAMESPACE::putchar;
using FTL_STDIO_NAMESPACE::puts;
using FTL_STDIO_NAMESPACE::ungetc;

using FTL_STDIO_NAMESPACE::fread;
using FTL_STDIO_NAMESPACE::fwrite;

using FTL_STDIO_NAMESPACE::fgetpos;
using FTL_STDIO_NAMESPACE::fseek;
using FTL_STDIO_NAMESPACE::fsetpos;
using FTL_STDIO_NAMESPACE::ftell;
using FTL_STDIO_NAMESPACE::rewind;

using FTL_STDIO_NAMESPACE::clearerr;
using FTL_STDIO_NAMESPACE::feof;
using FTL_STDIO_NAMESPACE::ferror;
using FTL_STDIO_NAMESPACE::perror;

#undef FTL_STDIO_NAMESPACE

#endif // FTL_STDIO_H_HEADER
