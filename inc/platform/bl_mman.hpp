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
#ifndef _BL_PLAT_MMAN_FILE_
#define _BL_PLAT_MMAN_FILE_
#include <bl_util.hpp>
#include <cstddef>
#include <cstdint>
#ifdef BL_PLATFORM_UNIX
#include <fcntl.h>
#include <sys/mman.h>
#elif defined(BL_PLATFORM_WINDOWS)
#include <windows.h>
#endif // __unix__
namespace blt::sys {
struct _Flag_t {
  enum Flags1 : uint8_t {
    All = 0x7,
    Exec = 0x1,
    Read = 0x2,
    Write = 0x4,
    None = 0x0
  };
  enum Flags2 : uint8_t { Shared = 0x1, Private = 0x2, Anonymous = 0x4 };
  enum Flags3 : uint8_t { Async, Sync };
};
using ProtFlagBits = _Flag_t::Flags1;
using MapFlagBits = _Flag_t::Flags2;
using SyncFlag = _Flag_t::Flags3;
struct MappedMemory {
  std::byte *m_Data;
  size_t m_Length;
#ifdef BL_PLATFORM_UNIX
  int m_fd, m_sync;
#elif defined(BL_PLATFORM_WINDOWS)
  HANDLE m_FileDescriptor;
  HANDLE m_FileMappingObject;
#else
  size_t m_Offset;
  bool m_WriteBack;
  std::fstream m_File;
#endif // __unix__
};
// map the file at %fpath to virtual memory, ranged from %offset to
// %offset + %len.
// prot: the file can be executed(ProtFlagBits::Exec), read(ProtFlagBits::Read),
//   write(ProtFlagBits::Write), or not accessible(ProtFlagBits::None).
// flags: the memory is shared with other processes(MapFlagBits::Shared), or
//   copy on write(MapFlagBits::Private), it can't be MapFlagBits::Anonymous.
auto memory_map_file(const char *fpath, std::byte *start, size_t offset,
                     size_t len, ProtFlagBits prot,
                     MapFlagBits flags) -> MappedMemory;
auto memory_map_sync(std::byte *start, size_t len, SyncFlag flag) -> int;
void memory_unmap_file(MappedMemory &&mem);
auto acquire_file_size(const char *path) -> size_t;
} // namespace blt::sys
#endif // !_BL_PLAT_MMAN_FILE_
