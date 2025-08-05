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
#ifndef _BL_LOADER_FILE_HPP_FILE_
#define _BL_LOADER_FILE_HPP_FILE_
#include <bl_util.hpp>
#include <cstdint>
#include <platform/bl_mman.hpp>
namespace blt {
/*
struct File {
  uint32_t header;
  uint32_t head_length;
  ...
  uint32_t crc32;
};
 */
#define READ_UINT32(p) *reinterpret_cast<uint32_t *>(p)
enum class ProfileStatBits : uint32_t { BitReverse = 0x1 };
constexpr ProfileStatBits operator|(ProfileStatBits a, ProfileStatBits b) {
  return static_cast<ProfileStatBits>(a | b);
}
constexpr ProfileStatBits operator&(ProfileStatBits a, ProfileStatBits b) {
  return static_cast<ProfileStatBits>(a & b);
}
template <typename T> struct Profile {
  sys::MappedMemory m_mem;
  ProfileStatBits m_stat;
  int open(const char *path) {
    m_mem = sys::memory_map_file(path, nullptr, 0, 0, sys::ProtFlagBits::Read,
                                 sys::MapFlagBits::Shared);
    if (!m_mem.m_Data)
      return -2;
    constexpr uint32_t head = T::get_headcode();
    auto *p = m_mem.m_Data;
    if (READ_UINT32(p) == byte_reverse(head)) {
      m_stat = m_stat | ProfileStatBits::BitReverse;
    } else if (READ_UINT32(p) == head) {
    } else
      return -1;
  }
};
} // namespace blt
#endif // !_BL_LOADER_FILE_HPP_FILE_
