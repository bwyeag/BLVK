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
#ifndef BL_OUTPUT_HPP_FILE
#define BL_OUTPUT_HPP_FILE
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <ostream>
#include <print>
#include <source_location>
#include <system_error>
#include <utils/bl_macro.hpp>

#ifdef BL_PLATFORM_WINDOWS
#include <Windows.h>
#endif
namespace blt {
void output_setup();
enum class ConsoleColor : uint8_t {
  green,
  red,
  blue,
  white,
  black,
  yellow,
  purple,
  gray,
  cyan,
  none,
  green_intensity,
  red_intensity,
  blue_intensity,
  white_intensity,
  black_intensity,
  yellow_intensity,
  purple_intensity,
  gray_intensity,
  cyan_intensity
};

enum class ConsoleBackgroundColor : uint8_t {
  green,
  red,
  blue,
  white,
  black,
  yellow,
  purple,
  gray,
  cyan,
  none
};

auto operator<<(std::ostream &os, ConsoleColor data) -> std::ostream &;
auto operator<<(std::ostream &os, ConsoleBackgroundColor data)
    -> std::ostream &;

namespace _output_impl {
/// @brief 打印文件位置
inline void print_source_loc(std::ostream &stm,
                             const std::source_location &loc) {
#ifdef __cpp_lib_print
#ifdef bl_lib_output_complex
  std::print(stm, "[{0}:{2}@{1}]", loc.file_name(), loc.function_name(),
             loc.line());
#else
  std::print(stm, "[{0}:{1}]", loc.file_name(), loc.line());
#endif // bl_lib_output_complex
#else
  stm << '[' << loc.file_name() << ':' << loc.line();
#ifdef bl_lib_output_complex
  stm << '@' << loc.function_name();
#endif // bl_lib_output_complex
  stm << ']';
#endif // __cpp_lib_print
}
/// @brief 打印时间点
inline void print_time(std::ostream &stm) {
  static constexpr int max_buf_length = 64;
  static constexpr int count_pre_sec_ms = 1000;
  auto now = std::chrono::system_clock::now();
  auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      now.time_since_epoch());
  std::time_t t = std::chrono::system_clock::to_time_t(now);
  char buf[max_buf_length]{};
#ifdef __cpp_lib_print
  if (!std::strftime(buf, max_buf_length, "%H:%M:%S", std::localtime(&t)))
    std::print(stm, "[{0}ms]", now_ms.count());
  else
    std::print(stm, "[{0}.{1}]", buf, now_ms.count() % count_pre_sec_ms);
#else
  if (!std::strftime(buf, max_buf_length, "%H:%M:%S", std::localtime(&t)))
    stm << '[' << now_ms.count() << "ms]";
  else
    stm << '[' << buf << '.' << (now_ms.count() % count_pre_sec_ms) << ']';
#endif // __cpp_lib_print
}
/// @brief 打印错误
template <typename... Types>
void print_error_internal(const std::source_location loc, const char *type,
                          const Types &...args) {
#ifndef bl_lib_no_output_color
  std::cout << ConsoleColor::red;
#endif // bl_lib_no_output_color
  print_time(std::cout);
  print_source_loc(std::cout, loc);
  std::cout << '[' << type << ']'
            << ((std::cout << ... << (std::cout << args, ' ')), '\n');
#ifndef bl_lib_no_output_color
  std::cout << ConsoleColor::none;
#endif // bl_lib_no_output_color
}
/// @brief 打印警告
template <typename... Types>
void print_warning_internal(const std::source_location loc, const char *type,
                            const Types &...args) {
#ifndef bl_lib_no_output_color
  std::cout << ConsoleColor::yellow;
#endif // bl_lib_no_output_color
  print_time(std::cout);
  print_source_loc(std::cout, loc);
  std::cout << '[' << type << ']'
            << ((std::cout << ... << (std::cout << args, ' ')), '\n');
#ifndef bl_lib_no_output_color
  std::cout << ConsoleColor::none;
#endif // bl_lib_no_output_color
}
/// @brief 打印输出
template <typename... Types>
void print_log_internal(const char *type, const Types &...args) {
#ifndef bl_lib_no_output_color
  std::cout << ConsoleColor::green;
#endif // bl_lib_no_output_color
  std::cout << '[' << type << ']'
            << ((std::cout << ... << (std::cout << args, ' ')), '\n');
#ifndef bl_lib_no_output_color
  std::cout << ConsoleColor::none;
#endif // bl_lib_no_output_color
}

template <typename T, typename Arg, typename... OtherArgs>
constexpr static bool is_all_same =
    std::is_same_v<T, Arg> &&
    (sizeof...(OtherArgs) > 0 ? is_all_same<T, OtherArgs...> : true);
template <typename... Types>
void print_errorcode_internal(const Types &...ecs) {
  static_assert(is_all_same<std::error_code, Types...>,
                "Wrong Argument Types!");
#ifndef bl_lib_no_output_color
  std::cout << ConsoleColor::red;
#endif // bl_lib_no_output_color
  print_time(std::cout);
  (std::cout << ...
             << (std::cout << '[' << ecs.category().name() << ']',
                 ecs.message()))
      << '\n';
#ifndef bl_lib_no_output_color
  std::cout << ConsoleColor::none;
#endif // bl_lib_no_output_color
}
} // namespace _output_impl
#define print_error(type, ...)                                                 \
  _output_impl::print_error_internal(std::source_location::current(), type,    \
                                     __VA_ARGS__)
#define print_warning(type, ...)                                               \
  _output_impl::print_error_internal(std::source_location::current(), type,    \
                                     __VA_ARGS__)
#define print_log(type, ...) _output_impl::print_log_internal(type, __VA_ARGS__)
#define print_errorcode(...) _output_impl::print_errorcode_internal(__VA_ARGS__)
} // namespace blt
#undef IS_WINDOWS
#endif //! BL_OUTPUT_HPP_FILE
