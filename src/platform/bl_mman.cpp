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
#include <platform/bl_mman.hpp>
#include <bl_util.hpp>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace blt::sys {
#ifdef BL_PLATFORM_UNIX
const char *const s_TypeName = "System_unix";
#elif defined(BL_PLATFORM_WINDOWS)
const char *const s_TypeName = "System_windows";
#else
const char *const s_TypeName = "System_other"
#endif
namespace _mmap_impl {
#ifdef BL_PLATFORM_UNIX
int to_open_flag_int(ProtFlagBits f) {
  if (f & (ProtFlagBits::Write | ProtFlagBits::Read))
    return O_RDWR;
  else if (f & ProtFlagBits::Write)
    return O_WRONLY;
  else
    return O_RDONLY;
}
int to_prot_int(ProtFlagBits f) {
  int r = 0;
  if (f & ProtFlagBits::Exec)
    r |= PROT_EXEC;
  if (f & ProtFlagBits::Read)
    r |= PROT_READ;
  if (f & ProtFlagBits::Write)
    r |= PROT_WRITE;
  return r;
}
int to_map_int(MapFlagBits f) {
  int r = 0;
  if (f & MapFlagBits::Shared)
    r |= MAP_SHARED;
  if (f & MapFlagBits::Private)
    r |= MAP_PRIVATE;
  return r;
}
int to_sync_flag_int(SyncFlag f) {
  switch (f) {
  case SyncFlag::Sync:
    return MS_SYNC;
  case SyncFlag::Async:
    return MS_ASYNC;
  default:
    return MS_SYNC;
  }
}
#elif defined(BL_PLATFORM_WINDOWS)
DWORD to_file_access_dw(ProtFlagBits f) {
  if (f & ProtFlagBits::All)
    return GENERIC_ALL;
  DWORD r = 0;
  if (f & ProtFlagBits::Exec)
    r |= GENERIC_EXECUTE;
  if (f & ProtFlagBits::Read)
    r |= GENERIC_READ;
  if (f & ProtFlagBits::Write)
    r |= GENERIC_WRITE;
  return r;
}
DWORD to_protect_dw(MapFlagBits f, ProtFlagBits pf) {
  if (pf & ProtFlagBits::None)
    return PAGE_NOACCESS;
  else if (pf & ProtFlagBits::Exec) {
    if (f & MapFlagBits::Private)
      return PAGE_EXECUTE_WRITECOPY;
    else if (pf & ProtFlagBits::Write)
      return PAGE_EXECUTE_READWRITE;
    else
      return PAGE_EXECUTE_READ;
  } else {
    if (f & MapFlagBits::Private)
      return PAGE_WRITECOPY;
    else if (pf & ProtFlagBits::Write)
      return PAGE_READWRITE;
    else
      return PAGE_READONLY;
  }
}
DWORD to_map_access_dw(MapFlagBits f, ProtFlagBits pf) {
  DWORD r = 0;
  if (pf & ProtFlagBits::Exec)
    r |= FILE_MAP_EXECUTE;
  if (f & MapFlagBits::Private)
    r |= FILE_MAP_COPY;
  else if (pf & ProtFlagBits::Write)
    r |= FILE_MAP_WRITE;
  else
    r |= FILE_MAP_READ;
  return r;
}
void close_handle_win(HANDLE h) {
  if (!CloseHandle(h))
    print_error(s_TypeName, GetLastError());
}
#endif
} // namespace _mmap_impl
MappedMemory memory_map_file(const char *fpath, std::byte *start, size_t offset,
                             size_t len, ProtFlagBits prot, MapFlagBits flags) {
#ifdef BL_PLATFORM_UNIX
  MappedMemory mem;
  struct stat fstat;
  if (int result = stat(fpath, &fstat); result < 0) {
    print_error(s_TypeName, strerror(result));
    goto FAILED_BASE;
  }
  if (fstat.st_size == 0) {
    print_error(s_TypeName, "Empty file:", fpath);
    goto FAILED_BASE;
  }
  if ((mem.m_fd = open(fpath, _mmap_impl::to_open_flag_int(prot))) < 0) {
    print_error(s_TypeName, strerror(errno));
    goto FAILED_BASE;
  }
  if (len == 0u)
    len = fstat.st_size;
  if constexpr (sizeof(size_t) == 8)
    mem.m_Data =
        (std::byte *)mmap64(start, len, _mmap_impl::to_prot_int(prot),
                            _mmap_impl::to_map_int(flags), mem.m_fd, offset);
  else
    mem.m_Data =
        (std::byte *)mmap(start, len, _mmap_impl::to_prot_int(prot),
                          _mmap_impl::to_map_int(flags), mem.m_fd, offset);
  if (!mem.m_Data) {
    print_error(s_TypeName, strerror(errno));
    goto FAILED_UNIX_1;
  }
  mem.m_Length = len;
  mem.m_sync =
      ((prot & ProtFlagBits::Write) && (flags & MapFlagBits::Shared)) ? 1 : 0;
  return mem;
FAILED_UNIX_1:
  close(mem.m_fd);
FAILED_BASE:
  mem.m_Data = nullptr;
  mem.m_Length = 0u;
  mem.m_fd = -1;
  return mem;
#elif defined(BL_PLATFORM_WINDOWS)
  MappedMemory mem;
  SECURITY_ATTRIBUTES file_security_attributes{
      .nLength = 0u, .lpSecurityDescriptor = nullptr, .bInheritHandle = TRUE};
  mem.m_FileDescriptor = CreateFileA(
      fpath, _mmap_impl::to_file_access_dw(prot),
      (flags & MapFlagBits::Shared) ? (FILE_SHARE_READ | FILE_SHARE_WRITE)
                                    : FILE_SHARE_READ,
      &file_security_attributes, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (mem.m_FileDescriptor == INVALID_HANDLE_VALUE) {
    print_error(s_TypeName, GetLastError());
    goto FAILED_BASE;
  }
  LARGE_INTEGER size;
  if (!GetFileSizeEx(mem.m_FileDescriptor, &size) || size.QuadPart == 0) {
    if (size.QuadPart == 0)
      print_error(s_TypeName, "File", fpath, "is empty.");
    else
      print_error(s_TypeName, GetLastError());
    goto FAILED_WIN_1;
  }
  mem.m_Length = static_cast<size_t>(size.QuadPart);
  mem.m_FileMappingObject =
      CreateFileMappingA(mem.m_FileDescriptor, &file_security_attributes,
                         _mmap_impl::to_protect_dw(flags, prot),
                         DWORD(len >> 32), DWORD(len & 0xffffffff), nullptr);
  if (mem.m_FileMappingObject == INVALID_HANDLE_VALUE) {
    print_error(s_TypeName, GetLastError());
    goto FAILED_WIN_1;
  }
  mem.m_Data = reinterpret_cast<std::byte *>(MapViewOfFileEx(
      mem.m_FileMappingObject, _mmap_impl::to_map_access_dw(flags, prot),
      DWORD(offset >> 32), DWORD(offset & 0xffffffff), (SIZE_T)len,
      (void *)start));
  if (mem.m_Data == nullptr) {
    print_error(s_TypeName, GetLastError());
    goto FAILED_WIN_2;
  }
  return mem;
FAILED_WIN_2:
  _mmap_impl::close_handle_win(mem.m_FileMappingObject);
FAILED_WIN_1:
  _mmap_impl::close_handle_win(mem.m_FileDescriptor);
FAILED_BASE:
  mem.m_Data = nullptr;
  mem.m_Length = 0u;
  mem.m_FileDescriptor = INVALID_HANDLE_VALUE;
  mem.m_FileMappingObject = INVALID_HANDLE_VALUE;
  return mem;
#else
  MappedMemory mem;
  mem.m_WriteBack =
      (flags & MapFlagBits::Shared) && (prot & ProtFlagBits::Write);
  mem.m_File.open(fpath, std::ios::binary |
                             std::ios::app(mem.m_WriteBack
                                               ? std::ios::in | std::ios::out
                                               : std::ios::in));
  if (!mem.m_File.is_open()) {
    print_error(s_TypeName, "File not exist or permission denied.");
    goto FAILED_BASE;
  }
  size_t eof_pos = mem.m_File.tellg();
  if (len == 0)
    mem.m_Length = eof_pos;
  else if (offset + len > eof_pos) {
    if (offset > eof_pos) {
      print_error(s_TypeName, "Wrong offset value:");
      goto FALIED_OTH_1;
    } else
      mem.m_Length = eof_pos - offset;
  } else
    mem.m_Length = len;
  mem.m_Offset = offset;
  mem.m_Data = (std::byte *)malloc(mem.m_Length);
  if (!mem.m_Data) {
    print_error(s_TypeName, "Out of memory.");
    goto FALIED_OTH_1;
  }
  mem.m_File.seekg(mem.m_Offset, std::ios::beg);
  mem.m_File.read((char *)mem.m_Data, mem.m_Length);
  if (!mem.m_File.good()) {
    print_error(s_TypeName, "File read error.");
    goto FALIED_OTH_2;
  }
  return mem;
FALIED_OTH_2:
  free(mem.m_Data);
FALIED_OTH_1:
  mem.m_File.close();
FAILED_BASE:
  mem.m_WriteBack = false;
  mem.m_Offset = 0u;
  mem.m_Data = nullptr;
  mem.m_Length = 0u;
  return mem;
#endif
}
int memory_map_sync(std::byte *start, size_t len, SyncFlag flag) {
#ifdef BL_PLATFORM_UNIX
  if (int result =
          msync((void *)start, len, _mmap_impl::to_sync_flag_int(flag));
      result < 0) {
    print_error(s_TypeName, strerror(result));
    return -1;
  } else
    return 0;
#elif defined(BL_PLATFORM_WINDOWS)
  if (flag == SyncFlag::Sync)
    if (!FlushViewOfFile((void *)start, len)) {
      print_error(s_TypeName, GetLastError());
      return -1;
    }
  return 0;
#else
  // do nothing
  return 0;
#endif
}
void memory_unmap_file(MappedMemory &&mem) {
#ifdef BL_PLATFORM_UNIX
  if (!mem.m_Data)
    return;
  int result;
  if (mem.m_sync)
    if (result = msync((void *)mem.m_Data, mem.m_Length, MS_SYNC); result < 0)
      goto ERROR_RESULT;
  if (result = munmap((void *)mem.m_Data, mem.m_Length); result < 0)
    goto ERROR_RESULT;
  if (result = close(mem.m_fd); result < 0)
    goto ERROR_RESULT;
  return;
ERROR_RESULT:
  print_error(s_TypeName, strerror(result));
  return;
#elif defined(BL_PLATFORM_WINDOWS)
  if (mem.m_Data == nullptr)
    return;
  if (!UnmapViewOfFile(mem.m_Data))
    print_error(s_TypeName, GetLastError());
  if (mem.m_FileDescriptor != INVALID_HANDLE_VALUE)
    _mmap_impl::close_handle_win(mem.m_FileDescriptor);
  if (mem.m_FileMappingObject != INVALID_HANDLE_VALUE)
    _mmap_impl::close_handle_win(mem.m_FileMappingObject);
  return;
#else
  if (mem.m_Data != nullptr && mem.m_WriteBack) {
    mem.m_File.seekp(mem.m_Offset);
    mem.m_File.write((char *)mem.m_Data, mem.m_Length);
    if (!mem.m_File.good())
      print_warning(s_TypeName, "Write back failed or incomplete!");
    mem.m_File.close();
  }
  return;
#endif
}
size_t acquire_file_size(const char *path) {
#ifdef BL_PLATFORM_UNIX
  struct stat stat_buf;
  return stat(path, &stat_buf) ? (~0u) : stat_buf.st_size;
#elif defined(BL_PLATFORM_WINDOWS)
  HANDLE handle = CreateFileA(path, 0, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
  if (handle == INVALID_HANDLE_VALUE) {
    print_error(s_TypeName, GetLastError());
    return (~0u);
  }
  LARGE_INTEGER size;
  if (!GetFileSizeEx(handle, &size)) {
    print_error(s_TypeName, GetLastError());
    return (~0u);
  }
  _mmap_impl::close_handle_win(handle);
  return size.QuadPart;
#else
  std::ifstream file(path, std::ios::app | std::ios::binary | std::ios::in);
  size_t size_2 = file.tellg();
  file.close();
  return size_2;
#endif
}
} // namespace blt::sys
