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
#undef BL_PLATFORM_WINDOWS
#undef BL_PLATFORM_UNIX
#undef INLINE
#undef INLINE_CALL
#undef RESTRICT

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||                 \
    defined(__NT__) && !defined(__CYGWIN__)
#define BL_PLATFORM_WINDOWS
#elif defined(__unix__)
#define BL_PLATFORM_UNIX
#endif
//*****************************************************************************
// 宏定义
//*****************************************************************************
#ifdef __GNUC__
//***************************gnuc**********************************************
#define INLINE [[gnu::always_inline]] inline
#define INLINE_CALL
#define RESTRICT __restrict
#elif defined(__clang__)
//***************************clang*********************************************
#define INLINE [[clang::always_inline]] inline
#define INLINE_CALL
#define RESTRICT __restrict
#elif defined(_MSC_VER)
//***************************msvc**********************************************
#define INLINE [[msvc::forceinline]] inline
#define INLINE_CALL [[msvc::forceinline_calls]]
#define RESTRICT __restrict
#else
//***************************other*********************************************
#define INLINE
#define INLINE_CALL
#define RESTRICT
#endif
