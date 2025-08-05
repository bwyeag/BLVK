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
#ifndef BL_UTIL_RESULT_TYPE_FILE
// 标准库include
#include <cstdint>
namespace blt {
//*****************************************************************************
// 字节序反转函数
//*****************************************************************************
constexpr uint16_t byte_reverse(uint16_t x) {
  x = (x & 0x00fful) << 8 | (x & 0xff00ul) >> 8;
  return x;
}
constexpr uint32_t byte_reverse(uint32_t x) {
  x = (x & 0x0000fffful) << 16 | (x & 0xffff0000ul) >> 16;
  x = (x & 0x00ff00fful) << 8 | (x & 0xff00ff00ul) >> 8;
  return x;
}
constexpr uint64_t byte_reverse(uint64_t x) {
  x = (x & 0x00000000ffffffffull) << 32 | (x & 0xffffffff00000000ull) >> 32;
  x = (x & 0x0000ffff0000ffffull) << 16 | (x & 0xffff0000ffff0000ull) >> 16;
  x = (x & 0x00ff00ff00ff00ffull) << 8 | (x & 0xff00ff00ff00ff00ull) >> 8;
  return x;
}
//*****************************************************************************
// CRC 循环冗余校验
//*****************************************************************************
} // namespace BLT
#endif // !BL_UTIL_RESULT_TYPE_FILE
