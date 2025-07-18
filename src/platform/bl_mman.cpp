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
#include <bl_util.hpp>
#include <cstddef>
#include <memoryapi.h>
#include <platform/bl_mman.hpp>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||                 \
    defined(__NT__) && !defined(__CYGWIN__)
#define IS_WINDOWS
#endif
namespace BLT::sys {
const char *const s_TypeName = "System";
#ifdef __unix__
int to_open_flag_int(ProtFlagBits f) {
  if (f & (ProtFlagBits::Write | PrProtFlagBits::Read))
    return O_RDWK;
  else if (f & ProProtFlagBits::Write)
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
#elif defined(IS_WINDOWS)
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
MappedMemory memory_map_file(const char *fpath, std::byte *start, size_t offset,
                             size_t len, ProtFlagBits prot, MapFlagBits flags) {
#ifdef __unix__
  int fd;
  if ((fd = open(fpath, to_open_flag_int(prot) | O_EXCL)))
  // todo...
#elif defined(IS_WINDOWS)
  MappedMemory mem;
  SECURITY_ATTRIBUTES file_security_attributes{
      .nLength = 0u, .lpSecurityDescriptor = nullptr, .bInheritHandle = TRUE};
  mem.m_FileDescriptor = CreateFileA(
      fpath, to_file_access_dw(prot),
      (flags & MapFlagBits::Shared) ? (FILE_SHARE_READ | FILE_SHARE_WRITE)
                                    : FILE_SHARE_READ,
      &file_security_attributes, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (mem.m_FileDescriptor == INVALID_HANDLE_VALUE) {
    print_error(s_TypeName, GetLastError());
    goto FAILED_WIN_0;
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
                         to_protect_dw(flags, prot), DWORD(len >> 32),
                         DWORD(len & 0xffffffff), nullptr);
  if (mem.m_FileMappingObject == INVALID_HANDLE_VALUE) {
    print_error(s_TypeName, GetLastError());
    goto FAILED_WIN_1;
  }
  mem.m_Data = reinterpret_cast<std::byte *>(
      MapViewOfFileEx(mem.m_FileMappingObject, to_map_access_dw(flags, prot),
                      DWORD(offset >> 32), DWORD(offset & 0xffffffff),
                      (SIZE_T)len, (void *)start));
  if (mem.m_Data == nullptr) {
    print_error(s_TypeName, GetLastError());
    goto FAILED_WIN_2;
  }
  return mem;
FAILED_WIN_2:
  close_handle_win(mem.m_FileMappingObject);
FAILED_WIN_1:
  close_handle_win(mem.m_FileDescriptor);
FAILED_WIN_0:
  mem.m_Data = nullptr;
  mem.m_Length = 0u;
  mem.m_FileDescriptor = INVALID_HANDLE_VALUE;
  mem.m_FileMappingObject = INVALID_HANDLE_VALUE;
  return mem;
#endif
}
int memory_map_sync(std::byte *start, size_t len, SyncFlag flag) {
#ifdef __unix__
  // todo...
#elif defined(IS_WINDOWS)
  if (flag == SyncFlag::Sync)
    if (!FlushViewOfFile((void *)start, len)) {
      print_error(s_TypeName, GetLastError());
      return -1;
    }
  return 0;
#endif
}
void memory_unmap_file(MappedMemory &&mem) {
#ifdef __unix__
  // todo...
#elif defined(IS_WINDOWS)
  if (mem.m_Data == nullptr)
    return;
  if (!UnmapViewOfFile(mem.m_Data))
    print_error(s_TypeName, GetLastError());
  if (mem.m_FileDescriptor != INVALID_HANDLE_VALUE)
    close_handle_win(mem.m_FileDescriptor);
  if (mem.m_FileMappingObject != INVALID_HANDLE_VALUE)
    close_handle_win(mem.m_FileMappingObject);
#endif
}
size_t acquire_file_size(const char *path) {
#ifdef __unix__
  // todo...
#elif defined(IS_WINDOWS)
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
  close_handle_win(handle);
  return size.QuadPart;
#endif
}
#undef IS_WINDOWS
} // namespace BLT::sys
