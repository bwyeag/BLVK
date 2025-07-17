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
#include <ctime>
#include <iostream>
#include <print>
#include <source_location>
#include <system_error>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||                 \
    defined(__NT__) && !defined(__CYGWIN__)
#define IS_WINDOWS
#include <Windows.h>
#endif
namespace BLT {
enum class ConsoleColor {
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

enum class ConsoleBackgroundColor {
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

std::ostream &operator<<(std::ostream &os, ConsoleColor data);
std::ostream &operator<<(std::ostream &os, ConsoleBackgroundColor data);

namespace _internal {
/// @brief 打印文件位置
inline void print_source_loc(std::ostream &stm,
                             const std::source_location &loc) {
  std::print(stm, "[{0}:{2}@{1}]", loc.file_name(), loc.function_name(),
             loc.line());
}
/// @brief 打印时间点
inline void print_time(std::ostream &stm) {
  auto now = std::chrono::system_clock::now();
  auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      now.time_since_epoch());
  std::time_t t = std::chrono::system_clock::to_time_t(now);
  char buf[64];
  if (!std::strftime(buf, 64, "%H:%M:%S.", std::localtime(&t)))
    std::print(stm, "[{0}ms]", now_ms.count());
  else
    std::print(stm, "[{0}.{1}]", buf, now_ms.count() % 1000);
}
/// @brief 打印错误
template <typename... Types>
void print_error_internal(const std::source_location loc, const char *type,
                          const Types &...args) {
  std::cout << ConsoleColor::red;
  print_time(std::cout);
  print_source_loc(std::cout, loc);
  std::cout << '[' << type << ']'
            << ((std::cout << ... << (std::cout << args, ' ')), '\n')
            << ConsoleColor::none;
}
/// @brief 打印警告
template <typename... Types>
void print_warning_internal(const std::source_location loc, const char *type,
                            const Types &...args) {
  std::cout << ConsoleColor::yellow;
  print_time(std::cout);
  print_source_loc(std::cout, loc);
  std::cout << '[' << type << ']'
            << ((std::cout << ... << (std::cout << args, ' ')), '\n')
            << ConsoleColor::none;
}
/// @brief 打印输出
template <typename... Types>
void print_log_internal(const char *type, const Types &...args) {
  std::cout << ConsoleColor::green;
  std::cout << '[' << type << ']'
            << ((std::cout << ... << (std::cout << args, ' ')), '\n')
            << ConsoleColor::none;
}

template <typename T, typename Arg, typename... OtherArgs>
constexpr static bool is_all_same =
    std::is_same_v<T, Arg> &&
    (sizeof...(OtherArgs) > 0 ? is_all_same<T, OtherArgs...> : true);
template <typename... Types>
void print_errorcode_internal(const Types &...ecs) {
  static_assert(is_all_same<std::error_code, Types...>,
                "Wrong Argument Types!");
  std::cout << ConsoleColor::red;
  print_time(std::cout);
  (std::cout << ...
             << (std::cout << '[' << ecs.category().name() << ']',
                 ecs.message()))
      << '\n'
      << ConsoleColor::none;
}
} // namespace _internal
#define print_error(type, ...)                                                 \
  _internal::print_error_internal(std::source_location::current(), type,       \
                                  __VA_ARGS__)
#define print_warning(type, ...)                                               \
  _internal::print_error_internal(std::source_location::current(), type,       \
                                  __VA_ARGS__)
#define print_log(type, ...) _internal::print_log_internal(type, __VA_ARGS__)
#define print_errorcode(...) _internal::print_errorcode_internal(__VA_ARGS__)
} // namespace BLT
#undef IS_WINDOWS
#endif //! BL_OUTPUT_HPP_FILE
