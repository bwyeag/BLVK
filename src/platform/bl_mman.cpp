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
#include "bl_output.hpp"
#include <bl_util.hpp>
#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <memoryapi.h>
#include <platform/bl_mman.hpp>
#include <winnt.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||                 \
    defined(__NT__) && !defined(__CYGWIN__)
#define IS_WINDOWS
#endif
namespace BLT::sys {
const char *const s_ErrorTypeString = "System";
#ifdef __unix__
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
  if (f & MapFlagBits::Anonymous)
    r |= MAP_ANONYMOUS;
  return r;
}
#elif defined(IS_WINDOWS)
DWORD to_prot_dw(ProtFlagBits f) {
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
    print_error(s_ErrorTypeString, GetLastError());
}
#endif // __unix__
MappedMemory memory_mapping(size_t offset, size_t len, const char *fpath,
                            ProtFlagBits prot, MapFlagBits flags) {
#ifdef __unix__
  // todo...
#elif defined(IS_WINDOWS)
  MappedMemory mem;
  if (flags & MapFlagBits::Anonymous)
    mem.m_FileDescriptor = INVALID_HANDLE_VALUE;
  else {
    mem.m_FileDescriptor = CreateFileA(
        fpath, to_prot_dw(prot),
        (flags & MapFlagBits::Shared)
            ? (FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE)
            : 0,
        nullptr, OPEN_EXISTING,
        (flags & MapFlagBits::Private) ? FILE_ATTRIBUTE_READONLY
                                       : FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (mem.m_FileDescriptor == INVALID_HANDLE_VALUE) {
      print_error(s_ErrorTypeString, GetLastError());
      mem.m_Data = nullptr;
      return mem;
    }
  }
  mem.m_FileMappingObject = CreateFileMappingA(
      mem.m_FileDescriptor, nullptr, to_protect_dw(flags, prot),
      DWORD(len >> 32), DWORD(len & 0xffffffff), nullptr);
  if (mem.m_FileMappingObject == INVALID_HANDLE_VALUE) {
    if ((flags & MapFlagBits::Anonymous) && len == 0u)
      print_error(s_ErrorTypeString, "Wrong mapping length(=0).");
    else
      print_error(s_ErrorTypeString, GetLastError());
    close_handle_win(mem.m_FileDescriptor);
    mem.m_Data = nullptr;
    return mem;
  }
  mem.m_Data = reinterpret_cast<std::byte *>(MapViewOfFile(
      mem.m_FileMappingObject, to_map_access_dw(flags, prot),
      DWORD(offset >> 32), DWORD(offset & 0xffffffff), (SIZE_T)len));
  if (mem.m_Data == nullptr) {
    print_error(s_ErrorTypeString, GetLastError());
    close_handle_win(mem.m_FileMappingObject);
    close_handle_win(mem.m_FileDescriptor);
    return mem;
  }
  return mem;
#endif
}
void memory_unmapping(MappedMemory &&mem) {
#ifdef __unix__
  // todo...
#elif defined(IS_WINDOWS)
  if (mem.m_Data == nullptr)
    return;
  if (!UnmapViewOfFile(mem.m_Data))
    print_error(s_ErrorTypeString, GetLastError());
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
    print_error(s_ErrorTypeString, GetLastError());
    return (~0u);
  }
  LARGE_INTEGER size;
  if (!GetFileSizeEx(handle, &size)) {
    print_error(s_ErrorTypeString, GetLastError());
    return (~0u);
  }
  close_handle_win(handle);
  return size.QuadPart;
#endif
}
#undef IS_WINDOWS
} // namespace BLT::sys
