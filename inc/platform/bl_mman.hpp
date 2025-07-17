/******************************************************************************
MIT License

Copyright (c) 2025 bwyeag

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
#ifndef BL_PLAT_MMAN_FILE
#include <cstddef>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||                 \
    defined(__NT__) && !defined(__CYGWIN__)
#define IS_WINDOWS
#endif
#ifdef __unix__
#include <fcntl.h>
#include <sys/mman.h>
#elif defined(IS_WINDOWS)
#include <fileapi.h>
#include <windows.h>
#endif // __unix__
namespace BLT::sys {
struct _ProtFlagBits_t {
  enum Flags { All = 0x7, Exec = 0x1, Read = 0x2, Write = 0x4, None = 0 };
};
using ProtFlagBits = _ProtFlagBits_t::Flags;
struct _MapFlagBits_t {
  enum Flags { Shared = 0x1, Private = 0x2, Anonymous = 0x4 };
};
using MapFlagBits = _MapFlagBits_t::Flags;
struct MappedMemory {
  std::byte *m_Data;
  // size_t m_Length;
#ifdef __unix__
  // todo...
#elif defined(__WIN32__)
  HANDLE m_FileDescriptor;
  HANDLE m_FileMappingObject;
#endif // __unix__
};
MappedMemory memory_mapping(size_t offset, size_t len, const char *fpath,
                            ProtFlagBits prot, MapFlagBits flags);
void memory_unmapping(MappedMemory &&mem);
size_t acquire_file_size(const char* path);
} // namespace BLT::sys
#undef IS_WINDOWS
#endif // !BL_PLAT_MMAN_FILE
