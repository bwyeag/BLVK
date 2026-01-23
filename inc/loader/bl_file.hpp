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
#include <platform/bl_mman.hpp>
// 标准库include
#include <bit>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
namespace blt {
/*
struct File {
  uint32_t header;          // file head magic number
  uint32_t session_count;   // == N serving as its name
  uint32_t head_length;     // the length of 'file head' part(in byte)
  ...file head...

  ...file session 0...
  ...file session 1...
  ...file session 2...
  ......
  ...file session N...
};
struct file_session {
  uint32_t session_length;  // the length of 'session data' part(in byte)
  ...session data...
};
 */
#define READ_UINT32(p) *reinterpret_cast<uint32_t *>(p)
enum class ProfileStatBits : uint32_t { None = 0x0, BitReverse = 0x1 };
constexpr ProfileStatBits operator|(ProfileStatBits a, ProfileStatBits b) {
  return static_cast<ProfileStatBits>(a | b);
}
constexpr ProfileStatBits operator&(ProfileStatBits a, ProfileStatBits b) {
  return static_cast<ProfileStatBits>(a & b);
}
template <typename T>
concept FileParser = requires(std::byte *p, uint32_t len) {
  (T::headcode)->uint32_t;
  T::parse_filehead_impl(p, len)->result_t;
};
template <typename T> struct Profile {
  sys::MappedMemory m_Memory;
  ProfileStatBits m_Stat;
  std::byte *m_Pointer;
  uint32_t m_SessionCount;
  result_t open(const char *path) {
#ifdef DEBUG
    if (m_Memory.m_Data)
      throw std::logic_error("re-open file error");
#endif
    m_Memory = sys::memory_map_file(
        path, nullptr, 0, 0, sys::ProtFlagBits::Read, sys::MapFlagBits::Shared);
    if (!m_Memory.m_Data)
      return make_result(LoadResult::FileNotFound);
    constexpr uint32_t head = T::headcode;
    m_Pointer = m_Memory.m_Data;
    if (READ_UINT32(m_Pointer) == byte_reverse(head)) {
      m_Stat = m_Stat | ProfileStatBits::BitReverse;
    } else if (READ_UINT32(m_Pointer) == head) {
      m_Stat = m_Stat | ProfileStatBits::BitReverse;
    } else
      return make_result(LoadResult::FileHeadError);
    m_Pointer += sizeof(uint32_t);
    m_SessionCount = read_from<uint32_t>(m_Pointer);
    m_Pointer += sizeof(uint32_t);
  }
  INLINE void close() {
#ifdef DEBUG
    if (!m_Memory.m_Data)
      throw std::logic_error("no open() before close()");
#endif
    sys::memory_unmap_file(std::move(m_Memory));
    m_Memory.m_Data = nullptr;
  }
  template <typename U> U read_from(std::byte *&p) {
    constexpr size_t L = sizeof(U);
    static_assert(L == 1 || L == 2 || L == 4 || L == 8,
                  "sizeof(T) must be power of 2.");
    if constexpr (L == 1)
      return std::bit_cast<U>(*reinterpret_cast<U *>(p));
    else {
      if ((m_Stat & ProfileStatBits::BitReverse) == ProfileStatBits::None)
        return std::bit_cast<U>(*reinterpret_cast<U *>(p));
      else {
        if constexpr (L == 2)
          return std::bit_cast<U>(
              byte_reverse(*reinterpret_cast<uint16_t *>(p)));
        else if constexpr (L == 4)
          return std::bit_cast<U>(
              byte_reverse(*reinterpret_cast<uint32_t *>(p)));
        else if constexpr (L == 8)
          return std::bit_cast<U>(
              byte_reverse(*reinterpret_cast<uint64_t *>(p)));
      }
    }
  }
  result_t parse() {
    if (result_t result = parse_filehead())
      return (result.install().fill(), result);
    for (; m_SessionCount > 0u; --m_SessionCount)
      if (result_t result = parse_filesession())
        return (result.install().fill(), result);
    return make_result(LoadResult::Success);
  }

private:
  result_t parse_filehead() {
    uint32_t head_length = read_from<uint32_t>(m_Pointer);
    result_t r =
        T::parse_filehead_impl((m_Pointer += sizeof(uint32_t)), head_length);
    return (m_Pointer += head_length, r);
  }
  result_t parse_filesession() {
    uint32_t session_length = read_from<uint32_t>(m_Pointer);
    result_t r = T::parse_filesession_impl((m_Pointer += sizeof(uint32_t)),
                                           session_length);
    return (m_Pointer += session_length, r);
  }
};
} // namespace blt
#endif // !_BL_LOADER_FILE_HPP_FILE_
