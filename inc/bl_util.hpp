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
#ifndef _BL_CORE_BL_UTIL_HPP_
#define _BL_CORE_BL_UTIL_HPP_
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||                 \
    defined(__NT__) && !defined(__CYGWIN__)
#define BL_PLATFORM_WINDOWS
#elif defined(__unix__)
#define BL_PLATFORM_UNIX
#endif
// 第三方库include
#include <vulkan/vulkan.h>
// 标准库include
#include <cstdint>
namespace blt {
#define BL_VERSION VK_MAKE_API_VERSION(0, 0, 1, 0)
#ifdef DEBUG
constexpr int8_t is_debuging = 1;
#else
constexpr int8_t is_debuging = 0;
#endif // DEBUG
} // namespace blt
// 本地include
#include <utils/bl_byte_magic.hpp>
#include <utils/bl_callback_type.hpp>
#include <utils/bl_output.hpp>
#include <utils/bl_result_type.hpp>
#include <utils/bl_timer_type.hpp>
#include <utils/bl_macro.hpp>
#endif //!_BL_CORE_BL_UTIL_HPP_
